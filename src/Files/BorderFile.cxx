
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

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QXmlStreamWriter>

#define __BORDER_FILE_DECLARE__
#include "BorderFile.h"
#undef __BORDER_FILE_DECLARE__

#include "Border.h"
#include "BorderPointFromSearch.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "DataFileEditorItem.h"
#include "DataFileEditorModel.h"
#include "DataFileException.h"
#include "GroupAndNameHierarchyModel.h"
#include "FileAdapter.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "TextFile.h"
#include "XmlAttributes.h"
#include "XmlSaxParser.h"
#include "XmlWriter.h"

using namespace caret;
using namespace std;

    
/**
 * \class caret::BorderFile 
 * \brief File containing borders.
 */

/**
 * Constructor.
 */
BorderFile::BorderFile()
: CaretDataFile(DataFileTypeEnum::BORDER),
GroupAndNameHierarchyUserInterface()
{
    initializeBorderFile();
}

/**
 * Destructor.
 */
BorderFile::~BorderFile()
{
    delete m_classColorTable;
    delete m_nameColorTable;
    delete m_metadata;
    
    for (std::vector<Border*>::iterator iter = m_borders.begin();
         iter != m_borders.end();
         iter++) {
        delete *iter;
    }
    m_borders.clear();

    delete m_classNameHierarchy;
}

/**
 * Initialize members of a border file.
 */
