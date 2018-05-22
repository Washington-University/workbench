
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

#include <cmath>

#define __TILE_TABS_CONFIGURATION_DECLARE__
#include "TileTabsConfiguration.h"
#undef __TILE_TABS_CONFIGURATION_DECLARE__

#include <QDomDocument>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SystemUtilities.h"

using namespace caret;


    
/**
 * \class caret::TileTabsConfiguration 
 * \brief Defines a tile tabs configuration
 * \ingroup Common
 */

/**
 * Constructor that creates a 2 by 2 configuration.
 */
TileTabsConfiguration::TileTabsConfiguration()
: CaretObject()
{
    initialize();
}

/**
 * Destructor.
 */
TileTabsConfiguration::~TileTabsConfiguration()
{
}

/**
 * Copy constructor.
 *
 * NOTE: Unique identifier remains the same ! See also: newCopyWithNewUniqueIdentifier()
 *
 * @param obj
 *    Object that is copied.
 */
TileTabsConfiguration::TileTabsConfiguration(const TileTabsConfiguration& obj)
: CaretObject(obj)
{
    initialize();
    this->copyHelperTileTabsConfiguration(obj);
}

/**
 * Assignment operator.
 *
 * NOTE: Unique identifier remains the same !  See also: newCopyWithNewUniqueIdentifier()
 *
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TileTabsConfiguration&
TileTabsConfiguration::operator=(const TileTabsConfiguration& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTileTabsConfiguration(obj);
    }
    return *this;    
}

/**
 * Copy this instance and give it a new unique identifier.
 * Note that copy constructor does not create a new unique identifier.
 *
 * @return The new Copy.
 */
TileTabsConfiguration*
TileTabsConfiguration::newCopyWithNewUniqueIdentifier() const
{
    TileTabsConfiguration* newCopy = new TileTabsConfiguration(*this);
    CaretAssert(newCopy);
    newCopy->m_uniqueIdentifier = SystemUtilities::createUniqueID();
    return newCopy;
}


/**
 * Initialize an instance of a tile tabs configuration.
 */
