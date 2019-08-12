
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

#define __TILE_TABS_LAYOUT_GRID_CONFIGURATION_DECLARE__
#include "TileTabsLayoutGridConfiguration.h"
#undef __TILE_TABS_LAYOUT_GRID_CONFIGURATION_DECLARE__

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SystemUtilities.h"

using namespace caret;


    
/**
 * \class caret::TileTabsLayoutGridConfiguration
 * \brief Defines a tile tabs configuration
 * \ingroup Common
 */

/**
 * @return A new instance containing a automatic grid configuration
 */
TileTabsLayoutGridConfiguration*
TileTabsLayoutGridConfiguration::newInstanceAutomaticGrid()
{
    TileTabsLayoutGridConfiguration* config = new TileTabsLayoutGridConfiguration(TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID);
    return config;
}

/**
 * @return A new instance containing a custom grid configuration
 */
TileTabsLayoutGridConfiguration*
TileTabsLayoutGridConfiguration::newInstanceCustomGrid()
{
    TileTabsLayoutGridConfiguration* config = new TileTabsLayoutGridConfiguration(TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID);
    return config;
}

/**
 * Constructor that creates a 2 by 2 configuration.
 */
TileTabsLayoutGridConfiguration::TileTabsLayoutGridConfiguration(const TileTabsLayoutConfigurationTypeEnum::Enum gridConfigType)
: TileTabsLayoutBaseConfiguration(gridConfigType)
{
    switch (gridConfigType) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            CaretAssert(0);
            break;
    }
    
    initialize();
}

/**
 * Destructor.
 */
