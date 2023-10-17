
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __ANNOTATION_INSERT_NEW_WIDGET_DECLARE__
#include "AnnotationInsertNewWidget.h"
#undef __ANNOTATION_INSERT_NEW_WIDGET_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QRadioButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "Annotation.h"
#include "AnnotationFile.h"
#include "AnnotationManager.h"
#include "AnnotationMenuFileSelection.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventAnnotationCreateNewType.h"
#include "EventAnnotationGetSelectedInsertNewFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "Model.h"
#include "UserInputModeEnum.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::AnnotationInsertNewWidget
 * \brief Widget for creating new annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parent
 *     Parent of this widget.
 */
AnnotationInsertNewWidget::AnnotationInsertNewWidget(const UserInputModeEnum::Enum userInputMode,
                                                     const int32_t browserWindowIndex,
                                                     QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    m_widgetMode = WidgetMode::INVALID;
    switch (m_userInputMode) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            m_widgetMode = WidgetMode::ANNOTATIONS;
            break;
        case UserInputModeEnum::Enum::BORDERS:
            CaretAssert(0);
            break;
        case UserInputModeEnum::Enum::FOCI:
            CaretAssert(0);
            break;
        case UserInputModeEnum::Enum::IMAGE:
            CaretAssert(0);
            break;
        case UserInputModeEnum::Enum::INVALID:
            CaretAssert(0);
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
            m_widgetMode = WidgetMode::SAMPLES;
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            CaretAssert(0);
            break;
        case UserInputModeEnum::Enum::VIEW:
            CaretAssert(0);
            break;
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            break;
    }
    
    switch (m_widgetMode) {
        case WidgetMode::INVALID:
            CaretAssert(0);
            break;
        case WidgetMode::ANNOTATIONS:
            createAnnotationsWidgets();
            break;
        case WidgetMode::SAMPLES:
            createEditSamplesWidgets();
            break;
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_NEW_DRAWING_POLYHEDRON_SLICE_DEPTH);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE);
}

/**
 * Destructor.
 */
AnnotationInsertNewWidget::~AnnotationInsertNewWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
AnnotationInsertNewWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE) {
        EventAnnotationGetSelectedInsertNewFile* fileEvent(dynamic_cast<EventAnnotationGetSelectedInsertNewFile*>(event));
        CaretAssert(fileEvent);
        if (fileEvent->getUserInputMode() == m_userInputMode) {
            fileEvent->setAnnotationFile(m_fileSelectionMenu->getSelectedAnnotationFile());
            fileEvent->setEventProcessed();
        }
    }
}

/**
 * Create widgets for annotations input mode
 */