void 
BorderFile::initializeBorderFile()
{
    m_classColorTable = new GiftiLabelTable();
    m_nameColorTable = new GiftiLabelTable();
    m_classNameHierarchy = new GroupAndNameHierarchyModel(this);
    m_metadata = new GiftiMetaData();
    m_forceUpdateOfGroupAndNameHierarchy = true;
    m_structure = StructureEnum::ALL;
    m_numNodes = -1;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BorderFile::BorderFile(const BorderFile& obj)
: CaretDataFile(obj),
GroupAndNameHierarchyUserInterface(obj)
{
    initializeBorderFile();

    copyHelperBorderFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
BorderFile&
BorderFile::operator=(const BorderFile& obj)
{
    if (this != &obj) {
        clear();
        CaretDataFile::operator=(obj);
        copyHelperBorderFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
BorderFile::copyHelperBorderFile(const BorderFile& obj)
{
    *m_classColorTable = *obj.m_classColorTable;
    *m_nameColorTable = *obj.m_nameColorTable;
    if (m_classNameHierarchy != NULL) {
        delete m_classNameHierarchy;
    }
    m_classNameHierarchy = new GroupAndNameHierarchyModel(this);
    *m_metadata = *obj.m_metadata;
    
    const int32_t numBorders = obj.getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        m_borders.push_back(new Border(*obj.getBorder(i)));
    }
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    setModified();
}

/**
 * @return Is this border file empty (contains zero borders)?
 */
bool 
BorderFile::isEmpty() const
{
    return m_borders.empty();
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
BorderFile::getStructure() const
{
    return m_structure;
}

/**
 * @return Structures of all borders.  In most cases, this method
 * is equivalent to getStructure().  However, there are some older,
 * obsolete border files that contain borders for multiple structures
 * in which case this returns the structures for those borders such as
 * CORTEX_LEFT and CORTEX_RIGHT.
 */
std::vector<StructureEnum::Enum>
BorderFile::getAllBorderStructures() const
{
    std::set<StructureEnum::Enum> uniqueStructures;
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        uniqueStructures.insert(getBorder(i)->getStructure());
    }
    
    std::vector<StructureEnum::Enum> structures(uniqueStructures.begin(),
                                                uniqueStructures.end());
    return structures;
}

/**
 * Create new border files each of which contains a border for a single structure.
 * If there is not a border file name for for a structure, no file will be
 * produced for that file and it is not considered an error.
 *
 * @param singleStructureFileNames
 *     Each 'pair' is the filename for a structure.
 * @param structureNumberOfNodes
 *     Each pair is the number of nodes for a structure.  This parameter is 
 *     optional and when available, is used to validate the barycentric
 *     node indices in a border.
 * @param singleStructureBorderFilesOut
 *     On output contains border files for each of the structures.
 * @param errorMessageOut
 *     If unsuccessful will contain description of error(s).
 * @return
 *     True if successful and all files were created, else false.
 */
bool
BorderFile::splitIntoSingleStructureFiles(const std::map<StructureEnum::Enum, AString>& singleStructureFileNames,
                                          const std::map<StructureEnum::Enum, int32_t>& structureNumberOfNodes,
                                          std::vector<BorderFile*>& singleStructureBorderFilesOut,
                                          AString& errorMessageOut) const
{
    singleStructureBorderFilesOut.clear();
    errorMessageOut.clear();
    
    /*
     * Create single structure border files.
     */
    std::map<StructureEnum::Enum, BorderFile*> structureBorderFiles;
    for (std::map<StructureEnum::Enum, AString>::const_iterator nameIter = singleStructureFileNames.begin();
         nameIter != singleStructureFileNames.end();
         nameIter++) {
        const StructureEnum::Enum structure = nameIter->first;
        if (StructureEnum::isSingleStructure(structure)) {
            /*
             * Create the border file and set the structure
             */
            BorderFile* borderFile = new BorderFile();
            borderFile->setFileName(nameIter->second);
            borderFile->setStructure(structure);
            
            /*
             * Set number of nodes for border file (if available)
             */
            const std::map<StructureEnum::Enum, int32_t>::const_iterator structNumNodesIter =
                structureNumberOfNodes.find(structure);
            if (structNumNodesIter != structureNumberOfNodes.end()) {
                const int32_t numNodes = structNumNodesIter->second;
                borderFile->setNumberOfNodes(numNodes);
            }
            
            /*
             * Add to border file for each structure.
             */
            structureBorderFiles.insert(std::make_pair(structure,
                                                       borderFile));
        }
        else {
            errorMessageOut.appendWithNewLine("Structure "
                                              + StructureEnum::toGuiName(structure)
                                              + " is not a 'single' type structure.");
        }
    }
    
    /*
     * Copy borders to the appropriate single-structure files.
     */
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        const Border* border = getBorder(i);
        CaretAssert(border);
        const StructureEnum::Enum structure = border->getStructure();

        /*
         * Find output file with same structure as this border
         */
        std::map<StructureEnum::Enum, BorderFile*>::iterator fileIter = structureBorderFiles.find(structure);
        if (fileIter != structureBorderFiles.end()) {
            if (border->verifyAllPointsOnSameStructure()) {
                /*
                 * Surface number of nodes may be available
                 */
                int32_t surfaceNumberOfNodes = 0;
                const std::map<StructureEnum::Enum, int32_t>::const_iterator structNumNodesIter =
                structureNumberOfNodes.find(structure);
                if (structNumNodesIter != structureNumberOfNodes.end()) {
                    surfaceNumberOfNodes = structNumNodesIter->second;
                }
                
                /*
                 * If possible verify border node indices valid for structure
                 */
                bool copyBorderFlag = false;
                if (surfaceNumberOfNodes > 0) {
                    if (border->verifyForSurfaceNumberOfNodes(surfaceNumberOfNodes)) {
                        copyBorderFlag = true;
                    }
                    else {
                        errorMessageOut.appendWithNewLine("Border index="
                                                          + AString::number(i)
                                                          + " name="
                                                          + border->getName()
                                                          + " contains an incompatible number of nodes for structure "
                                                          + StructureEnum::toGuiName(structure));
                    }
                }
                else {
                    /*
                     * Unable to verify number of nodes so assume okay
                     */
                    copyBorderFlag = true;
                }
                
                if (copyBorderFlag) {
                    Border* borderCopy = new Border(*border);
                    fileIter->second->addBorder(borderCopy);
                    
                    const GiftiLabel* nameLabel = m_nameColorTable->getLabelBestMatching(border->getName());
                    fileIter->second->getNameColorTable()->addLabel(nameLabel);
                    const GiftiLabel* classLabel = m_classColorTable->getLabelBestMatching(border->getClassName());
                    fileIter->second->getClassColorTable()->addLabel(classLabel);
                }
            }
            else {
                errorMessageOut.appendWithNewLine("Border index="
                                                  + AString::number(i)
                                                  + " name="
                                                  + border->getName()
                                                  + " is an invalid border that contains points on multiple structures");
            }
        }
    }
    
    if (errorMessageOut.isEmpty()) {
        /*
         * Success: return the single structure border files that were created.
         */
        for (std::map<StructureEnum::Enum, BorderFile*>::iterator fileIter = structureBorderFiles.begin();
             fileIter != structureBorderFiles.end();
             fileIter++) {
            singleStructureBorderFilesOut.push_back(fileIter->second);
        }
        return true;
    }

    /*
     * Had an error so delete any single structure border files that were created.
     */
    for (std::map<StructureEnum::Enum, BorderFile*>::iterator fileIter = structureBorderFiles.begin();
         fileIter != structureBorderFiles.end();
         fileIter++) {
        delete fileIter->second;
    }
    
    return false;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
BorderFile::setStructure(const StructureEnum::Enum structure)
{
    if (m_structure == StructureEnum::ALL && m_borders.size() != 0)
    {//not really sure what this should do, so throw an error for now
        throw DataFileException(getFileName(), "attempt to set structure on multi-structure border file");
    }
    int numBorders = (int)m_borders.size();
    for (int i = 0; i < numBorders; ++i)
    {
        Border* thisBorder = m_borders[i];
        int numPoints = thisBorder->getNumberOfPoints();
        for (int j = 0; j < numPoints; ++j)
        {
            thisBorder->getPoint(j)->setStructure(structure);
        }
    }
    m_structure = structure;
    setModified();
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
BorderFile::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
BorderFile::getFileMetaData() const
{
    return m_metadata;
}

/**
 * Clear the border file.
 */
void 
BorderFile::clear()
{
    CaretDataFile::clear();
    m_metadata->clear();
    clearBorders();
}

/**
 * Clear the borders
 */
void
BorderFile::clearBorders()
{
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        delete m_borders[i];
    }
    m_classNameHierarchy->clear();
    m_classColorTable->clear();
    m_nameColorTable->clear();
    m_borders.clear();
    m_structure = StructureEnum::ALL;
    m_numNodes = -1;
    m_borderMDKeys.clear();
    m_borderMDValues.clear();
}

int32_t BorderFile::getNumberOfNodes() const
{
    if (m_structure == StructureEnum::ALL) return -1;//TSC: i think multi-structure should always return -1
    return m_numNodes;
}

void BorderFile::setNumberOfNodes(const int32_t& numNodes)
{
    if (numNodes < 1)
    {
        throw DataFileException(getFileName(), "attempt to set non-positive number of vertices on border file");
    }
    int numBorders = (int)m_borders.size();
    for (int i = 0; i < numBorders; ++i)
    {
        if (!m_borders[i]->verifyForSurfaceNumberOfNodes(numNodes))
        {
            throw DataFileException(getFileName(), "cannot set border file number of vertices less than the vertices used by its borders");
        }
    }
    m_numNodes = numNodes;//even if we are currently multi-structure, remember the number of nodes that was set
    setModified();
}

/**
 * If the number of nodes is not valid, it may indicate an old border file
 * that allows borders from multiple structures.  So, examine all of the
 * borders and if ALL of the borders resized on the same structure, set the
 * number of nodes.
 *
 * @param structureToNodeCount
 *    A map containing the number of nodes for each valid structure.
 * @throw DataFileException
 *    If a border is not valid for its corresponding structure
 *    (meaning, a node index used by a border is greater than the 
 *    number of nodes in the corresponding structure).
 */
void
BorderFile::updateNumberOfNodesIfSingleStructure(const std::map<StructureEnum::Enum, int32_t>& structureToNodeCount)
{
    if (getNumberOfNodes() > 0) {
        return;
    }
    
    const int32_t numBorders = getNumberOfBorders();

    /*
     * Verify that all borders in this file are for the same structure
     */
    bool allStructuresMatchFlag = true;
    StructureEnum::Enum firstBorderStructure = StructureEnum::INVALID;
    for (int32_t i = 0; i < numBorders; i++) {
        const StructureEnum::Enum structure = m_borders[i]->getStructure();
        
        if (i == 0) {
            firstBorderStructure = structure;
        }
        else {
            if (structure != firstBorderStructure) {
                allStructuresMatchFlag = false;
                break;
            }
        }
    }
    
    /*
     * If all of the borders in this file are for the same structure,
     * see if the number of nodes is available for the structure, and if so,
     * set the number of borders for the file.
     */
    if (allStructuresMatchFlag) {
        if ((firstBorderStructure != StructureEnum::INVALID)
            && (firstBorderStructure != StructureEnum::ALL)) {
            const std::map<StructureEnum::Enum, int32_t>::const_iterator iter = structureToNodeCount.find(firstBorderStructure);
            if (iter != structureToNodeCount.end()) {
                const int32_t numNodes = iter->second;
                setNumberOfNodes(numNodes);
                setStructure(firstBorderStructure);
                CaretLogInfo("Updated border file: "
                             + getFileNameNoPath()
                             + " structure="
                             + StructureEnum::toGuiName(firstBorderStructure)
                             + " number-of-nodes="
                             + AString::number(numNodes));
            }
        }
    }
}


/**
 * @return the number of borders.
 */
int32_t 
BorderFile::getNumberOfBorders() const
{
    return m_borders.size();
}

/**
 * Get the border at the given index.
 * @param indx
 *   Index of the border.
 * @return
 *   Border at the given index.
 */
Border* 
BorderFile::getBorder(const int32_t indx)
{
    CaretAssertVectorIndex(m_borders, indx);
    return m_borders[indx];
}

/**
 * Is the given border in this border file?
 *
 * @param border
 *    The border being queried
 * @return 
 *     True if the border is in this file, else false.
 */
bool
BorderFile::containsBorder(const Border* border) const
{
    if (std::find(m_borders.begin(),
                  m_borders.end(),
                  border) != m_borders.end()) {
        return true;
    }
    
    return false;
}


/**
 * Get the border at the given index.
 * @param indx
 *   Index of the border.
 * @return
 *   Border at the given index.
 */
const Border* 
BorderFile::getBorder(const int32_t indx) const
{
    CaretAssertVectorIndex(m_borders, indx);
    return m_borders[indx];
}

/**
 * Find ALL borders that have one endpoint within the given distance
 * of the first point of the given border segment.
 *
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param surfaceFile
 *    Surface file used for unprojection of border points.
 * @param borderSegment
 *    The border segment.
 * @param maximumDistance
 *    Maximum distance coordinate can be from a border point.
 * @param borderPointsOut
 *    Contains result of search.
 */
void
BorderFile::findAllBordersWithEndPointNearSegmentFirstPoint(const DisplayGroupEnum::Enum displayGroup,
                                                            const int32_t browserTabIndex,
                                                            const SurfaceFile* surfaceFile,
                                                            const Border* borderSegment,
                                                            const float maximumDistance,
                                                            std::vector<BorderPointFromSearch>& borderPointsOut) const
{
    CaretAssert(surfaceFile);
    CaretAssert(borderSegment);
    
    borderPointsOut.clear();
    
    if (borderSegment->getNumberOfPoints() < 2) {
        return;
    }
    
    float segFirstXYZ[3];
    if (! borderSegment->getPoint(0)->getProjectedPosition(*surfaceFile, segFirstXYZ, false)) {
        /*
         * Point did not unproject
         */
        return;
    }
    
    BorderFile* nonConstBorderFile = const_cast<BorderFile*>(this);
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t borderIndex = 0; borderIndex < numBorders; borderIndex++) {
        Border* border = m_borders[borderIndex];
        if (nonConstBorderFile->isBorderDisplayed(displayGroup,
                                                  browserTabIndex,
                                                  border) == false) {
            continue;
        }
        if (border->getStructure() == surfaceFile->getStructure()) {
            /*
             * Test first endpoint
             */
            const int32_t numPoints = border->getNumberOfPoints();
            float nearestPointDistanceSquared = -1.0;
            int32_t neartestPointIndex = -1;
            if (numPoints > 0) {
                float pointXYZ[3];
                if (border->getPoint(0)->getProjectedPosition(*surfaceFile,
                                                              pointXYZ,
                                                              false)) {
                    nearestPointDistanceSquared = MathFunctions::distanceSquared3D(pointXYZ,
                                                                            segFirstXYZ);
                    neartestPointIndex = 0;
                }
            }
            
            /*
             * Test last endpoint
             */
            if (numPoints > 1) {
                const int32_t lastPointIndex = numPoints - 1;
                float pointXYZ[3];
                if (border->getPoint(lastPointIndex)->getProjectedPosition(*surfaceFile,
                                                                           pointXYZ,
                                                                           false)) {
                    const float dist2 = MathFunctions::distanceSquared3D(pointXYZ,
                                                                         segFirstXYZ);
                    if (nearestPointDistanceSquared >= 0.0) {
                        if (dist2 < nearestPointDistanceSquared) {
                            neartestPointIndex   = lastPointIndex;
                            nearestPointDistanceSquared = dist2;
                        }
                    }
                    else {
                        neartestPointIndex   = lastPointIndex;
                        nearestPointDistanceSquared = dist2;
                    }
                }
            }
            
            if (neartestPointIndex >= 0) {
                const float nearestPointDistance = std::sqrt(nearestPointDistanceSquared);
                if (nearestPointDistance <= maximumDistance) {
                    BorderPointFromSearch bpo;
                    bpo.setData(const_cast<BorderFile*>(this),
                                border,
                                borderIndex,
                                neartestPointIndex,
                                nearestPointDistance);
                    borderPointsOut.push_back(bpo);
                }
            }
        }
    }    
}


/**
 * Find ALL borders that have any point within the given distance
 * of the first point of the given border segment.
 *
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param surfaceFile
 *    Surface file used for unprojection of border points.
 * @param borderSegment
 *    The border segment.
 * @param maximumDistance
 *    Maximum distance coordinate can be from a border point.
 * @param borderPointsOut
 *    Contains result of search.
 */
void
BorderFile::findAllBordersWithAnyPointNearSegmentFirstPoint(const DisplayGroupEnum::Enum displayGroup,
                                                          const int32_t browserTabIndex,
                                                          const SurfaceFile* surfaceFile,
                                                          const Border* borderSegment,
                                                          const float maximumDistance,
                                                          std::vector<BorderPointFromSearch>& borderPointsOut) const
{
    CaretAssert(surfaceFile);
    CaretAssert(borderSegment);

    borderPointsOut.clear();
    
    if (borderSegment->getNumberOfPoints() < 2) {
        return;
    }
    
    float segFirstXYZ[3];
    if (! borderSegment->getPoint(0)->getProjectedPosition(*surfaceFile, segFirstXYZ, false)) {
        /*
         * Point did not unproject
         */
        return;
    }
    
    BorderFile* nonConstBorderFile = const_cast<BorderFile*>(this);
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t borderIndex = 0; borderIndex < numBorders; borderIndex++) {
        Border* border = m_borders[borderIndex];
        if (nonConstBorderFile->isBorderDisplayed(displayGroup,
                                                  browserTabIndex,
                                                  border) == false) {
            continue;
        }
        if (border->getStructure() == surfaceFile->getStructure()) {
            /*
             * Test all points
             */
            float nearestPointDistanceSquared = std::numeric_limits<float>::max();
            int32_t nearestPointIndex = -1;
            const int32_t numPoints = border->getNumberOfPoints();
            for (int32_t pointIndex = 0; pointIndex < numPoints; pointIndex++) {
                float pointXYZ[3];
                if (border->getPoint(pointIndex)->getProjectedPosition(*surfaceFile,
                                                              pointXYZ,
                                                              false)) {
                    const float distSQ = MathFunctions::distanceSquared3D(pointXYZ,
                                                                          segFirstXYZ);
                    if (distSQ < nearestPointDistanceSquared) {
                        nearestPointDistanceSquared = distSQ;
                        nearestPointIndex = pointIndex;
                    }
                }
            }
            
            if (nearestPointIndex >= 0) {
                const float nearestPointDistance = std::sqrt(nearestPointDistanceSquared);
                if (nearestPointDistance <= maximumDistance) {
                    BorderPointFromSearch bpo;
                    bpo.setData(const_cast<BorderFile*>(this),
                                border,
                                borderIndex,
                                nearestPointIndex,
                                nearestPointDistance);
                    borderPointsOut.push_back(bpo);
                }
            }
        }
    }    
}

/**
 * Find ALL borders that have ANY points within the given distance
 * of the two given coordinates.
 *
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param surfaceFile
 *    Surface file used for unprojection of border points.
 * @param borderSegment
 *    The border segment.
 * @param maximumDistance
 *    Maximum distance coordinate can be from a border point.
 * @param borderPointsOut
 *    Contains result of search.
 */
void
BorderFile::findAllBordersWithPointsNearBothSegmentEndPoints(const DisplayGroupEnum::Enum displayGroup,
                                                             const int32_t browserTabIndex,
                                                             const SurfaceFile* surfaceFile,
                                                             const Border* borderSegment,
                                                             const float maximumDistance,
                                                             std::vector<BorderPointFromSearch>& borderPointsOut) const
{
    CaretAssert(surfaceFile);
    CaretAssert(borderSegment);
    
    borderPointsOut.clear();
    
    if (borderSegment->getNumberOfPoints() < 2) {
        return;
    }
    
    float segFirstXYZ[3], segLastXYZ[3];
    const int32_t segLpIndex = borderSegment->getNumberOfPoints() - 1;
    if (borderSegment->getPoint(0)->getProjectedPosition(*surfaceFile, segFirstXYZ, false)
        && borderSegment->getPoint(segLpIndex)->getProjectedPosition(*surfaceFile, segLastXYZ, false)) {
        /* OK - both points have valid coordinates */
    }
    else {
        /* One or both points failed to project */
        return;
    }
    
    BorderFile* nonConstBorderFile = const_cast<BorderFile*>(this);
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t borderIndex = 0; borderIndex < numBorders; borderIndex++) {
        Border* border = m_borders[borderIndex];
        if (nonConstBorderFile->isBorderDisplayed(displayGroup,
                                                  browserTabIndex,
                                                  border) == false) {
            continue;
        }
        if (border->getStructure() == surfaceFile->getStructure()) {
            /*
             * Test first query point
             */
            float distance1 = 0.0;
            const int32_t nearestIndex1 = border->findPointIndexNearestXYZ(surfaceFile,
                                                                           segFirstXYZ,
                                                                           maximumDistance,
                                                                           distance1);
            
            float distance2 = 0.0;
            const int32_t nearestIndex2 = border->findPointIndexNearestXYZ(surfaceFile,
                                                                           segLastXYZ,
                                                                           maximumDistance,
                                                                           distance2);
            if ((nearestIndex1 >= 0)
                && (nearestIndex2 >= 0)) {
                const float averageDistance = (distance1 + distance2) / 2.0;
                BorderPointFromSearch bpo;
                bpo.setData(const_cast<BorderFile*>(this),
                            border,
                            borderIndex,
                            nearestIndex1,
                            averageDistance);
                borderPointsOut.push_back(bpo);
            }
        }
    }
}

