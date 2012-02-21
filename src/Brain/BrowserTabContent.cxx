
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

#define __BROWSER_TAB_CONTENT_DECLARE__
#include "BrowserTabContent.h"
#undef __BROWSER_TAB_CONTENT_DECLARE__

#include "BrowserTabYoking.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventModelDisplayControllerGetAll.h"
#include "EventManager.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerSurfaceSelector.h"
#include "ModelDisplayControllerVolume.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "ModelDisplayControllerYokingGroup.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Surface.h"
#include "StructureEnum.h"

using namespace caret;


/**
 * Constructor.
 * @param tabNumber
 *    Number for this tab.
 */
BrowserTabContent::BrowserTabContent(const int32_t tabNumber,
                                     ModelDisplayControllerYokingGroup* defaultYokingGroup)
: CaretObject()
{
    this->tabNumber = tabNumber;
    this->surfaceModelSelector = new ModelDisplayControllerSurfaceSelector();
    this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID;
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    this->guiName = "";
    this->userName = "";
    this->browserTabYoking = new BrowserTabYoking(this, defaultYokingGroup);
}

/**
 * Destructor.
 */
BrowserTabContent::~BrowserTabContent()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete this->browserTabYoking;
    this->browserTabYoking = NULL;

    delete this->surfaceModelSelector;
    this->surfaceModelSelector = NULL;
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
        const ModelDisplayController* displayedController =
            this->getModelControllerForDisplay();
        if (displayedController != NULL) {
            const AString name = displayedController->getNameForBrowserTab();
            s += name;
        }
    }
    
    return s;
}

/*
 * Set the name of this tab.  This is typically
 * called by the user-interface with a name that
 * indicates the content of the tab.
 *
 * @param guiName
 *    New name for tab.
 *
void 
BrowserTabContent::setGuiName(const AString& guiName)
{
    this->guiName = guiName;
}
*/

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
ModelDisplayControllerTypeEnum::Enum 
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
BrowserTabContent::setSelectedModelType(ModelDisplayControllerTypeEnum::Enum selectedModelType)
{
    this->selectedModelType = selectedModelType;
}

/**
 * Get the model controller for DISPLAY purposes.  Note: When applying
 * transformations, use getModelControllerForTransformation() so that
 * any yoking is properly carried out.
 * 
 * @return  Pointer to displayed controller or NULL
 *          if none are available.
 */   