void
AnnotationInsertNewWidget::createAnnotationsWidgets()
{
    /*
     * Shape buttons
     */
    m_shapeActionGroup = new QActionGroup(this);
    QToolButton* shapeBoxToolButton      = createShapeToolButton(AnnotationTypeEnum::BOX,
                                                                 m_shapeActionGroup);
    QToolButton* shapeImageToolButton    = createShapeToolButton(AnnotationTypeEnum::IMAGE,
                                                                 m_shapeActionGroup);
    QToolButton* shapeLineToolButton     = createShapeToolButton(AnnotationTypeEnum::LINE,
                                                                 m_shapeActionGroup);
    QToolButton* shapePolygonToolButton  = createShapeToolButton(AnnotationTypeEnum::POLYGON,
                                                                 m_shapeActionGroup);
    m_polygonToolButton = shapePolygonToolButton;
    QToolButton* shapePolyLineToolButton = createShapeToolButton(AnnotationTypeEnum::POLYLINE,
                                                                 m_shapeActionGroup);
    m_polyLineToolButton = shapePolyLineToolButton;
    
    QToolButton* shapeOvalToolButton     = createShapeToolButton(AnnotationTypeEnum::OVAL,
                                                                 m_shapeActionGroup);
    QToolButton* shapeTextToolButton     = createShapeToolButton(AnnotationTypeEnum::TEXT,
                                                                 m_shapeActionGroup);
    QObject::connect(m_shapeActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(spaceOrShapeActionTriggered()));
    
    /*
     * Space buttons
     */
    m_spaceActionGroup = new QActionGroup(this);
    QToolButton* chartSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::CHART,
                                                              m_spaceActionGroup);
    QToolButton* histologySpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::HISTOLOGY,
                                                                  m_spaceActionGroup);
    QToolButton* mediaSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL,
                                                              m_spaceActionGroup);
    QToolButton* tabSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::TAB,
                                                            m_spaceActionGroup);
    const bool showSpacerToolButtonFlag(false);
    QToolButton* spacerSpaceToolButton(NULL);
    if (showSpacerToolButtonFlag) {
        spacerSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::SPACER,
                                                      m_spaceActionGroup);
    }
    QToolButton* stereotaxicSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::STEREOTAXIC,
                                                                    m_spaceActionGroup);
    QToolButton* surfaceSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::SURFACE,
                                                                m_spaceActionGroup);
    QToolButton* windowSpaceToolButton = createSpaceToolButton(AnnotationCoordinateSpaceEnum::WINDOW,
                                                               m_spaceActionGroup);
    QObject::connect(m_spaceActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(spaceOrShapeActionTriggered()));
    
    const bool smallButtonsFlag = false;
    if (smallButtonsFlag) {
        const int mw = 24;
        const int mh = 24;
        
        shapeBoxToolButton->setMaximumSize(mw, mh);
        shapeImageToolButton->setMaximumSize(mw, mh);
        shapeLineToolButton->setMaximumSize(mw, mh);
        shapeOvalToolButton->setMaximumSize(mw, mh);
        shapePolygonToolButton->setMaximumSize(mw, mh);
        shapePolyLineToolButton->setMaximumSize(mw, mh);
        shapeTextToolButton->setMaximumSize(mw, mh);
        
        chartSpaceToolButton->setMaximumSize(mw, mh);
        histologySpaceToolButton->setMaximumSize(mw, mh);
        mediaSpaceToolButton->setMaximumSize(mw, mh);
        if (spacerSpaceToolButton != NULL) {
            spacerSpaceToolButton->setMaximumSize(mw, mh);
        }
        tabSpaceToolButton->setMaximumSize(mw, mh);
        stereotaxicSpaceToolButton->setMaximumSize(mw, mh);
        surfaceSpaceToolButton->setMaximumSize(mw, mh);
        windowSpaceToolButton->setMaximumSize(mw, mh);
    }
    
    QToolButton* fileSelectionToolButton = createFileSelectionToolButton();
    
    QLabel* fileLabel  = new QLabel("File");
    QLabel* spaceLabel = new QLabel("Space");
    QLabel* typeLabel  = new QLabel("Type");
    
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    
    const bool rowsLayoutFlag = true;
    if (rowsLayoutFlag) {
        QVBoxLayout* fileLayout = new QVBoxLayout();
        fileLayout->addWidget(fileLabel, 0, Qt::AlignHCenter);
        fileLayout->addWidget(fileSelectionToolButton, 0, Qt::AlignHCenter);
        fileLayout->addStretch();
        
        QLabel* insertLabel = new QLabel("Insert New");
        
        const int32_t topColumnCount((spacerSpaceToolButton != NULL)
                                     ? 9
                                     : 8);
        gridLayout->addWidget(insertLabel,
                              0, 0, 1, topColumnCount,
                              Qt::AlignHCenter);
        
        gridLayout->addLayout(fileLayout,
                              1, 0, 3, 1,
                              (Qt::AlignTop | Qt::AlignHCenter));
        
        gridLayout->setColumnMinimumWidth(1, 5);
        
        int32_t topColumn(2);
        gridLayout->addWidget(spaceLabel,
                              1, topColumn++, Qt::AlignLeft);
        gridLayout->addWidget(chartSpaceToolButton,
                              1, topColumn++);
        gridLayout->addWidget(histologySpaceToolButton,
                              1, topColumn++);
        gridLayout->addWidget(mediaSpaceToolButton,
                              1, topColumn++);
        if (spacerSpaceToolButton != NULL) {
            gridLayout->addWidget(spacerSpaceToolButton,
                                  1, topColumn++);
        }
        gridLayout->addWidget(stereotaxicSpaceToolButton,
                              1, topColumn++);
        gridLayout->addWidget(surfaceSpaceToolButton,
                              1, topColumn++);
        gridLayout->addWidget(tabSpaceToolButton,
                              1, topColumn++);
        gridLayout->addWidget(windowSpaceToolButton,
                              1, topColumn++);
        
        gridLayout->setRowMinimumHeight(2, 2);
        
        gridLayout->addWidget(typeLabel,
                              3, 2, Qt::AlignLeft);
        gridLayout->addWidget(shapeBoxToolButton,
                              3, 3);
        gridLayout->addWidget(shapeImageToolButton,
                              3, 4);
        gridLayout->addWidget(shapeLineToolButton,
                              3, 5);
        gridLayout->addWidget(shapePolygonToolButton,
                              3, 6);
        gridLayout->addWidget(shapePolyLineToolButton,
                              3, 7);
        gridLayout->addWidget(shapeOvalToolButton,
                              3, 8);
        gridLayout->addWidget(shapeTextToolButton,
                              3, 9);
    }
    else {
        QLabel* insertLabel = new QLabel("Insert New");
        
        gridLayout->addWidget(insertLabel,
                              0, 0, 1, 8, Qt::AlignHCenter);
        
        gridLayout->addWidget(fileLabel,
                              1, 0,
                              Qt::AlignHCenter);
        gridLayout->addWidget(fileSelectionToolButton,
                              2, 0, 2, 1,
                              (Qt::AlignTop | Qt::AlignHCenter));
        
        gridLayout->setColumnMinimumWidth(1, 15);
        gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                              1, 1, 3, 1,
                              Qt::AlignHCenter);
        
        gridLayout->addWidget(spaceLabel,
                              1, 2, Qt::AlignLeft);
        gridLayout->addWidget(chartSpaceToolButton,
                              1, 3);
        gridLayout->addWidget(histologySpaceToolButton,
                              1, 4);
        gridLayout->addWidget(mediaSpaceToolButton,
                              1, 5);
        gridLayout->addWidget(spacerSpaceToolButton,
                              1, 6);
        gridLayout->addWidget(stereotaxicSpaceToolButton,
                              1, 7);
        gridLayout->addWidget(surfaceSpaceToolButton,
                              1, 8);
        gridLayout->addWidget(tabSpaceToolButton,
                              1, 9);
        gridLayout->addWidget(windowSpaceToolButton,
                              1, 10);
        
        QSpacerItem* rowSpaceItem = new QSpacerItem(5, 5,
                                                    QSizePolicy::Fixed,
                                                    QSizePolicy::Fixed);
        gridLayout->addItem(rowSpaceItem,
                            2, 3, 1, 7);
        
        gridLayout->addWidget(typeLabel,
                              3, 2, Qt::AlignLeft);
        gridLayout->addWidget(shapeBoxToolButton,
                              3, 3);
        gridLayout->addWidget(shapeImageToolButton,
                              3, 4);
        gridLayout->addWidget(shapeLineToolButton,
                              3, 5);
        gridLayout->addWidget(shapePolygonToolButton,
                              3, 6);
        gridLayout->addWidget(shapePolyLineToolButton,
                              3, 7);
        gridLayout->addWidget(shapeOvalToolButton,
                              3, 8);
        gridLayout->addWidget(shapeTextToolButton,
                              3, 9);
    }
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
    /*
     * Default Space to Tab
     * Default Shape to Text
     * Do this before creating action groups to avoid
     * triggering signals.
     */
    m_spaceActionGroup->blockSignals(true);
    tabSpaceToolButton->defaultAction()->setChecked(true);
    m_spaceActionGroup->blockSignals(false);
    
    m_shapeActionGroup->blockSignals(true);
    shapeTextToolButton->defaultAction()->setChecked(true);
    m_shapeActionGroup->blockSignals(false);
}

