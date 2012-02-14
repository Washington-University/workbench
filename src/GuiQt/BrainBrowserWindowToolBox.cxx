

#include <iostream>

#include <QAction>
#include <QButtonGroup>
#include <QChar>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStringList>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolBar>

#include "BrainBrowserWindowToolBox.h"
#include "CaretAssert.h"
#include "ConnectivityLoaderControl.h"
#include "ConnectivityLoaderFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventIdentificationSymbolRemoval.h"
#include "EventInformationTextDisplay.h"
#include "EventUserInterfaceUpdate.h"
#include "InformationDisplayWidget.h"
#include "OverlaySelectionControl.h"
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
BrainBrowserWindowToolBox::BrainBrowserWindowToolBox(const int32_t browserWindowIndex,
                                                     const QString& title,
                                                     const Qt::Orientation defaultOrientation,
                                                     QWidget* parent)
:   QDockWidget(parent)
{
    this->orientation = defaultOrientation;
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_INFORMATION_TEXT_DISPLAY);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DATA_FILE_READ);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES);
    
    this->browserWindowIndex = browserWindowIndex;
    
    this->setAllowedAreas(Qt::TopDockWidgetArea
                          | Qt::BottomDockWidgetArea);
//    OverlaySelectionControl::Orientation overlayControlOrientation;
//    switch (location) {
//        case TOP_OR_BOTTOM:
//            overlayControlOrientation = OverlaySelectionControl::HORIZONTAL;
//            this->setAllowedAreas(Qt::TopDockWidgetArea
//                                  | Qt::BottomDockWidgetArea);
//            break;
//        case LEFT_OR_RIGHT:
//            overlayControlOrientation = OverlaySelectionControl::VERTICAL;
//            this->setAllowedAreas(Qt::RightDockWidgetArea
//                                  | Qt::LeftDockWidgetArea);
//            break;
//    }
    
    this->toggleViewAction()->setText("Toolbox");
    this->setWindowTitle(title);
    
    this->topBottomOverlayControl = this->createLayersWidget(Qt::Horizontal);
    this->leftRightOverlayControl = this->createLayersWidget(Qt::Vertical);
    this->leftRightOverlayControl->setVisible(false);
    this->overlayWidget = new QWidget();
    QVBoxLayout* overlayLayout = new QVBoxLayout(this->overlayWidget);
    overlayLayout->addWidget(this->topBottomOverlayControl);
    overlayLayout->addWidget(this->leftRightOverlayControl);
    
    this->informationWidget = new InformationDisplayWidget();
    
    this->topBottomConnectivityLoaderControl = this->createConnectivityWidget(Qt::Horizontal);
    this->leftRightConnectivityLoaderControl = this->createConnectivityWidget(Qt::Vertical);
    this->connectivityWidget = new QWidget();
    QVBoxLayout* connectivityLayout = new QVBoxLayout(this->connectivityWidget);
    connectivityLayout->addWidget(this->topBottomConnectivityLoaderControl);
    connectivityLayout->addWidget(this->leftRightConnectivityLoaderControl);
    
    //this->metricWidget   = this->createMetricWidget();
    //this->labelWidget   = this->createLabelWidget();
    
    this->tabWidget = new QTabWidget();
    this->tabWidget->setUsesScrollButtons(true);
    this->tabWidget->addTab(this->overlayWidget, "Layers");
    this->tabWidget->addTab(this->informationWidget, "Info");
    this->tabWidget->addTab(this->connectivityWidget, "Connectivity");
    //this->tabWidget->addTab(this->labelWidget, "Label");
    //this->tabWidget->addTab(this->metricWidget, "Metric");
    
    if (defaultOrientation == Qt::Vertical) {
        const int width = this->overlayWidget->maximumWidth();
        this->tabWidget->setFixedWidth(width);
        this->informationWidget->setFixedWidth(width);
        this->connectivityWidget->setFixedWidth(width);
    }
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(this->tabWidget);
    layout->addStretch();
    
    bool useScrollBars = false;
    if (useScrollBars) {
        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidget(w);
        scrollArea->setWidgetResizable(true);
        this->setWidget(scrollArea);
    }
    else {
        this->setWidget(w);
    }
    this->dockFloated(false);

    QObject::connect(this->tabWidget, SIGNAL(currentChanged(int)),
                     this, SLOT(updateDisplayedPanel()));
    
    QObject::connect(this, SIGNAL(topLevelChanged(bool)),
                     this, SLOT(dockFloated(bool)));
    QObject::connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
                     this, SLOT(dockMoved(Qt::DockWidgetArea)));
    
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->tabWidget->setCurrentWidget(this->overlayWidget);
}

