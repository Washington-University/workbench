
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
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QToolButton>

#define __USER_INPUT_MODE_IMAGE_WIDGET_DECLARE__
#include "UserInputModeImageWidget.h"
#undef __USER_INPUT_MODE_IMAGE_WIDGET_DECLARE__

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
#include "ImageFileConvertToVolumeFileDialog.h"
#include "SelectionManager.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemVoxel.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "Surface.h"
#include "UserInputModeImage.h"
#include "VolumeFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeImageWidget 
 * \brief Foci controls shown at bottom of toolbar
 */

/**
 * Constructor.
 * @param inputModeImage
 *    Process of mouse input for image
 * @param windowIndex
 *    Index of browser window
 * @param parent
 *    Parent widget
 */
UserInputModeImageWidget::UserInputModeImageWidget(UserInputModeImage* inputModeImage,
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
    
    m_inputModeImage = inputModeImage;
    QLabel* nameLabel = new QLabel("Image Control Points ");
    
    m_editOperationWidget = createEditOperationWidget();
    
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(nameLabel);
    layout->addSpacing(10);
    layout->addWidget(m_editOperationWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
UserInputModeImageWidget::~UserInputModeImageWidget()
{
    
}

/**
 * Update the contents of the widget.
 */
void
UserInputModeImageWidget::updateWidget()
{
    setActionGroupByActionData(m_editOperationActionGroup,
                               (int)m_inputModeImage->getEditOperation());
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
UserInputModeImageWidget::setActionGroupByActionData(QActionGroup* actionGroup,
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
 * @return The edit widget.
 */
QWidget*
UserInputModeImageWidget::createEditOperationWidget()
{
    /*
     * Add button
     */
    const AString addToolTipText = ("Click the mouse over an image and volume slice "
                                    "to add a control point."
                                    + m_transformToolTipText);
    QAction* addAction = WuQtUtilities::createAction("Add",
                                                     WuQtUtilities::createWordWrappedToolTipText(addToolTipText),
                                                     this);
    addAction->setCheckable(true);
    addAction->setData(static_cast<int>(UserInputModeImage::EDIT_OPERATION_ADD));
    QToolButton* addToolButton = new QToolButton();
    addToolButton->setDefaultAction(addAction);
    
    /*
     * Delete button
     */
    const AString deleteToolTipText = ("Delete a control point by clicking the mouse over the control point."
                               + m_transformToolTipText);
    QAction* deleteAction = WuQtUtilities::createAction("Delete",
                                                        WuQtUtilities::createWordWrappedToolTipText(deleteToolTipText),
                                                        this);
    deleteAction->setCheckable(true);
    deleteAction->setData(static_cast<int>(UserInputModeImage::EDIT_OPERATION_DELETE));
    QToolButton* deleteToolButton = new QToolButton();
    deleteToolButton->setDefaultAction(deleteAction);
    
    /*
     * Action group to make add/delete actions mutually exclusive
     */
    m_editOperationActionGroup = new QActionGroup(this);
    m_editOperationActionGroup->addAction(deleteAction);
    m_editOperationActionGroup->addAction(addAction);
    m_editOperationActionGroup->setExclusive(true);
    QObject::connect(m_editOperationActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(editOperationActionTriggered(QAction*)));
    
    /*
     * Convert button
     */
    const AString convertToolTipText = ("Convert image to volume");
    QAction* convertAction = WuQtUtilities::createAction("Convert...",
                                                           WuQtUtilities::createWordWrappedToolTipText(convertToolTipText),
                                                           this,
                                                           this,
                                                           SLOT(convertActionTriggered()));
    convertAction->setCheckable(false);
    QToolButton* convertToolButton = new QToolButton();
    convertToolButton->setDefaultAction(convertAction);
    
    /*
     * Delete all button
     */
    const AString deleteAllToolTipText = ("Delete all control points");
    QAction* deleteAllAction = WuQtUtilities::createAction("Delete All",
                                                           WuQtUtilities::createWordWrappedToolTipText(deleteAllToolTipText),
                                                           this,
                                                           this,
                                                           SLOT(deleteAllActionTriggered()));
    deleteAllAction->setCheckable(false);
    m_deleteAllToolButton = new QToolButton();
    m_deleteAllToolButton->setDefaultAction(deleteAllAction);

    
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(addToolButton);
    layout->addWidget(deleteToolButton);
    layout->addSpacing(15);
    layout->addWidget(m_deleteAllToolButton);
    layout->addSpacing(35);
    layout->addWidget(convertToolButton);
    layout->addStretch();
    
//    widget->setFixedWidth(widget->sizeHint().width());
    return widget;
}

/**
 * Called when convert action is triggered.
 */
void
UserInputModeImageWidget::convertActionTriggered()
{
    ImageFile* imageFile = m_inputModeImage->getImageFile();
    const int32_t tabIndex = m_inputModeImage->getTabIndex();
    if ((imageFile != NULL)
        && (tabIndex >= 0)) {
        ImageFileConvertToVolumeFileDialog convertDialog(this,
                                                         tabIndex,
                                                         imageFile);
        convertDialog.exec();
    }
}

/**
 * Called when convert action is triggered.
 */
void
UserInputModeImageWidget::deleteAllActionTriggered()
{
    if (WuQMessageBox::warningOkCancel(m_deleteAllToolButton,
                                       "Delete all control points?")) {
        m_inputModeImage->deleteAllControlPoints();
    }
}


/**
 * Called when an edit operation button is selected.
 * @param action
 *     Action that was selected.
 */
void
UserInputModeImageWidget::editOperationActionTriggered(QAction* action)
{
    const int editModeInteger = action->data().toInt();
    const UserInputModeImage::EditOperation editOperation = static_cast<UserInputModeImage::EditOperation>(editModeInteger);
    m_inputModeImage->setEditOperation(editOperation);
}