/**
 * Create widgets for edit samples input mode
 */
void
AnnotationInsertNewWidget::createEditSamplesWidgets()
{
    const AString sampleToolTipText("To draw a polyhedron:"
                                  "Click this button to insert a new polyhedron and then click or drag the mouse to draw "
                                  "the first face of the polyhedron.  As points are added, points are added on additional "
                                  "volume slices."
                                  "<ul>"
                                  "<li> <i>Click</i> the mouse to insert coordinates and create straight, possibly longer lines"
                                  "<li> <i>Drag</i> (move with left button down) the mouse to create curved lines "
                                  "<li> Note that one can intermix clicks and drags while drawing"
                                  "<li> When finished, <i>shift-click</i> the mouse to finalize the polygon (does NOT add "
                                  "another coordinate) or click the <i>Finish</i> button"
                                  "</ul>");

    QLabel* fileLabel(new QLabel("File"));
    QToolButton* fileSelectionToolButton = createFileSelectionToolButton();
    
    m_newSampleAction = new QAction();
    m_newSampleAction->setText("Insert New Sample");
    m_newSampleAction->setToolTip(sampleToolTipText);
    QObject::connect(m_newSampleAction, &QAction::triggered,
                     this, &AnnotationInsertNewWidget::newSampleActionTriggered);
    QToolButton* newSampleToolButton(new QToolButton());
    WuQtUtilities::setToolButtonStyleForQt5Mac(newSampleToolButton);
    newSampleToolButton->setDefaultAction(m_newSampleAction);
    
    m_newSampleDepthSpinBox = NULL;
    const bool showDepthFlag(true);
    if (showDepthFlag) {
        m_newSampleDepthSpinBox = new QSpinBox();
        m_newSampleDepthSpinBox->setMinimum(-99);
        m_newSampleDepthSpinBox->setMaximum(99);
        m_newSampleDepthSpinBox->setSingleStep(1);
        m_newSampleDepthSpinBox->setValue(m_previousNewSampleDepthSpinBoxValue);
        QObject::connect(m_newSampleDepthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                         this, &AnnotationInsertNewWidget::newSampleDepthValueChanged);
        m_newSampleDepthSpinBox->setToolTip("Polyhedron spans this number of slices");
    }
    
    QGridLayout* layout(new QGridLayout(this));
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(fileLabel,
                      0, 0, Qt::AlignHCenter);
    layout->addWidget(fileSelectionToolButton,
                      1, 0, Qt::AlignHCenter);
    layout->addWidget(newSampleToolButton,
                      1, 1);
    if (m_newSampleDepthSpinBox != NULL) {
        QLabel* slicesLabel(new QLabel("Slices"));
        layout->addWidget(slicesLabel,
                      0, 2, Qt::AlignHCenter);
        layout->addWidget(m_newSampleDepthSpinBox,
                          1, 2, Qt::AlignHCenter);
    }

    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Update the content.
 */
void
AnnotationInsertNewWidget::updateContent()
{
    switch (m_widgetMode) {
        case WidgetMode::INVALID:
            CaretAssert(0);
            break;
        case WidgetMode::ANNOTATIONS:
            enableDisableSpaceActions();
            enableDisableShapeActions();
            
            itemSelectedFromFileSelectionMenu();
            break;
        case WidgetMode::SAMPLES:
            break;
    }
}

/**
 * Gets called when an item is selected from the file selection menu.
 */
void
AnnotationInsertNewWidget::itemSelectedFromFileSelectionMenu()
{
    m_fileSelectionToolButtonAction->setText(m_fileSelectionMenu->getSelectedNameForToolButton());
}

/**
 * Enable/disable shape action based upon model in window and space
 */
void
AnnotationInsertNewWidget::enableDisableShapeActions()
{
    QAction* spaceAction = m_spaceActionGroup->checkedAction();
    if (spaceAction) {
        CaretAssert(spaceAction);
        const int spaceInt = spaceAction->data().toInt();
        bool spaceValidFlag = false;
        AnnotationCoordinateSpaceEnum::Enum annSpace = AnnotationCoordinateSpaceEnum::fromIntegerCode(spaceInt,
                                                                                                      &spaceValidFlag);
        CaretAssert(spaceValidFlag);
    }
}

/**
 * Enable/disable space action based upon model in window.
 */
void
AnnotationInsertNewWidget::enableDisableSpaceActions()
{
    QSignalBlocker signalBlocker(m_spaceActionGroup);
    
    /*
     * Window will be invalid when this widget is being created as part of window being created
     */
    BrainBrowserWindow* window = GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex);
    if (window == NULL) {
        return;
    }
    
    std::vector<BrowserTabContent*> allTabContent;
    if (window->isTileTabsSelected()) {
        window->getAllTabContent(allTabContent);
    }
    else {
        BrowserTabContent* tabContent = window->getBrowserTabContent();
        if (tabContent != NULL) {
            allTabContent.push_back(tabContent);
        }
    }
    
    const bool spacerSpaceValidFlag = window->isTileTabsSelected();
    const bool tabSpaceValidFlag = ( ! allTabContent.empty());
    const bool windowSpaceValidFlag = ( ! allTabContent.empty());
    
    bool chartSpaceValidFlag       = false;
    bool histologySpaceValidFlag   = false;
    bool mediaSpaceValidFlag       = false;
    bool surfaceSpaceValidFlag     = false;
    bool stereotaxicSpaceValidFlag = false;
    
    for (auto tabContent : allTabContent) {
        Model* model = tabContent->getModelForDisplay();
        if (model == NULL) {
            return;
        }
        const ModelTypeEnum::Enum modelType = model->getModelType();
        switch (modelType) {
            case ModelTypeEnum::MODEL_TYPE_CHART:
                break;
            case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
                chartSpaceValidFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_INVALID:
                break;
            case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
                histologySpaceValidFlag = true;
                stereotaxicSpaceValidFlag = true;
                break;
            case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
                mediaSpaceValidFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE:
                stereotaxicSpaceValidFlag = true;
                surfaceSpaceValidFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
                stereotaxicSpaceValidFlag = true;
                surfaceSpaceValidFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
                stereotaxicSpaceValidFlag = true;
                break;
            case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
                stereotaxicSpaceValidFlag = true;
                surfaceSpaceValidFlag = true;
                break;
        }
    }
    
    QAction* selectedAction = m_spaceActionGroup->checkedAction();
    QAction* tabSpaceAction = NULL;
    QListIterator<QAction*> actionsIterator(m_spaceActionGroup->actions());
    while (actionsIterator.hasNext()) {
        QAction* action = actionsIterator.next();
        const int spaceInt = action->data().toInt();
        bool spaceValidFlag = false;
        AnnotationCoordinateSpaceEnum::Enum annSpace = AnnotationCoordinateSpaceEnum::fromIntegerCode(spaceInt,
                                                                                                      &spaceValidFlag);
        CaretAssert(spaceValidFlag);
        
        bool enableSpaceFlag = false;
        switch (annSpace) {
            case AnnotationCoordinateSpaceEnum::CHART:
                enableSpaceFlag = chartSpaceValidFlag;
                break;
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                enableSpaceFlag = histologySpaceValidFlag;
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                enableSpaceFlag = mediaSpaceValidFlag;
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                enableSpaceFlag = spacerSpaceValidFlag;
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                enableSpaceFlag = stereotaxicSpaceValidFlag;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                enableSpaceFlag = surfaceSpaceValidFlag;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                tabSpaceAction = action;
                enableSpaceFlag = tabSpaceValidFlag;
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                enableSpaceFlag = windowSpaceValidFlag;
                break;
        }
        
        action->setEnabled(enableSpaceFlag);
        
        if (action == selectedAction) {
            if ( ! action->isEnabled()) {
                selectedAction = NULL;
            }
        }
    }
    
    if (selectedAction == NULL) {
        CaretAssert(tabSpaceAction);
        tabSpaceAction->setChecked(true);
    }
}


