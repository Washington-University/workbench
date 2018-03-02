
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <algorithm>
#include <memory>

#include <QAction>
#include <QActionGroup>
#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QStackedWidget>
#include <QToolButton>


#define __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__
#include "UserInputModeBordersWidget.h"
#undef __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__

#include "AlgorithmException.h"
#include "AlgorithmNodesInsideBorder.h"
#include "Border.h"
#include "BorderEditingSelectionDialog.h"
#include "BorderFile.h"
#include "BorderOptimizeDialog.h"
#include "BorderPointFromSearch.h"
#include "BorderPropertiesEditorDialog.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayPropertiesBorders.h"
#include "EventBrainReset.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelWholeBrain.h"
#include "RegionOfInterestCreateFromBorderDialog.h"
#include "Surface.h"
#include "UserInputModeBorders.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"


using namespace caret;



/**
 * \class caret::UserInputModeBordersWidget 
 * \brief Controls for when the user input mode is border operations
 */

/**
 * Constructor.
 *
 * @param inputModeBorders
 *    Mouse processing for border operations.
 * @param parent
 *    Optional parent widget.
 */
UserInputModeBordersWidget::UserInputModeBordersWidget(UserInputModeBorders* inputModeBorders,
                                                       QWidget* parent)
: QWidget(parent)
{
    m_transformToolTipText = ("\n\n"
                              "At any time, the view of the surface may be changed by\n"
                              "  PAN:    Move the mouse with the left mouse button down while "
                              "holding down the Shift key.\n"
                              "  ROTATE: Move the mouse with the left mouse button down.\n"
                              "  ZOOM:   Move the mouse with the left mouse button down while "
                              "holding down the Ctrl key (Apple key on Macs)."
                              );
    
    m_borderOptimizeDialog = NULL;
    
    this->inputModeBorders = inputModeBorders;
    
    QLabel* nameLabel = new QLabel("Borders ");
    
    this->widgetMode = this->createModeWidget();
    
    resetLastEditedBorder();
    
    this->widgetDrawOperation = this->createDrawOperationWidget();
    
    this->widgetEditOperation = this->createEditOperationWidget();
    
    this->widgetRoiOperation = this->createRoiOperationWidget();
    
    this->operationStackedWidget = new QStackedWidget();
    this->operationStackedWidget->addWidget(this->widgetDrawOperation);
    this->operationStackedWidget->addWidget(this->widgetEditOperation);
    this->operationStackedWidget->addWidget(this->widgetRoiOperation);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(nameLabel);
    layout->addWidget(this->widgetMode);
    layout->addSpacing(10);
    layout->addWidget(this->operationStackedWidget);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BRAIN_RESET);
}

/**
 * Destructor.
 */
UserInputModeBordersWidget::~UserInputModeBordersWidget()
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
UserInputModeBordersWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_RESET) {
        EventBrainReset* brainEvent = dynamic_cast<EventBrainReset*>(event);
        CaretAssert(brainEvent);
        
        if (m_borderOptimizeDialog != NULL) {
            delete m_borderOptimizeDialog;
            m_borderOptimizeDialog = NULL;
        }
        
        brainEvent->setEventProcessed();
    }
}

/**
 * Update the contents of the widget.
 */
void
UserInputModeBordersWidget::updateWidget()
{
    /*
     * Show the proper widget
     */
    switch (this->inputModeBorders->getMode()) {
        case UserInputModeBorders::MODE_DRAW:
            this->operationStackedWidget->setCurrentWidget(this->widgetDrawOperation);
            this->setActionGroupByActionData(this->drawOperationActionGroup,
                                             inputModeBorders->getDrawOperation());
            resetLastEditedBorder();
            break;
        case UserInputModeBorders::MODE_EDIT:
            this->operationStackedWidget->setCurrentWidget(this->widgetEditOperation);
            this->setActionGroupByActionData(this->editOperationActionGroup, 
                                             inputModeBorders->getEditOperation());
            break;
        case UserInputModeBorders::MODE_ROI:
            this->operationStackedWidget->setCurrentWidget(this->widgetRoiOperation);
            resetLastEditedBorder();
            break;
    }
    const int selectedModeInteger = (int)this->inputModeBorders->getMode();

    const int modeComboBoxIndex = this->modeComboBox->findData(selectedModeInteger);
    CaretAssert(modeComboBoxIndex >= 0);
    this->modeComboBox->blockSignals(true);
    this->modeComboBox->setCurrentIndex(modeComboBoxIndex);
    this->modeComboBox->blockSignals(false);
}

/**
 * Set the action with its data value of the given integer
 * as the active action.
 * @param actionGroup
 *   Action group for which action is selected.
 * @param dataInteger
 *   Integer value for data attribute.
 */