/**
 * Find ALL borders that have ANY points within the given
 * region of interest.  Since all borders are projected to 
 * the surface, 
 *
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param surfaceFile
 *    Surface file used for unprojection of border points.
 * @param nodesInROI
 *    Indices if a node is inside (true) or outside (false) the ROI.
 *    Number of elements MUST BE the number of nodes in the surface.
 * @param insideCountAndBorderOut
 *    Output vector pair with first being number of border points inside the ROI and
 *    and second is the corresponding border.
 */
void
BorderFile::findBordersInsideRegionOfInterest(const DisplayGroupEnum::Enum displayGroup,
                                              const int32_t browserTabIndex,
                                              const SurfaceFile* surfaceFile,
                                              const std::vector<bool>& nodesInROI,
                                              std::vector<pair<int32_t, Border*> >& insideCountAndBorderOut) const
{
    CaretAssert(surfaceFile);
    CaretAssert(surfaceFile->getNumberOfNodes() == static_cast<int32_t>(nodesInROI.size()));
    
    insideCountAndBorderOut.clear();
    
    const StructureEnum::Enum surfaceStructure = surfaceFile->getStructure();
    
    BorderFile* nonConstBorderFile = const_cast<BorderFile*>(this);
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t borderIndex = 0; borderIndex < numBorders; borderIndex++) {
        Border* border = m_borders[borderIndex];
        if (nonConstBorderFile->isBorderDisplayed(displayGroup,
                                                  browserTabIndex,
                                                  border) == false) {
            continue;
        }
        if (border->getStructure() == surfaceStructure) {
//            const int32_t numberOfPoints = border->getNumberOfPoints();
//            for (int32_t iPoint = 0; iPoint < numberOfPoints; iPoint++) {
//                const SurfaceProjectedItem* spi = border->getPoint(iPoint);
//                CaretAssert(spi);
//                const SurfaceProjectionBarycentric* bary = spi->getBarycentricProjection();
//                if (bary != NULL) {
//                    const int32_t* pointNodes = bary->getTriangleNodes();
//                    const int32_t p1 = pointNodes[0];
//                    const int32_t p2 = pointNodes[1];
//                    const int32_t p3 = pointNodes[2];
//                    if ((p1 < surfaceNumberOfNodes)
//                        && (p2 < surfaceNumberOfNodes)
//                        && (p3 < surfaceNumberOfNodes)) {
//                        if (nodesInROI[p1]
//                            || nodesInROI[p2]
//                            || nodesInROI[p3]) {
//                            bordersOut.push_back(border);
//                            break;
//                        }
//                    }
//                }
//            }
            
            /*
             * Get all node indices from the border
             * Note that the node indices may be used by more than one border point barycentric projection
             * so first get all UNIQUE node indices that are used by the border points
             */
            std::set<int32_t> borderNodeIndicesInsideROI;
            const int32_t numberOfPoints = border->getNumberOfPoints();
            for (int32_t iPoint = 0; iPoint < numberOfPoints; iPoint++) {
                const SurfaceProjectedItem* spi = border->getPoint(iPoint);
                CaretAssert(spi);
                const SurfaceProjectionBarycentric* bary = spi->getBarycentricProjection();
                if (bary != NULL) {
                    const int32_t* pointNodes = bary->getTriangleNodes();
                    borderNodeIndicesInsideROI.insert(pointNodes[0]);
                    borderNodeIndicesInsideROI.insert(pointNodes[1]);
                    borderNodeIndicesInsideROI.insert(pointNodes[2]);
                }
            }
            
            int32_t borderNodesInsideROICount = 0;
            for (std::set<int32_t>::iterator iter = borderNodeIndicesInsideROI.begin();
                 iter != borderNodeIndicesInsideROI.end();
                 iter++) {
                const int32_t nodeIndex = *iter;
                CaretAssertVectorIndex(nodesInROI, nodeIndex);
                if (nodesInROI[nodeIndex]) {
                    borderNodesInsideROICount++;
                }
            }
            
            if (borderNodesInsideROICount > 0) {
                insideCountAndBorderOut.push_back(std::make_pair(borderNodesInsideROICount,
                                                              border));
            }
        }
    }
}


/**
 * Add a border.  NOTE: This border file
 * takes ownership of the 'border' and
 * will handle deleting it.  After calling 
 * this method, the caller must never
 * do anything with the border that was passed
 * to this method.
 *
 * @param border
 *    Border added to this border file.
 */
void 
BorderFile::addBorder(Border* border)
{
    int numPoints = border->getNumberOfPoints();
    if (numPoints == 0)
    {
        delete border;//keep our word and handle deleting the argument
        throw DataFileException(getFileName(), "attempt to add border with zero points");
    }//NOTE: Border itself makes sure all points are on one structure, and have barycentric projections
    if (m_borders.empty())//TSC: i'm not actually sure if we want border files to automatically set their structure from the borders
    {
        m_structure = border->getStructure();
    } else {
        if (m_structure != StructureEnum::ALL && m_structure != border->getPoint(0)->getStructure())
        {
            m_structure = StructureEnum::ALL;
        }
    }
    if (m_numNodes != -1)
    {
        if (!border->verifyForSurfaceNumberOfNodes(m_numNodes))
        {
            delete border;
            throw DataFileException(getFileName(), "attempt to add border that has too large vertex indices for surface");
        }
    }
    m_borders.push_back(border);

// DO NOT WANT to add entries to name and class tables when the names are
// not an exact match as partial matches are acceptable.
//    const AString name = border->getName();
//    if (name.isEmpty() == false) {
//        const int32_t nameColorKey = m_nameColorTable->getLabelKeyFromName(name);
//        if (nameColorKey < 0) {
//            m_nameColorTable->addLabel(name, 0.0f, 0.0f, 0.0f, 1.0f);
//        }
//    }
//    const AString className = border->getClassName();
//    if (className.isEmpty() == false) {
//        const int32_t classColorKey = m_classColorTable->getLabelKeyFromName(className);
//        if (classColorKey < 0) {
//            m_classColorTable->addLabel(className, 0.0f, 0.0f, 0.0f, 1.0f);
//        }
//    }
    
    m_forceUpdateOfGroupAndNameHierarchy = true;
    setModified();
}

/**
 * Add a border.  NOTE: This border file
 * takes ownership of the 'border' and
 * will handle deleting it.  After calling
 * this method, the caller must never
 * do anything with the border that was passed
 * to this method.
 *
 * @param border
 *    Border added to this border file and if the border's name or class colors
 *    are not in the respective color tables, add them to the color tables.
 */
