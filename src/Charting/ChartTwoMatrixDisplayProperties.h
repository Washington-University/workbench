#ifndef __CHART_TWO_MATRIX_DISPLAY_PROPERTIES_H__
#define __CHART_TWO_MATRIX_DISPLAY_PROPERTIES_H__

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


#include "CaretObject.h"

#include "EventListenerInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ChartTwoMatrixDisplayProperties : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        ChartTwoMatrixDisplayProperties();
        
        virtual ~ChartTwoMatrixDisplayProperties();
        
        ChartTwoMatrixDisplayProperties(const ChartTwoMatrixDisplayProperties& obj);

        ChartTwoMatrixDisplayProperties& operator=(const ChartTwoMatrixDisplayProperties& obj);
        
        bool isGridLinesDisplayed() const;
        
        void setGridLinesDisplayed(const bool displayGridLines);
        
        void resetPropertiesToDefault();
        
        bool isSelectedRowColumnHighlighted() const;
        
        void setSelectedRowColumnHighlighted(const bool highlightStatus);

        float getCellPercentageZoomWidth() const;
        
        void setCellPercentageZoomWidth(const float cellPercentageZoomWidth);
        
        float getCellPercentageZoomHeight() const;
        
        void setCellPercentageZoomHeight(const float cellPercentageZoomHeight);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperChartTwoMatrixDisplayProperties(const ChartTwoMatrixDisplayProperties& obj);

        void initializeInstance();
        
        SceneClassAssistant* m_sceneAssistant;

        float m_cellPercentageZoomHeight;
        
        float m_cellPercentageZoomWidth;
        
        bool m_highlightSelectedRowColumnFlag;
        
        bool m_displayGridLinesFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_MATRIX_DISPLAY_PROPERTIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_MATRIX_DISPLAY_PROPERTIES_DECLARE__

} // namespace
#endif  //__CHART_TWO_MATRIX_DISPLAY_PROPERTIES_H__