void
TileTabsConfiguration::initialize()
{
    m_rowStretchFactors.resize(getMaximumNumberOfRows(),
                               1.0);
    m_columnStretchFactors.resize(getMaximumNumberOfColumns(),
                                  1.0);
    m_numberOfColumns = 0;
    m_numberOfRows    = 0;
    
    setNumberOfRows(2);
    setNumberOfColumns(2);
    
    m_defaultConfigurationFlag = false;

    m_uniqueIdentifier = SystemUtilities::createUniqueID();
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsConfiguration::copyHelperTileTabsConfiguration(const TileTabsConfiguration& obj)
{
    m_numberOfColumns = obj.m_numberOfColumns;
    m_numberOfRows    = obj.m_numberOfRows;
    m_rowStretchFactors = obj.m_rowStretchFactors;
    m_columnStretchFactors = obj.m_columnStretchFactors;
    m_name = obj.m_name;
    //DO NOT CHANGE THE UNIQUE IDENTIFIER:  m_uniqueIdentifier
}

/**
 * Copies the tile tabs configuration rows, columns, and
 * stretch factors.   Name is NOT copied.
 */
void
TileTabsConfiguration::copy(const TileTabsConfiguration& rhs)
{
    AString savedName = m_name;
    copyHelperTileTabsConfiguration(rhs);
    m_name = savedName;
}

/**
 * Get the row heights and column widths for this tile tabs configuration using the
 * given window width and height.
 *
 * @param windowWidth
 *      Width of window.
 * @param windowHeight
 *      Height of window.
 * @param numberOfModelsToDraw
 *      Number of models to draw.
 * @param automaticConfigurationEnabled
 *      Automatic configuration is enabled
 * @param rowHeightsOut
 *      Output containing height of each row.
 * @param columnWidthsOut
 *      Output containing width of each column.
 * @return
 *      True if the ouput is valid, else false.
 */
bool
TileTabsConfiguration::getRowHeightsAndColumnWidthsForWindowSize(const int32_t windowWidth,
                                                                 const int32_t windowHeight,
                                                                 const int32_t numberOfModelsToDraw,
                                                                 const bool automaticConfigurationEnabled,
                                                                 std::vector<int32_t>& rowHeightsOut,
                                                                 std::vector<int32_t>& columnWidthsOut)
{
    /*
     * NOTE: When computing widths and heights, do not round.
     * Rounding may cause the bottom most row or column to extend
     * outside the graphics region.  Shrinking the last row or
     * column is not desired since it might cause the last model
     * to be drawn slightly smaller than the others.
     */
    
    int32_t numRows = 0;
    int32_t numCols = 0;
    
    rowHeightsOut.clear();
    columnWidthsOut.clear();
    
    if (automaticConfigurationEnabled) {
        /*
         * Update number of rows/columns in the default configuration
         * so that if a scene is saved, the correct number of rows
         * and columns are saved to the scene.
         */
        updateAutomaticConfigurationRowsAndColumns(numberOfModelsToDraw);
        numRows = getNumberOfRows();
        numCols = getNumberOfColumns();
        
        for (int32_t i = 0; i < numRows; i++) {
            rowHeightsOut.push_back(windowHeight / numRows);
        }
        for (int32_t i = 0; i < numCols; i++) {
            columnWidthsOut.push_back(windowWidth / numCols);
        }
    }
    else {
        /*
         * Rows/columns from user configuration
         */
        numRows = getNumberOfRows();
        numCols = getNumberOfColumns();
        
        /*
         * Determine height of each row
         */
        float rowStretchTotal = 0.0;
        for (int32_t i = 0; i < numRows; i++) {
            rowStretchTotal += getRowStretchFactor(i);
        }
        CaretAssert(rowStretchTotal > 0.0);
        for (int32_t i = 0; i < numRows; i++) {
            const int32_t h = static_cast<int32_t>((getRowStretchFactor(i) / rowStretchTotal)
                                                   * windowHeight);
            
            rowHeightsOut.push_back(h);
        }
        
        /*
         * Determine width of each column
         */
        float columnStretchTotal = 0.0;
        for (int32_t i = 0; i < numCols; i++) {
            columnStretchTotal += getColumnStretchFactor(i);
        }
        CaretAssert(columnStretchTotal > 0.0);
        for (int32_t i = 0; i < numCols; i++) {
            const int32_t w = static_cast<int32_t>((getColumnStretchFactor(i) / columnStretchTotal)
                                                   * windowWidth);
            columnWidthsOut.push_back(w);
        }
    }
    
    if ((numRows == static_cast<int32_t>(rowHeightsOut.size()))
        && (numCols == static_cast<int32_t>(columnWidthsOut.size()))) {
        /*
         * Verify all rows fit within the window
         */
        int32_t rowHeightsSum = 0;
        for (int32_t i = 0; i < numRows; i++) {
            rowHeightsSum += rowHeightsOut[i];
        }
        if (rowHeightsSum > windowHeight) {
            CaretLogSevere("PROGRAM ERROR: Tile Tabs total row heights exceed window height");
            rowHeightsOut[numRows - 1] -= (rowHeightsSum - windowHeight);
        }
        
        /*
         * Adjust width of last column so that it does not extend beyond viewport
         */
        int32_t columnWidthsSum = 0;
        for (int32_t i = 0; i < numCols; i++) {
            columnWidthsSum += columnWidthsOut[i];
        }
        if (columnWidthsSum > windowWidth) {
            CaretLogSevere("PROGRAM ERROR: Tile Tabs total row heights exceed window height");
            columnWidthsOut[numCols - 1] = columnWidthsSum - windowWidth;
        }
        
        CaretLogFiner("Tile Tabs Row Heights: "
                      + AString::fromNumbers(rowHeightsOut, ", "));
        CaretLogFiner("Tile Tabs Column Widths: "
                      + AString::fromNumbers(columnWidthsOut, ", "));
        return true;
    }
    
    const QString msg("Row and heights failed rows="
                      + AString::number(numRows)
                      + " rowHeights="
                      + AString::number(rowHeightsOut.size())
                      + " cols="
                      + AString::number(numCols)
                      + " rowHeights="
                      + AString::number(columnWidthsOut.size()));
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
    return false;
}


/**
 * @return the name of the tile tabs configuration.
 */
AString
TileTabsConfiguration::getName() const
{
    return m_name;
}

/**
 * @return Get the unique identifier that uniquely identifies each configuration.
 */
AString
TileTabsConfiguration::getUniqueIdentifier() const
{
    return m_uniqueIdentifier;
}

/**
 * Set the name of the tile tabs configuration.
 *
 * @param name
 *    New name for configuration.
 */
void
TileTabsConfiguration::setName(const AString& name)
{
    m_name = name;
}

/**
 * @return Number of rows.
 */
int32_t
TileTabsConfiguration::getNumberOfRows() const
{
    return m_numberOfRows;
}

/**
 * Set number of rows.
 * 
 * @param numberOfRows
 *     New number of rows.
 */
void
TileTabsConfiguration::setNumberOfRows(const int32_t numberOfRows)
{
    const int32_t oldNumerOfRows = m_numberOfRows;
    
    CaretAssert(numberOfRows >= 1);
    m_numberOfRows = numberOfRows;
    if (m_numberOfRows > getMaximumNumberOfRows()) {
        CaretLogSevere("Requested number of rows is "
                       + AString::number(m_numberOfRows)
                       + " but maximum is "
                       + getMaximumNumberOfRows());
        m_numberOfRows = getMaximumNumberOfRows();
    }
    
    /*
     * Stretch factors default to 1.0
     */
    for (int32_t iRow = oldNumerOfRows; iRow < m_numberOfRows; iRow++) {
        CaretAssertVectorIndex(m_rowStretchFactors, iRow);
        m_rowStretchFactors[iRow] = 1.0;
    }
}

/**
 * @return Number of columns.
 */
int32_t
TileTabsConfiguration::getNumberOfColumns() const
{
    return m_numberOfColumns;
}

/**
 * Set number of rows.
 *
 * @param numberOfColumns
 *     New number of rows.
 */
void
TileTabsConfiguration::setNumberOfColumns(const int32_t numberOfColumns)
{
    const int32_t oldNumberOfColumns = m_numberOfColumns;
    
    CaretAssert(numberOfColumns >= 1);
    
    m_numberOfColumns = numberOfColumns;
    if (m_numberOfColumns > getMaximumNumberOfColumns()) {
        CaretLogSevere("Requested number of columns is "
                       + AString::number(m_numberOfColumns)
                       + " but maximum is "
                       + getMaximumNumberOfColumns());
        m_numberOfColumns = getMaximumNumberOfColumns();
    }
    
    /*
     * Stretch factors default to 1.0
     */
    for (int32_t iCol = oldNumberOfColumns; iCol < m_numberOfColumns; iCol++) {
        CaretAssertVectorIndex(m_columnStretchFactors, iCol);
        m_columnStretchFactors[iCol] = 1.0;
    }
}

/**
 * Get stretch factor for a column.
 *
 * @param columnIndex
 *    Index of the column.
 * @return
 *    Stretch factor for the column.
 */
float
TileTabsConfiguration::getColumnStretchFactor(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columnStretchFactors, columnIndex);
    
    return m_columnStretchFactors[columnIndex];
}

