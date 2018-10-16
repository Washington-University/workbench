
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
    const AString savedUniqueID = m_uniqueIdentifier;
    initialize();
    m_uniqueIdentifier = savedUniqueID;
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
    setNumberOfRows(2);
    setNumberOfColumns(2);

    m_name.clear();
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
    
    m_name    = obj.m_name;
    m_columns = obj.m_columns;
    m_rows    = obj.m_rows;
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
 * Get infoformation for the given elemnent in the columns.
 *
 * @param Information for element.
 */
TileTabsRowColumnElement*
TileTabsConfiguration::getColumn(const int32_t columnIndex)
{
    CaretAssertVectorIndex(m_columns, columnIndex);
    return &m_columns[columnIndex];
}

/**
 * Get infoformation for the given elemnent in the columns.
 *
 * @param Information for element.
 */
const TileTabsRowColumnElement*
TileTabsConfiguration::getColumn(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columns, columnIndex);
    return &m_columns[columnIndex];
}

/**
 * Get infoformation for the given elemnent in the rows.
 *
 * @param Information for element.
 */
TileTabsRowColumnElement*
TileTabsConfiguration::getRow(const int32_t rowIndex)
{
    CaretAssertVectorIndex(m_rows, rowIndex);
    return &m_rows[rowIndex];
}

/**
 * Get infoformation for the given elemnent in the rows.
 *
 * @param Information for element.
 */
const TileTabsRowColumnElement*
TileTabsConfiguration::getRow(const int32_t rowIndex) const
{
    CaretAssertVectorIndex(m_rows, rowIndex);
    return &m_rows[rowIndex];
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
            float rowPercentTotal = 0.0;
            float rowStretchTotal = 0.0;
            for (int32_t i = 0; i < numRows; i++) {
                const TileTabsRowColumnElement* e = getRow(i);
                switch (e->getStretchType()) {
                    case TileTabsRowColumnStretchTypeEnum::PERCENT:
                        rowPercentTotal += (e->getPercentStretch() / 100.0);
                        break;
                    case TileTabsRowColumnStretchTypeEnum::WEIGHT:
                        rowStretchTotal += e->getWeightStretch();
                        break;
                }
            }
            
            float windowWeightHeight = windowHeight;
            if (rowPercentTotal > 0.0) {
                if (rowPercentTotal >= 1.0) {
                    windowWeightHeight = 0.0;
                }
                else {
                    windowWeightHeight = (1.0 - rowPercentTotal) * windowHeight;
                }
            }
            for (int32_t i = 0; i < numRows; i++) {
                int32_t h = 0;
                const TileTabsRowColumnElement* e = getRow(i);
                switch (e->getStretchType()) {
                    case TileTabsRowColumnStretchTypeEnum::PERCENT:
                        h = (e->getPercentStretch() / 100.0) * windowHeight;
                        break;
                    case TileTabsRowColumnStretchTypeEnum::WEIGHT:
                        if (rowStretchTotal > 0.0) {
                            h = static_cast<int32_t>((e->getWeightStretch() / rowStretchTotal)
                                                     * windowWeightHeight);
                        }
                        break;
                }
                
                rowHeightsOut.push_back(h);
            }
            
            /*
             * Determine width of each column
             */
            float columnPercentTotal = 0.0;
            float columnStretchTotal = 0.0;
            for (int32_t i = 0; i < numCols; i++) {
                const TileTabsRowColumnElement* e = getColumn(i);
                switch (e->getStretchType()) {
                    case TileTabsRowColumnStretchTypeEnum::PERCENT:
                        columnPercentTotal += (e->getPercentStretch() / 100.0);
                        break;
                    case TileTabsRowColumnStretchTypeEnum::WEIGHT:
                        columnStretchTotal += e->getWeightStretch();
                        break;
                }
            }
            
            float windowWeightWidth = windowWidth;
            if (columnPercentTotal > 0.0) {
                if (columnPercentTotal >= 1.0) {
                    windowWeightWidth = 0.0;
                }
                else {
                    windowWeightWidth = (1.0 - columnPercentTotal) * windowWidth;
                }
            }
            
            for (int32_t i = 0; i < numCols; i++) {
                int32_t w = 0;
                const TileTabsRowColumnElement* e = getColumn(i);
                switch (e->getStretchType()) {
                    case TileTabsRowColumnStretchTypeEnum::PERCENT:
                        w = (e->getPercentStretch() / 100.0) * windowWidth;
                        break;
                    case TileTabsRowColumnStretchTypeEnum::WEIGHT:
                        if (columnStretchTotal > 0.0) {
                            w = static_cast<int32_t>((e->getWeightStretch() / columnStretchTotal)
                                                     * windowWeightWidth);
                        }
                        break;
                }
                columnWidthsOut.push_back(w);
            }
        }            break;
    }
    
    if ((numRows == static_cast<int32_t>(rowHeightsOut.size()))
        && (numCols == static_cast<int32_t>(columnWidthsOut.size()))) {
//        /*
//         * Verify all rows fit within the window
//         */
//        int32_t rowHeightsSum = 0;
//        for (int32_t i = 0; i < numRows; i++) {
//            rowHeightsSum += rowHeightsOut[i];
//        }
//        if (rowHeightsSum > windowHeight) {
//            CaretLogSevere("PROGRAM ERROR: Tile Tabs total row heights exceed window height");
////            rowHeightsOut[numRows - 1] -= (rowHeightsSum - windowHeight);
//        }
//        
//        /*
//         * Adjust width of last column so that it does not extend beyond viewport
//         */
//        int32_t columnWidthsSum = 0;
//        for (int32_t i = 0; i < numCols; i++) {
//            columnWidthsSum += columnWidthsOut[i];
//        }
//        if (columnWidthsSum > windowWidth) {
//            CaretLogSevere("PROGRAM ERROR: Tile Tabs total row heights exceed window height");
////            columnWidthsOut[numCols - 1] = columnWidthsSum - windowWidth;
//        }
//        
//        CaretLogFiner("Tile Tabs Row Heights: "
//                      + AString::fromNumbers(rowHeightsOut, ", "));
//        CaretLogFiner("Tile Tabs Column Widths: "
//                      + AString::fromNumbers(columnWidthsOut, ", "));
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
    return m_rows.size();
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
    m_rows.resize(numberOfRows);
}

