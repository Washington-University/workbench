
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
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
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
    QWidget* modeWidget = createModeWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(modeWidget);
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
        case UserInputModeFoci::MODE_CREATE_AT_ID:
            m_modeCreateLastIdRadioButton->setChecked(true);
            break;
        case UserInputModeFoci::MODE_DELETE:
            m_modeDeleteRadioButton->setChecked(true);
            break;
        case UserInputModeFoci::MODE_EDIT:
            m_modePropertiesRadioButton->setChecked(true);
            break;
    }
}

/**
 * @return The mode widget.
 */
QWidget*
UserInputModeFociWidget::createModeWidget()
{
    const AString newToolTipText = ("Click this button to display a dialog for creating a new focus. "
                                    + m_transformToolTipText);

    QToolButton* newFocusToolButton = new QToolButton();
    newFocusToolButton->setText("New Focus...");
    WuQtUtilities::setToolButtonStyleForQt5Mac(newFocusToolButton);
    newFocusToolButton->setToolTip(newToolTipText);
    QObject::connect(newFocusToolButton, &QToolButton::clicked,
                     this, &UserInputModeFociWidget::createNewFocusActionTriggered);
    
    const AString lastIDToolTipText = ("While this button is on, clicking the mouse on a surface "
                                       "will launch the create focus dialog initialized with coordinates "
                                       "of the selected surface vertex.  "
                                       + m_transformToolTipText);
    m_modeCreateLastIdRadioButton = new QRadioButton("Create Last ID");
    m_modeCreateLastIdRadioButton->setToolTip(lastIDToolTipText);
    
    const AString deleteToolTipText = ("While this button is on, clicking the mouse over a focus "
                                       "will delete the focus.  "
                                       + m_transformToolTipText);
    m_modeDeleteRadioButton = new QRadioButton("Delete");
    m_modeDeleteRadioButton->setToolTip(deleteToolTipText);
    
    const AString propertiesToolTipText = ("While this button is on, clicking the mouse over a focus "
                                           "displays a dialog for editing the focus' properties. "
                                           + m_transformToolTipText);
    m_modePropertiesRadioButton   = new QRadioButton("Edit Properties");
    m_modePropertiesRadioButton->setToolTip(propertiesToolTipText);
    
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(m_modeCreateLastIdRadioButton);
    buttonGroup->addButton(m_modeDeleteRadioButton);
    buttonGroup->addButton(m_modePropertiesRadioButton);
    QObject::connect(buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
                     this, &UserInputModeFociWidget::modeRadioButtonClicked);
    
    QLabel* mouseLabel = new QLabel("Mouse Mode:");
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 2);
    layout->addWidget(mouseLabel);
    layout->addWidget(m_modeCreateLastIdRadioButton);
    layout->addWidget(m_modeDeleteRadioButton);
    layout->addWidget(m_modePropertiesRadioButton);
    layout->addSpacing(8);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addSpacing(8);
    layout->addWidget(newFocusToolButton);

    widget->setFixedWidth(widget->sizeHint().width());
    
    return widget;
}

/**
 * Called when a mode radio button is selected..
 * @param button
 *   button selected.
 */
void
UserInputModeFociWidget::modeRadioButtonClicked(QAbstractButton* button)
{
    if (button == m_modeCreateLastIdRadioButton) {
        m_inputModeFoci->setMode(UserInputModeFoci::MODE_CREATE_AT_ID);
    }
    else if (button == m_modeDeleteRadioButton) {
        m_inputModeFoci->setMode(UserInputModeFoci::MODE_DELETE);
    }
    else if (button == m_modePropertiesRadioButton) {
        m_inputModeFoci->setMode(UserInputModeFoci::MODE_EDIT);
    }
    else {
        CaretAssert(0);
    }
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
                    const Vector3D xyz(voxelID->getVoxelXYZ());
                    const AString focusName = ("Last ID "
                                               + cmdf->getFileNameNoPath()
                                               + " XYZ ("
                                               + AString::fromNumbers(xyz, 3, ",")
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