TileTabsLayoutGridConfiguration::~TileTabsLayoutGridConfiguration()
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
TileTabsLayoutGridConfiguration::TileTabsLayoutGridConfiguration(const TileTabsLayoutGridConfiguration& obj)
: TileTabsLayoutBaseConfiguration(obj)
{
    initialize();
    this->copyHelperTileTabsLayoutGridConfiguration(obj);
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
TileTabsLayoutGridConfiguration&
TileTabsLayoutGridConfiguration::operator=(const TileTabsLayoutGridConfiguration& obj)
{
    if (this != &obj) {
        TileTabsLayoutBaseConfiguration::operator=(obj);
        this->copyHelperTileTabsLayoutGridConfiguration(obj);
    }
    return *this;    
}

/**
 * Copy this instance and give it a new unique identifier.
 * Note that copy constructor does not create a new unique identifier.
 *
 * @return The new Copy.
 */
TileTabsLayoutBaseConfiguration*
TileTabsLayoutGridConfiguration::newCopyWithNewUniqueIdentifier() const
{
    TileTabsLayoutGridConfiguration* newCopy = new TileTabsLayoutGridConfiguration(*this);
    CaretAssert(newCopy);
    return newCopy;
}


/**
 * Initialize an instance of a tile tabs configuration.
 */
void
TileTabsLayoutGridConfiguration::initialize()
{
    setNumberOfRows(2);
    setNumberOfColumns(2);

    m_centeringCorrectionEnabled = false;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsLayoutGridConfiguration::copyHelperTileTabsLayoutGridConfiguration(const TileTabsLayoutGridConfiguration& obj)
{
    if (this == &obj) {
        return;
    }
    
    copyHelperTileTabsLayoutBaseConfiguration(obj);
    m_columns = obj.m_columns;
    m_rows    = obj.m_rows;
    m_centeringCorrectionEnabled = obj.m_centeringCorrectionEnabled;
}

/**
 * Copy the given configuration to "this" configuration.  If given configuration
 * does not cast to "this class type" log a warning and do not copy.
 * Name property is not copied.
 *
 * @param rhs
 *      Configuration to copy.
 */
void
TileTabsLayoutGridConfiguration::copy(const TileTabsLayoutBaseConfiguration& rhs)
{
    const TileTabsLayoutGridConfiguration* gridConfig = rhs.castToGridConfiguration();
    if (gridConfig != NULL) {
        AString savedName = getName();
        copyHelperTileTabsLayoutGridConfiguration(*gridConfig);
        setName(savedName);
    }
    else {
        CaretLogSevere("Attempt to copy layout configuration "
                       + rhs.toString()
                       + " to "
                       + toString());
    }
}

/**
 * Get infoformation for the given elemnent in the columns.
 *
 * @param Information for element.
 */
TileTabsGridRowColumnElement*
TileTabsLayoutGridConfiguration::getColumn(const int32_t columnIndex)
{
    CaretAssertVectorIndex(m_columns, columnIndex);
    return &m_columns[columnIndex];
}

/**
 * Get infoformation for the given elemnent in the columns.
 *
 * @param Information for element.
 */
const TileTabsGridRowColumnElement*
TileTabsLayoutGridConfiguration::getColumn(const int32_t columnIndex) const
{
    CaretAssertVectorIndex(m_columns, columnIndex);
    return &m_columns[columnIndex];
}

/**
 * Get infoformation for the given elemnent in the rows.
 *
 * @param Information for element.
 */
TileTabsGridRowColumnElement*
TileTabsLayoutGridConfiguration::getRow(const int32_t rowIndex)
{
    CaretAssertVectorIndex(m_rows, rowIndex);
    return &m_rows[rowIndex];
}

/**
 * Get infoformation for the given elemnent in the rows.
 *
 * @param Information for element.
 */
const TileTabsGridRowColumnElement*
TileTabsLayoutGridConfiguration::getRow(const int32_t rowIndex) const
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
TileTabsLayoutGridConfiguration::getRowHeightsAndColumnWidthsForWindowSize(const int32_t windowWidth,
                                                                 const int32_t windowHeight,
                                                                 const int32_t numberOfModelsToDraw,
                                                                 const TileTabsLayoutConfigurationTypeEnum::Enum configurationMode,
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
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
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
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
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
                const TileTabsGridRowColumnElement* e = getRow(i);
                switch (e->getStretchType()) {
                    case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                        rowPercentTotal += (e->getPercentStretch() / 100.0);
                        break;
                    case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
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
                const TileTabsGridRowColumnElement* e = getRow(i);
                switch (e->getStretchType()) {
                    case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                        h = (e->getPercentStretch() / 100.0) * windowHeight;
                        break;
                    case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
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
                const TileTabsGridRowColumnElement* e = getColumn(i);
                switch (e->getStretchType()) {
                    case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                        columnPercentTotal += (e->getPercentStretch() / 100.0);
                        break;
                    case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
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
                const TileTabsGridRowColumnElement* e = getColumn(i);
                switch (e->getStretchType()) {
                    case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                        w = (e->getPercentStretch() / 100.0) * windowWidth;
                        break;
                    case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
                        if (columnStretchTotal > 0.0) {
                            w = static_cast<int32_t>((e->getWeightStretch() / columnStretchTotal)
                                                     * windowWeightWidth);
                        }
                        break;
                }
                columnWidthsOut.push_back(w);
            }
        }            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            CaretAssert(0);
            break;
    }
    
    if ((numRows == static_cast<int32_t>(rowHeightsOut.size()))
        && (numCols == static_cast<int32_t>(columnWidthsOut.size()))) {
        const bool debugFlag(false);
        if (debugFlag) {
            /*
             * Verify all rows fit within the window
             */
            int32_t rowHeightsSum = 0;
            for (int32_t i = 0; i < numRows; i++) {
                rowHeightsSum += rowHeightsOut[i];
            }
            if (rowHeightsSum > windowHeight) {
                CaretLogSevere("PROGRAM ERROR: Tile Tabs total row heights exceed window height");
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
            }
            
            CaretLogFiner("Tile Tabs Row Heights: "
                          + AString::fromNumbers(rowHeightsOut, ", "));
            CaretLogFiner("Tile Tabs Column Widths: "
                          + AString::fromNumbers(columnWidthsOut, ", "));
        }
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
 * @return Number of rows.
 */
int32_t
TileTabsLayoutGridConfiguration::getNumberOfRows() const
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
TileTabsLayoutGridConfiguration::setNumberOfRows(const int32_t numberOfRows)
{
    m_rows.resize(numberOfRows);
}

/**
 * @return Number of columns.
 */
int32_t
TileTabsLayoutGridConfiguration::getNumberOfColumns() const
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
TileTabsLayoutGridConfiguration::setNumberOfColumns(const int32_t numberOfColumns)
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
TileTabsLayoutGridConfiguration::getRowsAndColumnsForNumberOfTabs(const int32_t numberOfTabs,
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
 * @return True if the centering correction is enabled.
 */
bool
TileTabsLayoutGridConfiguration::isCenteringCorrectionEnabled() const
{
    return m_centeringCorrectionEnabled;
}

/**
 * Set the enabled status of the centering correction
 *
 * @param status
 *     New status for enabling the centering correction
 */
void
TileTabsLayoutGridConfiguration::setCenteringCorrectionEnabled(const bool status)
{
    m_centeringCorrectionEnabled = status;
}


/**
 * Updates the number of rows and columns for the automatic configuration
 * based upon the number of tabs.  
 *
 * Since screen width typically exceeds height, ensure the number of 
 * columns is always greater than the number of rows.
 */
void
TileTabsLayoutGridConfiguration::updateAutomaticConfigurationRowsAndColumns(const int32_t numberOfTabs)
{
    int32_t numRows(0), numCols(0);
    getRowsAndColumnsForNumberOfTabs(numberOfTabs,
                                     numRows,
                                     numCols);
    setNumberOfRows(numRows);
    setNumberOfColumns(numCols);
}

/**
 * Encode the configuration in XML.
 *
 * @param xmlTextOut
 *     Contains XML representation of configuration.
 */
void
TileTabsLayoutGridConfiguration::encodeInXMLString(AString& xmlTextOut) const
{
    xmlTextOut = encodeVersionInXML(2);
}

/**
 * @return Encoded tile tabs configuration in XML
 * using the give XML version of TileTabsLayoutGridConfiguration.
 */
AString
TileTabsLayoutGridConfiguration::encodeVersionInXML(const int32_t versionNumber) const
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
TileTabsLayoutGridConfiguration::encodeInXMLWithStreamWriterVersionOne() const
{
    AString xmlString;
    QXmlStreamWriter writer(&xmlString);
    writer.setAutoFormatting(true);
    
    writer.writeStartElement(s_v1_rootTagName);
    
    writer.writeStartElement(s_v1_versionTagName);
    writer.writeAttribute(s_v1_versionNumberAttributeName, "1");
    writer.writeEndElement();
    
    writer.writeTextElement(s_nameTagName, getName());
    writer.writeTextElement(s_uniqueIdentifierTagName, getUniqueIdentifier());
    
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
TileTabsLayoutGridConfiguration::encodeInXMLWithStreamWriterVersionTwo() const
{
    AString xmlString;
    QXmlStreamWriter writer(&xmlString);
    writer.setAutoFormatting(true);
    
    writer.writeStartElement(s_v2_rootTagName);
    writer.writeAttribute(s_v2_versionAttributeName, "2");
    
    writer.writeTextElement(s_nameTagName, getName());
    writer.writeTextElement(s_uniqueIdentifierTagName, getUniqueIdentifier());
    writer.writeTextElement(s_v2_centeringCorrectionName, AString::fromBool(m_centeringCorrectionEnabled));
    
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
TileTabsLayoutGridConfiguration::encodeRowColumnElement(QXmlStreamWriter& writer,
                                              const AString tagName,
                                              const std::vector<TileTabsGridRowColumnElement>& elements) const
{
    writer.writeStartElement(tagName);
    
    for (const auto e : elements) {
        writer.writeStartElement(s_v2_elementTagName);
        writer.writeAttribute(s_v2_contentTypeAttributeName,    TileTabsGridRowColumnContentTypeEnum::toName(e.getContentType()));
        writer.writeAttribute(s_v2_stretchTypeAttributeName,    TileTabsGridRowColumnStretchTypeEnum::toName(e.getStretchType()));
        writer.writeAttribute(s_v2_percentStretchAttributeName, AString::number(e.getPercentStretch(), 'f', 2));
        writer.writeAttribute(s_v2_weightStretchAttributeName,  AString::number(e.getWeightStretch(), 'f', 2));
        writer.writeEndElement();
    }
    
    writer.writeEndElement();
}

/**
 * Decode the configuration using the given XML stream reader and root element
 * If there is an error, xml.raiseError() should be used to specify the error
 * and caller of this method can test for the error using xml.isError().
 *
 * @param xml
 *     The XML stream reader.
 * @param rootElement
 *     The root element.
 */
void
TileTabsLayoutGridConfiguration::decodeFromXMLString(QXmlStreamReader& xml,
                                                    const AString& rootElementText)
{
    CaretAssert( ! rootElementText.isEmpty());
    
    m_centeringCorrectionEnabled = false;
    
        if (rootElementText == s_v1_rootTagName) {
            decodeFromXMLWithStreamReaderVersionOne(xml);
        }
        else if (rootElementText == s_v2_rootTagName) {
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
                xml.raiseError("TileTabsLayoutGridConfiguration invalid version="
                               + versionNumberText);
            }
        }
        else {
            xml.raiseError("TileTabsLayoutGridConfiguration first element is "
                           + xml.name().toString()
                           + " but should be "
                           + s_v1_rootTagName
                           + " or "
                           + s_v2_rootTagName);
        }    
}

/**
 * Decode Version One of the tile tabs configuration from XML with stream reader.
 *
 * @param xml
 *   Stream XML parser.
 */
void
TileTabsLayoutGridConfiguration::decodeFromXMLWithStreamReaderVersionOne(QXmlStreamReader& xml)
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
    
    static int32_t missingNameCounter = 1;
    if (name.isEmpty()) {
        name = ("Config_V1_"
                + AString::number(missingNameCounter));
        missingNameCounter++;
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
        setName(name);
        setUniqueIdentifierProtected(uniqueID);
        
        m_rows.clear();
        m_columns.clear();
        
        for (int32_t i = 0; i < numberOfRows; i++) {
            TileTabsGridRowColumnElement element;
            CaretAssertVectorIndex(rowStretchFactors, i);
            element.setWeightStretch(rowStretchFactors[i]);
            element.setContentType(TileTabsGridRowColumnContentTypeEnum::TAB);
            element.setStretchType(TileTabsGridRowColumnStretchTypeEnum::WEIGHT);
            m_rows.push_back(element);
        }
        CaretAssert(numberOfRows == static_cast<int32_t>(m_rows.size()));
        
        for (int32_t i = 0; i < numberOfColumns; i++) {
            TileTabsGridRowColumnElement element;
            CaretAssertVectorIndex(columnStretchFactors, i);
            element.setWeightStretch(columnStretchFactors[i]);
            element.setContentType(TileTabsGridRowColumnContentTypeEnum::TAB);
            element.setStretchType(TileTabsGridRowColumnStretchTypeEnum::WEIGHT);
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
TileTabsLayoutGridConfiguration::decodeFromXMLWithStreamReaderVersionTwo(QXmlStreamReader& xml)
{
    m_rows.clear();
    m_columns.clear();
    setName("");
    setUniqueIdentifierProtected("");
    
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
    
    AString centeringCorrectionTextString;
    
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
            else if (tagName == s_v2_centeringCorrectionName) {
                centeringCorrectionTextString = xml.readElementText();
            }
            else if (tagName == s_v2_elementTagName) {
                switch (readMode) {
                    case ReadMode::OTHER:
                        CaretAssert(0);
                        break;
                    case ReadMode::COLUMNS:
                    {
                        AString errorMessage;
                        TileTabsGridRowColumnElement e;
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
                        TileTabsGridRowColumnElement e;
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
    
    static int32_t missingNameCounter = 1;
    if (name.isEmpty()) {
        name = ("Config_"
                + AString::number(missingNameCounter));
        missingNameCounter++;
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
        setName(name);
        setUniqueIdentifierProtected(uniqueID);

        /*
         * Only set centering correction if it is found.  This allows usage
         * of the default value in the event this is not found in the XML.
         */
        if ( ! centeringCorrectionTextString.isEmpty()) {
            m_centeringCorrectionEnabled = centeringCorrectionTextString.toBool();
        }
        
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
TileTabsLayoutGridConfiguration::decodeRowColumnElement(QXmlStreamReader& reader,
                                              TileTabsGridRowColumnElement& element,
                                              AString& errorMessageOut)
{
    const QXmlStreamAttributes atts = reader.attributes();

    errorMessageOut.clear();
    
    if (atts.hasAttribute(s_v2_contentTypeAttributeName)) {
        bool validFlag(false);
        const AString s = atts.value(s_v2_contentTypeAttributeName).toString();
        element.setContentType(TileTabsGridRowColumnContentTypeEnum::fromName(s, &validFlag));
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
        element.setStretchType(TileTabsGridRowColumnStretchTypeEnum::fromName(s, &validFlag));
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
 * @return String version of an instance.
 */
AString
TileTabsLayoutGridConfiguration::toString() const
{
    AString s = TileTabsLayoutBaseConfiguration::toString();
    if ( ! s.isEmpty()) {
        s.append("\n");
    }
    s.append("TileTabsLayoutGridConfiguration:");
    s.append("\n");
    
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
 * Cast to a grid configuration (avoids dynamic_cast that can be slow)
 *
 * @return Pointer to grid configuration or NULL if not a grid configuration.
 */
TileTabsLayoutGridConfiguration*
TileTabsLayoutGridConfiguration::castToGridConfiguration()
{
    return this;
}

/**
 * Cast to a grid configuration (avoids dynamic_cast that can be slow)
 *
 * @return Pointer to grid configuration or NULL if not a grid configuration.
 */
const TileTabsLayoutGridConfiguration*
TileTabsLayoutGridConfiguration::castToGridConfiguration() const
{
    return this;
}

