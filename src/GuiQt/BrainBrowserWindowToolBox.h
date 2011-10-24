
#ifndef __BRAIN_BROWSER_WINDOW_TOOLBOX_H__
#define __BRAIN_BROWSER_WINDOW_TOOLBOX_H__

#include <QDockWidget>

#include "EventListenerInterface.h"
#include "OverlaySelectionControl.h"

class QLabel;
class QStackedWidget;
class QTabWidget;


namespace caret {
    class HyperLinkTextBrowser;
    class OverlaySelectionControl;
    
    class BrainBrowserWindowToolBox : public QDockWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBox(const int32_t browserWindowIndex,
                                  const QString& title,
                                  const Qt::Orientation defaultOrientation);
        
        ~BrainBrowserWindowToolBox();
        
        void receiveEvent(Event* event);

    signals:
        void controlRemoved();
        
    private slots:
        void dockFloated(bool);
        
        void dockMoved(Qt::DockWidgetArea area);
        
        void clearInformationText();
        
        void removeIdSymbols();
        
        void updateMySize();
        
        void updateDisplayedPanel();
        
    private:
        BrainBrowserWindowToolBox(const BrainBrowserWindowToolBox&);
        BrainBrowserWindowToolBox& operator=(const BrainBrowserWindowToolBox&);
        
        OverlaySelectionControl* createSurfaceLayersWidget(const Qt::Orientation orientation);
        
        OverlaySelectionControl* createVolumeLayersWidget(const Qt::Orientation orientation);
        
        QWidget* createInformationWidget();
        
        QWidget* createConnectivityWidget();
        
        QWidget* createLabelWidget();
        
        QWidget* createMetricWidget();
        
        QWidget* createGiftiColumnSelectionPanel();
        
        QWidget* createColorMappingPanel();
        
        QTabWidget* tabWidget;
        
        OverlaySelectionControl* topBottomSurfaceOverlayControl;
        OverlaySelectionControl* leftRightSurfaceOverlayControl;
        QWidget* surfaceOverlayWidget;
        
        OverlaySelectionControl* volumeOverlayControl;
        
        QWidget* connectivityWidget;
        
        QWidget* labelWidget;
        
        QWidget* metricWidget;
        
        QString dockTitle;
        
        Qt::Orientation orientation;
        
        int32_t browserWindowIndex;
        
        QWidget* informationWidget;
        
        HyperLinkTextBrowser* informationTextBrowser;
    };    
}

#endif // __BRAIN_BROWSER_WINDOW_TOOLBOX_H__

