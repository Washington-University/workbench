
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
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventUpdateVolumeEditingToolBar.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabel.h"
#include "GiftiLabelTableEditor.h"
#include "GuiManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "VolumeFile.h"
#include "VolumeFileEditorDelegate.h"
#include "VolumeFileCreateDialog.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQHyperlinkToolTip.h"
#include "WuQMessageBox.h"
#include "WuQSpinBoxOddValue.h"
#include "WuQtUtilities.h"

using namespace caret;

static const QString& helpHyperlink("help://VoxelEditing");
    
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
  EventListenerInterface(),
m_inputModeVolumeEdit(inputModeVolumeEdit),
m_windowIndex(windowIndex)
{
    CaretAssert(inputModeVolumeEdit);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addWidget(BrainBrowserWindowToolBar::createToolWidget("Voxel Operation",
                                                                  createOperationWidget(),
                                                                  BrainBrowserWindowToolBar::WIDGET_PLACEMENT_LEFT,
                                                                  BrainBrowserWindowToolBar::WIDGET_PLACEMENT_TOP,
                                                                  0));
    layout->addWidget(BrainBrowserWindowToolBar::createToolWidget("Options",
                                                                  createSelectionToolBar(),
                                                                  BrainBrowserWindowToolBar::WIDGET_PLACEMENT_LEFT,
                                                                  BrainBrowserWindowToolBar::WIDGET_PLACEMENT_TOP,
                                                                  0));
    layout->addStretch();
  
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_VOLUME_EDITING_TOOLBAR);
}

/**
 * Destructor.
 */
UserInputModeVolumeEditWidget::~UserInputModeVolumeEditWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event
 *
 * @param event
 *    The event.
 */
void
UserInputModeVolumeEditWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_UPDATE_VOLUME_EDITING_TOOLBAR) {
        EventUpdateVolumeEditingToolBar* editVolEvent = dynamic_cast<EventUpdateVolumeEditingToolBar*>(event);
        CaretAssert(editVolEvent);
        editVolEvent->setEventProcessed();
        
        updateWidget();
    }
}


/**
 * Update the widget.
 */
void
UserInputModeVolumeEditWidget::updateWidget()
{
    bool isValid = false;
    
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        m_lockAction->setChecked(volumeEditInfo.m_volumeFileEditorDelegate->isLocked(volumeEditInfo.m_mapIndex));
        
        if (volumeEditInfo.m_volumeFile != NULL) {
            if (volumeEditInfo.m_volumeFile->isMappedWithLabelTable()) {
                m_voxelLabelValueToolButton->setEnabled(true);
                m_voxelFloatValueSpinBox->setEnabled(false);

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
                         * Default to the unassigned label
                         */
                        const int32_t unassignedKey = labelTable->getUnassignedLabelKey();
                        GiftiLabel* unassignedLabel = labelTable->getLabel(unassignedKey);
                        if (unassignedLabel != NULL) {
                            buttonText = unassignedLabel->getName();
                        }
                        
                        /*
                         * Find the first label that is not the unassigned
                         * label
                         */
                        const std::set<int32_t> keys = labelTable->getKeys();
                        for (std::set<int32_t>::iterator iter = keys.begin();
                             iter != keys.end();
                             iter++) {
                            const int32_t key = *iter;
                            if (key != unassignedKey) {
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
                m_voxelLabelValueToolButton->setEnabled(false);
                m_voxelFloatValueSpinBox->setEnabled(true);
                isValid = true;
            }
            else if (volumeEditInfo.m_volumeFile->isMappedWithRGBA()) {
                /* nothing */
            }
            else {
                CaretAssert(0);
            }
            
            m_addMapsToolButton->defaultAction()->setEnabled(isValid);
            
            m_modeOnRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON));
            m_modeOffRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF));
            m_modeDilateRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE));
            m_modeErodeRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE));
            m_modeFillTwoDimRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D));
            m_modeFillThreeDimRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D));
            m_modeRemoveTwoDimRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D));
            m_modeRemoveThreeDimRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D));
            m_modeRetainThreeDimRadioButton->setEnabled(isModeButtonEnabled(VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D));
        }
    }
    
    this->setEnabled(isValid);
}

