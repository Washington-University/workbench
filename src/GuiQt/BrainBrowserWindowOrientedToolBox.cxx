

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
#include "ConnectivityLoaderFile.h"
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
                                                     const Qt::Orientation orientation,
                                                     QWidget* parent)
:   QDockWidget(parent)
{
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES);
    
    this->browserWindowIndex = browserWindowIndex;
        
    this->toggleViewAction()->setText("Toolbox");
    this->setWindowTitle(title);
    
    
    this->overlaySetViewController = new OverlaySetViewController(orientation,
                                                                  browserWindowIndex,
                                                                  this);    
    
    this->connectivityViewController = new ConnectivityManagerViewController(orientation,
                                                                             browserWindowIndex,
                                                                             DataFileTypeEnum::CONNECTIVITY_DENSE);
    
    this->timeSeriesViewController = new ConnectivityManagerViewController(orientation,
                                                                             browserWindowIndex,
                                                                             DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
    
    this->borderSelectionViewController = new BorderSelectionViewController(browserWindowIndex,
                                                                            this);
    
    this->volumeSurfaceOutlineSetViewController = new VolumeSurfaceOutlineSetViewController(orientation,
                                                                                            this->browserWindowIndex);

    this->stackedWidget = new QStackedWidget();
    this->stackedWidget->addWidget(this->overlaySetViewController);
    this->stackedWidget->addWidget(this->connectivityViewController);
    this->stackedWidget->addWidget(this->timeSeriesViewController);
    this->stackedWidget->addWidget(this->borderSelectionViewController);
    this->stackedWidget->addWidget(this->volumeSurfaceOutlineSetViewController);
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(this->stackedWidget);
    scrollArea->setWidgetResizable(true);    
    
    QTabBar* tabBar = new QTabBar();
    this->overlaySetTabIndex           = tabBar->addTab("Overlay");
    this->connectivityTabIndex         = tabBar->addTab("Connectivity");
    this->timeSeriesTabIndex           = tabBar->addTab("Time-Series");
    this->borderSelectionTabIndex      = tabBar->addTab("Borders");
    this->volumeSurfaceOutlineTabIndex = tabBar->addTab("Vol/Surf Outline");
    QObject::connect(tabBar, SIGNAL(currentChanged(int)),
                     this, SLOT(tabIndexSelected(int)));
    
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
        this->setMinimumWidth(300);
        this->setMaximumWidth(800);
    }

    this->tabIndexSelected(tabBar->currentIndex());
    
//    this->setMinimumWidth(325);
//    this->setMaximumWidth(600);
//    this->setSizePolicy(QSizePolicy::Minimum,
//                          QSizePolicy::MinimumExpanding);
}

BrainBrowserWindowOrientedToolBox::~BrainBrowserWindowOrientedToolBox()
{
    EventManager::get()->removeAllEventsFromListener(this);
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

