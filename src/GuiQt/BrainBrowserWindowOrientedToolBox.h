
#ifndef __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

#include <stdint.h>

#include <QDockWidget>

#include "EventListenerInterface.h"

namespace caret {
    class BorderSelectionViewController;
    class ConnectivityLoaderManagerViewController;
    class OverlaySetViewController;
    
    class BrainBrowserWindowOrientedToolBox : public QDockWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const Qt::Orientation orientation,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowOrientedToolBox();
        
        void receiveEvent(Event* event);
        
    private:
        BrainBrowserWindowOrientedToolBox(const BrainBrowserWindowOrientedToolBox&);
        BrainBrowserWindowOrientedToolBox& operator=(const BrainBrowserWindowOrientedToolBox&);
        
        ConnectivityLoaderManagerViewController* createConnectivityWidget(const Qt::Orientation orientation);
        
        OverlaySetViewController* overlaySetViewController;
        
        BorderSelectionViewController* borderSelectionViewController;
        
        ConnectivityLoaderManagerViewController* connectivityViewController;
        
        int32_t browserWindowIndex;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

