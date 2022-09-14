
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

#define __IDENTIFICATION_MANAGER_DECLARE__
#include "IdentificationManager.h"
#undef __IDENTIFICATION_MANAGER_DECLARE__

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "IdentificationFilter.h"
#include "IdentificationHistoryManager.h"
#include "IdentificationHistoryRecord.h"
#include "IdentifiedItemNode.h"
#include "IdentifiedItemUniversal.h"
#include "IdentifiedItemVoxel.h"
#include "MathFunctions.h"
#include "SceneClassAssistant.h"
#include "SceneClass.h"
#include "ScenePrimitive.h"
#include "SelectionItem.h"
#include "SelectionItemUniversalIdentificationSymbol.h"
#include "SessionManager.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class caret::IdentificationManager 
 * \brief Manages identified items.
 */

/**
 * Constructor.
 *
 * @param caretPreferences
 *    The caret preferencers  
 */
IdentificationManager::IdentificationManager(const CaretPreferences* caretPreferences)
: SceneableInterface()
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_contralateralIdentificationEnabled = false;
    m_identificationSymbolColor = CaretColorEnum::WHITE;
    m_identificationContralateralSymbolColor = CaretColorEnum::LIME;
    m_identificationSymbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;
    m_identifcationSymbolSize = 3.0;
    m_identifcationMostRecentSymbolSize = 5.0;
    m_identifcationSymbolPercentageSize = 3.0;
    m_identifcationMostRecentSymbolPercentageSize = 5.0;
    m_histologyIdentificationPercentageSymbolSize = 3.0;
    m_histologyIdentificationMostRecentPercentageSymbolSize = 5.0;
    m_mediaIdentificationPercentageSymbolSize = 3.0;
    m_mediaIdentificationMostRecentPercentageSymbolSize = 5.0;
    m_showHistologyIdentificationSymbols = caretPreferences->isShowHistologyIdentificationSymbols();
    m_showMediaIdentificationSymbols = caretPreferences->isShowMediaIdentificationSymbols();
    m_showSurfaceIdentificationSymbols = caretPreferences->isShowSurfaceIdentificationSymbols();
    m_showVolumeIdentificationSymbols  = caretPreferences->isShowVolumeIdentificationSymbols();
    m_identificationFilter.reset(new IdentificationFilter());
    m_identificationHistoryManager.reset(new IdentificationHistoryManager());
    
    m_chartLineLayerSymbolSize         = 4.0;
    m_chartLineLayerToolTipTextSize    = 4.0;
    
    m_sceneAssistant->add("m_contralateralIdentificationEnabled",
                          &m_contralateralIdentificationEnabled);
    
    m_sceneAssistant->add<IdentificationSymbolSizeTypeEnum, IdentificationSymbolSizeTypeEnum::Enum>("m_identificationSymbolSizeType",
                                                                                                    &m_identificationSymbolSizeType);
    m_sceneAssistant->add("m_identifcationSymbolSize",
                          &m_identifcationSymbolSize);
    
    m_sceneAssistant->add("m_identifcationMostRecentSymbolSize",
                          &m_identifcationMostRecentSymbolSize);
    
    m_sceneAssistant->add("m_identifcationSymbolPercentageSize",
                          &m_identifcationSymbolPercentageSize);
    
    m_sceneAssistant->add("m_identifcationMostRecentSymbolPercentageSize",
                          &m_identifcationMostRecentSymbolPercentageSize);
    
    m_sceneAssistant->add("m_mediaIdentificationPercentageSymbolSize",
                          &m_mediaIdentificationPercentageSymbolSize);
    m_sceneAssistant->add("m_mediaIdentificationMostRecentPercentageSymbolSize",
                          &m_mediaIdentificationMostRecentPercentageSymbolSize);
    
    m_sceneAssistant->add("m_histologyIdentificationPercentageSymbolSize",
                          &m_histologyIdentificationPercentageSymbolSize);
    m_sceneAssistant->add("m_histologyIdentificationMostRecentPercentageSymbolSize",
                          &m_histologyIdentificationMostRecentPercentageSymbolSize);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationSymbolColor",
                                                                &m_identificationSymbolColor);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationContralateralSymbolColor",
                                                                &m_identificationContralateralSymbolColor);

    m_sceneAssistant->add("m_showHistologyIdentificationSymbols",
                          &m_showHistologyIdentificationSymbols);
    m_sceneAssistant->add("m_showMediaIdentificationSymbols",
                          &m_showMediaIdentificationSymbols);
    m_sceneAssistant->add("m_showSurfaceIdentificationSymbols",
                          &m_showSurfaceIdentificationSymbols);
    m_sceneAssistant->add("m_showVolumeIdentificationSymbols",
                          &m_showVolumeIdentificationSymbols);
    m_sceneAssistant->add("m_identificationFilter",
                          "IdentificationFilter",
                          m_identificationFilter.get());
    m_sceneAssistant->add("m_identificationHistoryManager",
                          "m_identificationHistoryManager",
                          m_identificationHistoryManager.get());
    
    m_sceneAssistant->add("m_chartLineLayerSymbolSize",
                          &m_chartLineLayerSymbolSize);
    m_sceneAssistant->add("m_chartLineLayerToolTipTextSize",
                          &m_chartLineLayerToolTipTextSize);
    
    removeAllIdentifiedItems();
}

