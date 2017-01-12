#ifndef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_H__
#define __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_H__

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


#include "BrainOpenGLChartTwoDrawingInterface.h"



namespace caret {

    class BrainOpenGLChartTwoDrawingFixedPipeline : public BrainOpenGLChartTwoDrawingInterface {
        
    public:
        BrainOpenGLChartTwoDrawingFixedPipeline();
        
        virtual ~BrainOpenGLChartTwoDrawingFixedPipeline();
        
        virtual void drawChartOverlaySet(Brain* brain,
                                         BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                         BrainOpenGLTextRenderInterface* textRenderer,
                                         ChartOverlaySet* chartOverlaySet,
                                         const SelectionItemDataTypeEnum::Enum selectionItemDataType,
                                         const int32_t viewport[4],
                                         const int32_t tabIndex) override;

        // ADD_NEW_METHODS_HERE

    private:
//        class Margins {
//        public:
//            Margins(const double defaultSize) {
//                m_bottom = defaultSize;
//                m_left   = defaultSize;
//                m_right  = defaultSize;
//                m_top    = defaultSize;
//            }
//            
//            double m_bottom;
//            double m_left;
//            double m_right;
//            double m_top;
//        };

        BrainOpenGLChartTwoDrawingFixedPipeline(const BrainOpenGLChartTwoDrawingFixedPipeline&);

        BrainOpenGLChartTwoDrawingFixedPipeline& operator=(const BrainOpenGLChartTwoDrawingFixedPipeline&);
        
        void restoreStateOfOpenGL();
        
        void saveStateOfOpenGL();
        
        void resetIdentification();
        
        void processIdentification();
        
        void drawMatrixChart();
        
        Brain* m_brain;
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        BrainOpenGLTextRenderInterface* m_textRenderer;
        
        ChartOverlaySet* m_chartOverlaySet;
        
        SelectionItemDataTypeEnum::Enum m_selectionItemDataType;
        
        int32_t m_viewport[4];
        
        int32_t m_tabIndex;
        
        std::vector<int32_t> m_identificationIndices;
        
        bool m_identificationModeFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_CHART_TWO_DRAWING_FIXED_PIPELINE_H__
