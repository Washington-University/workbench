#ifndef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_INTERFACE_H__
#define __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/




#include "SelectionItemDataTypeEnum.h"

namespace caret {

    class Brain;
    class BrainOpenGLFixedPipeline;
    class BrainOpenGLTextRenderInterface;
    class ChartTwoOverlaySet;
    class ModelChartTwo;
    
    class BrainOpenGLChartTwoDrawingInterface {
        
    public:
        BrainOpenGLChartTwoDrawingInterface() { }
        
        virtual ~BrainOpenGLChartTwoDrawingInterface() { }
        
        /**
         * Draw charts from a chart overlay.
         *
         * @param brain
         *     Brain.
         * @param chartTwoModel
         *     The chart two model.
         * @param fixedPipelineDrawing
         *     The fixed pipeline OpenGL drawing.
         * @param textRenderer
         *     Text rendering.
         * @param translation
         *     The chart's translation.
         * @param zooming
         *     The chart's zooming.
         * @param chartOverlaySet
         *     Chart overlay set that is drawn.
         * @param selectionItemDataType
         *     Selected data type.
         * @param viewport
         *     Viewport for the chart.
         * @param tabIndex
         *     Index of the tab.
         */
        virtual void drawChartOverlaySet(Brain* brain,
                                         ModelChartTwo* chartTwoModel,
                                         BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                         BrainOpenGLTextRenderInterface* textRenderer,
                                         const float translation[3],
                                         const float zooming,
                                         ChartTwoOverlaySet* chartOverlaySet,
                                         const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                         const int32_t viewport[4],
                                         const int32_t tabIndex) = 0;


        // ADD_NEW_METHODS_HERE

    private:
        BrainOpenGLChartTwoDrawingInterface(const BrainOpenGLChartTwoDrawingInterface&);

        BrainOpenGLChartTwoDrawingInterface& operator=(const BrainOpenGLChartTwoDrawingInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    

} // namespace
#endif  //__BRAIN_OPEN_G_L_CHART_TWO_DRAWING_INTERFACE_H__