/**
 * Destructor.
 */
IdentificationManager::~IdentificationManager()
{
    removeAllIdentifiedItems();
    
    delete m_sceneAssistant;
}

/**
 * Add an identified item.
 * @param item
 *    Identified item that is added.
 *    NOTE: Takes ownership of this item and will delete, at the appropriate time.
 */
void
IdentificationManager::addIdentifiedItem(IdentifiedItemUniversal* item)
{
    CaretAssert(item);

    const bool restoringSceneFlag(false);
    addIdentifiedItemPrivate(item,
                             restoringSceneFlag);
}

/**
 * Add an identified item.
 * @param item
 *    Identified item that is added.
 *    NOTE: Takes ownership of this item and will delete, at the appropriate time.
 * @param restoringSceneFlag
 *    If true, item is from a scene
 */
void
IdentificationManager::addIdentifiedItemPrivate(IdentifiedItemUniversal* item,
                                                const bool restoringSceneFlag)
{
    CaretAssert(item);
    m_mostRecentIdentifiedItem = item;
    
    std::unique_ptr<IdentifiedItemUniversal> ptr(item);
    m_identifiedItems.push_back(std::move(ptr));
    
    if ( ! restoringSceneFlag) {
        IdentificationHistoryRecord* historyRecord = new IdentificationHistoryRecord();
        historyRecord->setText(item->getFormattedText());
        m_identificationHistoryManager->addHistoryRecord(historyRecord);
    }
}

/**
 * @return String containing identification text for information window.
 */
AString
IdentificationManager::getIdentificationText() const
{
    AString text;
    
    for (auto& itemPtr : m_identifiedItems) {
        if ( ! text.isEmpty()) {
            text.append("<p></p>");
        }
        text.append(itemPtr->getSimpleText());
    }
    
    return text;
}

/**
 * Remove all identification text.  Node and voxels items have their text
 * removed and all other identification items are removed.
 */
void
IdentificationManager::removeIdentificationText()
{
    std::vector<IdentifiedItemUniversal*> itemsToKeepNew;

    for (auto& itemPtr : m_identifiedItems) {
        bool keepFlag(false);
        switch (itemPtr->getType()) {
            case IdentifiedItemUniversalTypeEnum::INVALID:
                break;
            case IdentifiedItemUniversalTypeEnum::HISTOLOGY_PLANE_COORDINATE:
                keepFlag = true;
                break;
            case IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE:
                keepFlag = true;
                break;
            case IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE:
                keepFlag = true;
                break;
            case IdentifiedItemUniversalTypeEnum::SURFACE:
                keepFlag = true;
                break;
            case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
                break;
            case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D:
                keepFlag = true;
                break;
            case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D:
                keepFlag = true;
                break;
            case IdentifiedItemUniversalTypeEnum::VOLUME_SLICES:
                keepFlag = true;
                break;
        }
        
        if (keepFlag) {
            /*
             * Keep the item so symbols remain but clear the text
             */
            itemPtr->clearAllText();
            itemsToKeepNew.push_back(itemPtr.release());
        }
    }
    
    m_identifiedItems.clear();
    for (auto& item : itemsToKeepNew) {
        std::unique_ptr<IdentifiedItemUniversal> ptr(item);
        m_identifiedItems.push_back(std::move(ptr));
    }
}