/**
 * @return True if the button for the given mode is valid
 */
bool
UserInputModeVolumeEditWidget::isModeButtonEnabled(const VolumeEditingModeEnum::Enum mode) const
{
    bool modeEnabledFlag(false);
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        m_lockAction->setChecked(volumeEditInfo.m_volumeFileEditorDelegate->isLocked(volumeEditInfo.m_mapIndex));
        
        if (volumeEditInfo.m_volumeFile != NULL) {
            switch (volumeEditInfo.m_sliceProjectionType) {
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                    break;
                case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                    modeEnabledFlag = true;
                    break;
            }
            if ( ! modeEnabledFlag) {
                modeEnabledFlag = VolumeEditingModeEnum::isObliqueEditingAllowed(mode);
            }
        }
    }
    return modeEnabledFlag;
}
                                                

/**
 * @return Create and return the selection toolbar.
 */
QWidget*
UserInputModeVolumeEditWidget::createSelectionToolBar()
{
    QLabel* volumeLabel = new QLabel("File");
    
    m_newFileToolButton = new QToolButton();
    m_newFileToolButton->setDefaultAction(WuQtUtilities::createAction("New",
                                                                    "Create a new volume file that will become the top-most overlay",
                                                                    this,
                                                                    this, SLOT(newFileActionTriggered())));
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_newFileToolButton);
    WuQHyperlinkToolTip::addWithHyperlink(m_newFileToolButton,
                                          helpHyperlink);
    
    m_addMapsToolButton = new QToolButton();
    m_addMapsToolButton->setDefaultAction(WuQtUtilities::createAction("Add",
                                                                      ("Add maps to the selected volume file.\n"
                                                                       "First new map will become the top-most overlay."),
                                                                      this,
                                                                      this, SLOT(addMapsActionTriggered())));
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_addMapsToolButton);
    WuQHyperlinkToolTip::addWithHyperlink(m_addMapsToolButton,
                                          helpHyperlink);
    
    m_lockAction = WuQtUtilities::createAction("Lock",
                                               "Lock/unlock volume file to disallow/allow editing",
                                               this,
                                               this, SLOT(lockFileActionTriggered()));
    m_lockAction->setCheckable(true);
    QToolButton* lockFileToolButton = new QToolButton();
    lockFileToolButton->setDefaultAction(m_lockAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(lockFileToolButton);
    WuQHyperlinkToolTip::addWithHyperlink(lockFileToolButton,
                                          m_lockAction,
                                          helpHyperlink);

    

    QLabel* brushSizeLabel = new QLabel("Brush");
    const int MIN_BRUSH_SIZE = 1;
    const int MAX_BRUSH_SIZE = 9999;
    
    QLabel* xLabel = new QLabel("P:");
    m_xBrushSizeSpinBox = new WuQSpinBoxOddValue(this);
    m_xBrushSizeSpinBox->setRange(MIN_BRUSH_SIZE, MAX_BRUSH_SIZE);
    m_xBrushSizeSpinBox->setSingleStep(2);
    QObject::connect(m_xBrushSizeSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(xBrushSizeValueChanged(int)));
    m_xBrushSizeSpinBox->getWidget()->setToolTip("Parasagittal brush size (voxels).\n"
                                                 "Must be an odd value.");
    WuQHyperlinkToolTip::addWithHyperlink(m_xBrushSizeSpinBox->getWidget(),
                                          helpHyperlink);

    QLabel* yLabel = new QLabel("C:");
    m_yBrushSizeSpinBox = new WuQSpinBoxOddValue(this);
    m_yBrushSizeSpinBox->setRange(MIN_BRUSH_SIZE, MAX_BRUSH_SIZE);
    m_yBrushSizeSpinBox->setSingleStep(2);
    QObject::connect(m_yBrushSizeSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(yBrushSizeValueChanged(int)));
    m_yBrushSizeSpinBox->getWidget()->setToolTip("Coronal brush size (voxels).\n"
                                                 "Must be an odd value.");
    WuQHyperlinkToolTip::addWithHyperlink(m_yBrushSizeSpinBox->getWidget(),
                                          helpHyperlink);

    QLabel* zLabel = new QLabel("A:");
    m_zBrushSizeSpinBox = new WuQSpinBoxOddValue(this);
    m_zBrushSizeSpinBox->setRange(MIN_BRUSH_SIZE, MAX_BRUSH_SIZE);
    m_zBrushSizeSpinBox->setSingleStep(2);
    QObject::connect(m_zBrushSizeSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(zBrushSizeValueChanged(int)));
    m_zBrushSizeSpinBox->getWidget()->setToolTip("Axial brush size (voxels).\n"
                                                 "Must be an odd value.");
    WuQHyperlinkToolTip::addWithHyperlink(m_zBrushSizeSpinBox->getWidget(),
                                          helpHyperlink);

    m_voxelValueLabel = new QLabel("Value");
    m_voxelFloatValueSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(-1000.0,
                                                                                         1000.0,
                                                                                         1.0, 1,
                                                                                         this,
                                                                                         SLOT(voxelValueChanged(double)));
    m_voxelFloatValueSpinBox->setValue(1.0);
    m_voxelFloatValueSpinBox->setToolTip("Set value for functional volume editing");
    WuQHyperlinkToolTip::addWithHyperlink(m_voxelFloatValueSpinBox,
                                          helpHyperlink);

    m_voxelLabelValueAction = WuQtUtilities::createAction("Label",
                                                          "Choose Label for Voxels",
                                                          this,
                                                          this, SLOT(labelValueActionTriggered()));
    m_voxelLabelValueToolButton = new QToolButton();
    m_voxelLabelValueToolButton->setDefaultAction(m_voxelLabelValueAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_voxelLabelValueToolButton);
    WuQHyperlinkToolTip::addWithHyperlink(m_voxelLabelValueToolButton,
                                          helpHyperlink);

    m_voxelLabelValueToolButton->setFixedWidth(m_voxelLabelValueToolButton->sizeHint().width());
    m_voxelFloatValueSpinBox->setFixedWidth(m_voxelLabelValueToolButton->sizeHint().width());
    
    WuQtUtilities::matchWidgetWidths(m_newFileToolButton, m_addMapsToolButton, lockFileToolButton);
    const int32_t spinBoxWidth(20 + m_newFileToolButton->sizeHint().width());
    m_xBrushSizeSpinBox->setFixedWidth(spinBoxWidth);
    m_yBrushSizeSpinBox->setFixedWidth(spinBoxWidth);
    m_zBrushSizeSpinBox->setFixedWidth(spinBoxWidth);
    m_voxelFloatValueSpinBox->setFixedWidth(spinBoxWidth);
    
    QGridLayout* gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
    int32_t row(0);
    int32_t col(0);
    gridLayout->addWidget(volumeLabel, row++, col, Qt::AlignHCenter);
    gridLayout->addWidget(m_newFileToolButton, row++, col);
    gridLayout->addWidget(m_addMapsToolButton, row++, col);
    gridLayout->addWidget(lockFileToolButton, row++, col);
    row = 0;
    col++;
    gridLayout->addWidget(brushSizeLabel, row++, col, 1, 2, Qt::AlignHCenter);
    gridLayout->addWidget(xLabel, row++, col);
    gridLayout->addWidget(yLabel, row++, col);
    gridLayout->addWidget(zLabel, row++, col);
    row = 0;
    col++;
    gridLayout->addWidget(brushSizeLabel, row++, col, Qt::AlignHCenter);
    gridLayout->addWidget(m_xBrushSizeSpinBox->getWidget(), row++, col);
    gridLayout->addWidget(m_yBrushSizeSpinBox->getWidget(), row++, col);
    gridLayout->addWidget(m_zBrushSizeSpinBox->getWidget(), row++, col);
    row = 0;
    col++;
    gridLayout->addWidget(m_voxelValueLabel, row++, col, Qt::AlignHCenter);
    gridLayout->addWidget(m_voxelFloatValueSpinBox, row++, col);
    gridLayout->addWidget(m_voxelLabelValueToolButton, row++, col);

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addLayout(gridLayout);
    layout->addStretch();

    return widget;
}

