#ifndef __CHART_MATRIX_DISPLAY_PROPERTIES_H__
#define __CHART_MATRIX_DISPLAY_PROPERTIES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "BrainConstants.h"
#include "CaretObject.h"
#include "ChartMatrixScaleModeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class AnnotationColorBar;

    class SceneClassAssistant;

    class ChartMatrixDisplayProperties : public CaretObject, public SceneableInterface {
        
    public:
        ChartMatrixDisplayProperties();
        
        virtual ~ChartMatrixDisplayProperties();
        
        ChartMatrixDisplayProperties(const ChartMatrixDisplayProperties& obj);

        ChartMatrixDisplayProperties& operator=(const ChartMatrixDisplayProperties& obj);
        
        bool isColorBarDisplayed() const;
        
        void setColorBarDisplayed(const bool displayed);
        
        float getCellWidth() const;
        
        void setCellWidth(const float cellSizeX);
        
        float getCellHeight() const;
        
        void setCellHeight(const float cellHeight);
        
        float getViewZooming() const;
        
        void setViewZooming(const float viewZooming);
        
        void getViewPanning(float viewPanningOut[2]) const;
        
        void setViewPanning(const float viewPanning[2]);
        
        ChartMatrixScaleModeEnum::Enum getScaleMode() const;
        
        void setScaleMode(const ChartMatrixScaleModeEnum::Enum scaleMode);
        
        bool isGridLinesDisplayed() const;
        
        void setGridLinesDisplayed(const bool displayGridLines);
        
        void resetPropertiesToDefault();
        
        bool isSelectedRowColumnHighlighted() const;
        
        void setSelectedRowColumnHighlighted(const bool highlightStatus);
        
        AnnotationColorBar* getColorBar();
        
        const AnnotationColorBar* getColorBar() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperChartMatrixDisplayProperties(const ChartMatrixDisplayProperties& obj);

        SceneClassAssistant* m_sceneAssistant;

        /** matrix cell width in pixels*/
        float m_cellWidth;
        
        /** matrix cell height in pixels*/
        float m_cellHeight;
        
        /** zooming for view of matrix*/
        float m_viewZooming;
        
        /** panning for view of matrix*/
        float m_viewPanning[2];
        
        /** scale mode for view of matrix*/
        ChartMatrixScaleModeEnum::Enum m_scaleMode;
        
        /** Display color bar */
        bool m_colorBarDisplayed;
        
        /** Highlight the selected row/column */
        bool m_highlightSelectedRowColumn;
        
        /** Display grid lines */
        bool m_displayGridLines;
        
        /** The color bar displayed in the graphics window */
        AnnotationColorBar* m_colorBar;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_MATRIX_DISPLAY_PROPERTIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_MATRIX_DISPLAY_PROPERTIES_DECLARE__

} // namespace
#endif  //__CHART_MATRIX_DISPLAY_PROPERTIES_H__
