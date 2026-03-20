
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_DECLARE__
#include "BrainBrowserWindowToolBarSlicePlane.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SLICE_PLANE_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QWidgetAction>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferenceDataValue.h"
#include "CaretPreferences.h"
#include "GuiManager.h"
#include "SessionManager.h"
#include "WorkbenchAction.h"
#include "WorkbenchToolButton.h"
#include "WuQMacroManager.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::BrainBrowserWindowToolBarSlicePlane 
 * \brief Toolbar component for volume slice plane and projection selection.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * 
 * @param parentToolBar
 *     The parent toolbar
 */
BrainBrowserWindowToolBarSlicePlane::BrainBrowserWindowToolBarSlicePlane(const QString& parentObjectName,
                                                                         BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    const QString objectNamePrefix(parentObjectName
                                   + ":SlicePlane:");
    
    m_volumePlaneParasagittalToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL,
                                                                    this);
    m_volumePlaneParasagittalToolButtonAction->setToolTip("View the PARASAGITTAL slice");
    m_volumePlaneParasagittalToolButtonAction->setCheckable(true);
    m_volumePlaneParasagittalToolButtonAction->setObjectName(objectNamePrefix
                                                             + "ParasagittalSliceView");
    macroManager->addMacroSupportToObject(m_volumePlaneParasagittalToolButtonAction,
                                          "Select parasagittal slice view");
    QObject::connect(m_volumePlaneParasagittalToolButtonAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::volumePlaneParasagittalToolButtonActionTriggered);

    m_volumePlaneCoronalToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_CORONAL,
                                                               this);
    m_volumePlaneCoronalToolButtonAction->setToolTip("View the CORONAL slice");
    m_volumePlaneCoronalToolButtonAction->setCheckable(true);
    m_volumePlaneCoronalToolButtonAction->setObjectName(objectNamePrefix
                                                        + "CoronalSliceView");
    macroManager->addMacroSupportToObject(m_volumePlaneCoronalToolButtonAction,
                                          "Select coronal slice view");
    QObject::connect(m_volumePlaneCoronalToolButtonAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::volumePlaneCoronalToolButtonActionTriggered);

    m_volumePlaneAxialToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_AXIAL,
                                                             this);
    m_volumePlaneAxialToolButtonAction->setToolTip("View the AXIAL slice");
    m_volumePlaneAxialToolButtonAction->setCheckable(true);
    m_volumePlaneAxialToolButtonAction->setObjectName(objectNamePrefix
                                                      + "AxialSliceView");
    macroManager->addMacroSupportToObject(m_volumePlaneAxialToolButtonAction,
                                          "Select axial slice view");
    QObject::connect(m_volumePlaneAxialToolButtonAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::volumePlaneAxialToolButtonActionTriggered);
    
    m_volumePlaneAllToolButtonAction = NULL;
    const bool showVolumePlaneAllToolButton(false);
    if (showVolumePlaneAllToolButton) {
        m_volumePlaneAllToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ALL,
                                                               this);
        m_volumePlaneAllToolButtonAction->setToolTip("View the PARASAGITTAL, CORONAL, and AXIAL slices\n"
                                                     "Press arrow to display menu for layout selection");
        m_volumePlaneAllToolButtonAction->setCheckable(true);
        m_volumePlaneAllToolButtonAction->setObjectName(objectNamePrefix
                                                        + "AllSlicesView");
        macroManager->addMacroSupportToObject(m_volumePlaneAllToolButtonAction,
                                              "Select all planes view");
    }
    
    QToolButton* volumePlaneParasagittalToolButton = new WorkbenchToolButton();
    volumePlaneParasagittalToolButton->setDefaultAction(m_volumePlaneParasagittalToolButtonAction);
    m_volumePlaneParasagittalToolButtonAction->setParent(volumePlaneParasagittalToolButton);
    
    QToolButton* volumePlaneCoronalToolButton = new WorkbenchToolButton();
    volumePlaneCoronalToolButton->setDefaultAction(m_volumePlaneCoronalToolButtonAction);
    m_volumePlaneCoronalToolButtonAction->setParent(volumePlaneCoronalToolButton);
    
    QToolButton* volumePlaneAxialToolButton = new WorkbenchToolButton();
    volumePlaneAxialToolButton->setDefaultAction(m_volumePlaneAxialToolButtonAction);
    m_volumePlaneAxialToolButtonAction->setParent(volumePlaneAxialToolButton);
    
    QToolButton* volumePlaneAllToolButton(NULL);
    if (m_volumePlaneAllToolButtonAction != NULL) {
        volumePlaneAllToolButton= new WorkbenchToolButton();
        volumePlaneAllToolButton->setDefaultAction(m_volumePlaneAllToolButtonAction);
        m_volumePlaneAllToolButtonAction->setParent(volumePlaneAllToolButton);
        CaretAssertMessage(0, "Will need to add to match widget sizes below");
    }
    
    WuQtUtilities::matchWidgetHeights(volumePlaneParasagittalToolButton,
                                      volumePlaneCoronalToolButton,
                                      volumePlaneAxialToolButton);
    WuQtUtilities::matchWidgetWidths(volumePlaneParasagittalToolButton,
                                     volumePlaneCoronalToolButton,
                                     volumePlaneAxialToolButton);
    
    m_volumeAxisCrosshairsToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIRS,
                                                                 this);
    m_volumeAxisCrosshairsToolButtonAction->setToolTip("<html>"
                                                       "Show crosshairs on slice planes.<br>"
                                                       "Press arrow to adjust gap"
                                                       "<html>");
    m_volumeAxisCrosshairsToolButtonAction->setCheckable(true);
    m_volumeAxisCrosshairsToolButtonAction->setMenu(createCrosshairMenu(objectNamePrefix));
    QObject::connect(m_volumeAxisCrosshairsToolButtonAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::volumeAxisCrosshairsTriggered);
    m_volumeAxisCrosshairsToolButtonAction->setObjectName(objectNamePrefix
                                                          + "ShowVolumeSliceCrosshairs");
    macroManager->addMacroSupportToObject(m_volumeAxisCrosshairsToolButtonAction,
                                          "Show slice axis crosshairs");
    
    QToolButton* volumeCrosshairsToolButton = new WorkbenchToolButton(WorkbenchToolButton::MenuStatus::MENU_YES);
    volumeCrosshairsToolButton->setDefaultAction(m_volumeAxisCrosshairsToolButtonAction);
    volumeCrosshairsToolButton->setIconSize(QSize(22, 22));
    
    m_volumeAxisCrosshairLabelsToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS,
                                                                      this);
    m_volumeAxisCrosshairLabelsToolButtonAction->setCheckable(true);
    m_volumeAxisCrosshairLabelsToolButtonAction->setToolTip("Show crosshair slice plane labels");
    QObject::connect(m_volumeAxisCrosshairLabelsToolButtonAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::volumeAxisCrosshairLabelsTriggered);
    m_volumeAxisCrosshairLabelsToolButtonAction->setObjectName(objectNamePrefix
                                                               + "ShowVolumeSliceLabels");
    macroManager->addMacroSupportToObject(m_volumeAxisCrosshairLabelsToolButtonAction,
                                          "Show slice axis labels");
    
    QToolButton* volumeCrosshairLabelsToolButton = new WorkbenchToolButton();
    volumeCrosshairLabelsToolButton->setDefaultAction(m_volumeAxisCrosshairLabelsToolButtonAction);
    volumeCrosshairLabelsToolButton->setIconSize(QSize(22, 22));
    
    m_volumeLayoutComboBox = new QComboBox();
    m_volumeLayoutComboBox->setToolTip("Selects layout of volume slices (column, grid, row)");
    std::vector<VolumeSliceViewAllPlanesLayoutEnum::Enum> allLayouts;
    VolumeSliceViewAllPlanesLayoutEnum::getAllEnums(allLayouts);
    for (auto layout : allLayouts) {
        m_volumeLayoutComboBox->addItem(VolumeSliceViewAllPlanesLayoutEnum::toGuiName(layout));
        m_volumeLayoutComboBox->setItemData(m_volumeLayoutComboBox->count() - 1,
                                            VolumeSliceViewAllPlanesLayoutEnum::toIntegerCode(layout));
    }
    QObject::connect(m_volumeLayoutComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &BrainBrowserWindowToolBarSlicePlane::volumeLayoutComboBoxActivated);
    m_volumeLayoutComboBox->setObjectName(objectNamePrefix +
                                          "LayoutComboBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_volumeLayoutComboBox,
                                                         "Select volume slice layout");

    QHBoxLayout* planeLayout(new QHBoxLayout());
    planeLayout->setContentsMargins(0, 0, 0, 0);
    planeLayout->setSpacing(2);
    planeLayout->addStretch();
    planeLayout->addWidget(volumePlaneParasagittalToolButton);
    planeLayout->addWidget(volumePlaneCoronalToolButton);
    planeLayout->addWidget(volumePlaneAxialToolButton);
    planeLayout->addStretch();

    QHBoxLayout* crosshairLayout(new QHBoxLayout());
    crosshairLayout->setContentsMargins(0, 0, 0, 0);
    crosshairLayout->setSpacing(5);
    crosshairLayout->addStretch();
    crosshairLayout->addWidget(volumeCrosshairsToolButton);
    crosshairLayout->addWidget(volumeCrosshairLabelsToolButton);
    crosshairLayout->addStretch();
    
    QVBoxLayout* layout(new QVBoxLayout(this));
    layout->setContentsMargins(0, 2, 0, 2);
    layout->setSpacing(2);
    layout->addLayout(planeLayout);
    layout->addWidget(m_volumeLayoutComboBox);
    layout->addLayout(crosshairLayout);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarSlicePlane::~BrainBrowserWindowToolBarSlicePlane()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarSlicePlane::updateContent(BrowserTabContent* browserTabContent)
{
    m_volumePlaneAxialToolButtonAction->setChecked(browserTabContent->isShowVolumeViewAxialSlice());
    m_volumePlaneCoronalToolButtonAction->setChecked(browserTabContent->isShowVolumeViewCoronalSlice());
    m_volumePlaneParasagittalToolButtonAction->setChecked(browserTabContent->isShowVolumeViewParasagittalSlice());

    const VolumeSliceViewAllPlanesLayoutEnum::Enum layout(browserTabContent->getVolumeSlicePlanesAllViewLayout());
    const int32_t layoutInt(VolumeSliceViewAllPlanesLayoutEnum::toIntegerCode(layout));
    for (int32_t i = 0; i < m_volumeLayoutComboBox->count(); i++) {
        if (m_volumeLayoutComboBox->itemData(i).toInt() == layoutInt) {
            m_volumeLayoutComboBox->setCurrentIndex(i);
            break;
        }
    }

    m_volumeAxisCrosshairsToolButtonAction->setChecked(browserTabContent->isVolumeAxesCrosshairsDisplayed());
    m_volumeAxisCrosshairLabelsToolButtonAction->setChecked(browserTabContent->isVolumeAxesCrosshairLabelsDisplayed());
}