/**
 * @return Create and return the edit widget.
 */
QWidget*
UserInputModeVolumeEditWidget::createEditWidget()
{
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(WuQtUtilities::createAction("Undo",
                                                                 "Undo the last volume edit",
                                                                 this,
                                                                 this, SLOT(undoActionTriggered())));
    WuQtUtilities::setToolButtonStyleForQt5Mac(undoToolButton);
    WuQHyperlinkToolTip::addWithHyperlink(undoToolButton,
                                          helpHyperlink);

    
    QToolButton* redoToolButton = new QToolButton();
    redoToolButton->setDefaultAction(WuQtUtilities::createAction("Redo",
                                                                 "Redo (or is it undo) the last undo",
                                                                 this,
                                                                 this, SLOT(redoActionTriggered())));
    WuQtUtilities::setToolButtonStyleForQt5Mac(redoToolButton);
    WuQHyperlinkToolTip::addWithHyperlink(redoToolButton,
                                          helpHyperlink);
    
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(WuQtUtilities::createAction("Reset",
                                                                  "Reset all edting of the volume",
                                                                  this,
                                                                  this, SLOT(resetActionTriggered())));
    WuQtUtilities::setToolButtonStyleForQt5Mac(resetToolButton);
    WuQHyperlinkToolTip::addWithHyperlink(resetToolButton,
                                          helpHyperlink);

    QWidget* widget = new QWidget();
    QHBoxLayout* editLayout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(editLayout, 4, 2);
    editLayout->addStretch();
    editLayout->addWidget(undoToolButton);
    editLayout->addWidget(redoToolButton);
    editLayout->addWidget(resetToolButton);
    editLayout->addStretch();

    return widget;
}

