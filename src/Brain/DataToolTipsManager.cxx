
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __DATA_TOOL_TIPS_MANAGER_DECLARE__
#include "DataToolTipsManager.h"
#undef __DATA_TOOL_TIPS_MANAGER_DECLARE__

#include "Brain.h"
#include "Border.h"
#include "BorderFile.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CiftiMappableDataFile.h"
#include "EventManager.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabelTable.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class caret::DataToolTipsManager 
 * \brief Manages Data ToolTips.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
DataToolTipsManager::DataToolTipsManager()
: CaretObject()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
DataToolTipsManager::~DataToolTipsManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get text for the tooltip.
 *
 * @param brain
 *     The Brain.
 * @param browserTab
 *     Browser tab in which tooltip is displayed
 * @param selectionManager
 *     The selection manager.
 */
AString
DataToolTipsManager::getToolTip(const Brain* brain,
                                const BrowserTabContent* browserTab,
                                const SelectionManager* selectionManager) const
{
    CaretAssert(brain);
    CaretAssert(browserTab);
    CaretAssert(selectionManager);
    
    AString text;
    
    const SelectionItemSurfaceNode* selectedNode = selectionManager->getSurfaceNodeIdentification();
    CaretAssert(selectedNode);
    
    if (selectedNode->isValid()) {
        text = getSurfaceToolTip(brain,
                                 browserTab,
                                 selectionManager,
                                 selectedNode);
    }
    else {
        const SelectionItemVoxel* selectedVoxel = selectionManager->getVoxelIdentification();
        if (selectedVoxel->isValid()) {
            text = getVolumeToolTip(brain,
                                    browserTab,
                                    selectionManager,
                                    selectedVoxel);
        }
    }
    
    return text;
}

/**
 * Get text for the tooltip for a selected node.
 *
 * @param brain
 *     The Brain.
 * @param browserTab
 *     Browser tab in which tooltip is displayed
 * @param selectionManager
 *     The selection manager.
 * @param nodeSelection
 *     Node selection information.
 */
AString
DataToolTipsManager::getSurfaceToolTip(const Brain* brain,
                                       const BrowserTabContent* browserTab,
                                       const SelectionManager* selectionManager,
                                       const SelectionItemSurfaceNode* nodeSelection) const
{
    AString text;
    
    const Surface* surface = nodeSelection->getSurface();
    CaretAssert(surface);
    int32_t surfaceNumberOfNodes = surface->getNumberOfNodes();
    int32_t surfaceNodeIndex = nodeSelection->getNodeNumber();
    StructureEnum::Enum surfaceStructure = surface->getStructure();
    
    if ((surfaceStructure != StructureEnum::INVALID)
        && (surfaceNumberOfNodes > 0)
        && (surfaceNodeIndex >= 0)) {
        
        bool showSurfaceFlag = m_showSurfaceViewedFlag;
        if (m_showSurfacePrimaryAnatomicalFlag) {
            const Surface* anatSurface = brain->getPrimaryAnatomicalSurfaceForStructure(surfaceStructure);
            if (anatSurface != NULL) {
                if (anatSurface->getNumberOfNodes() == surfaceNumberOfNodes) {
                    float xyz[3];
                    anatSurface->getCoordinate(surfaceNodeIndex,
                                               xyz);
                    text.appendWithNewLine("Anatomy Surface: "
                                           + AString::fromNumbers(xyz, 3, ", "));
                    if (surface == anatSurface) {
                        showSurfaceFlag = false;
                    }
                }
            }
        }
        
        if (showSurfaceFlag) {
            float xyz[3];
            surface->getCoordinate(surfaceNodeIndex,
                                   xyz);
            text.appendWithNewLine("Surface: "
                                   + AString::fromNumbers(xyz, 3, ", "));
        }
        
        if (m_showTopLayerFlag) {
            const OverlaySet* overlaySet = browserTab->getOverlaySet();
            CaretAssert(overlaySet);
            Overlay* overlay = const_cast<Overlay*>(overlaySet->getOverlay(0));
            CaretAssert(overlay);
            CaretMappableDataFile* mapFile(NULL);
            int32_t mapIndex(-1);
            overlay->getSelectionData(mapFile,
                                      mapIndex);
            if ((mapFile != NULL)
                && (mapIndex >= 0)) {
                std::vector<int32_t> mapIndices { mapIndex };
                AString textValue;
                mapFile->getSurfaceNodeIdentificationForMaps(mapIndices,
                                                             surfaceStructure,
                                                             surfaceNodeIndex,
                                                             surfaceNumberOfNodes,
                                                             textValue);
                if ( ! textValue.isEmpty()) {
                    text.appendWithNewLine("Top Layer: "
                                           + textValue);
                }
            }
        }
    }
    
    if (m_showBorderFlag) {
        const SelectionItemBorderSurface* borderSelection = selectionManager->getSurfaceBorderIdentification();
        CaretAssert(borderSelection);
        const BorderFile* borderFile = borderSelection->getBorderFile();
        const int32_t borderIndex    = borderSelection->getBorderIndex();
        if ((borderFile != NULL)
            && (borderIndex >= 0)) {
            const Border* border = borderFile->getBorder(borderIndex);
            if (border != NULL) {
                text.appendWithNewLine("Border: "
                                       + border->getName());
            }
        }
    }
    
    if (m_showFocusFlag) {
        const SelectionItemFocusSurface* focusSelection = selectionManager->getSurfaceFocusIdentification();
        CaretAssert(focusSelection);
        const FociFile* fociFile = focusSelection->getFociFile();
        const int32_t focusIndex = focusSelection->getFocusIndex();
        if ((fociFile != NULL)
            && (focusIndex >= 0)) {
            const Focus* focus = fociFile->getFocus(focusIndex);
            if (focus != NULL) {
                text.appendWithNewLine("Focus: "
                                       + focus->getName());
            }
        }
    }
    
    return text;
}