/**
 * Get the symbol size and color for an identified item
 * @param item
 *    The item
 * @param drawingOnType
 *    Type of model on which symbol is drawn
 * @param referenceHeight
 *    Height for items that are sized as percentage
 * @param contralateralSurfaceFlag
 *    True if getting size and color for a contralateral surface
 * @param rgbaOut
 *    Output with RGBA components (if [3] == 0, do not draw)
 * @param symbolSizeOut
 *    Output with size for symbol
 */
void
IdentificationManager::getIdentifiedItemColorAndSize(const IdentifiedItemUniversal* item,
                                                     const IdentifiedItemUniversalTypeEnum::Enum drawingOnType,
                                                     const float referenceHeight,
                                                     const bool contralateralSurfaceFlag,
                                                     std::array<uint8_t, 4>& rgbaOut,
                                                     float& symbolDiameterOut) const
{
    CaretAssert(item);

    rgbaOut.fill(0);
    symbolDiameterOut = 5.0;
    
    IdentificationSymbolSizeTypeEnum::Enum sizeType(getIdentificationSymbolSizeType());

    bool histologyPlaneCoordFlag(false);
    bool mediaLogicalCoordFlag(false);
    bool mediaPlaneCoordFlag(false);
    switch (drawingOnType) {
        case IdentifiedItemUniversalTypeEnum::INVALID:
            CaretAssert(0);
            return;
            break;
        case IdentifiedItemUniversalTypeEnum::HISTOLOGY_PLANE_COORDINATE:
            histologyPlaneCoordFlag = true;
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE:
            mediaLogicalCoordFlag = true;
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE:
            mediaPlaneCoordFlag = true;
            break;
        case IdentifiedItemUniversalTypeEnum::SURFACE:
            break;
        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
            CaretAssert(0);
            return;
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D:
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D:
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_SLICES:
            break;
    }
    
    if (histologyPlaneCoordFlag) {
        /*
         * Media is always percentage
         */
        sizeType = IdentificationSymbolSizeTypeEnum::PERCENTAGE;
        symbolDiameterOut = getHistologyIdentificationPercentageSymbolSize();
        if (item == m_mostRecentIdentifiedItem) {
            symbolDiameterOut = getHistologyIdentificationMostRecentPercentageSymbolSize();
        }
        symbolDiameterOut = referenceHeight * (symbolDiameterOut / 100.0);
    }
    else if (mediaLogicalCoordFlag
        || mediaPlaneCoordFlag) {
        /*
         * Media is always percentage
         */
        sizeType = IdentificationSymbolSizeTypeEnum::PERCENTAGE;
        symbolDiameterOut = getMediaIdentificationPercentageSymbolSize();
        if (item == m_mostRecentIdentifiedItem) {
            symbolDiameterOut = getMediaIdentificationMostRecentPercentageSymbolSize();
        }
        symbolDiameterOut = referenceHeight * (symbolDiameterOut / 100.0);
    }
    else {
        switch (sizeType) {
            case IdentificationSymbolSizeTypeEnum::MILLIMETERS:
                symbolDiameterOut = getIdentificationSymbolSize();
                if (item == m_mostRecentIdentifiedItem) {
                    symbolDiameterOut = getMostRecentIdentificationSymbolSize();
                }
                break;
            case IdentificationSymbolSizeTypeEnum::PERCENTAGE:
                symbolDiameterOut = getIdentificationSymbolPercentageSize();
                if (item == m_mostRecentIdentifiedItem) {
                    symbolDiameterOut = getMostRecentIdentificationSymbolPercentageSize();
                }
                symbolDiameterOut = referenceHeight * (symbolDiameterOut / 100.0);
                break;
        }
    }

    const CaretColorEnum::Enum caretColor = (contralateralSurfaceFlag
                                             ? getIdentificationContralateralSymbolColor()
                                             : getIdentificationSymbolColor());
    CaretColorEnum::toRGBAByte(caretColor, rgbaOut.data());
}

