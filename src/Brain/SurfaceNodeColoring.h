#ifndef __SURFACE_NODE_COLORING__H_
#define __SURFACE_NODE_COLORING__H_

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


#include "CaretObject.h"

namespace caret {

    class BrainStructure;
    class BrowserTabContent;
    class ConnectivityLoaderFile;
    class LabelFile;
    class MetricFile;
    class Palette;
    class PaletteColorMapping;
    class RgbaFile;
    class Surface;
    class OverlaySet;
    
    /// Performs coloring of surface nodes
    class SurfaceNodeColoring : public CaretObject {
        
    public:
        SurfaceNodeColoring();
        
        virtual ~SurfaceNodeColoring();
        
        void colorSurfaceNodes(const Surface* surface,
                               OverlaySet* overlaySet,
                               float* rgbaNodeColors);
    private:
        SurfaceNodeColoring(const SurfaceNodeColoring&);

        SurfaceNodeColoring& operator=(const SurfaceNodeColoring&);
        
    public:
        virtual AString toString() const;
        
    private:
        enum MetricColorType {
            METRIC_COLOR_TYPE_NORMAL,
            METRIC_COLOR_TYPE_POS_THRESH_COLOR,
            METRIC_COLOR_TYPE_NEG_THRESH_COLOR,
            METRIC_COLOR_TYPE_DO_NOT_COLOR
        };        
        
        bool assignConnectivityColoring(const BrainStructure* brainStructure,
                                        ConnectivityLoaderFile* connectivityLoaderFile,
                                        const int32_t numberOfNodes,
                                        float* rgbv);
        
        bool assignLabelColoring(const BrainStructure* brainStructure,
                                 const LabelFile* labelFile,
                                 const AString& labelMapUniqueID,
                                 const int32_t numberOfNodes,
                                 float* rgbv);

        bool assignMetricColoring(const BrainStructure* brainStructure,
                                  MetricFile* metricFile,
                                  const AString& metricMapUniqueID,
                                  const int32_t numberOfNodes,
                                  float* rgbv);
        
        bool assignRgbaColoring(const BrainStructure* brainStructure,
                                const RgbaFile* rgbaFile,
                                const AString& rgbaMapUniqueID,
                                const int32_t numberOfNodes,
                                float* rgbv);
        
};
    
#ifdef __SURFACE_NODE_COLORING_DECLARE__
#endif // __SURFACE_NODE_COLORING_DECLARE__

} // namespace
#endif  //__SURFACE_NODE_COLORING__H_
