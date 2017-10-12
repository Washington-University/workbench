
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

#include <cmath>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_SELECTION_DECLARE__
#include "BrainBrowserWindowToolBarSliceSelection.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_SELECTION_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QSpinBox>
#include <QToolButton>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventUpdateVolumeEditingToolBar.h"
#include "GuiManager.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "VolumeFile.h"
#include "VolumeSliceObliqueDrawingMaskEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "WuQFactory.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarSliceSelection 
 * \brief Toolbar component for selection of volume slices.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarSliceSelection::BrainBrowserWindowToolBarSliceSelection(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QAction* volumeIndicesOriginToolButtonAction = WuQtUtilities::createAction("O\nR\nI\nG\nI\nN",
                                                                               "Set the slice indices to the origin, \n"
                                                                               "stereotaxic coordinate (0, 0, 0)",
                                                                               this,
                                                                               this,
                                                                               SLOT(volumeIndicesOriginActionTriggered()));
    QToolButton* volumeIndicesOriginToolButton = new QToolButton;
    volumeIndicesOriginToolButton->setDefaultAction(volumeIndicesOriginToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumeIndicesOriginToolButton);
    
    QLabel* parasagittalLabel = new QLabel("P:");
    QLabel* coronalLabel = new QLabel("C:");
    QLabel* axialLabel = new QLabel("A:");
    
    m_volumeIndicesParasagittalCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesParasagittalCheckBox,
                                          "Enable/Disable display of PARASAGITTAL slice");
    QObject::connect(m_volumeIndicesParasagittalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesParasagittalCheckBoxStateChanged(int)));
    
    m_volumeIndicesCoronalCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesCoronalCheckBox,
                                          "Enable/Disable display of CORONAL slice");
    QObject::connect(m_volumeIndicesCoronalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesCoronalCheckBoxStateChanged(int)));
    
    m_volumeIndicesAxialCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesAxialCheckBox,
                                          "Enable/Disable display of AXIAL slice");
    
    QObject::connect(m_volumeIndicesAxialCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesAxialCheckBoxStateChanged(int)));
    
    const int sliceIndexSpinBoxWidth = 55;
    const int sliceCoordinateSpinBoxWidth = 60;
    
    m_volumeIndicesParasagittalSpinBox = WuQFactory::newSpinBox();
    m_volumeIndicesParasagittalSpinBox->setFixedWidth(sliceIndexSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesParasagittalSpinBox,
                                          "Change the selected PARASAGITTAL slice");
    QObject::connect(m_volumeIndicesParasagittalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesParasagittalSpinBoxValueChanged(int)));
    
    m_volumeIndicesCoronalSpinBox = WuQFactory::newSpinBox();
    m_volumeIndicesCoronalSpinBox->setFixedWidth(sliceIndexSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesCoronalSpinBox,
                                          "Change the selected CORONAL slice");
    QObject::connect(m_volumeIndicesCoronalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesCoronalSpinBoxValueChanged(int)));
    
    m_volumeIndicesAxialSpinBox = WuQFactory::newSpinBox();
    m_volumeIndicesAxialSpinBox->setFixedWidth(sliceIndexSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesAxialSpinBox,
                                          "Change the selected AXIAL slice");
    QObject::connect(m_volumeIndicesAxialSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesAxialSpinBoxValueChanged(int)));
    
    m_volumeIndicesXcoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_volumeIndicesXcoordSpinBox->setDecimals(1);
    m_volumeIndicesXcoordSpinBox->setFixedWidth(sliceCoordinateSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesXcoordSpinBox,
                                          "Adjust coordinate to select PARASAGITTAL slice");
    QObject::connect(m_volumeIndicesXcoordSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeIndicesXcoordSpinBoxValueChanged(double)));
    
    m_volumeIndicesYcoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_volumeIndicesYcoordSpinBox->setDecimals(1);
    m_volumeIndicesYcoordSpinBox->setFixedWidth(sliceCoordinateSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesYcoordSpinBox,
                                          "Adjust coordinate to select CORONAL slice");
    QObject::connect(m_volumeIndicesYcoordSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeIndicesYcoordSpinBoxValueChanged(double)));
    
    m_volumeIndicesZcoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_volumeIndicesZcoordSpinBox->setDecimals(1);
    m_volumeIndicesZcoordSpinBox->setFixedWidth(sliceCoordinateSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesZcoordSpinBox,
                                          "Adjust coordinate to select AXIAL slice");
    QObject::connect(m_volumeIndicesZcoordSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeIndicesZcoordSpinBoxValueChanged(double)));
    
    const AString idToolTipText = ("When selected: If there is an identification operation "
                                   "in ths tab or any other tab with the same yoking status "
                                   "(not Off), the volume slices will move to the location "
                                   "of the identified brainordinate.");
    m_volumeIdentificationUpdatesSlicesAction = WuQtUtilities::createAction("",
                                                                            WuQtUtilities::createWordWrappedToolTipText(idToolTipText),
                                                                            this,
                                                                            this,
                                                                            SLOT(volumeIdentificationToggled(bool)));
    m_volumeIdentificationUpdatesSlicesAction->setCheckable(true);
    QIcon volumeCrossHairIcon;
    const bool volumeCrossHairIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/volume-crosshair-pointer.png",
                            volumeCrossHairIcon);
    if (volumeCrossHairIconValid) {
        m_volumeIdentificationUpdatesSlicesAction->setIcon(volumeCrossHairIcon);
    }
    else {
        m_volumeIdentificationUpdatesSlicesAction->setText("ID");
    }
    QToolButton* volumeIDToolButton = new QToolButton;
    volumeIDToolButton->setDefaultAction(m_volumeIdentificationUpdatesSlicesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumeIDToolButton);
    
    m_volumeSliceProjectionTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_volumeSliceProjectionTypeEnumComboBox->setup<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>();
    m_volumeSliceProjectionTypeEnumComboBox->getComboBox()->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    QObject::connect(m_volumeSliceProjectionTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(volumeSliceProjectionTypeEnumComboBoxItemActivated()));
    WuQtUtilities::setToolTipAndStatusTip(m_volumeSliceProjectionTypeEnumComboBox->getWidget(),
                                          "Chooses viewing orientation (oblique or orthogonal)");
    
    const AString maskToolTip("Masking for oblique slice viewing is used to remove "
                              "artifacts due to cubic interpolation.  In extreme "
                              "instance, the artifacts result in blocky and/or "
                              "striped patterns.");
    m_obliqueMaskingAction = new QAction("M");
    WuQtUtilities::setWordWrappedToolTip(m_obliqueMaskingAction,
                                         maskToolTip);
    m_obliqueMaskingAction->setCheckable(true);
    QObject::connect(m_obliqueMaskingAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSliceSelection::obliqueMaskingActionTriggered);
    QToolButton* obliqueMaskingToolButton = new QToolButton();
    obliqueMaskingToolButton->setDefaultAction(m_obliqueMaskingAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(obliqueMaskingToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->addWidget(m_volumeIndicesParasagittalCheckBox, 0, 0);
    gridLayout->addWidget(parasagittalLabel, 0, 1);
    gridLayout->addWidget(m_volumeIndicesCoronalCheckBox, 1, 0);
    gridLayout->addWidget(coronalLabel, 1, 1);
    gridLayout->addWidget(m_volumeIndicesAxialCheckBox, 2, 0);
    gridLayout->addWidget(axialLabel, 2, 1);

    gridLayout->addWidget(m_volumeIndicesParasagittalSpinBox, 0, 2);
    gridLayout->addWidget(m_volumeIndicesCoronalSpinBox, 1, 2);
    gridLayout->addWidget(m_volumeIndicesAxialSpinBox, 2, 2);

    gridLayout->addWidget(m_volumeIndicesXcoordSpinBox, 0, 3);
    gridLayout->addWidget(m_volumeIndicesYcoordSpinBox, 1, 3);
    gridLayout->addWidget(m_volumeIndicesZcoordSpinBox, 2, 3);

    gridLayout->addWidget(volumeIDToolButton, 3, 0, 1, 2, Qt::AlignLeft);
    gridLayout->addWidget(m_volumeSliceProjectionTypeEnumComboBox->getWidget(), 3, 2, 1, 2, Qt::AlignCenter);
    //gridLayout->addWidget(m_volumeSliceProjectionTypeEnumComboBox->getWidget(), 3, 2, 1, 3, Qt::AlignRight);
    gridLayout->addWidget(obliqueMaskingToolButton, 3, 4);

    gridLayout->addWidget(volumeIndicesOriginToolButton, 0, 4, 3, 1);
    
    m_volumeIndicesWidgetGroup = new WuQWidgetObjectGroup(this);
    m_volumeIndicesWidgetGroup->add(volumeIndicesOriginToolButtonAction);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesParasagittalCheckBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesParasagittalSpinBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesCoronalCheckBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesCoronalSpinBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesAxialCheckBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesAxialSpinBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesXcoordSpinBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesYcoordSpinBox);
    m_volumeIndicesWidgetGroup->add(m_volumeIndicesZcoordSpinBox);
    m_volumeIndicesWidgetGroup->add(m_volumeSliceProjectionTypeEnumComboBox->getWidget());
    m_volumeIndicesWidgetGroup->add(m_volumeIdentificationUpdatesSlicesAction);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarSliceSelection::~BrainBrowserWindowToolBarSliceSelection()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarSliceSelection::updateContent(BrowserTabContent* browserTabContent)
{
    m_volumeIndicesWidgetGroup->blockAllSignals(true);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    VolumeMappableInterface* vf = NULL;
    ModelVolume* volumeModel = browserTabContent->getDisplayedVolumeModel();
    if (volumeModel != NULL) {
        if (m_parentToolBar->getDisplayedModel() == volumeModel) {
            vf = volumeModel->getUnderlayVolumeFile(tabIndex);
            m_volumeIndicesAxialCheckBox->setVisible(false);
            m_volumeIndicesCoronalCheckBox->setVisible(false);
            m_volumeIndicesParasagittalCheckBox->setVisible(false);
        }
    }
    
    ModelWholeBrain* wholeBrainModel = browserTabContent->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        if (m_parentToolBar->getDisplayedModel() == wholeBrainModel) {
            vf = wholeBrainModel->getUnderlayVolumeFile(tabIndex);
            m_volumeIndicesAxialCheckBox->setVisible(true);
            m_volumeIndicesCoronalCheckBox->setVisible(true);
            m_volumeIndicesParasagittalCheckBox->setVisible(true);
        }
    }
    
    if (vf != NULL) {
        /*
         * Test selected file to see if it is an oblique volume file (not a CIFTI file)
         */
        bool obliqueVolumeFlag = false;
        VolumeFile* volumeFile = dynamic_cast<VolumeFile*>(vf);
        if (volumeFile != NULL) {
            if ( ! volumeFile->isPlumb()) {
                obliqueVolumeFlag = true;
            }
        }
        
        /*
         * Update slice projection type for allowed projection types
         */
        std::vector<VolumeSliceProjectionTypeEnum::Enum> validSliceProjections;
        validSliceProjections.push_back(VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE);
        if ( ! obliqueVolumeFlag) {
            validSliceProjections.push_back(VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL);
        }
        m_volumeSliceProjectionTypeEnumComboBox->setupWithItems<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>(validSliceProjections);

        /*
         * If volume is oblique, change its projection type to oblique
         */
        switch (browserTabContent->getSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                if (obliqueVolumeFlag) {
                    browserTabContent->setSliceProjectionType(VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE);
                }
                break;
        }
        
        m_volumeIndicesAxialCheckBox->setChecked(browserTabContent->isSliceAxialEnabled());
        m_volumeIndicesCoronalCheckBox->setChecked(browserTabContent->isSliceCoronalEnabled());
        m_volumeIndicesParasagittalCheckBox->setChecked(browserTabContent->isSliceParasagittalEnabled());
    }
    
    updateObliqueMaskingButton();

    m_volumeSliceProjectionTypeEnumComboBox->setSelectedItem<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>(browserTabContent->getSliceProjectionType());
    
    m_volumeIdentificationUpdatesSlicesAction->setChecked(browserTabContent->isIdentificationUpdatesVolumeSlices());
    
    this->updateSliceIndicesAndCoordinatesRanges();
    
    m_volumeIndicesWidgetGroup->blockAllSignals(false);
}