/**
 * @return Create a file selection/menu toolbutton.
 */
QToolButton*
AnnotationInsertNewWidget::createFileSelectionToolButton()
{
    m_fileSelectionMenu = new AnnotationMenuFileSelection(m_userInputMode);
    QObject::connect(m_fileSelectionMenu, SIGNAL(menuItemSelected()),
                     this, SLOT(itemSelectedFromFileSelectionMenu()));
    
    m_fileSelectionToolButtonAction = new QAction(m_fileSelectionMenu->getSelectedNameForToolButton(),
                                                  this);
    m_fileSelectionToolButtonAction->setToolTip("Choose file for new annotation");
    m_fileSelectionToolButtonAction->setMenu(m_fileSelectionMenu);
    
    QToolButton* fileSelectionToolButton = new QToolButton();
    fileSelectionToolButton->setDefaultAction(m_fileSelectionToolButtonAction);
    fileSelectionToolButton->setFixedWidth(fileSelectionToolButton->sizeHint().width());
    WuQtUtilities::setToolButtonStyleForQt5Mac(fileSelectionToolButton);
    
    return fileSelectionToolButton;
}

/**
 * @return Create the shape tool button for the given annotation type.
 * 
 * @param annotationType
 *     The annotation type.
 * @param actionGroup
 *     Action group to which button is added to make button exclusive.
 */
