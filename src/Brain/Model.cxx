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



#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "Model.h"
#include "ModelSurface.h"
#include "ModelVolume.h"
#include "OverlaySet.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param m_modelType Type of this model.
 * @param brain Brain that 'owns' this model.
 */
Model::Model(const ModelTypeEnum::Enum modelType,
             Brain* brain)
    : CaretObject()
{
    m_brain = brain;
    initializeMembersModel();
    m_modelType = modelType;
}

/**
 * Destructor
 */
Model::~Model()
{
}

void
Model::initializeMembersModel()
{
}

/**
 * @return The type of model.
 */
ModelTypeEnum::Enum 
Model::getModelType() const
{
    return m_modelType; 
}

/**
 * Get a String for use in the GUI.
 *
 * @return String for use in the GUI.
 *
 */
AString
Model::toString() const
{
       return getNameForGUI(true);
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
Model::getDescriptionOfContent(const int32_t /*tabIndex*/,
                               PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine(getNameForGUI(true));
}

/**
 * Get the brain that created this model.
 * @return The brain.
 */
Brain*
Model::getBrain()
{
    return m_brain;
}

/**
 * Intended for overriding by sub-classes so that they
 * can selected the desired surfaces after file loading.
 */
void
Model::initializeSelectedSurfaces()
{
    /* nothing */
}

/**
 * Get the chart overlay set for this model.
 * 
 * @param tabIndex
 *     Index for the chart overlay set.
 * @return
 *     Chart overlay set or NULL if not valid for this model.
 */
ChartOverlaySet*
Model::getChartOverlaySet(const int /*tabIndex*/)
{
    return NULL;
}

/**
 * Get the chart overlay set for this model.
 *
 * @param tabIndex
 *     Index for the chart overlay set.
 * @return
 *     Chart overlay set or NULL if not valid for this model.
 */
const ChartOverlaySet*
Model::getChartOverlaySet(const int /*tabIndex*/) const
{
    return NULL;
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
Model::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "Model",
                                            1);

    /*
     * Do not use scene assistant to save model type since special handling
     * is needed when it is restored.
     */
    sceneClass->addEnumeratedType<ModelTypeEnum, ModelTypeEnum::Enum>("m_modelType", m_modelType);
    
    if (m_modelType == ModelTypeEnum::MODEL_TYPE_SURFACE) {
        const ModelSurface* surfaceModel = dynamic_cast<ModelSurface*>(this);
        CaretAssert(surfaceModel);
        sceneClass->addString("surfaceName",
                              surfaceModel->getSurface()->getFileNameNoPath());
    }
                          
    
    /*
     * Get indices of tabs that are to be saved to scene.
     */ 
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    const int32_t numActiveTabs = static_cast<int32_t>(tabIndices.size()); 
        
    /*
     * Save the overlays (except for yoking)
     */
        std::vector<SceneClass*> overlaySetClassVector;
        for (int32_t iat = 0; iat < numActiveTabs; iat++) {
            const int32_t tabIndex = tabIndices[iat];
            SceneClass* overlaySetClass = new SceneClass(("modelOverlay["
                                                          + AString::number(iat)
                                                          + "]"),
                                                         "OverlaySet",
                                                         1);
            overlaySetClass->addInteger("tabIndex",
                                        tabIndex);
            overlaySetClass->addChild(getOverlaySet(tabIndex)->saveToScene(sceneAttributes, 
                                                                           "overlaySet"));
            overlaySetClassVector.push_back(overlaySetClass);
        }
        SceneClassArray* overlaySetClassArray = new SceneClassArray("m_overlaySet",
                                                                    overlaySetClassVector);
        sceneClass->addChild(overlaySetClassArray);
    
    /*
     * Save information specific to the type of model
     */
    saveModelSpecificInformationToScene(sceneAttributes,                                         
                                        sceneClass);
    
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
Model::restoreFromScene(const SceneAttributes* sceneAttributes,
                          const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }

    /*
     *  This model was created by the parent scene class.
     *  The model type in the scene should match what was saved.
     *  If not, a serious (programming) error has occurred.
     */
    const ModelTypeEnum::Enum savedModelType = sceneClass->getEnumeratedTypeValue<ModelTypeEnum, ModelTypeEnum::Enum>("m_modelType", 
                                                                                         ModelTypeEnum::MODEL_TYPE_INVALID);
    if (savedModelType == ModelTypeEnum::MODEL_TYPE_INVALID) {
        CaretLogSevere("Non-matching model type when restoring scene: "
                             + ModelTypeEnum::toName(savedModelType));
        return;
    }
    
    if (savedModelType != m_modelType) {
        return;
    }
    
    if (m_modelType == ModelTypeEnum::MODEL_TYPE_SURFACE) {
        const AString surfaceName = sceneClass->getStringValue("surfaceName",
                                                               "NOT-FOUND");
        const ModelSurface* surfaceModel = dynamic_cast<ModelSurface*>(this);
        CaretAssert(surfaceModel);        
        if (surfaceName != surfaceModel->getSurface()->getFileNameNoPath()) {
            /*
             * Exit as this is not the surface for restoring (name does not match)
             */
            return;
        }
    }
    
    /*
     * Restore the overlays (except for yoking)
     */
        const SceneClassArray* overlaySetClassArray = sceneClass->getClassArray("m_overlaySet");
        if (overlaySetClassArray != NULL) {
            const int32_t numSavedOverlaySets = overlaySetClassArray->getNumberOfArrayElements();
            for (int32_t i = 0; i < numSavedOverlaySets; i++) {
                const SceneClass* overlaySceneClass = overlaySetClassArray->getClassAtIndex(i);
                const int32_t tabIndex = overlaySceneClass->getIntegerValue("tabIndex",
                                                                            -1);
                const SceneClass* overlayClass = overlaySceneClass->getClass("overlaySet");
                if ((tabIndex >= 0) 
                    && (overlayClass != NULL)) {
                    getOverlaySet(tabIndex)->restoreFromScene(sceneAttributes, 
                                                              overlayClass);
                }
            }
        }
    
    /*
     * Restore any information specific to type of model
     */
    restoreModelSpecificInformationFromScene(sceneAttributes, 
                                             sceneClass);

    /*
     * Check for transformations that are stored in the model's scene.
     * These are only present in older scene files (circa March 2013 
     * and earlier).
     */
    m_oldSceneTransformations.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_oldSceneTransformations[i].m_rotationValid = false;
        m_oldSceneTransformations[i].m_scalingValid = false;
        m_oldSceneTransformations[i].m_translationValid = false;
    }
    
    /*
     * Restore scaling
     */
    const SceneClassArray* scalingClassArray = sceneClass->getClassArray("m_scaling");
    if (scalingClassArray != NULL) {
        const int32_t numSavedScaling = scalingClassArray->getNumberOfArrayElements();
        for (int32_t ism = 0; ism < numSavedScaling; ism++) {
            const SceneClass* scalingClass = scalingClassArray->getClassAtIndex(ism);
            const int32_t tabIndex = scalingClass->getIntegerValue("tabIndex", -1);
            if ((tabIndex >= 0)
                && (tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
                m_oldSceneTransformations[tabIndex].m_scaling = scalingClass->getFloatValue("scaling", 1.0);
                m_oldSceneTransformations[tabIndex].m_scalingValid = true;
            }
        }
    }
    
    /*
     * Restore translation
     */
    const SceneClassArray* translationClassArray = sceneClass->getClassArray("m_translation");
    if (translationClassArray != NULL) {
        const int32_t numSavedTanslations = translationClassArray->getNumberOfArrayElements();
        for (int32_t ism = 0; ism < numSavedTanslations; ism++) {
            const SceneClass* translationClass = translationClassArray->getClassAtIndex(ism);
            const int32_t tabIndex = translationClass->getIntegerValue("tabIndex", -1);
            const int32_t viewingTransformIndex = translationClass->getIntegerValue("viewingTransformIndex", -1);
            if ((tabIndex >= 0)
                && (tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
                if (viewingTransformIndex == 0) {
                    if (translationClass->getFloatArrayValue("translation",
                                                             m_oldSceneTransformations[tabIndex].m_translation,
                                                             3) == 3) {
                        m_oldSceneTransformations[tabIndex].m_translationValid = true;
                    }
                }
            }
        }
    }
    
    /*
     * Restore rotation matrices
     */
    const SceneClassArray* rotationMatrixClassArray = sceneClass->getClassArray("m_viewingRotationMatrix");
    if (rotationMatrixClassArray != NULL) {
        const int32_t numSavedMatrices = rotationMatrixClassArray->getNumberOfArrayElements();
        for (int32_t ism = 0; ism < numSavedMatrices; ism++) {
            const SceneClass* rotationMatrixClass = rotationMatrixClassArray->getClassAtIndex(ism);
            const int32_t tabIndex = rotationMatrixClass->getIntegerValue("tabIndex", -1);
            const int32_t viewingTransformIndex = rotationMatrixClass->getIntegerValue("viewingTransformIndex", -1);
            if ((tabIndex >= 0)
                && (tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
                if (viewingTransformIndex == 0) {
                    if (rotationMatrixClass->getFloatArrayValue("matrix",
                                                                (float*)m_oldSceneTransformations[tabIndex].m_rotationMatrix,
                                                                16) == 16) {
                        m_oldSceneTransformations[tabIndex].m_rotationValid = true;
                    }
                }
            }
        }
    }
    

}

/**
 * Get transformations for a given tab from older scenes that were
 * created when the transformations were present in every model for
 * every tab.  Transformations have since been moved into the 
 * browser tab content.
 *
 * @param tabIndex
 *     Index of tab for transformation.
 * @param translationOut
 *     The translation for the given tab.
 * @param scalingOut
 *     The scaling for the given tab.
 * @param rotationMatrixOut
 *     The rotation matrix for the given tab.
 * @return
 *     true if the transformations are valid, else false.
 */
bool
Model::getOldSceneTransformation(const int tabIndex,
                                 float translationOut[3],
                                 float& scalingOut,
                                 float rotationMatrixOut[4][4]) const
{
    if ((tabIndex >= 0)
        && (tabIndex < static_cast<int32_t>(m_oldSceneTransformations.size()))) {
        const OldSceneTransformation& ost = m_oldSceneTransformations[tabIndex];
        if (ost.m_rotationValid
            && ost.m_scalingValid
            && ost.m_translationValid) {
            translationOut[0] = ost.m_translation[0];
            translationOut[1] = ost.m_translation[1];
            translationOut[2] = ost.m_translation[2];
            
            scalingOut = ost.m_scaling;
            
            for (int32_t i = 0; i < 4; i++) {
                for (int32_t j = 0; j < 4; j++) {
                    rotationMatrixOut[i][j] = ost.m_rotationMatrix[i][j];
                }
            }
            
            return true;
        }
    }
    
    return false;
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
Model::copyTabContent(const int32_t /*sourceTabIndex*/,
                      const int32_t /*destinationTabIndex*/)
{
    
}