void 
UserInputModeBordersWidget::setActionGroupByActionData(QActionGroup* actionGroup, 
                                                       const int dataInteger)
{
    actionGroup->blockSignals(true);
    const QList<QAction*> actionList = actionGroup->actions();
    QListIterator<QAction*> iter(actionList);
    while (iter.hasNext()) {
        QAction* action = iter.next();
        const int actionDataInteger = action->data().toInt();
        if (dataInteger == actionDataInteger) {
            action->setChecked(true);
            break;
        }
    }
    actionGroup->blockSignals(false);
}

/**
 * @return The mode widget.
 */
QWidget* 
UserInputModeBordersWidget::createModeWidget()
{
    this->modeComboBox = new QComboBox();
    this->modeComboBox->addItem("Draw", (int)UserInputModeBorders::MODE_DRAW);
    this->modeComboBox->addItem("Edit", (int)UserInputModeBorders::MODE_EDIT);
    this->modeComboBox->addItem("ROI", (int)UserInputModeBorders::MODE_ROI);
    QObject::connect(this->modeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(modeComboBoxSelection(int)));
        
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(this->modeComboBox);
    
    widget->setFixedWidth(widget->sizeHint().width());
    
    return widget;
}

/**
 * Called when a mode is selected from the mode combo box.
 * @param indx
 *   Index of item selected.
 */
void 
UserInputModeBordersWidget::modeComboBoxSelection(int indx)
{
    const int modeInteger = this->modeComboBox->itemData(indx).toInt();
    const UserInputModeBorders::Mode mode = (UserInputModeBorders::Mode)modeInteger;
    this->inputModeBorders->setMode(mode);
    resetLastEditedBorder();
}

/**
 * @return The draw operation widget.
 */
