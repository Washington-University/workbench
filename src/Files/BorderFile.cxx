
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
#include <limits>
#include <memory>

#include <QFile>
#include <QStringList>
#include <QTextStream>

#define __BORDER_FILE_DECLARE__
#include "BorderFile.h"
#undef __BORDER_FILE_DECLARE__

#include "Border.h"
#include "BorderFileSaxReader.h"
#include "BorderPointFromSearch.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
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


    
/**
 * \class caret::BorderFile 
 * \brief File containing borders.
 */

/**
 * Constructor.
 */
BorderFile::BorderFile()
: CaretDataFile(DataFileTypeEnum::BORDER)
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
    m_classNameHierarchy = new GroupAndNameHierarchyModel();
    m_metadata = new GiftiMetaData();
    m_forceUpdateOfGroupAndNameHierarchy = true;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
BorderFile::BorderFile(const BorderFile& obj)
: CaretDataFile(obj)
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
    m_classNameHierarchy = new GroupAndNameHierarchyModel();
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
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
BorderFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    // does nothing since borders apply to all structures
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
    m_classNameHierarchy->clear();
    m_classColorTable->clear();
    m_nameColorTable->clear();
    m_metadata->clear();
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        delete m_borders[i];
    }
    m_borders.clear();
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
            float nearestPointDistance = -1.0;
            int32_t neartestPointIndex = -1;
            if (numPoints > 0) {
                float pointXYZ[3];
                if (border->getPoint(0)->getProjectedPosition(*surfaceFile,
                                                              pointXYZ,
                                                              false)) {
                    nearestPointDistance = MathFunctions::distanceSquared3D(pointXYZ,
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
                    if (nearestPointDistance >= 0.0) {
                        if (dist2 < nearestPointDistance) {
                            neartestPointIndex   = lastPointIndex;
                            nearestPointDistance = dist2;
                        }
                    }
                    else {
                        neartestPointIndex   = lastPointIndex;
                        nearestPointDistance = dist2;
                    }
                }
            }
            
            if (neartestPointIndex >= 0) {
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
                    BorderPointFromSearch bpo;
                    bpo.setData(const_cast<BorderFile*>(this),
                                border,
                                borderIndex,
                                nearestIndex1,
                                distance1);
                    borderPointsOut.push_back(bpo);
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
    m_borders.push_back(border);
    const AString name = border->getName();
    if (name.isEmpty() == false) {
        const int32_t nameColorKey = m_nameColorTable->getLabelKeyFromName(name);
        if (nameColorKey < 0) {
            m_nameColorTable->addLabel(name, 0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    const AString className = border->getClassName();
    if (className.isEmpty() == false) {
        const int32_t classColorKey = m_classColorTable->getLabelKeyFromName(className);
        if (classColorKey < 0) {
            m_classColorTable->addLabel(className, 0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
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

/**
 * Version 1 foci files contained one color table for both names
 * and classes.  Newer versions of the foci file keep them in
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
    
    const int32_t numFoci = getNumberOfBorders();
    for (int32_t i = 0;i < numFoci; i++) {
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
BorderFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();
    checkFileReadability(filename);
    
    BorderFileSaxReader saxReader(this);
    std::auto_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
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
    }
    
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
BorderFile::writeFile(const AString& filename) throw (DataFileException)
{
    checkFileWritability(filename);
    
    setFileName(filename);
    
    try {
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
            throw DataFileException(errorMessage);
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
            m_borders[i]->writeAsXML(xmlWriter);
        }
        
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        
        file.close();
        
        clearModified();
    }
    catch (const GiftiException& e) {
        throw DataFileException(e);
    }
    catch (const XmlException& e) {
        throw DataFileException(e);
    }
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
    
    QStringList fociNames = getAllBorderNamesSorted();
    const int32_t numNames = fociNames.size();
    if (numNames > 0) {
        AString namesListText = "BORDER NAMES";
        for (int32_t i = 0; i < numNames; i++) {
            namesListText.appendWithNewLine("    "
                                            + fociNames.at(i));
            
        }
        
        dataFileInformation.addText(namesListText);
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
                                 const AString& outputCaret5FilesPrefix) throw (DataFileException)
{
    AString errorMessage;
    
    if (getNumberOfBorders() <= 0) {
        errorMessage.appendWithNewLine("This border file "
                                       + getFileNameNoPath()
                                       + " contains zero borders.");
    }
    if (outputCaret5FilesPrefix.isEmpty()) {
        errorMessage.appendWithNewLine("Caret5 output file prefix is empty.");
    }
    
    if ( ! errorMessage.isEmpty()) {
        throw DataFileException(errorMessage);
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
        throw DataFileException(errorMessage);
    }
    
}