/**
 * Get text for the tooltip for a selected node.
 *
 * @param brain
 *     The Brain.
 * @param browserTab
 *     Browser tab in which tooltip is displayed
 * @param selectionManager
 *     The selection manager.
 * @param voxelSelection
 *     Voxel selection information.
 */
AString
DataToolTipsManager::getVolumeToolTip(const Brain* /*brain*/,
                                      const BrowserTabContent* browserTab,
                                      const SelectionManager* /*selectionManager*/,
                                      const SelectionItemVoxel* voxelSelection) const
{
    AString text;
    
    const VolumeMappableInterface* selectedVolumeInterface = voxelSelection->getVolumeFile();
    AString filename;
    {
        const VolumeFile* volumeFile = dynamic_cast<const VolumeFile*>(selectedVolumeInterface);
        if (volumeFile != NULL) {
            filename = volumeFile->getFileNameNoPath();
        }
    }
    {
        const CiftiMappableDataFile* ciftiMapFile = dynamic_cast<const CiftiMappableDataFile*>(selectedVolumeInterface);
        if (ciftiMapFile != NULL) {
            filename = ciftiMapFile->getFileNameNoPath();
        }
    }
    double modelXYZ[3];
    voxelSelection->getModelXYZ(modelXYZ);
    const float floatXYZ[3] {
        static_cast<float>(modelXYZ[0]),
        static_cast<float>(modelXYZ[1]),
        static_cast<float>(modelXYZ[2])
    };
    int64_t ijk[3];
    voxelSelection->getVoxelIJK(ijk);
    
    text.appendWithNewLine("Voxel IJK: "
                           + AString::fromNumbers(ijk, 3, ", ")
                           + " XYZ: "
                           + AString::fromNumbers(modelXYZ, 3, ", ")
                           + " "
                           + filename);
    const float value = selectedVolumeInterface->getVoxelValue(floatXYZ);
    text.appendWithNewLine("   Value: "
                           + AString::number(value, 'f'));
    
    if (m_showTopLayerFlag) {
        const OverlaySet* overlaySet = browserTab->getOverlaySet();
        CaretAssert(overlaySet);
        Overlay* overlay = const_cast<Overlay*>(overlaySet->getOverlay(0));
        CaretAssert(overlay);
        CaretMappableDataFile* mapFile(NULL);
        int32_t mapIndex(-1);
        overlay->getSelectionData(mapFile,
                                  mapIndex);
        if ((mapFile != NULL)
            && (mapIndex >= 0)) {
            std::vector<int32_t> mapIndices { mapIndex };
            AString textValue;
            int64_t ijk[3];
            mapFile->getVolumeVoxelIdentificationForMaps(mapIndices,
                                                         floatXYZ,
                                                         ijk,
                                                         textValue);
            if ( ! textValue.isEmpty()) {
                text.appendWithNewLine("Top Layer: "
                                       + textValue);
            }
        }
    }
    return text;
}


/**
 * @return Is tips enabled?
 */
bool
DataToolTipsManager::isEnabled() const
{
    return m_enabledFlag;
}

/**
 * Set status for tips enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setEnabled(const bool status)
{
    m_enabledFlag = status;
}

/**
 * @return Is show primary anatomical surface enabled?
 */
bool
DataToolTipsManager::isShowSurfacePrimaryAnatomical() const
{
    return m_showSurfacePrimaryAnatomicalFlag;
}

/**
 * Set status for primary anatomical surface enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowSurfacePrimaryAnatomical(const bool status)
{
    m_showSurfacePrimaryAnatomicalFlag = status;
}

/**
 * @return Is show viewed surface enabled?
 */
bool
DataToolTipsManager::isShowSurfaceViewed() const
{
    return m_showSurfaceViewedFlag;
}

/**
 * Set status for viewed surface enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowSurfaceViewed(const bool status)
{
    m_showSurfaceViewedFlag = status;
}

/**
 * @return Is show top layer enabled?
 */
bool
DataToolTipsManager::isShowTopLayer() const
{
    return m_showTopLayerFlag;
}

/**
 * Set status for top layer enabled
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowTopLayer(const bool status)
{
    m_showTopLayerFlag = status;
}

/**
 * @return Is show border enabled?
 */
bool
DataToolTipsManager::isShowBorder() const
{
    return m_showBorderFlag;
}

/**
 * Set status for show border
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowBorder(const bool status)
{
    m_showBorderFlag = status;
}

/**
 * @return Is show focus enabled?
 */
bool
DataToolTipsManager::isShowFocus() const
{
    return m_showFocusFlag;
}

/**
 * Set status for show focus
 *
 * @param status
 *     New status.
 */
void
DataToolTipsManager::setShowFocus(const bool status)
{
    m_showFocusFlag = status;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
DataToolTipsManager::toString() const
{
    return "DataToolTipsManager";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
DataToolTipsManager::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
DataToolTipsManager::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DataToolTipsManager",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
DataToolTipsManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

