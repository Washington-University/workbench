
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

#define __OVERLAY_SET_DECLARE__
#include "OverlaySet.h"
#undef __OVERLAY_SET_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerVolume.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class OverlaySet
 * \brief Contains a set of overlay assignments
 *
 * The maximum number of overlays is fixed.  The number
 * of overlays presented to the user varies and is
 * controlled using the ToolBox in a Browser Window.
 * 
 * The primary overlay is always the overlay at index zero.
 * The underlay is the overlay at (numberOfDisplayedOverlays - 1).
 * When models are colored, the overlays are assigned 
 * starting with the underlay and concluding with the primary
 * overlay.
 */

/**
 * Constructor.
 */
OverlaySet::OverlaySet()
: CaretObject()
{
    this->numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
}

/**
 * Destructor.
 */
OverlaySet::~OverlaySet()
{
    
}

/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
Overlay* 
OverlaySet::getPrimaryOverlay()
{
    return &this->overlays[0];
}

/**
 * @return Returns the underlay which is the lowest
 * displayed overlay.
 */
Overlay* 
OverlaySet::getUnderlay()
{
    return &this->overlays[this->getNumberOfDisplayedOverlays() - 1];
}

/*
 * Get the bottom-most overlay that is a volume file for the given
 * browser tab.
 * @param browserTabContent
 *    Content of browser tab.
 * @return Returns the bottom-most overlay that is set a a volume file.
 * Will return NULL if no, enabled overlays are set to a volume file.
 */
VolumeFile* 
OverlaySet::getUnderlayVolume(BrowserTabContent* browserTabContent)
{
    CaretAssert(browserTabContent);
    
    VolumeFile* vf = NULL;
    
    for (int32_t i = (this->getNumberOfDisplayedOverlays() - 1); i >= 0; i--) {
        if (this->overlays[i].isEnabled()) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            this->overlays[i].getSelectionData(browserTabContent,
                                               mapFile,
                                               mapIndex);
            
            if (mapFile != NULL) {
                vf = dynamic_cast<VolumeFile*>(mapFile);
                if (vf != NULL) {
                    break;
                }
            }
        }
    }
    return vf;
}

/**
 * If NO overlay (any overlay) is set to a volume, set the underlay to the first
 * volume that it finds.
 * @param browserTabContent
 *    Content of browser tab.
 * @return Returns the volume file that was selected or NULL if no
 *    volume file was found.
 */
VolumeFile* 
OverlaySet::setUnderlayToVolume(BrowserTabContent* browserTabContent)
{
    VolumeFile * vf = this->getUnderlayVolume(browserTabContent);
    
    if (vf == NULL) {
        const int32_t overlayIndex = this->getNumberOfDisplayedOverlays() - 1;
        if (overlayIndex >= 0) {
            std::vector<CaretMappableDataFile*> mapFiles;
            CaretMappableDataFile* mapFile;
            AString mapName;
            int32_t mapIndex;
            this->overlays[overlayIndex].getSelectionData(browserTabContent, 
                                                          mapFiles, 
                                                          mapFile, 
                                                          mapName, 
                                                          mapIndex);
            
            const int32_t numMapFiles = static_cast<int32_t>(mapFiles.size());
            for (int32_t i = 0; i < numMapFiles; i++) {
                vf = dynamic_cast<VolumeFile*>(mapFiles[i]);
                if (vf != NULL) {
                    this->overlays[overlayIndex].setSelectionData(vf, 0);
                    break;
                }
            }
        }
    }
    
    return vf;
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
const Overlay* 
OverlaySet::getOverlay(const int32_t overlayNumber) const
{
    CaretAssertArrayIndex(this->overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, 
                          overlayNumber);
    return &this->overlays[overlayNumber];    
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
Overlay* 
OverlaySet::getOverlay(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(this->overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, 
                          overlayNumber);
    return &this->overlays[overlayNumber];    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OverlaySet::toString() const
{
    return "OverlaySet";
}

/**
 * Add a displayed overlay.  If the maximum
 * number of surface overlays is reached,
 * this method has no effect.
 */
void 
OverlaySet::addDisplayedOverlay()
{
    this->numberOfDisplayedOverlays++;
    if (this->numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        this->numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
}

/**
 * @return Returns the number of displayed overlays. 
 */
int32_t 
OverlaySet::getNumberOfDisplayedOverlays() const
{
    return this->numberOfDisplayedOverlays;
}

/**
 * Remove a displayed overlay.  This method will have
 * no effect if the minimum number of overlays are
 * displayed
 *
 * @param overlayIndex 
 *    Index of overlay for removal from display.
 */
void 
OverlaySet::removeDisplayedOverlay(const int32_t overlayIndex)
{
    if (this->numberOfDisplayedOverlays > BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        this->numberOfDisplayedOverlays--;
        for (int32_t i = overlayIndex; i < this->numberOfDisplayedOverlays; i++) {
            this->overlays[i].copyData(&this->overlays[i+1]);
        }
    }
}

/**
 * Move the overlay at the given index up one level
 * (swap it with overlayIndex - 1).  This method will
 * have no effect if the overlay is the top-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved up.
 */
void 
OverlaySet::moveDisplayedOverlayUp(const int32_t overlayIndex)
{
    if (overlayIndex > 0) {
        this->overlays[overlayIndex].swapData(&this->overlays[overlayIndex - 1]);
    }
}

/**
 * Move the overlay at the given index down one level
 * (swap it with overlayIndex + 1).  This method will
 * have no effect if the overlay is the bottom-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved down.
 */
void 
OverlaySet::moveDisplayedOverlayDown(const int32_t overlayIndex)
{
    const int32_t nextOverlayIndex = overlayIndex + 1;
    if (nextOverlayIndex < this->numberOfDisplayedOverlays) {
        this->overlays[overlayIndex].swapData(&this->overlays[nextOverlayIndex]);
    }
}

/**
 * Initialize the overlays for the model display controller.
 * @param mdc
 *    Model Display Controller.
 */
void 
OverlaySet::initializeOverlays(ModelDisplayController* mdc)
{
    if (mdc == NULL) {
        return;
    }
    
    ModelDisplayControllerSurface* mdcs = dynamic_cast<ModelDisplayControllerSurface*>(mdc);
    ModelDisplayControllerVolume* mdcv = dynamic_cast<ModelDisplayControllerVolume*>(mdc);
    ModelDisplayControllerWholeBrain* mdcwb = dynamic_cast<ModelDisplayControllerWholeBrain*>(mdc);
    
    const int32_t numOverlays = this->getNumberOfDisplayedOverlays();
    if (mdcs != NULL) {
        Surface* surface = mdcs->getSurface();
        BrainStructure* brainStructure = surface->getBrainStructure();
        
        int32_t iOver = 0;
        if (brainStructure->getNumberOfLabelFiles() > 0) {
            
        }
        if (iOver >= numOverlays) {
            return;
        }
        
        int32_t numMetricFiles = brainStructure->getNumberOfMetricFiles();
        for (int32_t i = 0; i < numMetricFiles; i++) {
            if (iOver >= numOverlays) {
                return;
            }
        }
    }
    else if (mdcv != NULL) {
        
    }
    else if (mdcwb != NULL) {
        
    }
    else {
        CaretAssert(0);
    }
}



