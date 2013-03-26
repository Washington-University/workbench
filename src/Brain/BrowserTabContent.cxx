
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

#include <set>

#define __BROWSER_TAB_CONTENT_DECLARE__
#include "BrowserTabContent.h"
#undef __BROWSER_TAB_CONTENT_DECLARE__

#include "BorderFile.h"
#include "Brain.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyModel.h"
#include "GroupAndNameHierarchyName.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFoci.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "EventModelYokingGroupGetAll.h"
#include "FociFile.h"
#include "LabelFile.h"
#include "Matrix4x4.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelSurfaceSelector.h"
#include "ModelTransform.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "ModelYokingGroup.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "StructureEnum.h"
#include "VolumeFile.h"
#include "VolumeSurfaceOutlineModel.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;

/**
 * Constructor.
 * @param tabNumber
 *    Number for this tab.
 */
BrowserTabContent::BrowserTabContent(const int32_t tabNumber)
: CaretObject()
{
    m_tabNumber = tabNumber;
    m_surfaceModelSelector = new ModelSurfaceSelector();
    m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
    m_volumeModel = NULL;
    m_wholeBrainModel = NULL;
    m_surfaceMontageModel = NULL;
    m_guiName = "";
    m_userName = "";
    m_volumeSurfaceOutlineSetModel = new VolumeSurfaceOutlineSetModel();
    m_selectedYokingGroup = NULL;

    m_rotationMatrix = new Matrix4x4();
    m_volumeSliceRotationMatrix = new Matrix4x4();
    m_translation[0] = 0.0;
    m_translation[1] = 0.0;
    m_translation[2] = 0.0;
    m_volumeSliceTranslation[0] = 0.0;
    m_volumeSliceTranslation[1] = 0.0;
    m_volumeSliceTranslation[2] = 0.0;
    m_scaling = 1.0;
    m_volumeSliceScaling = 1.0;
    leftView();
    
    m_clippingCoordinate[0] = 0.0;
    m_clippingCoordinate[1] = 0.0;
    m_clippingCoordinate[2] = 0.0;

    m_clippingThickness[0] = 20.0;
    m_clippingThickness[1] = 20.0;
    m_clippingThickness[2] = 20.0;
    
    m_clippingEnabled[0] = false;
    m_clippingEnabled[1] = false;
    m_clippingEnabled[2] = false;
    
    m_sceneClassAssistant = new SceneClassAssistant();
    m_sceneClassAssistant->add("m_tabNumber", 
                               &m_tabNumber);
    m_sceneClassAssistant->add("m_userName",
                               &m_userName);
    m_sceneClassAssistant->add<ModelTypeEnum, ModelTypeEnum::Enum>("m_selectedModelType",
                                                                   &m_selectedModelType);
    m_sceneClassAssistant->add("m_surfaceModelSelector",
                               "ModelSurfaceSelector",
                               m_surfaceModelSelector);
    m_sceneClassAssistant->add("m_volumeSurfaceOutlineSetModel",
                               "VolumeSurfaceOutlineSetModel",
                               m_volumeSurfaceOutlineSetModel);
    m_sceneClassAssistant->addArray("m_clippingCoordinate",
                                    m_clippingCoordinate,
                                    3,
                                    0.0);
    m_sceneClassAssistant->addArray("m_clippingThickness",
                                    m_clippingThickness,
                                    3,
                                    100.0);
    m_sceneClassAssistant->addArray("m_clippingEnabled",
                                    m_clippingEnabled,
                                    3,
                                    false);
    
    m_sceneClassAssistant->addArray("m_translation",
                                    m_translation,
                                    3,
                                    0.0);
    m_sceneClassAssistant->addArray("m_volumeSliceTranslation",
                                    m_volumeSliceTranslation,
                                    3,
                                    0.0);
    
    m_sceneClassAssistant->add("m_scaling",
                               &m_scaling);
    m_sceneClassAssistant->add("m_volumeSliceScaling",
                               &m_volumeSliceScaling);
}

/**
 * Destructor.
 */
BrowserTabContent::~BrowserTabContent()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_rotationMatrix;
    delete m_volumeSliceRotationMatrix;
    
    delete m_surfaceModelSelector;
    m_surfaceModelSelector = NULL;
    
    delete m_volumeSurfaceOutlineSetModel;
    m_volumeSurfaceOutlineSetModel = NULL;
    
    delete m_sceneClassAssistant;
    m_sceneClassAssistant = NULL;
}

/**
 * Clone the contents of the given browser tab.
 * @param tabToClone
 *    Tab whose contents is cloned.
 */
void 
BrowserTabContent::cloneBrowserTabContent(BrowserTabContent* tabToClone)
{
    CaretAssert(tabToClone);
    m_surfaceModelSelector->setSelectedStructure(tabToClone->m_surfaceModelSelector->getSelectedStructure());
    m_surfaceModelSelector->setSelectedSurfaceController(tabToClone->m_surfaceModelSelector->getSelectedSurfaceController());

    m_selectedModelType = tabToClone->m_selectedModelType;
    m_volumeModel = tabToClone->m_volumeModel;
    m_wholeBrainModel = tabToClone->m_wholeBrainModel;
    m_surfaceMontageModel = tabToClone->m_surfaceMontageModel;
    m_selectedYokingGroup = tabToClone->m_selectedYokingGroup;
    
    m_translation[0] = tabToClone->m_translation[0];
    m_translation[1] = tabToClone->m_translation[1];
    m_translation[2] = tabToClone->m_translation[2];
    
    m_volumeSliceTranslation[0] = tabToClone->m_volumeSliceTranslation[0];
    m_volumeSliceTranslation[1] = tabToClone->m_volumeSliceTranslation[1];
    m_volumeSliceTranslation[2] = tabToClone->m_volumeSliceTranslation[2];
    
    *m_rotationMatrix = *tabToClone->m_rotationMatrix;
    *m_volumeSliceRotationMatrix = *tabToClone->m_volumeSliceRotationMatrix;
    
    m_scaling = tabToClone->m_scaling;
    m_volumeSliceScaling = tabToClone->m_volumeSliceScaling;
    
    Model* model = getModelControllerForDisplay();
    //Model* modelBeingCloned = tabToClone->getModelControllerForDisplay();
    
    const OverlaySet* overlaySetToClone = tabToClone->getOverlaySet();
    if (overlaySetToClone != NULL) {
        OverlaySet* overlaySet = getOverlaySet();
        if (overlaySet != NULL) {
            overlaySet->copyOverlaySet(overlaySetToClone);
        }
    }
    
    if (model != NULL) {
        Brain* brain = model->getBrain();
        
        brain->copyDisplayProperties(tabToClone->getTabNumber(),
                                     getTabNumber());
        
        const int32_t numberOfBrainStructures = brain->getNumberOfBrainStructures();
        for (int32_t i = 0; i < numberOfBrainStructures; i++) {
            BrainStructure* bs = brain->getBrainStructure(i);
            const int32_t numLabelFiles = bs->getNumberOfLabelFiles();
            for (int32_t j = 0; j < numLabelFiles; j++) {
                LabelFile* labelFile = bs->getLabelFile(j);
                labelFile->getGroupAndNameHierarchyModel()->copySelections(tabToClone->getTabNumber(),
                                                                           getTabNumber());
            }
        }

        const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
        for (int32_t i = 0; i < numBorderFiles; i++) {
            BorderFile* bf = brain->getBorderFile(i);
            bf->getGroupAndNameHierarchyModel()->copySelections(tabToClone->getTabNumber(),
                                                                getTabNumber());
        }
        
        const int32_t numFociFiles = brain->getNumberOfFociFiles();
        for (int32_t i = 0; i < numFociFiles; i++) {
            FociFile* ff = brain->getFociFile(i);
            ff->getGroupAndNameHierarchyModel()->copySelections(tabToClone->getTabNumber(),
                                                                getTabNumber());
        }
    }
    
    m_volumeSurfaceOutlineSetModel->copyVolumeSurfaceOutlineSetModel(tabToClone->getVolumeSurfaceOutlineSet());
    
    updateTransformationsForYoking();
}

