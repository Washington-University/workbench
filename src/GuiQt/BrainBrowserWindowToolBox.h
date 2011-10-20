
#ifndef __BRAIN_BROWSER_WINDOW_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_TOOLBOX_H__

#include <QDockWidget>

#include "EventListenerInterface.h"
#include "OverlaySelectionControl.h"

class QLabel;
class QStackedWidget;
class QTabWidget;


namespace caret {
    class OverlaySelectionControl;
    
    class BrainBrowserWindowToolBox : public QDockWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        enum Location {
            TOP_OR_BOTTOM,
            LEFT_OR_RIGHT
        };

        BrainBrowserWindowToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const Location location);
        
        ~BrainBrowserWindowToolBox();
        
        void receiveEvent(Event* event);

    signals:
        void controlRemoved();
        
    private slots:
        void dockFloated(bool);
        
        void dockMoved(Qt::DockWidgetArea area);
        
        void updateMySize();
        
        void updateDisplayedPanel();
        
    private:
        BrainBrowserWindowToolBox(const BrainBrowserWindowToolBox&);
        BrainBrowserWindowToolBox& operator=(const BrainBrowserWindowToolBox&);
        
        OverlaySelectionControl* createSurfaceLayersWidget(const OverlaySelectionControl::Orientation orientation);
        
        OverlaySelectionControl* createVolumeLayersWidget(const OverlaySelectionControl::Orientation orientation);
        
        QWidget* createIdentificationWidget();
        
        QWidget* createConnectivityWidget();
        
        QWidget* createLabelWidget();
        
        QWidget* createMetricWidget();
        
        QWidget* createGiftiColumnSelectionPanel();
        
        QWidget* createColorMappingPanel();
        
        QTabWidget* tabWidget;
        
        OverlaySelectionControl* surfaceOverlayControl;
        
        OverlaySelectionControl* volumeOverlayControl;
        
        QWidget* connectivityWidget;
        
        QWidget* labelWidget;
        
        QWidget* metricWidget;
        
        QString dockTitle;
        
        Qt::DockWidgetArea dockWidgetArea;
        
        int32_t browserWindowIndex;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_TOOLBOX_H__