/*
 * Set the values/minimums/maximums for volume slice indices and coordinate spin controls.
 */
void
BrainBrowserWindowToolBarSliceSelection::updateSliceIndicesAndCoordinatesRanges()
{
    const bool blockedStatus = m_volumeIndicesWidgetGroup->signalsBlocked();
    m_volumeIndicesWidgetGroup->blockAllSignals(true);
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    VolumeMappableInterface* vf = NULL;
    ModelVolume* volumeModel = btc->getDisplayedVolumeModel();
    if (volumeModel != NULL) {
        vf = volumeModel->getUnderlayVolumeFile(tabIndex);
    }
    
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        vf = wholeBrainModel->getUnderlayVolumeFile(tabIndex);
    }
    
    if (vf != NULL) {
        m_volumeIndicesAxialSpinBox->setEnabled(true);
        m_volumeIndicesCoronalSpinBox->setEnabled(true);
        m_volumeIndicesParasagittalSpinBox->setEnabled(true);
        
        std::vector<int64_t> dimensions;
        vf->getDimensions(dimensions);
        
        /*
         * Setup minimum and maximum slices for each dimension.
         * Range is unlimited when Yoked.
         */
        int minAxialDim = 0;
        int minCoronalDim = 0;
        int minParasagittalDim = 0;
        int maxAxialDim = (dimensions[2] > 0) ? (dimensions[2] - 1) : 0;
        int maxCoronalDim = (dimensions[1] > 0) ? (dimensions[1] - 1) : 0;
        int maxParasagittalDim = (dimensions[0] > 0) ? (dimensions[0] - 1) : 0;
        
        m_volumeIndicesAxialSpinBox->setRange(minAxialDim,
                                                  maxAxialDim);
        m_volumeIndicesCoronalSpinBox->setRange(minCoronalDim,
                                                    maxCoronalDim);
        m_volumeIndicesParasagittalSpinBox->setRange(minParasagittalDim,
                                                         maxParasagittalDim);
        
        
        /*
         * Setup minimum and maximum coordinates for each dimension.
         * Range is unlimited when Yoked.
         */
        int64_t slicesZero[3] = { 0, 0, 0 };
        float sliceZeroCoords[3];
        vf->indexToSpace(slicesZero,
                         sliceZeroCoords);
        int64_t slicesMax[3] = { maxParasagittalDim, maxCoronalDim, maxAxialDim };
        float sliceMaxCoords[3];
        vf->indexToSpace(slicesMax,
                         sliceMaxCoords);
        
        m_volumeIndicesXcoordSpinBox->setMinimum(std::min(sliceZeroCoords[0],
                                                              sliceMaxCoords[0]));
        m_volumeIndicesYcoordSpinBox->setMinimum(std::min(sliceZeroCoords[1],
                                                              sliceMaxCoords[1]));
        m_volumeIndicesZcoordSpinBox->setMinimum(std::min(sliceZeroCoords[2],
                                                              sliceMaxCoords[2]));
        
        m_volumeIndicesXcoordSpinBox->setMaximum(std::max(sliceZeroCoords[0],
                                                              sliceMaxCoords[0]));
        m_volumeIndicesYcoordSpinBox->setMaximum(std::max(sliceZeroCoords[1],
                                                              sliceMaxCoords[1]));
        m_volumeIndicesZcoordSpinBox->setMaximum(std::max(sliceZeroCoords[2],
                                                              sliceMaxCoords[2]));
        
        int64_t slicesOne[3] = { 1, 1, 1 };
        float slicesOneCoords[3];
        vf->indexToSpace(slicesOne,
                         slicesOneCoords);
        const float dx = std::fabs(slicesOneCoords[0] - sliceZeroCoords[0]);
        const float dy = std::fabs(slicesOneCoords[1] - sliceZeroCoords[1]);
        const float dz = std::fabs(slicesOneCoords[2] - sliceZeroCoords[2]);
        m_volumeIndicesXcoordSpinBox->setSingleStep(dx);
        m_volumeIndicesYcoordSpinBox->setSingleStep(dy);
        m_volumeIndicesZcoordSpinBox->setSingleStep(dz);
        
        m_volumeIndicesAxialSpinBox->setValue(btc->getSliceIndexAxial(vf));
        m_volumeIndicesCoronalSpinBox->setValue(btc->getSliceIndexCoronal(vf));
        m_volumeIndicesParasagittalSpinBox->setValue(btc->getSliceIndexParasagittal(vf));
        
        int64_t slices[3] = {
            btc->getSliceIndexParasagittal(vf),
            btc->getSliceIndexCoronal(vf),
            btc->getSliceIndexAxial(vf)
        };
        float sliceCoords[3] = { 0.0, 0.0, 0.0 };
        if (vf != NULL) {
            vf->indexToSpace(slices,
                             sliceCoords);
        }
        m_volumeIndicesXcoordSpinBox->setValue(btc->getSliceCoordinateParasagittal());
        m_volumeIndicesYcoordSpinBox->setValue(btc->getSliceCoordinateCoronal());
        m_volumeIndicesZcoordSpinBox->setValue(btc->getSliceCoordinateAxial());
        
    }
    
    m_volumeIndicesWidgetGroup->blockAllSignals(blockedStatus);
}