/**
 * Get the name of this browser tab.
 * Name priority is (1) name set by user, (2) name set by
 * user-interface, and (3) the default name.
 *
 * @return  Name of this tab.
 */
AString 
BrowserTabContent::getName() const
{
    AString s = "(" + AString::number(m_tabNumber + 1) + ") ";
    
    if (m_userName.isEmpty() == false) {
        s += m_userName;
    }
    else {
        const Model* displayedController =
            getModelControllerForDisplay();
        if (displayedController != NULL) {
            const AString name = displayedController->getNameForBrowserTab();
            s += name;
        }
    }
    
    return s;
}

/**
 * Set the user name of this tab.  The user name
 * overrides the default naming.
 *
 * @param userName
 *    User name for tab.
 */
void 
BrowserTabContent::setUserName(const AString& userName)
{
    m_userName = userName;
}

/**
 * @return The user name.
 */
AString 
BrowserTabContent::getUserName() const
{
    return m_userName;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrowserTabContent::toString() const
{
    return "WindowTabContent";
}

/**
 * Get the selected model type.
 * 
 * @return The selected model type.
 */   
ModelTypeEnum::Enum 
BrowserTabContent::getSelectedModelType() const
{
    return m_selectedModelType;
}

/**
 * Set the selected model type.
 *
 * @param selectedModelType
 *    New selected model type. 
 */   
void 
BrowserTabContent::setSelectedModelType(ModelTypeEnum::Enum selectedModelType)
{
    m_selectedModelType = selectedModelType;
    updateTransformationsForYoking();
}

/**
 * Get the model controller for DISPLAY purposes.  Note: When applying
 * transformations, use getModelControllerForTransformation() so that
 * any yoking is properly carried out.
 * 
 * @return  Pointer to displayed controller or NULL
 *          if none are available.
 */   
Model* 
BrowserTabContent::getModelControllerForDisplay()
{
    Model* mdc = NULL;
    
    switch (m_selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            mdc = m_surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = m_surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = m_volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = m_wholeBrainModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    return mdc;
}

/**
 * Get the model controller for DISPLAY purposes.  Note: When applying
 * transformations, use getModelControllerForTransformation() so that
 * any yoking is properly carried out.
 * 
 * @return  Pointer to displayed controller or NULL
 *          if none are available.
 */   
const Model* 
BrowserTabContent::getModelControllerForDisplay() const
{
    Model* mdc = NULL;
    
    switch (m_selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            mdc = m_surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = m_surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = m_volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = m_wholeBrainModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    return mdc;
}

/**
 * Get the model controller for TRANSFORMATION purposes.  If yoked
 * to a valid yoking model, the transformation for the yoking model
 * will be returned.
 * 
 * @return  Pointer to transformation controller or NULL
 *          if none are available.
 */   
Model* 
BrowserTabContent::getModelControllerForTransformation()
{
    Model* mdc = getModelControllerForDisplay();
    if (mdc == NULL) {
        return NULL;
    }
    
    ModelYokingGroup* myg = getSelectedYokingGroupForModel(mdc);
    if (myg != NULL) {
        mdc = myg;
    }
    
    return mdc;
}

/**
 * @return Is the displayed model a right surface that is lateral/medial yoked?
 */
bool 
BrowserTabContent::isDisplayedModelSurfaceRightLateralMedialYoked() const
{
    bool itIs = false;
    
    const ModelSurface* surfaceController = getDisplayedSurfaceModel();
    if (surfaceController != NULL) {
        const Surface* surface = surfaceController->getSurface();
        if (surface->getStructure() == StructureEnum::CORTEX_RIGHT) {
            if (getSelectedYokingGroupForModel(surfaceController) != NULL) {
                itIs = true;
            }
        }
    }
    
    return itIs;
}


/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is NOT a 
 *          surface.
 */   
ModelSurface* 
BrowserTabContent::getDisplayedSurfaceModel()
{
    ModelSurface* mdcs =
        dynamic_cast<ModelSurface*>(getModelControllerForDisplay());
    return mdcs;
}

/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is NOT a 
 *          surface.
 */   
const ModelSurface* 
BrowserTabContent::getDisplayedSurfaceModel() const
{
    const ModelSurface* mdcs =
    dynamic_cast<const ModelSurface*>(getModelControllerForDisplay());
    return mdcs;
}

/**
 * Get the displayed volume model.
 * 
 * @return  Pointer to displayed volume model or 
 *          NULL if the displayed model is NOT a 
 *          volume.
 */   
ModelVolume* 
BrowserTabContent::getDisplayedVolumeModel()
{
    ModelVolume* mdcv =
        dynamic_cast<ModelVolume*>(getModelControllerForDisplay());
    return mdcv;
}

/**
 * @return Is the displayed model a volume slice model?
 */
bool
BrowserTabContent::isVolumeSlicesDisplayed() const
{
    const ModelVolume* mdcv = dynamic_cast<const ModelVolume*>(getModelControllerForDisplay());
    
    const bool volumeFlag = (mdcv != NULL);
    return volumeFlag;
}

/**
 * Get the displayed whole brain model.
 * 
 * @return  Pointer to displayed whole brain model or 
 *          NULL if the displayed model is NOT a 
 *          whole brain.
 */   
ModelWholeBrain* 
BrowserTabContent::getDisplayedWholeBrainModel()
{
    ModelWholeBrain* mdcwb =
        dynamic_cast<ModelWholeBrain*>(getModelControllerForDisplay());
    return mdcwb;

}

/**
 * @return Pointer to displayed surface montage model
 * or NULL if the displayed model is not a surface
 * montage model.
 */
ModelSurfaceMontage* 
BrowserTabContent::getDisplayedSurfaceMontageModel()
{
    ModelSurfaceMontage* mdcsm =
    dynamic_cast<ModelSurfaceMontage*>(getModelControllerForDisplay());
    return mdcsm;
}

/**
 * Get all of the available surface models.
 * 
 * @return Vector containing all surface models.
 */   
const std::vector<ModelSurface*> 
BrowserTabContent::getAllSurfaceModels() const
{
    return m_allSurfaceModels;
}

/**
 * @return The surface model selector used to 
 * select a surface and structure.
 */
ModelSurfaceSelector* 
BrowserTabContent::getSurfaceModelSelector()
{
    return m_surfaceModelSelector;
}

/**
 * Get the overlay assignments for this tab.
 * 
 * @return  Overlay assignments for this tab or NULL if no valid model.
 */
OverlaySet* 
BrowserTabContent::getOverlaySet()
{
    Model* modelDisplayController = getModelControllerForDisplay();
    if (modelDisplayController != NULL) {
       return modelDisplayController->getOverlaySet(m_tabNumber);
    }
    return NULL;
}

/**
 * Get the tab number for this content.
 * 
 * @return  Tab number.
 */
int32_t 
BrowserTabContent::getTabNumber() const
{
    return m_tabNumber;
}

/**
 * Update the selected models.
 */
void 
BrowserTabContent::update(const std::vector<Model*> modelDisplayControllers)
{
    m_surfaceModelSelector->updateSelector(modelDisplayControllers);
    
    const int32_t numModels = static_cast<int32_t>(modelDisplayControllers.size());
    
    m_allSurfaceModels.clear();
    m_surfaceModelSelector->getSelectableSurfaceControllers(m_allSurfaceModels);
    m_volumeModel = NULL;
    m_wholeBrainModel = NULL;
    m_surfaceMontageModel = NULL;
    
    for (int i = 0; i < numModels; i++) {
        Model* mdc = modelDisplayControllers[i];
        
        ModelSurface* mdcs = dynamic_cast<ModelSurface*>(mdc);
        ModelVolume* mdcv = dynamic_cast<ModelVolume*>(mdc);
        ModelWholeBrain* mdcwb = dynamic_cast<ModelWholeBrain*>(mdc);
        ModelSurfaceMontage* mdcsm = dynamic_cast<ModelSurfaceMontage*>(mdc);
        
        if (mdcs != NULL) {
            /* nothing to do since the surface model selector handles surfaces */
        }
        else if (mdcv != NULL) {
            CaretAssertMessage((m_volumeModel == NULL), "There is more than one volume model.");
            m_volumeModel = mdcv;
        }
        else if (mdcwb != NULL) {
            CaretAssertMessage((m_wholeBrainModel == NULL), "There is more than one whole brain model.");
            m_wholeBrainModel = mdcwb;
        }
        else if (mdcsm != NULL) {
            CaretAssertMessage((m_surfaceMontageModel == NULL), "There is more than one surface montage model.");
            m_surfaceMontageModel = mdcsm;
        }
        else {
            CaretAssertMessage(0, (AString("Unknown type of brain model.") + mdc->getNameForGUI(true)));
        }
    }
    
    switch (m_selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            if (m_surfaceModelSelector->getSelectedSurfaceController() == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            if (m_surfaceMontageModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            if (m_volumeModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            if (m_wholeBrainModel == NULL) {
                m_selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    if (m_selectedModelType == ModelTypeEnum::MODEL_TYPE_INVALID) {
        if (m_surfaceModelSelector->getSelectedSurfaceController() != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE;
        }
        else if (m_volumeModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES;
        }
        else if (m_wholeBrainModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN;
        }
        else if (m_surfaceMontageModel != NULL) {
            m_selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE;
        }
    }

    updateTransformationsForYoking();
}

/**
 * Is the surface model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isSurfaceModelValid() const
{
    bool valid = (m_allSurfaceModels.empty() == false);
    return valid;
}

/**
 * Is the volume model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isVolumeSliceModelValid() const
{
    bool valid = (m_volumeModel != NULL);
    return valid;
}

/**
 * Is the whole brain model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isWholeBrainModelValid() const
{
    bool valid = (m_wholeBrainModel != NULL);
    return valid;
}

/**
 * Is the surface montage model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isSurfaceMontageModelValid() const
{
    bool valid = (m_surfaceMontageModel != NULL);
    return valid;
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
BrowserTabContent::receiveEvent(Event* /*event*/)
{
}

/**
 * Update the transformations for yoking prior to drawing.
 * Does nothing if not yoked.
 */
void 
BrowserTabContent::updateTransformationsForYoking()
{
    Model* transformController = getModelControllerForTransformation();
    ModelYokingGroup* yokingController = 
        dynamic_cast<ModelYokingGroup*>(transformController);
    if (yokingController != NULL) {
        Model* mdc = getModelControllerForDisplay();
        if (mdc != NULL) {
//            mdc->copyTransformationsAndViews(*yokingController, 
//                                     0, // always used window 0  
//                                     m_tabNumber);
        }
    }
}

/**
 * Get the map files for which a palette should be displayed in the 
 * graphcis window.  Note that the order of map files and indices starts
 * with the bottom most layer and ends with the top most overlay.
 *
 * @param mapFiles
 *    Outut Map files that should have a palette displayed in the graphics window.
 * @param mapIndices
 *    Output Indices of the maps in the mapFiles.
 */
void 
BrowserTabContent::getDisplayedPaletteMapFiles(std::vector<CaretMappableDataFile*>& mapFiles,
                                               std::vector<int32_t>& mapIndices)
{
    mapFiles.clear();
    mapIndices.clear();
    
    OverlaySet* overlaySet = getOverlaySet();
    const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t i = (numOverlays - 1); i >= 0; i--) {
        Overlay* overlay = overlaySet->getOverlay(i);
        if (overlay->isEnabled()) {
            if (overlay->isPaletteDisplayEnabled()) {
                CaretMappableDataFile* mapFile;
                int32_t mapFileIndex;
                overlay->getSelectionData(mapFile, mapFileIndex);
                if (mapFile != NULL) {
                    if (mapFile->isMappedWithPalette()) {
                        if ((mapFileIndex >= 0) 
                            && (mapFileIndex < mapFile->getNumberOfMaps())) {
                            mapFiles.push_back(mapFile);
                            mapIndices.push_back(mapFileIndex);
                        }
                    }
                }
            }
        }
    }
}

/**
 * @return The volume surface outline model for this tab.
 */
VolumeSurfaceOutlineSetModel* 
BrowserTabContent::getVolumeSurfaceOutlineSet()
{
    return m_volumeSurfaceOutlineSetModel;
}

/**
 * @return The volume surface outline model for this tab.
 */
const VolumeSurfaceOutlineSetModel* 
BrowserTabContent::getVolumeSurfaceOutlineSet() const
{
    return m_volumeSurfaceOutlineSetModel;
}

/**
 * @return The model yoking group (NULL if NOT yoked).
 * NOTE: This just returns the selected yoking model, it does
 * not indicate if the yoking is compatible with a brain model.
 * In most cases, getSelectedYokingGroupForModel() is the
 * appropriate method to use.
 */
const ModelYokingGroup* 
BrowserTabContent::getSelectedYokingGroup() const
{
    return m_selectedYokingGroup;
}

/**
 * @return The model yoking group (NULL if NOT yoked).
 * NOTE: This just returns the selected yoking model, it does
 * not indicate if the yoking is compatible with a brain model.
 * In most cases, getSelectedYokingGroupForModel() is the
 * appropriate method to use.
 */
ModelYokingGroup* 
BrowserTabContent::getSelectedYokingGroup()
{
    return m_selectedYokingGroup;
}

/**
 * If yoking is selected and the yoking is appropriate for
 * the given model, return the selected yoking group.  Otherwise,
 * return NULL.
 * 
 * @param model
 *     Model which is tested for compatibility with selected yoking.
 * @return The model yoking group if yoking is selected and valid
 *     for the given model, else NULL.
 */
const ModelYokingGroup* 
BrowserTabContent::getSelectedYokingGroupForModel(const Model* model) const
{
    ModelYokingGroup* myg = m_selectedYokingGroup;
    if (myg != NULL) {
        if (model->isYokeable()) {
            switch (model->getControllerType()) {
                case ModelTypeEnum::MODEL_TYPE_INVALID:
                    break;
                case ModelTypeEnum::MODEL_TYPE_SURFACE:
                case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                    if (myg->isSurfaceYoking() == false) {
                        myg = NULL;
                    }
                    break;
                case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                    if (myg->isVolumeYoking() == false) {
                        myg = NULL;
                    }
                    break;
                case ModelTypeEnum::MODEL_TYPE_YOKING:
                    break;
            }
        }
    }
    
    return myg;
}

/**
 * If yoking is selected and the yoking is appropriate for
 * the given model, return the selected yoking group.  Otherwise,
 * return NULL.
 * 
 * @param model
 *     Model which is tested for compatibility with selected yoking.
 * @return The model yoking group if yoking is selected and valid
 *     for the given model, else NULL.
 */
ModelYokingGroup* 
BrowserTabContent::getSelectedYokingGroupForModel(const Model* model)
{
    ModelYokingGroup* myg = m_selectedYokingGroup;
    if (myg != NULL) {
        if (model->isYokeable()) {
            switch (model->getControllerType()) {
                case ModelTypeEnum::MODEL_TYPE_INVALID:
                    break;
                case ModelTypeEnum::MODEL_TYPE_SURFACE:
                case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                    if (myg->isSurfaceYoking() == false) {
                        myg = NULL;
                    }
                    break;
                case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                    if (myg->isVolumeYoking() == false) {
                        myg = NULL;
                    }
                    break;
                case ModelTypeEnum::MODEL_TYPE_YOKING:
                    break;
            }
        }
    }
    
    return myg;
}

/**
 * Set the model yoking group to the given value.
 * @param selectedYokingGroup
 *     New value for yoking group.
 */
void 
BrowserTabContent::setSelectedYokingGroup(ModelYokingGroup* selectedYokingGroup)
{
    m_selectedYokingGroup = selectedYokingGroup;
}

/**
 * Get the data files displayed in this tab.
 * @param displayedDataFilesOut
 *    Displayed data file are loaded into this parameter.
 */
void 
BrowserTabContent::getFilesDisplayedInTab(std::vector<CaretDataFile*>& displayedDataFilesOut)
{
    displayedDataFilesOut.clear();
 
    Model* model = getModelControllerForDisplay();
    if (model == NULL) {
        return;
    }
    
    std::set<CaretDataFile*> displayedDataFiles;
    
    const int32_t tabIndex = getTabNumber();
    switch (getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
        {
            ModelSurface* ms = getDisplayedSurfaceModel();
            displayedDataFiles.insert(ms->getSurface());
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
        {
            ModelSurfaceMontage* msm = getDisplayedSurfaceMontageModel();
            if (msm->isFirstSurfaceEnabled(tabIndex)) {
                if (msm->isLeftEnabled(tabIndex)) {
                    displayedDataFiles.insert(msm->getLeftSurfaceSelectionModel(tabIndex)->getSurface());
                }
                if (msm->isRightEnabled(tabIndex)) {
                    displayedDataFiles.insert(msm->getRightSurfaceSelectionModel(tabIndex)->getSurface());
                }
            }
            if (msm->isSecondSurfaceEnabled(tabIndex)) {
                if (msm->isLeftEnabled(tabIndex)) {
                    displayedDataFiles.insert(msm->getLeftSecondSurfaceSelectionModel(tabIndex)->getSurface());
                }
                if (msm->isRightEnabled(tabIndex)) {
                    displayedDataFiles.insert(msm->getRightSecondSurfaceSelectionModel(tabIndex)->getSurface());
                }
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
        {
            const int32_t numOutlines = m_volumeSurfaceOutlineSetModel->getNumberOfDislayedVolumeSurfaceOutlines();
            for (int32_t i = 0; i < numOutlines; i++) {
                VolumeSurfaceOutlineModel* model = m_volumeSurfaceOutlineSetModel->getVolumeSurfaceOutlineModel(i);
                if (model->isDisplayed()) {
                    displayedDataFiles.insert(model->getSurface());
                }
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
        {
            ModelWholeBrain* wbm = getDisplayedWholeBrainModel();
            if (wbm->isLeftEnabled(tabIndex)) {
                displayedDataFiles.insert(wbm->getSelectedSurface(StructureEnum::CORTEX_LEFT, tabIndex));
            }
            if (wbm->isRightEnabled(tabIndex)) {
                displayedDataFiles.insert(wbm->getSelectedSurface(StructureEnum::CORTEX_RIGHT, tabIndex));
            }
            if (wbm->isCerebellumEnabled(tabIndex)) {
                displayedDataFiles.insert(wbm->getSelectedSurface(StructureEnum::CEREBELLUM, tabIndex));
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            break;
    }

    /*
     * Check overlay files
     */
    OverlaySet* overlaySet = model->getOverlaySet(tabIndex);
    const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        Overlay* overlay = overlaySet->getOverlay(i);
        CaretMappableDataFile* overlayDataFile = NULL;
        int32_t mapIndex;
        overlay->getSelectionData(overlayDataFile, 
                                  mapIndex);
        displayedDataFiles.insert(overlayDataFile);
    }
    
    
    /*
     * Check border files
     */
    Brain* brain = model->getBrain();
    const DisplayPropertiesBorders* borderDisplayProperties = brain->getDisplayPropertiesBorders();
    const DisplayGroupEnum::Enum borderDisplayGroup = borderDisplayProperties->getDisplayGroupForTab(tabIndex);
    if (borderDisplayProperties->isDisplayed(borderDisplayGroup,
                                             tabIndex)) {
        const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
        for (int32_t i = 0; i < numBorderFiles; i++) {
            BorderFile* borderFile = brain->getBorderFile(i);
            const GroupAndNameHierarchyModel* classAndNameSelection = borderFile->getGroupAndNameHierarchyModel();
            if (classAndNameSelection->isSelected(borderDisplayGroup,
                                                  tabIndex)) {
                displayedDataFilesOut.push_back(borderFile);
            }
        }
    }

    /*
     * Check foci files
     */
    const DisplayPropertiesFoci* fociDisplayProperties = brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum fociDisplayGroup = fociDisplayProperties->getDisplayGroupForTab(tabIndex);
    
    if (fociDisplayProperties->isDisplayed(fociDisplayGroup,
                                           tabIndex)) {
        const int32_t numFociFiles = brain->getNumberOfFociFiles();
        for (int32_t i = 0; i < numFociFiles; i++) {
            FociFile* fociFile = brain->getFociFile(i);
            
            const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
            if (classAndNameSelection->isSelected(fociDisplayGroup,
                                                  tabIndex)) {
                displayedDataFilesOut.push_back(fociFile);
            }
        }
    }
    
    /*
     * Might be NULLs so filter them out and return the results
     */
    for (std::set<CaretDataFile*>::iterator iter = displayedDataFiles.begin();
         iter != displayedDataFiles.end();
         iter++) {
        CaretDataFile* cdf = *iter;
        if (cdf != NULL) {
            displayedDataFilesOut.push_back(cdf);
        }
    }
}

/**
 * @return The viewing translation.
 */
const float*
BrowserTabContent::getTranslation() const
{
    if (isVolumeSlicesDisplayed()) {
        return m_volumeSliceTranslation;
    }
    else {
        return m_translation;
    }
}

/**
 * Get the viewing translation.
 *
 * @param translationOut
 *    Translation values output.
 */
void
BrowserTabContent::getTranslation(float translationOut[3]) const
{
    if (isVolumeSlicesDisplayed()) {
        translationOut[0] = m_volumeSliceTranslation[0];
        translationOut[1] = m_volumeSliceTranslation[1];
        translationOut[2] = m_volumeSliceTranslation[2];
    }
    else {
        translationOut[0] = m_translation[0];
        translationOut[1] = m_translation[1];
        translationOut[2] = m_translation[2];
    }
}

/**
 * Set the viewing translation.
 *
 * @param translation
 *    New translation values.
 */
void
BrowserTabContent::setTranslation( const float translation[3])
{
    if (isVolumeSlicesDisplayed()) {
        m_volumeSliceTranslation[0] = translation[0];
        m_volumeSliceTranslation[1] = translation[1];
        m_volumeSliceTranslation[2] = translation[2];
    }
    else {
        m_translation[0] = translation[0];
        m_translation[1] = translation[1];
        m_translation[2] = translation[2];
    }
}

/**
 * Set the viewing translation.
 *
 * @param translationX
 *    New translation X-value.
 * @param translationY
 *    New translation Y-value.
 * @param translationZ
 *    New translation Z-value.
 */
void
BrowserTabContent::setTranslation(const float translationX,
                                  const float translationY,
                                  const float translationZ)
{
    if (isVolumeSlicesDisplayed()) {        
        m_volumeSliceTranslation[0] = translationX;
        m_volumeSliceTranslation[1] = translationY;
        m_volumeSliceTranslation[2] = translationZ;
    }
    else {
        m_translation[0] = translationX;
        m_translation[1] = translationY;
        m_translation[2] = translationZ;
    }
}

/**
 * @return The viewing scaling.
 */
float
BrowserTabContent::getScaling() const
{
    if (isVolumeSlicesDisplayed()) {
        return m_volumeSliceScaling;
    }
    else {
        return m_scaling;
    }
}

/**
 * Set the viewing scaling.
 * @param scaling
 *    New value for scaling.
 */
void
BrowserTabContent::setScaling(const float scaling)
{
    if (isVolumeSlicesDisplayed()) {
        m_volumeSliceScaling = scaling;
    }
    else {
        m_scaling = scaling;
    }
}

/**
 * @return The viewing rotation matrix.
 */
Matrix4x4*
BrowserTabContent::getViewingRotationMatrix()
{
    if (isVolumeSlicesDisplayed()) {
        return m_volumeSliceRotationMatrix;
    }
    else {
        return m_rotationMatrix;
    }
}

/**
 * @return The viewing rotation matrix.
 */
const Matrix4x4*
BrowserTabContent::getViewingRotationMatrix() const
{
    if (isVolumeSlicesDisplayed()) {
        return m_volumeSliceRotationMatrix;
    }
    else {
        return m_rotationMatrix;
    }
}

/**
 * Reset the view to the default view.
 */
void
BrowserTabContent::resetView()
{
    if (isVolumeSlicesDisplayed()) {
        setTranslation(0.0, 0.0, 0.0);
        m_volumeSliceRotationMatrix->identity();
        setScaling(1.0);
    }
    else {
        setTranslation(0.0, 0.0, 0.0);
        m_rotationMatrix->identity();
        setScaling(1.0);
        leftView();
    }
}

/**
 * Set to a right side view.
 */
void
BrowserTabContent::rightView()
{
    if (isVolumeSlicesDisplayed()) {
        /* Nothing */
    }
    else {
        m_rotationMatrix->identity();
        m_rotationMatrix->rotateY(-90.0);
        m_rotationMatrix->rotateZ(-90.0);
    }

//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateZ(-90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateZ(-90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateZ(-90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateZ(-90.0);
}

/**
 * set to a left side view.
 */
void
BrowserTabContent::leftView()
{
    if (isVolumeSlicesDisplayed()) {
        /* Nothing */
    }
    else {
        m_rotationMatrix->identity();
        m_rotationMatrix->rotateY(90.0);
        m_rotationMatrix->rotateZ(90.0);
    }
    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateZ(90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateZ(90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateZ(90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateZ(90.0);
}

/**
 * set to a anterior view.
 */
void
BrowserTabContent::anteriorView()
{
    if (isVolumeSlicesDisplayed()) {
        /* Nothing */
    }
    else {
        m_rotationMatrix->identity();
        m_rotationMatrix->rotateX(-90.0);
        m_rotationMatrix->rotateY(180.0);
    }
    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateX(-90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateX(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(180.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateX(-90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateX(-90.0);
}

/**
 * set to a posterior view.
 */
void
BrowserTabContent::posteriorView()
{
    if (isVolumeSlicesDisplayed()) {
        /* Nothing */
    }
    else {
        m_rotationMatrix->identity();
        m_rotationMatrix->rotateX(-90.0);
    }
    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateX(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(180.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateX(-90.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateX(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(-180.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateX(-90.0);
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(-180.0);
}

/**
 * set to a dorsal view.
 */
void
BrowserTabContent::dorsalView()
{
    if (isVolumeSlicesDisplayed()) {
        /* Nothing */
    }
    else {
        m_rotationMatrix->identity();
    }
    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].rotateY(-180.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].rotateY(180.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].rotateY(180.0);
}

/**
 * set to a ventral view.
 */
void
BrowserTabContent::ventralView()
{
    if (isVolumeSlicesDisplayed()) {
        /* Nothing */
    }
    else {
        m_rotationMatrix->identity();
        m_rotationMatrix->rotateY(-180.0);
    }
    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED].identity();
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].identity();
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE].rotateY(-180.0);
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT].identity();
//    
//    m_viewingRotationMatrix[windowTabNumber][VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE].identity();
}

/**
 * Apply mouse rotation to the displayed model.
 *
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    X coordinate of where mouse was pressed.
 * @param mouseDX
 *    Change in mouse X coordinate.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseRotation(const int32_t mousePressX,
                                      const int32_t mousePressY,
                                      const int32_t mouseDX,
                                      const int32_t mouseDY)
{
    if (isVolumeSlicesDisplayed()) {
        /* Volume slices are not rotated */
    }
    else {
//        if (isDisplayedModelSurfaceRightLateralMedialYoked()) {
//            m_rotationMatrix->rotateX(-mouseDY);
//            m_rotationMatrix->rotateY(-mouseDX);
//        }
        if (getProjectionViewType() == ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL) {
            m_rotationMatrix->rotateX(-mouseDY);
            m_rotationMatrix->rotateY(-mouseDX);
        }
        else {
            float dx = mouseDX;
            float dy = mouseDY;
            
            ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
            if (montageModel != NULL) {
                std::vector<SurfaceMontageViewport> montageViewports;
                montageModel->getMontageViewports(getTabNumber(),
                                                  montageViewports);
                
                bool isValid = false;
                bool isLeft = true;
                bool isLateral = true;
                const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
                for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                    const SurfaceMontageViewport& smv = montageViewports[ivp];
                    if (smv.isInside(mousePressX,
                                     mousePressY)) {
                        switch (smv.projectionViewType) {
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
                                isLeft = true;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
                                isLeft = true;
                                isLateral = false;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
                                isLeft = false;
                                break;
                            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
                                isLeft = false;
                                isLateral = false;
                                break;
                        }
                        isValid = true;
//                        if (StructureEnum::isLeft(smv.structure)) {
//                            isValid = true;
//                            isLeft  = true;
//                        }
//                        else if (StructureEnum::isRight(smv.structure)) {
//                            isValid = true;
//                            isLeft  = false;
//                        }
//                        
//                        if (isValid) {
//                            switch (smv.viewingMatrixIndex) {
//                                case Model::VIEWING_TRANSFORM_COUNT:
//                                    isValid = false;
//                                    break;
//                                case Model::VIEWING_TRANSFORM_NORMAL:
//                                    isLateral = true;
//                                    break;
//                                case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
//                                    isValid = false;
//                                    break;
//                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
//                                    isLateral = false;
//                                    break;
//                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
//                                    isLateral = true;
//                                    break;
//                                case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
//                                    isLateral = false;
//                                    break;
//                            }
//                        }
                    }
                    
                    if (isValid) {
                        break;
                    }
                }
                
                if (isValid) {
                    if (isLeft == false) {
                        dx = -dx;
                    }
                    if (isLateral == false) {
                        dy = -dy;
                    }
                }
            }
            
            m_rotationMatrix->rotateX(-dy);
            m_rotationMatrix->rotateY(dx);
        }
    }
}

/**
 * Apply mouse scaling to the displayed model.
 *
 * @param mouseDX
 *    Change in mouse X coordinate.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseScaling(const int32_t mouseDX,
                                     const int32_t mouseDY)
{
    if (isVolumeSlicesDisplayed()) {
        if (mouseDY != 0.0) {
            m_volumeSliceScaling *= (1.0f + (mouseDY * 0.01));
        }
        if (m_volumeSliceScaling < 0.01) {
            m_volumeSliceScaling = 0.01;
        }
    }
    else {
        if (mouseDY != 0.0) {
            m_scaling *= (1.0f + (mouseDY * 0.01));
        }
        if (m_scaling < 0.01) {
            m_scaling = 0.01;
        }
    }
}

/**
 * Apply mouse translation to the displayed model.
 *
 * @param viewportContent
 *    Content of the viewport.
 * @param mousePressX
 *    X coordinate of where mouse was pressed.
 * @param mousePressY
 *    X coordinate of where mouse was pressed.
 * @param mouseDX
 *    Change in mouse X coordinate.
 * @param mouseDY
 *    Change in mouse Y coordinate.
 */
void
BrowserTabContent::applyMouseTranslation(BrainOpenGLViewportContent* viewportContent,
                                         const int32_t mousePressX,
                                         const int32_t mousePressY,
                                         const int32_t mouseDX,
                                         const int32_t mouseDY)
{
    const int tabIndex = getTabNumber();
    
    if (isVolumeSlicesDisplayed()) {
        ModelVolume* modelVolume = getDisplayedVolumeModel();
        VolumeFile* vf = modelVolume->getUnderlayVolumeFile(tabIndex);
        BoundingBox mybox = vf->getSpaceBoundingBox();
        float cubesize = std::max(std::max(mybox.getDifferenceX(), mybox.getDifferenceY()), mybox.getDifferenceZ());//factor volume bounding box into slowdown for zoomed in
        //float slowdown = 0.005f * cubesize / modelVolume->getScaling(tabIndex);//when zoomed in, make the movements slower to match - still changes based on viewport currently
        float slowdown = 0.005f * cubesize / m_volumeSliceScaling;//when zoomed in, make the movements slower to match - still changes based on viewport currently
        
        float dx = 0.0;
        float dy = 0.0;
        float dz = 0.0;
        switch (modelVolume->getSliceViewPlane(tabIndex))
        {
            case VolumeSliceViewPlaneEnum::ALL:
            {
                int viewport[4];
                viewportContent->getModelViewport(viewport);
                const int32_t halfWidth  = viewport[2] / 2;
                const int32_t halfHeight = viewport[3] / 2;
                const int32_t viewportMousePressedX = mousePressX - viewport[0];
                const int32_t viewportMousePressedY = mousePressY - viewport[1];
                bool isRight  = false;
                bool isTop = false;
                if (viewportMousePressedX > halfWidth) {
                    isRight = true;
                }
                if (viewportMousePressedY > halfHeight) {
                    isTop = true;
                }
                //CaretLogInfo("right: " + AString::fromBool(isRight) + " top: " + AString::fromBool(isTop));
                if (isTop)
                {
                    if (isRight)//coronal
                    {
                        dx = mouseDX * slowdown;
                        dz = mouseDY * slowdown;
                    } else {//parasaggital
                        dy = -mouseDX * slowdown;
                        dz = mouseDY * slowdown;
                    }
                } else {
                    if (isRight)//axial
                    {
                        dx = mouseDX * slowdown;
                        dy = mouseDY * slowdown;
                    }//bottom left has no slice
                }
                break;
            }
            case VolumeSliceViewPlaneEnum::AXIAL:
                dx = mouseDX * slowdown;
                dy = mouseDY * slowdown;
                break;
            case VolumeSliceViewPlaneEnum::CORONAL:
                dx = mouseDX * slowdown;
                dz = mouseDY * slowdown;
                break;
            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                dy = -mouseDX * slowdown;
                dz = mouseDY * slowdown;
                break;
        }
        
        m_volumeSliceTranslation[0] += dx;
        m_volumeSliceTranslation[1] += dy;
        m_volumeSliceTranslation[2] += dz;
    }
    else {
        float dx = mouseDX;
        float dy = mouseDY;
        
        ModelSurfaceMontage* montageModel = getDisplayedSurfaceMontageModel();
        if (montageModel != NULL) {
            std::vector<SurfaceMontageViewport> montageViewports;
            montageModel->getMontageViewports(getTabNumber(),
                                              montageViewports);
            
            bool isValid = false;
            bool isLeft = true;
            bool isLateral = true;
            const int32_t numViewports = static_cast<int32_t>(montageViewports.size());
            for (int32_t ivp = 0; ivp < numViewports; ivp++) {
                const SurfaceMontageViewport& smv = montageViewports[ivp];
                if (smv.isInside(mousePressX,
                                 mousePressY)) {
                    switch (smv.projectionViewType) {
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
                            isLeft = true;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
                            isLeft = true;
                            isLateral = false;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
                            isLeft = false;
                            break;
                        case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
                            isLeft = false;
                            isLateral = false;
                            break;
                    }
                    isValid = true;
//                    if (StructureEnum::isLeft(smv.structure)) {
//                        isValid = true;
//                        isLeft  = true;
//                    }
//                    else if (StructureEnum::isRight(smv.structure)) {
//                        isValid = true;
//                        isLeft  = false;
//                    }
//                    
//                    if (isValid) {
//                        switch (smv.viewingMatrixIndex) {
//                            case Model::VIEWING_TRANSFORM_COUNT:
//                                isValid = false;
//                                break;
//                            case Model::VIEWING_TRANSFORM_NORMAL:
//                                isLateral = true;
//                                break;
//                            case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
//                                isValid = false;
//                                break;
//                            case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
//                                isLateral = false;
//                                break;
//                            case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
//                                isLateral = true;
//                                break;
//                            case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
//                                isLateral = false;
//                                break;
//                        }
//                    }
                }
                
                if (isValid) {
                    break;
                }
            }
            
            if (isValid) {
                if (isLeft == false) {
                    dx = -dx;
                }
                if (isLateral == false) {
                    dx = -dx;
                }
                
                m_translation[0] += dx;
                m_translation[1] += dy;
            }
        }
        else {
            if (getProjectionViewType() == ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL) {
                dx = -dx;
            }
//            if (isDisplayedModelSurfaceRightLateralMedialYoked()) {
//                dx = -dx;
//            }
            m_translation[0] += dx;
            m_translation[1] += dy;
        }
    }
}

/**
 * Get the transformations for drawing a model.
 *
 * @param projectionViewType
 *    Type of projection view
 * @param translationOut
 *    Translation
 * @param rotationMatrixOut
 *    OpenGL rotation matrix.
 * @param scalingOut
 *    Scaling.
 */
void
BrowserTabContent::getTransformationsForOpenGLDrawing(const ProjectionViewTypeEnum::Enum projectionViewType,
                                                      float translationOut[3],
                                                      double rotationMatrixOut[16],
                                                      float& scalingOut) const
{
    if (isVolumeSlicesDisplayed()) {
        translationOut[0] = m_volumeSliceTranslation[0];
        translationOut[1] = m_volumeSliceTranslation[1];
        translationOut[2] = m_volumeSliceTranslation[2];
        
        m_volumeSliceRotationMatrix->getMatrixForOpenGL(rotationMatrixOut);
        
        scalingOut = m_volumeSliceScaling;
    }
    else {
        translationOut[0] = m_translation[0];
        translationOut[1] = m_translation[1];
        translationOut[2] = m_translation[2];
        
        double rotationX, rotationY, rotationZ;
        m_rotationMatrix->getRotation(rotationX,
                                      rotationY,
                                      rotationZ);
        const double rotationFlippedX = -rotationX;
        const double rotationFlippedY = 180.0 - rotationY;
        
        switch (projectionViewType) {
            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL:
                break;
            case ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL:
                break;
            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL:
                rotationX = rotationFlippedX;
                rotationY = rotationFlippedY;
                break;
            case ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL:
                rotationX = rotationFlippedX;
                rotationY = rotationFlippedY;
                break;
        }
//            case Model::VIEWING_TRANSFORM_COUNT:
//                break;
//            case Model::VIEWING_TRANSFORM_NORMAL:
//            {
//                const Model* model = getModelControllerForDisplay();
//                if (model != NULL) {
//                    const ModelSurface* surfaceModel = dynamic_cast<const ModelSurface*>(model);
//                    if (surfaceModel != NULL) {
//                        const Surface* surface = surfaceModel->getSurface();
//                        StructureEnum::Enum structure = surface->getStructure();
//                        if (StructureEnum::isRight(structure)) {
//                            rotationX = rotationFlippedX;
//                            rotationY = rotationFlippedY;
//                        }
//                    }
//                }
//            }
//                break;
//            case Model::VIEWING_TRANSFORM_RIGHT_LATERAL_MEDIAL_YOKED:
//                rotationX = rotationFlippedX;
//                rotationY = rotationFlippedY;
//                break;
//            case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_LEFT_OPPOSITE:
//                break;
//            case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT:
//                rotationX = rotationFlippedX;
//                rotationY = rotationFlippedY;
//                break;
//            case Model::VIEWING_TRANSFORM_SURFACE_MONTAGE_RIGHT_OPPOSITE:
//                rotationX = rotationFlippedX;
//                rotationY = rotationFlippedY;
//                break;
//        }
        
        Matrix4x4 matrix;
        matrix.setRotation(rotationX,
                           rotationY,
                           rotationZ);
        matrix.getMatrixForOpenGL(rotationMatrixOut);
        
        scalingOut = m_scaling;
    }
}

/**
 * Place the transformations for the given window tab into
 * the model transform.
 * @param windowTabNumber
 *    Tab number for transformations.
 * @param modelTransform
 *    Model transform into which transformations are loaded.
 */
void
BrowserTabContent::getTransformationsInModelTransform(ModelTransform& modelTransform) const
{
    modelTransform.setTranslation(getTranslation());
    
    const Matrix4x4* rotMatrix = getViewingRotationMatrix();
    float m[4][4];
    rotMatrix->getMatrix(m);
    modelTransform.setRotation(m);
    modelTransform.setScaling(getScaling());
}

/**
 * Apply the transformations to the browser tab.
 * @param modelTransform
 *    Model transform into which transformations are retrieved.
 */
void
BrowserTabContent::setTransformationsFromModelTransform(const ModelTransform& modelTransform)
{
    float translation[3];
    modelTransform.getTranslation(translation);
    const float tx = translation[0];
    const float ty = translation[1];
    const float tz = translation[2];
    
    setTranslation(tx, ty, tz);

    Matrix4x4* rotationMatrix = getViewingRotationMatrix();
    float m[4][4];
    modelTransform.getRotation(m);
    rotationMatrix->setMatrix(m);
    
    const float scale = modelTransform.getScaling();
    setScaling(scale);
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
BrowserTabContent::saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrowserTabContent",
                                            1);

    m_sceneClassAssistant->saveMembers(sceneAttributes, 
                                       sceneClass);
    
    AString yokingGroupName = "";
    if (m_selectedYokingGroup != NULL) {
        yokingGroupName = m_selectedYokingGroup->getYokingName();
    }
    sceneClass->addString("m_selectedYokingGroup", 
                          yokingGroupName);
    
    /*
     * Save rotation matrices.
     */
    float matrix[4][4];
    m_rotationMatrix->getMatrix(matrix);
    sceneClass->addFloatArray("m_rotationMatrix", (float*)matrix, 16);
    m_volumeSliceRotationMatrix->getMatrix(matrix);
    sceneClass->addFloatArray("m_volumeSliceRotationMatrix", (float*)matrix, 16);
    
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
BrowserTabContent::restoreFromScene(const SceneAttributes* sceneAttributes,
                              const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneClassAssistant->restoreMembers(sceneAttributes, 
                                          sceneClass);
    
    /*
     * Restore the selected yoking group
     */
    const AString yokingGroupName = sceneClass->getStringValue("m_selectedYokingGroup", "");
    m_selectedYokingGroup = NULL;
    if (yokingGroupName.isEmpty() == false) {
        EventModelYokingGroupGetAll getYokingGroups;
        EventManager::get()->sendEvent(getYokingGroups.getPointer());
        std::vector<ModelYokingGroup*> yokingGroups;
        getYokingGroups.getYokingGroups(yokingGroups);
        for (std::vector<ModelYokingGroup*>::iterator iter= yokingGroups.begin();
             iter != yokingGroups.end();
             iter++) {
            ModelYokingGroup* myg = *iter;
            if (myg->getYokingName() == yokingGroupName) {
                m_selectedYokingGroup = myg;
                break;
            }
        }
    }
    
    /*
     * Restore rotation matrices.
     */
    float matrix[4][4];
    if (sceneClass->getFloatArrayValue("m_rotationMatrix", (float*)matrix, 16) == 16) {
        m_rotationMatrix->setMatrix(matrix);
    }
    else {
        m_rotationMatrix->identity();
    }
    if (sceneClass->getFloatArrayValue("m_volumeSliceRotationMatrix", (float*)matrix, 16) == 16) {
        m_volumeSliceRotationMatrix->setMatrix(matrix);
    }
    else {
        m_volumeSliceRotationMatrix->identity();
    }
}

/**
 * Is the clipping plane enabled?
 * @param indx
 *    Index of clipping plane
 * @return
 *    true if clipping plane enabled, else false.
 */
bool
BrowserTabContent::isClippingPlaneEnabled(const int32_t indx) const
{
    CaretAssertArrayIndex(m_clippingEnabled, 3, indx);
    return m_clippingEnabled[indx];
}

/**
 * Set the clipping plane enabled
 * @param indx
 *    Index of clipping plane
 * status
 *    true if clipping plane enabled, else false.
 */
void
BrowserTabContent::setClippingPlaneEnabled(const int32_t indx,
                                           const bool status)
{
    CaretAssertArrayIndex(m_clippingEnabled, 3, indx);
    m_clippingEnabled[indx] = status;
}

/**
 * Get the clipping plane thickness.
 * @param indx
 *    Index of clipping plane
 * @return
 *    Thickeness of the clipping plane.
 */
float
BrowserTabContent::getClippingPlaneThickness(const int32_t indx) const
{
    CaretAssertArrayIndex(m_clippingThickness, 3, indx);
    return m_clippingThickness[indx];
}

/**
 * Set the clipping plane thickness.
 * @param indx
 *    Index of clipping plane
 * @param value
 *    Thickeness of the clipping plane.
 */
void
BrowserTabContent::setClippingPlaneThickness(const int32_t indx,
                                             const float value)
{
    CaretAssertArrayIndex(m_clippingThickness, 3, indx);
    m_clippingThickness[indx] = value;
}

/**
 * Get the clipping plane coordinate
 * @param indx
 *    Index of clipping plane
 * @return
 *    Coordinate of the clipping plane.
 */
float
BrowserTabContent::getClippingPlaneCoordinate(const int32_t indx) const
{
    CaretAssertArrayIndex(m_clippingCoordinate, 3, indx);
    return m_clippingCoordinate[indx];
}

/**
 * Set the clipping plane coordinate.
 * @param indx
 *    Index of clipping plane
 * @param value
 *    Coordinate of the clipping plane.
 */
void
BrowserTabContent::setClippingPlaneCoordinate(const int32_t indx,
                                const float value)
{
    CaretAssertArrayIndex(m_clippingCoordinate, 3, indx);
    m_clippingCoordinate[indx] = value;
}

/**
 * @return the projection view type (view from left/right)
 */
ProjectionViewTypeEnum::Enum
BrowserTabContent::getProjectionViewType() const
{
    ProjectionViewTypeEnum::Enum projectionViewType = ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL;
    
    const ModelSurface* modelSurface = getDisplayedSurfaceModel();
    if (modelSurface != NULL) {
        const SurfaceFile* surfaceFile = modelSurface->getSurface();
        if (surfaceFile != NULL) {
            const StructureEnum::Enum structure = surfaceFile->getStructure();
            if (StructureEnum::isRight(structure)) {
                projectionViewType = ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL;
            }
        }
    }
    
    return projectionViewType;
}



