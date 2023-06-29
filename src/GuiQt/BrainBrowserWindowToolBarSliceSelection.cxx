
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
#include <QActionGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QSpinBox>
#include <QToolButton>
#include <QWidgetAction>

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
#include "VolumeMprSettingsWidget.h"
#include "VolumeObliqueOptionsWidget.h"
#include "VolumeSliceInterpolationEdgeEffectsMaskingEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
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
 *
 * @param parentToolBar
 *     The parent toolbar.
 * @param parentObjectName
 *     Name of parent object.
 */
BrainBrowserWindowToolBarSliceSelection::BrainBrowserWindowToolBarSliceSelection(BrainBrowserWindowToolBar* parentToolBar,
                                                                                 const QString parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar),
m_objectNamePrefix(parentObjectName
                   + ":ToolBar:SliceSelection:")
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    
    QAction* volumeIndicesOriginToolButtonAction = WuQtUtilities::createAction("O\nR\nI\nG\nI\nN",
                                                                               "Set the slice indices to the origin, \n"
                                                                               "stereotaxic coordinate (0, 0, 0)",
                                                                               this,
                                                                               this,
                                                                               SLOT(volumeIndicesOriginActionTriggered()));
    QToolButton* volumeIndicesOriginToolButton = new QToolButton;
    volumeIndicesOriginToolButton->setDefaultAction(volumeIndicesOriginToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumeIndicesOriginToolButton);
    volumeIndicesOriginToolButtonAction->setObjectName(m_objectNamePrefix
                                                       + "MoveVolumeSlicesToOrigin");
    volumeIndicesOriginToolButtonAction->setParent(volumeIndicesOriginToolButton);
    macroManager->addMacroSupportToObject(volumeIndicesOriginToolButtonAction,
                                          "Set volume slices to origin");
    
    QLabel* parasagittalLabel = new QLabel("P:");
    QLabel* coronalLabel = new QLabel("C:");
    QLabel* axialLabel = new QLabel("A:");
    
    m_volumeIndicesParasagittalCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesParasagittalCheckBox,
                                          "Enable/Disable display of PARASAGITTAL slice");
    m_volumeIndicesParasagittalCheckBox->setObjectName(m_objectNamePrefix
                                                       + "EnableParasagittalSlice");
    QObject::connect(m_volumeIndicesParasagittalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesParasagittalCheckBoxStateChanged(int)));
    macroManager->addMacroSupportToObject(m_volumeIndicesParasagittalCheckBox,
                                          "Enable parasagittal volume slice");
    
    m_volumeIndicesCoronalCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesCoronalCheckBox,
                                          "Enable/Disable display of CORONAL slice");
    m_volumeIndicesCoronalCheckBox->setObjectName(m_objectNamePrefix
                                                       + "EnableCoronalSlice");
    QObject::connect(m_volumeIndicesCoronalCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesCoronalCheckBoxStateChanged(int)));
    macroManager->addMacroSupportToObject(m_volumeIndicesCoronalCheckBox,
                                          "Enable coronal volume slice");
    
    m_volumeIndicesAxialCheckBox = new QCheckBox(" ");
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesAxialCheckBox,
                                          "Enable/Disable display of AXIAL slice");
    
    m_volumeIndicesAxialCheckBox->setObjectName(m_objectNamePrefix
                                                       + "EnableAxialSlice");
    QObject::connect(m_volumeIndicesAxialCheckBox, SIGNAL(stateChanged(int)),
                     this, SLOT(volumeIndicesAxialCheckBoxStateChanged(int)));
    macroManager->addMacroSupportToObject(m_volumeIndicesAxialCheckBox,
                                          "Enable axial volume slice");
    
    const int sliceIndexSpinBoxWidth = 55;
    const int sliceCoordinateSpinBoxWidth = 60;
    
    m_volumeIndicesParasagittalSpinBox = WuQFactory::newSpinBox();
    m_volumeIndicesParasagittalSpinBox->setFixedWidth(sliceIndexSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesParasagittalSpinBox,
                                          "Change the selected PARASAGITTAL slice");
    QObject::connect(m_volumeIndicesParasagittalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesParasagittalSpinBoxValueChanged(int)));
    m_volumeIndicesParasagittalSpinBox->setObjectName(m_objectNamePrefix
                                                      + "VolumeParasagittalSliceIndex");
    macroManager->addMacroSupportToObject(m_volumeIndicesParasagittalSpinBox,
                                          "Set parasagittal volume slice index");
    
    m_volumeIndicesCoronalSpinBox = WuQFactory::newSpinBox();
    m_volumeIndicesCoronalSpinBox->setFixedWidth(sliceIndexSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesCoronalSpinBox,
                                          "Change the selected CORONAL slice");
    QObject::connect(m_volumeIndicesCoronalSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesCoronalSpinBoxValueChanged(int)));
    m_volumeIndicesCoronalSpinBox->setObjectName(m_objectNamePrefix
                                                 + "VolumeCoronalSliceIndex");
    macroManager->addMacroSupportToObject(m_volumeIndicesCoronalSpinBox,
                                          "Set coronal volume slice index");
    
    m_volumeIndicesAxialSpinBox = WuQFactory::newSpinBox();
    m_volumeIndicesAxialSpinBox->setFixedWidth(sliceIndexSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesAxialSpinBox,
                                          "Change the selected AXIAL slice");
    QObject::connect(m_volumeIndicesAxialSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(volumeIndicesAxialSpinBoxValueChanged(int)));
    m_volumeIndicesAxialSpinBox->setObjectName(m_objectNamePrefix
                                               + "VolumeAxialSliceIndex");
    macroManager->addMacroSupportToObject(m_volumeIndicesAxialSpinBox,
                                          "Set axial volume slice index");
    
    m_volumeIndicesXcoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_volumeIndicesXcoordSpinBox->setDecimals(1);
    m_volumeIndicesXcoordSpinBox->setFixedWidth(sliceCoordinateSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesXcoordSpinBox,
                                          "Adjust coordinate to select PARASAGITTAL slice");
    QObject::connect(m_volumeIndicesXcoordSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeIndicesXcoordSpinBoxValueChanged(double)));
    m_volumeIndicesXcoordSpinBox->setObjectName(m_objectNamePrefix
                                                + "VolumeParasagittalCoordinate");
    macroManager->addMacroSupportToObject(m_volumeIndicesXcoordSpinBox,
                                          "Set parasagittal volume slice coordinate");
    
    m_volumeIndicesYcoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_volumeIndicesYcoordSpinBox->setDecimals(1);
    m_volumeIndicesYcoordSpinBox->setFixedWidth(sliceCoordinateSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesYcoordSpinBox,
                                          "Adjust coordinate to select CORONAL slice");
    QObject::connect(m_volumeIndicesYcoordSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeIndicesYcoordSpinBoxValueChanged(double)));
    m_volumeIndicesYcoordSpinBox->setObjectName(m_objectNamePrefix
                                                + "VolumeCoronalCoordinate");
    macroManager->addMacroSupportToObject(m_volumeIndicesYcoordSpinBox,
                                          "Set coronal volume slice coordinate");
    
    m_volumeIndicesZcoordSpinBox = WuQFactory::newDoubleSpinBox();
    m_volumeIndicesZcoordSpinBox->setDecimals(1);
    m_volumeIndicesZcoordSpinBox->setFixedWidth(sliceCoordinateSpinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_volumeIndicesZcoordSpinBox,
                                          "Adjust coordinate to select AXIAL slice");
    QObject::connect(m_volumeIndicesZcoordSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeIndicesZcoordSpinBoxValueChanged(double)));
    m_volumeIndicesZcoordSpinBox->setObjectName(m_objectNamePrefix
                                                + "VolumeAxialCoordinate");
    macroManager->addMacroSupportToObject(m_volumeIndicesZcoordSpinBox,
                                          "Set axial volume slice coordinate");
    
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
    QToolButton* volumeIDToolButton = new QToolButton;
    if (volumeCrossHairIconValid) {
        m_volumeIdentificationUpdatesSlicesAction->setIcon(volumeCrossHairIcon);
        m_volumeIdentificationUpdatesSlicesAction->setIcon(createVolumeIdentificationUpdatesSlicesIcon(volumeIDToolButton));
    }
    else {
        m_volumeIdentificationUpdatesSlicesAction->setText("ID");
    }
    volumeIDToolButton->setDefaultAction(m_volumeIdentificationUpdatesSlicesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumeIDToolButton);
    m_volumeIdentificationUpdatesSlicesAction->setObjectName(m_objectNamePrefix
                                                             + "MoveSliceToID");
    macroManager->addMacroSupportToObject(m_volumeIdentificationUpdatesSlicesAction,
                                          "Enable move volume slice to ID location");
    
    m_volumeSliceProjectionTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_volumeSliceProjectionTypeEnumComboBox->setup<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>();
    m_volumeSliceProjectionTypeEnumComboBox->getComboBox()->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QObject::connect(m_volumeSliceProjectionTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(volumeSliceProjectionTypeEnumComboBoxItemActivated()));
    WuQtUtilities::setToolTipAndStatusTip(m_volumeSliceProjectionTypeEnumComboBox->getWidget(),
                                          VolumeSliceProjectionTypeEnum::getToolTipForGuiInHtml());
    m_volumeSliceProjectionTypeEnumComboBox->getComboBox()->setObjectName(m_objectNamePrefix
                                                                          + "Orthogonal/Oblique");
    macroManager->addMacroSupportToObject(m_volumeSliceProjectionTypeEnumComboBox->getComboBox(),
                                          "Select volume slice projection type");
    m_volumeSliceProjectionTypeEnumComboBox->getComboBox()->setSizePolicy(QSizePolicy::Fixed,
                                                                          m_volumeSliceProjectionTypeEnumComboBox->getComboBox()->sizePolicy().verticalPolicy());
    
    /*
     * MPR Options action
     */
    m_mprOptionsAction = createMprOptionsAction();

    /*
     * Oblique options action
     */
    m_obliqueOptionsAction = createObliqueOptionsAction();
    
    /*
     * Options button for MPR and Oblique options
     * Add some text and set fixed width so that
     * button does not change size as its text
     * is updated by the MPR or Oblique QAction.
     */
    m_optionsToolButton = new QToolButton();
    m_optionsToolButton->setText("123456");
    m_optionsToolButton->setFixedWidth(m_optionsToolButton->sizeHint().width());
    m_optionsToolButton->setText("");
    if ( ! m_newOptionsToolButtonFlag) {
        m_optionsToolButton->setDefaultAction(m_mprOptionsAction);
    }
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_optionsToolButton);
    if (m_newOptionsToolButtonFlag) {
        m_optionsToolButton->setArrowType(Qt::DownArrow);
        QObject::connect(m_optionsToolButton, &QToolButton::clicked,
                         this, &BrainBrowserWindowToolBarSliceSelection::optionsToolButtonClicked);
    }
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->setHorizontalSpacing(2);
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

    QHBoxLayout* bottomLayout(new QHBoxLayout());
    bottomLayout->setSpacing(1);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->addWidget(volumeIDToolButton);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_volumeSliceProjectionTypeEnumComboBox->getWidget());
    bottomLayout->addWidget(m_optionsToolButton);