/**
 * Called when volume indices ORIGIN tool button is pressed.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesOriginActionTriggered()
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setSlicesToOrigin();
    
    updateContent(btc);
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when volume indices parasagittal check box is toggled.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesParasagittalCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setSliceParasagittalEnabled(m_volumeIndicesParasagittalCheckBox->isChecked());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when volume indices coronal check box is toggled.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesCoronalCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setSliceCoronalEnabled(m_volumeIndicesCoronalCheckBox->isChecked());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when volume indices axial check box is toggled.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesAxialCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setSliceAxialEnabled(m_volumeIndicesAxialCheckBox->isChecked());
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when volume indices parasagittal spin box value is changed.
 *
 * @param sliceIndex
 *     New index of slice.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesParasagittalSpinBoxValueChanged(int sliceIndex)
{
    this->readVolumeSliceIndicesAndUpdateSliceCoordinates(VolumeSliceViewPlaneEnum::PARASAGITTAL,
                                                          sliceIndex);
}

/**
 * Called when volume indices coronal spin box value is changed.
 *
 * @param sliceIndex
 *     New index of slice.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesCoronalSpinBoxValueChanged(int sliceIndex)
{
    this->readVolumeSliceIndicesAndUpdateSliceCoordinates(VolumeSliceViewPlaneEnum::CORONAL,
                                                          sliceIndex);
}

/**
 * Called when volume indices axial spin box value is changed.
 *
 * @param sliceIndex
 *     New index of slice.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesAxialSpinBoxValueChanged(int sliceIndex)
{
    
    this->readVolumeSliceIndicesAndUpdateSliceCoordinates(VolumeSliceViewPlaneEnum::AXIAL,
                                                          sliceIndex);
}

/**
 * Called when X stereotaxic coordinate is changed.
 * @param d
 *    New value.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesXcoordSpinBoxValueChanged(double /*d*/)
{
    this->readVolumeSliceCoordinatesAndUpdateSliceIndices();
}