/**
 * @return Number of columns.
 */
int32_t
TileTabsConfiguration::getNumberOfColumns() const
{
    return m_columns.size();
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
    m_columns.resize(numberOfColumns);
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
}

/**
 * @return Encoded tile tabs configuration in XML
 */
AString
TileTabsConfiguration::encodeInXML(AString s) const
{
    return encodeVersionInXML(2);
}

/**
 * @return Encoded tile tabs configuration in XML
 * using the give XML version of TileTabsConfiguration.
 */
AString
TileTabsConfiguration::encodeVersionInXML(const int32_t versionNumber) const
{
    AString s;
    
    switch (versionNumber) {
        case 1:
            s = encodeInXMLWithStreamWriterVersionOne();
            break;
        case 2:
            s = encodeInXMLWithStreamWriterVersionTwo();
            break;
        default:
            CaretAssertMessage(0, "Requested invalid version=" + AString::number(versionNumber));
            break;
    }
    
    return s;
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
    
    writer.writeStartElement(s_v1_rootTagName);
    
    writer.writeStartElement(s_v1_versionTagName);
    writer.writeAttribute(s_v1_versionNumberAttributeName, "1");
    writer.writeEndElement();
    
    writer.writeTextElement(s_nameTagName, m_name);
    writer.writeTextElement(s_uniqueIdentifierTagName, m_uniqueIdentifier);
    
    const int32_t numberOfRows = getNumberOfRows();
    writer.writeStartElement(s_v1_rowStretchFactorsTagName);
    writer.writeAttribute(s_v1_rowStretchFactorsSelectedCountAttributeName, AString::number(numberOfRows));
    writer.writeAttribute(s_v1_rowStretchFactorsTotalCountAttributeName, AString::number(numberOfRows));
    std::vector<float> rowStretchFactors;
    for (const auto e : m_rows) {
        rowStretchFactors.push_back(e.getWeightStretch());
    }
    writer.writeCharacters(AString::fromNumbers(rowStretchFactors, " "));
    writer.writeEndElement();
    
    const int32_t numberOfColumns = getNumberOfColumns();
    writer.writeStartElement(s_v1_columnStretchFactorsTagName);
    writer.writeAttribute(s_v1_columnStretchFactorsSelectedCountAttributeName, AString::number(numberOfColumns));
    writer.writeAttribute(s_v1_columnStretchFactorsTotalCountAttributeName, AString::number(numberOfColumns));
    std::vector<float> columnStretchFactors;
    for (const auto e : m_columns) {
        columnStretchFactors.push_back(e.getWeightStretch());
    }
    writer.writeCharacters(AString::fromNumbers(columnStretchFactors, " "));
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
    
    writer.writeStartElement(s_v2_rootTagName);
    writer.writeAttribute(s_v2_versionAttributeName, "2");
    
    writer.writeTextElement(s_nameTagName, m_name);
    writer.writeTextElement(s_uniqueIdentifierTagName, m_uniqueIdentifier);
    
    encodeRowColumnElement(writer, s_v2_columnsTagName, m_columns);
    encodeRowColumnElement(writer, s_v2_rowsTagName, m_rows);
    
    writer.writeEndElement();
    
    return xmlString;
}

