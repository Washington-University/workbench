
#ifndef __BRAIN_BROWSER_WINDOW_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_TOOLBOX_H__

#include <QDockWidget>

#include "EventListenerInterface.h"
#include "OverlaySelectionControl.h"

class QLabel;
class QStackedWidget;
class QTabWidget;


namespace caret {
    class ConnectivityLoaderControl;
    class HyperLinkTextBrowser;
    class OverlaySelectionControl;
    
    class BrainBrowserWindowToolBox : public QDockWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const Qt::Orientation defaultOrientation,
                                  QWidget* parent = 0);
        
        ~BrainBrowserWindowToolBox();
        
        void receiveEvent(Event* event);

    signals:
        void controlRemoved();
        
    private slots:
        void dockFloated(bool);
        
        void dockMoved(Qt::DockWidgetArea area);
        
        void clearInformationText();
        
        void removeIdSymbols();
        
        void updateDisplayedPanel();
        
    private:
        BrainBrowserWindowToolBox(const BrainBrowserWindowToolBox&);
        BrainBrowserWindowToolBox& operator=(const BrainBrowserWindowToolBox&);
        
        OverlaySelectionControl* createLayersWidget(const Qt::Orientation orientation);
        
        QWidget* createInformationWidget();
        
        ConnectivityLoaderControl* createConnectivityWidget(const Qt::Orientation orientation);
        
        QWidget* createLabelWidget();
        
        QWidget* createMetricWidget();
        
        QWidget* createGiftiColumnSelectionPanel();
        
        QWidget* createColorMappingPanel();
        
        QTabWidget* tabWidget;
        
        OverlaySelectionControl* topBottomOverlayControl;
        OverlaySelectionControl* leftRightOverlayControl;
        QWidget* overlayWidget;
        
        ConnectivityLoaderControl* topBottomConnectivityLoaderControl;
        ConnectivityLoaderControl* leftRightConnectivityLoaderControl;
        QWidget* connectivityWidget;
        
        QWidget* labelWidget;
        
        QWidget* metricWidget;
        
        Qt::Orientation orientation;
        
        int32_t browserWindowIndex;
        
        QWidget* informationWidget;
        
        HyperLinkTextBrowser* informationTextBrowser;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_TOOLBOX_H__

