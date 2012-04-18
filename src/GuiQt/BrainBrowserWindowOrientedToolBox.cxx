

#include <iostream>

#include <QAction>
#include <QLayout>
#include <QToolBox>

#include "BorderSelectionViewController.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "ConnectivityLoaderManagerViewController.h"
#include "ConnectivityLoaderFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "OverlaySetViewController.h"
#include "SessionManager.h"
#include "VolumeSurfaceOutlineSetViewController.h"
#include "WuQCollapsibleWidget.h"
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
    
    this->connectivityViewController = NULL;
//    this->connectivityViewController = new ConnectivityLoaderManagerViewController(orientation);
    
    this->borderSelectionViewController = new BorderSelectionViewController(browserWindowIndex,
                                                                            this);
    
    this->volumeSurfaceOutlineSetViewController = new VolumeSurfaceOutlineSetViewController(orientation,
                                                                                            this->browserWindowIndex);
    
    std::vector<QWidget*> contentWidgets;
    std::vector<QString> contentWidgetNames;
    
    contentWidgets.push_back(this->overlaySetViewController);
    contentWidgetNames.push_back("Overlay");
    
    if (this->connectivityViewController != NULL) {
        contentWidgets.push_back(this->connectivityViewController);
        contentWidgetNames.push_back("Connectivity");
    }
    
    contentWidgets.push_back(this->borderSelectionViewController);
    contentWidgetNames.push_back("Borders");
    
    contentWidgets.push_back(this->volumeSurfaceOutlineSetViewController);
    contentWidgetNames.push_back("Volume Surface Outline");
    
    WuQCollapsibleWidget* collapsibleWidget = new WuQCollapsibleWidget(this);
            //this->collapsibleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    const int32_t numContentWidgets = static_cast<int32_t>(contentWidgets.size());
    for (int32_t i = 0; i < numContentWidgets; i++) {
            collapsibleWidget->addItem(contentWidgets[i], 
                                         contentWidgetNames[i]);
    }
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(collapsibleWidget);
    
    this->setWidget(widget);

    if (orientation == Qt::Horizontal) {
        this->setMinimumHeight(150);
        this->setMaximumHeight(800);
    }
    else {
        this->setMinimumWidth(300);
        this->setMaximumWidth(800);
    }
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
    else if ((event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)
             || (event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)) {
//        this->tabWidget->setCurrentWidget(this->overlayWidget);
    }
    else {
    }
}

