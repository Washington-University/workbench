
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <memory>

#include <QAction>
#include <QActionGroup>
#include <QBoxLayout>
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QStackedWidget>
#include <QToolButton>


#define __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__
#include "UserInputModeBordersWidget.h"
#undef __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__

#include "Border.h"
#include "BorderPropertiesEditorDialog.h"
#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "ModelDisplayControllerSurface.h"
#include "Surface.h"
#include "UserInputModeBorders.h"
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
    this->inputModeBorders = inputModeBorders;
    
    QLabel* nameLabel = new QLabel("Borders ");
    
    this->widgetMode = this->createModeWidget();
    
    this->widgetDrawOperation = this->createDrawOperationWidget();
    
    this->widgetEditOperation = this->createEditOperationWidget();
    
    this->widgetSelectOperation = this->createSelectOperationWidget();
    
    this->operationStackedWidget = new QStackedWidget();
    this->operationStackedWidget->addWidget(this->widgetDrawOperation);
    this->operationStackedWidget->addWidget(this->widgetEditOperation);
    this->operationStackedWidget->addWidget(this->widgetSelectOperation);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(nameLabel);
    layout->addWidget(this->widgetMode);
    layout->addSpacing(10);
    layout->addWidget(this->operationStackedWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
UserInputModeBordersWidget::~UserInputModeBordersWidget()
{
    
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
            break;
        case UserInputModeBorders::MODE_EDIT:
            this->operationStackedWidget->setCurrentWidget(this->widgetEditOperation);
            this->setActionGroupByActionData(this->editOperationActionGroup, 
                                             inputModeBorders->getEditOperation());
            break;
        case UserInputModeBorders::MODE_SELECT:
            this->operationStackedWidget->setCurrentWidget(this->widgetSelectOperation);
            break;
    }
    const int selectedModeInteger = (int)this->inputModeBorders->getMode();

    const int modeComboBoxIndex = this->modeComboBox->findData(selectedModeInteger);
    CaretAssert(modeComboBoxIndex >= 0);
    this->modeComboBox->blockSignals(true);
    this->modeComboBox->setCurrentIndex(modeComboBoxIndex);
    this->modeComboBox->blockSignals(false);
    
    /*
     * Select the button for the mode
     */
    this->modeActionGroup->blockSignals(true);
    const QList<QAction*> modeActions = this->modeActionGroup->actions();
    QListIterator<QAction*> iter(modeActions);
    while (iter.hasNext()) {
        QAction* action = iter.next();
        const int modeInteger = action->data().toInt();
        if (selectedModeInteger == modeInteger) {
            action->setChecked(true);
            break;
        }
    }
    this->modeActionGroup->blockSignals(false);
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
    this->modeComboBox->addItem("Select", (int)UserInputModeBorders::MODE_SELECT);
    QObject::connect(this->modeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(modeComboBoxSelection(int)));
    
    QAction* drawAction = WuQtUtilities::createAction("Draw", "Draw new and updated borders", this);
    drawAction->setCheckable(true);
    drawAction->setData((int)UserInputModeBorders::MODE_DRAW);
    QToolButton* drawToolButton = new QToolButton();
    drawToolButton->setDefaultAction(drawAction);
    
    QAction* editAction = WuQtUtilities::createAction("Edit", "Edit Borders", this);
    editAction->setCheckable(true);
    editAction->setData((int)UserInputModeBorders::MODE_EDIT);
    QToolButton* editToolButton = new QToolButton();
    editToolButton->setDefaultAction(editAction);
    
    QAction* selectAction = WuQtUtilities::createAction("Select", "Select Borders", this);
    selectAction->setCheckable(true);
    selectAction->setData((int)UserInputModeBorders::MODE_SELECT);
    QToolButton* selectToolButton = new QToolButton();
    selectToolButton->setDefaultAction(selectAction);
    
    this->modeActionGroup = new QActionGroup(this);
    this->modeActionGroup->addAction(drawAction);
    this->modeActionGroup->addAction(editAction);
    this->modeActionGroup->addAction(selectAction);
    this->modeActionGroup->setExclusive(true);
    QObject::connect(this->modeActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(modeActionTriggered(QAction*)));
    
    const bool useComboBoxFlag = true;
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
    if (useComboBoxFlag) {
        layout->addWidget(this->modeComboBox);
    }
    else {
        layout->addWidget(drawToolButton);
        layout->addWidget(editToolButton);
        layout->addWidget(selectToolButton);
    }
    
    widget->setFixedWidth(widget->sizeHint().width());
    
    return widget;
}

