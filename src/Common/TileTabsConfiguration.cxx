
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

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
TileTabsConfiguration::TileTabsConfiguration(const int32_t numberOfRows,
                                             const int32_t numberOfColumns)
: CaretObject()
{
    resizeStretchFactors(numberOfRows,
                         numberOfColumns);
    m_uniqueIdentifier = SystemUtilities::createUniqueID();
}

/**
 * Constructor that creates a 2 by 2 configuration.
 */
TileTabsConfiguration::TileTabsConfiguration()
: CaretObject()
{
    resizeStretchFactors(2, 2);
    m_uniqueIdentifier = SystemUtilities::createUniqueID();
}

/**
 * Destructor.
 */
TileTabsConfiguration::~TileTabsConfiguration()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TileTabsConfiguration::TileTabsConfiguration(const TileTabsConfiguration& obj)
: CaretObject(obj)
{
    this->copyHelperTileTabsConfiguration(obj);
}

/**
 * Assignment operator.
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
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsConfiguration::copyHelperTileTabsConfiguration(const TileTabsConfiguration& obj)
{
    m_rowStretchFactors = obj.m_rowStretchFactors;
    m_columnStretchFactors = obj.m_columnStretchFactors;
    m_name = obj.m_name;
    m_uniqueIdentifier = SystemUtilities::createUniqueID();
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
    return m_rowStretchFactors.size();
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
    CaretAssert(numberOfRows >= 1);
    
    resizeStretchFactors(numberOfRows,
                         getNumberOfColumns());
}

/**
 * @return Number of columns.
 */
int32_t
TileTabsConfiguration::getNumberOfColumns() const
{
    return m_columnStretchFactors.size();
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
    CaretAssert(numberOfColumns >= 1);
    
    resizeStretchFactors(getNumberOfRows(),
                         numberOfColumns);
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
TileTabsConfiguration::getColumnStretchFactor(const int32_t columnIndex)
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
TileTabsConfiguration::getRowStretchFactor(const int32_t rowIndex)
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
 * Resize the stretch factors.
 *
 * @param numberOfRows
 *     New number of rows.
 * @param numberOfColumns
 *     New number of columns.
 */
void
TileTabsConfiguration::resizeStretchFactors(const int32_t numberOfRows,
                                            const int32_t numberOfColumns)
{
    CaretAssert(numberOfRows > 0);
    CaretAssert(numberOfColumns > 0);
    
    m_rowStretchFactors.resize(numberOfRows,
                               1.0);
    m_columnStretchFactors.resize(numberOfColumns,
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
    
    QDomElement rowStretchFactorsTag = doc.createElement(s_rowStretchFactorsTagName);
    rowStretchFactorsTag.setAttribute(s_rowStretchFactorsNumberOfRowsAttributeName,
                                      static_cast<int>(m_rowStretchFactors.size()));
    rowStretchFactorsTag.appendChild(doc.createTextNode(AString::fromNumbers(m_rowStretchFactors,
                                                                             " ")));
    root.appendChild(rowStretchFactorsTag);

    QDomElement columnStretchFactorsTag = doc.createElement(s_columnStretchFactorsTagName);
    columnStretchFactorsTag.setAttribute(s_columnStretchFactorsNumberOfColumnsAttributeName,
                                         static_cast<int>(m_columnStretchFactors.size()));
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
    resizeStretchFactors(2, 2);
    
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
TileTabsConfiguration::parseVersionOneXML(QDomDocument& doc) throw (CaretException)
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
    
    QDomNodeList rowNodeList = doc.elementsByTagName(s_rowStretchFactorsTagName);
    if (rowNodeList.isEmpty()) {
        throw CaretException("Error finding row stretch factors tag");
    }
    QDomElement rowElement = rowNodeList.at(0).toElement();
    if (rowElement.isNull()) {
        throw CaretException("Error finding row element");
    }
    
    const AString numberOfRowsString = rowElement.attribute(s_rowStretchFactorsNumberOfRowsAttributeName,
                                                            "0");
    if (numberOfRowsString.isEmpty()) {
        throw CaretException("Error finding number of rows attribute");
    }
    const int32_t numberOfRows = numberOfRowsString.toInt();
    if (numberOfRows <= 0) {
        throw CaretException("Invalid number of rows attribute "
                             + numberOfRowsString);
    }
    const AString rowStretchFactorsText = rowElement.text();
    std::vector<float> rowStretchFactors;
    AString::toNumbers(rowStretchFactorsText,
                       rowStretchFactors);
    if (static_cast<int32_t>(rowStretchFactors.size()) != numberOfRows) {
        throw CaretException("Stretch factor number of rows is "
                             + AString::number(numberOfRows)
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
    const AString numberOfColumnsString = columnElement.attribute(s_columnStretchFactorsNumberOfColumnsAttributeName,
                                                               "0");
    if (numberOfColumnsString.isEmpty()) {
        throw CaretException("Error finding number of columns attribute");
    }
    const int32_t numberOfColumns = numberOfColumnsString.toInt();
    if (numberOfColumns <= 0) {
        throw CaretException("Invalid number of columns attribute "
                             + numberOfColumnsString);
    }
    const AString columnStretchFactorsText = columnElement.text();
    std::vector<float> columnStretchFactors;
    AString::toNumbers(columnStretchFactorsText,
                       columnStretchFactors);
    if (static_cast<int32_t>(columnStretchFactors.size()) != numberOfColumns) {
        throw CaretException("Stretch factor number of columns is "
                             + AString::number(numberOfColumns)
                             + " but have "
                             + AString::number(static_cast<int32_t>(columnStretchFactors.size()))
                             + " stretch factors.");
    }
    
    resizeStretchFactors(numberOfRows,
                         numberOfColumns);
    for (int32_t i = 0; i < numberOfRows; i++) {
        m_rowStretchFactors[i] = rowStretchFactors[i];
    }
    for (int32_t i = 0; i < numberOfColumns; i++) {
        m_columnStretchFactors[i] = columnStretchFactors[i];
    }
}


