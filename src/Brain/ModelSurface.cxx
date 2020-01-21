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

#include <algorithm>
#include <cmath>

#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "ModelSurface.h"

#include "Brain.h"
#include "BrainOpenGL.h"
#include "OverlaySet.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this model.
 *
 */
ModelSurface::ModelSurface(Brain* brain,
                           Surface* surface)
: Model(ModelTypeEnum::MODEL_TYPE_SURFACE,
                         brain)
{
    CaretAssert(surface);
    initializeMembersModelSurface();
    m_surface = surface;
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_MODEL_SURFACE_GET);
}

/**
 * Destructor
 */
ModelSurface::~ModelSurface()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
ModelSurface::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MODEL_SURFACE_GET) {
        EventModelSurfaceGet* getSurfaceEvent =
        dynamic_cast<EventModelSurfaceGet*>(event);
        CaretAssert(getSurfaceEvent);
        
        
        /*
         * Looking for this model?
         */
        if (getSurfaceEvent->getSurface() == getSurface()) {
            getSurfaceEvent->setModelSurface(this);
            getSurfaceEvent->setEventProcessed();
        }
    }
    else {     
        CaretAssertMessage(0, "Unexpected event: " + EventTypeEnum::toName(event->getEventType()));
    }
}

void
ModelSurface::initializeMembersModelSurface()
{
    m_surface = NULL;
}

/**
 * Get the surface in this model.
 * @return  Surface in this model.
 */
Surface*
ModelSurface::getSurface()
{
    return m_surface;
}

/**
 * Get the surface in this model.
 * @return  Surface in this model.
 */
const Surface*
ModelSurface::getSurface() const
{
    return m_surface;
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelSurface::getNameForGUI(const bool includeStructureFlag) const
{
    AString name;
    if (includeStructureFlag) {
        const StructureEnum::Enum structure = m_surface->getStructure();
        name += StructureEnum::toGuiName(structure);
        name += " ";
    }
    name += m_surface->getFileNameNoPath();
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelSurface::getNameForBrowserTab() const
{
    const StructureEnum::Enum structure = m_surface->getStructure();
    AString name = StructureEnum::toGuiName(structure);
    if (structure == StructureEnum::CEREBELLUM) {
        name = "Cbllm";
    }
    return name;
}

/**
 * Set the scaling so that the model fills the window.
 *
 */
//void
//ModelSurface::setDefaultScalingToFitWindow()
//{
//    BoundingBox bounds;
//    m_surface->getBounds(bounds);
//    
//    float bigY = std::max(std::abs(bounds.getMinY()), bounds.getMaxY());
//    float percentScreenY = BrainOpenGL::getModelViewingHalfWindowHeight() * 0.90f;
//    float scale = percentScreenY / bigY;
//    m_defaultModelScaling = scale;
//    
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        setScaling(i, m_defaultModelScaling);
//    }
//}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelSurface::getOverlaySet(const int tabIndex)
{
    if (m_surface != NULL) {
        BrainStructure* brainStructure = m_surface->getBrainStructure();
        if (brainStructure != NULL) {
            return brainStructure->getOverlaySet(tabIndex);
        }
    }
    
    return NULL;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelSurface::getOverlaySet(const int tabIndex) const
{
    if (m_surface != NULL) {
        const BrainStructure* brainStructure = m_surface->getBrainStructure();
        if (brainStructure != NULL) {
            return brainStructure->getOverlaySet(tabIndex);
        }
    }
    
    return NULL;
}

/**
 * Initilize the overlays for this model.
 */
void 
ModelSurface::initializeOverlays()
{
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
ModelSurface::saveModelSpecificInformationToScene(const SceneAttributes* /*sceneAttributes*/,
                                                      SceneClass* /*sceneClass*/)
{
    /* nothing to add to scene */
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
ModelSurface::restoreModelSpecificInformationFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                           const SceneClass* /*sceneClass*/)
{
    /* nothing to restore from scene */
}

/**
 * Get a text description of the window's content.
 *
 * @param tabIndex
 *    Index of the tab for content description.
 * @param descriptionOut
 *    Description of the window's content.
 */
void
ModelSurface::getDescriptionOfContent(const int32_t /*tabIndex*/,
                                      PlainTextStringBuilder& descriptionOut) const
{
    AString msg;
    
    const Surface* surface = getSurface();
    if (surface != NULL) {
        surface->getDescriptionOfContent(descriptionOut);
    }
}

/**
 * Copy the tab content from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ModelSurface::copyTabContent(const int32_t sourceTabIndex,
                      const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);
    
    /*
     * Fix WB-879 Overlay not copied when duplicating tab containing surface
     */
    getOverlaySet(destinationTabIndex)->copyOverlaySet(getOverlaySet(sourceTabIndex));
}