QToolButton*
AnnotationInsertNewWidget::createShapeToolButton(const AnnotationTypeEnum::Enum annotationType,
                                                 QActionGroup* actionGroup)
{
    const QString typeGuiName = AnnotationTypeEnum::toGuiName(annotationType);
    QToolButton* toolButton = new QToolButton();
    
    QAction* action = new QAction(createShapePixmap(toolButton,
                                                    annotationType),
                                  typeGuiName,
                                  this);
    
    action->setData(AnnotationTypeEnum::toIntegerCode(annotationType));

    const AString polyDrawingText("To draw a polygon:"
                                  "<ul>"
                                  "<li> <i>Click</i> the mouse to insert coordinates and create straight, possibly longer lines"
                                  "<li> <i>Drag</i> (move with left button down) the mouse to create curved lines "
                                  "<li> Note that one can intermix clicks and drags while drawing"
                                  "<li> When finished, <i>shift-click</i> the mouse to finalize the polygon (does NOT add "
                                  "another coordinate)"
                                  "</ul>");
    AString typeText;
    AString clickText;
    AString dragText;
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            typeText = "Box Annotation";
            clickText = ("Click the mouse to create a box with a default size.  "
                         "Change the box by moving its corners or edges.");
            dragText = ("Press and hold the left mouse button down at a corner of the box.  "
                        "While continuing to hold the mouse button down, drag the mouse to "
                        "another corner of the box and release the mouse button to create the box.");
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::IMAGE:
            typeText = "Image Annotation";
            clickText = ("Click the mouse and a dialog is displayed.  In the dialog, click the "
                         "Choose Image File button and select an image file.  Click the OK button "
                         "to finish creation of the image annotation.  Adjust the size of the image "
                         "by moving the edges or corners.");
            dragText = ("Press and hold the left mouse button down at a corner for the image.  "
                        "While continuing to hold the mouse button down, drag the mouse to "
                        "another corner for the image and release the mouse button.  In the "
                        "dialog that is displayed, click the "
                        "Choose Image File button and select an image file.  Click the OK button "
                        "to finish creation of the image annotation.");
            break;
        case AnnotationTypeEnum::LINE:
            typeText = "Line Segment Annotation";
            clickText = ("Click the mouse to create a line with a default size.  "
                         "Change the line by moving its end points.");
            dragText = ("Press and hold the left mouse button down at one end of the line.  "
                        "While continuing to hold the mouse button down, drag the mouse to "
                        "the other end of the line and release the mouse button to create the line.");

            break;
        case AnnotationTypeEnum::OVAL:
            typeText = "Oval Annotation";
            clickText = ("Click the mouse to create an oval with a default size.  "
                         "Change the oval by moving the bounds of the oval.");
            dragText = ("Press and hold the left mouse button down at a side of the oval.  "
                        "While continuing to hold the mouse button down, drag the mouse to "
                        "another side of the oval and release the mouse button to create the oval.");
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            typeText = ("Polyhedron Annotation.<p>"
                        "A polyhedron consists of two polygons that are connected at corresponding "
                        "vertices.  User draws first polygon and the second polygon is created to "
                        "form the polyhedron.  When drawing the first polygon, "
                        "last coordinate automatically connects to first coordinate. "
                        "<p>"
                        "To draw the first polygon:"
                        "<ul>"
                        "<li> <i>Click</i> the mouse to insert coordinates and create straight, possibly longer lines"
                        "<li> <i>Drag</i> (move with left button down) the mouse to create curved lines "
                        "<li> Note that one can intermix clicks and drags while drawing"
                        "<li> When finished, <i>shift-click</i> the mouse to finalize the polygon (does NOT add "
                        "another coordinate)"
                        "</ul>");
            break;
        case AnnotationTypeEnum::POLYGON:
            typeText = ("Polygon Annotation.<p>"
                        "A polygon is a series of connected line segments that form a closed shape "
                        "(last coordinate automatically connects to first coordinate). "
                        "<p>"
                        + polyDrawingText);
            break;
        case AnnotationTypeEnum::POLYLINE:
            {
                AString polyDrawTextCopy(polyDrawingText);
                polyDrawTextCopy.replace("olygon", "olyline");
                typeText = ("Polyline Annotation.<p>"
                            "A polyline is a series of connected line segments"
                            "(last coordinate DOES NOT connect to first coordinate). "
                            "<p>"
                            + polyDrawTextCopy);
            }
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::TEXT:
        {
            typeText = "Text Annotation";
            clickText = ("Click the mouse at the location for the text.  After the mouse is clicked, "
                         "a dialog is displayed for entry of the text.  Enter text in the dialog "
                         "and click the OK button to finish.  The resulting text is offset from the mouse-click location using the "
                         "current Text Alignment and Orientation selections in the toolbar.");
            dragText = ("Press and hold the left mouse button down at a box corner for the text.  "
                        "While continuing to hold the mouse button down, drag the mouse to "
                        "another corner of the box and release the mouse button.  In the dialog "
                        "that appears, enter the text and click the OK button to finish.  "
                        "The Text Alignment and Orientation options affect the location of the "
                        "text within the box formed by the mouse actions.");
        }
            break;
    }
    
    AString toolTip("<html>");
    if ( ! typeText.isEmpty()) {
        toolTip.appendWithNewLine(typeText);
    }
    if ( ( ! clickText.isEmpty())
        && ( ! dragText.isEmpty())) {
        if ( ! clickText.isEmpty()) {
            toolTip.appendWithNewLine("<p>");
        }
        toolTip.appendWithNewLine("There are two methods for creating this type of annotation:<p> ");
        clickText.insert(0, "(1) ");
        dragText.insert(0, "(2) ");
    }
    if ( ! clickText.isEmpty()) {
        if ( ! clickText.isEmpty()) {
            toolTip.appendWithNewLine("<p>");
        }
        toolTip.appendWithNewLine(clickText);
    }
    if ( ! dragText.isEmpty()) {
        if ( ! clickText.isEmpty()) {
            toolTip.appendWithNewLine("<p>");
        }
        toolTip.appendWithNewLine(dragText);
    }
    toolTip.appendWithNewLine("</html>");
    
    action->setToolTip(toolTip);

    action->setCheckable(true);
    action->setChecked(false);
    
    toolButton->setDefaultAction(action);
    
    /*
     * Must set style AFTER button is added to action group
     * so that checked property is enabled for button
     */
    actionGroup->addAction(action);
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolButton);
    
    return toolButton;
}