ModelDisplayController* 
BrowserTabContent::getModelControllerForDisplay()
{
    ModelDisplayController* mdc = NULL;
    
    switch (this->selectedModelType) {
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE:
            mdc = this->surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = this->volumeModel;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = this->wholeBrainModel;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_YOKING:
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
const ModelDisplayController* 
BrowserTabContent::getModelControllerForDisplay() const
{
    ModelDisplayController* mdc = NULL;
    
    switch (this->selectedModelType) {
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE:
            mdc = this->surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = this->volumeModel;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = this->wholeBrainModel;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    return mdc;
}

/**
 * Get the model controller for TRANSFORMATION purposes.  If yoked
 * to a different tab, this will return the model controller for
 * the tab to which this is yoked.
 * 
 * @return  Pointer to transformation controller or NULL
 *          if none are available.
 */   
ModelDisplayController* 
BrowserTabContent::getModelControllerForTransformation()
{
    ModelDisplayController* mdc = this->getModelControllerForDisplay();
    if (mdc == NULL) {
        return NULL;
    }
    
    if (mdc->isYokeable()) {
        ModelDisplayControllerYokingGroup* mdcyg = this->browserTabYoking->getSelectedYokingGroup();
        if (mdcyg->getYokingType() != YokingTypeEnum::OFF) {
            mdc = mdcyg;
        }
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
    
    const ModelDisplayControllerSurface* surfaceController = this->getDisplayedSurfaceModel();
    if (surfaceController != NULL) {
        const Surface* surface = surfaceController->getSurface();
        if (surface->getStructure() == StructureEnum::CORTEX_RIGHT) {
            ModelDisplayControllerYokingGroup* mdcyg = this->browserTabYoking->getSelectedYokingGroup();
            if (mdcyg->getYokingType() == YokingTypeEnum::ON_LATERAL_MEDIAL) {
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
ModelDisplayControllerSurface* 
BrowserTabContent::getDisplayedSurfaceModel()
{
    ModelDisplayControllerSurface* mdcs =
        dynamic_cast<ModelDisplayControllerSurface*>(this->getModelControllerForDisplay());
    return mdcs;
}

/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is NOT a 
 *          surface.
 */   
const ModelDisplayControllerSurface* 
BrowserTabContent::getDisplayedSurfaceModel() const
{
    const ModelDisplayControllerSurface* mdcs =
    dynamic_cast<const ModelDisplayControllerSurface*>(this->getModelControllerForDisplay());
    return mdcs;
}

/**
 * Get the displayed volume model.
 * 
 * @return  Pointer to displayed volume model or 
 *          NULL if the displayed model is NOT a 
 *          volume.
 */   
ModelDisplayControllerVolume* 
BrowserTabContent::getDisplayedVolumeModel()
{
    ModelDisplayControllerVolume* mdcv =
        dynamic_cast<ModelDisplayControllerVolume*>(this->getModelControllerForDisplay());
    return mdcv;
}

/**
 * Get the displayed whole brain model.
 * 
 * @return  Pointer to displayed whole brain model or 
 *          NULL if the displayed model is NOT a 
 *          whole brain.
 */   
ModelDisplayControllerWholeBrain* 
BrowserTabContent::getDisplayedWholeBrainModel()
{
    ModelDisplayControllerWholeBrain* mdcwb =
        dynamic_cast<ModelDisplayControllerWholeBrain*>(this->getModelControllerForDisplay());
    return mdcwb;

}

/**
 * Get the displayed volume model.
 * 
 * @return  Pointer to displayed volume model or 
 *          NULL if the displayed model is not a 
 *          volume.
 */   
ModelDisplayControllerVolume* 
BrowserTabContent::getSelectedVolumeModel()
{
    ModelDisplayControllerVolume* mdcv =
        dynamic_cast<ModelDisplayControllerVolume*>(this->getModelControllerForDisplay());
    return mdcv;
}

/**
 * Get the displayed whole brain model.
 * 
 * @return  Pointer to displayed whole brain model or 
 *          NULL if the displayed model is not a 
 *          whole brain.
 */   
ModelDisplayControllerWholeBrain* 
BrowserTabContent::getSelectedWholeBrainModel()
{
    ModelDisplayControllerWholeBrain* mdcwb =
        dynamic_cast<ModelDisplayControllerWholeBrain*>(this->getModelControllerForDisplay());
    return mdcwb;
}

/**
 * Get all of the available surface models.
 * 
 * @return Vector containing all surface models.
 */   
const std::vector<ModelDisplayControllerSurface*> 
BrowserTabContent::getAllSurfaceModels() const
{
    return this->allSurfaceModels;
}

/**
 * @return The surface model selector used to 
 * select a surface and structure.
 */
ModelDisplayControllerSurfaceSelector* 
BrowserTabContent::getSurfaceModelSelector()
{
    return this->surfaceModelSelector;
}

/**
 * Get the overlay assignments for this tab.
 * 
 * @return  Overlay assignments for this tab.
 */
OverlaySet* 
BrowserTabContent::getOverlaySet()
{
    ModelDisplayController* modelDisplayController = this->getModelControllerForDisplay();
    return modelDisplayController->getOverlaySet(this->tabNumber);
}

/**
 * @return The yoking for this browser tab.
 */
BrowserTabYoking* 
BrowserTabContent::getBrowserTabYoking()
{
    return this->browserTabYoking;
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
BrowserTabContent::update(const std::vector<ModelDisplayController*> modelDisplayControllers)
{
    this->surfaceModelSelector->updateSelector(modelDisplayControllers);
    
    const int32_t numModels = static_cast<int32_t>(modelDisplayControllers.size());
    
    this->allSurfaceModels.clear();
    this->surfaceModelSelector->getSelectableSurfaceControllers(allSurfaceModels);
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    
    for (int i = 0; i < numModels; i++) {
        ModelDisplayController* mdc = modelDisplayControllers[i];
        
        ModelDisplayControllerSurface* mdcs = dynamic_cast<ModelDisplayControllerSurface*>(mdc);
        ModelDisplayControllerVolume* mdcv = dynamic_cast<ModelDisplayControllerVolume*>(mdc);
        ModelDisplayControllerWholeBrain* mdcwb = dynamic_cast<ModelDisplayControllerWholeBrain*>(mdc);
        
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
        else {
            CaretAssertMessage(0, (AString("Unknown type of brain model.") + mdc->getNameForGUI(true)));
        }
    }
    
    switch (this->selectedModelType) {
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE:
            if (this->surfaceModelSelector->getSelectedSurfaceController() == NULL) {
                this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            if (this->volumeModel == NULL) {
                this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            if (this->wholeBrainModel == NULL) {
                this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    if (this->selectedModelType == ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID) {
        if (this->wholeBrainModel != NULL) {
            this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN;
        }
        else if (this->surfaceModelSelector->getSelectedSurfaceController() != NULL) {
            this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE;
        }
        else if (this->volumeModel != NULL) {
            this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES;
        }
    }
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
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
BrowserTabContent::receiveEvent(Event* event)
{
}

/**
 * Update the transformations for yoking prior to drawing.
 * Does nothing if not yoked.
 */
void 
BrowserTabContent::updateTransformationsForYoking()
{
    ModelDisplayController* transformController = this->getModelControllerForTransformation();
    ModelDisplayControllerYokingGroup* yokingController = 
    dynamic_cast<ModelDisplayControllerYokingGroup*>(transformController);
    if (yokingController != NULL) {
        ModelDisplayController* mdc = this->getModelControllerForDisplay();
        if (mdc != NULL) {
            mdc->copyTransformations(*yokingController, 
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



