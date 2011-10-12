
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

#define __SURFACE_OVERLAY_DECLARE__
#include "SurfaceOverlay.h"
#undef __SURFACE_OVERLAY_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventNodeDataFilesGet.h"
#include "EventManager.h"
#include "GiftiTypeFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerSurfaceSelector.h"
#include "RgbaFile.h"
#include "Surface.h"

using namespace caret;


/**
 * Constructor.
 */
SurfaceOverlay::SurfaceOverlay()
: Overlay()
{
    this->selectedDataFile = NULL;
    this->selectedColumnName = "";
}

/**
 * Destructor.
 */
SurfaceOverlay::~SurfaceOverlay()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceOverlay::toString() const
{
    return "SurfaceOverlay";
}

/**
 * @param browserTabContent
 *    Tab in which this overlay is applied.
 * Return the selection information.
 * @param selectedDataTypeOut
 *    Type of data that is selected.
 * @param selectedColumnNameOut
 *    Name of column that is selected.
 */
void 
SurfaceOverlay::getSelectionData(BrowserTabContent* browserTabContent,
                                 SurfaceOverlayDataTypeEnum::Enum& selectedDataTypeOut,
                                 AString& selectedColumnNameOut)
{
    std::vector<GiftiTypeFile*> dataFiles;
    GiftiTypeFile* selectedFile;
    int32_t selectedColumnIndex;
    this->getSelectionData(browserTabContent,
                           dataFiles,
                           selectedFile,
                           selectedColumnNameOut,
                           selectedColumnIndex);
    
    selectedDataTypeOut = SurfaceOverlayDataTypeEnum::NONE;
    
    if (this->selectedDataFile != NULL) {
        if (dynamic_cast<LabelFile*>(this->selectedDataFile) != NULL) {
            selectedDataTypeOut = SurfaceOverlayDataTypeEnum::LABEL;
        }
        else if (dynamic_cast<MetricFile*>(this->selectedDataFile) != NULL) {
            selectedDataTypeOut = SurfaceOverlayDataTypeEnum::METRIC;
        }
        else if (dynamic_cast<RgbaFile*>(this->selectedDataFile) != NULL) {
            selectedDataTypeOut = SurfaceOverlayDataTypeEnum::RGBA;
        }
        else {
            CaretAssertMessage(0, "File type is not supported for surface overlay: " + this->selectedDataFile->toString());
        }
    }
    else {
        selectedColumnNameOut = "";
    }
}

/**
 * Return the selection information.  This method is typically
 * called to update the user-interface.
 *
 * @param browserTabContent
 *    Tab in which this overlay is applied.
 * @param dataFilesOut
 *    Contains all data files that can be selected.
 * @param selectedFileOut
 *    The selected file.  May be NULL.
 * @param selectedColumnNameOut
 *    Name of selected column.
 * @param selectedColumnIndexOut
 *    Index of selected column.
 */
void 
SurfaceOverlay::getSelectionData(BrowserTabContent* browserTabContent,
                                 std::vector<GiftiTypeFile*>& dataFilesOut,
                                 GiftiTypeFile* &selectedFileOut,
                                 AString& selectedColumnNameOut,
                                 int32_t& selectedColumnIndexOut)
{
    dataFilesOut.clear();
    selectedFileOut = NULL;
    selectedColumnNameOut = "";
    selectedColumnIndexOut = -1;
    
    /**
     * Get the data files.
     */
    std::vector<GiftiTypeFile*> allDataFiles;
    EventNodeDataFilesGet eventGetDataFiles;
    EventManager::get()->sendEvent(eventGetDataFiles.getPointer());
    eventGetDataFiles.getAllFiles(allDataFiles);

    /*
     * Get the structure for the browser content
     */
    ModelDisplayControllerSurfaceSelector* surfaceSelector = browserTabContent->getSurfaceModelSelector();
    const StructureEnum::Enum selectedStructure = surfaceSelector->getSelectedStructure();
    bool useAllFlag = (selectedStructure == StructureEnum::ALL);
    
    /*
     * Use only those data files that match the selected structure.
     */
    for (std::vector<GiftiTypeFile*>::iterator iter = allDataFiles.begin();
         iter != allDataFiles.end();
         iter++) {
        GiftiTypeFile* giftiFile = *iter;
        bool useIt = false;
        if (useAllFlag) {
            useIt = true;
        }
        else if (selectedStructure == giftiFile->getStructure()) {
            useIt = true;
        }
        if (useIt) {
            dataFilesOut.push_back(giftiFile);
        }
    }
    
    /*
     * Does selected data file still no longer exist?
     */
    if (std::find(dataFilesOut.begin(), 
                  dataFilesOut.end(),
                  this->selectedDataFile) == dataFilesOut.end()) {
        this->selectedDataFile = NULL;
    }
    
    /*
     * If selected data file is valid, see if selected
     * column is still valid.  If not, use first column.
     */
    if (this->selectedDataFile != NULL) {
        const int32_t columnIndex = this->selectedDataFile->getColumnIndexFromColumnName(this->selectedColumnName);
        if (columnIndex < 0) {
            this->selectedColumnName = this->selectedDataFile->getColumnName(0);
        }
    }
    else {
        /*
         * Look for a file that contains the selected column name.
         */
        if (this->selectedColumnName.isEmpty() == false) {
            for (std::vector<GiftiTypeFile*>::iterator iter = dataFilesOut.begin();
                 iter != dataFilesOut.end();
                 iter++) {
                GiftiTypeFile* giftiTypeFile = *iter;
                const int32_t column = giftiTypeFile->getColumnIndexFromColumnName(this->selectedColumnName);
                if (column >= 0) {
                    this->selectedDataFile = giftiTypeFile;
                    break;
                }
            }
        }
        
        /*
         * Use first column in first file.
         */
        if (this->selectedDataFile == NULL) {
            if (dataFilesOut.empty() == false) {
                this->selectedDataFile = dataFilesOut[0];
                this->selectedColumnName = dataFilesOut[0]->getColumnName(0);
            }
        }
    }
    
    selectedFileOut = this->selectedDataFile;
    if (selectedFileOut != NULL) {
        selectedColumnNameOut = this->selectedColumnName;
        selectedColumnIndexOut = this->selectedDataFile->getColumnIndexFromColumnName(selectedColumnNameOut);
    }
}

/**
 * Set the selected data file and column.
 * @param selectedDataFile 
 *    File that is selected.
 * @param selectedColumnName
 *    Column name that is selected.
 */
void 
SurfaceOverlay::setSelectionData(GiftiTypeFile* selectedDataFile,
                                 const int32_t selectedColumnIndex)
{
    this->selectedDataFile = selectedDataFile;
    this->selectedColumnName = selectedDataFile->getColumnName(selectedColumnIndex);
}

/**
 * Copy the overlay's data to "this" overlay.
 * Will need to cast the input overlay.
 * 
 * @param overlay
 *    Overlay that is copied.
 */
void 
SurfaceOverlay::copyOverlayData(const Overlay* overlay)
{
    const SurfaceOverlay* so = dynamic_cast<const SurfaceOverlay*>(overlay);
    CaretAssert(so);
    if (so == NULL) {
        CaretLogSevere("Overlay parameter is NOT a SurfaceOverlay!!!");
    }
    
    this->dataFiles = so->dataFiles;
    this->selectedDataFile = so->selectedDataFile;
    this->selectedColumnName = so->selectedColumnName;
}
