#ifndef __CHART_TOOL_BOX_VIEW_CONTROLLER_H__
#define __CHART_TOOL_BOX_VIEW_CONTROLLER_H__

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


#include <QWidget>

#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QStackedWidget;
class QTabWidget;

namespace caret {
    class ChartHistoryViewController;
    class ChartModel;
    class ChartSelectionViewController;
    class SceneClassAssistant;

    class ChartToolBoxViewController : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        ChartToolBoxViewController(const Qt::Orientation orientation,
                                   const int32_t browserWindowIndex,
                                   const QString& parentObjectName,
                                   QWidget* parent);
        
        virtual ~ChartToolBoxViewController();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

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
        ChartToolBoxViewController(const ChartToolBoxViewController&);

        ChartToolBoxViewController& operator=(const ChartToolBoxViewController&);
        
        ChartModel* getSelectedChartModel();
        
        SceneClassAssistant* m_sceneAssistant;

        const int32_t m_browserWindowIndex;
        
        QTabWidget* m_tabWidget;
        
        ChartSelectionViewController* m_chartSelectionViewController;
        
        ChartHistoryViewController* m_chartHistoryViewController;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TOOL_BOX_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TOOL_BOX_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_TOOL_BOX_VIEW_CONTROLLER_H__
