
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

#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QGridLayout>
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
#include "EventUserInterfaceUpdate.h"
#include "FociFile.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GuiManager.h"
#include "SelectionManager.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
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
    m_transformToolTipText = ("\n\n"
                              "At any time, the view of the surface may be changed by\n"
                              "  PAN:    Move the mouse with the left mouse button down while "
                              "holding down the Shift key.\n"
                              "  ROTATE: Move the mouse with the left mouse button down.\n"
                              "  ZOOM:   Move the mouse with the left mouse button down while "
                              "holding down the Ctrl key (Apple key on Macs)."
                              );
    
    m_inputModeFoci = inputModeFoci;
    QLabel* nameLabel = new QLabel("Foci ");
    
    QWidget* modeWidget = createModeWidget();
    
    m_createOperationWidget = createCreateOperationWidget();
    
    m_editOperationWidget = createEditOperationWidget();
    
    m_taskOperationWidget = createTaskOperationWidget();
    
    m_operationStackedWidget = new QStackedWidget();
    m_operationStackedWidget->addWidget(m_createOperationWidget);
    m_operationStackedWidget->addWidget(m_editOperationWidget);
    //m_operationStackedWidget->addWidget(m_taskOperationWidget);
    
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->setColumnStretch(0,   0);
    layout->setColumnStretch(1,   0);
    layout->setColumnStretch(2, 100);
    int32_t row(0);
    layout->addWidget(nameLabel,
                      row, 0);
    layout->addWidget(modeWidget,
                      row, 1);
    row++;
    layout->addWidget(m_operationStackedWidget,
                      row, 0, 1, 3, Qt::AlignLeft);
    row++;
    layout->setRowStretch(row, 100);
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
//    m_modeComboBox->addItem("Tasks", (int)UserInputModeFoci::MODE_OPERATIONS);
    QObject::connect(m_modeComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(modeComboBoxSelection(int)));
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
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
    const AString newToolTipText = ("Press this button to display a dialog for creating a new focus. "
                            "If the mouse is clicked over a model, the dialog for creating a focus is "
                            "displayed with the focus' coordinates set to the stereotaxic coordinates at "
                            "the location of the mouse click."
                            + m_transformToolTipText);
    QAction* newFocusAction = WuQtUtilities::createAction("New...",
                                                          WuQtUtilities::createWordWrappedToolTipText(newToolTipText),
                                                          this,
                                                          this,
                                                          SLOT(createNewFocusActionTriggered()));
    QToolButton* newFocusToolButton = new QToolButton();
    newFocusToolButton->setDefaultAction(newFocusAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(newFocusToolButton);
    
    const AString lastIDToolTipText = ("Press this button to display a dialog for creating a new focus "
                                       "with the focus' coordinates set to the stereotaxic location of the "
                                       "last identification operation.  While in focus mode, an identification "
                                       "is performed by holding down the Shift key and clicking the mouse."
                                       + m_transformToolTipText);
    QAction* lastIdFocusAction = WuQtUtilities::createAction("Last ID",
                                                          WuQtUtilities::createWordWrappedToolTipText(lastIDToolTipText),
                                                          this,
                                                          this,
                                                          SLOT(createLastIdentificationFocusActionTriggered()));
    QToolButton* lastIdFocusToolButton = new QToolButton();
    lastIdFocusToolButton->setDefaultAction(lastIdFocusAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(lastIdFocusToolButton);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(newFocusToolButton);
    layout->addSpacing(5);
    layout->addWidget(lastIdFocusToolButton);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
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
    
    FociPropertiesEditorDialog::createFocus(new Focus(),
                                            btc,
                                            this);
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
    const SelectionManager* idManager = brain->getSelectionManager();
    const SelectionItem* idItem = idManager->getLastSelectedItem();
    if (idItem != NULL) {
        const SelectionItemSurfaceNode* nodeID = dynamic_cast<const SelectionItemSurfaceNode*>(idItem);
        const SelectionItemVoxel* voxelID = dynamic_cast<const SelectionItemVoxel*>(idItem);
        
        BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(m_windowIndex);
        BrowserTabContent* browserTabContent = NULL;
        if (browserWindow != NULL) {
            browserTabContent = browserWindow->getBrowserTabContent();
        }
        
        if (nodeID != NULL) {
            if (nodeID->isValid()) {
                const Surface* idSurface = nodeID->getSurface();
                if (brain->isFileValid(idSurface)) {
                    CaretAssert(idSurface);
                    const StructureEnum::Enum structure = idSurface->getStructure();
                    const Surface* surface = brain->getPrimaryAnatomicalSurfaceForStructure(structure);
                    if (surface != NULL) {
                        const int32_t nodeIndex = nodeID->getNodeNumber();
                        
                        const AString focusName = ("Last ID "
                                                   + StructureEnum::toGuiName(structure)
                                                   + " Node "
                                                   + AString::number(nodeIndex));
                        const float* xyz = surface->getCoordinate(nodeIndex);
                        
                        const AString comment = ("Created from "
                                                 + focusName);
                        
                        Focus* focus = new Focus();
                        focus->setName(focusName);
                        focus->getProjection(0)->setStereotaxicXYZ(xyz);
                        focus->setComment(comment);
                        FociPropertiesEditorDialog::createFocus(focus,
                                                                browserTabContent,
                                                                this);
                    }
                    else {
                        WuQMessageBox::errorOk(this,
                                               ("No anatomical surface found for "
                                                + StructureEnum::toGuiName(structure)));
                    }
                }
            }
        }
        else if (voxelID != NULL) {
            if (voxelID->isValid()) {
                const VolumeMappableInterface* volumeFile = voxelID->getVolumeFile();
                const CaretMappableDataFile* cmdf = dynamic_cast<const CaretMappableDataFile*>(volumeFile);
                if (brain->isFileValid(cmdf)) {
                    CaretAssert(volumeFile);
                    int64_t ijk[3];
                    voxelID->getVoxelIJK(ijk);
                    float xyz[3];
                    volumeFile->indexToSpace(ijk, xyz);
                    
                    const AString focusName = ("Last ID "
                                               + cmdf->getFileNameNoPath()
                                               + " IJK ("
                                               + AString::fromNumbers(ijk, 3, ",")
                                               + ")");
                    
                    const AString comment = ("Created from "
                                             + focusName);
                    
                    Focus* focus = new Focus();
                    focus->setName(focusName);
                    focus->getProjection(0)->setStereotaxicXYZ(xyz);
                    focus->setComment(comment);
                    FociPropertiesEditorDialog::createFocus(focus,
                                                            browserTabContent,
                                                            this);
                }
            }
        }
    }
}

/**
 * @return The edit widget.
 */
QWidget*
UserInputModeFociWidget::createEditOperationWidget()
{
    const AString deleteToolTipText = ("Delete a focus by clicking the mouse over the focus."
                               + m_transformToolTipText);
    QAction* deleteAction = WuQtUtilities::createAction("Delete",
                                                        WuQtUtilities::createWordWrappedToolTipText(deleteToolTipText),
                                                        this);
    deleteAction->setCheckable(true);
    deleteAction->setData(static_cast<int>(UserInputModeFoci::EDIT_OPERATION_DELETE));
    QToolButton* deleteToolButton = new QToolButton();
    deleteToolButton->setDefaultAction(deleteAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(deleteToolButton);
    
    const AString propertiesToolTipText = ("Click the mouse over a focus to display a dialog "
                                           "for editing the focus' properties."
                                           + m_transformToolTipText);
    QAction* propertiesAction = WuQtUtilities::createAction("Properties",
                                                            WuQtUtilities::createWordWrappedToolTipText(propertiesToolTipText),
                                                            this);
    propertiesAction->setCheckable(true);
    propertiesAction->setData(static_cast<int>(UserInputModeFoci::EDIT_OPERATION_PROPERTIES));
    QToolButton* propertiesToolButton = new QToolButton();
    propertiesToolButton->setDefaultAction(propertiesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(propertiesToolButton);
    
    m_editOperationActionGroup = new QActionGroup(this);
    m_editOperationActionGroup->addAction(deleteAction);
    m_editOperationActionGroup->addAction(propertiesAction);
    m_editOperationActionGroup->setExclusive(true);
    QObject::connect(m_editOperationActionGroup, SIGNAL(triggered(QAction*)),
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
 * @return The task operation widget.
 */
QWidget*
UserInputModeFociWidget::createTaskOperationWidget()
{
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    
    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
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