/**
 * @return All of the identified items
 */
std::vector<const IdentifiedItemUniversal*>
IdentificationManager::getIdentifiedItems() const
{
    std::vector<const IdentifiedItemUniversal*> itemsOut;
    
    for (auto& itemPtr : m_identifiedItems) {
        itemsOut.push_back(itemPtr.get());
    }
    
    return itemsOut;
}

/**
 * @return Identified item with the given unique identifier or NULL if not found
 * @param uniqueIdentifier
 *    Unique identifier of the item
 */
const IdentifiedItemUniversal*
IdentificationManager::getIdentifiedItemWithIdentifier(const int64_t uniqueIdentifier) const
{
    for (const auto& ptr : m_identifiedItems) {
        if (ptr->getUniqueIdentifier() == uniqueIdentifier) {
            return ptr.get();
        }
    }
    return NULL;
}

/**
 * Remove surface symbol
 * @param selectedItem
 *    Info about surface symbol that is to be removed
 * @return True if identified item was found and removed, else false.
 */
bool
IdentificationManager::removeIdentifiedItem(const SelectionItem* selectedItem)
{
    CaretAssert(selectedItem);
    const int64_t uniqueID(selectedItem->getIdentifiedItemUniqueIdentifier());
    
    if (uniqueID >= 0) {
        for (auto iter = m_identifiedItems.begin();
             iter != m_identifiedItems.end();
             iter++) {
            auto& item = *iter;
            if (item->getUniqueIdentifier() == uniqueID) {
                m_identifiedItems.erase(iter);
                return true;
            }
        }
    }
    
    CaretLogSevere("Failed to remove identified item: "
                   + selectedItem->toString());
    
    return false;
}

/**
 * Remove all identified items.
 */
void
IdentificationManager::removeAllIdentifiedItems()
{
    m_identificationHistoryManager->clearHistory();
    
    m_identifiedItems.clear();
    
    m_mostRecentIdentifiedItem = NULL;
}

/**
 * Remove all identification symbols while preserving text.
 *
 * Text from identification symbols for surface or volume are 
 * inserted into new identified items and the symbol items
 * are removed.
 */
void
IdentificationManager::removeAllIdentifiedSymbols()
{
    m_identifiedItems.clear();
}

/**
 * @return Status of contralateral identification.
 */
bool
IdentificationManager::isContralateralIdentificationEnabled() const
{
    return m_contralateralIdentificationEnabled;
}

/**
 * Set status of contralateral identification.
 * @param
 *    New status.
 */
void
IdentificationManager::setContralateralIdentificationEnabled(const bool enabled)
{
    m_contralateralIdentificationEnabled = enabled;
}

/**
 * @param The identification symbol size type
 */
IdentificationSymbolSizeTypeEnum::Enum
IdentificationManager::getIdentificationSymbolSizeType() const
{
    return m_identificationSymbolSizeType;
}

/**
 * Set the identification size type
 * @param sizeType
 *    The new size type
 */
void
IdentificationManager::setIdentificationSymbolSizeType(const IdentificationSymbolSizeTypeEnum::Enum sizeType)
{
    m_identificationSymbolSizeType = sizeType;
}

/**
 * @return The size of the identification symbol
 */
float
IdentificationManager::getIdentificationSymbolSize() const
{
    return m_identifcationSymbolSize;
}

/**
 * Set the size of the identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setIdentificationSymbolSize(const float symbolSize)
{
    m_identifcationSymbolSize = symbolSize;
}

/**
 * @return The size of the most recent identification symbol
 */
