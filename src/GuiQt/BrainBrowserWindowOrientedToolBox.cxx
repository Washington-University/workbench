

#include <iostream>

#include <QAction>
#include <QLayout>
#include <QScrollArea>
#include <QToolBox>
#include <QTabWidget>

#include "BorderSelectionViewController.h"
#include "BrainBrowserWindowOrientedToolBox.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "ConnectivityManagerViewController.h"
#include "FociSelectionViewController.h"
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
            this->toggleViewAction()->setText("Features Toolbox");
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
    this->fociSelectionViewController = NULL;
    this->overlaySetViewController = NULL;
    this->timeSeriesViewController = NULL;
    this->volumeSurfaceOutlineSetViewController = NULL;

    this->overlaySetTabIndex = -1;
    this->connectivityTabIndex = -1;
    this->timeSeriesTabIndex = -1;
    this->borderSelectionTabIndex = -1;
    this->fociSelectionTabIndex = -1;
    this->volumeSurfaceOutlineTabIndex = -1;
    
    this->tabWidget = new QTabWidget();
    
    if (isOverlayToolBox) {
        this->overlaySetViewController = new OverlaySetViewController(orientation,
                                                                      browserWindowIndex,
                                                                      this);  
//        this->addToTabWidget(this->overlaySetViewController, 
//                             "Overlay");
        this->tabWidget->addTab(this->overlaySetViewController ,
                                "Layers");
    }
    if (isOverlayToolBox) {
        this->connectivityViewController = new ConnectivityManagerViewController(orientation,
                                                                                 browserWindowIndex,
                                                                                 DataFileTypeEnum::CONNECTIVITY_DENSE);
        this->addToTabWidget(this->connectivityViewController, 
                             "Connectivity");
    }
    if (isOverlayToolBox) {
        this->timeSeriesViewController = new ConnectivityManagerViewController(orientation,
                                                                               browserWindowIndex,
                                                                               DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
        this->addToTabWidget(this->timeSeriesViewController, 
                             "Time-Series");
    }
    if (isLayersToolBox) {
        this->borderSelectionViewController = new BorderSelectionViewController(browserWindowIndex,
                                                                                this);
        this->addToTabWidget(this->borderSelectionViewController, 
                             "Borders");
    }
    
    if (isLayersToolBox) {
        this->fociSelectionViewController = new FociSelectionViewController(browserWindowIndex,
                                                                                this);
        this->addToTabWidget(this->fociSelectionViewController, 
                             "Foci");
    }
    
    if (isOverlayToolBox) {
        this->volumeSurfaceOutlineSetViewController = new VolumeSurfaceOutlineSetViewController(orientation,
                                                                                                this->browserWindowIndex);
        this->addToTabWidget(this->volumeSurfaceOutlineSetViewController, 
                             "Vol/Surf Outline");
    }
    
    /*
     * Layout the widgets
     */
//    QWidget* widget = new QWidget();
//    QVBoxLayout* layout = new QVBoxLayout(widget);
//    WuQtUtilities::setLayoutMargins(layout, 0, 0);
//    layout->addWidget(tabBar);
//    layout->addWidget(scrollArea);
    
    this->setWidget(this->tabWidget);

    if (orientation == Qt::Horizontal) {
        this->setMinimumHeight(200);
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

    QObject::connect(this, SIGNAL(topLevelChanged(bool)),
                     this, SLOT(floatingStatusChanged(bool)));
}

/**
 * Destructor.
 */
BrainBrowserWindowOrientedToolBox::~BrainBrowserWindowOrientedToolBox()
{
}

/**
 * Place widget into a scroll area and then into the tab widget.
 * @param page
 *    Widget that is added.
 * @param label
 *    Name corresponding to widget's tab.
 */
int 
BrainBrowserWindowOrientedToolBox::addToTabWidget(QWidget* page,
                                                  const QString& label)
{
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(page);
    scrollArea->setWidgetResizable(true);    
    
    int indx = this->tabWidget->addTab(scrollArea,
                                       label);
    return indx;
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
//    if (status) {
//        title += (" "
//                  + QString::number(this->browserWindowIndex + 1));
//    }
    this->setWindowTitle(title);
}

