#ifndef __BRAIN_OPEN_G_L_CHART_DRAWING_INTERFACE_H__
#define __BRAIN_OPEN_G_L_CHART_DRAWING_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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


#include "AString.h"

#include "SelectionItemDataTypeEnum.h"

/**
 * \class caret::BrainOpenGLTextRenderInterface
 * \brief Interface for drawing charts with OpenGL.
 * \ingroup Brain
 */

namespace caret {

    class Brain;
    class BrainOpenGLFixedPipeline;
    class BrainOpenGLTextRenderInterface;
    class ChartModelCartesian;
    class ChartableMatrixInterface;
    
    class BrainOpenGLChartDrawingInterface {
        
    public:
        BrainOpenGLChartDrawingInterface() { }
        
        virtual ~BrainOpenGLChartDrawingInterface() { }
        
        /**
         * Draw a cartesian chart in the given viewport.
         *
         * @param brain
         *     Brain.
         * @param fixedPipelineDrawing
         *     The fixed pipeline OpenGL drawing.
         * @param viewport
         *     Viewport for the chart.
         * @param textRenderer
         *     Text rendering.
         * @param cartesianChart
         *     Cartesian Chart that is drawn.
         * @param selectionItemDataType
         *     Selected data type.
         * @param tabIndex
         *     Index of the tab.
         */
        virtual void drawCartesianChart(Brain* brain,
                                        BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                        const int32_t viewport[4],
                                        BrainOpenGLTextRenderInterface* textRenderer,
                                        ChartModelCartesian* cartesianChart,
                                        const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                        const int32_t tabIndex) = 0;
        
        /**
         * Draw a matrix chart in the given viewport.
         *
         * @param brain
         *     Brain.
         * @param fixedPipelineDrawing
         *     The fixed pipeline OpenGL drawing.
         * @param viewport
         *     Viewport for the chart.
         * @param textRenderer
         *     Text rendering.
         * @param chartMatrixInterface
         *     Chart matrix interface containing matrix data.
         * @param scalarDataSeriesMapIndex
         *     Scalar data series selected map index.
         * @param selectionItemDataType
         *     Selected data type.
         * @param tabIndex
         *     Index of the tab.
         */
        virtual void drawMatrixChart(Brain* brain,
                                     BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                     const int32_t viewport[4],
                                     BrainOpenGLTextRenderInterface* textRenderer,
                                     ChartableMatrixInterface* chartMatrixInterface,
                                     const int32_t scalarDataSeriesMapIndex,
                                     const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                     const int32_t tabIndex) = 0;
        
    private:
        BrainOpenGLChartDrawingInterface(const BrainOpenGLChartDrawingInterface&);

        BrainOpenGLChartDrawingInterface& operator=(const BrainOpenGLChartDrawingInterface&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_CHART_DRAWING_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_CHART_DRAWING_INTERFACE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_CHART_DRAWING_INTERFACE_H__
