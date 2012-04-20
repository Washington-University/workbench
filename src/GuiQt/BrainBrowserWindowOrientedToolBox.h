
#ifndef __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

#include <stdint.h>

#include <QDockWidget>

#include "EventListenerInterface.h"

class QStackedWidget;

namespace caret {
    class BorderSelectionViewController;
    class ConnectivityManagerViewController;
    class OverlaySetViewController;
    class VolumeSurfaceOutlineSetViewController;
    
    class BrainBrowserWindowOrientedToolBox : public QDockWidget, public EventListenerInterface {
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
        
        void receiveEvent(Event* event);
        
    private slots:
        void tabIndexSelected(int indx);
        
        void floatingStatusChanged(bool);
        
    private:
        BrainBrowserWindowOrientedToolBox(const BrainBrowserWindowOrientedToolBox&);
        BrainBrowserWindowOrientedToolBox& operator=(const BrainBrowserWindowOrientedToolBox&);
        
        OverlaySetViewController* overlaySetViewController;
        int overlaySetTabIndex;
        
        BorderSelectionViewController* borderSelectionViewController;
        int borderSelectionTabIndex;
        
        ConnectivityManagerViewController* connectivityViewController;
        int connectivityTabIndex;
        
        ConnectivityManagerViewController* timeSeriesViewController;
        int timeSeriesTabIndex;
        
        VolumeSurfaceOutlineSetViewController* volumeSurfaceOutlineSetViewController;
        int volumeSurfaceOutlineTabIndex;
        
        QStackedWidget* stackedWidget;
        
        QString toolBoxTitle;
        
        int32_t browserWindowIndex;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