QWidget* 
UserInputModeBordersWidget::createDrawOperationWidget()
{
    /*
     * Draw
     */
    const AString drawToolTipText = ("To draw a new border segment either click the mouse "
                                 "to discretely add border points or hold down the CTRL (Apple key on Mac) and SHIFT keys "
                                 "and move the mouse with the left mouse button down to continuously "
                                     "add points. "
                                 "When the border is complete, either press the Finish button "
                                 "or hold down the Shift key and click the mouse to display a "
                                 "dialog for assigning the borders attributes (name and color). "
                                     + m_transformToolTipText);
    QAction* drawAction = WuQtUtilities::createAction("New",
                                                      WuQtUtilities::createWordWrappedToolTipText(drawToolTipText),
                                                      this);
    drawAction->setCheckable(true);
    drawAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_CREATE));
    QToolButton* drawToolButton = new QToolButton();
    drawToolButton->setDefaultAction(drawAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(drawToolButton);
    
    /*
     * Erase
     */
    const AString eraseToolTipText = ("To erase a section of a border, click the mouse twice, once "
                                      "at the beginning of the section that is to be removed and a "
                                      "second time at the end of the section. "
                                      "Press the Finish button or hold down the Shift key and click the "
                                      "mouse remove the border section."
                                      + m_transformToolTipText);
    QAction* eraseAction = WuQtUtilities::createAction("Erase",
                                                       WuQtUtilities::createWordWrappedToolTipText(eraseToolTipText),
                                                       this);
    eraseAction->setCheckable(true);
    eraseAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_ERASE));
    QToolButton* eraseToolButton = new QToolButton();
    eraseToolButton->setDefaultAction(eraseAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(eraseToolButton);
    
    /*
     * Extend
     */
    const AString extendToolTipText = ("To extend a border, move the mouse ANY point in the border and "
                                       "either click the mouse to discretely add points or hold down the CTRL (Apple key on Mac) and SHIFT keys "
                                       "and move the mouse with the left mouse button down to continuously add points. "
                                       "Press the Finish button or hold down the Shift key and click the "
                                       "mouse add the extension to the border."
                                       "\n\n"
                                       "If the segment starts at a point within the border (not an end point), points will be removed "
                                       "from that point to the nearest end point in the border and then the extension "
                                       "will be added."
                                       + m_transformToolTipText);
    QAction* extendAction = WuQtUtilities::createAction("Extend", 
                                                        WuQtUtilities::createWordWrappedToolTipText(extendToolTipText),
                                                        this);
    extendAction->setCheckable(true);
    extendAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_EXTEND));
    QToolButton* extendToolButton = new QToolButton();
    extendToolButton->setDefaultAction(extendAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(extendToolButton);
    
    /*
     * Replace
     */
    const AString replaceToolTipText = ("To replace a section of a border, move the mouse to the "
                                        "start of the section that is being replaced.  Either click "
                                        "the mouse to discretely add points in the new section or hold down the CTRL (Apple key on Mac) and SHIFT keys "
                                        "and move the mouse with the left mouse button down to continuously add points. "
                                        "Press the Finish button or hold down the Shift key and click the "
                                        "mouse to conclude replacing the section in the border."
                                        "\n\n"
                                        "Both the first point and the last point in the segment must "
                                        "overlap points in the border."
                                        + m_transformToolTipText);
    QAction* replaceAction = WuQtUtilities::createAction("Replace",
                                                         WuQtUtilities::createWordWrappedToolTipText(replaceToolTipText),
                                                         this);
    replaceAction->setCheckable(true);
    replaceAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_REPLACE));
    QToolButton* replaceToolButton = new QToolButton();
    replaceToolButton->setDefaultAction(replaceAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(replaceToolButton);
    
    /*
     * Optimize
     */
    const AString optimizeToolTipText("A new border optimization process automatically repositions a manually drawn "
                                      "border segment to follow the most probable path based on spatial gradients of "
                                      "a set of user-selected feature maps (useful for cortical parcellation).");
    QAction* optimizeAction = WuQtUtilities::createAction("Optimize",
                                                          WuQtUtilities::createWordWrappedToolTipText(optimizeToolTipText),
                                                          this);
    optimizeAction->setCheckable(true);
    optimizeAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_OPTIMIZE));
    QToolButton* optimizeToolButton = new QToolButton();
    optimizeToolButton->setDefaultAction(optimizeAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(optimizeToolButton);
    
    /*
     * Finish
     */
    const AString finishToolTipText = ("The finish button must be pressed (holding down the Shift key "
                                       "and clicking the mouse is a shortcut to clicking the Finish "
                                       "button) to complete any of the border drawing operations.");
    QAction* finishAction = WuQtUtilities::createAction("Finish", 
                                                       WuQtUtilities::createWordWrappedToolTipText(finishToolTipText),
                                                        this,
                                                        this,
                                                        SLOT(drawFinishButtonClicked()));
    QToolButton* finishToolButton = new QToolButton();
    finishToolButton->setDefaultAction(finishAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(finishToolButton);
    
    /*
     * Undo
     */
    QAction* undoAction = WuQtUtilities::createAction("Undo", 
                                                      "Remove (undo) the last point in the\n"
                                                      "drawn border segment.  If the button\n"
                                                      "is held down, it will repeat removal\n"
                                                      "of points until the button is released.",
                                                      this,
                                                      this,
                                                      SLOT(drawUndoButtonClicked()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(undoAction);
    undoToolButton->setAutoRepeat(true);
    undoToolButton->setAutoRepeatDelay(500);  // 500ms = 1/2 second
    undoToolButton->setAutoRepeatInterval(100);  // 100ms = 1/10 second
    WuQtUtilities::setToolButtonStyleForQt5Mac(undoToolButton);
    
    
    QAction* undoFinishAction = WuQtUtilities::createAction("Undo Finish",
                                                      "Undo the last Erase/Extend/Replace\n"
                                                      "performed on a border.",
                                                      this,
                                                      this,
                                                      SLOT(drawUndoLastEditButtonClicked()));
    m_undoFinishToolButton = new QToolButton();
    m_undoFinishToolButton->setDefaultAction(undoFinishAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_undoFinishToolButton);
    
    /*
     * Reset
     */
    QAction* resetAction = WuQtUtilities::createAction("Reset",
                                                       "Remove all points in the unfinished border", 
                                                       this,
                                                       this,
                                                       SLOT(drawResetButtonClicked()));
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(resetAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(resetToolButton);
    
    this->drawOperationActionGroup = new QActionGroup(this);
    this->drawOperationActionGroup->addAction(drawAction);
    this->drawOperationActionGroup->addAction(eraseAction);
    this->drawOperationActionGroup->addAction(extendAction);
    this->drawOperationActionGroup->addAction(optimizeAction);
    this->drawOperationActionGroup->addAction(replaceAction);
    this->drawOperationActionGroup->setExclusive(true);
    QObject::connect(this->drawOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(drawOperationActionTriggered(QAction*)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(drawToolButton);
    layout->addWidget(eraseToolButton);
    layout->addWidget(extendToolButton);
    layout->addWidget(optimizeToolButton);
    layout->addWidget(replaceToolButton);
    layout->addSpacing(10);
    layout->addWidget(finishToolButton);
    layout->addWidget(m_undoFinishToolButton);
    layout->addSpacing(10);
    layout->addWidget(undoToolButton);
    layout->addWidget(resetToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called when draw border reset button clicked.
 */
void 
UserInputModeBordersWidget::drawResetButtonClicked()
{
    this->inputModeBorders->drawOperationReset();
}

/**
 * Called when draw border undo button clicked.
 */
void 
UserInputModeBordersWidget::drawUndoButtonClicked()
{
    this->inputModeBorders->drawOperationUndo();
}

/**
 * Undo editing (erase/extend/replace) of last border.
 */
void
UserInputModeBordersWidget::drawUndoLastEditButtonClicked()
{
    for (std::vector<BorderFileAndBorderMemento>::iterator iter = m_undoFinishBorders.begin();
         iter != m_undoFinishBorders.end();
         iter++) {
        BorderFile* undoBorderFile = iter->m_borderFile;
        Border*     undoBorder     = iter->m_border;
        
        bool foundBorderFlag = false;
        Brain* brain = GuiManager::get()->getBrain();
        const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
        for (int32_t i = 0; i < numBorderFiles; i++) {
            BorderFile* bf = brain->getBorderFile(i);
            if (bf == undoBorderFile) {
                foundBorderFlag = true;
                break;
            }
        }
        
        if (foundBorderFlag) {
            foundBorderFlag = false;
            const int32_t numBorders = undoBorderFile->getNumberOfBorders();
            for (int32_t i = 0; i < numBorders; i++) {
                if (undoBorderFile->getBorder(i) == undoBorder) {
                    foundBorderFlag = true;
                    break;
                }
            }
        }
        
        if (foundBorderFlag) {
            if (undoBorder->isUndoBorderValid()) {
                if (WuQMessageBox::warningOkCancel(m_undoFinishToolButton,
                                                   ("Undo changes to " + undoBorder->getName()))) {
                    undoBorder->undoLastBorderEditing();
                }
            }
            else {
                WuQMessageBox::errorOk(m_undoFinishToolButton,
                                       ("Cannot undo border " + undoBorder->getName()));
            }
        }
        else {
            WuQMessageBox::errorOk(m_undoFinishToolButton, "Cannot undo last edited border.  "
                                   "Did not find border for undoing.");
        }
    }
    
    resetLastEditedBorder();
}

/**
 * Publicly accessible method for initiating
 * an operation as if the Finish button was
 * pressed.
 */
void 
UserInputModeBordersWidget::executeFinishOperation()
{
    this->drawFinishButtonClicked();
}

/**
 * Called when draw border finish button clicked.
 */
void 
UserInputModeBordersWidget::drawFinishButtonClicked()
{
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->inputModeBorders->windowIndex);
    if (browserWindow == NULL) {
        return;
    }
    BrowserTabContent* btc = browserWindow->getBrowserTabContent();
    if (btc == NULL) {
        return;
    }
    const int32_t browserTabIndex = btc->getTabNumber();
    
    if (this->inputModeBorders->borderBeingDrawnByOpenGL->verifyAllPointsOnSameStructure() == false) {
        WuQMessageBox::errorOk(this, "Error: Border points are on more than one structure.");
        return;
    }
    
    AString modeText;
    int32_t minimumNumberOfBorderPoints = 2;
    switch (this->inputModeBorders->getDrawOperation()) {
        case UserInputModeBorders::DRAW_OPERATION_CREATE:
            modeText = "creating a border.";
            minimumNumberOfBorderPoints = 2;
            break;
        case UserInputModeBorders::DRAW_OPERATION_OPTIMIZE:
            modeText = "optimizing borders.";
            minimumNumberOfBorderPoints = 3;
            break;
        case UserInputModeBorders::DRAW_OPERATION_ERASE:
            modeText = "erasing a segment in a border.";
            minimumNumberOfBorderPoints = 2;
            break;
        case UserInputModeBorders::DRAW_OPERATION_EXTEND:
            modeText = "extending a border.";
            minimumNumberOfBorderPoints = 2;
            break;
        case UserInputModeBorders::DRAW_OPERATION_REPLACE:
            modeText = "replacing points in a border.";
            minimumNumberOfBorderPoints = 2;
            break;
    }
    
    if (this->inputModeBorders->borderBeingDrawnByOpenGL->getNumberOfPoints() < minimumNumberOfBorderPoints) {
        WuQMessageBox::errorOk(this,
                               ("There must be at least "
                                + AString::number(minimumNumberOfBorderPoints)
                                + " points when "
                                + modeText));
        return;
    }

    ModelSurface* surfaceController = btc->getDisplayedSurfaceModel();
    ModelWholeBrain* wholeBrainController = btc->getDisplayedWholeBrainModel();
    ModelSurfaceMontage* surfaceMontageController = btc->getDisplayedSurfaceMontageModel();
    
    Brain* brain = NULL;
    Surface* surface = NULL;
    if (surfaceController != NULL) {
        brain = surfaceController->getBrain();
        surface = surfaceController->getSurface();
    }
    else if (wholeBrainController != NULL) {
        brain = wholeBrainController->getBrain();
        const StructureEnum::Enum structure = this->inputModeBorders->borderBeingDrawnByOpenGL->getStructure();
        surface = wholeBrainController->getSelectedSurface(structure, btc->getTabNumber());
    }
    else if (surfaceMontageController != NULL) {
        brain = surfaceMontageController->getBrain();
        const StructureEnum::Enum structure = this->inputModeBorders->borderBeingDrawnByOpenGL->getStructure();
        surface = surfaceMontageController->getSelectedSurface(structure, btc->getTabNumber());
    }
    
    if (surface == NULL) {
        AString tabsMessage;
        if (browserWindow->isTileTabsSelected()) {
            tabsMessage = "Verify that points are added in the SELECTED tab.";
        }
        WuQMessageBox::errorOk(this, "Borders may only be drawn on surface models.  "
                               + tabsMessage);
        return;
    }
    if (brain == NULL) {
        CaretLogSevere("PROGRAM ERROR: Cannot find brain for border drawing.");
        return;
    }
    
    DisplayPropertiesBorders* dpb = GuiManager::get()->getBrain()->getDisplayPropertiesBorders();
    const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(btc->getTabNumber());
    dpb->setDisplayed(displayGroup, 
                      browserTabIndex,
                      true);
    
    switch (this->inputModeBorders->getDrawOperation()) {
        case UserInputModeBorders::DRAW_OPERATION_CREATE:
        {
            std::auto_ptr<BorderPropertiesEditorDialog> finishBorderDialog(
                    BorderPropertiesEditorDialog::newInstanceFinishBorder(this->inputModeBorders->borderBeingDrawnByOpenGL,
                                                                          surface,
                                                                          this));
            if (finishBorderDialog->exec() == BorderPropertiesEditorDialog::Accepted) {
                this->inputModeBorders->drawOperationFinish();
            }
        }
            break;
        case UserInputModeBorders::DRAW_OPERATION_OPTIMIZE:
            processBorderOptimization(displayGroup,
                                      browserTabIndex,
                                      surface,
                                      this->inputModeBorders->borderBeingDrawnByOpenGL);
            break;
        case UserInputModeBorders::DRAW_OPERATION_ERASE:
        case UserInputModeBorders::DRAW_OPERATION_EXTEND:
        case UserInputModeBorders::DRAW_OPERATION_REPLACE:
        {
            const float nearestTolerance = 10;
            
            std::vector<BorderPointFromSearch> allNearbyBorders;
            const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
            for (int32_t ibf = 0; ibf < numBorderFiles; ibf++) {
                const BorderFile* borderFile = brain->getBorderFile(ibf);
                std::vector<BorderPointFromSearch> bordersFoundFromFile;
                
                switch (this->inputModeBorders->getDrawOperation()) {
                    case UserInputModeBorders::DRAW_OPERATION_CREATE:
                        CaretAssert(0);
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_ERASE:
                        borderFile->findAllBordersWithPointsNearBothSegmentEndPoints(displayGroup,
                                                                                     browserTabIndex,
                                                                                     surface,
                                                                                     this->inputModeBorders->borderBeingDrawnByOpenGL,
                                                                                     nearestTolerance,
                                                                                     bordersFoundFromFile);
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_EXTEND:
                        borderFile->findAllBordersWithAnyPointNearSegmentFirstPoint(displayGroup,
                                                                                    browserTabIndex,
                                                                                    surface,
                                                                                    this->inputModeBorders->borderBeingDrawnByOpenGL,
                                                                                    nearestTolerance,
                                                                                    bordersFoundFromFile);
//                        borderFile->findAllBordersWithEndPointNearSegmentFirstPoint(displayGroup,
//                                                                                  browserTabIndex,
//                                                                                  surface,
//                                                                                  this->inputModeBorders->borderBeingDrawnByOpenGL,
//                                                                                  nearestTolerance,
//                                                                                  bordersFoundFromFile);
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_OPTIMIZE:
                        CaretAssert(0);
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_REPLACE:
                        borderFile->findAllBordersWithPointsNearBothSegmentEndPoints(displayGroup,
                                                                                     browserTabIndex,
                                                                                     surface,
                                                                                     this->inputModeBorders->borderBeingDrawnByOpenGL,
                                                                                     nearestTolerance,
                                                                                     bordersFoundFromFile);
                        break;
                }
                
                allNearbyBorders.insert(allNearbyBorders.end(),
                                        bordersFoundFromFile.begin(),
                                        bordersFoundFromFile.end());
            }
            
            if (allNearbyBorders.empty()) {
                WuQMessageBox::errorOk(this, "No borders were found near the border editing points");
                return;
            }
            else {
                std::sort(allNearbyBorders.begin(),
                          allNearbyBorders.end());
                const int32_t numBorders = static_cast<int32_t>(allNearbyBorders.size());
                
                AString modeMessage;
                switch (this->inputModeBorders->getDrawOperation()) {
                    case UserInputModeBorders::DRAW_OPERATION_CREATE:
                        CaretAssert(0);
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_ERASE:
                        modeMessage = "Erase segement in";
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_EXTEND:
                        modeMessage = "Extend";
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_OPTIMIZE:
                        CaretAssert(0);
                        break;
                    case UserInputModeBorders::DRAW_OPERATION_REPLACE:
                        modeMessage = "Replace segment in";
                        break;
                }
                
                std::vector<AString> borderNames;
                for (int32_t i = 0; i < numBorders; i++) {
                    BorderPointFromSearch& bpfs = allNearbyBorders[i];
                    borderNames.push_back(bpfs.border()->getName()
                                          + "  ("
                                          + AString::number(bpfs.distance(), 'f', 6)
                                          + " mm)");
                }
                BorderEditingSelectionDialog selDialog(modeMessage,
                                                       borderNames,
                                                       this);
                if (selDialog.exec() == BorderEditingSelectionDialog::Accepted) {
                    AString errorMessage;
                    
                    std::vector<BorderFileAndBorderMemento> undoBorders;
                    
                    for (int32_t i = 0; i < numBorders; i++) {
                        if (selDialog.isBorderNameSelected(i)) {
                            BorderPointFromSearch& bpfs = allNearbyBorders[i];
                            BorderFile* borderFile = bpfs.borderFile();
                            Border* border = bpfs.border();
                            int32_t borderPointIndex = bpfs.borderPointIndex();
                            CaretAssert(borderFile);
                            CaretAssert(border);
                            
                            try {
                                switch (this->inputModeBorders->getDrawOperation()) {
                                    case UserInputModeBorders::DRAW_OPERATION_CREATE:
                                        CaretAssert(0);
                                        break;
                                    case UserInputModeBorders::DRAW_OPERATION_ERASE:
                                        border->reviseEraseFromEnd(surface,
                                                                   this->inputModeBorders->borderBeingDrawnByOpenGL);
                                        break;
                                    case UserInputModeBorders::DRAW_OPERATION_EXTEND:
                                        border->reviseExtendFromPointIndex(surface,
                                                                           borderPointIndex,
                                                                           this->inputModeBorders->borderBeingDrawnByOpenGL);
                                        //border->reviseExtendFromEnd(surface,
                                        //                            this->inputModeBorders->borderBeingDrawnByOpenGL);
                                        break;
                                    case UserInputModeBorders::DRAW_OPERATION_OPTIMIZE:
                                        CaretAssert(0);
                                        break;
                                    case UserInputModeBorders::DRAW_OPERATION_REPLACE:
                                        border->reviseReplaceSegment(surface,
                                                                     this->inputModeBorders->borderBeingDrawnByOpenGL);
                                        break;
                                }
                                
                                undoBorders.push_back(BorderFileAndBorderMemento(borderFile,
                                                                                 border));
                            }
                            catch (BorderException& e) {
                                errorMessage.appendWithNewLine(e.whatString());
                            }
                        }
                    }
                    
                    if ( ! errorMessage.isEmpty()) {
                        WuQMessageBox::errorOk(this,
                                               errorMessage);
                    }
                    else {
                        setLastEditedBorder(undoBorders);
                        this->inputModeBorders->borderBeingDrawnByOpenGL->clear();
                    }
                    
                    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
                    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
                }
            }
        }
            break;
    }
}

/**
 * Process the border optimization operation.
 *
 * @param surface
 *     Surface on which border is drawn.
 * @param borderDrawnByUser
 *     Border drawn by the user.
 */
void
UserInputModeBordersWidget::processBorderOptimization(const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t browserTabIndex,
                                                      Surface* surface,
                                                      Border* borderDrawnByUser)
{
    std::vector<int32_t> nodesInsideBorder;

    try {
        ProgressObject* myProgObj = NULL;
        const bool inverseSelectionFlag = false;
        
        /*
         * Find nodes inside border
         */
        AlgorithmNodesInsideBorder nib(myProgObj,
                                       surface,
                                       borderDrawnByUser,
                                       inverseSelectionFlag,
                                       nodesInsideBorder);
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
        return;
    }
    
    if (nodesInsideBorder.empty()) {
        WuQMessageBox::errorOk(this,
                               ("No nodes were found inside the drawn border "
                                + borderDrawnByUser->getName()));
        return;
    }
    
    Brain* brain = GuiManager::get()->getBrain();
    const StructureEnum::Enum surfaceStructure = surface->getStructure();
    const BrainStructure* brainStructure = brain->getBrainStructure(surfaceStructure, false);
    if (brainStructure == NULL) {
        WuQMessageBox::errorOk(this,
                               "No files for surface structure "
                               + StructureEnum::toGuiName(surfaceStructure));
        return;
    }
    
    
    std::vector<DataFileTypeEnum::Enum> optimizeDataFileTypes;
    optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE);
    optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR);
    optimizeDataFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES);
    optimizeDataFileTypes.push_back(DataFileTypeEnum::METRIC);
    
    std::vector<CaretMappableDataFile*> optimizeDataFiles;
    brain->getAllMappableDataFileWithDataFileTypes(optimizeDataFileTypes,
                                                   optimizeDataFiles);
    
    /*
     * Create a bool vector that indicates nodes inside border 
     * from the node index.
     */
    const int32_t numberOfSurfaceNodes = surface->getNumberOfNodes();
    std::vector<bool> nodeInROI(numberOfSurfaceNodes, false);
    for (std::vector<int32_t>::const_iterator iter = nodesInsideBorder.begin();
         iter != nodesInsideBorder.end();
         iter++) {
        const int32_t nodeIndex = *iter;
        CaretAssertVectorIndex(nodeInROI, nodeIndex);
        nodeInROI[nodeIndex] = true;
    }
    
    /*
     * Find borders inside region of interest
     */
    std::map<Border*, BorderFile*> borderToBorderFileMap;
    std::vector<std::pair<int32_t, Border*> > bordersInsideRegionOfInterest;
    const int32_t numberOfBorderFiles = brain->getNumberOfBorderFiles();
    for (int32_t iFile = 0; iFile < numberOfBorderFiles; iFile++) {
        BorderFile* borderFile = brain->getBorderFile(iFile);
        
        std::vector<std::pair<int32_t, Border*> > nodeCountAndBordersFromFileInROI;
        borderFile->findBordersInsideRegionOfInterest(displayGroup,
                                                      browserTabIndex,
                                                      surface,
                                                      nodeInROI,
                                                      nodeCountAndBordersFromFileInROI);
        
        for (std::vector<std::pair<int32_t, Border*> > ::iterator bi = nodeCountAndBordersFromFileInROI.begin();
             bi != nodeCountAndBordersFromFileInROI.end();
             bi++) {
            Border* border = bi->second;
            borderToBorderFileMap.insert(std::make_pair(border, borderFile));
            bordersInsideRegionOfInterest.push_back(*bi);
        }
    }
    
    if (bordersInsideRegionOfInterest.empty()) {
        WuQMessageBox::errorOk(this,
                               "No borders were found inside the drawn border.");
        return;
    }
    
    std::sort(bordersInsideRegionOfInterest.begin(), bordersInsideRegionOfInterest.end());

    if (m_borderOptimizeDialog == NULL) {
        m_borderOptimizeDialog = new BorderOptimizeDialog(this);
    }
    m_borderOptimizeDialog->updateDialog(browserTabIndex,
                                         surface,
                                         bordersInsideRegionOfInterest,
                                         const_cast<Border*>(borderDrawnByUser),
                                         nodesInsideBorder);
    if (m_borderOptimizeDialog->exec() == BorderOptimizeDialog::Accepted) {
        std::vector<Border*> modifiedBorders;
        m_borderOptimizeDialog->getModifiedBorders(modifiedBorders);
        
        /*
         * Track modified borders so that changes can be 'undone' by 
         * the user.
         */
        std::vector<BorderFileAndBorderMemento> undoBorders;
        for (std::vector<Border*>::iterator mbi = modifiedBorders.begin();
             mbi != modifiedBorders.end();
             mbi++) {
            Border* border = *mbi;
            std::map<Border*, BorderFile*>::iterator mapIter = borderToBorderFileMap.find(border);
            if (mapIter != borderToBorderFileMap.end()) {
                BorderFile* borderFile = mapIter->second;
                undoBorders.push_back(BorderFileAndBorderMemento(borderFile,
                                                                 border));
            }
            else {
                CaretAssertMessage(0, "PROGRAM ERROR: border file not found for border.");
            }
        }
        
        setLastEditedBorder(undoBorders);
        
        if ( ! m_borderOptimizeDialog->isKeepBoundaryBorderSelected()) {
            this->inputModeBorders->borderBeingDrawnByOpenGL->clear();
        }
    }
    
    /*
     * Update all graphics windows to displayed changed borders
     */
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    adjustViewActionTriggered();
}


/**
 * Called when Adjust View button is pressed.
 */
void 
UserInputModeBordersWidget::adjustViewActionTriggered()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when a draw mode button is clicked.
 * @param action
 *    Action that was triggered.
 */
void 
UserInputModeBordersWidget::drawOperationActionTriggered(QAction* action)
{
    const int drawModeInteger = action->data().toInt();
    const UserInputModeBorders::DrawOperation drawOperation = 
        static_cast<UserInputModeBorders::DrawOperation>(drawModeInteger);
    this->inputModeBorders->setDrawOperation(drawOperation);
}

/**
 * @return The edit widget.
 */
QWidget* 
UserInputModeBordersWidget::createEditOperationWidget()
{
    const AString deleteToolTipText = ("Delete a border by clicking the mouse over "
                                       "any point in the border."
                                       + m_transformToolTipText);
    QAction* deleteAction = WuQtUtilities::createAction("Delete",
                                                        WuQtUtilities::createWordWrappedToolTipText(deleteToolTipText),
                                                        this);
    deleteAction->setCheckable(true);
    deleteAction->setData(static_cast<int>(UserInputModeBorders::EDIT_OPERATION_DELETE));
    QToolButton* deleteToolButton = new QToolButton();
    deleteToolButton->setDefaultAction(deleteAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(deleteToolButton);
    
    const AString propertiesToolTipText = ("A dialog for editing a border's properties is displayed by "
                                           "clicking any point in a border."
                                           + m_transformToolTipText);
    QAction* propertiesAction = WuQtUtilities::createAction("Properties", 
                                                         WuQtUtilities::createWordWrappedToolTipText(propertiesToolTipText),
                                                         this);
    propertiesAction->setCheckable(true);
    propertiesAction->setData(static_cast<int>(UserInputModeBorders::EDIT_OPERATION_PROPERTIES));
    QToolButton* propertiesToolButton = new QToolButton();
    propertiesToolButton->setDefaultAction(propertiesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(propertiesToolButton);
    
    this->editOperationActionGroup = new QActionGroup(this);
    this->editOperationActionGroup->addAction(deleteAction);
    this->editOperationActionGroup->addAction(propertiesAction);
    this->editOperationActionGroup->setExclusive(true);
    QObject::connect(this->editOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(editOperationActionTriggered(QAction*)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(deleteToolButton);
    layout->addWidget(propertiesToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called when a edit button is clicked.
 * @param action
 *    Action that was triggered.
 */
void 
UserInputModeBordersWidget::editOperationActionTriggered(QAction* action)
{
    const int editModeInteger = action->data().toInt();
    const UserInputModeBorders::EditOperation editOperation = 
    static_cast<UserInputModeBorders::EditOperation>(editModeInteger);
    this->inputModeBorders->setEditOperation(editOperation);
}

/**
 * @return The ROI widget.
 */
QWidget* 
UserInputModeBordersWidget::createRoiOperationWidget()
{
    QWidget* widget = new QWidget();
//    QHBoxLayout* layout = new QHBoxLayout(widget);
//    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
//    
//    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called when the user selects a border in ROI opeation.
 *
 * @param brain
 *    Brain on which identification occurred.
 * @param surfaceFile
 *    Surface on which border is located.
 * @param border
 *    Border for which nodes are found inside.
 */
void 
UserInputModeBordersWidget::executeRoiInsideSelectedBorderOperation(Brain* /*brain*/,
                                                                    Surface* surface,
                                                                    Border* border)
{
    if (border->verifyAllPointsOnSameStructure() == false) {
        WuQMessageBox::errorOk(this, "Error: Border points are on more than one structure.");
        return;
    }
    
    RegionOfInterestCreateFromBorderDialog createRoiDialog(border,
                                                           surface,
                                                           this);
    createRoiDialog.exec();
}

/**
 * Reset the last edited border.
 */
void
UserInputModeBordersWidget::resetLastEditedBorder()
{
    m_undoFinishBorders.clear();
}

/**
 * Set the last edited border.
 *
 * @param undoFinishBorders
 *    Borders that were changed by the last border edit operation.
 */
void
UserInputModeBordersWidget::setLastEditedBorder(std::vector<BorderFileAndBorderMemento>& undoFinishBorders)
{
    m_undoFinishBorders = undoFinishBorders;
}



