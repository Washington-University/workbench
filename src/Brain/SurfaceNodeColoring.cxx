
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
                                       const SurfaceOverlaySet* surfaceOverlaySet,
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
    
    bool firstOverlayFlag = true;
    float* overlayRGBV = new float[numNodes * 4];
    for (int32_t iOver = 0; iOver < numberOfDisplayedOverlays; iOver++) {
        const SurfaceOverlay* overlay = surfaceOverlaySet->getOverlay(iOver);
        if (overlay->isEnabled()) {
            const SurfaceOverlayDataTypeEnum::Enum overlayType = overlay->getSelectedType();
            switch (overlayType) {
                case SurfaceOverlayDataTypeEnum::NONE:
                    break;
                case SurfaceOverlayDataTypeEnum::CONNECTIVITY:
                    break;
                case SurfaceOverlayDataTypeEnum::LABEL:
                    this->assignLabelColoring(overlay, numNodes, overlayRGBV);
                    break;
                case SurfaceOverlayDataTypeEnum::METRIC:
                    this->assignMetricColoring(overlay, numNodes, overlayRGBV);
                    break;
                case SurfaceOverlayDataTypeEnum::RGBA:
                    this->assignRgbaColoring(overlay, numNodes, overlayRGBV);
                    break;
            }
            
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

/**
 * Assign label coloring to nodes.
 * @param surfaceOverlay
 *    The surface overlay.
 * @param numberOfNodes
 *    Number of nodes in surface.
 * @param rgbv
 *    Color components set by this method.
 *    Red, green, blue, valid.  If the valid component is
 *    zero, it indicates that the overlay did not assign
 *    any coloring to the node.
 */
void 
SurfaceNodeColoring::assignLabelColoring(const SurfaceOverlay* surfaceOverlay,
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
}

void 
SurfaceNodeColoring::assignMetricColoring(const SurfaceOverlay* surfaceOverlay,
                                          const int32_t numberOfNodes,
                                          float* rgbv)
{
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t i4 = i * 4;
        rgbv[i4]   = 0.0;
        rgbv[i4+1] = 1.0;
        rgbv[i4+2] = 0.0;
        rgbv[i4+3] = 1.0;
    }    
}

void 
SurfaceNodeColoring::assignRgbaColoring(const SurfaceOverlay* surfaceOverlay,
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
}