/**
 * Called when new sample action triggered
 */
void
AnnotationInsertNewWidget::newSampleActionTriggered()
{
    AnnotationFile* annotationFile = m_fileSelectionMenu->getSelectedAnnotationFile();
    if (annotationFile == NULL) {
        WuQMessageBox::errorOk(this, "No file is selected.  Click the File Select button to create a new samples file.");
        return;
    }
    
    const AnnotationCoordinateSpaceEnum::Enum annSpace(AnnotationCoordinateSpaceEnum::STEREOTAXIC);
    const AnnotationTypeEnum::Enum annShape(AnnotationTypeEnum::POLYHEDRON);
    EventAnnotationCreateNewType::PolyhedronDrawingMode polyhedronDrawingMode
    = EventAnnotationCreateNewType::PolyhedronDrawingMode::SAMPLES_DRAWING;

    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    dpa->setDisplayAnnotations(true);
    EventManager::get()->sendEvent(EventAnnotationCreateNewType(m_browserWindowIndex,
                                                                m_userInputMode,
                                                                annotationFile,
                                                                annSpace,
                                                                annShape,
                                                                polyhedronDrawingMode).getPointer());

}

/**
 * Called when the depth value for the new sample is  changed
 * @param value
 *    The new value
 */
void
AnnotationInsertNewWidget::newSampleDepthValueChanged(int value)
{
    /*
     * Do not allow a zero value
     * If user transitions to zero from positive one, set to negative one
     * If user transitions to zero from negative one, set to positive one
     */
    if (value == 0) {
        if (m_previousNewSampleDepthSpinBoxValue > 0) {
            value = -1;
        }
        else if (m_previousNewSampleDepthSpinBoxValue < 0) {
            value = 1;
        }
        else {
            value = 1;
        }
        QSignalBlocker blocker(m_newSampleDepthSpinBox);
        m_newSampleDepthSpinBox->setValue(value);
    }
    
    m_previousNewSampleDepthSpinBoxValue = value;

    /*
     * Depth value is requested while the annotation is being drawn by graphics
     * so request a graphics update
     */
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
}

/**
 * Called when a space or shape action triggered.
 */