/**
 * Set stretch factor for a column.
 *
 * @param columnIndex
 *    Index of the column.
 * @param stretchFactor
 *    Stretch factor for the column.
 */
void
TileTabsConfiguration::setColumnStretchFactor(const int32_t columnIndex,
                            const float stretchFactor)
{
    CaretAssertVectorIndex(m_columnStretchFactors, columnIndex);
    
    m_columnStretchFactors[columnIndex] = stretchFactor;
}

/**
 * Get stretch factor for a column.
 *
 * @param columnIndex
 *    Index of the column.
 * @return
 *    Stretch factor for the column.
 */
float
TileTabsConfiguration::getRowStretchFactor(const int32_t rowIndex) const
{
    CaretAssertVectorIndex(m_rowStretchFactors, rowIndex);
    
    return m_rowStretchFactors[rowIndex];
}

/**
 * Set stretch factor for a column.
 *
 * @param rowIndex
 *    Index of the row.
 * @param stretchFactor
 *    Stretch factor for the column.
 */
void
TileTabsConfiguration::setRowStretchFactor(const int32_t rowIndex,
                         const float stretchFactor)
{
    CaretAssertVectorIndex(m_rowStretchFactors, rowIndex);
    
    m_rowStretchFactors[rowIndex] = stretchFactor;
    
}