/**
 * @return Radio button for selection of the given mode
 *  @mode The editing mode
 */
QRadioButton*
UserInputModeVolumeEditWidget::createModeRadioButton(const VolumeEditingModeEnum::Enum mode)
{
    QRadioButton* rb = new QRadioButton(VolumeEditingModeEnum::toGuiName(mode));
    WuQtUtilities::setWordWrappedToolTip(rb, VolumeEditingModeEnum::toToolTip(mode));
    WuQHyperlinkToolTip::addWithHyperlink(rb,
                                          helpHyperlink);
    

    return rb;
}
/**
 * @return Create and return the mode toolbar.
 */
QWidget*
UserInputModeVolumeEditWidget::createOperationWidget()
{
    m_modeOnRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON);
    m_modeOnRadioButton->setChecked(true);
    
    m_modeOffRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF);
    
    m_modeDilateRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE);
    
    m_modeErodeRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE);
    
    m_modeFillTwoDimRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D);
    
    m_modeFillThreeDimRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D);
    
    m_modeRemoveTwoDimRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D);
    
    m_modeRemoveThreeDimRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D);
    
    m_modeRetainThreeDimRadioButton = createModeRadioButton(VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D);
    
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(m_modeOnRadioButton);
    buttonGroup->addButton(m_modeOffRadioButton);
    buttonGroup->addButton(m_modeDilateRadioButton);
    buttonGroup->addButton(m_modeErodeRadioButton);
    buttonGroup->addButton(m_modeFillTwoDimRadioButton);
    buttonGroup->addButton(m_modeFillThreeDimRadioButton);
    buttonGroup->addButton(m_modeRemoveTwoDimRadioButton);
    buttonGroup->addButton(m_modeRemoveThreeDimRadioButton);
    buttonGroup->addButton(m_modeRetainThreeDimRadioButton);

    QGridLayout* gridLayout = new QGridLayout();