void
AnnotationInsertNewWidget::spaceOrShapeActionTriggered()
{
    const QAction* spaceAction = m_spaceActionGroup->checkedAction();
    if (spaceAction == NULL) {
        WuQMessageBox::errorOk(this, "No space is selected.  Select a space.");
        return;
    }
    
    const QAction* shapeAction = m_shapeActionGroup->checkedAction();
    if (shapeAction == NULL) {
        WuQMessageBox::errorOk(this, "No shape is selected.  Select a shape.");
        return;
    }
    
    AnnotationFile* annotationFile = m_fileSelectionMenu->getSelectedAnnotationFile();
    if (annotationFile == NULL) {
        WuQMessageBox::errorOk(this, "No file is selected.  Create a new file or select a file.");
        return;
    }
    
    CaretAssert(spaceAction);
    const int spaceInt = spaceAction->data().toInt();
    bool spaceValidFlag = false;
    AnnotationCoordinateSpaceEnum::Enum annSpace = AnnotationCoordinateSpaceEnum::fromIntegerCode(spaceInt,
                                                                                               &spaceValidFlag);
    CaretAssert(spaceValidFlag);

    CaretAssert(shapeAction);
    const int shapeInt = shapeAction->data().toInt();
    bool shapeValidFlag = false;
    AnnotationTypeEnum::Enum annShape = AnnotationTypeEnum::fromIntegerCode(shapeInt,
                                                                            &shapeValidFlag);
    CaretAssert(shapeValidFlag);
    
    EventAnnotationCreateNewType::PolyhedronDrawingMode polyhedronDrawingMode
    = EventAnnotationCreateNewType::PolyhedronDrawingMode::ANNOTATION_DRAWING;
    
    switch (annShape) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            switch (m_userInputMode) {
                case UserInputModeEnum::Enum::INVALID:
                    CaretAssert(0);
                    break;
                case UserInputModeEnum::Enum::ANNOTATIONS:
                    polyhedronDrawingMode = EventAnnotationCreateNewType::PolyhedronDrawingMode::ANNOTATION_DRAWING;
                    break;
                case UserInputModeEnum::Enum::BORDERS:
                    CaretAssert(0);
                    break;
                case UserInputModeEnum::Enum::FOCI:
                    CaretAssert(0);
                    break;
                case UserInputModeEnum::Enum::IMAGE:
                    CaretAssert(0);
                    break;
                case UserInputModeEnum::Enum::SAMPLES_EDITING:
                    polyhedronDrawingMode = EventAnnotationCreateNewType::PolyhedronDrawingMode::SAMPLES_DRAWING;
                    break;
                case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
                    CaretAssert(0);
                    break;
                case UserInputModeEnum::Enum::VIEW:
                    CaretAssert(0);
                    break;
                case UserInputModeEnum::Enum::VOLUME_EDIT:
                    CaretAssert(0);
                    break;
            }
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            break;
        case AnnotationTypeEnum::TEXT:
            break;
    }
    
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    dpa->setDisplayAnnotations(true);
    EventManager::get()->sendEvent(EventAnnotationCreateNewType(m_browserWindowIndex,
                                                                m_userInputMode,
                                                                annotationFile,
                                                                annSpace,
                                                                annShape,
                                                                polyhedronDrawingMode).getPointer());
}

/**
 * Create a pixmap for the given annotation shape type.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param annotationType
 *    The annotation type.
 * @return
 *    Pixmap with icon for the given annotation type.
 */