/**
 * @return Is this the default configuration?  Each browser window
 * always has ONE default configuration which displays all tabs.
 */
bool
TileTabsConfiguration::isDefaultConfiguration() const
{
    return m_defaultConfigurationFlag;
}

/**
 * Set the default configuration status.  This should only be called by
 * the browser window which contains one tile tabs configuration that is
 * used by the browser window for display of all tabs.
 */
void
TileTabsConfiguration::setDefaultConfiguration(const bool defaultConfiguration)
{
    m_defaultConfigurationFlag = defaultConfiguration;
}

/**
 * Updates the number of rows and columns for the automatic configuration
 * based upon the number of tabs.  
 *
 * Since screen width typically exceeds height, ensure the number of 
 * columns is always greater than the number of rows.
 */
void
TileTabsConfiguration::updateAutomaticConfigurationRowsAndColumns(const int32_t numberOfTabs)
{
    int32_t numRows = (int)std::sqrt((double)numberOfTabs);
    int32_t numCols = numRows;
    int32_t row2 = numRows * numRows;
    if (row2 < numberOfTabs) {
        numCols++;
    }
    if ((numRows * numCols) < numberOfTabs) {
        numRows++;
    }
    
    setNumberOfRows(numRows);
    setNumberOfColumns(numCols);
    
    std::fill(m_columnStretchFactors.begin(),
              m_columnStretchFactors.end(),
              1.0);
    std::fill(m_rowStretchFactors.begin(),
              m_rowStretchFactors.end(),
              1.0);
}


/**
 * @return Encoded tile tabs configuration in XML
 */
AString
TileTabsConfiguration::encodeInXML() const
{
    QDomDocument doc(s_rootTagName);
    QDomElement root = doc.createElement(s_rootTagName);
    doc.appendChild(root);
    
    QDomElement versionTag = doc.createElement(s_versionTagName);
    versionTag.setAttribute(s_versionNumberAttributeName,
                            (int)1);
    root.appendChild(versionTag);
    
    QDomElement nameTag = doc.createElement(s_nameTagName);
    nameTag.appendChild(doc.createTextNode(m_name));
    root.appendChild(nameTag);
    
    QDomElement uniqueIdentifierTag = doc.createElement(s_uniqueIdentifierTagName);
    uniqueIdentifierTag.appendChild(doc.createTextNode(m_uniqueIdentifier));
    root.appendChild(uniqueIdentifierTag);
    
    QDomElement rowStretchFactorsTag = doc.createElement(s_rowStretchFactorsTagName);
    rowStretchFactorsTag.setAttribute(s_rowStretchFactorsTotalCountAttributeName,
                                      static_cast<int>(m_rowStretchFactors.size()));
    rowStretchFactorsTag.setAttribute(s_rowStretchFactorsSelectedCountAttributeName,
                                      static_cast<int>(m_numberOfRows));
    rowStretchFactorsTag.appendChild(doc.createTextNode(AString::fromNumbers(m_rowStretchFactors,
                                                                             " ")));
    root.appendChild(rowStretchFactorsTag);

    QDomElement columnStretchFactorsTag = doc.createElement(s_columnStretchFactorsTagName);
    columnStretchFactorsTag.setAttribute(s_columnStretchFactorsTotalCountAttributeName,
                                         static_cast<int>(m_columnStretchFactors.size()));
    columnStretchFactorsTag.setAttribute(s_columnStretchFactorsSelectedCountAttributeName,
                                         static_cast<int>(m_numberOfColumns));
    columnStretchFactorsTag.appendChild(doc.createTextNode(AString::fromNumbers(m_columnStretchFactors,
                                                                                " ")));
    root.appendChild(columnStretchFactorsTag);
    
    const AString xmlString = doc.toString();
    return xmlString;
}