bottomLayout->setStretchFactor(m_volumeSliceProjectionTypeEnumComboBox->getWidget(), 100);
    
    gridLayout->addLayout(bottomLayout, 3, 0, 1, 5);

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
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_VOLUME_SLICE_INDICES_COORDS_TOOLBAR);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarSliceSelection::~BrainBrowserWindowToolBarSliceSelection()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
BrainBrowserWindowToolBarSliceSelection::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_UPDATE_VOLUME_SLICE_INDICES_COORDS_TOOLBAR) {
        m_volumeIndicesWidgetGroup->blockAllSignals(true);
        this->updateSliceIndicesAndCoordinatesRanges();
        m_volumeIndicesWidgetGroup->blockAllSignals(false);
        event->setEventProcessed();
    }
    else {
        BrainBrowserWindowToolBarSliceSelection::receiveEvent(event);
    }
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
         * Update slice projection type for allowed projection types
         */
        std::vector<VolumeSliceProjectionTypeEnum::Enum> validSliceProjections;
        browserTabContent->getValidVolumeSliceProjectionTypes(validSliceProjections);
        m_volumeSliceProjectionTypeEnumComboBox->setupWithItems<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>(validSliceProjections);

        m_volumeIndicesAxialCheckBox->setChecked(browserTabContent->isVolumeSliceAxialEnabled());
        m_volumeIndicesCoronalCheckBox->setChecked(browserTabContent->isVolumeSliceCoronalEnabled());
        m_volumeIndicesParasagittalCheckBox->setChecked(browserTabContent->isVolumeSliceParasagittalEnabled());
    }
    
    updateOptionsButton();

    m_volumeSliceProjectionTypeEnumComboBox->setSelectedItem<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>(browserTabContent->getVolumeSliceProjectionType());
    
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
        
        int64_t dimParasagittal(0), dimCoronal(0), dimAxial(0);
        vf->getDimensionsPCA(dimParasagittal, dimCoronal, dimAxial);
        
        /*
         * Setup minimum and maximum slices for each dimension.
         * Range is unlimited when Yoked.
         */
        const int minAxialDim = 0;
        const int minCoronalDim = 0;
        const int minParasagittalDim = 0;
        const int maxAxialDim = (dimAxial > 0) ? (dimAxial - 1) : 0;
        const int maxCoronalDim = (dimCoronal > 0) ? (dimCoronal - 1) : 0;
        const int maxParasagittalDim = (dimParasagittal > 0) ? (dimParasagittal - 1) : 0;
        
        /*
         * BUG NOTE:
         * On Linux, if the user hold down an arrow key in a spin box
         * and the time to process the signal is "slow", qt will emit 
         * a second signal (QTBUG-14259).  In addition, calling any of
         * the setMinimum(), setMaximum(), or setRange() methods seems
         * to also cause emission of a signal.  The result when the 
         * user releases the arrow key, there are many backlogged signals
         * and it may take a while for them to process and the user 
         * sees the slices scrolling for a while after the arrow key
         * is released.
         *
         * So, do not update range min/max unless the new range
         * is different than the range in the spin box.
         */
        if (m_volumeIndicesAxialSpinBox->minimum() != minAxialDim) {
            m_volumeIndicesAxialSpinBox->setMinimum(minAxialDim);
        }
        if (m_volumeIndicesAxialSpinBox->maximum() != maxAxialDim) {
            m_volumeIndicesAxialSpinBox->setMaximum(maxAxialDim);
        }
        
        if (m_volumeIndicesCoronalSpinBox->minimum() != minCoronalDim) {
            m_volumeIndicesCoronalSpinBox->setMinimum(minCoronalDim);
        }
        if (m_volumeIndicesCoronalSpinBox->maximum() != maxCoronalDim) {
            m_volumeIndicesCoronalSpinBox->setMaximum(maxCoronalDim);
        }
        
        if (m_volumeIndicesParasagittalSpinBox->minimum() != minParasagittalDim) {
            m_volumeIndicesParasagittalSpinBox->setMinimum(minParasagittalDim);
        }
        if (m_volumeIndicesParasagittalSpinBox->maximum() != maxParasagittalDim) {
            m_volumeIndicesParasagittalSpinBox->setMaximum(maxParasagittalDim);
        }
        
        
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
        
        BoundingBox boundingBox;
        vf->getVoxelSpaceBoundingBox(boundingBox);
        const double minX(boundingBox.getMinX());
        const double maxX(boundingBox.getMaxX());
        const double minY(boundingBox.getMinY());
        const double maxY(boundingBox.getMaxY());
        const double minZ(boundingBox.getMinZ());
        const double maxZ(boundingBox.getMaxZ());
        
        /*
         * See BUG NOTE above.
         */
        if (m_volumeIndicesXcoordSpinBox->minimum() != minX) {
            m_volumeIndicesXcoordSpinBox->setMinimum(minX);
        }
        if (m_volumeIndicesXcoordSpinBox->maximum() != maxX) {
            m_volumeIndicesXcoordSpinBox->setMaximum(maxX);
        }
        if (m_volumeIndicesYcoordSpinBox->minimum() != minY) {
            m_volumeIndicesYcoordSpinBox->setMinimum(minY);
        }
        if (m_volumeIndicesYcoordSpinBox->maximum() != maxY) {
            m_volumeIndicesYcoordSpinBox->setMaximum(maxY);
        }
        if (m_volumeIndicesZcoordSpinBox->minimum() != minZ) {
            m_volumeIndicesZcoordSpinBox->setMinimum(minZ);
        }
        if (m_volumeIndicesZcoordSpinBox->maximum() != maxZ) {
            m_volumeIndicesZcoordSpinBox->setMaximum(maxZ);
        }
        
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
        
        m_volumeIndicesAxialSpinBox->setValue(btc->getVolumeSliceIndexAxial(vf));
        m_volumeIndicesCoronalSpinBox->setValue(btc->getVolumeSliceIndexCoronal(vf));
        m_volumeIndicesParasagittalSpinBox->setValue(btc->getVolumeSliceIndexParasagittal(vf));
        
        int64_t slices[3] = {
            btc->getVolumeSliceIndexParasagittal(vf),
            btc->getVolumeSliceIndexCoronal(vf),
            btc->getVolumeSliceIndexAxial(vf)
        };
        float sliceCoords[3] = { 0.0, 0.0, 0.0 };
        if (vf != NULL) {
            vf->indexToSpace(slices,
                             sliceCoords);
        }
        m_volumeIndicesXcoordSpinBox->setValue(btc->getVolumeSliceCoordinateParasagittal());
        m_volumeIndicesYcoordSpinBox->setValue(btc->getVolumeSliceCoordinateCoronal());
        m_volumeIndicesZcoordSpinBox->setValue(btc->getVolumeSliceCoordinateAxial());
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
    btc->selectVolumeSlicesAtOrigin();
    
    updateContent(btc);
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume indices parasagittal check box is toggled.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesParasagittalCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setVolumeSliceParasagittalEnabled(m_volumeIndicesParasagittalCheckBox->isChecked());
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume indices coronal check box is toggled.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesCoronalCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setVolumeSliceCoronalEnabled(m_volumeIndicesCoronalCheckBox->isChecked());
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume indices axial check box is toggled.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeIndicesAxialCheckBoxStateChanged(int /*state*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setVolumeSliceAxialEnabled(m_volumeIndicesAxialCheckBox->isChecked());
    this->updateGraphicsWindowAndYokedWindows();
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
                switch (viewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        btc->setVolumeSliceIndexAxial(underlayVolumeFile,
                                                sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        btc->setVolumeSliceIndexCoronal(underlayVolumeFile,
                                                  sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        btc->setVolumeSliceIndexParasagittal(underlayVolumeFile,
                                                       sliceIndex);
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
                        btc->setVolumeSliceIndexAxial(underlayVolumeFile,
                                                sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        btc->setVolumeSliceIndexCoronal(underlayVolumeFile,
                                                  sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        btc->setVolumeSliceIndexParasagittal(underlayVolumeFile,
                                                       sliceIndex);
                        break;
                }
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
                switch (viewPlane) {
                    case VolumeSliceViewPlaneEnum::ALL:
                        CaretAssert(0);
                        break;
                    case VolumeSliceViewPlaneEnum::AXIAL:
                        btc->setVolumeSliceIndexAxial(underlayVolumeFile,
                                                sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::CORONAL:
                        btc->setVolumeSliceIndexCoronal(underlayVolumeFile,
                                                  sliceIndex);
                        break;
                    case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                        btc->setVolumeSliceIndexParasagittal(underlayVolumeFile,
                                                       sliceIndex);
                        break;
                }
                break;
        }
    }
    
    this->updateSliceIndicesAndCoordinatesRanges();
    
    this->updateGraphicsWindowAndYokedWindows();
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
            (float)m_volumeIndicesXcoordSpinBox->value(),
            (float)m_volumeIndicesYcoordSpinBox->value(),
            (float)m_volumeIndicesZcoordSpinBox->value()
        };
        
        btc->selectVolumeSlicesAtCoordinate(sliceCoords);
    }
    
    this->updateSliceIndicesAndCoordinatesRanges();
    
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when projection type is changed.
 */
void
BrainBrowserWindowToolBarSliceSelection::volumeSliceProjectionTypeEnumComboBoxItemActivated()
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType = m_volumeSliceProjectionTypeEnumComboBox->getSelectedItem<VolumeSliceProjectionTypeEnum,VolumeSliceProjectionTypeEnum::Enum>();
    btc->setVolumeSliceProjectionType(sliceProjectionType);
    this->updateGraphicsWindowAndYokedWindows();
    updateOptionsButton();
    EventManager::get()->sendEvent(EventUpdateVolumeEditingToolBar().getPointer());
    updateUserInterface();
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

void
BrainBrowserWindowToolBarSliceSelection::updateOptionsButton()
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    
    switch (browserTabContent->getVolumeSliceProjectionType()) {
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
            m_mprOptionsAction->setEnabled(false);
            m_obliqueOptionsAction->setEnabled(true);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
            m_mprOptionsAction->setEnabled(false);
            m_obliqueOptionsAction->setEnabled(false);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR:
            m_mprOptionsAction->setEnabled(true);
            m_obliqueOptionsAction->setEnabled(false);
            break;
        case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_THREE:
            m_mprOptionsAction->setEnabled(true);
            m_obliqueOptionsAction->setEnabled(false);
            break;
    }
        
    if (m_newOptionsToolButtonFlag) {
        if (m_mprOptionsAction->isEnabled()) {
            m_optionsToolButton->setEnabled(true);
            m_optionsToolButton->setToolTip("Click to set MPR intensity type and other options");
        }
        else if (m_obliqueOptionsAction->isEnabled()) {
            m_optionsToolButton->setEnabled(true);
            m_optionsToolButton->setToolTip("Click to change oblique masking");
        }
        else {
            m_optionsToolButton->setEnabled(false);
            m_optionsToolButton->setToolTip("");
        }
    }
    else {
        QList<QAction*> buttonActions(m_optionsToolButton->actions());
        for (auto action : buttonActions) {
            m_optionsToolButton->removeAction(action);
        }
        if (m_mprOptionsAction->isEnabled()) {
            m_optionsToolButton->setDefaultAction(m_mprOptionsAction);
            updateMprOptionsAction();
        }
        else {
            m_optionsToolButton->setDefaultAction(m_obliqueOptionsAction);
            updateObliqueOptionsAction();
        }
    }
}

