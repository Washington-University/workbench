
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __USER_INPUT_MODE_VOLUME_EDIT_WIDGET_DECLARE__
#include "UserInputModeVolumeEditWidget.h"
#undef __USER_INPUT_MODE_VOLUME_EDIT_WIDGET_DECLARE__

#include <QAction>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "VolumeFileCreateDialog.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::UserInputModeVolumeEditWidget 
 * \brief User input widget for volume editing.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param inputModeVolumeEdit
 *    Process of mouse input for volume editing
 * @param windowIndex
 *    Index of browser window
 * @param parent
 *    Parent widget
 */
UserInputModeVolumeEditWidget::UserInputModeVolumeEditWidget(UserInputModeVolumeEdit* inputModeVolumeEdit,
                                                             const int32_t windowIndex,
                                                             QWidget* parent)
: QWidget(parent),
m_inputModeVolumeEdit(inputModeVolumeEdit),
m_windowIndex(windowIndex)
{
    CaretAssert(inputModeVolumeEdit);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(createSelectionToolBar());
    layout->addWidget(createModeToolBar());
    setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
UserInputModeVolumeEditWidget::~UserInputModeVolumeEditWidget()
{
}

/**
 * Update the widget.
 */
void
UserInputModeVolumeEditWidget::updateWidget()
{
    
}

/**
 * @return Create and return the selection toolbar.
 */
QWidget*
UserInputModeVolumeEditWidget::createSelectionToolBar()
{
    QLabel* volumeLabel = new QLabel("Volume:");
    
    m_newFileToolButton = new QToolButton();
    m_newFileToolButton->setDefaultAction(WuQtUtilities::createAction("New",
                                                                    "Create a new volume file",
                                                                    this,
                                                                    this, SLOT(newFileActionTriggered())));
    
    
    QToolButton* lockFileToolButton = new QToolButton();
    lockFileToolButton->setDefaultAction(WuQtUtilities::createAction("Lock",
                                                                    "Lock/unlock volume file to disallow/allow editing",
                                                                    this,
                                                                    this, SLOT(lockFileActionTriggered())));
    
    
    QLabel* editLabel = new QLabel("Edit:");
    
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(WuQtUtilities::createAction("Undo",
                                                                    "Undo the last volume edit",
                                                                    this,
                                                                    this, SLOT(undoActionTriggered())));
    
    
    QToolButton* redoToolButton = new QToolButton();
    redoToolButton->setDefaultAction(WuQtUtilities::createAction("Redo",
                                                                    "Redo (or is it undo) the last undo",
                                                                    this,
                                                                    this, SLOT(redoActionTriggered())));
    
    
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(WuQtUtilities::createAction("Reset",
                                                                    "Reset all edting of the volume",
                                                                    this,
                                                                    this, SLOT(resetActionTriggered())));
    
    QLabel* brushSizeLabel = new QLabel("Brush Size:");
    const int maxBrushSize = 999;
    m_xBrushSizeSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                        maxBrushSize,
                                                                        1,
                                                                        this,
                                                                        SLOT(xBrushSizeValueChanged(int)));
    m_yBrushSizeSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                        maxBrushSize,
                                                                        1,
                                                                        this,
                                                                        SLOT(yBrushSizeValueChanged(int)));
    m_zBrushSizeSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                        maxBrushSize,
                                                                        1,
                                                                        this,
                                                                        SLOT(zBrushSizeValueChanged(int)));
    
    QLabel* voxelValueLabel = new QLabel("Value:");
    m_voxelValueSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-1000.0,
                                                                                         1000.0,
                                                                                         1.0, 1,
                                                                                         this,
                                                                                         SLOT(voxelValueChanged(double)));
    m_voxelValueSpinBox->setValue(1.0);

    
    const int SPACE = 10;
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(volumeLabel);
    layout->addWidget(m_newFileToolButton);
    layout->addWidget(lockFileToolButton);
    layout->addSpacing(SPACE);
    layout->addWidget(editLabel);
    layout->addWidget(undoToolButton);
    layout->addWidget(redoToolButton);
    layout->addWidget(resetToolButton);
    layout->addSpacing(SPACE);
    layout->addWidget(brushSizeLabel);
    layout->addWidget(m_xBrushSizeSpinBox);
    layout->addWidget(m_yBrushSizeSpinBox);
    layout->addWidget(m_zBrushSizeSpinBox);
    layout->addSpacing(SPACE);
    layout->addWidget(voxelValueLabel);
    layout->addWidget(m_voxelValueSpinBox);
    layout->addStretch();
    
    return widget;
}

/**
 * @return Create and return the mode toolbar.
 */