float
IdentificationManager::getMostRecentIdentificationSymbolSize() const
{
    return m_identifcationMostRecentSymbolSize;
}

/**
 * Set the size of the most recent identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setMostRecentIdentificationSymbolSize(const float symbolSize)
{
    m_identifcationMostRecentSymbolSize = symbolSize;
}

/**
 * @return The percentage size of the identification symbol
 */
float
IdentificationManager::getIdentificationSymbolPercentageSize() const
{
    return m_identifcationSymbolPercentageSize;
}

/**
 * Set the percentage size of the identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setIdentificationSymbolPercentageSize(const float symbolSize)
{
    m_identifcationSymbolPercentageSize = symbolSize;
}

/**
 * @return The percentage size of the most recent identification symbol
 */
float
IdentificationManager::getMostRecentIdentificationSymbolPercentageSize() const
{
    return m_identifcationMostRecentSymbolPercentageSize;
}

/**
 * Set the percentage size of the most recent identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setMostRecentIdentificationSymbolPercentageSize(const float symbolSize)
{
    m_identifcationMostRecentSymbolPercentageSize = symbolSize;
}

/**
 * @return The percentage size of the most recent histology identification symbol
 */
float
IdentificationManager::getHistologyIdentificationPercentageSymbolSize() const
{
    return m_histologyIdentificationPercentageSymbolSize;
}

/**
 * Set the percentage size of the most recent histology identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setHistologyIdentificationPercentageSymbolSize(const float symbolSize)
{
    m_histologyIdentificationPercentageSymbolSize = symbolSize;
}

/**
 * @return The percentage size of the most recent histology identification symbol
 */
float
IdentificationManager::getHistologyIdentificationMostRecentPercentageSymbolSize() const
{
    return m_histologyIdentificationMostRecentPercentageSymbolSize;
}

/**
 * Set the percentage size of the most recent histology identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setHistologyIdentificationMostRecentPercentageSymbolSize(const float symbolSize)
{
    m_histologyIdentificationMostRecentPercentageSymbolSize = symbolSize;
}

/**
 * @return The percentage size of the most recent media identification symbol
 */
float
IdentificationManager::getMediaIdentificationPercentageSymbolSize() const
{
    return m_mediaIdentificationPercentageSymbolSize;
}

/**
 * Set the percentage size of the most recent media identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setMediaIdentificationPercentageSymbolSize(const float symbolSize)
{
    m_mediaIdentificationPercentageSymbolSize = symbolSize;
}

/**
 * @return The percentage size of the most recent media identification symbol
 */
float
IdentificationManager::getMediaIdentificationMostRecentPercentageSymbolSize() const
{
    return m_mediaIdentificationMostRecentPercentageSymbolSize;
}

/**
 * Set the percentage size of the most recent media identification symbol
 * @param symbolSize
 *    New size of symbol.
 */
void
IdentificationManager::setMediaIdentificationMostRecentPercentageSymbolSize(const float symbolSize)
{
    m_mediaIdentificationMostRecentPercentageSymbolSize = symbolSize;
}

/**
 * @return The color of the identification symbol.
 */
CaretColorEnum::Enum
IdentificationManager::getIdentificationSymbolColor() const
{
    return m_identificationSymbolColor;
}

/**
 * Set the color of the identification symbol.
 * @param color
 *    New color.
 */
void
IdentificationManager::setIdentificationSymbolColor(const CaretColorEnum::Enum color)
{
    m_identificationSymbolColor = color;
}

/**
 * @return The color of the contralateral identification symbol.
 */
CaretColorEnum::Enum
IdentificationManager::getIdentificationContralateralSymbolColor() const
{
    return m_identificationContralateralSymbolColor;
}

/**
 * Set the color of the contralateral identification symbol.
 * @param color
 *    New color.
 */
void
IdentificationManager::setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color)
{
    m_identificationContralateralSymbolColor = color;
}

/**
 * @return show histology identification symbols
 */
bool
IdentificationManager::isShowHistologyIdentificationSymbols() const
{
    return m_showHistologyIdentificationSymbols;
}