#ifdef CARET_OS_LINUX
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 2);
#else
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 4, 2);
#endif
    int32_t row(0);
    gridLayout->addWidget(m_modeOnRadioButton,
                          row, 0);
    row++;
    gridLayout->addWidget(m_modeOffRadioButton,
                          row, 0);
    row++;
    gridLayout->addWidget(m_modeDilateRadioButton,
                          row, 0);
    row++;
    gridLayout->addWidget(m_modeErodeRadioButton,
                          row, 0);
    row++;
    
    row = 0;
    gridLayout->addWidget(m_modeFillTwoDimRadioButton,
                          row, 1);
    row++;
    gridLayout->addWidget(m_modeFillThreeDimRadioButton,
                          row, 1);
    row++;
    gridLayout->addWidget(m_modeRemoveTwoDimRadioButton,
                          row, 1);
    row++;
    gridLayout->addWidget(m_modeRemoveThreeDimRadioButton,
                          row, 1);
    row++;
    gridLayout->addWidget(m_modeRetainThreeDimRadioButton,
                          row, 1);
    row++;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
#ifdef CARET_OS_LINUX
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
#else
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
#endif
    layout->addLayout(gridLayout);
    layout->addWidget(createEditWidget());
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
    VolumeEditingModeEnum::Enum editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON;
    
    if (m_modeOnRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_ON;
    }
    else if (m_modeOffRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_OFF;
    }
    else if (m_modeDilateRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_DILATE;
    }
    else if (m_modeErodeRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_ERODE;
    }
    else if (m_modeFillTwoDimRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_2D;
    }
    else if (m_modeFillThreeDimRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_FLOOD_FILL_3D;
    }
    else if (m_modeRemoveTwoDimRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_2D;
    }
    else if (m_modeRemoveThreeDimRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_REMOVE_CONNECTED_3D;
    }
    else if (m_modeRetainThreeDimRadioButton->isChecked()) {
        editMode = VolumeEditingModeEnum::VOLUME_EDITING_MODE_RETAIN_CONNECTED_3D;
    }
    else {
        CaretAssert(0);
    }
    
    return editMode;
}



/**
 * Called when new volume file button is clicked.
 */
void
UserInputModeVolumeEditWidget::newFileActionTriggered()
{
    VolumeMappableInterface* underlayVolume(NULL);
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        underlayVolume = volumeEditInfo.m_underlayVolume;
    }
    
    VolumeFileCreateDialog newVolumeDialog(underlayVolume,
                                           m_newFileToolButton);
    if (newVolumeDialog.exec() == VolumeFileCreateDialog::Accepted) {
        VolumeFile* vf = newVolumeDialog.getVolumeFile();
        if (vf != NULL) {
            for (int32_t i = 0; i < vf->getNumberOfMaps(); i++) {
                vf->getVolumeFileEditorDelegate()->setLocked(i,
                                                             false);
            }
            
            EventDataFileAdd addFileEvent(vf);
            EventManager::get()->sendEvent(addFileEvent.getPointer());
            
            viewVolumeInNewOverlay(vf,
                                   0);
        }
    }
}

/**
 * View the volume in a new overlay at the top of the overlays
 *
 * @param vf
 *     The volume file.
 * @param mapIndex
 *     Select this map index in overlay.
 */
