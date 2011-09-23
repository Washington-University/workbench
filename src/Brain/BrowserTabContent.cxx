
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

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventModelDisplayControllerGetAll.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerVolume.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "Surface.h"
#include "SurfaceNodeColoring.h"
#include "StructureEnum.h"

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
    this->selectedSurfaceModel = NULL;
    this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID;
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    this->guiName = "";
    this->userName = "";
    this->yokeToTabNumber = 0;
    this->yokingType = YokingTypeEnum::OFF;
    
    this->surfaceColoring = new SurfaceNodeColoring();
    
    this->invalidateSurfaceColoring();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE);
}

/**
 * Destructor.
 */
BrowserTabContent::~BrowserTabContent()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete this->surfaceColoring;
    this->surfaceColoring = NULL;
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
    else if (this->guiName.isEmpty() == false) {
        s += this->guiName;   
    }
    
    return s;
}

/**
 * Set the name of this tab.  This is typically
 * called by the user-interface with a name that
 * indicates the content of the tab.
 *
 * @param guiName
 *    New name for tab.
 */
void 
BrowserTabContent::setGuiName(const AString& guiName)
{
    this->guiName = guiName;
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
    this->invalidateSurfaceColoring();
}

/**
 * Get the displayed model controller.
 * 
 * @return  Pointer to displayed controller or NULL
 *          if none are available.
 */   
ModelDisplayController* 
BrowserTabContent::getDisplayedModelController()
{
    ModelDisplayController* mdc = NULL;
    
    switch (this->selectedModelType) {
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE:
            mdc = this->selectedSurfaceModel;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = this->volumeModel;
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = this->wholeBrainModel;
            break;
    }
    
    return mdc;
}

/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is not a 
 *          surface.
 */   
