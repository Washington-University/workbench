
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
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ClassAndNameHierarchyModel.h"
#include "DisplayPropertiesBorders.h"
#include "DisplayPropertiesFoci.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "FociFile.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelSurfaceSelector.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "ModelYokingGroup.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "StructureEnum.h"
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
    this->tabNumber = tabNumber;
    this->surfaceModelSelector = new ModelSurfaceSelector();
    this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    this->surfaceMontageModel = NULL;
    this->guiName = "";
    this->userName = "";
    this->volumeSurfaceOutlineSetModel = new VolumeSurfaceOutlineSetModel();
    this->selectedYokingGroup = NULL;
}

/**
 * Destructor.
 */
BrowserTabContent::~BrowserTabContent()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete this->surfaceModelSelector;
    this->surfaceModelSelector = NULL;
    
    delete this->volumeSurfaceOutlineSetModel;
    this->volumeSurfaceOutlineSetModel = NULL;
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
    this->surfaceModelSelector->setSelectedStructure(tabToClone->surfaceModelSelector->getSelectedStructure());
    this->surfaceModelSelector->setSelectedSurfaceController(tabToClone->surfaceModelSelector->getSelectedSurfaceController());

    this->selectedModelType = tabToClone->selectedModelType;
    this->volumeModel = tabToClone->volumeModel;
    this->wholeBrainModel = tabToClone->wholeBrainModel;
    this->surfaceMontageModel = tabToClone->surfaceMontageModel;
    this->selectedYokingGroup = tabToClone->selectedYokingGroup;
    
    Model* model = this->getModelControllerForDisplay();
    Model* modelBeingCloned = tabToClone->getModelControllerForDisplay();
    if ((model != NULL)
        && (modelBeingCloned != NULL)) {
        model->copyTransformationsAndViews(*modelBeingCloned, 
                                           tabToClone->getTabNumber(), 
                                           this->getTabNumber());
    }
    
    const OverlaySet* overlaySetToClone = tabToClone->getOverlaySet();
    if (overlaySetToClone != NULL) {
        OverlaySet* overlaySet = this->getOverlaySet();
        if (overlaySet != NULL) {
            overlaySet->copyOverlaySet(overlaySetToClone);
        }
    }
    
    if (model != NULL) {
        Brain* brain = model->getBrain();
        
        DisplayPropertiesBorders* dpb = brain->getDisplayPropertiesBorders();
        dpb->copyDisplayPropertiesBorders(tabToClone->getTabNumber(),
                                          this->getTabNumber());
        DisplayPropertiesFoci* dpf = brain->getDisplayPropertiesFoci();
        dpf->copyDisplayPropertiesFoci(tabToClone->getTabNumber(),
                                       this->getTabNumber());
        
        const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
        for (int32_t i = 0; i < numBorderFiles; i++) {
            BorderFile* bf = brain->getBorderFile(i);
            bf->getClassAndNameHierarchyModel()->copyClassNameAndHierarchy(tabToClone->getTabNumber(),
                                                                           this->getTabNumber());
        }
        
        const int32_t numFociFiles = brain->getNumberOfFociFiles();
        for (int32_t i = 0; i < numFociFiles; i++) {
            FociFile* ff = brain->getFociFile(i);
            ff->getClassAndNameHierarchyModel()->copyClassNameAndHierarchy(tabToClone->getTabNumber(),
                                                                           this->getTabNumber());
        }
    }
    
    this->volumeSurfaceOutlineSetModel->copyVolumeSurfaceOutlineSetModel(tabToClone->getVolumeSurfaceOutlineSet());
    
    this->updateTransformationsForYoking();
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
    AString s = "(" + AString::number(this->tabNumber + 1) + ") ";
    
    if (this->userName.isEmpty() == false) {
        s += userName;
    }
    else {
        const Model* displayedController =
            this->getModelControllerForDisplay();
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
    this->userName = userName;
}

/**
 * @return The user name.
 */
