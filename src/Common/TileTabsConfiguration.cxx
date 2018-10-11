
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
#include <set>

#define __TILE_TABS_CONFIGURATION_DECLARE__
#include "TileTabsConfiguration.h"
#undef __TILE_TABS_CONFIGURATION_DECLARE__

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

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
    if (this == &obj) {
        return;
    }
    
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
 * @param configurationMode
 *      The tile tabs configuration mode
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
                                                                 const TileTabsConfigurationModeEnum::Enum configurationMode,
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
    
    switch (configurationMode) {
        case TileTabsConfigurationModeEnum::AUTOMATIC:
        {
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
        }            break;
        case TileTabsConfigurationModeEnum::CUSTOM:
        {
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
        }            break;
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
 * Get the number of rows and columns for an automatic layout with the
 * given number of tabs.
 * @param numberOfTabs
 *     Number of tabs.
 * @param numberOfRowsOut
 *     Output with number of rows.
 * @param numberOfColumnsOut
 *     Output with number of columns.
 */
void
TileTabsConfiguration::getRowsAndColumnsForNumberOfTabs(const int32_t numberOfTabs,
                                                        int32_t& numberOfRowsOut,
                                                        int32_t& numberOfColumnsOut)
{
    CaretAssert(numberOfTabs >= 0);
    
    numberOfRowsOut = (int)std::sqrt((double)numberOfTabs);
    numberOfColumnsOut = numberOfRowsOut;
    int32_t row2 = numberOfRowsOut * numberOfRowsOut;
    if (row2 < numberOfTabs) {
        numberOfColumnsOut++;
    }
    if ((numberOfRowsOut * numberOfColumnsOut) < numberOfTabs) {
        numberOfRowsOut++;
    }
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
    int32_t numRows(0), numCols(0);
    getRowsAndColumnsForNumberOfTabs(numberOfTabs,
                                     numRows,
                                     numCols);
    
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
    return encodeInXMLWithStreamWriterVersionOne();
}

/**
 * @return Encoded tile tabs configuration in XML with Stream Writer
 */
AString
TileTabsConfiguration::encodeInXMLWithStreamWriterVersionOne() const
{
    AString xmlString;
    QXmlStreamWriter writer(&xmlString);
    writer.setAutoFormatting(true);
    
    writer.writeStartElement(s_rootTagName);
    
    writer.writeStartElement(s_versionTagName);
    writer.writeAttribute(s_versionNumberAttributeName, "1");
    writer.writeEndElement();
    
    writer.writeTextElement(s_nameTagName, m_name);
    writer.writeTextElement(s_uniqueIdentifierTagName, m_uniqueIdentifier);
    
    writer.writeStartElement(s_rowStretchFactorsTagName);
    writer.writeAttribute(s_rowStretchFactorsSelectedCountAttributeName, AString::number(m_numberOfRows));
    writer.writeAttribute(s_rowStretchFactorsTotalCountAttributeName, AString::number(m_rowStretchFactors.size()));
    writer.writeCharacters(AString::fromNumbers(m_rowStretchFactors, " "));
    writer.writeEndElement();
    
    writer.writeStartElement(s_columnStretchFactorsTagName);
    writer.writeAttribute(s_columnStretchFactorsSelectedCountAttributeName, AString::number(m_numberOfColumns));
    writer.writeAttribute(s_columnStretchFactorsTotalCountAttributeName, AString::number(m_columnStretchFactors.size()));
    writer.writeCharacters(AString::fromNumbers(m_columnStretchFactors, " "));
    writer.writeEndElement();
    
    writer.writeEndElement();
    
    return xmlString;
}

/**
 * @return Encoded tile tabs configuration in XML with Stream Writer
 */
AString
TileTabsConfiguration::encodeInXMLWithStreamWriterVersionTwo() const
{
    AString xmlString;
    QXmlStreamWriter writer(&xmlString);
    writer.setAutoFormatting(true);
    
    writer.writeStartElement(s_rootTagName);
    writer.writeAttribute(s_v2_versionAttributeName, "2");
    
    writer.writeTextElement(s_nameTagName, m_name);
    writer.writeTextElement(s_uniqueIdentifierTagName, m_uniqueIdentifier);
    
    writer.writeStartElement(s_rowStretchFactorsTagName);
    writer.writeAttribute(s_rowStretchFactorsSelectedCountAttributeName, AString::number(m_numberOfRows));
    writer.writeAttribute(s_rowStretchFactorsTotalCountAttributeName, AString::number(m_rowStretchFactors.size()));
    writer.writeCharacters(AString::fromNumbers(m_rowStretchFactors, " "));
    writer.writeEndElement();
    
    writer.writeStartElement(s_columnStretchFactorsTagName);
    writer.writeAttribute(s_columnStretchFactorsSelectedCountAttributeName, AString::number(m_numberOfColumns));
    writer.writeAttribute(s_columnStretchFactorsTotalCountAttributeName, AString::number(m_columnStretchFactors.size()));
    writer.writeCharacters(AString::fromNumbers(m_columnStretchFactors, " "));
    writer.writeEndElement();
    
    writer.writeEndElement();
    
    return xmlString;
}

/**
 * Decode the tile tabs configuration from XML with stream reader.
 *
 * @param xmlString
 *   String containing XML.
 * @param errorMessageOut
 *   Will contain error information.
 * @return
 *   True if decoding is successful, else false.
 */
bool
TileTabsConfiguration::decodeFromXMLWithStreamReader(const AString& xmlString,
                                                     AString& errorMessageOut)
{
    QXmlStreamReader xml(xmlString);
    
    if (xml.readNextStartElement()) {
        const QStringRef tagName(xml.name());
        if (tagName == s_rootTagName) {
            QString versionNumberText("Unknown");
            const QXmlStreamAttributes atts = xml.attributes();
            if (atts.hasAttribute(s_v2_versionAttributeName)) {
                versionNumberText = atts.value(s_v2_versionAttributeName).toString();
            }
            else {
                /*
                 * Version attribute was added to root element by version 2.
                 * So, if not present, must be version 1.
                 * Version 1 contained a version XML element.
                 */
                versionNumberText = "1";
            }
            
            if (versionNumberText == "1") {
                decodeFromXMLWithStreamReaderVersionOne(xml);
            }
            else if (versionNumberText == "2") {
                decodeFromXMLWithStreamReaderVersionTwo(xml);
            }
            else {
                xml.raiseError("TileTabsConfiguration invalid version="
                               + versionNumberText);
            }
        }
        else {
            xml.raiseError("TileTabsConfiguration first element is "
                           + xml.name().toString()
                           + " but should be "
                           + s_rootTagName);
        }
    }
    else {
        xml.raiseError("TileTabsConfiguration failed to find start elemnent.");
    }
    
    if (xml.hasError()) {
        errorMessageOut = ("Tile Tabs Configuration Read Error at line number="
                           + AString::number(xml.lineNumber())
                           + " column number="
                           + AString::number(xml.columnNumber())
                           + " description="
                           + xml.errorString());
        return false;
    }
    
    return true;
}

/**
 * Decode Version One of the tile tabs configuration from XML with stream reader.
 *
 * @param xml
 *   Stream XML parser.
 */
void
TileTabsConfiguration::decodeFromXMLWithStreamReaderVersionOne(QXmlStreamReader& xml)
{
    std::set<QString> invalidElements;
    
    AString name;
    AString uniqueID;
    std::vector<float> rowStretchFactors;
    std::vector<float> columnStretchFactors;
    int32_t numberOfRows(0);
    int32_t numberOfColumns(0);
    
    QString message;
    
    while ( ! xml.atEnd()) {
        xml.readNext();
        
        if (xml.isStartElement()) {
            const QStringRef tagName(xml.name());

            if (tagName == s_versionTagName) {
                /* ignore */
            }
            else if (tagName == s_nameTagName) {
                name = xml.readElementText();
            }
            else if (tagName == s_uniqueIdentifierTagName) {
                uniqueID = xml.readElementText();
            }
            else if (tagName == s_rowStretchFactorsTagName) {
                QXmlStreamAttributes atts = xml.attributes();
                if (atts.hasAttribute(s_rowStretchFactorsSelectedCountAttributeName)) {
                    numberOfRows = atts.value(s_rowStretchFactorsSelectedCountAttributeName).toInt();
                }
                
                AString::toNumbers(xml.readElementText(), rowStretchFactors);
            }
            else if (tagName == s_columnStretchFactorsTagName) {
                QXmlStreamAttributes atts = xml.attributes();
                if (atts.hasAttribute(s_columnStretchFactorsSelectedCountAttributeName)) {
                    numberOfColumns = atts.value(s_columnStretchFactorsSelectedCountAttributeName).toInt();
                }
                AString::toNumbers(xml.readElementText(), columnStretchFactors);
            }
            else {
                invalidElements.insert(tagName.toString());
                xml.skipCurrentElement();
            }
        }
    }
    
    if (name.isEmpty()) {
        message.append(s_nameTagName
                       + " not found or invalid.  ");
    }
    if (uniqueID.isEmpty()) {
        uniqueID = SystemUtilities::createUniqueID();
    }
    if (rowStretchFactors.empty()) {
        message.append(s_rowStretchFactorsTagName
                       + " not found or invalid.  ");
    }
    if (columnStretchFactors.empty()) {
        message.append(s_columnStretchFactorsTagName
                       + " not found or invalid.  ");
    }
    if (numberOfRows <= 0) {
        message.append(s_rowStretchFactorsTagName
                       + " attribute "
                       + s_rowStretchFactorsSelectedCountAttributeName
                       + " is missing or invalid."  );
    }
    if (numberOfRows <= 0) {
        message.append(s_columnStretchFactorsTagName
                       + " attribute "
                       + s_columnStretchFactorsSelectedCountAttributeName
                       + " is missing or invalid."  );
    }
    
    if ( ! invalidElements.empty()) {
        /*
         * If invalid elements were encountered, don't throw
         */
        AString msg("Invalid element(s) ignored: ");
        for (const auto s : invalidElements) {
            msg.append(s + " ");
        }
        CaretLogWarning(msg);
    }
    
    if (message.isEmpty()) {
        m_name = name;
        m_uniqueIdentifier = uniqueID;
        
        setRowStretchFactors(rowStretchFactors,
                             numberOfRows);
        setColumnStretchFactors(columnStretchFactors,
                                numberOfColumns);
    }
    else {
        xml.raiseError(message);
    }
}

/**
 * Set the number of rows.
 *
 * @param stretchFactors
 *     The stretch factors.
 * @param numberOfRows
 *     Number of rows for the configuration.
 */
void
TileTabsConfiguration::setRowStretchFactors(const std::vector<float>& stretchFactors,
                                            const int32_t numberOfRows)
{
    setNumberOfRows(numberOfRows);
    
    const int32_t maxRowStretchFactors = std::min(static_cast<int32_t>(stretchFactors.size()),
                                                  static_cast<int32_t>(m_rowStretchFactors.size()));
    for (int32_t i = 0; i < maxRowStretchFactors; i++) {
        m_rowStretchFactors[i] = stretchFactors[i];
    }
}

/**
 * Set the number of columns.
 *
 * @param stretchFactors
 *     The stretch factors.
 * @param numberOfColumns
 *     Number of columns for the configuration.
 */
void
TileTabsConfiguration::setColumnStretchFactors(const std::vector<float>& stretchFactors,
                                               const int32_t numberOfColumns)
{
    setNumberOfColumns(numberOfColumns);
    
    const int32_t maxColumnStretchFactors = std::min(static_cast<int32_t>(stretchFactors.size()),
                                                  static_cast<int32_t>(m_columnStretchFactors.size()));
    for (int32_t i = 0; i < maxColumnStretchFactors; i++) {
        m_columnStretchFactors[i] = stretchFactors[i];
    }
}

/**
 * Decode Version Two of the tile tabs configuration from XML with stream reader.
 *
 * @param xml
 *   Stream XML parser.
 */
void
TileTabsConfiguration::decodeFromXMLWithStreamReaderVersionTwo(QXmlStreamReader& /*xml*/)
{
}

/**
 * Decode the tile tabs configuration from XML using DOM
 *
 * @param xmlString
 *   String containing XML.
 * @param errorMessageOut
 *   Contains error information if decoding fails.
 * @return
 *   True if configuration was successfully read from the XML, else false.
 */
bool
TileTabsConfiguration::decodeFromXML(const AString& xmlString,
                                     AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    return decodeFromXMLWithStreamReader(xmlString,
                                         errorMessageOut);
}

/**
 * @return String version of an instance.
 */
AString
TileTabsConfiguration::toString() const
{
    AString s("Name: %1, Unique ID: %2, Number of Rows: %3, Number of Columns: %4, "
              "Row Stretch Factors: %5, Columns Stretch Factors: %6");
    
    const AString rs(AString::fromNumbers(m_rowStretchFactors, ", "));
    const AString cs(AString::fromNumbers(m_columnStretchFactors, ", "));
    
    s = s.arg(m_name).arg(m_uniqueIdentifier).arg(m_numberOfRows).arg(m_numberOfColumns).arg(rs).arg(cs);
    
    return s;
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

