
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
#include <QButtonGroup>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QStackedWidget>
#include <QToolButton>

#define __USER_INPUT_MODE_IMAGE_WIDGET_DECLARE__
#include "UserInputModeImageWidget.h"
#undef __USER_INPUT_MODE_IMAGE_WIDGET_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ControlPointFile.h"
#include "DisplayPropertiesFoci.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FociFile.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GuiManager.h"
#include "ImageFile.h"
#include "ImageFileConvertToVolumeFileDialog.h"
#include "Matrix4x4.h"
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
    
    /*
     * Add button
     */
    const AString addToolTipText = ("Click the mouse over an image and volume slice "
                                    "to add a control point."
                                    + m_transformToolTipText);
    
    m_addControlPointRadioButton = new QRadioButton("Add");
    m_addControlPointRadioButton->setChecked(true);
    m_addControlPointRadioButton->setToolTip(addToolTipText);
    
    /*
     * Delete button
     */
    const AString deleteToolTipText = ("Delete a control point by clicking the mouse over the control point."
                                       + m_transformToolTipText);
    m_deleteControlPointRadioButton = new QRadioButton("Delete");
    
    QButtonGroup* addDeleteButtonGroup = new QButtonGroup(this);
    addDeleteButtonGroup->addButton(m_addControlPointRadioButton);
    addDeleteButtonGroup->addButton(m_deleteControlPointRadioButton);
    QObject::connect(addDeleteButtonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
                     this, &UserInputModeImageWidget::addDeleteRadioButtonClicked);
    
    
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
    m_convertToolButton = new QToolButton();
    m_convertToolButton->setDefaultAction(convertAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_convertToolButton);
    
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
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_deleteAllToolButton);
    
    QLabel* mouseModeLabel = new QLabel("Control Points:");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(mouseModeLabel);
    layout->addWidget(m_addControlPointRadioButton);
    layout->addWidget(m_deleteControlPointRadioButton);
    layout->addSpacing(10);
    layout->addWidget(m_deleteAllToolButton);
    layout->addSpacing(12);
    layout->addWidget(m_convertToolButton);
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
    switch (m_inputModeImage->getEditOperation()) {
        case UserInputModeImage::EDIT_OPERATION_ADD:
            m_addControlPointRadioButton->setChecked(true);
            break;
        case UserInputModeImage::EDIT_OPERATION_DELETE:
            m_deleteControlPointRadioButton->setChecked(true);
            break;
    }
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
        ControlPointFile* controlPointFile = imageFile->getControlPointFile();
        AString errorMessage;
        if ( ! controlPointFile->updateLandmarkTransformationMatrix(errorMessage)) {
            WuQMessageBox::errorOk(m_convertToolButton,
                                   errorMessage);
            return;
        }
        
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
 * Called when a add/delete  radio button is selected..
 * @param button
 *   button selected.
 */
void
UserInputModeImageWidget::addDeleteRadioButtonClicked(QAbstractButton* button)
{
    if (button == m_addControlPointRadioButton) {
        m_inputModeImage->setEditOperation(UserInputModeImage::EDIT_OPERATION_ADD);
    }
    else if (button == m_deleteControlPointRadioButton) {
        m_inputModeImage->setEditOperation(UserInputModeImage::EDIT_OPERATION_DELETE);
    }
    else {
        CaretAssert(0);
    }
}