void
UserInputModeVolumeEditWidget::viewVolumeInNewOverlay(VolumeFile* vf,
                                                      const int32_t mapIndex)
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo);
    if (volumeEditInfo.m_topOverlay != NULL) {
        /*
         * Add new overlay and place new volume file in the top most overlay
         */
        volumeEditInfo.m_overlaySet->insertOverlayAbove(0);
        volumeEditInfo.m_topOverlay = volumeEditInfo.m_overlaySet->getPrimaryOverlay();
        
        CaretAssert((mapIndex >= 0)
                    && (mapIndex < vf->getNumberOfMaps()));
        volumeEditInfo.m_topOverlay->setSelectionData(vf,
                                                      mapIndex);
        volumeEditInfo.m_topOverlay->setEnabled(true);
        volumeEditInfo.m_topOverlay->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
        m_inputModeVolumeEdit->updateGraphicsAfterEditing();
    }
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    updateWidget();
}

/**
 * Called when the add maps action is triggered.
 */
void
UserInputModeVolumeEditWidget::addMapsActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        VolumeFile* vf = volumeEditInfo.m_volumeFile;
        
        WuQDataEntryDialog ded("Add Map",
                               m_addMapsToolButton);
        ded.setTextAtTop(("Add Map to Volume File: "
                          + vf->getFileNameNoPath()), true);
        const int32_t newMapIndex = vf->getNumberOfMaps();
        QLineEdit* nameLineEdit = ded.addLineEditWidget("Map Name");
        nameLineEdit->setText("Map " + AString::number(newMapIndex + 1));
        
        if (ded.exec() == WuQDataEntryDialog::Accepted) {
            /*
             * Add map, set the map name,
             * set the selected map to the new map,
             * update graphics, update user interface.
             */
            vf->addSubvolumes(1);
            vf->setMapName(newMapIndex,
                           nameLineEdit->text().trimmed());
            vf->getVolumeFileEditorDelegate()->setLocked(newMapIndex,
                                                         false);
            viewVolumeInNewOverlay(vf,
                                   newMapIndex);
        }
    }
}


/**
 * Called when lock button is clicked.
 */
void
UserInputModeVolumeEditWidget::lockFileActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        /*
         * Warn user if unlocking and volume is not plumb
         */
        if ( ! m_lockAction->isChecked()) {
            if ( ! volumeEditInfo.m_volumeFile->isPlumb()) {
                const AString msg("Volume is not aligned to P/C/A Axes.\n"
                                  "Editing may not function correctly.");
                const bool result(WuQMessageBox::warningOkCancel(this, msg));
                if ( ! result) {
                    m_lockAction->setChecked(true);
                    return;
                }
            }
        }
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
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        AString errorMessage;
        if ( ! volumeEditInfo.m_volumeFileEditorDelegate->undo(volumeEditInfo.m_mapIndex,
                                                               errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        m_inputModeVolumeEdit->updateGraphicsAfterEditing();
    }
}

/**
 * Called when redo button is clicked.
 */
void
UserInputModeVolumeEditWidget::redoActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        AString errorMessage;
        if ( ! volumeEditInfo.m_volumeFileEditorDelegate->redo(volumeEditInfo.m_mapIndex,
                                                               errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        m_inputModeVolumeEdit->updateGraphicsAfterEditing();
    }
}

/**
 * Called when reset button is clicked.
 */
void
UserInputModeVolumeEditWidget::resetActionTriggered()
{
    UserInputModeVolumeEdit::VolumeEditInfo volumeEditInfo;
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
        AString errorMessage;
        if ( ! volumeEditInfo.m_volumeFileEditorDelegate->reset(volumeEditInfo.m_mapIndex,
                                                               errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        m_inputModeVolumeEdit->updateGraphicsAfterEditing();
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
    if (m_inputModeVolumeEdit->getVolumeEditInfoForStatus(volumeEditInfo)) {
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
    (void)VolumeEditingModeEnum::fromIntegerCode(modeInt,
                                                 &validFlag);
    CaretAssert(validFlag);
}