/**
 * Set show histology identification symbols
 *
 * @param showHistologyIdentificationSymbols
 *    New value for show histology identification symbols
 */
void
IdentificationManager::setShowHistologyIdentificationSymbols(const bool showHistologyIdentificationSymbols)
{
    m_showHistologyIdentificationSymbols = showHistologyIdentificationSymbols;
}

/**
 * @return show media identification symbols
 */
bool
IdentificationManager::isShowMediaIdentificationSymbols() const
{
    return m_showMediaIdentificationSymbols;
}

/**
 * Set show media identification symbols
 *
 * @param showMediaIdentificationSymbols
 *    New value for show media identification symbols
 */
void
IdentificationManager::setShowMediaIdentificationSymbols(const bool showMediaIdenficationSymbols)
{
    m_showMediaIdentificationSymbols = showMediaIdenficationSymbols;
}

/**
 * @return show surface identification symbols
 */
bool
IdentificationManager::isShowSurfaceIdentificationSymbols() const
{
    return m_showSurfaceIdentificationSymbols;
}

/**
 * Set show surface identification symbols
 *
 * @param showSurfaceIdentificationSymbols
 *    New value for show surface identification symbols
 */
void
IdentificationManager::setShowSurfaceIdentificationSymbols(const bool showSurfaceIdentificationSymbols)
{
    m_showSurfaceIdentificationSymbols = showSurfaceIdentificationSymbols;
}

/**
 * @return show volume identification symbols
 */
bool
IdentificationManager::isShowVolumeIdentificationSymbols() const
{
    return m_showVolumeIdentificationSymbols;
}

/**
 * Set show volume identification symbols
 *
 * @param showVolumeIdentificationSymbols
 *    New value for show volume identification symbols
 */
void
IdentificationManager::setShowVolumeIdentificationSymbols(const bool showVolumeIdentificationSymbols)
{
    m_showVolumeIdentificationSymbols = showVolumeIdentificationSymbols;
}

/**
 * @return Size for chart line layer symbols, except selected layer (percentage of viewport height)
 */
float
IdentificationManager::getChartLineLayerSymbolSize() const
{
    return m_chartLineLayerSymbolSize;
}

/**
 * Set size for chart line layer symbols, except selected layer (percentage of viewport height)
 * @param symbolSize
 *    New size for symbol
 */
void
IdentificationManager::setChartLineLayerSymbolSize(const float symbolSize)
{
    m_chartLineLayerSymbolSize = symbolSize;
}

/**
 * @return Size for chart line layer tooltip text (percentage of viewport height)
 */
float
IdentificationManager::getChartLineLayerToolTipTextSize() const
{
    return m_chartLineLayerToolTipTextSize;
}

/**
 * Set size for chart line layer tooltip text (percentage of viewport height)
 * @param textSize
 *    New size for text
 */
void
IdentificationManager::setChartLineLayerToolTipTextSize(const float textSize)
{
    m_chartLineLayerToolTipTextSize = textSize;
}

/**
 * Get surface information for an identification symbol
 * @param symbol
 *     The identication symbol
 * @param structureOut
 *     Output with structure of surface
 * @param surfaceNumberOfVerticesOut
 *     Output with number of vertices in surface
 * @param surfaceVertexIndexOut
 *     Output with index of the vertex
 * @return
 *     True if the identification is for a surface vertex and the output information is valid
 */