/**
 * Encode a vector of elements into xml.
 * 
 * @param writer
 *     The XML stream writer.
 * @param tagName
 *     Tag name for enclosing the elements.
 * @param elements
 *     Vector of elements added to XML.
 */
void
TileTabsConfiguration::encodeRowColumnElement(QXmlStreamWriter& writer,
                                              const AString tagName,
                                              const std::vector<TileTabsRowColumnElement>& elements) const
{
    writer.writeStartElement(tagName);
    
    for (const auto e : elements) {
        writer.writeStartElement(s_v2_elementTagName);
        writer.writeAttribute(s_v2_contentTypeAttributeName,    TileTabsRowColumnContentTypeEnum::toName(e.getContentType()));
        writer.writeAttribute(s_v2_stretchTypeAttributeName,    TileTabsRowColumnStretchTypeEnum::toName(e.getStretchType()));
        writer.writeAttribute(s_v2_percentStretchAttributeName, AString::number(e.getPercentStretch(), 'f', 2));
        writer.writeAttribute(s_v2_weightStretchAttributeName,  AString::number(e.getWeightStretch(), 'f', 2));
        writer.writeEndElement();
    }
    
    writer.writeEndElement();
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
        if (tagName == s_v1_rootTagName) {
                decodeFromXMLWithStreamReaderVersionOne(xml);
        }
        else if (tagName == s_v2_rootTagName) {
            /*
             * Version 2 uses a different root tag than version 1.  The reason is that 
             * the older code for decoding from XML will throw an exception if it 
             * encounters invalid elements or the version number is invalid.  The problem
             * is that the exception is not caught and wb_view will terminate.
             */
            QString versionNumberText("Unknown");
            const QXmlStreamAttributes atts = xml.attributes();
            if (atts.hasAttribute(s_v2_versionAttributeName)) {
                versionNumberText = atts.value(s_v2_versionAttributeName).toString();
            }

            if (versionNumberText == "2") {
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
                           + s_v1_rootTagName
                           + " or "
                           + s_v2_rootTagName);
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
    
    const bool debugFlag(false);
    if (debugFlag) {
        AString xmlText = encodeInXMLWithStreamWriterVersionTwo();
        std::cout << std::endl << "NEW: " << xmlText << std::endl << std::endl;
        AString em;
        TileTabsConfiguration temp;
        QXmlStreamReader tempReader(xmlText);
        tempReader.readNextStartElement();
        temp.decodeFromXMLWithStreamReaderVersionTwo(tempReader);
        if (tempReader.hasError()) {
            std::cout << "Decode error: " << tempReader.errorString() << std::endl;
        }
        else {
            std::cout << "Decoded: " << temp.toString() << std::endl;
        }

        std::cout << std::endl;
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

            if (tagName == s_v1_versionTagName) {
                /* ignore */
            }
            else if (tagName == s_nameTagName) {
                name = xml.readElementText();
            }
            else if (tagName == s_uniqueIdentifierTagName) {
                uniqueID = xml.readElementText();
            }
            else if (tagName == s_v1_rowStretchFactorsTagName) {
                QXmlStreamAttributes atts = xml.attributes();
                if (atts.hasAttribute(s_v1_rowStretchFactorsSelectedCountAttributeName)) {
                    numberOfRows = atts.value(s_v1_rowStretchFactorsSelectedCountAttributeName).toInt();
                }
                
                AString::toNumbers(xml.readElementText(), rowStretchFactors);
            }
            else if (tagName == s_v1_columnStretchFactorsTagName) {
                QXmlStreamAttributes atts = xml.attributes();
                if (atts.hasAttribute(s_v1_columnStretchFactorsSelectedCountAttributeName)) {
                    numberOfColumns = atts.value(s_v1_columnStretchFactorsSelectedCountAttributeName).toInt();
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
        message.append(s_v1_rowStretchFactorsTagName
                       + " not found or invalid.  ");
    }
    if (columnStretchFactors.empty()) {
        message.append(s_v1_columnStretchFactorsTagName
                       + " not found or invalid.  ");
    }
    if (numberOfRows <= 0) {
        message.append(s_v1_rowStretchFactorsTagName
                       + " attribute "
                       + s_v1_rowStretchFactorsSelectedCountAttributeName
                       + " is missing or invalid."  );
    }
    if (numberOfRows <= 0) {
        message.append(s_v1_columnStretchFactorsTagName
                       + " attribute "
                       + s_v1_columnStretchFactorsSelectedCountAttributeName
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
        
        m_rows.clear();
        m_columns.clear();
        
        for (int32_t i = 0; i < numberOfRows; i++) {
            TileTabsRowColumnElement element;
            CaretAssertVectorIndex(rowStretchFactors, i);
            element.setWeightStretch(rowStretchFactors[i]);
            element.setContentType(TileTabsRowColumnContentTypeEnum::TAB);
            element.setStretchType(TileTabsRowColumnStretchTypeEnum::WEIGHT);
            m_rows.push_back(element);
        }
        CaretAssert(numberOfRows == static_cast<int32_t>(m_rows.size()));
        
        for (int32_t i = 0; i < numberOfColumns; i++) {
            TileTabsRowColumnElement element;
            CaretAssertVectorIndex(columnStretchFactors, i);
            element.setWeightStretch(columnStretchFactors[i]);
            element.setContentType(TileTabsRowColumnContentTypeEnum::TAB);
            element.setStretchType(TileTabsRowColumnStretchTypeEnum::WEIGHT);
            m_columns.push_back(element);
        }
        CaretAssert(numberOfColumns == static_cast<int32_t>(m_columns.size()));
    }
    else {
        xml.raiseError(message);
    }
}

/**
 * Decode Version Two of the tile tabs configuration from XML with stream reader.
 *
 * @param xml
 *   Stream XML parser.
 */
void
TileTabsConfiguration::decodeFromXMLWithStreamReaderVersionTwo(QXmlStreamReader& xml)
{
    m_rows.clear();
    m_columns.clear();
    m_uniqueIdentifier.clear();
    
    std::set<QString> invalidElements;
    
    AString name;
    AString uniqueID;
    
    QString message;

    enum class ReadMode {
        OTHER,
        COLUMNS,
        ROWS
    };
    ReadMode readMode = ReadMode::OTHER;
    
    while ( ! xml.atEnd()) {
        xml.readNext();
        
        if (xml.isStartElement()) {
            const QStringRef tagName(xml.name());
            
            if (tagName == s_nameTagName) {
                name = xml.readElementText();
            }
            else if (tagName == s_uniqueIdentifierTagName) {
                uniqueID = xml.readElementText();
            }
            else if (tagName == s_v2_columnsTagName) {
                readMode = ReadMode::COLUMNS;
            }
            else if (tagName == s_v2_rowsTagName) {
                readMode = ReadMode::ROWS;
            }
            else if (tagName == s_v2_elementTagName) {
                switch (readMode) {
                    case ReadMode::OTHER:
                        CaretAssert(0);
                        break;
                    case ReadMode::COLUMNS:
                    {
                        AString errorMessage;
                        TileTabsRowColumnElement e;
                        if (decodeRowColumnElement(xml, e, errorMessage)) {
                            m_columns.push_back(e);
                        }
                        else {
                            message.append(errorMessage);
                        }
                    }
                        break;
                    case ReadMode::ROWS:
                    {
                        AString errorMessage;
                        TileTabsRowColumnElement e;
                        if (decodeRowColumnElement(xml, e, errorMessage)) {
                            m_rows.push_back(e);
                        }
                        else {
                            message.append(errorMessage);
                        }
                    }
                        break;
                }
            }
            else {
                invalidElements.insert(tagName.toString());
                xml.skipCurrentElement();
            }
        }
        else if (xml.isEndElement()) {
            const QStringRef tagName(xml.name());
            
            if (tagName == s_v2_columnsTagName) {
                readMode = ReadMode::OTHER;
            }
            else if (tagName == s_v2_rowsTagName) {
                readMode = ReadMode::OTHER;
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
    }
    else {
        xml.raiseError(message);
    }
}

/**
 * Decode elements in a row or column.
 *
 * @param reader
 *     The XML stream reader.
 * @param element
 *     row/column element that is read
 * @param errorMessageOut
 *     Contains error information.
 * @return
 *     True if read successfully, else false.
 */
bool
TileTabsConfiguration::decodeRowColumnElement(QXmlStreamReader& reader,
                                              TileTabsRowColumnElement& element,
                                              AString& errorMessageOut)
{
    const QXmlStreamAttributes atts = reader.attributes();

    errorMessageOut.clear();
    
    if (atts.hasAttribute(s_v2_contentTypeAttributeName)) {
        bool validFlag(false);
        const AString s = atts.value(s_v2_contentTypeAttributeName).toString();
        element.setContentType(TileTabsRowColumnContentTypeEnum::fromName(s, &validFlag));
        if ( ! validFlag) {
            errorMessageOut.append("Content type \"" + s + "\" is not valid.  ");
        }
    }
    else {
        errorMessageOut.append("Content type is missing.  ");
    }
    
    if (atts.hasAttribute(s_v2_stretchTypeAttributeName)) {
        bool validFlag(false);
        const AString s = atts.value(s_v2_stretchTypeAttributeName).toString();
        element.setStretchType(TileTabsRowColumnStretchTypeEnum::fromName(s, &validFlag));
        if ( ! validFlag) {
            errorMessageOut.append("Stretch type \"" + s + "\" is not valid.  ");
        }
    }
    else {
        errorMessageOut.append("Stretch type is missing.  ");
    }
    
    if (atts.hasAttribute(s_v2_percentStretchAttributeName)) {
        const float f = atts.value(s_v2_percentStretchAttributeName).toFloat();
        if ((f >= 0.0) && (f < 100.0)) {
            element.setPercentStretch(f);
        }
        else {
            errorMessageOut.append("Stretch percentage=" + AString::number(f) + " is invalid.");
        }
    }
    else {
        errorMessageOut.append("Stretch percentage is missing.  ");
    }
    
    if (atts.hasAttribute(s_v2_weightStretchAttributeName)) {
        const float f = atts.value(s_v2_weightStretchAttributeName).toFloat();
        if ((f >= 0.0) && (f < 100.0)) {
            element.setWeightStretch(f);
        }
        else {
            errorMessageOut.append("Stretch weight=" + AString::number(f) + " is invalid.");
        }
    }
    else {
        errorMessageOut.append("Stretch weight is missing.  ");
    }
    
    if (errorMessageOut.isEmpty()) {
        return true;
    }
    return false;
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
    AString s("Name: %1, Unique ID: %2\n");
    s = s.arg(m_name).arg(m_uniqueIdentifier);
    
    int32_t indx(0);
    for (const auto item : m_columns) {
        s.append("   Column " + AString::number(indx) + ": " + item.toString() + "\n");
        indx++;
    }
    indx = 0;
    for (const auto item : m_rows) {
        s.append("   Row " + AString::number(indx) + ": " + item.toString() + "\n");
        indx++;
    }
    
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