ModelDisplayControllerSurface* 
BrowserTabContent::getDisplayedSurfaceModel()
{
    ModelDisplayControllerSurface* mdcs =
        dynamic_cast<ModelDisplayControllerSurface*>(this->getDisplayedModelController());
    return mdcs;
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
        dynamic_cast<ModelDisplayControllerVolume*>(this->getDisplayedModelController());
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
        dynamic_cast<ModelDisplayControllerWholeBrain*>(this->getDisplayedModelController());
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
 * Get the selected surface model.
 * 
 * @return  Pointer to selected surface model.
 */   
ModelDisplayControllerSurface* 
BrowserTabContent::getSelectedSurfaceModel()
{
    return this->selectedSurfaceModel;
}

/**
 * Set the selected surface model.
 * @param selectedSurfaceModel
 *    New selected surface model.
 */   
void 
BrowserTabContent::setSelectedSurfaceModel(ModelDisplayControllerSurface* selectedSurfaceModel)
{
    this->selectedSurfaceModel = selectedSurfaceModel;
    this->invalidateSurfaceColoring();
}

/**
 * Get the surface overlay assignments for this tab.
 * 
 * @return  Surface overlay assignments for this tab.
 */
SurfaceOverlaySet* 
BrowserTabContent::getSurfaceOverlaySet()
{
    return &this->surfaceOverlayAssignment;
}

/**
 * Get the tab number to which this tab is yoked.
 * 
 * @return  Tab number to which this tab is yoked.
 */
int32_t 
BrowserTabContent::getYokeToTabNumber() const
{
    return this->yokeToTabNumber;
}

/**
 * Set the tab number to which this tab is yoked.
 *
 * @param yokeToTabNumber
 *    Tab number to which this tab is yoked.
 */
void 
BrowserTabContent::setYokeToTabNumber(const int32_t yokeToTabNumber)
{
    this->yokeToTabNumber = yokeToTabNumber;
}

/**
 * Get the type of yoking.
 *
 * @return The type of yoking.
 */
YokingTypeEnum::Enum 
BrowserTabContent::getYokingType() const
{
    return this->yokingType;
}

/**
 * Set the type of yoking.
 *
 * @param yokingType
 *    New type of yoking.
 */
void 
BrowserTabContent::setYokingType(const YokingTypeEnum::Enum yokingType)
{
    this->yokingType = yokingType;
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
    const int32_t numModels = static_cast<int32_t>(modelDisplayControllers.size());
    
    this->allSurfaceModels.clear();
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    
    bool foundSelectedSurfaceModel = false;
    
    for (int i = 0; i < numModels; i++) {
        ModelDisplayController* mdc = modelDisplayControllers[i];
        
        ModelDisplayControllerSurface* mdcs = dynamic_cast<ModelDisplayControllerSurface*>(mdc);
        ModelDisplayControllerVolume* mdcv = dynamic_cast<ModelDisplayControllerVolume*>(mdc);
        ModelDisplayControllerWholeBrain* mdcwb = dynamic_cast<ModelDisplayControllerWholeBrain*>(mdc);
        
        if (mdcs != NULL) {
            this->allSurfaceModels.push_back(mdcs);
            if (mdcs == this->selectedSurfaceModel) {
                foundSelectedSurfaceModel = true;
            }
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
    
    if (foundSelectedSurfaceModel == false) {
        if (this->allSurfaceModels.empty() == false) {
            this->selectedSurfaceModel = this->allSurfaceModels[0];
        }
        else {
            this->selectedSurfaceModel = NULL;
        }
    }
    
    switch (this->selectedModelType) {
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE:
            if (this->selectedSurfaceModel == NULL) {
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
    }
    
    if (this->selectedModelType == ModelDisplayControllerTypeEnum::MODEL_TYPE_INVALID) {
        if (this->wholeBrainModel != NULL) {
            this->selectedModelType = ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN;
        }
        else if (this->selectedSurfaceModel != NULL) {
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
 * Invalidate surface coloring for this browser tab.
 */
void 
BrowserTabContent::invalidateSurfaceColoring()
{
    this->surfaceCerebellumLastColored = NULL;
    this->surfaceLeftLastColored = NULL;
    this->surfaceRightLastColored = NULL;
}

/**
 * Get the surface node coloring for a surface.
 * @param Surface for coloring.
 * @return Returns pointer to a float array with 
 * four elements per node containing the red, green,
 * blue, and alpha color components.
 */
const float* 
BrowserTabContent::getSurfaceColoring(const Surface* surface)
{
    CaretAssert(surface);
    
    Surface* lastSurface;
    std::vector<float>* nodeColoring = NULL;
    const StructureEnum::Enum structure = surface->getStructure();
    switch (structure) {
        case StructureEnum::CEREBELLUM:
            nodeColoring = &this->surfaceCerebellumColoringRGBA;
            lastSurface = this->surfaceCerebellumLastColored;
            break;
        case StructureEnum::CORTEX_LEFT:
            nodeColoring = &this->surfaceLeftColoringRGBA;
            lastSurface = this->surfaceLeftLastColored;
            break;
        case StructureEnum::CORTEX_RIGHT:
            nodeColoring = &this->surfaceRightColoringRGBA;
            lastSurface = this->surfaceRightLastColored;
            break;
        default:
            CaretLogSevere("Unreconized structure: " + StructureEnum::toGuiName(structure));
            return NULL;
            break;
    }
    
    /*
     * Add nodes to coloring, if necessary, but due not
     * shrink the size to avoid memory reallocation.
     */
    const int32_t numNodes = surface->getNumberOfNodes();
    const int32_t numberOfColorComponents = numNodes * 4;
    const int32_t coloringComponentCount = static_cast<int32_t>(nodeColoring->size());
    if (coloringComponentCount < numberOfColorComponents) {
        nodeColoring->resize(numberOfColorComponents);
    }    
    float* rgba = &(*nodeColoring)[0];
    
    /*
     * If surface for structure has not changed,
     * then the current coloring is valid.
     */
    if (surface == lastSurface) {
        return rgba;
    }
    
    /*
     * Color the surface nodes.
     */
    this->surfaceColoring->colorSurfaceNodes(surface,
                                             &surfaceOverlayAssignment, 
                                             rgba);
    
    /*
     * Save surface that was colored.
     */
    switch (structure) {
        case StructureEnum::CEREBELLUM:
            this->surfaceCerebellumLastColored = (Surface*)surface;
            break;
        case StructureEnum::CORTEX_LEFT:
            this->surfaceLeftLastColored = (Surface*)surface;
            break;
        case StructureEnum::CORTEX_RIGHT:
            this->surfaceRightLastColored = (Surface*)surface;
            break;
        default:
            CaretLogSevere("Unreconized structure: " + StructureEnum::toGuiName(structure));
            return NULL;
            break;
    }

    return rgba;
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
    if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_COLORING_INVALIDATE) {
        EventSurfaceColoringInvalidate* invalidateEvent =
        dynamic_cast<EventSurfaceColoringInvalidate*>(event);
        CaretAssert(invalidateEvent);
        
        invalidateEvent->setEventProcessed();
        
        this->invalidateSurfaceColoring();
    }    
}
