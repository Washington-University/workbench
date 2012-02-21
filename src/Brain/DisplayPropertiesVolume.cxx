
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
#include "CaretAssert.h"
#include "SurfaceSelection.h"
#include "VolumeSurfaceOutlineSelection.h"
#include "BrainStructure.h"

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
        VolumeSurfaceOutlineSelection* vsos = new VolumeSurfaceOutlineSelection();
        bool validColor = false;
        CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(colorCounter,
                                                                     &validColor);
        if (validColor == false) {
            colorCounter = 0;
            color = CaretColorEnum::fromIntegerCode(colorCounter,
                                                    &validColor);
        }
        vsos->setColor(color);
        this->volumeSurfaceOutlineSelections.push_back(vsos);
        colorCounter++;
    }
}

/**
 * Destructor.
 */
DisplayPropertiesVolume::~DisplayPropertiesVolume()
{
    for (int32_t i = 0; i < MAXIMUM_NUMBER_OF_SURFACE_OUTLINES; i++) {
        delete this->volumeSurfaceOutlineSelections[i];
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
VolumeSurfaceOutlineSelection* 
DisplayPropertiesVolume::getSurfaceOutlineSelection(const int32_t indx)
{
    CaretAssertVectorIndex(this->volumeSurfaceOutlineSelections, indx);
    return this->volumeSurfaceOutlineSelections[indx];
}

/**
 * Get the volume surface outline at the given index.
 * @param indx
 *   Index of volume surface outline.
 * @return
 *   Volume surface outline.
 */
const VolumeSurfaceOutlineSelection* 
DisplayPropertiesVolume::getSurfaceOutlineSelection(const int32_t indx) const
{
    CaretAssertVectorIndex(this->volumeSurfaceOutlineSelections, indx);
    return this->volumeSurfaceOutlineSelections[indx];
}

/**
 * Set the default selected surfaces after a spec file is loaded.
 */ 
void 
DisplayPropertiesVolume::selectSurfacesAfterSpecFileLoaded()
{
    const int32_t numBrainStructures = this->getBrain()->getNumberOfBrainStructures();
    
    const int32_t maxIndex = std::min(numBrainStructures,
                                      MAXIMUM_NUMBER_OF_SURFACE_OUTLINES);
    for (int32_t i = 0; i < maxIndex; i++) {
        BrainStructure* brainStructure = this->getBrain()->getBrainStructure(i);
        Surface* surface = brainStructure->getVolumeInteractionSurface();
        if (surface != NULL) {
            this->volumeSurfaceOutlineSelections[i]->getSurfaceSelection()->setSurface(surface);
        }
    }
}
