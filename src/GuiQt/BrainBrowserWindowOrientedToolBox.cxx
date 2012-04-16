

#include <iostream>

#include <QAction>
#include <QLayout>
#include <QToolBox>

#include "BorderSelectionViewController.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "ConnectivityLoaderControl.h"
#include "ConnectivityLoaderFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "OverlaySetViewController.h"
#include "SessionManager.h"
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
    
    this->setAllowedAreas(Qt::LeftDockWidgetArea);
    
    this->toggleViewAction()->setText("Toolbox");
    this->setWindowTitle(title);
    
    this->overlaySetViewController = new OverlaySetViewController(browserWindowIndex,
                                                                  this);    
    
    //this->connectivityLoaderControl = this->createConnectivityWidget(orientation);
    
    this->borderSelectionViewController = new BorderSelectionViewController(browserWindowIndex,
                                                                            this);
    std::vector<QWidget*> contentWidgets;
    std::vector<QString> contentWidgetNames;
    
    contentWidgets.push_back(this->overlaySetViewController);
    contentWidgetNames.push_back("Overlay");
    
    contentWidgets.push_back(this->borderSelectionViewController);
    contentWidgetNames.push_back("Borders");
    
    WuQCollapsibleWidget* collapsibleWidget = NULL;
    QToolBox* toolBoxWidget = NULL;
    
    const int toolBoxType = SessionManager::get()->getCaretPreferences()->getToolBoxType();
    switch (toolBoxType) {
        case 2:
            toolBoxWidget = new QToolBox();
            break;
        default:
            collapsibleWidget = new WuQCollapsibleWidget(this);
            //this->collapsibleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            break;
    }
    
    const int32_t numContentWidgets = static_cast<int32_t>(contentWidgets.size());
    for (int32_t i = 0; i < numContentWidgets; i++) {
        if (collapsibleWidget != NULL) {
            collapsibleWidget->addItem(contentWidgets[i], 
                                         contentWidgetNames[i]);
        }
        else if (toolBoxWidget != NULL) {
            toolBoxWidget->addItem(contentWidgets[i], 
                                         contentWidgetNames[i]);
        }
    }
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    if (collapsibleWidget != NULL) {
        layout->addWidget(collapsibleWidget);
    }
    else if (toolBoxWidget != NULL) {
        layout->addWidget(toolBoxWidget);
    }
    
    this->setWidget(widget);

    this->setMinimumWidth(300);
    this->setMaximumWidth(800);
//    this->setMinimumWidth(325);
//    this->setMaximumWidth(600);
//    this->setSizePolicy(QSizePolicy::Minimum,
//                          QSizePolicy::MinimumExpanding);
}

BrainBrowserWindowOrientedToolBox::~BrainBrowserWindowOrientedToolBox()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

ConnectivityLoaderControl* 
BrainBrowserWindowOrientedToolBox::createConnectivityWidget(const Qt::Orientation orientation)
{
    ConnectivityLoaderControl* clc = new ConnectivityLoaderControl(orientation);
    return clc;
/*
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(this->connectivityLoaderControl);
    return w;
*/
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

