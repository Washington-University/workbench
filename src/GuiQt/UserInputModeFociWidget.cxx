
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QToolButton>

#define __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__
#include "UserInputModeFociWidget.h"
#undef __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayPropertiesFoci.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "FociFile.h"
#include "FociProjectionDialog.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemVoxel.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "Surface.h"
#include "UserInputModeFoci.h"
#include "VolumeFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeFociWidget 
 * \brief Foci controls shown at bottom of toolbar
 */

/**
 * Constructor.
 * @param inputModeFoci
 *    Process of mouse input for foci
 * @param windowIndex
 *    Index of browser window
 * @param parent
 *    Parent widget
 */
UserInputModeFociWidget::UserInputModeFociWidget(UserInputModeFoci* inputModeFoci,
                                                 const int32_t windowIndex,
                                                 QWidget* parent)
: QWidget(parent),
  m_windowIndex(windowIndex)
{
    m_inputModeFoci = inputModeFoci;
    QLabel* nameLabel = new QLabel("Foci ");
    
    QWidget* modeWidget = createModeWidget();
    
    m_createOperationWidget = createCreateOperationWidget();
    
    m_editOperationWidget = createEditOperationWidget();
    
    m_taskOperationWidget = createTaskOperationWidget();
    
    m_operationStackedWidget = new QStackedWidget();
    m_operationStackedWidget->addWidget(m_createOperationWidget);
    m_operationStackedWidget->addWidget(m_editOperationWidget);
    m_operationStackedWidget->addWidget(m_taskOperationWidget);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(nameLabel);
    layout->addWidget(modeWidget);
    layout->addSpacing(10);
    layout->addWidget(m_operationStackedWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
UserInputModeFociWidget::~UserInputModeFociWidget()
{
    
}

/**
 * Update the contents of the widget.
 */
void
UserInputModeFociWidget::updateWidget()
{
    /*
     * Show the proper widget
     */
    switch (m_inputModeFoci->getMode()) {
        case UserInputModeFoci::MODE_CREATE:
            m_operationStackedWidget->setCurrentWidget(m_createOperationWidget);
//            setActionGroupByActionData(m_createOperationActionGroup,
//                                             m_inputModeFoci->getCreateOperation());
            break;
        case UserInputModeFoci::MODE_EDIT:
            m_operationStackedWidget->setCurrentWidget(m_editOperationWidget);
            setActionGroupByActionData(m_editOperationActionGroup,
                                             m_inputModeFoci->getEditOperation());
            break;
        case UserInputModeFoci::MODE_OPERATIONS:
            m_operationStackedWidget->setCurrentWidget(m_taskOperationWidget);
            break;
    }
    const int selectedModeInteger = (int)m_inputModeFoci->getMode();
    
    const int modeComboBoxIndex = m_modeComboBox->findData(selectedModeInteger);
    CaretAssert(modeComboBoxIndex >= 0);
    m_modeComboBox->blockSignals(true);
    m_modeComboBox->setCurrentIndex(modeComboBoxIndex);
    m_modeComboBox->blockSignals(false);
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
UserInputModeFociWidget::setActionGroupByActionData(QActionGroup* actionGroup,
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
UserInputModeFociWidget::createModeWidget()
{
    m_modeComboBox = new QComboBox();
    m_modeComboBox->addItem("Create", (int)UserInputModeFoci::MODE_CREATE);
    m_modeComboBox->addItem("Edit", (int)UserInputModeFoci::MODE_EDIT);
    m_modeComboBox->addItem("Tasks", (int)UserInputModeFoci::MODE_OPERATIONS);
    QObject::connect(m_modeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(modeComboBoxSelection(int)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
    layout->addWidget(m_modeComboBox);
    
    widget->setFixedWidth(widget->sizeHint().width());
    
    return widget;
}

/**
 * Called when a mode is selected from the mode combo box.
 * @param indx
 *   Index of item selected.
 */
void
UserInputModeFociWidget::modeComboBoxSelection(int indx)
{
    const int modeInteger = m_modeComboBox->itemData(indx).toInt();
    const UserInputModeFoci::Mode mode = (UserInputModeFoci::Mode)modeInteger;
    m_inputModeFoci->setMode(mode);
}

/**
 * @return The draw operation widget.
 */
QWidget*
UserInputModeFociWidget::createCreateOperationWidget()
{
    QAction* newFocusAction = WuQtUtilities::createAction("New...",
                                                          "Display the create focus window for entering new focus data\n",
                                                          this,
                                                          this,
                                                          SLOT(createNewFocusActionTriggered()));
    QToolButton* newFocusToolButton = new QToolButton();
    newFocusToolButton->setDefaultAction(newFocusAction);
    
    QAction* lastIdFocusAction = WuQtUtilities::createAction("Last ID",
                                                          "Display the create focus findow with position of last ID\n",
                                                          this,
                                                          this,
                                                          SLOT(createLastIdentificationFocusActionTriggered()));
    QToolButton* lastIdFocusToolButton = new QToolButton();
    lastIdFocusToolButton->setDefaultAction(lastIdFocusAction);
    
    m_createFocusMouseClickAction = WuQtUtilities::createAction("From Mouse-Click",
                                                             "Display the create focus findow with position mouse click\n",
                                                             this);
    m_createFocusMouseClickAction->setCheckable(true);
    m_createFocusMouseClickAction->setChecked(false);
    QToolButton* mouseClickFocusToolButton = new QToolButton();
    mouseClickFocusToolButton->setDefaultAction(m_createFocusMouseClickAction);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
    layout->addWidget(newFocusToolButton);
    layout->addSpacing(5);
    layout->addWidget(lastIdFocusToolButton);
    layout->addSpacing(10);
    layout->addWidget(mouseClickFocusToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * @return true if a mouse click should pop create focus window
 * in Create Mode.
 */
bool
UserInputModeFociWidget::isMouseClickCreateFocusEnabled() const
{
    return m_createFocusMouseClickAction->isChecked();
}


/**
 * Called when new focus button is triggered
 * @param action
 *     Action that was selected.
 */
void
UserInputModeFociWidget::createNewFocusActionTriggered()
{
    BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(m_windowIndex);
    if (browserWindow == NULL) {
        return;
    }
    BrowserTabContent* btc = browserWindow->getBrowserTabContent();
    if (btc == NULL) {
        return;
    }
    const int32_t browserTabIndex = btc->getTabNumber();
    
    DisplayPropertiesFoci* dpf = GuiManager::get()->getBrain()->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = dpf->getDisplayGroupForTab(btc->getTabNumber());
    dpf->setDisplayed(displayGroup,
                      browserTabIndex,
                      true);
    
    Focus* focus = new Focus();
    if (s_previousFocus == NULL) {
        s_previousFocus = new Focus();
    }
    *focus = *s_previousFocus;
    displayFocusCreationDialog(focus);
}

/**
 * Called when last ID focus button is triggered
 * @param action
 *     Action that was selected.
 */
void
UserInputModeFociWidget::createLastIdentificationFocusActionTriggered()
{
    Brain* brain = GuiManager::get()->getBrain();
    const IdentificationManager* idManager = brain->getIdentificationManager();
    const IdentificationItem* idItem = idManager->getLastIdentifiedItem();
    if (idItem != NULL) {
        const IdentificationItemSurfaceNode* nodeID = dynamic_cast<const IdentificationItemSurfaceNode*>(idItem);
        const IdentificationItemVoxel* voxelID = dynamic_cast<const IdentificationItemVoxel*>(idItem);
        
        if (nodeID != NULL) {
            if (nodeID->isValid()
                && (nodeID->isContralateral() == false)) {
                const Surface* surface = nodeID->getSurface();
                if (brain->isFileValid(surface)) {
                    CaretAssert(surface);
                    const StructureEnum::Enum structure = surface->getStructure();
                    const int32_t nodeIndex = nodeID->getNodeNumber();
                    
                    const AString focusName = ("Last ID "
                                               + StructureEnum::toGuiName(structure)
                                               + " Node "
                                               + AString::number(nodeIndex));
                    const float* xyz = surface->getCoordinate(nodeIndex);
                    
                    const AString comment = ("Created from "
                                             + focusName);
                    
                    displayFocusCreationDialog(focusName,
                                               xyz,
                                               comment);
                }
            }
        }
        else if (voxelID != NULL) {
            if (voxelID->isValid()) {
                const VolumeFile* volumeFile = voxelID->getVolumeFile();
                if (brain->isFileValid(volumeFile)) {
                    CaretAssert(volumeFile);
                    int64_t ijk[3];
                    voxelID->getVoxelIJK(ijk);
                    float xyz[3];
                    volumeFile->indexToSpace(ijk, xyz);
                    
                    const AString focusName = ("Last ID "
                                               + volumeFile->getFileNameNoPath()
                                               + " IJK ("
                                               + AString::fromNumbers(ijk, 3, ",")
                                               + ")");
                    
                    const AString comment = ("Created from "
                                             + focusName);
                    
                    displayFocusCreationDialog(focusName,
                                               xyz,
                                               comment);
                }
            }
        }
    }
}


/**
 * Display the focus creation dialog for creating a focus with
 * the given name, coordinate, and comment.
 * @param name
 *    Name for focus.
 * @param xyz
 *    Coordinate for focus.
 * @param comment
 *    Comment for focus.
 */
void
UserInputModeFociWidget::displayFocusCreationDialog(const AString& name,
                                                    const float xyz[3],
                                                    const AString& comment)
{
    if (s_previousFocus == NULL) {
        s_previousFocus = new Focus();
    }

    Focus* focus = new Focus(*s_previousFocus);
    focus->setName(name);
    CaretAssert(focus->getNumberOfProjections() > 0);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);

    displayFocusCreationDialog(focus);
}

/**
 * Display the focus creation dialog with the given focus.  After
 * calling this method, the caller must not manipulate the given
 * focus!!!
 *
 * @param newFocus
 *    Focus that will be created if the user presses the OK button.
 *    If the user cancels, this focus will be deleted.
 */
void
UserInputModeFociWidget::displayFocusCreationDialog(Focus* newFocus)
{
    CaretAssert(newFocus);
    
    FociPropertiesEditorDialog focusCreateDialog("Create Focus",
                                                 s_previousFociFile,
                                                 newFocus,
                                                 true,
                                                 this);
    if (focusCreateDialog.exec() == FociPropertiesEditorDialog::Accepted) {
        s_previousFociFile = focusCreateDialog.getSelectedFociFile();
        if (s_previousFocus == NULL) {
            s_previousFocus = new Focus();
        }
        focusCreateDialog.loadFromDialogIntoFocusData(s_previousFocus);
        s_previousFociFile->addFocus(newFocus);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    else {
        delete newFocus;
    }
}

/**
 * @return The edit widget.
 */
QWidget*
UserInputModeFociWidget::createEditOperationWidget()
{
    QAction* deleteAction = WuQtUtilities::createAction("Delete",
                                                        "Delete a border by clicking\n"
                                                        "the mouse over the border",
                                                        this);
    deleteAction->setCheckable(true);
    deleteAction->setData(static_cast<int>(UserInputModeFoci::EDIT_OPERATION_DELETE));
    QToolButton* deleteToolButton = new QToolButton();
    deleteToolButton->setDefaultAction(deleteAction);
    
    QAction* propertiesAction = WuQtUtilities::createAction("Properties",
                                                            "Edit the properties of a border by clicking\n"
                                                            "on a border which causes a border properties\n"
                                                            "editor to appear",
                                                            this);
    propertiesAction->setCheckable(true);
    propertiesAction->setData(static_cast<int>(UserInputModeFoci::EDIT_OPERATION_PROPERTIES));
    QToolButton* propertiesToolButton = new QToolButton();
    propertiesToolButton->setDefaultAction(propertiesAction);
    
    m_editOperationActionGroup = new QActionGroup(this);
    m_editOperationActionGroup->addAction(deleteAction);
    m_editOperationActionGroup->addAction(propertiesAction);
    m_editOperationActionGroup->setExclusive(true);
    QObject::connect(m_editOperationActionGroup, SIGNAL(triggered(QAction*)),
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
 * @return The task operation widget.
 */
QWidget*
UserInputModeFociWidget::createTaskOperationWidget()
{
    QAction* projectAction = WuQtUtilities::createAction("Project",
                                                         "Project the Foci",
                                                         this,
                                                         this,
                                                         SLOT(taskProjectFociTriggered()));
    
    QToolButton* projectToolButton = new QToolButton();
    projectToolButton->setDefaultAction(projectAction);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 0);
    layout->addWidget(projectToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called to project foci.
 */
void
UserInputModeFociWidget::taskProjectFociTriggered()
{
    FociProjectionDialog fpd(this);
    fpd.exec();
}


/**
 * Called when an edit operation button is selected.
 * @param action
 *     Action that was selected.
 */
void
UserInputModeFociWidget::editOperationActionTriggered(QAction* action)
{
    const int editModeInteger = action->data().toInt();
    const UserInputModeFoci::EditOperation editOperation =
    static_cast<UserInputModeFoci::EditOperation>(editModeInteger);
    m_inputModeFoci->setEditOperation(editOperation);
}

/**
 * Delete all static members to eliminate reported memory leaks.
 */
void
UserInputModeFociWidget::deleteStaticMembers()
{
    if (s_previousFocus != NULL) {
        delete s_previousFocus;
    }
    s_previousFocus = NULL;
}

