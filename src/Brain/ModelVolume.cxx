/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include "Brain.h"
#include "BrowserTabContent.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "OverlaySetArray.h"
#include "ModelVolume.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneEnumeratedType.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this volume controller belongs.
 *
 */
ModelVolume::ModelVolume(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES,
                         brain)
{
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    m_overlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_YES,
                                            "Volume View");
    m_lastVolumeFile = NULL;
    
    /*
     * Scene note: overlaySet is restored by parent class
     */ 
    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * Destructor
 */
ModelVolume::~ModelVolume()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_overlaySetArray;
    
    delete m_sceneAssistant;
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelVolume::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return "Volume";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelVolume::getNameForBrowserTab() const
{
    return "Volume";
}

/**
 * Get the bottom-most active volume in the given window tab.
 * If no overlay is set to volume data, one will be set to a 
 * volume if there is a volume loaded.
 * @param windowTabNumber 
 *    Tab number for content.
 * @return 
 *    Bottom-most volume or NULL if no volumes available.
 */
VolumeMappableInterface* 
ModelVolume::getUnderlayVolumeFile(const int32_t windowTabNumber) const
{
    OverlaySet* overlaySet = m_overlaySetArray->getOverlaySet(windowTabNumber);
    VolumeMappableInterface* vf = overlaySet->getUnderlayVolume();
    if (vf == NULL) {
        vf = overlaySet->setUnderlayToVolume();
    }
    
//    EventBrowserTabGet getBrowserTabEvent(windowTabNumber);
//    EventManager::get()->sendEvent(getBrowserTabEvent.getPointer());
//    BrowserTabContent* btc = getBrowserTabEvent.getBrowserTab();
//    if (btc != NULL) {
//        OverlaySet* overlaySet = btc->getOverlaySet();
//        vf = overlaySet->getUnderlayVolume();
//        if (vf == NULL) {
//            vf = overlaySet->setUnderlayToVolume();
//        }
//    }
    
    return vf;
}


/**
 * Update the controller.
 * @param windowTabNumber
 *    Tab number of window.
 */
void 
ModelVolume::updateController(const int32_t /*windowTabNumber*/)
{
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
ModelVolume::receiveEvent(Event* /*event*/)
{
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelVolume::getOverlaySet(const int tabIndex)
{
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelVolume::getOverlaySet(const int tabIndex) const
{
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Initilize the overlays for this controller.
 */
void 
ModelVolume::initializeOverlays()
{
    m_overlaySetArray->initializeOverlaySelections();
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param sceneClass
 *    SceneClass to which model specific information is added.
 */
void 
ModelVolume::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
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
ModelVolume::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
}

/**
 * @return A descriptive string describing the model.
 */
AString
ModelVolume::toDescriptiveString() const
{
    AString msg;
    
    msg.appendWithNewLine("Volume: ");
    
    return msg;
}

