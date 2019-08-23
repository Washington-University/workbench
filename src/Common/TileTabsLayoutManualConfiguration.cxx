
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include <set>

#define __TILE_TABS_LAYOUT_MANUAL_CONFIGURATION_DECLARE__
#include "TileTabsLayoutManualConfiguration.h"
#undef __TILE_TABS_LAYOUT_MANUAL_CONFIGURATION_DECLARE__

#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SystemUtilities.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsBrowserTabGeometry.h"

using namespace caret;

/**
 * \class caret::TileTabsLayoutManualConfiguration
 * \brief Contains a manual layout for tabs
 * \ingroup Common
 */

/**
 * Constructor.
 */
TileTabsLayoutManualConfiguration::TileTabsLayoutManualConfiguration()
: TileTabsLayoutBaseConfiguration(TileTabsLayoutConfigurationTypeEnum::MANUAL)
{
    
}

/**
 * Destructor.
 */
TileTabsLayoutManualConfiguration::~TileTabsLayoutManualConfiguration()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
TileTabsLayoutManualConfiguration::TileTabsLayoutManualConfiguration(const TileTabsLayoutManualConfiguration& obj)
: TileTabsLayoutBaseConfiguration(obj)
{
    this->copyHelperTileTabsLayoutManualConfiguration(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
TileTabsLayoutManualConfiguration&
TileTabsLayoutManualConfiguration::operator=(const TileTabsLayoutManualConfiguration& obj)
{
    if (this != &obj) {
        TileTabsLayoutBaseConfiguration::operator=(obj);
        this->copyHelperTileTabsLayoutManualConfiguration(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
TileTabsLayoutManualConfiguration::copyHelperTileTabsLayoutManualConfiguration(const TileTabsLayoutManualConfiguration& obj)
{
    copyHelperTileTabsLayoutBaseConfiguration(obj);
    m_tabInfo.clear();
    
    for (const auto& ti : obj.m_tabInfo) {
        TileTabsBrowserTabGeometry* tabInfo = new TileTabsBrowserTabGeometry(*ti);
        addTabInfo(tabInfo);
    }
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
TileTabsLayoutManualConfiguration::copy(const TileTabsLayoutBaseConfiguration& rhs)
{
    const TileTabsLayoutManualConfiguration* manualConfig = rhs.castToManualConfiguration();
    if (manualConfig != NULL) {
        AString savedName = getName();
        copyHelperTileTabsLayoutManualConfiguration(*manualConfig);
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
 * Copy this instance and give it a new unique identifier.
 * Note that copy constructor does not create a new unique identifier.
 *
 * @return The new Copy.
 */
TileTabsLayoutBaseConfiguration*
TileTabsLayoutManualConfiguration::newCopyWithNewUniqueIdentifier() const
{
    TileTabsLayoutBaseConfiguration* newCopy = new TileTabsLayoutManualConfiguration(*this);
    CaretAssert(newCopy);
    return newCopy;
}

/**
 * Create a manual layout from the given grid layout using the given tab indices
 *
 * @param gridLayout
 *     The grid layout
 * @param gridMode
 *     The grid mode
 * @param tabIndices
 *     Indices of tabs
 */
TileTabsLayoutManualConfiguration*
TileTabsLayoutManualConfiguration::newInstanceFromGridLayout(TileTabsLayoutGridConfiguration* gridLayout,
                                                             const TileTabsLayoutConfigurationTypeEnum::Enum gridMode,
                                                             const std::vector<int32_t>& tabIndices)
{
    CaretAssert(gridLayout);
    
    const int32_t numTabs = static_cast<int32_t>(tabIndices.size());
    if (numTabs <= 0) {
        return NULL;
    }
    
    const int32_t windowWidth(10000);
    const int32_t windowHeight(10000);
    std::vector<int32_t> rowHeights;
    std::vector<int32_t> columnWidths;
    
    if (gridLayout->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                              windowHeight,
                                                              numTabs,
                                                              gridMode,
                                                              rowHeights,
                                                              columnWidths)) {
        const int32_t numRows = static_cast<int32_t>(rowHeights.size());
        const int32_t numCols = static_cast<int32_t>(columnWidths.size());
        
        TileTabsLayoutManualConfiguration* manualLayout = new TileTabsLayoutManualConfiguration();
        manualLayout->setName("From row:"
                              + AString::number(rowHeights.size())
                              + " col:"
                              + AString::number(columnWidths.size()));
        
        int32_t tabCounter(0);
        float yBottom(windowHeight);
        for (int32_t i = 0; i < numRows; i++) {
            CaretAssertVectorIndex(rowHeights, i);
            const float height = rowHeights[i];
            yBottom -= height;
            
            switch (gridLayout->getRow(i)->getContentType()) {
                case TileTabsGridRowColumnContentTypeEnum::SPACE:
                    break;
                case TileTabsGridRowColumnContentTypeEnum::TAB:
                {
                    float xLeft(0.0);
                    for (int32_t j = 0; j < numCols; j++) {
                        CaretAssertVectorIndex(columnWidths, j);
                        const float width = columnWidths[j];
                        
                        switch (gridLayout->getColumn(j)->getContentType()) {
                            case TileTabsGridRowColumnContentTypeEnum::SPACE:
                                break;
                            case TileTabsGridRowColumnContentTypeEnum::TAB:
                            {
                                CaretAssertVectorIndex(tabIndices, tabCounter);
                                const int32_t tabIndex(tabIndices[tabCounter]);
                                
                                const float centerX = xLeft + (width / 2.0);
                                const float centerY = yBottom + (height / 2.0);
                                TileTabsBrowserTabGeometry* tabInfo = new TileTabsBrowserTabGeometry(tabIndex);
                                tabInfo->setCenterX((centerX / windowWidth) * 100.0f);
                                tabInfo->setCenterY((centerY / windowHeight) * 100.0f);
                                tabInfo->setWidth((width / windowWidth) * 100.0f);
                                tabInfo->setHeight((height / windowHeight) * 100.0f);
                                tabInfo->setStackingOrder(tabCounter);
                                tabInfo->setBackgroundType(TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG);
                                
                                manualLayout->addTabInfo(tabInfo);
                                
                                tabCounter++;
                            }
                                break;
                        }
                        
                        xLeft += width;
                    }
                }
                    break;
            }
        }

        return manualLayout;
    }

    return NULL;
}


/**
 * Add the tab info to this configuration.  This configuration will take
 * ownership of the tab including destruction of it.
 *
 * @param tabInfo
 *     Tab info to add.
 */
void
TileTabsLayoutManualConfiguration::addTabInfo(TileTabsBrowserTabGeometry* tabInfo)
{
    std::unique_ptr<TileTabsBrowserTabGeometry> ptr(tabInfo);
    m_tabInfo.push_back(std::move(ptr));
}

/**
 * @return Number of tab info in this configuration
 */
int32_t
TileTabsLayoutManualConfiguration::getNumberOfTabs() const
{
    return m_tabInfo.size();
}

/**
 * @return The tab info at the given index
 *
 * @param index
 *     Index of the tab info
 */
TileTabsBrowserTabGeometry*
TileTabsLayoutManualConfiguration::getTabInfo(const int32_t index)
{
    CaretAssertVectorIndex(m_tabInfo, index);
    return m_tabInfo[index].get();
}

/**
 * @return The tab info at the given index
 *
 * @param index
 *     Index of the tab info
 */
const TileTabsBrowserTabGeometry*
TileTabsLayoutManualConfiguration::getTabInfo(const int32_t index) const
{
    CaretAssertVectorIndex(m_tabInfo, index);
    return m_tabInfo[index].get();
}

/**
 * @return String version of an instance.
 */
AString
TileTabsLayoutManualConfiguration::toString() const
{
    QString str(TileTabsLayoutBaseConfiguration::toString());
    
    QTextStream ts(&str);
    ts << "\n" << "TileTabsLayoutManualConfiguration: " << "\n";
    
    for (const auto& ti : m_tabInfo) {
        ts << ti->toString() << "\n";
    }
    
    return str;
}


/**
 * Encode the configuration in XML.
 *
 * @param xmlTextOut
 *     Contains XML representation of configuration.
 */
void
TileTabsLayoutManualConfiguration::encodeInXMLString(AString& xmlTextOut) const
{
    xmlTextOut.clear();
    
    QXmlStreamWriter writer(&xmlTextOut);
    writer.setAutoFormatting(true);
    
    writer.writeStartElement(s_rootElementName);
    writer.writeAttribute(s_rootElementAttributeName,
                          getName());
    writer.writeAttribute(s_rootElementAttributeVersion,
                          s_rootElementAttributeValueVersionOne);
    writer.writeAttribute(s_rootElementAttributeUniqueID,
                          getUniqueIdentifier());
    
    for (const auto& tabInfo : m_tabInfo) {
        writer.writeStartElement(s_tabInfoElementName);
        writer.writeAttribute(s_tabInfoAttributeDisplayStatus,
                              AString::fromBool(tabInfo->isDisplayed()));
        writer.writeAttribute(s_tabInfoAttributeTabIndex,
                              AString::number(tabInfo->getTabIndex()));
        writer.writeAttribute(s_tabInfoAttributeMinX,
                              AString::number(tabInfo->getMinX(), 'f', 2));
        writer.writeAttribute(s_tabInfoAttributeMaxX,
                              AString::number(tabInfo->getMaxX(), 'f', 2));
        writer.writeAttribute(s_tabInfoAttributeMinY,
                              AString::number(tabInfo->getMinY(), 'f', 2));
        writer.writeAttribute(s_tabInfoAttributeMaxY,
                              AString::number(tabInfo->getMaxY(), 'f', 2));
        writer.writeAttribute(s_tabInfoAttributeStackingOrder,
                              AString::number(tabInfo->getStackingOrder()));
        writer.writeAttribute(s_tabInfoAttributeBackground,
                              TileTabsLayoutBackgroundTypeEnum::toName(tabInfo->getBackgroundType()));
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
TileTabsLayoutManualConfiguration::decodeFromXMLString(QXmlStreamReader& xml,
                                                       const AString& rootElementText)
{
    static int32_t invalidNameCounter(1);
    
    CaretAssert( ! rootElementText.isEmpty());
    
    if (rootElementText != s_rootElementName) {
        xml.raiseError("TileTabsLayoutManualConfiguration first element is "
                       + rootElementText
                       + " but should be "
                       + s_rootElementName);
        return;
    }

    AString errorString;
    const QXmlStreamAttributes rootAttributes = xml.attributes();
    
    const AString version = rootAttributes.value(s_rootElementAttributeVersion).toString();
    if (version != s_rootElementAttributeValueVersionOne) {
        errorString.appendWithNewLine("Invalid version \"" + version + "\"");
    }
    
    AString name = rootAttributes.value(s_rootElementAttributeName).toString();
    if (name.isEmpty()) {
        name = ("Tabs_" + AString::number(invalidNameCounter));
        invalidNameCounter++;
        CaretLogWarning("Tile Tabs Manual Configuration is missing name and has been assigned name \""
                        + name
                        + "\"");
    }
    setName(name);

    AString uniqueID = rootAttributes.value(s_rootElementAttributeUniqueID).toString();
    if (uniqueID.isEmpty()) {
        uniqueID = SystemUtilities::createUniqueID();
        CaretLogWarning("Tile Tabs Manual Configuration is missing uniqueID and has been assigned uniqueID \""
                        + uniqueID
                        + "\"");
        
    }
    setUniqueIdentifierProtected(uniqueID);

    if (! errorString.isEmpty()) {
        xml.skipCurrentElement();
        xml.raiseError(errorString);
        return;
    }
    
    std::set<AString> invalidElementNames;
    
    while ( ! xml.atEnd()) {
        xml.readNext();
        
        if (xml.isStartElement()) {
            const QString elementName(xml.name().toString());
            
            if (elementName == s_tabInfoElementName) {
                bool tabIndexValid(false);
                bool minXValid(false);
                bool maxXValid(false);
                bool minYValid(false);
                bool maxYValid(false);
                bool stackingValid(false);
                const QXmlStreamAttributes atts = xml.attributes();
                AString displayStatusText = atts.value(s_tabInfoAttributeDisplayStatus).toString();
                const int32_t tabIndex = atts.value(s_tabInfoAttributeTabIndex).toInt(&tabIndexValid);
                const float   minX     = atts.value(s_tabInfoAttributeMinX).toFloat(&minXValid);
                const float   maxX     = atts.value(s_tabInfoAttributeMaxX).toFloat(&maxXValid);
                const float   minY     = atts.value(s_tabInfoAttributeMinY).toFloat(&minYValid);
                const float   maxY     = atts.value(s_tabInfoAttributeMaxY).toFloat(&maxYValid);
                const int32_t stacking = atts.value(s_tabInfoAttributeStackingOrder).toInt(&stackingValid);
                const QString backStr  = atts.value(s_tabInfoAttributeBackground).toString();
                const TileTabsLayoutBackgroundTypeEnum::Enum backType = TileTabsLayoutBackgroundTypeEnum::fromName(backStr, NULL);
                
                if (displayStatusText.isEmpty()) {
                    displayStatusText = "true";
                }
                const bool displayStatus = displayStatusText.toBool();
                
                if (tabIndexValid
                    && minXValid
                    && maxXValid
                    && minYValid
                    && maxYValid
                    && stackingValid) {
                    TileTabsBrowserTabGeometry* tabInfo = new TileTabsBrowserTabGeometry(tabIndex);
                    tabInfo->setMinX(minX);
                    tabInfo->setMaxX(maxX);
                    tabInfo->setMinY(minY);
                    tabInfo->setMaxY(maxY);
                    tabInfo->setDisplayed(displayStatus);
                    tabInfo->setStackingOrder(stacking);
                    tabInfo->setBackgroundType(backType);
                    
                    addTabInfo(tabInfo);
                }
                else {
                    CaretLogWarning("Failed to parse Manual Tile Tabs Configuration from Preferences: "
                                    + xml.tokenString());
                }
            }
            else {
                invalidElementNames.insert(elementName);
            }
        }
    }
}

/**
 * Cast to a manual configuration (avoids dynamic_cast that can be slow)
 *
 * @return Pointer to manual configuration or NULL if not a manual configuration.
 */
TileTabsLayoutManualConfiguration*
TileTabsLayoutManualConfiguration::castToManualConfiguration()
{
    return this;
}

/**
 * Cast to a manual configuration (avoids dynamic_cast that can be slow)
 *
 * @return Pointer to manual configuration or NULL if not a manual configuration.
 */
const TileTabsLayoutManualConfiguration*
TileTabsLayoutManualConfiguration::castToManualConfiguration() const
{
    return this;
}

