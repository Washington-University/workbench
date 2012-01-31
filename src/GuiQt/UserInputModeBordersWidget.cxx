
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

#include "CaretAssert.h"
#include "BorderDrawFinishDialog.h"
#include "UserInputModeBorders.h"
#include "WuQtUtilities.h"

using namespace caret;



/**
 * \class UserInputModeBordersWidget 
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
    
    this->widgetCreateOperation = this->createCreateOperationWidget();
    
    this->widgetReviseOperation = this->createReviseOperationWidget();
    
    this->widgetSelectOperation = this->createSelectOperationWidget();
    
    this->operationStackedWidget = new QStackedWidget();
    this->operationStackedWidget->addWidget(this->widgetCreateOperation);
    this->operationStackedWidget->addWidget(this->widgetReviseOperation);
    this->operationStackedWidget->addWidget(this->widgetSelectOperation);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0, 0);
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
        case UserInputModeBorders::MODE_CREATE:
            this->operationStackedWidget->setCurrentWidget(this->widgetCreateOperation);
            this->setActionGroupByActionData(this->createOperationActionGroup,
                                             inputModeBorders->getCreateOperation());
            break;
        case UserInputModeBorders::MODE_REVISE:
            this->operationStackedWidget->setCurrentWidget(this->widgetReviseOperation);
            this->setActionGroupByActionData(this->reviseOperationActionGroup, 
                                             inputModeBorders->getReviseOperation());
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
    this->modeComboBox->addItem("Create", (int)UserInputModeBorders::MODE_CREATE);
    this->modeComboBox->addItem("Revise", (int)UserInputModeBorders::MODE_REVISE);
    this->modeComboBox->addItem("Select", (int)UserInputModeBorders::MODE_SELECT);
    QObject::connect(this->modeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(modeComboBoxSelection(int)));
    
    QAction* createAction = WuQtUtilities::createAction("Create", "Create new borders", this);
    createAction->setCheckable(true);
    createAction->setData((int)UserInputModeBorders::MODE_CREATE);
    QToolButton* createToolButton = new QToolButton();
    createToolButton->setDefaultAction(createAction);
    
    QAction* reviseAction = WuQtUtilities::createAction("Revise", "Revise Borders", this);
    reviseAction->setCheckable(true);
    reviseAction->setData((int)UserInputModeBorders::MODE_REVISE);
    QToolButton* reviseToolButton = new QToolButton();
    reviseToolButton->setDefaultAction(reviseAction);
    
    QAction* selectAction = WuQtUtilities::createAction("Select", "Select Borders", this);
    selectAction->setCheckable(true);
    selectAction->setData((int)UserInputModeBorders::MODE_SELECT);
    QToolButton* selectToolButton = new QToolButton();
    selectToolButton->setDefaultAction(selectAction);
    
    this->modeActionGroup = new QActionGroup(this);
    this->modeActionGroup->addAction(createAction);
    this->modeActionGroup->addAction(reviseAction);
    this->modeActionGroup->addAction(selectAction);
    this->modeActionGroup->setExclusive(true);
    QObject::connect(this->modeActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(modeActionTriggered(QAction*)));
    
    const bool useComboBoxFlag = true;
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0, 0);
    if (useComboBoxFlag) {
        layout->addWidget(this->modeComboBox);
    }
    else {
        layout->addWidget(createToolButton);
        layout->addWidget(reviseToolButton);
        layout->addWidget(selectToolButton);
    }
    
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
 * @return The create operation widget.
 */