/**
 * @return A new instance of the crosshair menu
 * @param objectNamePrefix
 *     Prefix for object names in macro system
 * @return
 *     Menu for crosshair button
 */
QMenu*
BrainBrowserWindowToolBarSlicePlane::createCrosshairMenu(const QString& objectNamePrefix)
{
    m_crosshairGapSpinBox = new QDoubleSpinBox();
    m_crosshairGapSpinBox->setMinimum(0.0);
    m_crosshairGapSpinBox->setMaximum(100);
    m_crosshairGapSpinBox->setSingleStep(1.0);
    m_crosshairGapSpinBox->setDecimals(1);
    m_crosshairGapSpinBox->setSuffix("%");
    m_crosshairGapSpinBox->setObjectName(objectNamePrefix
                                         + "CrossHairGapSinBox");
    m_crosshairGapSpinBox->setToolTip("Gap for cross hairs as percentage of window height");
    
    QObject::connect(m_crosshairGapSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarSlicePlane::crosshairGapSpinBoxValueChanged);
    
    QLabel* crosshairLabel = new QLabel("Gap ");
    
    QWidget* crosshairWidget = new QWidget();
    QHBoxLayout* crosshairLayout = new QHBoxLayout(crosshairWidget);
    crosshairLayout->setContentsMargins(0, 0, 0, 0);
    crosshairLayout->addWidget(crosshairLabel);
    crosshairLayout->addWidget(m_crosshairGapSpinBox);
    crosshairWidget->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    
    QWidgetAction* crossHairGapWidgetAction = new QWidgetAction(this);
    crossHairGapWidgetAction->setDefaultWidget(crosshairWidget);
    
    QMenu* menu = new QMenu(this);
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBarSlicePlane::crosshairMenuAboutToShow);
    menu->addAction(crossHairGapWidgetAction);
    
    return menu;
}

