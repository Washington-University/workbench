
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

#include <algorithm>

#define __OVERLAY_DECLARE__
#include "Overlay.h"
#undef __OVERLAY_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerVolume.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "RgbaFile.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


/**
 * \class Overlay
 * \brief  Base class for overlays.
 */

/**
 * Constructor.
 */
Overlay::Overlay()
: CaretObject()
{
    this->opacity = 1.0;
    
    this->name = "Overlay ";
    this->enabled = true;
    this->paletteDisplayedFlag = false;
}

/**
 * Destructor.
 */
Overlay::~Overlay()
{
    
}

/**
 * Set the number of this overlay.
 * 
 * @param overlayIndex
 *    Index for this overlay.
 */
void 
Overlay::setOverlayNumber(const int32_t overlayIndex)
{    
    this->name = "Overlay " + AString::number(overlayIndex + 1);
}

/**
 * Get the opacity.
 * 
 * @return  The opacity.
 */
float 
Overlay::getOpacity() const
{
    return this->opacity;
}

/**
 * Set the opacity.
 *
 * @param opacity
 *    New value for opacity.
 */
void 
Overlay::setOpacity(const float opacity)
{
    this->opacity = opacity;
}

AString 
Overlay::getName() const
{
    return this->name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Overlay::toString() const
{
    return "Overlay";
}

/**
 * @return Enabled status for this surface overlay.
 */
bool 
Overlay::isEnabled() const
{
    return this->enabled;
}

/**
 * Set the enabled status for this surface overlay.
 * @param enabled
 *    New status.
 */
void 
Overlay::setEnabled(const bool enabled)
{
    this->enabled = enabled;
}

/**
 * Copy the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
Overlay::copyData(const Overlay* overlay)
{
    CaretAssert(overlay);
    
    this->opacity = overlay->opacity;
    this->enabled = overlay->enabled;
    
    this->mapFiles = overlay->mapFiles;
    this->selectedMapFile = overlay->selectedMapFile;
    this->selectedMapName = overlay->selectedMapName;
}

/**
 * Swap the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
Overlay::swapData(Overlay* overlay)
{
    Overlay swapOverlay;
    swapOverlay.copyData(overlay);
    
    overlay->copyData(this);
    this->copyData(&swapOverlay);
}

/**
 * Get the current selection.  If the current selection is
 * invalid, new map data will be selected.
 *
 * @param browserTabContent
 *    Tab in which this overlay is applied.
 * @param mapDataFileTypeOut
 *    Type of map file out.
 * @param selectedMapNameOut
 *    Name of map that is selected.
 */
void 
Overlay::getSelectionData(BrowserTabContent* browserTabContent,
                          DataFileTypeEnum::Enum& mapDataFileTypeOut,
                          AString& selectedMapNameOut)
{
    std::vector<CaretMappableDataFile*> allFiles;
    CaretMappableDataFile* selectedFile;
    int32_t selectedIndex;
    this->getSelectionData(browserTabContent,
                           allFiles,
                           selectedFile,
                           selectedMapNameOut,
                           selectedIndex);
    
    mapDataFileTypeOut = DataFileTypeEnum::UNKNOWN;
    if (selectedFile != NULL) {
        mapDataFileTypeOut = selectedFile->getDataFileType();
    }
    else {
        selectedMapNameOut = "";
    }    
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param browserTabContent
 *    Tab in which this overlay is applied.
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void 
Overlay::getSelectionData(BrowserTabContent* browserTabContent,
                          CaretMappableDataFile* &selectedMapFileOut,
                          int32_t& selectedMapIndexOut)
{
    std::vector<CaretMappableDataFile*> mapFiles;
    AString mapName;
    
    this->getSelectionData(browserTabContent, 
                           mapFiles, 
                           selectedMapFileOut, 
                           mapName, 
                           selectedMapIndexOut);
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param browserTabContent
 *    Tab in which this overlay is applied.
 * @param mapFilesOut
 *    Contains all map files that can be selected.
 * @param selectedMapFileOut
 *    The selected map file.  May be NULL.
 * @param selectedMapNameOut
 *    Name of selected map.
 * @param selectedMapIndexOut
 *    Index of selected map in the selected file.
 */
void 
Overlay::getSelectionData(BrowserTabContent* browserTabContent,
                          std::vector<CaretMappableDataFile*>& mapFilesOut,
                          CaretMappableDataFile* &selectedMapFileOut,
                          AString& selectedMapNameOut,
                          int32_t& selectedMapIndexOut)
{
    mapFilesOut.clear();
    selectedMapFileOut = NULL;
    selectedMapNameOut = "";
    selectedMapIndexOut = -1;
    
    /**
     * Get the data files.
     */
    std::vector<CaretMappableDataFile*> allDataFiles;
    EventCaretMappableDataFilesGet eventGetMapDataFiles;
    EventManager::get()->sendEvent(eventGetMapDataFiles.getPointer());
    eventGetMapDataFiles.getAllFiles(allDataFiles);
    
    bool showSurfaceMapFiles = false;
    bool showVolumeMapFiles  = false;

    /*
     * If a surface is displayed, restrict selections to files that
     * match the structure of the displayed surface.
     */
    StructureEnum::Enum selectedSurfaceStructure = StructureEnum::ALL;
    ModelDisplayControllerSurface* surfaceController = browserTabContent->getDisplayedSurfaceModel();
    if (surfaceController != NULL) {
        selectedSurfaceStructure = surfaceController->getSurface()->getStructure();
        showSurfaceMapFiles = true;
    }
    
    /*
     * If a volume is selected, restrict selections to volume files.
     */
    ModelDisplayControllerVolume* volumeController = browserTabContent->getDisplayedVolumeModel();
    if (volumeController != NULL) {
        showVolumeMapFiles = true;
    }
    
    /*
     * If whole brain is selected, show surface and volume files.
     */
    ModelDisplayControllerWholeBrain* wholeBrainController = browserTabContent->getDisplayedWholeBrainModel();
    if (wholeBrainController != NULL) {
        showSurfaceMapFiles = true;
        showVolumeMapFiles = true;
    }
    
    /*
     * Use only those data files that meet criteria.
     */
    for (std::vector<CaretMappableDataFile*>::iterator iter = allDataFiles.begin();
         iter != allDataFiles.end();
         iter++) {
        CaretMappableDataFile* mapFile = *iter;
        bool useIt = false;
        bool mappable = false;
        
        if (mapFile->isSurfaceMappable()) {
            mappable = true;
            if (showSurfaceMapFiles) {
                if (selectedSurfaceStructure == StructureEnum::ALL) {
                    useIt = true;
                }
                else if (mapFile->getStructure() == StructureEnum::ALL) {
                    useIt = true;
                }
                else if (selectedSurfaceStructure == mapFile->getStructure()) {
                    useIt = true;
                }
            }
        }
        if (mapFile->isVolumeMappable()) {
            mappable = true;
            if (showVolumeMapFiles) {
                useIt = true;
            }
        }
        
        if (mappable == false) {
            CaretAssertMessage(0, "Map file is neither surface nor volume mappable: " + mapFile->getFileName());
        }
        
        if (useIt) {
            mapFilesOut.push_back(mapFile);
        }
    }
    
    /*
     * Does selected data file still no longer exist?
     */
    if (std::find(mapFilesOut.begin(), 
                  mapFilesOut.end(),
                  this->selectedMapFile) == mapFilesOut.end()) {
        this->selectedMapFile = NULL;
    }
    
    /*
     * If selected data file is valid, see if selected
     * map is still valid.  If not, use first map.
     */
    if (this->selectedMapFile != NULL) {
        const int32_t mapIndex = this->selectedMapFile->getMapIndexFromName(this->selectedMapName);
        if (mapIndex < 0) {
            this->selectedMapName = this->selectedMapFile->getMapName(0);
        }
    }
    else {
        /*
         * Look for a file that contains the selected map name.
         */
        if (this->selectedMapName.isEmpty() == false) {
            for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
                 iter != mapFilesOut.end();
                 iter++) {
                CaretMappableDataFile* mapTypeFile = *iter;
                const int32_t mapIndex = mapTypeFile->getMapIndexFromName(this->selectedMapName);
                if (mapIndex >= 0) {
                    this->selectedMapFile = mapTypeFile;
                    break;
                }
            }
        }
        
        /*
         * Use first map in first file that has one or more maps.
         */
        if (this->selectedMapFile == NULL) {
            if (mapFilesOut.empty() == false) {
                for (std::vector<CaretMappableDataFile*>::iterator iter = mapFilesOut.begin();
                     iter != mapFilesOut.end();
                     iter++) {
                    CaretMappableDataFile* mapTypeFile = *iter;
                    if (mapTypeFile->getNumberOfMaps() > 0) {
                        this->selectedMapFile = mapTypeFile;
                        this->selectedMapName = mapTypeFile->getMapName(0);
                    }
                }
            }
        }
    }
    
    selectedMapFileOut = this->selectedMapFile;
    if (selectedMapFileOut != NULL) {
        selectedMapNameOut = this->selectedMapName;
        selectedMapIndexOut = this->selectedMapFile->getMapIndexFromName(selectedMapNameOut);
    }
}

/**
 * Set the selected map file and map.
 * @param selectedMapFile 
 *    File that is selected.
 * @param selectedMapName
 *    Map name that is selected.
 */
void 
Overlay::setSelectionData(CaretMappableDataFile* selectedMapFile,
                          const int32_t selectedMapIndex)
{
    this->selectedMapFile = selectedMapFile;
    this->selectedMapName = selectedMapFile->getMapName(selectedMapIndex);    
}

/**
 * @return Is display of palette in graphics window enabled?
 */
bool 
Overlay::isPaletteDisplayEnabled() const
{
    return this->paletteDisplayedFlag;
}

/**
 * Set display of palette in graphics window.
 * @param enabled
 *   New status for palette display in graphics window.
 */
void 
Overlay::setPaletteDisplayEnabled(const bool enabled)
{
    this->paletteDisplayedFlag = enabled;
}



