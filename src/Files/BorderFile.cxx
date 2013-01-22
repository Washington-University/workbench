
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

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
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GroupAndNameHierarchyModel.h"
#include "FileAdapter.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "SurfaceFile.h"
#include "SurfaceProjectedItem.h"
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
 * Find the border nearest the given coordinate within
 * the given tolerance.
 *
 * @param displayGroup
 *    Display group in which border is tested for display.
 * @param browserTabIndex
 *    Tab index in which border is displayed.
 * @param surfaceFile
 *    Surface file used for unprojection of border points.
 * @param xyz
 *    Coordinate for nearest border.
 * @param maximumDistance
 *    Maximum distance coordinate can be from a border point.
 * @param borderOut
 *    Border containing the point nearest the coordinate.
 * @param borderIndexOut
 *    Index of border in the border file containing the point nearest the coordinate.
 * @param borderPointIndexOut
 *    Index of border point nearest the coordinate, in the border.
 * @param borderPointOut
 *    Point in border nearest the coordinate.
 * @param distanceToNearestPointOut
 *    Distance to the nearest border point.
 * @return
 *    Returns true if a border point was found that was within
 *    tolerance distance of the coordinate in which case ALL of
 *    the output parameters will be valid.  Otherwise, false
 *    will be returned.
 */
bool 
BorderFile::findBorderNearestXYZ(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t browserTabIndex,
                                 const SurfaceFile* surfaceFile,
                                const float xyz[3],
                                const float maximumDistance,
                                Border*& borderOut,
                                int32_t& borderIndexOut,
                                SurfaceProjectedItem*& borderPointOut,
                                int32_t& borderPointIndexOut,
                                float& distanceToNearestPointOut) const
{
    CaretAssert(surfaceFile);
    
    borderOut = NULL;
    borderIndexOut = -1;
    borderPointOut = NULL;
    borderPointIndexOut = -1;
    distanceToNearestPointOut = std::numeric_limits<float>::max();
    
    float nearestDistance = std::numeric_limits<float>::max();
    
    BorderFile* nonConstBorderFile = const_cast<BorderFile*>(this);
    
    const int32_t numBorders = getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        Border* border = m_borders[i];
        if (nonConstBorderFile->isBorderDisplayed(displayGroup,
                                                  browserTabIndex,
                                                  border) == false) {
            continue;
        }
        float distanceToPoint = 0.0;
        const int32_t pointIndex = border->findPointIndexNearestXYZ(surfaceFile, 
                                                              xyz,
                                                              maximumDistance,
                                                              distanceToPoint);
        if (pointIndex >= 0) {
            SurfaceProjectedItem* borderPoint = border->getPoint(pointIndex);
            if (distanceToPoint < nearestDistance) {
                borderOut = border;
                borderIndexOut = i;
                borderPointOut = borderPoint;
                borderPointIndexOut = pointIndex;
                distanceToNearestPointOut = distanceToPoint;
            }
        }
    }
    
    const bool valid = (borderOut != NULL);
    return valid;
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
        //m_foci[i]->setNameRgbaInvalid();
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
    checkFileReadability(filename);
    
    BorderFileSaxReader saxReader(this);
    std::auto_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
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