void
BorderFile::addBorderUseColorsFromBorder(Border* border)
{
    const AString name = border->getName();
    if (name.isEmpty() == false) {
        const int32_t nameColorKey = m_nameColorTable->getLabelKeyFromName(name);
        if (nameColorKey < 0) {
            if (border->isNameRgbaValid()) {
                float rgba[4];
                border->getNameRgba(rgba);
                m_nameColorTable->addLabel(name, rgba[0], rgba[1], rgba[2], 1.0f);
            }
            else {
                m_nameColorTable->addLabel(name, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        else if (border->isNameRgbaValid()) {
            const GiftiLabel* label(m_nameColorTable->getLabel(nameColorKey));
            if (label != NULL) {
                std::array<float, 4> borderRGBA;
                border->getNameRgba(borderRGBA.data());
                std::array<float, 4> rgba;
                label->getColor(rgba.data());
                if (borderRGBA != rgba) {
                    AString msg("Adding border with name color ("
                                + AString::fromNumbers(borderRGBA.data(), 4)
                                + ") different than existing name color ("
                                + AString::fromNumbers(rgba.data(), 4)
                                + ") in border file.");
                    CaretLogWarning(msg);
                }
            }
        }
    }
    AString className = border->getClassName();
    if (className.isEmpty() == false) {
        const int32_t classColorKey = m_classColorTable->getLabelKeyFromName(className);
        if (classColorKey < 0) {
            if (border->isClassRgbaValid()) {
                float rgba[4];
                border->getClassRgba(rgba);
                m_classColorTable->addLabel(className, rgba[0], rgba[1], rgba[2], 1.0f);
            }
            else {
                m_classColorTable->addLabel(className, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        else if (border->isClassRgbaValid()) {
            const GiftiLabel* label(m_classColorTable->getLabel(classColorKey));
            if (label != NULL) {
                std::array<float, 4> borderRGBA;
                border->getClassRgba(borderRGBA.data());
                std::array<float, 4> rgba;
                label->getColor(rgba.data());
                if (borderRGBA != rgba) {
                    AString msg("Adding border with class color ("
                                + AString::fromNumbers(borderRGBA.data(), 4)
                                + ") different than existing border color ("
                                + AString::fromNumbers(rgba.data(), 4)
                                + ") in border file.");
                    CaretLogWarning(msg);
                }
            }
        }
    }
    
    border->setNameRgbaInvalid();
    border->setClassRgbaInvalid();
    addBorder(border);
    
    m_forceUpdateOfGroupAndNameHierarchy = true;
    setModified();
}

/**
 * Remove the border at the given index.
 * @param indx
 *   Index of border for removal.
 */
void 
BorderFile::removeBorder(const int32_t indx)
{
    CaretAssertVectorIndex(m_borders, indx);
    Border* border = getBorder(indx);
    m_borders.erase(m_borders.begin() + indx);
    delete border;
    if (m_structure == StructureEnum::ALL)
    {
        int numBorders = (int)m_borders.size();
        bool singleStructure = true;
        for (int i = 1; i < numBorders; ++i)
        {
            if (m_borders[i]->getStructure() != m_borders[0]->getStructure())
            {
                singleStructure = false;
                break;
            }
        }
        if (singleStructure)
        {
            if (numBorders != 0)//have it remember its structure when the last border is removed
            {
                m_structure = m_borders[0]->getStructure();
            }
        }
    }
    m_forceUpdateOfGroupAndNameHierarchy = true;
    setModified();
}

/**
 * Remove the border.
 * @param border
 *   Border that will be removed and DELETED.
 */
void 
BorderFile::removeBorder(Border* border)
{
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0;i < numBorders; i++) {
        if (m_borders[i] == border) {
            removeBorder(i);
            return;
        }
    }
    CaretLogWarning("Attempting to delete border not in border file with name: "
                        + border->getName());
}

/**
 * Is the given border, that MUST be in this file, displayed?
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param border
 *    Border that is tested to see if it is displayed.
 * @return 
 *    true if border is displayed, else false.
 */
bool
BorderFile::isBorderDisplayed(const DisplayGroupEnum::Enum displayGroup,
                              const int32_t browserTabIndex,
                              const Border* border)
{
    const GroupAndNameHierarchyItem* selectionItem = border->getGroupNameSelectionItem();
    if (selectionItem != NULL) {
        if (selectionItem->isSelected(displayGroup,
                                      browserTabIndex) == false) {
            return false;
        }
    }
        
    return true;
}

/**
 * @return The class and name hierarchy.
 */
GroupAndNameHierarchyModel*
BorderFile::getGroupAndNameHierarchyModel()
{
    m_classNameHierarchy->update(this,
                                 m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * @return  The class color table.
 */
GiftiLabelTable* 
BorderFile::getClassColorTable()
{
    return m_classColorTable;
}

/**
 * @return  The class color table.
 */
const GiftiLabelTable* 
BorderFile::getClassColorTable() const
{
    return m_classColorTable;
}

/**
 * @return  The name color table.
 */
GiftiLabelTable*
BorderFile::getNameColorTable()
{
    return m_nameColorTable;
}

/**
 * @return  The name color table.
 */
const GiftiLabelTable*
BorderFile::getNameColorTable() const
{
    return m_nameColorTable;
}

int BorderFile::getIndexForBorderMetadataKey(const AString& key) const
{
    for (int i = 0; i < (int)m_borderMDKeys.size(); ++i)//for now, the dumb way, we don't expect hundreds of keys - can speed it up with a secondary map if needed
    {
        if (m_borderMDKeys[i] == key) return i;//NOTE: if we use a second structure to speed this up, make sure parseBorderMDNames3 also updates it!
    }
    return -1;
}

const AString& BorderFile::getBorderMetadataKey(const int& index) const
{
    CaretAssert(index >= 0 && index < (int)m_borderMDKeys.size());
    return m_borderMDKeys[index];
}

int BorderFile::addBorderMetadataKey(const AString& key)
{
    int checkKey = getIndexForBorderMetadataKey(key);
    if (checkKey != -1) return checkKey;
    m_borderMDKeys.push_back(key);//NOTE: if we use a second structure to speed this up, make sure parseBorderMDNames3 also updates it!
    for (map<pair<AString, AString>, vector<AString> >::iterator iter = m_borderMDValues.begin(); iter != m_borderMDValues.end(); ++iter)
    {
        iter->second.push_back(AString());
        CaretAssert(iter->second.size() == m_borderMDKeys.size());
    }
    setModified();
    return m_borderMDKeys.size() - 1;
}

void BorderFile::removeBorderMetadataKey(const int& index)
{
    CaretAssert(index >= 0 && index < (int)m_borderMDKeys.size());
    m_borderMDKeys.erase(m_borderMDKeys.begin() + index);
    for (map<pair<AString, AString>, vector<AString> >::iterator iter = m_borderMDValues.begin(); iter != m_borderMDValues.end(); ++iter)
    {
        iter->second.erase(iter->second.begin() + index);
        CaretAssert(iter->second.size() == m_borderMDKeys.size());
    }
    setModified();
}

void BorderFile::clearBorderMetaData()
{
    m_borderMDKeys.clear();
    m_borderMDValues.clear();
}

AString BorderFile::getBorderMetadataValue(const AString& name, const AString& className, const int& index) const
{
    CaretAssert(index >= 0 && index < (int)m_borderMDKeys.size());
    map<pair<AString, AString>, vector<AString> >::const_iterator iter = m_borderMDValues.find(make_pair(name, className));
    if (iter == m_borderMDValues.end()) return AString();
    CaretAssert(iter->second.size() == m_borderMDKeys.size());
    return iter->second[index];
}

void BorderFile::setBorderMetadataValue(const AString& name, const AString& className, const int& index, const AString& value)
{
    CaretAssert(index >= 0 && index < (int)m_borderMDKeys.size());
    //TODO: check if such a border actually exists?  maybe not needed
    map<pair<AString, AString>, vector<AString> >::iterator iter = m_borderMDValues.find(make_pair(name, className));
    if (iter == m_borderMDValues.end())
    {
        if (value == "") return;//don't create the metadata values array if we only store the empty string
        iter = m_borderMDValues.insert(make_pair(make_pair(name, className), vector<AString>(m_borderMDKeys.size()))).first;
    }
    CaretAssert(iter->second.size() == m_borderMDKeys.size());
    iter->second[index] = value;
    setModified();
}

/**
 * Version 1 border files contained one color table for both names
 * and classes.  Newer versions of the border file keep them in
 * separate tables.
 *
 * @param oldColorTable
 *    Old color table that is split into name and class color tables.
 */
void
BorderFile::createNameAndClassColorTables(const GiftiLabelTable* oldColorTable)
{
    CaretAssert(oldColorTable);
    
    m_classColorTable->clear();
    m_nameColorTable->clear();
    
    std::set<QString> nameSet;
    std::set<QString> classSet;
    
    const int numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        const Border* border = getBorder(i);
        nameSet.insert(border->getName());
        classSet.insert(border->getClassName());
    }
    
    /*
     * Create colors for only the "best matching" color.
     */
    for (std::set<QString>::iterator iter = nameSet.begin();
         iter != nameSet.end();
         iter++) {
        const AString colorName = *iter;
        const GiftiLabel* oldLabel = oldColorTable->getLabelBestMatching(colorName);
        if (oldLabel != NULL) {
            const AString bestMatchingName = oldLabel->getName();
            const int32_t labelKey = m_nameColorTable->getLabelKeyFromName(bestMatchingName);
            if (labelKey < 0) {
                float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
                oldLabel->getColor(rgba);
                m_nameColorTable->addLabel(bestMatchingName,
                                           rgba[0],
                                           rgba[1],
                                           rgba[2],
                                           rgba[3]);
            }
        }
    }
    
    /*
     * Create a color for each class name using the best matching color
     */
    for (std::set<QString>::iterator iter = classSet.begin();
         iter != classSet.end();
         iter++) {
        const AString colorName = *iter;
        const GiftiLabel* label = oldColorTable->getLabelBestMatching(colorName);
        float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
        if (label != NULL) {
            label->getColor(rgba);
        }
        m_classColorTable->addLabel(colorName,
                                    rgba[0],
                                    rgba[1],
                                    rgba[2],
                                    rgba[3]);
    }
}

/**
 * @return A string list containing all border names
 * sorted in alphabetical order.
 */
QStringList
BorderFile::getAllBorderNamesSorted() const
{
    std::set<QString> nameSet;
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0;i < numBorders; i++) {
        nameSet.insert(m_borders[i]->getName());
    }
    
    QStringList sl;
    for (std::set<QString>::iterator iter = nameSet.begin();
         iter != nameSet.end();
         iter++) {
        sl += *iter;
    }
    
    return sl;
}

/**
 * Invalidate all assigned colors.
 */
void
BorderFile::invalidateAllAssignedColors()
{
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        m_borders[i]->setClassRgbaInvalid();
        m_borders[i]->setNameRgbaInvalid();
    }
    m_forceUpdateOfGroupAndNameHierarchy = true;
}

/**
 * @return The version of the file as a number.
 */
int32_t
BorderFile::getFileVersion()
{
    return s_borderFileVersion;
}

/**
 * @return The version of the file as a string.
 */
AString 
BorderFile::getFileVersionAsString()
{
    return AString::number(s_borderFileVersion);
}

/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void 
BorderFile::readFile(const AString& filename)
{
    clear();
    checkFileReadability(filename);
    setFileName(filename);
    
    {
        QFile inFile(filename);
        if (!inFile.open(QIODevice::ReadOnly)) throw DataFileException(filename,
                                                                       "failed to open file for reading");
        QXmlStreamReader myReader(&inFile);
        readXML(myReader);
    }
    
    /*BorderFileSaxReader saxReader(this);
    std::unique_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
        clear();
        setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        AString msg =
        "Parse Error while reading "
        + filename;
        
        if ((lineNum >= 0) && (colNum >= 0)) {
            msg += (" line/col ("
                    + AString::number(e.getLineNumber())
                    + "/"
                    + AString::number(e.getColumnNumber())
                    + ")");
        }
        
        msg += (": " + e.whatString());
        
        DataFileException dfe(msg);
        CaretLogThrowing(dfe);
        throw dfe;
    }//*/
    
    setFileName(filename);
    
    m_classNameHierarchy->update(this,
                                     true);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    m_classNameHierarchy->setAllSelected(true);
    
    CaretLogFiner("CLASS/NAME Table for : "
                  + getFileNameNoPath()
                  + "\n"
                  + m_classNameHierarchy->toString());
    
    clearModified();
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void 
BorderFile::writeFile(const AString& filename)
{
    if (canWriteAsVersion(3))
    {
        writeFile(filename, 3);
    } else {
        CaretLogWarning("border file missing information required for writing as version 3, falling back to older format");
        writeFile(filename, 1);
    }
}

/**
 * @return Error message for attempting a gui operation on an obsolete, multi-structure border file.
 */
AString
BorderFile::getObsoleteMultiStructureFormatMessage()
{
    return ("This border file ("
            + getFileNameNoPath()
            + ") contains borders for multiple structures and must be split into single-structure border files.  "
            "This can be done in the gui, using a selection in the Data menu, "
            "or on the command line using -file-convert with the -border-version-convert option.");
}

void BorderFile::writeFile(const AString& filename, const int& version)
{
    if ( ! isSingleStructure()) {
        throw DataFileException(filename,
                                "Writing multi-structure border files is no longer supported.  "
                                "Any existing multi-structure border files should be split into single-structure border files.  "
                                "This can be done on the command line using -file-convert with the -border-version-convert option, "
                                "or in the gui, using a selection in the Data menu.");
    }
    
    if (!canWriteAsVersion(version)) throw DataFileException(filename, "cannot write border file as version '" + AString::number(version) + "'");
    
    if (!(filename.endsWith(".border") || filename.endsWith(".wb_border")))
    {
        CaretLogWarning("border file '" + filename + "' should be saved ending in .border");
    }
    checkFileWritability(filename);
    
    setFileName(filename);
    //QFile::remove(filename);//delete it if it exists, to play better with file symlinks
    remove(QDir::toNativeSeparators(filename).toLocal8Bit());//QFile::remove inappropriately checks file permissions and refuses to try deleting (when folder permissions may allow it)
    switch (version)
    {
        case 3:
        {
            QFile myFile(filename);
            if (!myFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) throw DataFileException(filename,
                                                                                                  "could not open for writing");
            QXmlStreamWriter myXML(&myFile);
            myXML.setAutoFormatting(true);
            writeVersion3(myXML);
            break;
        }
        case 1:
        {
            //
            // Format the version string so that it ends with at most one zero
            //
            const AString versionString = AString::number(1.0);
            
            //
            // Open the file
            //
            FileAdapter file;
            AString errorMessage;
            QTextStream* textStream = file.openQTextStreamForWritingFile(getFileName(),
                                                                        errorMessage);
            if (textStream == NULL) {
                throw DataFileException(getFileName(),
                                        errorMessage);
            }

            //
            // Create the xml writer
            //
            XmlWriter xmlWriter(*textStream);
            
            //
            // Write header info
            //
            xmlWriter.writeStartDocument("1.0");
            
            //
            // Write GIFTI root element
            //
            XmlAttributes attributes;
            
            //attributes.addAttribute("xmlns:xsi",
            //                        "http://www.w3.org/2001/XMLSchema-instance");
            //attributes.addAttribute("xsi:noNamespaceSchemaLocation",
            //                        "http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd");
            attributes.addAttribute(BorderFile::XML_ATTRIBUTE_VERSION,
                                    versionString);
            xmlWriter.writeStartElement(BorderFile::XML_TAG_BORDER_FILE,
                                        attributes);
            
            //
            // Write Metadata
            //
            if (m_metadata != NULL) {
                m_metadata->writeAsXML(xmlWriter);
            }
                
            //
            // Write the class color table
            //
            xmlWriter.writeStartElement(XML_TAG_CLASS_COLOR_TABLE);
            m_classColorTable->writeAsXML(xmlWriter);
            xmlWriter.writeEndElement();
            
            //
            // Write the name color table
            //
            xmlWriter.writeStartElement(XML_TAG_NAME_COLOR_TABLE);
            m_nameColorTable->writeAsXML(xmlWriter);
            xmlWriter.writeEndElement();
            
            
            //
            // Write borders
            //
            const int32_t numBorders = getNumberOfBorders();
            for (int32_t i = 0; i < numBorders; i++) {
                if (m_borders[i]->getNumberOfPoints() < 1)
                {
                    CaretLogWarning("skipped writing zero-point border: '" + m_borders[i]->getName() + "'");
                    continue;
                }
                m_borders[i]->writeAsXML(xmlWriter);
            }
            
            xmlWriter.writeEndElement();
            xmlWriter.writeEndDocument();
            
            file.close();
            break;
        }
        default:
            CaretAssertMessage(0, "unimplemented writer for claimed supported version");
            break;
    }
            
    clearModified();
}

bool BorderFile::canWriteAsVersion(const int& version) const
{
    switch (version)
    {
        case 1:
            return true;
        case 3:
            if (!StructureEnum::isSingleStructure(m_structure)) return false;
            if (m_numNodes == -1) return false;
            return true;
        default:
            return false;
    }
}

void BorderFile::writeVersion3(QXmlStreamWriter& output) const
{
    CaretAssert(canWriteAsVersion(3));//if this function is made public, this should also throw
    output.writeStartDocument();
    output.writeStartElement("BorderFile");
    output.writeAttribute("Version", "3");
    output.writeAttribute("Structure", StructureEnum::toName(m_structure));
    output.writeAttribute("SurfaceNumberOfVertices", AString::number(m_numNodes));
    
    m_metadata->writeBorderFileXML3(output);
    
    int numBorderMDKeys = (int)m_borderMDKeys.size();
    if (numBorderMDKeys > 0)
    {
        output.writeStartElement("BorderMetaDataNames");
        for (int i = 0; i < numBorderMDKeys; ++i)
        {
            output.writeStartElement("Name");
            output.writeCharacters(m_borderMDKeys[i]);//CDATA?
            output.writeEndElement();
        }
        output.writeEndElement();
    }
    
    int numBorders = getNumberOfBorders();
    vector<bool> used(numBorders, false);//multi-part border behavior
    for (int i = 0; i < numBorders; ++i)
    {
        if (used[i]) continue;
        const Border* classBorder = getBorder(i);
        AString thisClass = classBorder->getClassName();//hierarchical representation in file
        output.writeStartElement("Class");
        output.writeAttribute("Name", thisClass);
        writeColorHelper(output, getClassColorTable()->getLabelBestMatching(thisClass));
        //writeColorHelper(output, getClassColorTable()->getLabel(thisClass));
        for (int j = i; j < numBorders; ++j)
        {
            if (used[j]) continue;
            const Border* nameBorder = getBorder(j);
            if (nameBorder->getClassName() == thisClass)
            {
                AString thisName = nameBorder->getName();//multipart borders
                output.writeStartElement("Border");
                output.writeAttribute("Name", thisName);
                writeColorHelper(output, getNameColorTable()->getLabelBestMatching(thisName));
                //writeColorHelper(output, getNameColorTable()->getLabel(thisName));
                map<pair<AString, AString>, vector<AString> >::const_iterator iter = m_borderMDValues.find(make_pair(thisName, thisClass));
                if (iter != m_borderMDValues.end())
                {
                    CaretAssert(iter->second.size() == m_borderMDKeys.size());
                    output.writeStartElement("BorderMetaDataValues");
                    for (int k = 0; k < (int)m_borderMDKeys.size(); ++k)
                    {
                        output.writeStartElement("Value");
                        output.writeCharacters(iter->second[k]);//CDATA?
                        output.writeEndElement();
                    }
                    output.writeEndElement();
                }
                for (int k = j; k < numBorders; ++k)
                {
                    if (used[k]) continue;
                    const Border* thisBorder = getBorder(k);
                    if (thisBorder->getNumberOfPoints() < 1)
                    {
                        used[k] = true;
                        CaretLogWarning("skipped writing zero-point border: '" + thisBorder->getName() + "'");
                        continue;
                    }
                    if (thisBorder->getName() == thisName && thisBorder->getClassName() == thisClass)
                    {
                        used[k] = true;
                        thisBorder->writeXML3(output);
                    }
                }
                output.writeEndElement();//Border
            }
        }
        output.writeEndElement();//Class
    }
    output.writeEndElement();//BorderFile
}

void BorderFile::writeColorHelper(QXmlStreamWriter& output, const GiftiLabel* colorLabel) const
{
    if (colorLabel == NULL)//default to black if we somehow have no color
    {
        output.writeAttribute("Red", AString::number(0.0f));
        output.writeAttribute("Green", AString::number(0.0f));
        output.writeAttribute("Blue", AString::number(0.0f));
    } else {
        output.writeAttribute("Red", AString::number(colorLabel->getRed()));
        output.writeAttribute("Green", AString::number(colorLabel->getGreen()));
        output.writeAttribute("Blue", AString::number(colorLabel->getBlue()));
    }
}

void BorderFile::readXML(QXmlStreamReader& xml)
{
    clear();
    bool haveRoot = false;
    while (!xml.atEnd())
    {
        switch(xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                if (xml.name() != QLatin1String("BorderFile")) throw DataFileException(getFileName(),
                                                                        "unexpected root element: " + xml.name().toString());
                if (haveRoot) throw DataFileException(getFileName(),
                                                      "multiple BorderFile elements in one file");
                QXmlStreamAttributes myAttrs = xml.attributes();
                if (!myAttrs.hasAttribute("Version")) throw DataFileException(getFileName(),
                                                                              "missing required attribute Version of element BorderFile");
                auto versionStr = myAttrs.value("Version");
                if (versionStr == QLatin1String("1") || versionStr == QLatin1String("1.0"))
                {
                    parseBorderFile1(xml);
                } else if (versionStr == QLatin1String("3")) {
                    parseBorderFile3(xml);
                } else {
                    throw DataFileException(getFileName(),
                                            "unrecognized border file version: " + versionStr.toString());
                }
                haveRoot = true;
                break;
            }
            default:
                break;
        }
        xml.readNext();
    }
    if (xml.hasError()) throw DataFileException(getFileName(),
                                                "XML parsing error in root of border file: " + xml.errorString());
    if (!haveRoot) throw DataFileException(getFileName(),
                                           "BorderFile root element not found");
    clearModified();
}

void BorderFile::parseBorderFile1(QXmlStreamReader& xml)
{
    CaretAssert(xml.isStartElement() && xml.name() == QLatin1String("BorderFile"));
    bool haveSingleTable = false, haveClassTable = false, haveNameTable = false;
    GiftiLabelTable singleTable;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch(xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                auto name = xml.name();
                if (name == QLatin1String("MetaData"))
                {
                    m_metadata->readBorderFileXML1(xml);
                    if (xml.hasError()) throw DataFileException(getFileName(),
                                                                "XML parsing error in MetaData: " + xml.errorString());
                } else if (name == QLatin1String("BorderClassColorTable")) {
                    if (haveSingleTable) throw DataFileException(getFileName(),
                                                                 "file has both single-table and split-table coloring information");
                    if (haveClassTable) throw DataFileException(getFileName(),
                                                                "file has multiple BorderClassColorTable elements");
                    if (!xml.readNextStartElement())
                    {
                        if (xml.hasError()) throw DataFileException(getFileName(),
                                                                    "XML parsing error in BorderClassColorTable: " + xml.errorString());
                        throw DataFileException(getFileName(),
                                                "empty BorderClassColorTable found");
                    }
                    m_classColorTable->readFromQXmlStreamReader(xml);
                    xml.readNextStartElement();//find the end element of BorderClassColorTable
                    if (xml.hasError()) throw DataFileException(getFileName(),
                                                                "XML parsing error in BorderClassColorTable: " + xml.errorString());
                    haveClassTable = true;
                } else if (name == QLatin1String("BorderNameColorTable")) {
                    if (haveSingleTable) throw DataFileException(getFileName(),
                                                                 "file has both single-table and split-table coloring information");
                    if (haveNameTable) throw DataFileException(getFileName(),
                                                               "file has multiple BorderNameColorTable elements");
                    if (!xml.readNextStartElement())
                    {
                        if (xml.hasError()) throw DataFileException(getFileName(),
                                                                    "XML parsing error in BorderNameColorTable: " + xml.errorString());
                        throw DataFileException(getFileName(),
                                                "empty BorderNameColorTable found");
                    }
                    m_nameColorTable->readFromQXmlStreamReader(xml);
                    xml.readNextStartElement();//find the end element of BorderNameColorTable
                    if (xml.hasError()) throw DataFileException(getFileName(),
                                                                "XML parsing error in BorderNameColorTable: " + xml.errorString());
                    haveNameTable = true;
                } else if (name == QLatin1String("LabelTable")) {
                    if (haveNameTable || haveClassTable) throw DataFileException(getFileName(),
                                                                                 "file has both single-table and split-table coloring information");
                    if (haveSingleTable) throw DataFileException(getFileName(),
                                                                 "file has multiple LabelTable elements");
                    singleTable.readFromQXmlStreamReader(xml);
                    if (xml.hasError()) throw DataFileException(getFileName(),
                                                                "XML parsing error in LabelTable: " + xml.errorString());
                    haveSingleTable = true;
                } else if (name == QLatin1String("Border")) {
                    CaretPointer<Border> toParse(new Border());//so throw can clean up, but we can also release the Border pointer
                    toParse->readXML1(xml);
                    if (toParse->getNumberOfPoints() > 0)
                    {
                        addBorder(toParse.releasePointer());
                    } else {
                        CaretLogWarning("ignored border with zero points: '" + toParse->getName() + "'");
                    }
                } else {
                    throw DataFileException(getFileName(),
                                            "unexpected element in BorderFile: " + name.toString());
                }
                break;
            }
            default:
                break;
        }
    }
    if (haveSingleTable)
    {
        createNameAndClassColorTables(&singleTable);
    }
    if (xml.hasError()) throw DataFileException(getFileName(),
                                                "XML parsing error in BorderFile: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("BorderFile"));
    if (!haveSingleTable && (!haveClassTable || !haveNameTable))
    {
        throw DataFileException(getFileName(),
                                "border file is missing a required color table");
    }
}

void BorderFile::parseBorderFile3(QXmlStreamReader& xml)
{
    CaretAssert(xml.isStartElement() && xml.name() == QLatin1String("BorderFile"));
    QXmlStreamAttributes myAttrs = xml.attributes();
    bool ok = false;
    if (!myAttrs.hasAttribute("Structure")) throw DataFileException(getFileName(),
                                                                    "BorderFile is missing required attribute Structure");
    StructureEnum::Enum myStructure = StructureEnum::fromName(myAttrs.value("Structure").toString(), &ok);
    if (!ok) throw DataFileException(getFileName(),
                                     "unrecognized structure: " + myAttrs.value("Structure").toString());
    setStructure(myStructure);
    if (!myAttrs.hasAttribute("SurfaceNumberOfVertices")) throw DataFileException(getFileName(),
                                                                                  "BorderFile is missing required attribute SurfaceNumberOfVertices");
    int myNumNodes = myAttrs.value("SurfaceNumberOfVertices").toString().toInt(&ok);
    if (!ok) throw DataFileException(getFileName(),
                                     "non-integer number of vertices: " + myAttrs.value("SurfaceNumberOfVertices").toString());
    if (myNumNodes < 1) throw DataFileException(getFileName(),
                                                "number of vertices too small: ");
    setNumberOfNodes(myNumNodes);
    bool haveFileMD = false, haveBorderMDNames = false;
    set<AString> classNames;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        if (xml.isStartElement())
        {
            auto name = xml.name();
            if (name == QLatin1String("MetaData"))
            {
                if (haveFileMD) throw DataFileException(getFileName(),
                                                        "file has multiple MetaData elements");
                m_metadata->readBorderFileXML3(xml);
                if (xml.hasError()) throw DataFileException(getFileName(),
                                                            "XML parsing error in MetaData: " + xml.errorString());
                haveFileMD = true;
            } else if (name == QLatin1String("BorderMetaDataNames")) {
                if (haveBorderMDNames) throw DataFileException(getFileName(),
                                                               "file has multiple BorderMetaDataNames elements");
                parseBorderMDNames3(xml);
                haveBorderMDNames = true;
            } else if (name == QLatin1String("Class")) {
                AString className = parseClass3(xml);
                if (!classNames.insert(className).second) throw DataFileException(getFileName(),
                                                                                  "multiple classes using same name: " + className);
            } else {
                throw DataFileException(getFileName(),
                                        "unexpected element in BorderFile: " + name.toString());
            }
        }
    }
    if (xml.hasError()) throw DataFileException(getFileName(),
                                                "XML parsing error in BorderFile: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("BorderFile"));
    for (map<pair<AString, AString>, vector<AString> >::const_iterator iter = m_borderMDValues.begin(); iter != m_borderMDValues.end(); ++iter)
    {//someone could put the BorderMetaDataNames after a class, so check at the very end
        if (iter->second.size() != m_borderMDKeys.size())
        {
            throw DataFileException(getFileName(),
                                    "wrong number of border metadata values for border " + iter->first.first + ", class " + iter->first.second);
        }
    }
}

void BorderFile::parseBorderMDNames3(QXmlStreamReader& xml)
{
    CaretAssert(xml.isStartElement() && xml.name() == QLatin1String("BorderMetaDataNames"));
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        if (xml.isStartElement())
        {
            auto name = xml.name();
            if (name == QLatin1String("Name"))
            {
                QString mdName = xml.readElementText();//errors on unexpected element
                if (xml.hasError()) throw DataFileException(getFileName(),
                                                            "XML parsing error in Name: " + xml.errorString());
                int checkIndex = getIndexForBorderMetadataKey(mdName);
                if (checkIndex != -1) throw DataFileException(getFileName(),
                                                              "duplicate border metadata name: " + mdName);
                m_borderMDKeys.push_back(mdName);//NOTE: do NOT use addBorderMetadataKey, as if there are borders with metadata before this, it will mess things up
            } else {
                throw DataFileException(getFileName(),
                                        "unexpected element in BorderMetaDataNames: " + name.toString());
            }
        }
    }
    if (xml.hasError()) throw DataFileException(getFileName(),
                                                "XML parsing error in BorderMetaDataNames: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("BorderMetaDataNames"));
}

AString BorderFile::parseClass3(QXmlStreamReader& xml)
{
    CaretAssert(xml.isStartElement() && xml.name() == QLatin1String("Class"));
    QXmlStreamAttributes myAttrs = xml.attributes();
    if (!myAttrs.hasAttribute("Name")) throw DataFileException(getFileName(),
                                                               "Class is missing required attribute Name");
    AString className = myAttrs.value("Name").toString();
    float colorRGB[3];
    colorAttribHelper3(getFileName(),
                       xml, colorRGB);
    m_classColorTable->addLabel(className, colorRGB[0], colorRGB[1], colorRGB[2]);
    set<AString> borderNames;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        if (xml.isStartElement())
        {
            auto name = xml.name();
            if (name == QLatin1String("Border"))
            {
                AString borderName = parseBorder3(xml, className);
                if (!borderNames.insert(borderName).second) throw DataFileException(getFileName(),
                                                                                    "multiple borders in one class using same name: " + borderName);
            } else {
                throw DataFileException(getFileName(),
                                        "unexpected element in Class: " + name.toString());
            }
        }
    }
    if (xml.hasError()) throw DataFileException(getFileName(),
                                                "XML parsing error in Class: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("Class"));
    if (borderNames.size() == 0) throw DataFileException(getFileName(),
                                                         "Class " + className + " has no Border elements");
    return className;
}

AString BorderFile::parseBorder3(QXmlStreamReader& xml, const AString& className)
{
    CaretAssert(xml.isStartElement() && xml.name() == QLatin1String("Border"));
    bool haveMDValues = false;
    int numBorderParts = 0;
    QXmlStreamAttributes myAttrs = xml.attributes();
    if (!myAttrs.hasAttribute("Name")) throw DataFileException(getFileName(),
                                                               "Class is missing required attribute Name");
    AString borderName = myAttrs.value("Name").toString();
    float colorRGB[3];
    colorAttribHelper3(getFileName(),
                       xml, colorRGB);
    m_nameColorTable->addLabel(borderName, colorRGB[0], colorRGB[1], colorRGB[2]);
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        if (xml.isStartElement())
        {
            auto name = xml.name();
            if (name == QLatin1String("BorderPart"))
            {
                CaretPointer<Border> thisBorder(new Border());//again, because the current interface/internals take ownership of raw pointers
                thisBorder->readXML3(xml);
                thisBorder->setStructure(getStructure());
                thisBorder->setClassName(className);
                thisBorder->setName(borderName);
                if (!thisBorder->verifyForSurfaceNumberOfNodes(getNumberOfNodes())) throw DataFileException(getFileName(),
                                                                                                            "BorderPart uses node numbers larger than are valid for its surface");
                if (thisBorder->getNumberOfPoints() > 0)
                {
                    addBorder(thisBorder.releasePointer());
                } else {
                    CaretLogWarning("ignored border with zero points: '" + thisBorder->getName() + "'");
                }
                ++numBorderParts;
            } else if (name == QLatin1String("BorderMetaDataValues")) {
                if (haveMDValues) throw DataFileException(getFileName(),
                                                          "Border has multiple BorderMetaDataValues elements");
                m_borderMDValues[make_pair(borderName, className)] = parseBorderMDValues3(getFileName(),
                                                                                          xml);
                haveMDValues = true;
            } else {
                throw DataFileException(getFileName(),
                                        "unexpected element in Border: " + name.toString());
            }
        }
    }
    if (xml.hasError()) throw DataFileException(getFileName(),
                                                "XML parsing error in Border: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("Border"));
    if (numBorderParts == 0) throw DataFileException(getFileName(),
                                                     "Border has no BorderPart elements");
    return borderName;
}

void BorderFile::colorAttribHelper3(const AString& filename,
                                    QXmlStreamReader& xml, float rgbOut[3])
{
    QXmlStreamAttributes myAttrs = xml.attributes();
    bool ok = false;
    if (!myAttrs.hasAttribute("Red")) throw DataFileException(filename,
                                                              xml.name().toString() + " element missing required attribute Red");
    rgbOut[0] = myAttrs.value("Red").toString().toFloat(&ok);
    if (!ok) throw DataFileException(filename,
                                     "non-numeric Red attribute of " + xml.name().toString() + ": " + myAttrs.value("Red").toString());
    if (!myAttrs.hasAttribute("Green")) throw DataFileException(filename,
                                                                xml.name().toString() + " element missing required attribute Green");
    rgbOut[1] = myAttrs.value("Green").toString().toFloat(&ok);
    if (!ok) throw DataFileException(filename,
                                     "non-numeric Green attribute of " + xml.name().toString() + ": " + myAttrs.value("Green").toString());
    if (!myAttrs.hasAttribute("Blue")) throw DataFileException(filename,
                                                               xml.name().toString() + " element missing required attribute Blue");
    rgbOut[2] = myAttrs.value("Blue").toString().toFloat(&ok);
    if (!ok) throw DataFileException(filename,
                                     "non-numeric Blue attribute of " + xml.name().toString() + ": " + myAttrs.value("Blue").toString());
}

vector<AString> BorderFile::parseBorderMDValues3(const AString& filename,
                                                 QXmlStreamReader& xml)
{
    CaretAssert(xml.isStartElement() && xml.name() == QLatin1String("BorderMetaDataValues"));
    vector<AString> ret;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        if (xml.isStartElement())
        {
            auto name = xml.name();
            if (name == QLatin1String("Value"))
            {
                ret.push_back(xml.readElementText());//errors on unexpected element
                if (xml.hasError()) throw DataFileException(filename,
                                                            "XML parsing error in BorderMetaDataValues: " + xml.errorString());
            } else {
                throw DataFileException(filename,
                                        "unexpected element in BorderMetaDataValues: " + name.toString());
            }
        }
    }
    if (xml.hasError()) throw DataFileException(filename,
                                                "XML parsing error in BorderMetaDataValues: " + xml.errorString());
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("BorderMetaDataValues"));
    return ret;
}

/**
 * @return Is this border file modified?
 */
bool 
BorderFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    if (m_metadata->isModified()) {
        return true;
    }
    if (m_classColorTable->isModified()) {
        return true;
    }
    if (m_nameColorTable->isModified()) {
        return true;
    }
    
    /* 
     * Note, these members do not affect modification status:
     * classNameHierarchy 
     */
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        if (m_borders[i]->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modification status of this border file.
 */
void 
BorderFile::clearModified()
{
    CaretDataFile::clearModified();
    m_metadata->clearModified();
    
    m_classColorTable->clearModified();
    m_nameColorTable->clearModified();
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        m_borders[i]->clearModified();
    }
}


/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
BorderFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    const std::vector<StructureEnum::Enum> allStructures = getAllBorderStructures();
    if (allStructures.size() >= 2) {
        AString structuresText;
        for (std::vector<StructureEnum::Enum>::const_iterator iter = allStructures.begin();
             iter != allStructures.end();
             iter++) {
            structuresText += (StructureEnum::toGuiName(*iter) + " ");
        }
        dataFileInformation.addNameAndValue("Border Structures", structuresText);
    }
    
    int nameSize = 4, classSize = 0;//reserve space for headings, but classSize is only to know how much to reserve()
    BorderMultiPartHelper myHelp(this);
    for (int i = 0; i < (int)myHelp.borderPieceList.size(); ++i)
    {
        CaretAssert(myHelp.borderPieceList[i].size() > 0);
        const Border* thisPart = getBorder(myHelp.borderPieceList[i][0]);
        nameSize = max(nameSize, (int)thisPart->getName().length());
        classSize = max(classSize, (int)thisPart->getClassName().length());
    }
    nameSize += 3;//minimum number spaces between fields
    int numberSize = max(8, (int)(AString::number(myHelp.borderPieceList.size()).length() + 3));//spacing for border index
    AString header = AString("INDEX").leftJustified(numberSize) + AString("NAME").leftJustified(nameSize) + "CLASS";
    dataFileInformation.addText(header);
    for (int i = 0; i < (int)myHelp.borderPieceList.size(); ++i)
    {
        CaretAssert(myHelp.borderPieceList[i].size() > 0);
        const Border* thisPart = getBorder(myHelp.borderPieceList[i][0]);
        AString line;
        line.reserve(numberSize + nameSize + classSize + 1);
        line = "\n" + AString::number(i + 1).leftJustified(numberSize);
        line += thisPart->getName().leftJustified(nameSize);
        line += thisPart->getClassName();
        dataFileInformation.addText(line);
    }
}

