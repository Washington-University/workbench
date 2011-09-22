
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

#define __SURFACE_NODE_COLORING_DECLARE__
#include "SurfaceNodeColoring.h"
#undef __SURFACE_NODE_COLORING_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "EventBrainStructureGet.h"
#include "EventManager.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "RgbaFile.h"
#include "Surface.h"
#include "SurfaceOverlay.h"
#include "SurfaceOverlaySet.h"

using namespace caret;


/**
 * Constructor.
 */
SurfaceNodeColoring::SurfaceNodeColoring()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
SurfaceNodeColoring::~SurfaceNodeColoring()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceNodeColoring::toString() const
{
    return "SurfaceNodeColoring";
}

/**
 * Assign color components to surface nodes. 
 *
 * @param surface
 *    Surface that has its nodes colored.
 * @param surfaceOverlaySet
 *    Surface overlay assignments for surface.
 * @param rgbaNodeColors
 *    RGBA color components that are set by this method.
 */
void 
SurfaceNodeColoring::colorSurfaceNodes(const Surface* surface,
                                       SurfaceOverlaySet* surfaceOverlaySet,
                                       float* rgbaNodeColors)
{
    const int32_t numNodes = surface->getNumberOfNodes();
    const int32_t numberOfDisplayedOverlays = surfaceOverlaySet->getNumberOfDisplayedOverlays();
    
    /*
     * Default color.
     */
    for (int32_t i = 0; i < numNodes; i++) {
        const int32_t i4 = i * 4;
        rgbaNodeColors[i4] = 0.70;
        rgbaNodeColors[i4+1] = 0.70;
        rgbaNodeColors[i4+2] = 0.70;
        rgbaNodeColors[i4+3] = 1.0;
    }
    
    
    EventBrainStructureGet brainStructureEvent(surface->getBrainStructureIdentifier());
    EventManager::get()->sendEvent(brainStructureEvent.getPointer());
    BrainStructure* brainStructure = brainStructureEvent.getBrainStructure();
    CaretAssert(brainStructure);
    
    bool firstOverlayFlag = true;
    float* overlayRGBV = new float[numNodes * 4];
    for (int32_t iOver = (numberOfDisplayedOverlays - 1); iOver >= 0; iOver--) {
        SurfaceOverlay* overlay = surfaceOverlaySet->getOverlay(iOver);
        if (overlay->isEnabled()) {
            SurfaceOverlayDataTypeEnum::Enum overlayType;
            AString selectedColumnName;
            overlay->getSelectionData(overlayType,
                                      selectedColumnName);
            
            bool isColoringValid = false;
            switch (overlayType) {
                case SurfaceOverlayDataTypeEnum::NONE:
                    break;
                case SurfaceOverlayDataTypeEnum::CONNECTIVITY:
                    break;
                case SurfaceOverlayDataTypeEnum::LABEL:
                    isColoringValid = this->assignLabelColoring(brainStructure, selectedColumnName, numNodes, overlayRGBV);
                    break;
                case SurfaceOverlayDataTypeEnum::METRIC:
                    isColoringValid = this->assignMetricColoring(brainStructure, selectedColumnName, numNodes, overlayRGBV);
                    break;
                case SurfaceOverlayDataTypeEnum::RGBA:
                    isColoringValid = this->assignRgbaColoring(brainStructure, selectedColumnName, numNodes, overlayRGBV);
                    break;
            }
            
            if (isColoringValid) {
                const float opacity = overlay->getOpacity();
                const float oneMinusOpacity = 1.0 - opacity;
                
                for (int32_t i = 0; i < numNodes; i++) {
                    const int32_t i4 = i * 4;
                    const float valid = overlayRGBV[i4 + 3];
                    if (valid > 0.0 ) {
                        if ((opacity < 1.0) && (firstOverlayFlag == false)) {
                            
                            rgbaNodeColors[i4]   = (overlayRGBV[i4]   * opacity) 
                            + (rgbaNodeColors[i4] * oneMinusOpacity);
                            rgbaNodeColors[i4+1] = (overlayRGBV[i4+1] * opacity)
                            + (rgbaNodeColors[i4+1] * oneMinusOpacity);
                            rgbaNodeColors[i4+2] = (overlayRGBV[i4+2] * opacity)
                            + (rgbaNodeColors[i4+2] * oneMinusOpacity);
                        }
                        else {
                            rgbaNodeColors[i4] = overlayRGBV[i4];
                            rgbaNodeColors[i4+1] = overlayRGBV[i4+1];
                            rgbaNodeColors[i4+2] = overlayRGBV[i4+2];
                        }
                    }
                }
                
                firstOverlayFlag = false;
            }
        }
    }
    
    delete[] overlayRGBV;
}

/**
 * Assign label coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param labelColumnName
 *    Name of selected column.
 * @param numberOfNodes
 *    Number of nodes in surface.
 * @param rgbv
 *    Color components set by this method.
 *    Red, green, blue, valid.  If the valid component is
 *    zero, it indicates that the overlay did not assign
 *    any coloring to the node.
 * @return
 *    True if coloring is valid, else false.
 */
bool 
SurfaceNodeColoring::assignLabelColoring(BrainStructure* brainStructure, 
                                         const AString& labelColumnName,
                                         const int32_t numberOfNodes,
                                         float* rgbv)
{
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t i4 = i * 4;
        rgbv[i4]   = 1.0;
        rgbv[i4+1] = 0.0;
        rgbv[i4+2] = 0.0;
        rgbv[i4+3] = 1.0;
    }
    
    return true;
}

bool 
SurfaceNodeColoring::assignMetricColoring(BrainStructure* brainStructure, 
                                          const AString& metricColumnName,
                                          const int32_t numberOfNodes,
                                          float* rgbv)
{
    std::vector<MetricFile*> allMetricFiles;
    brainStructure->getMetricFiles(allMetricFiles);
    
    int32_t columnIndex = -1;
    MetricFile* metricFile = NULL;
    for (std::vector<MetricFile*>::iterator iter = allMetricFiles.begin();
         iter != allMetricFiles.end();
         iter++) {
        MetricFile* mf = *iter;
        columnIndex = mf->getColumnIndexFromColumnName(metricColumnName);
        if (columnIndex >= 0) {
            metricFile = mf;
            break;
        }
    }
    
    if (columnIndex < 0) {
        return false;
    }
    
    const float* data = metricFile->getValuePointerForColumn(columnIndex);
    
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t i4 = i * 4;
        rgbv[i4]   = 0.0;
        rgbv[i4+1] = 0.0;
        rgbv[i4+2] = 0.0;
        rgbv[i4+3] = 1.0;
        if (data[i] > 0) {
            rgbv[i4] = 1.0;
        }
        else if (data[i] < 0) {
            rgbv[i4+1] = 1.0;
        }
        else {
            rgbv[i4+3] = 0.0;
        }
    }   
    
    return true;
}

bool 
SurfaceNodeColoring::assignRgbaColoring(BrainStructure* brainStructure, 
                                        const AString& rgbaColumnName,
                                        const int32_t numberOfNodes,
                                        float* rgbv)
{
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t i4 = i * 4;
        rgbv[i4]   = 0.0;
        rgbv[i4+1] = 0.0;
        rgbv[i4+2] = 1.0;
        rgbv[i4+3] = 1.0;
    }
    
    return true;
}