QPixmap
AnnotationInsertNewWidget::createShapePixmap(const QWidget* widget,
                                             const AnnotationTypeEnum::Enum annotationType)
{
    CaretAssert(widget);

    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    
    /**
     * NOTE: ORIGIN is in TOP LEFT corner of pixmap.
     */
    switch (annotationType) {
        case AnnotationTypeEnum::BOX:
            painter->drawRect(1, 1, width - 2, height - 2);
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            CaretAssertMessage(0, "No pixmap for browser tab as user does not create them like other annotations");
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            CaretAssertMessage(0, "No pixmap for colorbar as user does not create them like other annotations");
            break;
        case AnnotationTypeEnum::IMAGE:
        {
            const int blueAsGray = qGray(25,25,255);
            QColor skyColor(blueAsGray, blueAsGray, blueAsGray);
            
            /*
             * Background (sky)
             */
            painter->fillRect(pixmap.rect(), skyColor);
            
            const int greenAsGray = qGray(0, 255, 0);
            QColor terrainColor(greenAsGray, greenAsGray, greenAsGray);
            
            /*
             * Terrain
             */
            painter->setBrush(terrainColor);
            painter->setPen(terrainColor);
            const int w14 = width * 0.25;
            const int h23 = height * 0.667;
            const int h34 = height * 0.75;
            QPolygon terrain;
            terrain.push_back(QPoint(1, height - 1));
            terrain.push_back(QPoint(width - 1, height - 1));
            terrain.push_back(QPoint(width - 1, h23));
            terrain.push_back(QPoint(w14 * 3, h34));
            terrain.push_back(QPoint(w14 * 2, h23));
            terrain.push_back(QPoint(w14, h34));
            terrain.push_back(QPoint(1, h23));
            terrain.push_back(QPoint(1, height - 1));
            painter->drawPolygon(terrain);
            
            const int yellowAsGray = qGray(255, 255, 0);
            QColor sunColor(yellowAsGray, yellowAsGray, yellowAsGray);
            
            /*
             * Sun
             */
            painter->setBrush(sunColor);
            painter->setPen(sunColor);
            const int radius = width * 0.25;
            painter->drawEllipse(width * 0.33, height * 0.33, radius, radius);
        }
            break;
        case AnnotationTypeEnum::LINE:
            painter->drawLine(1, height - 1, width - 1, 1);
            break;
        case AnnotationTypeEnum::OVAL:
            painter->drawEllipse(1, 1, width - 1, height - 1);
            break;
        case AnnotationTypeEnum::POLYHEDRON:
        {
            CaretAssert(width  == 24);
            CaretAssert(height == 24);
            {
                QPolygon polygon;
                polygon.push_back(QPoint(12, 2));
                polygon.push_back(QPoint(6, 8));
                polygon.push_back(QPoint(12, 12));
                polygon.push_back(QPoint(18, 8));
                painter->drawPolygon(polygon);
            }
            {
                QPolygon polygon;
                polygon.push_back(QPoint(6, 8));
                polygon.push_back(QPoint(2, 18));
                polygon.push_back(QPoint(10, 20));
                polygon.push_back(QPoint(12, 12));
                painter->drawPolygon(polygon);
            }
            {
                QPolygon polygon;
                polygon.push_back(QPoint(12, 12));
                polygon.push_back(QPoint(10, 20));
                polygon.push_back(QPoint(16, 20));
                polygon.push_back(QPoint(18, 8));
                painter->drawPolygon(polygon);
            }
        }
            break;
        case AnnotationTypeEnum::POLYGON:
        {
            const int hh(height / 2);
            const int hw(width / 2);
            QPolygon polygon;
            polygon.push_back(QPoint(2, 2));
            polygon.push_back(QPoint(width - 2, 5));
            polygon.push_back(QPoint(width - 5, height - 2));
            polygon.push_back(QPoint(hh, hw));
            polygon.push_back(QPoint(2, height - 4));
            painter->drawPolygon(polygon);
        }
            break;
        case AnnotationTypeEnum::POLYLINE:
        {
            const int hh(height / 2);
            const int hw(width / 2);
            QPolygon polyLine;
            polyLine.push_back(QPoint(2, hh));
            polyLine.push_back(QPoint(6, 3));
            polyLine.push_back(QPoint(hw + 5, height - 3));
            polyLine.push_back(QPoint(width - 2, hh - 3));
            painter->drawPolyline(polyLine);
        }
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            CaretAssertMessage(0, "No pixmap for scale bar as user does not create them like other annotations");
            break;
        case AnnotationTypeEnum::TEXT:
        {
            QFont font = painter->font();
            font.setPixelSize(20);
            painter->setFont(font);
            painter->drawText(pixmap.rect(),
                             (Qt::AlignCenter),
                             "A");
        }
            break;
    }
    
    return pixmap;
}

/**
 * @return Create the space tool button for the given annotation space.
 *
 * @param annotationSpace
 *     The annotation space
 * @param actionGroup
 *     Action group to which button is added to make button exclusive.
 */
QToolButton*
AnnotationInsertNewWidget::createSpaceToolButton(const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                                 QActionGroup* actionGroup)
{
    switch (annotationSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssertMessage(0, "Annotations in viewport space not supported.");
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }
    
    const bool useIconFlag = true;
    QToolButton* toolButton = new QToolButton();
    QAction* action = NULL;
    if (useIconFlag) {
        action = new QAction(createSpacePixmap(toolButton,
                                               annotationSpace),
                             AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(annotationSpace),
                             this);
    }
    else {
        action = new QAction(AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(annotationSpace),
                             this);
    }

    action->setData((int)AnnotationCoordinateSpaceEnum::toIntegerCode(annotationSpace));
    action->setToolTip(AnnotationCoordinateSpaceEnum::toToolTip(annotationSpace));
    action->setCheckable(true);
    action->setChecked(false);
    
    toolButton->setDefaultAction(action);
    
    /*
     * Must set style AFTER button is added to action group
     * so that checked property is enabled for button
     */
    actionGroup->addAction(action);
    WuQtUtilities::setToolButtonStyleForQt5Mac(toolButton);
    
    return toolButton;
}

/**
 * Create a pixmap for the given annotation coordinate space
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @param annotationSpace
 *    The annotation coordinate space.
 * @return
 *    Pixmap with icon for the given annotation coordinate space.
 */
QPixmap
AnnotationInsertNewWidget::createSpacePixmap(const QWidget* widget,
                                             const AnnotationCoordinateSpaceEnum::Enum annotationSpace)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    const float width  = 24.0;
    const float height = 24.0;
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    
    /**
     * NOTE: ORIGIN is in TOP LEFT corner of pixmap.
     */
    switch (annotationSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssertMessage(0, "Annotations in viewport space not supported.");
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }
    
    const QString letter = AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(annotationSpace);
    QFont font = painter->font();
    font.setPixelSize(20);
    painter->setFont(font);
    painter->drawText(pixmap.rect(),
                      (Qt::AlignCenter),
                      letter);
    
    return pixmap;
}

