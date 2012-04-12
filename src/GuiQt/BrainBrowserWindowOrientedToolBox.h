
#ifndef __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

#include <QDockWidget>

#include "EventListenerInterface.h"

namespace caret {
    class BorderSelectionViewController;
    class ConnectivityLoaderControl;
    class OverlaySetViewController;
    class WuQCollapsibleWidget;
    
    class BrainBrowserWindowOrientedToolBox : public QDockWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const Qt::Orientation orientation,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowOrientedToolBox();
        
        void receiveEvent(Event* event);
        
        virtual QSize sizeHint() const;
        
    private:
        BrainBrowserWindowOrientedToolBox(const BrainBrowserWindowOrientedToolBox&);
        BrainBrowserWindowOrientedToolBox& operator=(const BrainBrowserWindowOrientedToolBox&);
        
        ConnectivityLoaderControl* createConnectivityWidget(const Qt::Orientation orientation);
        
        WuQCollapsibleWidget* collapsibleWidget;
        
        OverlaySetViewController* overlaySetViewController;
        
        BorderSelectionViewController* borderSelectionViewController;
        
        ConnectivityLoaderControl* connectivityLoaderControl;
        
        int32_t browserWindowIndex;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_ORIENTED_TOOLBOX_H__