/**
 * Called when Y stereotaxic coordinate is changed.
 * @param d
 *    New value.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesYcoordSpinBoxValueChanged(double /*d*/)
{
    this->readVolumeSliceCoordinatesAndUpdateSliceIndices();
}

/**
 * Called when Z stereotaxic coordinate is changed.
 * @param d
 *    New value.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesZcoordSpinBoxValueChanged(double /*d*/)
{
    this->readVolumeSliceCoordinatesAndUpdateSliceIndices();
}

/**
 * Read the slice indices and update the slice coordinates.
 *
 * @param viewPlane
 *     View plane whose slice index was changed
 * @param sliceIndex
 *     New slice index.
 */
void
BrainBrowserWindowToolBarSliceSelection::readVolumeSliceIndicesAndUpdateSliceCoordinates(const VolumeSliceViewPlaneEnum::Enum viewPlane,
                                                                                         const int64_t sliceIndex)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    VolumeMappableInterface* underlayVolumeFile = NULL;
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        if (m_parentToolBar->getDisplayedModel() == wholeBrainModel) {
            underlayVolumeFile = wholeBrainModel->getUnderlayVolumeFile(tabIndex);
        }
    }
    
    ModelVolume* volumeModel = btc->getDisplayedVolumeModel();
    if (volumeModel != NULL) {
        if (m_parentToolBar->getDisplayedModel() == volumeModel) {
            underlayVolumeFile = volumeModel->getUnderlayVolumeFile(tabIndex);
        }
    }
    
    if (underlayVolumeFile != NULL) {
        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType = m_volumeSliceProjectionTypeEnumComboBox->getSelectedItem<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>();
        switch (sliceProjectionType) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            {
                float sx(1.0), sy(1.0), sz(1.0);
                underlayVolumeFile->getVoxelSpacing(sx, sy, sz);
                
                switch (viewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                    {
                        const int32_t diff  = sliceIndex - btc->getSliceIndexAxial(underlayVolumeFile);
                        if (diff > 0) {
                            btc->setSliceCoordinateAxial(btc->getSliceCoordinateAxial() + sz);
                        }
                        else {
                            btc->setSliceCoordinateAxial(btc->getSliceCoordinateAxial() - sz);
                        }
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                    {
                        const int32_t diff  = sliceIndex - btc->getSliceIndexCoronal(underlayVolumeFile);
                        if (diff > 0) {
                            btc->setSliceCoordinateCoronal(btc->getSliceCoordinateCoronal() + sy);
                        }
                        else {
                            btc->setSliceCoordinateCoronal(btc->getSliceCoordinateCoronal() - sy);
                        }
                    }
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                    {
                        const int32_t diff  = sliceIndex - btc->getSliceIndexParasagittal(underlayVolumeFile);
                        if (diff > 0) {
                            btc->setSliceCoordinateParasagittal(btc->getSliceCoordinateParasagittal() + sx);
                        }
                        else {
                            btc->setSliceCoordinateParasagittal(btc->getSliceCoordinateParasagittal() - sx);
                        }
                    }
                        break;
                }
            }
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                switch (viewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        btc->setSliceIndexAxial(underlayVolumeFile,
                                                sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        btc->setSliceIndexCoronal(underlayVolumeFile,
                                                  sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        btc->setSliceIndexParasagittal(underlayVolumeFile,
                                                       sliceIndex);
                        break;
                }
                break;
        }
    }
    
    this->updateSliceIndicesAndCoordinatesRanges();
    
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Read the slice coordinates and convert to slices indices and then
 * update the displayed slices.
 */
void
BrainBrowserWindowToolBarSliceSelection::readVolumeSliceCoordinatesAndUpdateSliceIndices()
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    
    VolumeMappableInterface* underlayVolumeFile = NULL;
    ModelWholeBrain* wholeBrainModel = btc->getDisplayedWholeBrainModel();
    if (wholeBrainModel != NULL) {
        if (m_parentToolBar->getDisplayedModel() == wholeBrainModel) {
            underlayVolumeFile = wholeBrainModel->getUnderlayVolumeFile(tabIndex);
        }
    }
    
    ModelVolume* volumeModel = btc->getDisplayedVolumeModel();
    if (volumeModel != NULL) {
        if (m_parentToolBar->getDisplayedModel() == volumeModel) {
            underlayVolumeFile = volumeModel->getUnderlayVolumeFile(tabIndex);
        }
    }
    
    if (underlayVolumeFile != NULL) {
        float sliceCoords[3] = {
            m_volumeIndicesXcoordSpinBox->value(),
            m_volumeIndicesYcoordSpinBox->value(),
            m_volumeIndicesZcoordSpinBox->value()
        };
        
        btc->selectSlicesAtCoordinate(sliceCoords);
    }
    
    this->updateSliceIndicesAndCoordinatesRanges();
    
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when projection type is changed.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeSliceProjectionTypeEnumComboBoxItemActivated()
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType = m_volumeSliceProjectionTypeEnumComboBox->getSelectedItem<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>();
    btc->setSliceProjectionType(sliceProjectionType);
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
    updateObliqueMaskingButton();
    EventManager::get()->sendEvent(EventUpdateVolumeEditingToolBar().getPointer());
}

/**
 * Called when volume identification action toggled.
 *
 * @param value
 *     New value.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIdentificationToggled(bool value)
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    browserTabContent->setIdentificationUpdatesVolumeSlices(value);
}

/**
 * Called when the oblique masking action is triggered.
 */
void
BrainBrowserWindowToolBarSliceSelection::obliqueMaskingActionTriggered(bool)
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    std::vector<VolumeSliceObliqueDrawingMaskEnum::Enum> allMaskEnums;
    VolumeSliceObliqueDrawingMaskEnum::getAllEnums(allMaskEnums);
    
    QMenu obliqueMaskingMenu("Oblique Sampling");
    QActionGroup* maskActionGroup = new QActionGroup(this);
    maskActionGroup->setExclusive(true);
    QAction* selectedAction = NULL;
    for (auto maskEnum : allMaskEnums) {
        QAction* action = maskActionGroup->addAction(VolumeSliceObliqueDrawingMaskEnum::toGuiName(maskEnum));
        action->setCheckable(true);
        action->setData(VolumeSliceObliqueDrawingMaskEnum::toIntegerCode(maskEnum));
        if (maskEnum == browserTabContent->getObliqueSliceDrawingMaskingType()) {
            selectedAction = action;
        }
        obliqueMaskingMenu.addAction(action);
    }
    if (selectedAction != NULL) {
        selectedAction->setChecked(true);
    }
    
    selectedAction = obliqueMaskingMenu.exec(QCursor::pos());
    if (selectedAction != NULL) {
        const int32_t intValue = selectedAction->data().toInt();
        bool validFlag = false;
        VolumeSliceObliqueDrawingMaskEnum::Enum maskType = VolumeSliceObliqueDrawingMaskEnum::fromIntegerCode(intValue,
                                                                                                              &validFlag);
        CaretAssert(validFlag);
        browserTabContent->setObliqueSliceDrawingMaskingType(maskType);
        
        this->updateGraphicsWindow();
        this->updateOtherYokedWindows();
        EventManager::get()->sendEvent(EventUpdateVolumeEditingToolBar().getPointer());
    }
    
    updateObliqueMaskingButton();
}

/**
 * Update the oblique masking button so that it enabled only 
 * when oblique slice drawing is selected and it is "checked"
 * when a masking is applied.
 */
void
BrainBrowserWindowToolBarSliceSelection::updateObliqueMaskingButton()
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    
    QSignalBlocker obliqueBlocker(m_obliqueMaskingAction);
    switch (browserTabContent->getObliqueSliceDrawingMaskingType()) {
        case VolumeSliceObliqueDrawingMaskEnum::OFF:
            m_obliqueMaskingAction->setChecked(false);
            break;
        case VolumeSliceObliqueDrawingMaskEnum::ENCLOSING_VOXEL:
        case VolumeSliceObliqueDrawingMaskEnum::TRILINEAR_INTERPOLATION:
            m_obliqueMaskingAction->setChecked(true);
            break;
    }
    
    switch (browserTabContent->getSliceProjectionType()) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            m_obliqueMaskingAction->setEnabled(true);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            m_obliqueMaskingAction->setEnabled(false);
            break;
    }
}


