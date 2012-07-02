
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__
#include "ModelSurfaceSelector.h"
#undef __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "ModelSurface.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include <algorithm>

using namespace caret;

/**
 * \class ModelSurfaceSelector
 * \brief Maintains selection of surface controller
 *
 * Maintains selection of a surface controller with the ability to limit the
 * surface controllers to those from a specific brain structure.
 */

/**
 * Constructor.
 */
ModelSurfaceSelector::ModelSurfaceSelector()
: CaretObject()
{
    m_defaultStructure = StructureEnum::ALL;
    m_selectedStructure = StructureEnum::ALL;
    m_selectedSurfaceController = NULL;
}

/**
 * Destructor.
 */
ModelSurfaceSelector::~ModelSurfaceSelector()
{
    
}

/**
 * @return The selected surface controller.
 */
ModelSurface* 
ModelSurfaceSelector::getSelectedSurfaceController()
{
    this->updateSelector();
    return m_selectedSurfaceController;
}

/**
 * @return The selected structure.
 */
StructureEnum::Enum 
ModelSurfaceSelector::getSelectedStructure()
{
    this->updateSelector();
    return m_selectedStructure;
}

/**
 * Set the selected surface controller.
 * 
 * @param surfaceController
 *    Controller that is selected.
 */
void 
ModelSurfaceSelector::setSelectedSurfaceController(
                                                               ModelSurface* surfaceController)
{
    m_selectedSurfaceController = surfaceController;
    if (m_selectedStructure != StructureEnum::ALL) {
        m_selectedStructure = surfaceController->getSurface()->getStructure();
    }
    this->updateSelector();
}

/**
 * Set the selected structure.
 * 
 * @param selectedStructure
 *    Structure that is selected.
 */
void 
ModelSurfaceSelector::setSelectedStructure(const StructureEnum::Enum selectedStructure)
{
    m_selectedStructure = selectedStructure;
    this->updateSelector();
}

/**
 * Get the structures available for selection.
 * 
 * @param selectableStructuresOut
 *    Output containing structures that can be selected.
 */ 
void 
ModelSurfaceSelector::getSelectableStructures(
                             std::vector<StructureEnum::Enum>& selectableStructuresOut) const
{
    selectableStructuresOut.clear();
    selectableStructuresOut.insert(selectableStructuresOut.end(),
                                  m_availableStructures.begin(),
                                  m_availableStructures.end());
}

/**
 * Get the surface controllers available for selection.
 * 
 * @param selectableSurfaceControllersOut
 *    Output containing surface controllers that can be selected.
 */ 
void 
ModelSurfaceSelector::getSelectableSurfaceControllers(
                        std::vector<ModelSurface*>& selectableSurfaceControllersOut) const
{
    selectableSurfaceControllersOut.clear();
    selectableSurfaceControllersOut.insert(selectableSurfaceControllersOut.end(),
                                           m_availableSurfaceControllers.begin(),
                                           m_availableSurfaceControllers.end());
}

/**
 * Update the selector with the available surface controllers.
 */
void 
ModelSurfaceSelector::updateSelector(const std::vector<Model*> modelDisplayControllers)
{
    m_allSurfaceControllers.clear();
    for (std::vector<Model*>::const_iterator iter = modelDisplayControllers.begin();
         iter != modelDisplayControllers.end();
         iter++) {
        ModelSurface* surfaceController =
        dynamic_cast<ModelSurface*>(*iter);
        if (surfaceController != NULL) {
            m_allSurfaceControllers.push_back(surfaceController);
        }
    }
    
    this->updateSelector();
}

/**
 * Update the selector with the available surface controllers.
 */
