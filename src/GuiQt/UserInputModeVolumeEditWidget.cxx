
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
#include "CaretLogger.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabel.h"
#include "GiftiLabelTableEditor.h"
#include "GuiManager.h"
#include "VolumeFile.h"
#include "VolumeFileEditorDelegate.h"
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
    bool isValid = false;
    
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfo(volumeEditInfo)) {
        m_lockAction->setChecked(volumeEditInfo.m_volumeFileEditorDelegate->isLocked(volumeEditInfo.m_mapIndex));
        
        if (volumeEditInfo.m_volumeFile != NULL) {
            if (volumeEditInfo.m_volumeFile->isMappedWithLabelTable()) {
                m_voxelLabelValueToolButton->setVisible(true);
                m_voxelFloatValueSpinBox->setVisible(false);

                AString buttonText = m_voxelLabelValueAction->text();
                GiftiLabelTable* labelTable = volumeEditInfo.m_volumeFile->getMapLabelTable(volumeEditInfo.m_mapIndex);
                if (labelTable != NULL) {
                    GiftiLabel* label = labelTable->getLabel(buttonText);
                    
                    /*
                     * Is there a label in the label table whose name
                     * matches the name in the button's actoin?
                     */
                    if (label == NULL) {
                        /*
                         * Find the first label that is not the unassigned
                         * label
                         */
                        const std::set<int32_t> keys = labelTable->getKeys();
                        for (std::set<int32_t>::iterator iter = keys.begin();
                             iter != keys.end();
                             iter++) {
                            const int32_t key = *iter;
                            if (key != labelTable->getUnassignedLabelKey()) {
                                GiftiLabel* keyLabel = labelTable->getLabel(key);
                                if (keyLabel != NULL) {
                                    buttonText = keyLabel->getName();
                                    break;
                                }
                            }
                        }
                    }
                }
                
                m_voxelLabelValueAction->setText(buttonText);
                
                isValid = true;
            }
            else if (volumeEditInfo.m_volumeFile->isMappedWithPalette()) {
                m_voxelLabelValueToolButton->setVisible(false);
                m_voxelFloatValueSpinBox->setVisible(true);
                isValid = true;
            }
        }
    }
    
    this->setEnabled(isValid);
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
    
    
    m_lockAction = WuQtUtilities::createAction("Lock",
                                               "Lock/unlock volume file to disallow/allow editing",
                                               this,
                                               this, SLOT(lockFileActionTriggered()));
    m_lockAction->setCheckable(true);
    QToolButton* lockFileToolButton = new QToolButton();
    lockFileToolButton->setDefaultAction(m_lockAction);
    
    
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
    
    m_voxelValueLabel = new QLabel("Value:");
    m_voxelFloatValueSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-1000.0,
                                                                                         1000.0,
                                                                                         1.0, 1,
                                                                                         this,
                                                                                         SLOT(voxelValueChanged(double)));
    m_voxelFloatValueSpinBox->setValue(1.0);

    m_voxelLabelValueAction = WuQtUtilities::createAction("XXX",
                                                          "Choose Label for Voxels",
                                                          this,
                                                          this, SLOT(labelValueActionTriggered()));
    m_voxelLabelValueToolButton = new QToolButton();
    m_voxelLabelValueToolButton->setDefaultAction(m_voxelLabelValueAction);
    
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
    layout->addWidget(m_voxelValueLabel);
    layout->addWidget(m_voxelFloatValueSpinBox);
    layout->addWidget(m_voxelLabelValueToolButton);
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
 * @param floatValueOut
 *     Float value for palette mapped volume files.
 * @param labelNameOut
 *     Label name for label mapped volume files.
 */
void
UserInputModeVolumeEditWidget::getEditingParameters(VolumeEditingModeEnum::Enum& editingModeOut,
                                                    int32_t brushSizesOut[3],
                                                    float& floatValueOut,
                                                    AString& labelNameOut) const
{
    editingModeOut   = getEditingMode();
    brushSizesOut[0] = m_xBrushSizeSpinBox->value();
    brushSizesOut[1] = m_yBrushSizeSpinBox->value();
    brushSizesOut[2] = m_zBrushSizeSpinBox->value();
    floatValueOut    = m_voxelFloatValueSpinBox->value();
    labelNameOut     = m_voxelLabelValueAction->text();
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
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfo(volumeEditInfo)) {
        volumeEditInfo.m_volumeFileEditorDelegate->setLocked(volumeEditInfo.m_mapIndex,
                                                             m_lockAction->isChecked());
    }
}

/**
 * Called when undo button is clicked.
 */
void
UserInputModeVolumeEditWidget::undoActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfo(volumeEditInfo)) {
        volumeEditInfo.m_volumeFileEditorDelegate->undo(volumeEditInfo.m_mapIndex);
        m_inputModeVolumeEdit->updateGraphicsAfterEditing(volumeEditInfo.m_volumeFile,
                                                          volumeEditInfo.m_mapIndex);
    }
}

/**
 * Called when redo button is clicked.
 */
void
UserInputModeVolumeEditWidget::redoActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfo(volumeEditInfo)) {
        volumeEditInfo.m_volumeFileEditorDelegate->redo(volumeEditInfo.m_mapIndex);
        m_inputModeVolumeEdit->updateGraphicsAfterEditing(volumeEditInfo.m_volumeFile,
                                                          volumeEditInfo.m_mapIndex);
    }
}

/**
 * Called when reset button is clicked.
 */
void
UserInputModeVolumeEditWidget::resetActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfo(volumeEditInfo)) {
        volumeEditInfo.m_volumeFileEditorDelegate->reset(volumeEditInfo.m_mapIndex);
        m_inputModeVolumeEdit->updateGraphicsAfterEditing(volumeEditInfo.m_volumeFile,
                                                          volumeEditInfo.m_mapIndex);
    }
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
 * Called when voxel label value action is triggered.
 */
void
UserInputModeVolumeEditWidget::labelValueActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfo(volumeEditInfo)) {
        if (volumeEditInfo.m_volumeFile != NULL) {
            GiftiLabelTableEditor lte(volumeEditInfo.m_volumeFile,
                                      volumeEditInfo.m_mapIndex,
                                      "Select Label",
                                      GiftiLabelTableEditor::OPTION_NONE,
                                      m_voxelLabelValueToolButton);
            const AString defaultLabelName = m_voxelLabelValueAction->text();
            if ( ! defaultLabelName.isEmpty()) {
                lte.selectLabelWithName(defaultLabelName);
            }
            if (lte.exec() == GiftiLabelTableEditor::Accepted) {
                const AString selectedName = lte.getLastSelectedLabelName();
                m_voxelLabelValueAction->setText(selectedName);
            }
        }
    }
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

