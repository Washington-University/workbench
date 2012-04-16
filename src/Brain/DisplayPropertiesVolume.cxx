
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

#define __DISPLAY_PROPERTIES_VOLUME_DECLARE__
#include "DisplayPropertiesVolume.h"
#undef __DISPLAY_PROPERTIES_VOLUME_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "ModelSurface.h"
#include "StructureEnum.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceTypeEnum.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"

using namespace caret;
    
/**
 * \class DisplayPropertiesVolume 
 * \brief Display properties for volume slices.
 *
 * Display properties for volume slices.
 */


/**
 * Constructor.
 */
DisplayPropertiesVolume::DisplayPropertiesVolume(Brain* brain)
: DisplayProperties(brain)
{
    int32_t colorCounter = 0;
    for (int32_t i = 0; i < MAXIMUM_NUMBER_OF_SURFACE_OUTLINES; i++) {
        VolumeSurfaceOutlineModel* vsos = new VolumeSurfaceOutlineModel();
        bool validColor = false;
        CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(colorCounter,
                                                                     &validColor);
        if (validColor == false) {
            colorCounter = 0;
            color = CaretColorEnum::fromIntegerCode(colorCounter,
                                                    &validColor);
        }
        vsos->getColorOrTabModel()->setColor(color);
        this->volumeSurfaceOutlineModels.push_back(vsos);
        colorCounter++;
    }
}

/**
 * Destructor.
 */