/**
 * Called when crosshair menu is about to show
 */
void
BrainBrowserWindowToolBarSlicePlane::crosshairMenuAboutToShow()
{
    const float gapValue = SessionManager::get()->getCaretPreferences()->getVolumeCrosshairGap();
    QSignalBlocker blocker(m_crosshairGapSpinBox);
    m_crosshairGapSpinBox->setValue(gapValue);
}

/**
 * Called when crosshair gap spin box value changed
 *
 * @param value
 *     New value for crosshair gap.
 */
void
BrainBrowserWindowToolBarSlicePlane::crosshairGapSpinBoxValueChanged(double value)
{
    SessionManager::get()->getCaretPreferences()->setVolumeCrosshairGap(value);
    GuiManager::updateGraphicsAllWindows();
}

/**
 * Called when volume slice Axial button action is triggered
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarSlicePlane::volumePlaneAxialToolButtonActionTriggered(bool checked)
{
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    btc->setShowVolumeViewAxialSlice(checked);
    m_parentToolBar->updateVolumeIndicesWidget(btc);
    updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume slice Coronal button action is triggered
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarSlicePlane::volumePlaneCoronalToolButtonActionTriggered(bool checked)
{
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    btc->setShowVolumeViewCoronalSlice(checked);
    m_parentToolBar->updateVolumeIndicesWidget(btc);
    updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume slice Parasagittal button action is triggered
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarSlicePlane::volumePlaneParasagittalToolButtonActionTriggered(bool checked)
{
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    btc->setShowVolumeViewParasagittalSlice(checked);
    m_parentToolBar->updateVolumeIndicesWidget(btc);
    updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume axis crosshairs button triggered
 *
 * @param checked
 *     New checked status
 */
