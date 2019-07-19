
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

#define __TILE_TABS_LAYOUT_BASE_CONFIGURATION_DECLARE__
#include "TileTabsLayoutBaseConfiguration.h"
#undef __TILE_TABS_LAYOUT_BASE_CONFIGURATION_DECLARE__

#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SystemUtilities.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsLayoutManualConfiguration.h"

using namespace caret;


    
/**
 * \class caret::TileTabsLayoutBaseConfiguration
 * \brief Defines a tile tabs configuration
 * \ingroup Common
 */

/**
 * Constructor that creates a 2 by 2 configuration.
 */
TileTabsLayoutBaseConfiguration::TileTabsLayoutBaseConfiguration(const TileTabsLayoutConfigurationTypeEnum::Enum layoutType)
: CaretObject(),
m_layoutType(layoutType)
{
    initializeTileTabsLayoutBaseConfiguration();
}

/**
 * Destructor.
 */
TileTabsLayoutBaseConfiguration::~TileTabsLayoutBaseConfiguration()
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
TileTabsLayoutBaseConfiguration::TileTabsLayoutBaseConfiguration(const TileTabsLayoutBaseConfiguration& obj)
: CaretObject(obj),
m_layoutType(obj.m_layoutType)
{
    const AString savedUniqueID = m_uniqueIdentifier;
    initializeTileTabsLayoutBaseConfiguration();
    m_uniqueIdentifier = savedUniqueID;
    this->copyHelperTileTabsLayoutBaseConfiguration(obj);
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
TileTabsLayoutBaseConfiguration&
TileTabsLayoutBaseConfiguration::operator=(const TileTabsLayoutBaseConfiguration& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperTileTabsLayoutBaseConfiguration(obj);
    }
    return *this;    
}

/**
 * Initialize an instance of a tile tabs configuration.
 */