DisplayPropertiesVolume::~DisplayPropertiesVolume()
{
    for (int32_t i = 0; i < MAXIMUM_NUMBER_OF_SURFACE_OUTLINES; i++) {
        delete this->volumeSurfaceOutlineModels[i];
    }
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesVolume::reset()
{
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesVolume::update()
{
    
}

/**
 * Get the volume surface outline at the given index.
 * @param indx
 *   Index of volume surface outline.
 * @return
 *   Volume surface outline.
 */
VolumeSurfaceOutlineModel* 
DisplayPropertiesVolume::getSurfaceOutlineSelection(const int32_t indx)
{
    CaretAssertVectorIndex(this->volumeSurfaceOutlineModels, indx);
    return this->volumeSurfaceOutlineModels[indx];
}

/**
 * Get the volume surface outline at the given index.
 * @param indx
 *   Index of volume surface outline.
 * @return
 *   Volume surface outline.
 */
const VolumeSurfaceOutlineModel* 
DisplayPropertiesVolume::getSurfaceOutlineSelection(const int32_t indx) const
{
    CaretAssertVectorIndex(this->volumeSurfaceOutlineModels, indx);
    return this->volumeSurfaceOutlineModels[indx];
}

/**
 * Set the default selected surfaces after a spec file is loaded.
 * @searchForTabs
 *   If true, examine the loaded tabs to find left and right surfaces.
 */ 
void 
DisplayPropertiesVolume::selectSurfacesAfterSpecFileLoaded(const bool searchForTabs)
{
    EventBrowserTabGetAll getAllTabs;
    EventManager::get()->sendEvent(getAllTabs.getPointer());
    
    /*
     * Find tabs with left/right
     */
    int32_t leftTabIndex  = -1;
    int32_t rightTabIndex = -1;
    const int numTabs = getAllTabs.getNumberOfBrowserTabs();
    if (searchForTabs) {
        for (int32_t i = 0; i < numTabs; i++) {
            BrowserTabContent* tabContent = getAllTabs.getBrowserTab(i);
            ModelSurface* surfaceModel = tabContent->getDisplayedSurfaceModel();
            if (surfaceModel != NULL) {
                const StructureEnum::Enum structure = surfaceModel->getSurface()->getStructure();
                switch (structure) {
                    case StructureEnum::CORTEX_LEFT:
                        leftTabIndex = tabContent->getTabNumber();
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        rightTabIndex = tabContent->getTabNumber();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else {
        if (numTabs >= 1) {
            leftTabIndex = 0;
        }
        if (numTabs >= 2) {
            rightTabIndex = 1;
        }
    }
    
    Surface* leftMidThickSurface = NULL;
    Surface* leftWhiteSurface    = NULL;
    Surface* leftPialSurface     = NULL;
    BrainStructure* leftBrainStructure = this->getBrain()->getBrainStructure(StructureEnum::CORTEX_LEFT, 
                                                                             false);
    if (leftBrainStructure != NULL) {
        leftMidThickSurface = leftBrainStructure->getSurfaceContainingTextInName("midthick");
        if (leftMidThickSurface == NULL) {
            leftMidThickSurface = leftBrainStructure->getVolumeInteractionSurface();
        }

        leftWhiteSurface    = leftBrainStructure->getSurfaceContainingTextInName("white");
        leftPialSurface     = leftBrainStructure->getSurfaceContainingTextInName("pial");
    }
    
    Surface* rightMidThickSurface = NULL;
    Surface* rightWhiteSurface    = NULL;
    Surface* rightPialSurface     = NULL;
    BrainStructure* rightBrainStructure = this->getBrain()->getBrainStructure(StructureEnum::CORTEX_RIGHT, 
                                                                             false);
    if (rightBrainStructure != NULL) {
        rightMidThickSurface = rightBrainStructure->getSurfaceContainingTextInName("midthick");
        if (rightMidThickSurface == NULL) {
            rightMidThickSurface = rightBrainStructure->getVolumeInteractionSurface();
        }
        
        rightWhiteSurface    = rightBrainStructure->getSurfaceContainingTextInName("white");
        rightPialSurface     = rightBrainStructure->getSurfaceContainingTextInName("pial");
    }
    
    
    for (int32_t i = 0; i < MAXIMUM_NUMBER_OF_SURFACE_OUTLINES; i++) {
        this->volumeSurfaceOutlineModels[i]->getColorOrTabModel()->setColor(CaretColorEnum::BLACK);
        this->volumeSurfaceOutlineModels[i]->setThickness(1);
    }

    int nextOutlineIndex = 0;
    
    this->addSurfaceOutline(leftMidThickSurface, 
                            5.0, 
                            leftTabIndex, 
                            CaretColorEnum::BLACK, 
                            nextOutlineIndex);
    this->addSurfaceOutline(rightMidThickSurface, 
                            5.0, 
                            rightTabIndex, 
                            CaretColorEnum::BLACK, 
                            nextOutlineIndex);
    
    this->addSurfaceOutline(leftWhiteSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::LIME, 
                            nextOutlineIndex);
    this->addSurfaceOutline(rightWhiteSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::LIME, 
                            nextOutlineIndex);
    
    this->addSurfaceOutline(leftPialSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::BLUE, 
                            nextOutlineIndex);
    this->addSurfaceOutline(rightPialSurface, 
                            0.5, 
                            -1, 
                            CaretColorEnum::BLUE, 
                            nextOutlineIndex);
}

/**
 * Add a surface outline at the given outlineIndex.  The
 * outlineIndex is incremented.
 *
 * @param surface
 *    Surface that is added.  If NULL, no action is taken.
 * @param thickness
 *    Thickness for surface outline.
 * @param browserTabIndex
 *    If greater than or equal to zero, the color source
 *    is set to this tab index.
 * @param color
 *    If browserTabIndex is less than zero, the color source
 *    is set to this color.
 * @param outlineIndex
 *    If an outline was added, it is placed at this value
 *    and it is incremented.  If this index is greater 
 *    than or equal to the number of available surface 
 *    outlines, no action is taken.
 */
void 
DisplayPropertiesVolume::addSurfaceOutline(Surface* surface,
                                           const float thickness,
                                           const int32_t browserTabIndex,
                                           const CaretColorEnum::Enum color,
                                           int32_t& outlineIndex)
{
    if (surface != NULL) {
        if (surface->getSurfaceType() == SurfaceTypeEnum::ANATOMICAL) {
            if (outlineIndex < MAXIMUM_NUMBER_OF_SURFACE_OUTLINES) {
                VolumeSurfaceOutlineModel* vsos = this->volumeSurfaceOutlineModels[outlineIndex];
                vsos->getSurfaceSelectionModel()->setSurface(surface);
                vsos->setThickness(thickness);
                if (browserTabIndex >= 0) {
                    vsos->getColorOrTabModel()->setBrowserTabIndex(browserTabIndex);
                }
                else {
                    vsos->getColorOrTabModel()->setColor(color);
                }
                
                outlineIndex++;
            }
        }
    }
}