/**
 * Create a pixmap for the volume identification updates slice selection button.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @return
 *    The pixmap.
 */
QPixmap
BrainBrowserWindowToolBarSliceSelection::createVolumeIdentificationUpdatesSlicesIcon(const QWidget* widget)
{
    CaretAssert(widget);
    const int pixmapSize = 24;
    const int halfSize = pixmapSize / 2;
    
    QPixmap pixmap(pixmapSize,
                   pixmapSize);
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter(widget,
                                                                                            pixmap,
                                                                                            static_cast<uint32_t>(WuQtUtilities::PixMapCreationOptions::TransparentBackground));
    const int startXY = 3;
    const int endXY   = 8;
    QPen pen(painter->pen());
    pen.setWidth(2);
    painter->setPen(pen);
    const int tx(-3);
    const int ty(3);
    painter->translate(tx, ty);
    painter->drawLine(-startXY, 0, -endXY, 0);
    painter->drawLine( startXY, 0,  endXY, 0);
    painter->drawLine(0, -startXY, 0, -endXY);
    painter->drawLine(0,  startXY, 0,  endXY);
    painter->translate(-tx, -ty);
    
    const int tipX(3);
    const int tipY(-3);
    const int tailX(halfSize);
    const int tailY(-halfSize);
    painter->drawLine(tipX, tipY, tailX, tailY);
    
    const int headLength(3);
    painter->drawLine(tipX, tipY, tipX + headLength, tipY);
    painter->drawLine(tipX, tipY, tipX, tipY - headLength);
    
    return pixmap;
}