/**
 * Export this border file to Caret5 formatted border color and 
 * border projection files.
 *
 * @param surfaceFiles
 *     Surface files for unprojection of borders.
 * @param outputCaret5FilesPrefix
 *     Prefix for Caret5 output files.
 */
void
BorderFile::exportToCaret5Format(const std::vector<SurfaceFile*>& surfaceFiles,
                                 const AString& outputCaret5FilesPrefix)
{
    AString errorMessage;
    
    if (getNumberOfBorders() <= 0) {
        errorMessage.appendWithNewLine("This border file contains zero borders.");
    }
    if (outputCaret5FilesPrefix.isEmpty()) {
        errorMessage.appendWithNewLine("Caret5 output file prefix is empty.");
    }
    
    if ( ! errorMessage.isEmpty()) {
        throw DataFileException(getFileName(),
                                errorMessage);
    }

    /*
     * In Caret7, each border contains a Structure attribute and a Caret7
     * border file may contain borders from more than one structure.  However,
     * Caret5 borders do not contain a structure attribute and so each 
     * Caret5 border file contains borders for one structure only.
     *
     * So, group borders by structure.
     *
     */
    std::map<StructureEnum::Enum, std::vector<Border*> > bordersPerStructuresMap;
    for (std::vector<Border*>::iterator borderIter = m_borders.begin();
         borderIter != m_borders.end();
         borderIter++) {
        Border* border = *borderIter;
        if (border->getNumberOfPoints() > 0) {
            const StructureEnum::Enum structure = border->getStructure();
            std::map<StructureEnum::Enum, std::vector<Border*> >::iterator iter = bordersPerStructuresMap.find(structure);
            if (iter != bordersPerStructuresMap.end()) {
                iter->second.push_back(border);
            }
            else {
                std::vector<Border*> borderVector;
                borderVector.push_back(border);
                bordersPerStructuresMap.insert(std::pair<StructureEnum::Enum, std::vector<Border*> >(structure,
                                                                                                         borderVector));
            }
        }
    }
    
    int32_t filesWrittenCount = 0;
    
    /*
     * Surface are needed for unprojecting to create border files.
     * This will track missing surface structure types.
     */
    std::set<StructureEnum::Enum> missingSurfaceStructures;
    
    /*
     * Place borders for each structure in separate files.
     */
    for (std::map<StructureEnum::Enum, std::vector<Border*> >::iterator iter = bordersPerStructuresMap.begin();
         iter != bordersPerStructuresMap.end();
         iter++) {
        const StructureEnum::Enum structure = iter->first;
        const std::vector<Border*>& borderVector = iter->second;
        const int32_t numberOfBorders = static_cast<int32_t>(borderVector.size());
        
        AString structureName = "unknown";
        if (structure == StructureEnum::CEREBELLUM) {
            structureName = "cerebellum";
        }
        else if (StructureEnum::isLeft(structure)) {
            structureName = "left";
        }
        else if (StructureEnum::isRight(structure)) {
            structureName = "right";
        }
        
        AString headerText;
        headerText.appendWithNewLine("BeginHeader");
        headerText.appendWithNewLine("comment exported from wb_view file " + getFileNameNoPath());
        headerText.appendWithNewLine("encoding ASCII");
        headerText.appendWithNewLine("structure "
                                     + structureName);
        headerText.appendWithNewLine("EndHeader");
        
        if (numberOfBorders > 0) {
            bool allBorderProjectionsValid = true;
            AString borderProjFileText;
            borderProjFileText.appendWithNewLine(headerText);
            borderProjFileText.appendWithNewLine(AString::number(numberOfBorders));
            
            bool allBordersValid = true;
            AString borderFileText;
            borderFileText.appendWithNewLine(headerText);
            borderFileText.appendWithNewLine(AString::number(numberOfBorders));
            
            for (int32_t iBorder = 0; iBorder < numberOfBorders; iBorder++) {                
                const Border* border = borderVector[iBorder];
                const int32_t numPoints = border->getNumberOfPoints();
                const AString name = border->getName();
                
                /*
                 * Border Projection
                 * Write index, number of points, name,
                 * sampling density/variance/topography/uncertainty
                 *
                 * Center (0, 0, 0)
                 */
                borderProjFileText.appendWithNewLine(AString::number(iBorder)
                                                     + " "
                                                     + AString::number(numPoints)
                                                     + " "
                                                     + name
                                                     + " 20.0 1.0 0.0 1.0");
                borderProjFileText.appendWithNewLine("0.0 0.0 0.0");
                
                /*
                 * Border
                 * Write index, number of points, name,
                 * sampling density/variance/topography/uncertainty
                 *
                 * Center (0, 0, 0)
                 */
                borderFileText.appendWithNewLine(AString::number(iBorder)
                                                 + " "
                                                 + AString::number(numPoints)
                                                 + " "
                                                 + name
                                                 + " 20.0 1.0 0.0 1.0");
                borderFileText.appendWithNewLine("0.0 0.0 0.0");
                
                
                for (int32_t jPoint = 0; jPoint < numPoints; jPoint++) {
                    const SurfaceProjectedItem* spi = border->getPoint(jPoint);
                    
                    const SurfaceProjectionBarycentric* baryProj = spi->getBarycentricProjection();
                    if (baryProj->isValid()) {
                        const float* triangleAreas = baryProj->getTriangleAreas();
                        const int32_t* triangleNodes = baryProj->getTriangleNodes();
                        
                        /*
                         * Add points nodes, section, areas, and radius
                         */
                        borderProjFileText.appendWithNewLine(AString::number(triangleNodes[0])
                                                             + " "
                                                             + AString::number(triangleNodes[1])
                                                             + " "
                                                             + AString::number(triangleNodes[2])
                                                             + " 0 "
                                                             + AString::number(triangleAreas[0], 'f', 6)
                                                             + " "
                                                             + AString::number(triangleAreas[1], 'f', 6)
                                                             + " "
                                                             + AString::number(triangleAreas[2], 'f', 6)
                                                             + " 0.0");
                    }
                    else {
                        allBorderProjectionsValid = false;
                    }
                    
                    if ( ! surfaceFiles.empty()) {
                        SurfaceFile* surface = NULL;
                        for (std::vector<SurfaceFile*>::const_iterator surfaceIter = surfaceFiles.begin();
                             surfaceIter != surfaceFiles.end();
                             surfaceIter++) {
                            SurfaceFile* sf = *surfaceIter;
                            if (sf->getStructure() == structure) {
                                surface = sf;
                                break;
                            }
                        }
                        
                        if (surface != NULL) {
                            float xyz[3];
                            if (spi->getProjectedPosition(*surface, xyz, false)) {
                                /*
                                 * Add point index, section, xyz, and radius
                                 */
                                borderFileText.appendWithNewLine(AString::number(jPoint)
                                                                 + " 0 "
                                                                 + AString::number(xyz[0], 'f', 3)
                                                                 + " "
                                                                 + AString::number(xyz[1], 'f', 3)
                                                                 + " "
                                                                 + AString::number(xyz[2], 'f', 3)
                                                                 + " 0.0");
                            }
                            else {
                                allBordersValid = false;
                            }
                        }
                        else {
                            missingSurfaceStructures.insert(structure);
                            allBordersValid = false;
                        }
                    }
                    else {
                        missingSurfaceStructures.insert(structure);
                        allBordersValid = false;
                    }
                }
            }
            
            if (allBorderProjectionsValid) {
                try {
                    const AString filename = (outputCaret5FilesPrefix
                                        + "_"
                                        + StructureEnum::toName(structure)
                                        + ".borderproj");
                    TextFile borderProjectionFile;
                    borderProjectionFile.addText(borderProjFileText);
                    borderProjectionFile.writeFile(filename);
                    
                    filesWrittenCount++;
                }
                catch (const DataFileException& dfe) {
                    errorMessage.appendWithNewLine(dfe.whatString());
                }
            }
            else {
                errorMessage.appendWithNewLine("There were failures creating at least one border projection for structure: "
                                               + StructureEnum::toName(structure));
            }
            
            if (allBordersValid) {
                try {
                    const AString filename = (outputCaret5FilesPrefix
                                        + "_"
                                        + StructureEnum::toName(structure)
                                        + ".border");
                    TextFile borderFile;
                    borderFile.addText(borderFileText);
                    borderFile.writeFile(filename);
                    
                    filesWrittenCount++;
                }
                catch (const DataFileException& dfe) {
                    errorMessage.appendWithNewLine(dfe.whatString());
                }
            }
            else {
                errorMessage.appendWithNewLine("There were failures creating at least one border for structure: "
                                               + StructureEnum::toName(structure));
            }
        }
    }

    for (std::set<StructureEnum::Enum>::iterator missingStructureIter = missingSurfaceStructures.begin();
         missingStructureIter != missingSurfaceStructures.end();
         missingStructureIter++) {
        errorMessage.appendWithNewLine("No surface was available for structure: "
                                       + StructureEnum::toName(*missingStructureIter));
    }
    
    if (filesWrittenCount > 0) {
        try {
            const AString filename = (outputCaret5FilesPrefix
                                      + ".bordercolor");
            
            GiftiLabelTable* colorTable = getNameColorTable();
            colorTable->exportToCaret5ColorFile(filename);
        }
        catch (const GiftiException& ge) {
            errorMessage.appendWithNewLine(ge.whatString());
        }
    }
    
    if ( ! errorMessage.isEmpty()) {
        throw DataFileException(getFileName(),
                                errorMessage);
    }
    
}