BrainBrowserWindowToolBox::~BrainBrowserWindowToolBox()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

void 
BrainBrowserWindowToolBox::dockFloated(bool /*isFloated*/)
{
}

void 
BrainBrowserWindowToolBox::dockMoved(Qt::DockWidgetArea area)
{
    int surfaceWidgetIndex = -1;
    
    switch (area) {
        case Qt::LeftDockWidgetArea:
            this->orientation = Qt::Vertical;
            surfaceWidgetIndex = 1;
            break;
        case Qt::RightDockWidgetArea:
            this->orientation = Qt::Vertical;
            surfaceWidgetIndex = 1;
            break;
        case Qt::TopDockWidgetArea:
            this->orientation = Qt::Horizontal;
            surfaceWidgetIndex = 0;
            break;
        case Qt::BottomDockWidgetArea:
            this->orientation = Qt::Horizontal;
            surfaceWidgetIndex = 0;
            break;
        default:
            break;
    }
    
    this->updateDisplayedPanel();

//    //surfaceWidgetIndex = 0;
//    
//    if (surfaceWidgetIndex >= 0) {
//        QWidget* viewedWidget = this->tabWidget->currentWidget();
//                
//        this->tabWidget->setCurrentWidget(viewedWidget);
//        
//        /*
//        std::cout << "Widget index: " << surfaceWidgetIndex << std::endl;
//        GuiHelper::printSize("Stacked Widget Size: ", viewedWidget->size());
//        GuiHelper::printSize("Stacked Widget: ", viewedWidget->sizeHint());
//        GuiHelper::printSize("Widget 0 Size: ", this->surfaceLayersStackedWidget->widget(0)->size());
//        GuiHelper::printSize("Widget 0 Size Hint: ", this->surfaceLayersStackedWidget->widget(0)->sizeHint());
//        GuiHelper::printSize("Widget 1 Size: ", this->surfaceLayersStackedWidget->widget(1)->size());
//        GuiHelper::printSize("Widget 1 Size Hint: ", this->surfaceLayersStackedWidget->widget(1)->sizeHint());
//        */
//    }
    //this->updateGeometry();
}

OverlaySelectionControl* 
BrainBrowserWindowToolBox::createLayersWidget(const Qt::Orientation orientation)
{
    OverlaySelectionControl* overlaySelectionControl =
    new OverlaySelectionControl(this->browserWindowIndex,
                                orientation);

    QObject::connect(overlaySelectionControl, SIGNAL(controlRemoved()),
                     this, SIGNAL(controlRemoved()));
    
    return overlaySelectionControl;
}

ConnectivityLoaderControl* 
BrainBrowserWindowToolBox::createConnectivityWidget(const Qt::Orientation orientation)
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

QWidget* 
BrainBrowserWindowToolBox::createLabelWidget()
{
    QWidget* w = new QWidget();
    
    QVBoxLayout* l = new QVBoxLayout(w);
    l->addWidget(this->createGiftiColumnSelectionPanel(), Qt::AlignLeft);
    l->addStretch();
    
    return w;
}

QWidget* 
BrainBrowserWindowToolBox::createMetricWidget()
{
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(this->createColorMappingPanel(), "ColorMapping");
    tabWidget->addTab(this->createGiftiColumnSelectionPanel(), "Selection");
    tabWidget->setCurrentIndex(1);
    QWidget* w = new QWidget();
    QVBoxLayout* l = new QVBoxLayout(w);
    l->addWidget(tabWidget, Qt::AlignLeft);
    l->addStretch();
    
    return w;
}

QWidget* 
BrainBrowserWindowToolBox::createColorMappingPanel()
{
    QWidget* w = new QWidget();
    return w;
    
}