/**
 * Called when options tool button is clicked
 */
void
BrainBrowserWindowToolBarSliceSelection::optionsToolButtonClicked()
{
    if (m_mprOptionsAction->isEnabled()) {
        updateMprOptionsAction();
        m_mprOptionsMenu->exec(m_optionsToolButton->mapToGlobal(QPoint(0, m_optionsToolButton->height())));
    }
    else if (m_obliqueOptionsAction->isEnabled()) {
        updateObliqueOptionsAction();
        m_obliqueOptionsMenu->exec(m_optionsToolButton->mapToGlobal(QPoint(0, m_optionsToolButton->height())));
    }
}

/**
 * @return Instance of the MPR options
 */
QMenu*
BrainBrowserWindowToolBarSliceSelection::createMprOptionsMenu()
{
    m_volumeMprSettingsWidget = new VolumeMprSettingsWidget(m_objectNamePrefix);
    QWidgetAction* mprSettingsAction = new QWidgetAction(this);
    mprSettingsAction->setDefaultWidget(m_volumeMprSettingsWidget);
    
    QMenu* menu = new QMenu(this);
    menu->addAction(mprSettingsAction);
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBarSliceSelection::mprOptionsMenuAboutToShow);
    return menu;
}

/**
 * @return New instance of MPR options action
 */