BorderMultiPartHelper::BorderMultiPartHelper(const BorderFile* bf)
{
    int numBorderParts = bf->getNumberOfBorders();
    for (int i = 0; i < numBorderParts; ++i)
    {
        const Border* thisPart = bf->getBorder(i);
        map<pair<AString, AString>, int>::const_iterator iter = stringLookup.find(make_pair(thisPart->getName(), thisPart->getClassName()));
        if (iter == stringLookup.end())
        {
            stringLookup.insert(make_pair(make_pair(thisPart->getName(), thisPart->getClassName()), (int)borderPieceList.size()));
            borderPieceList.push_back(vector<int>(1, i));
        } else {
            borderPieceList[iter->second].push_back(i);
        }
    }
}

int BorderMultiPartHelper::fromNumberOrName(const AString& ident) const
{
    bool ok = false;
    int whichBorder = ident.toInt(&ok) - 1;//first border is "1"
    if (ok)
    {
        if (whichBorder < 0 || whichBorder >= (int)borderPieceList.size()) return -1;
        return whichBorder;
    } else {//only search for name if the string isn't a number, to prevent surprises
        for (std::map<std::pair<AString, AString>, int>::const_iterator iter = stringLookup.begin(); iter != stringLookup.end(); ++iter)
        {
            if (iter->first.first == ident)
            {
                return iter->second;
            }
        }
        return -1;
    }
}   