QWidget* 
BrainBrowserWindowToolBox::createGiftiColumnSelectionPanel()
{
    QWidget* gridWidget = new QWidget();
    QGridLayout* gl = new QGridLayout(gridWidget);
    
    int32_t iRow = 0;
    gl->addWidget(new QLabel("1"), iRow, 0);
    gl->addWidget(new QLabel("2"), iRow, 1);
    gl->addWidget(new QLabel("3"), iRow, 2);
    gl->addWidget(new QLabel("4"), iRow, 3);
    gl->addWidget(new QLabel("5"), iRow, 4);
    gl->addWidget(new QLabel("6"), iRow, 5);
    gl->addWidget(new QLabel("7"), iRow, 6);
    gl->addWidget(new QLabel("8"), iRow, 7);
    gl->addWidget(new QLabel("9"), iRow, 8);
    gl->addWidget(new QLabel("10"), iRow, 9);
    gl->addWidget(new QLabel("Structure"), iRow, 10);
    gl->addWidget(new QLabel("Data Name"), iRow, 11);
    iRow++;
    
    QButtonGroup* bg[10];
    for (int i = 0; i < 10; i++) {
        bg[i] = new QButtonGroup();
        
    }
    for (int32_t i = iRow; i < (iRow + 7); i++) {
        for (int32_t j = 0; j < 10; j++) {
            QRadioButton* rb = new QRadioButton("");
            gl->addWidget(rb, i, j);
            bg[j]->addButton(rb);
            
            if (i == iRow) {
                rb->setChecked(true);
            }
        }
        gl->addWidget(new QLabel("L,R"), i, 10);
        const QString dataColumnName = 
            QString::number(i)
            + ": subject-"
            + QString(QChar::fromAscii('A' + i));
        gl->addWidget(new QLabel(dataColumnName), i, 11);
    }
    
    
    QWidget* w = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(w);
    vl->addWidget(gridWidget);
    vl->addStretch();
    return w;
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
BrainBrowserWindowToolBox::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        uiEvent->setEventProcessed();
        
        this->updateDisplayedPanel();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_INFORMATION_TEXT_DISPLAY) {
        EventInformationTextDisplay* textEvent =
        dynamic_cast<EventInformationTextDisplay*>(event);
        CaretAssert(textEvent);
        
        textEvent->setEventProcessed();
        
        const AString text = textEvent->getText();
        if (text.isEmpty() == false) {
            if (textEvent->isImportant()) {
                this->tabWidget->setCurrentWidget(this->informationWidget);
            }
            this->informationWidget->processTextEvent(textEvent);
            textEvent->setEventProcessed();
        }
    }
    else if ((event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)
             || (event->getEventType() == EventTypeEnum::EVENT_SPEC_FILE_READ_DATA_FILES)) {
        this->tabWidget->setCurrentWidget(this->overlayWidget);
    }
    else {
    }
}

/**
 * Update the panel displayed in the toolbox.
 */
void 
BrainBrowserWindowToolBox::updateDisplayedPanel()
{
    const QWidget* selectedTopLevelWidget = this->tabWidget->currentWidget();
    if (selectedTopLevelWidget == this->overlayWidget) {
        switch (this->orientation) {
            case Qt::Horizontal:
                this->leftRightOverlayControl->setVisible(false);
                this->topBottomOverlayControl->setVisible(true);
                break;
            case Qt::Vertical:
                this->topBottomOverlayControl->setVisible(false);
                this->leftRightOverlayControl->setVisible(true);
                break;
        }
        this->topBottomOverlayControl->updateControl();
        this->leftRightOverlayControl->updateControl();
    }
    else if (selectedTopLevelWidget == this->informationWidget) {
        this->informationWidget->updateInformationDisplayWidget();
    }
    else if (selectedTopLevelWidget == this->connectivityWidget) {
        switch (this->orientation) {
            case Qt::Horizontal:
                this->leftRightConnectivityLoaderControl->setVisible(false);
                this->topBottomConnectivityLoaderControl->setVisible(true);
                break;
            case Qt::Vertical:
                this->topBottomConnectivityLoaderControl->setVisible(false);
                this->leftRightConnectivityLoaderControl->setVisible(true);
                break;
        }
        this->topBottomConnectivityLoaderControl->updateControl();
        this->leftRightConnectivityLoaderControl->updateControl();
    }
    else if (selectedTopLevelWidget == this->metricWidget) {
        
    }
    else if (selectedTopLevelWidget == this->labelWidget) {
        
    }
    else {
        CaretAssertMessage(0, "Invalid top level widget in ToolBox.");
    }
    
    switch (this->orientation) {
        case Qt::Horizontal:
            this->informationWidget->setMaximumHeight(this->topBottomOverlayControl->height());
            break;
        case Qt::Vertical:
            this->informationWidget->setMaximumHeight(5000);
            break;
    }
}