QAction*
BrainBrowserWindowToolBarSliceSelection::createMprOptionsAction()
{
    const QString toolTip("Click button to cycle through MPR modes "
                          "or click arrow to select a mode and adjust "
                          "other options");
    QAction* action = new QAction(this);
    action->setCheckable(false);
    action->setText("MPR");
    WuQtUtilities::setWordWrappedToolTip(action,
                                         toolTip);
    m_mprOptionsMenu = createMprOptionsMenu();
    action->setMenu(m_mprOptionsMenu);
    action->setObjectName(m_objectNamePrefix
                          + ":MprOptions");
    QObject::connect(action, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSliceSelection::mprOptionsActionTriggered);
    WuQMacroManager::instance()->addMacroSupportToObject(action,
                                                         "View MPR Options");
    return action;
}

/**
 * Called when MPR options action is triggered
 */
void
BrainBrowserWindowToolBarSliceSelection::mprOptionsActionTriggered(bool)
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        VolumeMprViewModeEnum::Enum viewMode(browserTabContent->getVolumeMprViewMode());
        viewMode = VolumeMprViewModeEnum::nextEnum(viewMode);
        browserTabContent->setVolumeMprViewMode(viewMode);
        updateMprOptionsAction();
        updateGraphicsWindowAndYokedWindows();
    }
}