void
TileTabsLayoutBaseConfiguration::initializeTileTabsLayoutBaseConfiguration()
{
    m_name.clear();
    m_uniqueIdentifier = SystemUtilities::createUniqueID();
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsLayoutBaseConfiguration::copyHelperTileTabsLayoutBaseConfiguration(const TileTabsLayoutBaseConfiguration& obj)
{
    if (this == &obj) {
        return;
    }
    
    m_name    = obj.m_name;
    //DO NOT CHANGE THE UNIQUE IDENTIFIER:  m_uniqueIdentifier
}

/**
 * Copies the tile tabs configuration rows, columns, and
 * stretch factors.   Name is NOT copied.
 */
void
TileTabsLayoutBaseConfiguration::copy(const TileTabsLayoutBaseConfiguration& rhs)
{
    CaretAssertToDoFatal();  // need to do this a different way
    AString savedName = m_name;
    copyHelperTileTabsLayoutBaseConfiguration(rhs);
    m_name = savedName;
}

/**
 * @return Type of layout for the configuration
 */
TileTabsLayoutConfigurationTypeEnum::Enum
TileTabsLayoutBaseConfiguration::getLayoutType() const
{
    return m_layoutType;
}

/**
 * @return the name of the tile tabs configuration.
 */
AString
TileTabsLayoutBaseConfiguration::getName() const
{
    return m_name;
}

/**
 * @return Get the unique identifier that uniquely identifies each configuration.
 */
AString
TileTabsLayoutBaseConfiguration::getUniqueIdentifier() const
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
TileTabsLayoutBaseConfiguration::setName(const AString& name)
{
    m_name = name;
}

/**
 * Set the unique identifier of the tile tabs configuration.
 *
 * @param uniqueID
 *    New unique identifier for configuration.
 */
void
TileTabsLayoutBaseConfiguration::setUniqueIdentifierProtected(const AString& uniqueID)
{
    m_uniqueIdentifier = uniqueID;
}


/**
 * @return Encoded tile tabs configuration in XML
 */
AString
TileTabsLayoutBaseConfiguration::encodeInXML() const
{
    AString s;
    encodeInXMLString(s);
    return s;
}

/**
 * Decode the tile tabs configuration from XML
 *
 * @param xmlString
 *   String containing XML.
 * @param errorMessageOut
 *   Contains error information if decoding fails.
 * @return
 *   Pointer to the configuration or NULL if there was an error.
 */
TileTabsLayoutBaseConfiguration*
TileTabsLayoutBaseConfiguration::decodeFromXML(const AString& xmlString,
                                         AString& errorMessageOut)
{
    errorMessageOut.clear();
    TileTabsLayoutBaseConfiguration* configurationOut(NULL);
    
    QXmlStreamReader xml(xmlString);

    if (xml.readNextStartElement()) {
        const QStringRef tagName(xml.name());
        if (tagName == TileTabsLayoutGridConfiguration::s_v1_rootTagName) {
            TileTabsLayoutGridConfiguration* v1 = new TileTabsLayoutGridConfiguration();
            v1->decodeFromXMLString(xml,
                                    tagName.toString());
            configurationOut = v1;
        }
        else if (tagName == TileTabsLayoutGridConfiguration::s_v2_rootTagName) {
            TileTabsLayoutGridConfiguration* v2 = new TileTabsLayoutGridConfiguration();
            v2->decodeFromXMLString(xml,
                                    tagName.toString());
            configurationOut = v2;
        }
        else if (tagName == TileTabsLayoutManualConfiguration::s_rootElementName) {
            TileTabsLayoutManualConfiguration* manConfig = new TileTabsLayoutManualConfiguration();
            manConfig->decodeFromXMLString(xml,
                                           tagName.toString());
            configurationOut = manConfig;
        }
        else {
            xml.raiseError("TileTabsLayoutBaseConfiguration first element is "
                           + xml.name().toString()
                           + " but should be "
                           + TileTabsLayoutGridConfiguration::s_v1_rootTagName
                           + " or "
                           + TileTabsLayoutGridConfiguration::s_v2_rootTagName);
        }
    }
    else {
        xml.raiseError("TileTabsLayoutBaseConfiguration failed to find start elemnent.");
    }

    if (xml.hasError()) {
        errorMessageOut = ("Tile Tabs Configuration Read Error at line number="
                           + AString::number(xml.lineNumber())
                           + " column number="
                           + AString::number(xml.columnNumber())
                           + " description="
                           + xml.errorString());
        if (configurationOut != NULL) {
            delete configurationOut;
            configurationOut = NULL;
        }
        return configurationOut;
    }

    const bool debugFlag(false);
    if (debugFlag) {
//        AString xmlText = encodeInXMLWithStreamWriterVersionTwo();
//        std::cout << std::endl << "NEW: " << xmlText << std::endl << std::endl;
//        AString em;
//        TileTabsLayoutBaseConfiguration temp;
//        QXmlStreamReader tempReader(xmlText);
//        tempReader.readNextStartElement();
//        temp.decodeFromXMLWithStreamReaderVersionTwo(tempReader);
//        if (tempReader.hasError()) {
//            std::cout << "Decode error: " << tempReader.errorString() << std::endl;
//        }
//        else {
//            std::cout << "Decoded: " << temp.toString() << std::endl;
//        }
//
//        std::cout << std::endl;
    }
    return configurationOut;
}


/**
 * @return String version of an instance.
 */
AString
TileTabsLayoutBaseConfiguration::toString() const
{
    QString str;
    QTextStream ts(&str);
    ts << "TileTabsLayoutBaseConfiguration: "
    << " m_name=" << m_name
    << " m_uniqueIdentifier=" << m_uniqueIdentifier
    << " m_layoutType=" << TileTabsLayoutConfigurationTypeEnum::toName(m_layoutType);
//    AString s("Name: %1, Unique ID: %2\n");
//    s = s.arg(m_name).arg(m_uniqueIdentifier);
//
//    int32_t indx(0);
//    for (const auto item : m_columns) {
//        s.append("   Column " + AString::number(indx) + ": " + item.toString() + "\n");
//        indx++;
//    }
//    indx = 0;
//    for (const auto item : m_rows) {
//        s.append("   Row " + AString::number(indx) + ": " + item.toString() + "\n");
//        indx++;
//    }
    
    return str;
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
TileTabsLayoutBaseConfiguration::lessThanComparisonByName(const TileTabsLayoutBaseConfiguration* ttc1,
                                                const TileTabsLayoutBaseConfiguration* ttc2)
{
    if (ttc1->getName() < ttc2->getName()) {
        return true;
    }
    return false;
}