QWidget* 
UserInputModeBordersWidget::createCreateOperationWidget()
{
    QAction* drawAction = WuQtUtilities::createAction("Draw", 
                                                      "Draw a border  by either clicking the\n"
                                                      "mouse along the desired border path or by\n"
                                                      "moving the mouse with the left mouse button\n"
                                                      "depressed until the end point is reached.\n"
                                                      "Press the \"Finish\" button to complete the \n"
                                                      "border by setting its name and color", 
                                                      this);
    drawAction->setCheckable(true);
    drawAction->setData(static_cast<int>(UserInputModeBorders::CREATE_OPERATION_DRAW));
    QToolButton* drawToolButton = new QToolButton();
    drawToolButton->setDefaultAction(drawAction);
    
    QAction* transformAction = WuQtUtilities::createAction("Adjust View", 
                                                        "Pause border drawing and allow the mouse to\n"
                                                        "adjust view (pan/zoom/rotate) of the surface", 
                                                        this);
    transformAction->setCheckable(true);
    transformAction->setData(static_cast<int>(UserInputModeBorders::CREATE_OPERATION_TRANSFORM));
    QToolButton* transformToolButton = new QToolButton();
    transformToolButton->setDefaultAction(transformAction);
    
    QAction* finishAction = WuQtUtilities::createAction("Finish", 
                                                        "Finish drawing a new border by\n"
                                                        "setting the name and color", 
                                                        this,
                                                        this,
                                                        SLOT(createFinishButtonClicked()));
    QToolButton* finishToolButton = new QToolButton();
    finishToolButton->setDefaultAction(finishAction);
    
    QAction* undoAction = WuQtUtilities::createAction("Undo", 
                                                      "Remove (undo) the last border point\n"
                                                      "in the unfinished border",
                                                      this,
                                                      this,
                                                      SLOT(createUndoButtonClicked()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(undoAction);
    
    QAction* resetAction = WuQtUtilities::createAction("Reset", 
                                                       "Remove all points in the unfinished border", 
                                                       this,
                                                       this,
                                                       SLOT(createResetButtonClicked()));
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(resetAction);
    
    this->createOperationActionGroup = new QActionGroup(this);
    this->createOperationActionGroup->addAction(drawAction);
    this->createOperationActionGroup->addAction(transformAction);
    this->createOperationActionGroup->setExclusive(true);
    QObject::connect(this->createOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(createOperationActionTriggered(QAction*)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0, 0);
    layout->addWidget(drawToolButton);
    layout->addWidget(transformToolButton);
    layout->addSpacing(20);
    layout->addWidget(finishToolButton);
    layout->addWidget(undoToolButton);
    layout->addWidget(resetToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called when create border reset button clicked.
 */
void 
UserInputModeBordersWidget::createResetButtonClicked()
{
    this->inputModeBorders->createOperationReset();
}

/**
 * Called when create border undo button clicked.
 */
void 
UserInputModeBordersWidget::createUndoButtonClicked()
{
    this->inputModeBorders->createOperationUndo();
}

/**
 * Called when create border finish button clicked.
 */
void 
UserInputModeBordersWidget::createFinishButtonClicked()
{
    BorderDrawFinishDialog finishBorderDialog(this->inputModeBorders->borderBeingDrawnByOpenGL,
                                              this);
    if (finishBorderDialog.exec() == BorderDrawFinishDialog::Accepted) {
        this->inputModeBorders->createOperationFinish();
    }
}

/**
 * Called when a create mode button is clicked.
 * @param action
 *    Action that was triggered.
 */
void 
UserInputModeBordersWidget::createOperationActionTriggered(QAction* action)
{
    const int createModeInteger = action->data().toInt();
    const UserInputModeBorders::CreateOperation createOperation = 
        static_cast<UserInputModeBorders::CreateOperation>(createModeInteger);
    this->inputModeBorders->setCreateOperation(createOperation);
}

/**
 * @return The revise widget.
 */
QWidget* 
UserInputModeBordersWidget::createReviseOperationWidget()
{
    QAction* eraseAction = WuQtUtilities::createAction("Erase", 
                                                      "Erase the an end of the border by \n"
                                                      "dragging the mouse along the region\n"
                                                      "that is to be deleted", 
                                                      this);
    eraseAction->setCheckable(true);
    eraseAction->setData(static_cast<int>(UserInputModeBorders::REVISE_OPERATION_ERASE));
    QToolButton* eraseToolButton = new QToolButton();
    eraseToolButton->setDefaultAction(eraseAction);
    
    QAction* extendAction = WuQtUtilities::createAction("Extend", 
                                                      "Extend a border by dragging the mouse\n"
                                                      "starting at the end of a border and \n"
                                                      "until its new termination point.", 
                                                      this);
    extendAction->setCheckable(true);
    extendAction->setData(static_cast<int>(UserInputModeBorders::REVISE_OPERATION_EXTEND));
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
    replaceAction->setData(static_cast<int>(UserInputModeBorders::REVISE_OPERATION_REPLACE));
    QToolButton* replaceToolButton = new QToolButton();
    replaceToolButton->setDefaultAction(replaceAction);
    
    QAction* acceptAction = WuQtUtilities::createAction("Accept", 
                                                        "Accept revisons made to a border.",
                                                        this,
                                                        this,
                                                        SLOT(reviseAcceptButtonClicked()));
    QToolButton* acceptToolButton = new QToolButton();
    acceptToolButton->setDefaultAction(acceptAction);
    
    QAction* undoAction = WuQtUtilities::createAction("Undo", 
                                                      "Remove (undo) the last point\n"
                                                      "in the border revision.",
                                                      this,
                                                      this,
                                                      SLOT(reviseUndoButtonClicked()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(undoAction);
    
    QAction* resetAction = WuQtUtilities::createAction("Reset", 
                                                       "Remove all points in the border revision.", 
                                                       this,
                                                       this,
                                                       SLOT(reviseResetButtonClicked()));
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(resetAction);
    
    QAction* deleteAction = WuQtUtilities::createAction("Delete", 
                                                        "Delete a border by clicking\n"
                                                        "the mouse over the border",
                                                        this);
    deleteAction->setCheckable(true);
    deleteAction->setData(static_cast<int>(UserInputModeBorders::REVISE_OPERATION_DELETE));
    QToolButton* deleteToolButton = new QToolButton();
    deleteToolButton->setDefaultAction(deleteAction);
    
    QAction* reverseAction = WuQtUtilities::createAction("Reverse", 
                                                         "Reverse the order of the points in a\n"
                                                         "border by clicking the mouse on the border", 
                                                         this);
    reverseAction->setCheckable(true);
    reverseAction->setData(static_cast<int>(UserInputModeBorders::REVISE_OPERATION_REVERSE));
    QToolButton* reverseToolButton = new QToolButton();
    reverseToolButton->setDefaultAction(reverseAction);
    
    this->reviseOperationActionGroup = new QActionGroup(this);
    this->reviseOperationActionGroup->addAction(eraseAction);
    this->reviseOperationActionGroup->addAction(extendAction);
    this->reviseOperationActionGroup->addAction(replaceAction);
    this->reviseOperationActionGroup->addAction(deleteAction);
    this->reviseOperationActionGroup->addAction(reverseAction);
    this->reviseOperationActionGroup->setExclusive(true);
    QObject::connect(this->reviseOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(reviseOperationActionTriggered(QAction*)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0, 0);
    layout->addWidget(eraseToolButton);
    layout->addWidget(extendToolButton);
    layout->addWidget(replaceToolButton);
    layout->addSpacing(20);
    layout->addWidget(acceptToolButton);
    layout->addWidget(undoToolButton);
    layout->addWidget(resetToolButton);
    layout->addSpacing(20);
    layout->addWidget(deleteToolButton);
    layout->addWidget(reverseToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called when revise border reset button clicked.
 */
void 
UserInputModeBordersWidget::reviseResetButtonClicked()
{
    this->inputModeBorders->reviseOperationReset();
}

/**
 * Called when revise border undo button clicked.
 */
void 
UserInputModeBordersWidget::reviseUndoButtonClicked()
{
    this->inputModeBorders->reviseOperationUndo();
}

/**
 * Called when revise border accept button clicked.
 */
void 
UserInputModeBordersWidget::reviseAcceptButtonClicked()
{
    this->inputModeBorders->reviseOperationAccept();
}

/**
 * Called when a revise button is clicked.
 * @param action
 *    Action that was triggered.
 */
void 
UserInputModeBordersWidget::reviseOperationActionTriggered(QAction* action)
{
    const int reviseModeInteger = action->data().toInt();
    const UserInputModeBorders::ReviseOperation reviseOperation = 
    static_cast<UserInputModeBorders::ReviseOperation>(reviseModeInteger);
    this->inputModeBorders->setReviseOperation(reviseOperation);
}

/**
 * @return The select widget.
 */
QWidget* 
UserInputModeBordersWidget::createSelectOperationWidget()
{
    QAction* classAction = WuQtUtilities::createAction("Class", 
                                                        "Filter display of borders by\n"
                                                        "selection of border classes", 
                                                        this);
    classAction->setCheckable(true);
    classAction->setData(static_cast<int>(UserInputModeBorders::SELECT_CLASS));
    QToolButton* classToolButton = new QToolButton();
    classToolButton->setDefaultAction(classAction);
    
    QAction* nameAction = WuQtUtilities::createAction("Name", 
                                                      "Filter display of borders by \n"
                                                      "selection of border names", 
                                                      this);
    nameAction->setCheckable(true);
    nameAction->setData(static_cast<int>(UserInputModeBorders::SELECT_NAME));
    QToolButton* nameToolButton = new QToolButton();
    nameToolButton->setDefaultAction(nameAction);
    
    this->selectOperationActionGroup = new QActionGroup(this);
    this->selectOperationActionGroup->addAction(classAction);
    this->selectOperationActionGroup->addAction(nameAction);
    this->selectOperationActionGroup->setExclusive(true);
    QObject::connect(this->selectOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(selectOperationActionTriggered(QAction*)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0, 0);
    layout->addWidget(classToolButton);
    layout->addWidget(nameToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called when a select button is clicked.
 * @param action
 *    Action that was triggered.
 */
void 
UserInputModeBordersWidget::selectOperationActionTriggered(QAction* action)
{
    const int selectModeInteger = action->data().toInt();
    const UserInputModeBorders::SelectOperation selectOperation = 
    static_cast<UserInputModeBorders::SelectOperation>(selectModeInteger);
    this->inputModeBorders->setSelectOperation(selectOperation);
}