/**
 * Update the MPR options action
 */
void
BrainBrowserWindowToolBarSliceSelection::updateMprOptionsAction()
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        VolumeMprViewModeEnum::Enum viewMode(browserTabContent->getVolumeMprViewMode());

        const AString text(VolumeMprViewModeEnum::toShortGuiName(viewMode));
        m_mprOptionsAction->setText(text);
    }
}

/**
 * Called when MPR options menu is about to show
 */
void
BrainBrowserWindowToolBarSliceSelection::mprOptionsMenuAboutToShow()
{
    int32_t tabIndex = -1;
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        tabIndex = browserTabContent->getTabNumber();
    }
    
    m_volumeMprSettingsWidget->updateContent(tabIndex);
}

/**
 * @return New instance of oblique options action
 */
QAction*
BrainBrowserWindowToolBarSliceSelection::createObliqueOptionsAction()
{
    const QString toolTip("Click button to cycle through Oblique masking modes "
                          "or click arrow to select masking mode directly");
    QAction* action = new QAction(this);
    action->setCheckable(false);
    action->setText("Opts");
    WuQtUtilities::setWordWrappedToolTip(action,
                                         toolTip);
    m_obliqueOptionsMenu = createObliqueOptionsMenu();
    action->setMenu(m_obliqueOptionsMenu);
    action->setObjectName(m_objectNamePrefix
                                          + ":ObliqueOptions");
    QObject::connect(action, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSliceSelection::obliqueOptionsActionTriggered);
    WuQMacroManager::instance()->addMacroSupportToObject(action,
                                                         "View Oblique Options");
    return action;
}

