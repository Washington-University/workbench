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
Model::getControllerType() const
{
    return m_modelType; 
}

/**
 * Get a String for use in the GUI.  Use toDescriptiveString() for
 * information about this controller's content.
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
 * Returns a descriptive string containing info about this instance.
 *
 * @return  String describing contents of this instance.
 *
 */
AString
Model::toDescriptiveString() const
{
    AString s = CaretObject::toString();
    
    return s;
}

/**
 * Get the brain that created this controller.
 * @return The brain.
 */
Brain*
Model::getBrain()
{
    return m_brain;
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
}




