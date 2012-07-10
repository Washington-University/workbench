
#ifndef __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

#include <stdint.h>

#include <QDockWidget>

#include "SceneableInterface.h"

class QTabWidget;

namespace caret {
    class BorderSelectionViewController;
    class ConnectivityManagerViewController;
    class FociSelectionViewController;
    class OverlaySetViewController;
    class VolumeSurfaceOutlineSetViewController;
    
    class BrainBrowserWindowOrientedToolBox :  public QDockWidget, public SceneableInterface {
        Q_OBJECT
        
    public:
        enum ToolBoxType {
            TOOL_BOX_LAYERS,
            TOOL_BOX_OVERLAYS_HORIZONTAL,
            TOOL_BOX_OVERLAYS_VERTICAL
        };
        
        BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const ToolBoxType toolBoxType,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowOrientedToolBox();

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
        int m_overlaySetTabIndex;
        
        BorderSelectionViewController* m_borderSelectionViewController;
        int m_borderSelectionTabIndex;
        
        ConnectivityManagerViewController* m_connectivityViewController;
        int m_connectivityTabIndex;
        
        ConnectivityManagerViewController* m_timeSeriesViewController;
        int m_timeSeriesTabIndex;
        
        FociSelectionViewController* m_fociSelectionViewController;
        int m_fociSelectionTabIndex;
        
        VolumeSurfaceOutlineSetViewController* m_volumeSurfaceOutlineSetViewController;
        int m_volumeSurfaceOutlineTabIndex;
        
        QTabWidget* m_tabWidget;
        
        QString m_toolBoxTitle;
        
        int32_t m_browserWindowIndex;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