AString 
BrowserTabContent::getUserName() const
{
    return this->userName;
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
    return this->selectedModelType;
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
    this->selectedModelType = selectedModelType;
    this->updateTransformationsForYoking();
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
    
    switch (this->selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            mdc = this->surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = this->surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = this->volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = this->wholeBrainModel;
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
    
    switch (this->selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            mdc = this->surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = this->surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = this->volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = this->wholeBrainModel;
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
    Model* mdc = this->getModelControllerForDisplay();
    if (mdc == NULL) {
        return NULL;
    }
    
    ModelYokingGroup* myg = this->getSelectedYokingGroupForModel(mdc);
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
    
    const ModelSurface* surfaceController = this->getDisplayedSurfaceModel();
    if (surfaceController != NULL) {
        const Surface* surface = surfaceController->getSurface();
        if (surface->getStructure() == StructureEnum::CORTEX_RIGHT) {
            if (this->getSelectedYokingGroupForModel(surfaceController) != NULL) {
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
        dynamic_cast<ModelSurface*>(this->getModelControllerForDisplay());
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
    dynamic_cast<const ModelSurface*>(this->getModelControllerForDisplay());
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
        dynamic_cast<ModelVolume*>(this->getModelControllerForDisplay());
    return mdcv;
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
        dynamic_cast<ModelWholeBrain*>(this->getModelControllerForDisplay());
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
    dynamic_cast<ModelSurfaceMontage*>(this->getModelControllerForDisplay());
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
    return this->allSurfaceModels;
}

/**
 * @return The surface model selector used to 
 * select a surface and structure.
 */
ModelSurfaceSelector* 
BrowserTabContent::getSurfaceModelSelector()
{
    return this->surfaceModelSelector;
}

/**
 * Get the overlay assignments for this tab.
 * 
 * @return  Overlay assignments for this tab or NULL if no valid model.
 */
OverlaySet* 
BrowserTabContent::getOverlaySet()
{
    Model* modelDisplayController = this->getModelControllerForDisplay();
    if (modelDisplayController != NULL) {
       return modelDisplayController->getOverlaySet(this->tabNumber);
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
    return this->tabNumber;
}

/**
 * Update the selected models.
 */
void 
BrowserTabContent::update(const std::vector<Model*> modelDisplayControllers)
{
    this->surfaceModelSelector->updateSelector(modelDisplayControllers);
    
    const int32_t numModels = static_cast<int32_t>(modelDisplayControllers.size());
    
    this->allSurfaceModels.clear();
    this->surfaceModelSelector->getSelectableSurfaceControllers(allSurfaceModels);
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    this->surfaceMontageModel = NULL;
    
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
            CaretAssertMessage((this->volumeModel == NULL), "There is more than one volume model.");
            this->volumeModel = mdcv;
        }
        else if (mdcwb != NULL) {
            CaretAssertMessage((this->wholeBrainModel == NULL), "There is more than one whole brain model.");
            this->wholeBrainModel = mdcwb;
        }
        else if (mdcsm != NULL) {
            CaretAssertMessage((this->surfaceMontageModel == NULL), "There is more than one surface montage model.");
            this->surfaceMontageModel = mdcsm;
        }
        else {
            CaretAssertMessage(0, (AString("Unknown type of brain model.") + mdc->getNameForGUI(true)));
        }
    }
    
    switch (this->selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            if (this->surfaceModelSelector->getSelectedSurfaceController() == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            if (this->surfaceMontageModel == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            if (this->volumeModel == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            if (this->wholeBrainModel == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    if (this->selectedModelType == ModelTypeEnum::MODEL_TYPE_INVALID) {
        if (this->surfaceModelSelector->getSelectedSurfaceController() != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE;
        }
        else if (this->volumeModel != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES;
        }
        else if (this->wholeBrainModel != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN;
        }
        else if (this->surfaceMontageModel != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE;
        }
    }

    this->updateTransformationsForYoking();
}

/**
 * Is the surface model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isSurfaceModelValid() const
{
    bool valid = (this->allSurfaceModels.empty() == false);
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
    bool valid = (this->volumeModel != NULL);
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
    bool valid = (this->wholeBrainModel != NULL);
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
    bool valid = (this->surfaceMontageModel != NULL);
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
    Model* transformController = this->getModelControllerForTransformation();
    ModelYokingGroup* yokingController = 
        dynamic_cast<ModelYokingGroup*>(transformController);
    if (yokingController != NULL) {
        Model* mdc = this->getModelControllerForDisplay();
        if (mdc != NULL) {
            mdc->copyTransformationsAndViews(*yokingController, 
                                     0, // always used window 0  
                                     this->tabNumber);
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
    
    OverlaySet* overlaySet = this->getOverlaySet();
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
    return this->volumeSurfaceOutlineSetModel;
}

/**
 * @return The volume surface outline model for this tab.
 */
const VolumeSurfaceOutlineSetModel* 
BrowserTabContent::getVolumeSurfaceOutlineSet() const
{
    return this->volumeSurfaceOutlineSetModel;
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
    return this->selectedYokingGroup;
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
    return this->selectedYokingGroup;
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
    ModelYokingGroup* myg = this->selectedYokingGroup;
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
    ModelYokingGroup* myg = this->selectedYokingGroup;
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
    this->selectedYokingGroup = selectedYokingGroup;
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
 
    Model* model = this->getModelControllerForDisplay();
    if (model == NULL) {
        return;
    }
    
    std::set<CaretDataFile*> displayedDataFiles;
    
    const int32_t tabIndex = this->getTabNumber();
    switch (this->getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
        {
            ModelSurface* ms = this->getDisplayedSurfaceModel();
            displayedDataFiles.insert(ms->getSurface());
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
        {
            ModelSurfaceMontage* msm = this->getDisplayedSurfaceMontageModel();
            displayedDataFiles.insert(msm->getLeftSurfaceSelectionModel(tabIndex)->getSurface());
            displayedDataFiles.insert(msm->getRightSurfaceSelectionModel(tabIndex)->getSurface());
            if (msm->isDualConfigurationEnabled(tabIndex)) {
                displayedDataFiles.insert(msm->getLeftSecondSurfaceSelectionModel(tabIndex)->getSurface());
                displayedDataFiles.insert(msm->getRightSecondSurfaceSelectionModel(tabIndex)->getSurface());
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
        {
            const int32_t numOutlines = this->volumeSurfaceOutlineSetModel->getNumberOfDislayedVolumeSurfaceOutlines();
            for (int32_t i = 0; i < numOutlines; i++) {
                VolumeSurfaceOutlineModel* model = this->volumeSurfaceOutlineSetModel->getVolumeSurfaceOutlineModel(i);
                if (model->isDisplayed()) {
                    displayedDataFiles.insert(model->getSurface());
                }
            }
        }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
        {
            ModelWholeBrain* wbm = this->getDisplayedWholeBrainModel();
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
     * Check feature files
     */
    
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