/**
 * @return Is the transform button selected in draw mode?
 */
bool 
UserInputModeBordersWidget::isDrawModeTransformSelected() const
{
    return this->drawModeTransformAction->isChecked();
    
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
}

/**
 * Called when a mode button is clicked.
 * @param action
 *    Action that was triggered.
 */
void 
UserInputModeBordersWidget::modeActionTriggered(QAction* action)
{
    CaretAssert(action);
    const int modeInteger = action->data().toInt();
    const UserInputModeBorders::Mode mode = (UserInputModeBorders::Mode)modeInteger;
    this->inputModeBorders->setMode(mode);
}

/**
 * @return The draw operation widget.
 */
QWidget* 
UserInputModeBordersWidget::createDrawOperationWidget()
{
    this->drawModeTransformAction = WuQtUtilities::createAction("Adjust View", 
                                                        "Pause border drawing and allow the mouse to\n"
                                                        "adjust view (pan/zoom/rotate) of the surface", 
                                                        this);
    this->drawModeTransformAction->setCheckable(true);
    QToolButton* transformToolButton = new QToolButton();
    transformToolButton->setDefaultAction(this->drawModeTransformAction);
    
    QAction* drawAction = WuQtUtilities::createAction("Draw", 
                                                      "Draw a border  by either clicking the\n"
                                                      "mouse along the desired border path or by\n"
                                                      "moving the mouse with the left mouse button\n"
                                                      "depressed until the end point is reached.\n"
                                                      "Press the \"Finish\" button to complete the \n"
                                                      "border by setting its name and color", 
                                                      this);
    drawAction->setCheckable(true);
    drawAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_CREATE));
    QToolButton* drawToolButton = new QToolButton();
    drawToolButton->setDefaultAction(drawAction);
    
    QAction* eraseAction = WuQtUtilities::createAction("Erase", 
                                                       "Erase the an end of the border by \n"
                                                       "dragging the mouse along the region\n"
                                                       "that is to be deleted", 
                                                       this);
    eraseAction->setCheckable(true);
    eraseAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_ERASE));
    QToolButton* eraseToolButton = new QToolButton();
    eraseToolButton->setDefaultAction(eraseAction);
    
    QAction* extendAction = WuQtUtilities::createAction("Extend", 
                                                        "Extend a border by dragging the mouse\n"
                                                        "starting at the end of a border and \n"
                                                        "until its new termination point.", 
                                                        this);
    extendAction->setCheckable(true);
    extendAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_EXTEND));
    QToolButton* extendToolButton = new QToolButton();
    extendToolButton->setDefaultAction(extendAction);
    
    QAction* replaceAction = WuQtUtilities::createAction("Replace", 
                                                         "Replace a section of a border by placing\n"
                                                         "the mouse over a point on the border where\n"
                                                         "the new segment begins and dragging the \n"
                                                         "mouse to the end of the section that is\n"
                                                         "being replaced.", 
                                                         this);
    replaceAction->setCheckable(true);
    replaceAction->setData(static_cast<int>(UserInputModeBorders::DRAW_OPERATION_REPLACE));
    QToolButton* replaceToolButton = new QToolButton();
    replaceToolButton->setDefaultAction(replaceAction);
    
    QAction* finishAction = WuQtUtilities::createAction("Finish", 
                                                        "Finish drawing a new border by\n"
                                                        "setting the name and color", 
                                                        this,
                                                        this,
                                                        SLOT(drawFinishButtonClicked()));
    QToolButton* finishToolButton = new QToolButton();
    finishToolButton->setDefaultAction(finishAction);
    
    QAction* undoAction = WuQtUtilities::createAction("Undo", 
                                                      "Remove (undo) the last border point\n"
                                                      "in the unfinished border",
                                                      this,
                                                      this,
                                                      SLOT(drawUndoButtonClicked()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(undoAction);
    
    QAction* resetAction = WuQtUtilities::createAction("Reset", 
                                                       "Remove all points in the unfinished border", 
                                                       this,
                                                       this,
                                                       SLOT(drawResetButtonClicked()));
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(resetAction);
    
    this->drawOperationActionGroup = new QActionGroup(this);
    this->drawOperationActionGroup->addAction(drawAction);
    this->drawOperationActionGroup->addAction(eraseAction);
    this->drawOperationActionGroup->addAction(extendAction);
    this->drawOperationActionGroup->addAction(replaceAction);
    this->drawOperationActionGroup->setExclusive(true);
    QObject::connect(this->drawOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(drawOperationActionTriggered(QAction*)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
    layout->addWidget(transformToolButton);
    layout->addSpacing(10);
    layout->addWidget(drawToolButton);
    layout->addWidget(eraseToolButton);
    layout->addWidget(extendToolButton);
    layout->addWidget(replaceToolButton);
    layout->addSpacing(10);
    layout->addWidget(finishToolButton);
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
 * Called when draw border finish button clicked.
 */
void 
UserInputModeBordersWidget::drawFinishButtonClicked()
{
    switch (this->inputModeBorders->getDrawOperation()) {
        case UserInputModeBorders::DRAW_OPERATION_CREATE:
        {
            std::auto_ptr<BorderPropertiesEditorDialog> finishBorderDialog(
                    BorderPropertiesEditorDialog::newInstanceFinishBorder(this->inputModeBorders->borderBeingDrawnByOpenGL,
                                                                  this));
            if (finishBorderDialog->exec() == BorderPropertiesEditorDialog::Accepted) {
                this->inputModeBorders->drawOperationFinish();
            }
        }
            break;
        case UserInputModeBorders::DRAW_OPERATION_ERASE:
        case UserInputModeBorders::DRAW_OPERATION_EXTEND:
        case UserInputModeBorders::DRAW_OPERATION_REPLACE:
        {
            BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(this->inputModeBorders->windowIndex);
            if (browserWindow == NULL) {
                return;
            }
            BrowserTabContent* btc = browserWindow->getBrowserTabContent();
            if (btc == NULL) {
                return;
            }
            ModelDisplayControllerSurface* surfaceController = btc->getDisplayedSurfaceModel();
            if (surfaceController == NULL) {
                return;
            }
            
            Surface* surface = surfaceController->getSurface();
            Brain* brain = surfaceController->getBrain();
            
            float nearestTolerance = 15;
            BorderFile* borderFile;
            int32_t borderFileIndex; 
            Border* border;
            int32_t borderIndex;
            SurfaceProjectedItem* borderPoint;
            int32_t borderPointIndex;
            float distanceToNearestBorder;
            if (brain->findBorderNearestBorder(surface, 
                                               this->inputModeBorders->borderBeingDrawnByOpenGL,
                                               Brain::NEAREST_BORDER_TEST_MODE_ENDPOINTS, 
                                               nearestTolerance,
                                               borderFile,
                                               borderFileIndex,
                                               border, 
                                               borderIndex,
                                               borderPoint,
                                               borderPointIndex,
                                               distanceToNearestBorder)) {
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
                            border->reviseExtendFromEnd(surface,
                                                        this->inputModeBorders->borderBeingDrawnByOpenGL);
                            break;
                        case UserInputModeBorders::DRAW_OPERATION_REPLACE:
                            border->reviseReplaceSegment(surface, 
                                                         this->inputModeBorders->borderBeingDrawnByOpenGL);
                            break;
                    }
                }
                catch (BorderException& e) {
                    WuQMessageBox::errorOk(this,
                                           e.whatString());
                }
            }
            this->inputModeBorders->borderBeingDrawnByOpenGL->clear();
            
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            
        }
            break;
    }
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
    QAction* deleteAction = WuQtUtilities::createAction("Delete", 
                                                        "Delete a border by clicking\n"
                                                        "the mouse over the border",
                                                        this);
    deleteAction->setCheckable(true);
    deleteAction->setData(static_cast<int>(UserInputModeBorders::EDIT_OPERATION_DELETE));
    QToolButton* deleteToolButton = new QToolButton();
    deleteToolButton->setDefaultAction(deleteAction);
    
    QAction* propertiesAction = WuQtUtilities::createAction("Properties", 
                                                         "Edit the properties of a border by clicking\n"
                                                         "on a border which causes a border properties\n"
                                                         "editor to appear", 
                                                         this);
    propertiesAction->setCheckable(true);
    propertiesAction->setData(static_cast<int>(UserInputModeBorders::EDIT_OPERATION_PROPERTIES));
    QToolButton* propertiesToolButton = new QToolButton();
    propertiesToolButton->setDefaultAction(propertiesAction);
    
    this->editOperationActionGroup = new QActionGroup(this);
    this->editOperationActionGroup->addAction(deleteAction);
    this->editOperationActionGroup->addAction(propertiesAction);
    this->editOperationActionGroup->setExclusive(true);
    QObject::connect(this->editOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(editOperationActionTriggered(QAction*)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
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
 * @return The select widget.
 */
QWidget* 
UserInputModeBordersWidget::createSelectOperationWidget()
{
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