bool
IdentificationManager::getSurfaceInformationForIdentificationSymbol(const SelectionItemUniversalIdentificationSymbol* symbol,
                                                                    StructureEnum::Enum& structureOut,
                                                                    int32_t& surfaceNumberOfVerticesOut,
                                                                    int32_t& surfaceVertexIndexOut) const
{
    CaretAssert(symbol);
    structureOut = StructureEnum::INVALID;
    surfaceNumberOfVerticesOut = -1;
    surfaceVertexIndexOut = -1;
    
    const int64_t uniqueID(symbol->getIdentifiedItemUniqueIdentifier());
    const IdentifiedItemUniversal* idItem(getIdentifiedItemWithIdentifier(uniqueID));
    if (idItem != NULL) {
        if (idItem->getType() == IdentifiedItemUniversalTypeEnum::SURFACE) {
            structureOut = idItem->getStructure();
            surfaceNumberOfVerticesOut = idItem->getSurfaceNumberOfVertices();
            surfaceVertexIndexOut = idItem->getSurfaceVertexIndex();
            return true;
        }
    }
    
    return false;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
IdentificationManager::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    /*
     * Note: version 2 added separate symbol sizes for media
     */
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentificationManager",
                                            2);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    for (auto& itemPtr : m_identifiedItems) {
        IdentifiedItemUniversal* item(itemPtr.get());
        sceneClass->addClass(item->saveToScene(sceneAttributes,
                                               "identifiedItemUniversal"));
    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
IdentificationManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    std::list<IdentifiedItemBase*> oldIdentifiedItems;
    
    m_identificationSymbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;
    removeAllIdentifiedItems();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    const bool restoringSceneFlag(true);
    const int32_t numChildren = sceneClass->getNumberOfObjects();
    for (int32_t i = 0; i < numChildren; i++) {
        const SceneObject* so = sceneClass->getObjectAtIndex(i);
        if (so->getName() == "identifiedItemUniversal") {
            IdentifiedItemUniversal* item(IdentifiedItemUniversal::newInstanceInvalidIdentification());
            const SceneClass* sc = dynamic_cast<const SceneClass*>(so);
            CaretAssert(sc);
            item->restoreFromScene(sceneAttributes, sc);
            CaretAssert(item->isValid());
            addIdentifiedItemPrivate(item,
                                     restoringSceneFlag);
        }
        else if (so->getName() == "identifiedItem") {
            const SceneClass* sc = dynamic_cast<const SceneClass*>(so);
            if (sc != NULL) {
                const AString className = sc->getClassName();
                if (className == "IdentifiedItem") {
                    IdentifiedItemBase* item = new IdentifiedItemBase();
                    item->restoreFromScene(sceneAttributes, sc);
                    if (item->isValid()) {
                        oldIdentifiedItems.push_back(item);
                    }
                    else {
                        delete item;
                    }
                }
                else if (className == "IdentifiedItemNode") {
                    IdentifiedItemNode* item = new IdentifiedItemNode();
                    item->restoreFromScene(sceneAttributes, sc);
                    if (item->isValid()) {
                        oldIdentifiedItems.push_back(item);
                    }
                    else {
                        delete item;
                    }
                }
                else if (className == "IdentifiedItemVoxel") {
                    IdentifiedItemVoxel* item = new IdentifiedItemVoxel();
                    item->restoreFromScene(sceneAttributes, sc);
                    if (item->isValid()) {
                        oldIdentifiedItems.push_back(item);
                    }
                    else {
                        delete item;
                    }
                }
                else {
                    const AString msg = ("IdentifiedItem from scene is invalid.  "
                                         "Has a new IdentifiedItem type been added?  "
                                         "Class name=" + className);
                    CaretAssertMessage(0,
                                       msg);
                    CaretLogSevere(msg);
                }
            }
        }
    }
    
    /*
     * "m_volumeIdentificationEnabled" was removed when volume identification
     * was made a tab property.  If this item is present in the scene,
     * update volume ID status in all tabs.
     */
    const ScenePrimitive* idPrimitive = sceneClass->getPrimitive("m_volumeIdentificationEnabled");
    if (idPrimitive != NULL) {
        const bool volumeID = sceneClass->getBooleanValue("m_volumeIdentificationEnabled");
        
        EventBrowserTabGetAll allTabs;
        EventManager::get()->sendEvent(allTabs.getPointer());
        std::vector<BrowserTabContent*> tabContent = allTabs.getAllBrowserTabs();
        
        for (std::vector<BrowserTabContent*>::iterator iter = tabContent.begin();
             iter != tabContent.end();
             iter++) {
            BrowserTabContent* btc = *iter;
            btc->setIdentificationUpdatesVolumeSlices(volumeID);
        }
    }
    
    /*
     * Move all of the old identified items to the new system.
     * Also delete all old identified items.
     */
    for (auto* oldItem : oldIdentifiedItems) {
        IdentifiedItemUniversal* newItem = IdentifiedItemUniversal::newInstanceFromOldIdentification(oldItem);
        if (newItem != NULL) {
            addIdentifiedItemPrivate(newItem,
                                     restoringSceneFlag);
        }
        delete oldItem;
    }
    
    if (sceneClass->getVersionNumber() < 2) {
        /*
         * In version 1, media used the generic symbol percentage sizes.  
         * Media unique sizes added in version 2.
         */
        m_mediaIdentificationPercentageSymbolSize           = m_identifcationSymbolPercentageSize;
        m_mediaIdentificationMostRecentPercentageSymbolSize = m_identifcationMostRecentSymbolPercentageSize;
    }
}

