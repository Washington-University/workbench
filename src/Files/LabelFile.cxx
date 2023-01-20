/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GroupAndNameHierarchyModel.h"
#include "DataFileException.h"
#include "DataFileTypeEnum.h"
#include "GiftiFile.h"
#include "GiftiLabel.h"
#include "MathFunctions.h"
#include "LabelFile.h"

using namespace caret;

/**
 * Constructor.
 */
LabelFile::LabelFile()
: GiftiTypeFile(DataFileTypeEnum::LABEL)
{
    m_classNameHierarchy = NULL;
    this->initializeMembersLabelFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
LabelFile::LabelFile(const LabelFile& sf)
: GiftiTypeFile(sf)
{
    m_classNameHierarchy = NULL;
    this->copyHelperLabelFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the LabelFile parameter.
 */
LabelFile& 
LabelFile::operator=(const LabelFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperLabelFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
LabelFile::~LabelFile()
{
    this->columnDataPointers.clear();
    delete m_classNameHierarchy;
}

void LabelFile::writeFile(const AString& filename)
{
    if (!filename.endsWith(".label.gii"))
    {
        CaretLogWarning("label file '" + filename + "' should be saved ending in .label.gii, see wb_command -gifti-help");
    }
    caret::GiftiTypeFile::writeFile(filename);
}

/**
 * Clear the surface file.
 */
void 
LabelFile::clear()
{
    GiftiTypeFile::clear();
    this->columnDataPointers.clear();
    m_classNameHierarchy->clear();
}

/** 
 * @return Return the GIFTI Label Table. 
 */
GiftiLabelTable* 
LabelFile::getLabelTable() 
{ 
    return this->giftiFile->getLabelTable(); 
}

/** 
 * @return Return the GIFTI Label Table. 
 */
const GiftiLabelTable* 
LabelFile::getLabelTable() const 
{ 
    return this->giftiFile->getLabelTable(); 
}

/**
 * @return The class and name hierarchy.
 */
GroupAndNameHierarchyModel*
LabelFile::getGroupAndNameHierarchyModel()
{
    m_classNameHierarchy->update(this,
                                 m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * @return The class and name hierarchy.
 */
const GroupAndNameHierarchyModel*
LabelFile::getGroupAndNameHierarchyModel() const
{
    m_classNameHierarchy->update(const_cast<LabelFile*>(this),
                                 m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
LabelFile::validateDataArraysAfterReading()
{
    this->columnDataPointers.clear();

    this->initializeMembersLabelFile();
    
    this->verifyDataArraysHaveSameNumberOfRows(0, 0);
    
    bool haveWarned = false;
    
    const int32_t numberOfDataArrays = this->giftiFile->getNumberOfDataArrays();
    for (int32_t i = 0; i < numberOfDataArrays; i++) {
        GiftiDataArray* thisArray = this->giftiFile->getDataArray(i);
        if (thisArray->getDataType() != NiftiDataTypeEnum::NIFTI_TYPE_INT32)
        {
            thisArray->convertToDataType(NiftiDataTypeEnum::NIFTI_TYPE_INT32);
            if (!haveWarned)
            {
                CaretLogWarning("label file '" + getFileName() + "' contains data array with data type other than int32");
                haveWarned = true;
            }
        }
        int32_t* tempPointer = thisArray->getDataPointerInt();
        CaretAssert(tempPointer != NULL);
        if (tempPointer == NULL) throw DataFileException(getFileName(),
                                                         "failed to convert data array to int32.");//shouldn't happen, can probably be removed
        this->columnDataPointers.push_back(tempPointer);
    }
    
    validateKeysAndLabels();
    
    m_classNameHierarchy->update(this,
                                 true);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    m_classNameHierarchy->setAllSelected(true);
    
    CaretLogFiner("CLASS/NAME Table for : "
                  + this->getFileNameNoPath()
                  + "\n"
                  + m_classNameHierarchy->toString());
}

/**
 * Validate keys and labels in the file.
 */
void
LabelFile::validateKeysAndLabels() const
{
    /*
     * Skip if logging is not fine or less.
     */
    if (CaretLogger::getLogger()->isFine() == false) {
        return;
    }
    
    AString message;
    
    /*
     * Find the label keys that are in the data
     */
    std::set<int32_t> dataKeys;
    const int32_t numNodes = getNumberOfNodes();
    const int32_t numMaps  = getNumberOfMaps();
    for (int32_t iNode = 0; iNode < numNodes; iNode++) {
        for (int32_t jMap = 0; jMap < numMaps; jMap++) {
            const int32_t key = getLabelKey(iNode, jMap);
            dataKeys.insert(key);
        }
    }
    
    /*
     * Find any keys that are not in the label table
     */
    const GiftiLabelTable* labelTable = getLabelTable();
    std::set<int32_t> missingLabelKeys;
    for (std::set<int32_t>::iterator dataKeyIter = dataKeys.begin();
         dataKeyIter != dataKeys.end();
         dataKeyIter++) {
        const int32_t dataKey = *dataKeyIter;
        
        const GiftiLabel* label = labelTable->getLabel(dataKey);
        if (label == NULL) {
            missingLabelKeys.insert(dataKey);
        }
    }
    
    if (missingLabelKeys.empty() == false) {
        for (std::set<int32_t>::iterator missingKeyIter = missingLabelKeys.begin();
             missingKeyIter != missingLabelKeys.end();
             missingKeyIter++) {
            const int32_t missingKey = *missingKeyIter;
            
            message.appendWithNewLine("    Missing Label for Key: "
                                      + AString::number(missingKey));
        }
    }
    
    /*
     * Find any label table names that are not used
     */
    std::map<int32_t, AString> labelTableKeysAndNames;
    labelTable->getKeysAndNames(labelTableKeysAndNames);
    for (std::map<int32_t, AString>::const_iterator ltIter = labelTableKeysAndNames.begin();
         ltIter != labelTableKeysAndNames.end();
         ltIter++) {
        const int32_t ltKey = ltIter->first;
        if (std::find(dataKeys.begin(),
                      dataKeys.end(),
                      ltKey) == dataKeys.end()) {
            message.appendWithNewLine("    Label Not Used Key="
                                      + AString::number(ltKey)
                                      + ": "
                                      + ltIter->second);
        }
    }
    
    AString msg = ("File: "
                   + getFileName()
                   + "\n"
                   + labelTable->toFormattedString("    ")
                   + message);
    CaretLogFine(msg);
}


/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
LabelFile::getNumberOfNodes() const
{
    int32_t numNodes = 0;
    int32_t numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays > 0) {
        numNodes = this->giftiFile->getDataArray(0)->getNumberOfRows();
    }
    return numNodes;
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
LabelFile::getNumberOfColumns() const
{
    const int32_t numCols = this->giftiFile->getNumberOfDataArrays();
    return numCols;
}


/**
 * Initialize members of this class.
 */
void 
LabelFile::initializeMembersLabelFile()
{
    if (m_classNameHierarchy != NULL) {
        delete m_classNameHierarchy;
    }
    m_classNameHierarchy = new GroupAndNameHierarchyModel(this);
    m_forceUpdateOfGroupAndNameHierarchy = true;
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
LabelFile::copyHelperLabelFile(const LabelFile& /*sf*/)
{
    if (m_classNameHierarchy != NULL) {
        delete m_classNameHierarchy;
    }
    m_classNameHierarchy = new GroupAndNameHierarchyModel(this);
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    this->validateDataArraysAfterReading();
}

/**
 * Get label name for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * @return
 *     Label name at the given node and column indices
 *     Empty string if label is not available.
 */
AString 
LabelFile::getLabelName(const int32_t nodeIndex,
                        const int32_t columnIndex) const
{
    const int32_t labelKey = this->getLabelKey(nodeIndex, columnIndex);
    AString label = this->giftiFile->getLabelTable()->getLabelName(labelKey);
    return label;
}

/**
 * Get label key for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * @return
 *     Label key at the given node and column indices.
 */
int32_t 
LabelFile::getLabelKey(const int32_t nodeIndex,
                       const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), 
                       "Node Index out of range.");
    
    return this->columnDataPointers[columnIndex][nodeIndex];
}

/**
 * set label key for a node.
 * 
 * @param nodeIndex
 *     Node index.
 * @param columnIndex
 *     Column index.
 * param labelKey
 *     Label key inserted at the given node and column indices.
 */
void 
LabelFile::setLabelKey(const int32_t nodeIndex,
                       const int32_t columnIndex,
                       const int32_t labelKey)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    CaretAssertMessage((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()), "Node Index out of range.");
    
    this->columnDataPointers[columnIndex][nodeIndex] = labelKey;
    this->setModified();
    m_forceUpdateOfGroupAndNameHierarchy = true;
}

/**
 * Get nodes in the given column with the given lable key.
 * @param columnIndex
 *    Index of column
 * @param labelKey
 *    Key of label that is desired.
 * @param nodeIndicesOut
 *    On exit, will contain indices of nodes that have the 
 *    given label key in the given column.
 */
void 
LabelFile::getNodeIndicesWithLabelKey(const int32_t columnIndex,
                                const int32_t labelKey,
                                std::vector<int32_t>& nodeIndicesOut) const
{
    const int32_t numberOfNodes = this->getNumberOfNodes();
    nodeIndicesOut.clear();
    nodeIndicesOut.reserve(numberOfNodes);
    
    for (int32_t i = 0; i < numberOfNodes; i++) {
        if (this->getLabelKey(i, columnIndex) == labelKey) {
            nodeIndicesOut.push_back(i);
        }
    }
}

/**
 * Get a pointer to the keys for a label file column.
 * @param columnIndex
 *     Index of the column.
 * @return 
 *     Pointer to keys for the given column.
 */
const int32_t* 
LabelFile::getLabelKeyPointerForColumn(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    return this->columnDataPointers[columnIndex];    
}

void LabelFile::setNumberOfNodesAndColumns(int32_t nodes, int32_t columns)
{
    giftiFile->clearAndKeepMetadata();
    columnDataPointers.clear();

    const int32_t unassignedKey = this->getLabelTable()->getUnassignedLabelKey();
    
    std::vector<int64_t> dimensions;
    dimensions.push_back(nodes);
    for (int32_t i = 0; i < columns; ++i)
    {
        giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_LABEL, NiftiDataTypeEnum::NIFTI_TYPE_INT32, dimensions, GiftiEncodingEnum::GZIP_BASE64_BINARY));
        columnDataPointers.push_back(giftiFile->getDataArray(i)->getDataPointerInt());
        int32_t* ptr = giftiFile->getDataArray(i)->getDataPointerInt();
        for (int32_t j = 0; j < nodes; j++) {
            ptr[j] = unassignedKey;
        }
    }
    setModified();
    m_forceUpdateOfGroupAndNameHierarchy = true;
}

/**
 * Add map(s) to this GIFTI file.
 * @param numberOfNodes
 *     Number of nodes.  If file is not empty, this value must
 *     match the number of nodes that are in the file.
 * @param numberOfMaps
 *     Number of maps to add.
 */
void 
LabelFile::addMaps(const int32_t numberOfNodes,
                       const int32_t numberOfMaps)
{
    if (numberOfNodes <= 0) {
        throw DataFileException(getFileName(),
                                "When adding maps the number of nodes must be greater than zero");
    }
    
    if (this->getNumberOfNodes() > 0) {
        if (numberOfNodes != this->getNumberOfNodes()) {
            throw DataFileException(getFileName(),
                                    "When adding maps the requested number of nodes is "
                                    + AString::number(numberOfNodes)
                                    + " but the file contains "
                                    + AString::number(this->getNumberOfNodes())
                                    + " nodes.");
        }
    }
    
    if (numberOfMaps <= 0) {
        throw DataFileException(getFileName(),
                                "When adding maps, the number of maps must be greater than zero.");
    }
    
    const int32_t unassignedKey = this->getLabelTable()->getUnassignedLabelKey();
    
    if ((this->getNumberOfNodes() > 0) 
        && (this->getNumberOfMaps() > 0)) {
        std::vector<int64_t> dimensions;
        dimensions.push_back(numberOfNodes);
        
        for (int32_t i = 0; i < numberOfMaps; ++i)
        {
            this->giftiFile->addDataArray(new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_LABEL, 
                                                             NiftiDataTypeEnum::NIFTI_TYPE_INT32, 
                                                             dimensions, 
                                                             GiftiEncodingEnum::GZIP_BASE64_BINARY));
            const int32_t mapIndex = giftiFile->getNumberOfDataArrays() - 1;
            this->columnDataPointers.push_back(giftiFile->getDataArray(mapIndex)->getDataPointerInt());
            
            int32_t* ptr = giftiFile->getDataArray(mapIndex)->getDataPointerInt();
            for (int32_t j = 0; j < numberOfNodes; j++) {
                ptr[j] = unassignedKey;
            }
        }
    }
    else {
        this->setNumberOfNodesAndColumns(numberOfNodes, 
                                         numberOfMaps);
    }
    
    m_forceUpdateOfGroupAndNameHierarchy = true;
    this->setModified();
}

void LabelFile::setLabelKeysForColumn(const int32_t columnIndex, const int32_t* valuesIn)
{
    CaretAssertVectorIndex(this->columnDataPointers, columnIndex);
    int32_t* myColumn = columnDataPointers[columnIndex];
    int numNodes = (int)getNumberOfNodes();
    for (int i = 0; i < numNodes; ++i)
    {
        myColumn[i] = valuesIn[i];
    }
    m_forceUpdateOfGroupAndNameHierarchy = true;
    setModified();
}

/**
 * Return a vector containing the keys used in a map.  Each key is listed
 * once and the keys will be in ascending order. 
 * @param mapIndex
 *   Index of map.
 * @return
 *   Vector containing the keys.
 */
std::vector<int32_t>
LabelFile::getUniqueLabelKeysUsedInMap(const int32_t mapIndex) const
{
    CaretAssertVectorIndex(this->columnDataPointers, mapIndex);
    
    std::set<int32_t> uniqueKeys;
    const int32_t numNodes = getNumberOfNodes();
    for (int32_t i = 0; i < numNodes; i++) {
        const int32_t key = getLabelKey(i, mapIndex);
        uniqueKeys.insert(key);
    }
    
    std::vector<int32_t> keyVector;
    keyVector.insert(keyVector.end(),
                     uniqueKeys.begin(),
                     uniqueKeys.end());
    return keyVector;
}

/**
 * Called when a group and name hierarchy item has attribute/status changed
 */
void
LabelFile::groupAndNameHierarchyItemStatusChanged()
{
    
}