/**
 * @return Instance of the Oblique Options
 */
QMenu*
BrainBrowserWindowToolBarSliceSelection::createObliqueOptionsMenu()
{
    m_obliqueOptionsWidget = new VolumeObliqueOptionsWidget(m_objectNamePrefix);
    QWidgetAction* obliqueOptionsAction = new QWidgetAction(this);
    obliqueOptionsAction->setDefaultWidget(m_obliqueOptionsWidget);
    
    QMenu* menu = new QMenu(this);
    menu->addAction(obliqueOptionsAction);
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBarSliceSelection::obliqueOptionsMenuAboutToShow);
    return menu;
}

/**
 * Called when MPR options menu is about to show
 */
void
BrainBrowserWindowToolBarSliceSelection::obliqueOptionsMenuAboutToShow()
{
    int32_t tabIndex = -1;
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        tabIndex = browserTabContent->getTabNumber();
    }
    
    m_obliqueOptionsWidget->updateContent(tabIndex);
}

/**
 * Called when Oblique options action is triggered
 */
void
BrainBrowserWindowToolBarSliceSelection::obliqueOptionsActionTriggered(bool)
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskEnum(browserTabContent->getVolumeSliceInterpolationEdgeEffectsMaskingType());
        maskEnum = VolumeSliceInterpolationEdgeEffectsMaskingEnum::nextEnum(maskEnum);
        browserTabContent->setVolumeSliceInterpolationEdgeEffectsMaskingType(maskEnum);
        updateObliqueOptionsAction();
        updateGraphicsWindowAndYokedWindows();
    }
}

/**
 * Update the Oblique options action
 */
void
BrainBrowserWindowToolBarSliceSelection::updateObliqueOptionsAction()
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskEnum(browserTabContent->getVolumeSliceInterpolationEdgeEffectsMaskingType());

        const AString text(VolumeSliceInterpolationEdgeEffectsMaskingEnum::toShortGuiName(maskEnum));
        m_obliqueOptionsAction->setText(text);
    }
}