void 
ModelSurfaceSelector::updateSelector()
{
    bool haveCortexLeft = false;
    bool haveCortexRight = false;
    bool haveCerebellum = false;
    
    /*
     * Find the ALL surface controllers and structures
     */
    for (std::vector<ModelSurface*>::const_iterator iter = m_allSurfaceControllers.begin();
         iter != m_allSurfaceControllers.end();
         iter++) {
        ModelSurface* surfaceController = *iter;
        const Surface* surface = surfaceController->getSurface();
        const StructureEnum::Enum structure = surface->getStructure();
        
        switch (structure) {
            case StructureEnum::CEREBELLUM:
                haveCerebellum = true;
                break;
            case StructureEnum::CORTEX_LEFT:
                haveCortexLeft = true;
                break;
            case StructureEnum::CORTEX_RIGHT:
                haveCortexRight = true;
                break;
            default:
                break;
        }
    }
    
    /*
     * Determine which structures are available.
     */
    m_availableStructures.clear();
    m_availableStructures.push_back(StructureEnum::ALL);
    if (haveCerebellum) {
        m_availableStructures.push_back(StructureEnum::CEREBELLUM);    
    }
    if (haveCortexLeft) {
        m_availableStructures.push_back(StructureEnum::CORTEX_LEFT);    
    }
    if (haveCortexRight) {
        m_availableStructures.push_back(StructureEnum::CORTEX_RIGHT);    
    }
    
    /*
     * Update the structure selection.
     */
    if (std::find(m_availableStructures.begin(),
                  m_availableStructures.end(),
                  m_selectedStructure) == m_availableStructures.end()) {
        if (m_availableStructures.empty() == false) {
            m_selectedStructure = m_availableStructures[0];
        }
        else {
            m_selectedStructure = m_defaultStructure;
        }
    }
    
    /*
     * Update the available surface controllers.
     */
    m_availableSurfaceControllers.clear();
    for (std::vector<ModelSurface*>::iterator iter = m_allSurfaceControllers.begin();
         iter != m_allSurfaceControllers.end();
         iter++) {
        ModelSurface* surfaceController = *iter;
        const Surface* surface = surfaceController->getSurface();
        const StructureEnum::Enum structure = surface->getStructure();
        
        bool useIt = false;
        if (m_selectedStructure == StructureEnum::ALL) {
            useIt = true;
        }
        else if (m_selectedStructure == structure) {
            useIt = true;
        }
            
        if (useIt) {
            m_availableSurfaceControllers.push_back(surfaceController);
        }
    }
    
    /*
     * Update the surface selection.
     */
    if (std::find(m_availableSurfaceControllers.begin(),
                  m_availableSurfaceControllers.end(),
                  m_selectedSurfaceController) == m_availableSurfaceControllers.end()) {
        /*
         * Selected controller is not found.
         */
        m_selectedSurfaceController = NULL;

        /*
         * First, see if a previous controller for structure still exists, if so, use it.
         */
        std::map<StructureEnum::Enum, ModelSurface*>::iterator iter =
        m_previousSelectedSurfaceController.find(m_selectedStructure);
        if (iter != m_previousSelectedSurfaceController.end()) {
            ModelSurface* previousController = iter->second;
            if (std::find(m_availableSurfaceControllers.begin(),
                          m_availableSurfaceControllers.end(),
                          previousController) != m_availableSurfaceControllers.end()) {
                m_selectedSurfaceController = previousController;
            }
        }

        /*
         * Still not found?
         */
        if (m_selectedSurfaceController == NULL) {
            /*
             * Default to first
             */
            if (m_availableSurfaceControllers.empty() == false) {
                m_selectedSurfaceController = m_availableSurfaceControllers[0];
                
                /*
                 * Try to find and used the volume interaction surface.
                 */
                Brain* brain = m_selectedSurfaceController->getBrain();
                if (brain != NULL) {
                    BrainStructure* brainStructure = brain->getBrainStructure(m_selectedStructure, false);
                    if (brainStructure != NULL) {
                        const Surface* volInteractSurface = brainStructure->getVolumeInteractionSurface();
                        if (volInteractSurface != NULL) {
                            const int numSurfaceControllers = static_cast<int32_t>(m_availableSurfaceControllers.size());
                            for (int32_t i = 0; i < numSurfaceControllers; i++) {
                                if (m_availableSurfaceControllers[i]->getSurface() == volInteractSurface) {
                                    m_selectedSurfaceController = m_availableSurfaceControllers[i];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    /*
     * Save controller for retrieval later.
     */
    if (m_selectedSurfaceController != NULL) {
        m_previousSelectedSurfaceController[m_selectedStructure] = m_selectedSurfaceController;
    }
}
/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ModelSurfaceSelector::toString() const
{
    AString msg = "selectedStructure="
    + StructureEnum::toName(m_selectedStructure)
    + "selectedSurface=";
    if (m_selectedSurfaceController != NULL) {
        msg += m_selectedSurfaceController->getNameForGUI(false);
    }
    return msg;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
ModelSurfaceSelector::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                               const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ModelSurfaceSelector",
                                            1);
    
    sceneClass->addEnumeratedType("m_selectedStructure", 
                                  StructureEnum::toName(m_selectedStructure));
    if (m_selectedSurfaceController != NULL) {
        const Surface* surface = m_selectedSurfaceController->getSurface();
        if (surface != NULL) {
            const AString filename = surface->getFileNameNoPath();
            sceneClass->addString("surfaceFileName", 
                                  filename);
        }
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
ModelSurfaceSelector::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                    const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_selectedStructure = StructureEnum::fromName(sceneClass->getEnumeratedTypeValue("m_selectedStructure",
                                                                                     StructureEnum::toName(StructureEnum::ALL)),
                                                  NULL);
    setSelectedStructure(m_selectedStructure);
    
    const AString surfaceFileName = sceneClass->getStringValue("surfaceFileName",
                                                               "");
    if (surfaceFileName.isEmpty() == false) {
        for (std::vector<ModelSurface*>::iterator iter = m_availableSurfaceControllers.begin();
             iter != m_availableSurfaceControllers.end();
             iter++) {
            const Surface* surface = (*iter)->getSurface();
            if (surfaceFileName == surface->getFileNameNoPath()) {
                setSelectedSurfaceController(*iter);
            }
        }
    }
}

