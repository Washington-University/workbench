
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

#define __MODEL_SURFACE_SELECTOR_DECLARE__
#include "ModelSurfaceSelector.h"
#undef __MODEL_SURFACE_SELECTOR_DECLARE__

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
 * \brief Maintains selection of surface model
 *
 * Maintains selection of a surface model with the ability to limit the
 * surface models to those from a specific brain structure.
 */

/**
 * Constructor.
 */
ModelSurfaceSelector::ModelSurfaceSelector()
: CaretObject()
{
    m_defaultStructure = StructureEnum::ALL;
    m_selectedStructure = StructureEnum::ALL;
    m_selectedSurfaceModel = NULL;
}

/**
 * Destructor.
 */
ModelSurfaceSelector::~ModelSurfaceSelector()
{
    
}

/**
 * @return The selected surface model.
 */
ModelSurface* 
ModelSurfaceSelector::getSelectedSurfaceModel()
{
    this->updateSelector();
    return m_selectedSurfaceModel;
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
 * Set the selected surface model.
 * 
 * @param surfaceModel
 *    Model that is selected.
 */
void 
ModelSurfaceSelector::setSelectedSurfaceModel(ModelSurface* surfaceModel)
{
    m_selectedSurfaceModel = surfaceModel;
    if (m_selectedStructure != StructureEnum::ALL) {
        m_selectedStructure = surfaceModel->getSurface()->getStructure();
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
 * Get the surface models available for selection.
 * 
 * @param selectableSurfaceModelsOut
 *    Output containing surface models that can be selected.
 */ 
void 
ModelSurfaceSelector::getSelectableSurfaceModels(
                        std::vector<ModelSurface*>& selectableSurfaceModelsOut) const
{
    selectableSurfaceModelsOut.clear();
    selectableSurfaceModelsOut.insert(selectableSurfaceModelsOut.end(),
                                           m_availableSurfaceModels.begin(),
                                           m_availableSurfaceModels.end());
}

/**
 * Update the selector with the available surface models.
 */
void 
ModelSurfaceSelector::updateSelector(const std::vector<Model*> modelDisplayModels)
{
    m_allSurfaceModels.clear();
    for (std::vector<Model*>::const_iterator iter = modelDisplayModels.begin();
         iter != modelDisplayModels.end();
         iter++) {
        ModelSurface* surfaceModel =
        dynamic_cast<ModelSurface*>(*iter);
        if (surfaceModel != NULL) {
            m_allSurfaceModels.push_back(surfaceModel);
        }
    }
    
    this->updateSelector();
}

/**
 * Update the selector with the available surface models.
 */
void 
ModelSurfaceSelector::updateSelector()
{
    bool haveCortexLeft = false;
    bool haveCortexRight = false;
    bool haveCerebellum = false;
    
    /*
     * Find the ALL surface models and structures
     */
    for (std::vector<ModelSurface*>::const_iterator iter = m_allSurfaceModels.begin();
         iter != m_allSurfaceModels.end();
         iter++) {
        ModelSurface* surfaceModel = *iter;
        const Surface* surface = surfaceModel->getSurface();
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
     * Update the available surface models.
     */
    m_availableSurfaceModels.clear();
    for (std::vector<ModelSurface*>::iterator iter = m_allSurfaceModels.begin();
         iter != m_allSurfaceModels.end();
         iter++) {
        ModelSurface* surfaceModel = *iter;
        const Surface* surface = surfaceModel->getSurface();
        const StructureEnum::Enum structure = surface->getStructure();
        
        bool useIt = false;
        if (m_selectedStructure == StructureEnum::ALL) {
            useIt = true;
        }
        else if (m_selectedStructure == structure) {
            useIt = true;
        }
            
        if (useIt) {
            m_availableSurfaceModels.push_back(surfaceModel);
        }
    }
    
    /*
     * Update the surface selection.
     */
    if (std::find(m_availableSurfaceModels.begin(),
                  m_availableSurfaceModels.end(),
                  m_selectedSurfaceModel) == m_availableSurfaceModels.end()) {
        /*
         * Selected model is not found.
         */
        m_selectedSurfaceModel = NULL;

        /*
         * First, see if a previous model for structure still exists, if so, use it.
         */
        std::map<StructureEnum::Enum, ModelSurface*>::iterator iter =
        m_previousSelectedSurfaceModel.find(m_selectedStructure);
        if (iter != m_previousSelectedSurfaceModel.end()) {
            ModelSurface* previousModel = iter->second;
            if (std::find(m_availableSurfaceModels.begin(),
                          m_availableSurfaceModels.end(),
                          previousModel) != m_availableSurfaceModels.end()) {
                m_selectedSurfaceModel = previousModel;
            }
        }

        /*
         * Still not found?
         */
        if (m_selectedSurfaceModel == NULL) {
            /*
             * Default to first
             */
            if (m_availableSurfaceModels.empty() == false) {
                m_selectedSurfaceModel = m_availableSurfaceModels[0];
                
                /*
                 * Try to find and used the primary anatomical surface.
                 */
                Brain* brain = m_selectedSurfaceModel->getBrain();
                if (brain != NULL) {
                    BrainStructure* brainStructure = brain->getBrainStructure(m_selectedStructure, false);
                    if (brainStructure != NULL) {
                        const Surface* volInteractSurface = brainStructure->getPrimaryAnatomicalSurface();
                        if (volInteractSurface != NULL) {
                            const int numSurfaceModels = static_cast<int32_t>(m_availableSurfaceModels.size());
                            for (int32_t i = 0; i < numSurfaceModels; i++) {
                                if (m_availableSurfaceModels[i]->getSurface() == volInteractSurface) {
                                    m_selectedSurfaceModel = m_availableSurfaceModels[i];
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
     * Save model for retrieval later.
     */
    if (m_selectedSurfaceModel != NULL) {
        m_previousSelectedSurfaceModel[m_selectedStructure] = m_selectedSurfaceModel;
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
    if (m_selectedSurfaceModel != NULL) {
        msg += m_selectedSurfaceModel->getNameForGUI(false);
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
    
    sceneClass->addEnumeratedType<StructureEnum, StructureEnum::Enum>("m_selectedStructure",
                                                                      m_selectedStructure);
    if (m_selectedSurfaceModel != NULL) {
        const Surface* surface = m_selectedSurfaceModel->getSurface();
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
    
    m_selectedStructure = sceneClass->getEnumeratedTypeValue<StructureEnum, StructureEnum::Enum>("m_selectedStructure", 
                                                                                                 m_selectedStructure);
    setSelectedStructure(m_selectedStructure);
    
    const AString surfaceFileName = sceneClass->getStringValue("surfaceFileName",
                                                               "");
    ModelSurface* matchedSurfaceModel = NULL;
    
    if (surfaceFileName.isEmpty() == false) {
        for (std::vector<ModelSurface*>::iterator iter = m_availableSurfaceModels.begin();
             iter != m_availableSurfaceModels.end();
             iter++) {
            const Surface* surface = (*iter)->getSurface();
            if (surfaceFileName == surface->getFileNameNoPath()) {
                matchedSurfaceModel = *iter;
                break;
            }
        }
    }
    
    /*
     * Note: setSelectedSurfaceModel() will update the content of
     * m_availableSurfaceModels and the above iterators will become
     * invalid.  So setSelectedSurfaceModel() must be called outside
     * of the loop.
     * Bug found by JS.
     */
    if (matchedSurfaceModel != NULL) {
        setSelectedSurfaceModel(matchedSurfaceModel);        
    }
}

