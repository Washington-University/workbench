#ifndef __BRAIN_OPEN_G_L_CHART_DRAWING_INTERFACE_H__
#define __BRAIN_OPEN_G_L_CHART_DRAWING_INTERFACE_H__

/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
