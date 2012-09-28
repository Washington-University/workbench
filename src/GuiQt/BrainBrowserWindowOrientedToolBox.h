
#ifndef __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

#include <stdint.h>

#include <QDockWidget>

#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QTabWidget;

namespace caret {
    class BorderSelectionViewController;
    class ConnectivityManagerViewController;
    class FiberOrientationSelectionViewController;
    class FociSelectionViewController;
    class LabelSelectionViewController;
    class OverlaySetViewController;
    class VolumeSurfaceOutlineSetViewController;
    
    class BrainBrowserWindowOrientedToolBox :  public QDockWidget, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        enum ToolBoxType {
            TOOL_BOX_FEATURES,
            TOOL_BOX_OVERLAYS_HORIZONTAL,
            TOOL_BOX_OVERLAYS_VERTICAL
        };
        
        BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const ToolBoxType toolBoxType,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowOrientedToolBox();

        void receiveEvent(Event* event);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private slots:
        void floatingStatusChanged(bool);
        
    private:
        BrainBrowserWindowOrientedToolBox(const BrainBrowserWindowOrientedToolBox&);
        BrainBrowserWindowOrientedToolBox& operator=(const BrainBrowserWindowOrientedToolBox&);
        
        int addToTabWidget(QWidget* page,
                           const QString& label);
        
        OverlaySetViewController* m_overlaySetViewController;
        
        BorderSelectionViewController* m_borderSelectionViewController;
        
        ConnectivityManagerViewController* m_connectivityViewController;
        
        ConnectivityManagerViewController* m_timeSeriesViewController;
        
        FiberOrientationSelectionViewController* m_fiberOrientationViewController;
        
        FociSelectionViewController* m_fociSelectionViewController;
        
        LabelSelectionViewController* m_labelSelectionViewController;
        
        VolumeSurfaceOutlineSetViewController* m_volumeSurfaceOutlineSetViewController;
        
        QTabWidget* m_tabWidget;
        
        QString m_toolBoxTitle;
        
        int32_t m_browserWindowIndex;

                
        int32_t m_overlayTabIndex;
        
        int32_t m_borderTabIndex;
        
        int32_t m_connectivityTabIndex;
        
        int32_t m_timeSeriesTabIndex;
        
        int32_t m_fiberOrientationTabIndex;
        
        int32_t m_fociTabIndex;
        
        int32_t m_labelTabIndex;
        
        int32_t m_volumeSurfaceOutlineTabIndex;
        
    };
}

#endif // __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