/**
 * Decode the tile tabs configuration from XML.
 *
 * @param xmlString
 *   String containing XML.
 * @return
 *   True if configuration was successfully read from the XML, else false.
 */
bool
TileTabsConfiguration::decodeFromXML(const AString& xmlString)
{
    m_defaultConfigurationFlag = false;
    setNumberOfRows(2);
    setNumberOfColumns(2);
    
    try {
        QDomDocument doc(s_rootTagName);
        if (doc.setContent(xmlString) == false) {
            throw CaretException("Error parsing DomDocument");
        }
        
        QDomNodeList nodeList = doc.elementsByTagName(s_versionTagName);
        if (nodeList.isEmpty()) {
            throw CaretException("Error finding version tag");
        }
        QDomElement versionElement = nodeList.at(0).toElement();
        if (versionElement.isNull()) {
            throw CaretException("Error finding version element");
        }
        const AString versionNumberString = versionElement.attribute(s_versionNumberAttributeName,
                                                                     "");
        if (versionNumberString.isEmpty()) {
            throw CaretException("Error finding version number attribute");
        }
        
        const int versionNumber = versionNumberString.toInt();
        if (versionNumber == 1) {
            parseVersionOneXML(doc);
        }
        else {
            throw CaretException("Invalid version number attribute "
                           + versionNumberString);
        }
    }
    catch (const CaretException& e) {
        CaretLogSevere("Error parsing tile tabs configuration XML:\n"
                       + e.whatString()
                       + "\n\n"
                       + xmlString);
        return false;
    }
    
    return true;
}

/**
 * Parse XML for Version One.
 *
 * @param doc
 *    XML DOM document.
 */