/**
 * Called when a group and name hierarchy item has attribute/status changed
 */
void
BorderFile::groupAndNameHierarchyItemStatusChanged()
{
    
}

/**
 * @return Names (unique) of all borders
 */
std::vector<AString>
BorderFile::getAllBorderNames() const
{
    std::set<AString> names;
    for (const Border* border : m_borders) {
        if ( ! border->getName().isEmpty()) {
            names.insert(border->getName());
        }
    }
    
    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * Names (unique) of all classes
 */
std::vector<AString>
BorderFile::getAllBorderClasses() const
{
    std::set<AString> names;
    for (const Border* border : m_borders) {
        if ( ! border->getClassName().isEmpty()) {
            names.insert(border->getClassName());
        }
    }
    
    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * @return Names of all borders that use the given class name
 * @param className
 *    Name of class
 */
std::vector<AString>
BorderFile::getAllBorderNamesThatUseClass(const AString& className)
{
    std::set<AString> names;
    for (const Border* border : m_borders) {
        if (border->getClassName() == className) {
            names.insert(border->getName());
        }
    }
    
    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * @return Classes for all borders with the given name (usually just one)
 * @param borderName
 *    Name of the border
 */
std::vector<AString>
BorderFile::getAllClassesForBordersWithName(const AString& borderName) const
{
    std::set<AString> names;
    for (const Border* border : m_borders) {
        if (borderName == border->getName()) {
            if ( ! border->getClassName().isEmpty()) {
                names.insert(border->getClassName());
            }
        }
    }

    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * Export the content of a border file to a DataFileEditorModel
 * @return The DataFileEditorModel containing border data.
 * Caller takes ownership of returned model.
 */
FunctionResultValue<DataFileEditorModel*>
BorderFile::exportToDataFileEditorModel() const
{
    const int32_t numBorders(getNumberOfBorders());
    if (numBorders <= 0) {
        return FunctionResultValue<DataFileEditorModel*>(NULL,
                                                         ("There are no borders to export from "
                                                          + getFileNameNoPath()),
                                                         false);
    }
    
    DataFileEditorModel* dataFileEditorModel(new DataFileEditorModel());
    dataFileEditorModel->setColumnCount(3);
    const int32_t classColumnIndex(1);
    dataFileEditorModel->setDefaultSortingColumnIndex(classColumnIndex);

    /*
     * Titles for columns
     */
    QList<QString> columnTitles;
    columnTitles << "Name" << "Class" << "XYZ";
    dataFileEditorModel->setHorizontalHeaderLabels(columnTitles);
    
    const GiftiLabelTable* classColorTable(getClassColorTable());
    const GiftiLabelTable* nameColorTable(getNameColorTable());
    
    for (int32_t i = 0; i < numBorders; i++) {
        const Border* border(getBorder(i));
        
        /*
         * For Border Name
         */
        float nameRGBA[4] { 0.0, 0.0, 0.0, 0.0 };
        const GiftiLabel* nameLabel(nameColorTable->getLabelBestMatching(border->getName()));
        if (nameLabel != NULL) {
            nameLabel->getColor(nameRGBA);
        }
        
        /*
         * For Border Class
         */
        float classRGBA[4] { 0.0, 0.0, 0.0, 0.0 };
        const GiftiLabel* classLabel(classColorTable->getLabelBestMatching(border->getClassName()));
        if (classLabel != NULL) {
            classLabel->getColor(classRGBA);
        }
        
        /*
         * For Border XYZ
         */
        AString xyzText;
        const int32_t numPoints(border->getNumberOfPoints());
        for (int32_t i = 0; i < numPoints; i++) {
            const SurfaceProjectedItem* spi = border->getPoint(i);
            CaretAssert(spi);
            Vector3D xyz;
            spi->getStereotaxicXYZ(xyz);
            xyzText = xyz.toString();
            break;
        }
        
        /*
         * All items in row represent the same focus
         */
        std::shared_ptr<Border> borderShared(new Border(*border));
        
        /*
         * Set colors for focus so that they are available
         * when and if the focus is copied
         */
        borderShared->setNameRgba(nameRGBA);
        borderShared->setClassRgba(classRGBA);
        
        /*
         * Create a row and add it to model
         */
        QList<QStandardItem*> rowItems;
        rowItems.push_back(new DataFileEditorItem(DataFileEditorItem::ItemType::NAME,
                                                  borderShared,
                                                  border->getName(),
                                                  nameRGBA));
        rowItems.push_back(new DataFileEditorItem(DataFileEditorItem::ItemType::CLASS,
                                                  borderShared,
                                                  border->getClassName(),
                                                  classRGBA));
        
        float xyzRGBA[4] { 0.0, 0.0, 0.0, 0.0 };
        rowItems.push_back(new DataFileEditorItem(DataFileEditorItem::ItemType::XYZ,
                                                  borderShared,
                                                  xyzText,
                                                  xyzRGBA));
        dataFileEditorModel->appendRow(rowItems);
    }
    
    return  FunctionResultValue<DataFileEditorModel*>(dataFileEditorModel,
                                                      "",
                                                      true);
}

/**
 * Import border data from the given DataFileEditorModel
 * Replaces content of this instance.
 * @param dataFileEditorModel
 *    Model that contains border data
 * @return
 *    Function result indicating success or failure
 */
FunctionResult
BorderFile::importFromDataFileEditorModel(const DataFileEditorModel& dataFileEditorModel)
{
    AString errorMessage;
    std::vector<const Border*> newBorders;
    
    const int32_t numRows(dataFileEditorModel.rowCount());
    for (int32_t iRow = 0; iRow < numRows; iRow++) {
        const int32_t column(0);
        const DataFileEditorItem* item(dataFileEditorModel.getDataFileItemAtRowColumn(iRow, column));
        if (item != NULL) {
            const Border* border(item->getBorder());
            if (border != NULL) {
                newBorders.push_back(border);
            }
            else {
                errorMessage.appendWithNewLine("PROGRAM ERROR: Border missing at row=" + AString::number(iRow));
            }
        }
        else {
            errorMessage.appendWithNewLine("PROGRAM ERROR: Invalid item at row=" + AString::number(iRow));
        }
    }
    
    if ( ! errorMessage.isEmpty()) {
        return FunctionResult::error(errorMessage);
    }
    
    /*
     * Remove all border
     */
    clearBorders();
    
    /*
     * Add border from data file editor model
     */
    for (const Border* border : newBorders) {
        addBorder(new Border(*border));
    }
    
    return FunctionResult::ok();
}

