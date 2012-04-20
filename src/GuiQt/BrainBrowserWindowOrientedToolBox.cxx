

#include <iostream>

#include <QAction>
#include <QLayout>
#include <QScrollArea>
#include <QStackedWidget>
#include <QToolBox>
#include <QTabBar>

#include "BorderSelectionViewController.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "ConnectivityManagerViewController.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "OverlaySetViewController.h"
#include "SessionManager.h"
#include "VolumeSurfaceOutlineSetViewController.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Construct the toolbox.
 * 
 * @param browserWindowIndex
 *    Index of browser window that contains this toolbox.
 * @param title
 *    Title for the toolbox.
 * @param location
 *    Locations allowed for this toolbox.
 */
BrainBrowserWindowOrientedToolBox::BrainBrowserWindowOrientedToolBox(const int32_t browserWindowIndex,
                                                                     const QString& title,
                                                                     const ToolBoxType toolBoxType,
                                                                     QWidget* parent)
:   QDockWidget(parent)
{
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES);
    
    this->browserWindowIndex = browserWindowIndex;
    
    this->toggleViewAction()->setText("Toolbox");
    
    this->toolBoxTitle = title;
    this->setWindowTitle(this->toolBoxTitle);
    
    bool isLayersToolBox  = false;
    bool isOverlayToolBox = false;
    Qt::Orientation orientation = Qt::Horizontal;
    switch (toolBoxType) {
        case TOOL_BOX_LAYERS:
            orientation = Qt::Vertical;
            isLayersToolBox = true;
            break;
        case TOOL_BOX_OVERLAYS_HORIZONTAL:
            orientation = Qt::Horizontal;
            isOverlayToolBox = true;
            break;
        case TOOL_BOX_OVERLAYS_VERTICAL:
            orientation = Qt::Vertical;
            isOverlayToolBox = true;
            break;
    }
    
    this->borderSelectionViewController = NULL;
    this->connectivityViewController = NULL;
    this->overlaySetViewController = NULL;
    this->timeSeriesViewController = NULL;
    this->volumeSurfaceOutlineSetViewController = NULL;

    this->overlaySetTabIndex = -1;
    this->connectivityTabIndex = -1;
    this->timeSeriesTabIndex = -1;
    this->borderSelectionTabIndex = -1;
    this->volumeSurfaceOutlineTabIndex = -1;
    
    this->stackedWidget = new QStackedWidget();
    QTabBar* tabBar = new QTabBar();

    if (isOverlayToolBox) {
        this->overlaySetViewController = new OverlaySetViewController(orientation,
                                                                      browserWindowIndex,
                                                                      this);    
        this->stackedWidget->addWidget(this->overlaySetViewController);
        this->overlaySetTabIndex           = tabBar->addTab("Overlay");
    }
    if (isOverlayToolBox) {
        this->connectivityViewController = new ConnectivityManagerViewController(orientation,
                                                                                 browserWindowIndex,
                                                                                 DataFileTypeEnum::CONNECTIVITY_DENSE);
        this->stackedWidget->addWidget(this->connectivityViewController);
        this->connectivityTabIndex         = tabBar->addTab("Connectivity");
    }
    if (isOverlayToolBox) {
        this->timeSeriesViewController = new ConnectivityManagerViewController(orientation,
                                                                               browserWindowIndex,
                                                                               DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
        
        this->stackedWidget->addWidget(this->timeSeriesViewController);
        this->timeSeriesTabIndex           = tabBar->addTab("Time-Series");
    }
    if (isLayersToolBox) {
        this->borderSelectionViewController = new BorderSelectionViewController(browserWindowIndex,
                                                                                this);
        this->stackedWidget->addWidget(this->borderSelectionViewController);        
        this->borderSelectionTabIndex      = tabBar->addTab("Borders");
    }
    if (isOverlayToolBox) {
        this->volumeSurfaceOutlineSetViewController = new VolumeSurfaceOutlineSetViewController(orientation,
                                                                                                this->browserWindowIndex);
        this->stackedWidget->addWidget(this->volumeSurfaceOutlineSetViewController);
        this->volumeSurfaceOutlineTabIndex = tabBar->addTab("Vol/Surf Outline");
    }
    
    /*
     * Put the stacked widget in a scroll area
     */ 
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(this->stackedWidget);
    scrollArea->setWidgetResizable(true);    
    
    /*
     * Adding tabs causes signal to be emitted so setup 
     * connection after adding tabs
     */
    QObject::connect(tabBar, SIGNAL(currentChanged(int)),
                     this, SLOT(tabIndexSelected(int)));
    
    /*
     * Layout the widgets
     */
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(tabBar);
    layout->addWidget(scrollArea);
    
    this->setWidget(widget);

    if (orientation == Qt::Horizontal) {
        this->setMinimumHeight(150);
        this->setMaximumHeight(800);
    }
    else {
        if (isOverlayToolBox) {
            this->setMinimumWidth(300);
            this->setMaximumWidth(800);
        }
        else {
            this->setMinimumWidth(200);
            this->setMaximumWidth(800);
        }
    }

    this->tabIndexSelected(tabBar->currentIndex());
    
    QObject::connect(this, SIGNAL(topLevelChanged(bool)),
                     this, SLOT(floatingStatusChanged(bool)));
}

/**
 * Destructor.
 */
BrainBrowserWindowOrientedToolBox::~BrainBrowserWindowOrientedToolBox()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when floating status changes.
 * @param status
 *   New floating status.
 */
void 
BrainBrowserWindowOrientedToolBox::floatingStatusChanged(bool status)
{
    QString title = this->toolBoxTitle;
    if (status) {
        title += (" "
                  + QString::number(this->browserWindowIndex + 1));
    }
    this->setWindowTitle(title);
}

/**
 * Called when a tab is selected.
 * @param indx
 *    Index of selected tab.
 */
void 
BrainBrowserWindowOrientedToolBox::tabIndexSelected(int indx)
{
        if (indx == this->borderSelectionTabIndex) {
            this->stackedWidget->setCurrentWidget(this->borderSelectionViewController);
        }
        else if (indx == this->connectivityTabIndex) {
            this->stackedWidget->setCurrentWidget(this->connectivityViewController);
        }
        else if (indx == this->overlaySetTabIndex) {
            this->stackedWidget->setCurrentWidget(this->overlaySetViewController);
        }
        else if (indx == this->timeSeriesTabIndex) {
            this->stackedWidget->setCurrentWidget(this->timeSeriesViewController);
        }
        else if (indx == this->volumeSurfaceOutlineTabIndex) {
            this->stackedWidget->setCurrentWidget(this->volumeSurfaceOutlineSetViewController);
        }
        else {
            CaretAssertMessage(0, "Widget index is invalid!");
        }
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
BrainBrowserWindowOrientedToolBox::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();
    }
    else if ((event->getEventType() == EventTypeEnum::EVENT_DATA_FILE_READ)
             || (event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)) {
//        this->tabWidget->setCurrentWidget(this->overlayWidget);
    }
    else {
    }
}