void
TileTabsConfiguration::parseVersionOneXML(QDomDocument& doc)
{
    QDomNodeList nameNodeList = doc.elementsByTagName(s_nameTagName);
    if (nameNodeList.isEmpty()) {
        throw CaretException("Error finding name tag");
    }
    QDomElement nameElement = nameNodeList.at(0).toElement();
    if (nameElement.isNull()) {
        throw CaretException("Error finding name element");
    }
    m_name = nameElement.text();
    
    QDomNodeList uniqueIdNodeList = doc.elementsByTagName(s_uniqueIdentifierTagName);
    if (uniqueIdNodeList.isEmpty()) {
        CaretLogWarning("Tile Tabs Configuration "
                        + m_name
                        + " is missing its unique identifier");
        m_uniqueIdentifier = SystemUtilities::createUniqueID();
    }
    else {
        QDomElement uniqueIdElement = uniqueIdNodeList.at(0).toElement();
        if (uniqueIdElement.isNull()) {
            throw CaretException("Error finding unique identifier element");
        }
        m_uniqueIdentifier = uniqueIdElement.text();
    }
    
    QDomNodeList rowNodeList = doc.elementsByTagName(s_rowStretchFactorsTagName);
    if (rowNodeList.isEmpty()) {
        throw CaretException("Error finding row stretch factors tag");
    }
    QDomElement rowElement = rowNodeList.at(0).toElement();
    if (rowElement.isNull()) {
        throw CaretException("Error finding row element");
    }
    
    const AString numberOfRowsString = rowElement.attribute(s_rowStretchFactorsSelectedCountAttributeName,
                                                            "");
    if (numberOfRowsString.isEmpty()) {
        throw CaretException("Error finding number of rows attribute");
    }
    const int32_t selectedNumberOfRows = numberOfRowsString.toInt();
    if (selectedNumberOfRows <= 0) {
        throw CaretException("Invalid number of rows attribute "
                             + numberOfRowsString);
    }
    
    const AString totalNumberOfRowsString = rowElement.attribute(s_rowStretchFactorsTotalCountAttributeName,
                                                                     "");
    int32_t totalNumberOfRows = 0;
    if (totalNumberOfRowsString.isEmpty()) {
        CaretLogWarning("Total number of rows attribute is missing.");
    }
    else {
        totalNumberOfRows = totalNumberOfRowsString.toInt();
    }
    
    const AString rowStretchFactorsText = rowElement.text();
    std::vector<float> rowStretchFactors;
    AString::toNumbers(rowStretchFactorsText,
                       rowStretchFactors);
    if (static_cast<int32_t>(rowStretchFactors.size()) != totalNumberOfRows) {
        throw CaretException("Stretch factor number of rows is "
                             + AString::number(totalNumberOfRows)
                             + " but have "
                             + AString::number(static_cast<int32_t>(rowStretchFactors.size()))
                             + " stretch factors.");
    }
    
    
    QDomNodeList columnNodeList = doc.elementsByTagName(s_columnStretchFactorsTagName);
    if (columnNodeList.isEmpty()) {
        throw CaretException("Error finding column stretch factors tag");
    }
    QDomElement columnElement = columnNodeList.at(0).toElement();
    if (columnElement.isNull()) {
        throw CaretException("Error finding column element");
    }
    
    const AString numberOfColumnsString = columnElement.attribute(s_columnStretchFactorsSelectedCountAttributeName,
                                                                  "");
    if (numberOfColumnsString.isEmpty()) {
        throw CaretException("Error finding number of columns attribute");
    }
    const int32_t selectedNumberOfColumns = numberOfColumnsString.toInt();
    if (selectedNumberOfColumns <= 0) {
        throw CaretException("Invalid number of columns attribute "
                             + numberOfColumnsString);
    }
    
    
    const AString totalNumberOfColumnsString = columnElement.attribute(s_columnStretchFactorsTotalCountAttributeName,
                                                               "");
    int32_t totalNumberOfColumns = 0;
    if (totalNumberOfColumnsString.isEmpty()) {
        CaretLogWarning("Total number of columns attribute is missing.");
    }
    else {
        totalNumberOfColumns = totalNumberOfColumnsString.toInt();
    }

    const AString columnStretchFactorsText = columnElement.text();
    std::vector<float> columnStretchFactors;
    AString::toNumbers(columnStretchFactorsText,
                       columnStretchFactors);
    if (static_cast<int32_t>(columnStretchFactors.size()) != totalNumberOfColumns) {
        throw CaretException("Stretch factor number of columns is "
                             + AString::number(totalNumberOfColumns)
                             + " but have "
                             + AString::number(static_cast<int32_t>(columnStretchFactors.size()))
                             + " stretch factors.");
    }
    
    setNumberOfRows(selectedNumberOfRows);
    setNumberOfColumns(selectedNumberOfColumns);

    const int32_t maxRowStretchFactors = std::min(totalNumberOfRows,
                                                  static_cast<int32_t>(m_rowStretchFactors.size()));
    for (int32_t i = 0; i < maxRowStretchFactors; i++) {
        m_rowStretchFactors[i] = rowStretchFactors[i];
    }
    const int32_t maxColumnStretchFactors = std::min(totalNumberOfColumns,
                                                  static_cast<int32_t>(m_columnStretchFactors.size()));
    for (int32_t i = 0; i < maxColumnStretchFactors; i++) {
        m_columnStretchFactors[i] = columnStretchFactors[i];
    }
}

/**
 * Compare two tile tabs configurations by name.
 *
 * @param ttc1
 *    First tile tab configuration.
 * @param ttc2
 *    Second tile tab configuration.
 * @return
 *    True if ttc1 is "less than" when compared by name, else false.
 */
bool
TileTabsConfiguration::lessThanComparisonByName(const TileTabsConfiguration* ttc1,
                                                const TileTabsConfiguration* ttc2)
{
    if (ttc1->getName() < ttc2->getName()) {
        return true;
    }
    return false;
}

