
#ifndef __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

#include <stdint.h>

#include <QDockWidget>

#include "EventListenerInterface.h"

class QStackedWidget;

namespace caret {
    class BorderSelectionViewController;
    class ConnectivityLoaderManagerViewController;
    class OverlaySetViewController;
    class VolumeSurfaceOutlineSetViewController;
    
    class BrainBrowserWindowOrientedToolBox : public QDockWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const Qt::Orientation orientation,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowOrientedToolBox();
        
        void receiveEvent(Event* event);
        
    private slots:
        void tabIndexSelected(int indx);
        
    private:
        BrainBrowserWindowOrientedToolBox(const BrainBrowserWindowOrientedToolBox&);
        BrainBrowserWindowOrientedToolBox& operator=(const BrainBrowserWindowOrientedToolBox&);
        
        OverlaySetViewController* overlaySetViewController;
        int overlaySetTabIndex;
        
        BorderSelectionViewController* borderSelectionViewController;
        int borderSelectionTabIndex;
        
        ConnectivityLoaderManagerViewController* connectivityViewController;
        int connectivityTabIndex;
        
        VolumeSurfaceOutlineSetViewController* volumeSurfaceOutlineSetViewController;
        int volumeSurfaceOutlineTabIndex;
        
        QStackedWidget* stackedWidget;
        
        int32_t browserWindowIndex;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