/**
 * @return Pointer to the identification filter (const method)
 */
const IdentificationFilter*
IdentificationManager::getIdentificationFilter() const
{
    return m_identificationFilter.get();
}

/**
 * @return Pointer to the identification filter
 */
IdentificationFilter*
IdentificationManager::getIdentificationFilter()
{
    return m_identificationFilter.get();
}

/**
 * @return Pointer to the identification history manager
 */
const IdentificationHistoryManager*
IdentificationManager::getIdentificationHistoryManager() const
{
    return m_identificationHistoryManager.get();
}

/**
 * @return Pointer to the identification history manager
 */
IdentificationHistoryManager*
IdentificationManager::getIdentificationHistoryManager()
{
    return m_identificationHistoryManager.get();
}

/**
 * @return Text for "show symbol on <type>"  checkbox
 * @param type
 *    Indentification type
 */
AString
IdentificationManager::getShowSymbolOnTypeLabel(const IdentifiedItemUniversalTypeEnum::Enum type)
{
    AString text;
    
    switch (type) {
        case IdentifiedItemUniversalTypeEnum::INVALID:
            text = "INVALID";
            break;
        case IdentifiedItemUniversalTypeEnum::HISTOLOGY_PLANE_COORDINATE:
            text = "Show ID Symbols on Histology Slices";
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE:
            text = "Show ID Symbols on Media Logical Coordinates";
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE:
            text = "Show ID Symbols on Media Plane Coordinates";
            break;
        case IdentifiedItemUniversalTypeEnum::SURFACE:
            text = "Show ID Symbols on Surface";
            break;
        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
            text = "TEXT";
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D:
            CaretAssertMessage(0, "Use Volume Slices for Intensity 2D");
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D:
            CaretAssertMessage(0, "Use Volume Slices for Intensity 3D");
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_SLICES:
            text = "Show ID Symbols on Volume";
            break;
    }
    
    return text;
}

/**
 * @return Text for "show symbol on <type>"  tooltip
 * @param type
 *    Indentification type
 */
AString
IdentificationManager::getShowSymbolOnTypeToolTip(const IdentifiedItemUniversalTypeEnum::Enum type)
{
    AString text("<html>");
    text.append(getShowSymbolOnTypeLabel(type) + "; ");
    text.append("Symbol may have been created on another type ");
    switch (type) {
        case IdentifiedItemUniversalTypeEnum::INVALID:
            break;
        case IdentifiedItemUniversalTypeEnum::HISTOLOGY_PLANE_COORDINATE:
            text.append("(Surface, Volume, or other Media)");
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE:
            text.append("(Surface, Volume, or other Media)");
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE:
            text.append("(Surface, Volume, or other Media)");
            break;
        case IdentifiedItemUniversalTypeEnum::SURFACE:
            text.append("(Media, Volume)");
            break;
        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D:
            CaretAssertMessage(0, "Use Volume Slices for Intensity 2D ToolTip");
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D:
            CaretAssertMessage(0, "Use Volume Slices for Intensity 3D ToolTip");
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_SLICES:
            text.append("(Media, Surface)");
            break;
    }
    text.append("</html>");
    
    return text;
}