QWidget*
UserInputModeVolumeEditWidget::createModeToolBar()
{
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    
    QLabel* modeLabel = new QLabel("Mode:");
    layout->addWidget(modeLabel);
    
    m_volumeEditModeActionGroup = new QActionGroup(this);
    m_volumeEditModeActionGroup->setExclusive(true);
    QObject::connect(m_volumeEditModeActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(editingModeActionTriggered(QAction*)));
    
    std::vector<VolumeEditingModeEnum::Enum> editModes;
    VolumeEditingModeEnum::getAllEnums(editModes);
    
    bool firstActionFlag = true;
    for (std::vector<VolumeEditingModeEnum::Enum>::iterator iter = editModes.begin();
         iter != editModes.end();
         iter++) {
        const VolumeEditingModeEnum::Enum mode = *iter;
        
        const int     modeInt  = VolumeEditingModeEnum::toIntegerCode(mode);
        const AString modeName = VolumeEditingModeEnum::toGuiName(mode);
        const AString toolTip  = VolumeEditingModeEnum::toToolTip(mode);
        
        QAction* action = new QAction(modeName,
                                      this);
        action->setData(modeInt);
        action->setToolTip(toolTip);
        action->setCheckable(true);
        if (firstActionFlag) {
            firstActionFlag = false;
            action->setChecked(true);
        }
        
        m_volumeEditModeActionGroup->addAction(action);
        
        QToolButton* toolButton = new QToolButton();
        toolButton->setDefaultAction(action);

        layout->addWidget(toolButton);
    }
    
    layout->addStretch();
    
    return widget;
}

/**
 * Get the editing parameters from the toolbar.
 *
 * @param editingModeOut
 *     Output containing the selected editing mode.
 * @param brushSizeOut
 *     Brush IJK/XYZ sizes.
 * @param valueOut
 *     Value assigned to modified voxels.
 */
void
UserInputModeVolumeEditWidget::getEditingParameters(VolumeEditingModeEnum::Enum& editingModeOut,
                                                    int32_t brushSizesOut[3],
                                                    float& valueOut) const
{
    editingModeOut   = getEditingMode();
    brushSizesOut[0] = m_xBrushSizeSpinBox->value();
    brushSizesOut[1] = m_yBrushSizeSpinBox->value();
    brushSizesOut[2] = m_zBrushSizeSpinBox->value();
    valueOut         = m_voxelValueSpinBox->value();
}


/**
 * @return The volume editing mode.
 */
VolumeEditingModeEnum::Enum
UserInputModeVolumeEditWidget::getEditingMode() const
{
    QAction* action = m_volumeEditModeActionGroup->checkedAction();
    CaretAssert(action);
    
    const int modeInt = action->data().toInt();
    
    bool validFlag = false;
    const VolumeEditingModeEnum::Enum editMode = VolumeEditingModeEnum::fromIntegerCode(modeInt,
                                                                                        &validFlag);
    CaretAssert(validFlag);
    
    return editMode;
}



/**
 * Called when new volume file button is clicked.
 */
void
UserInputModeVolumeEditWidget::newFileActionTriggered()
{
    VolumeFileCreateDialog newVolumeDialog(m_newFileToolButton);
    if (newVolumeDialog.exec() == VolumeFileCreateDialog::Accepted) {
        VolumeFile* vf = newVolumeDialog.getVolumeFile();
        
        EventDataFileAdd addFileEvent(vf);
        EventManager::get()->sendEvent(addFileEvent.getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
}

/**
 * Called when lock button is clicked.
 */
void
UserInputModeVolumeEditWidget::lockFileActionTriggered()
{
    
}

/**
 * Called when undo button is clicked.
 */
void
UserInputModeVolumeEditWidget::undoActionTriggered()
{
    
}

/**
 * Called when redo button is clicked.
 */
void
UserInputModeVolumeEditWidget::redoActionTriggered()
{
    
}

/**
 * Called when reset button is clicked.
 */
void
UserInputModeVolumeEditWidget::resetActionTriggered()
{
    
}

/**
 * Called when X-size brush value is changed.
 */
void
UserInputModeVolumeEditWidget::xBrushSizeValueChanged(int)
{
    
}

/**
 * Called when X-size brush value is changed.
 */
void
UserInputModeVolumeEditWidget::yBrushSizeValueChanged(int)
{
    
}

/**
 * Called when X-size brush value is changed.
 */
void
UserInputModeVolumeEditWidget::zBrushSizeValueChanged(int)
{
    
}

/**
 * Called when voxel value is changed.
 */
void
UserInputModeVolumeEditWidget::voxelValueChanged(double)
{
    
}

/**
 * Called when an editing mode is selected.
 *
 * @param action
 *     Editing action that was selected.
 */
void
UserInputModeVolumeEditWidget::editingModeActionTriggered(QAction* action)
{
    CaretAssert(action);
    
    const int modeInt = action->data().toInt();
    
    bool validFlag = false;
    const VolumeEditingModeEnum::Enum editMode = VolumeEditingModeEnum::fromIntegerCode(modeInt,
                                                                                        &validFlag);
    CaretAssert(validFlag);
}