void
BrainBrowserWindowToolBarSlicePlane::volumeAxisCrosshairsTriggered(bool checked)
{
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    btc->setVolumeAxesCrosshairsDisplayed(checked);
    updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume axis crosshair labels button triggered
 *
 * @param checked
 *     New checked status
 */
void
BrainBrowserWindowToolBarSlicePlane::volumeAxisCrosshairLabelsTriggered(bool checked)
{
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    btc->setVolumeAxesCrosshairLabelsDisplayed(checked);
    updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when an item is selected from the volume layout combo box
 * @param index
 *    Index of item selected
 */
void
BrainBrowserWindowToolBarSlicePlane::volumeLayoutComboBoxActivated(int index)
{
    if ((index >= 0)
        && (index < m_volumeLayoutComboBox->count())) {
        bool validFlag(false);
        const VolumeSliceViewAllPlanesLayoutEnum::Enum layout =
        VolumeSliceViewAllPlanesLayoutEnum::fromIntegerCode(m_volumeLayoutComboBox->itemData(index).toInt(),
                                                            &validFlag);
        BrowserTabContent* btc = getTabContentFromSelectedTab();
        btc->setVolumeSlicePlanesAllViewLayout(layout);
        updateGraphicsWindowAndYokedWindows();
    }
}

