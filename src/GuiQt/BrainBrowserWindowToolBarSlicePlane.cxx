
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
    
    m_volumePlaneCoronalToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_CORONAL,
                                                               this);
    m_volumePlaneCoronalToolButtonAction->setToolTip("View the CORONAL slice");
    m_volumePlaneCoronalToolButtonAction->setCheckable(true);
    m_volumePlaneCoronalToolButtonAction->setObjectName(objectNamePrefix
                                                        + "CoronalSliceView");
    macroManager->addMacroSupportToObject(m_volumePlaneCoronalToolButtonAction,
                                          "Select coronal slice view");
    
    m_volumePlaneAxialToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_AXIAL,
                                                             this);
    m_volumePlaneAxialToolButtonAction->setToolTip("View the AXIAL slice");
    m_volumePlaneAxialToolButtonAction->setCheckable(true);
    m_volumePlaneAxialToolButtonAction->setObjectName(objectNamePrefix
                                                      + "AxialSliceView");
    macroManager->addMacroSupportToObject(m_volumePlaneAxialToolButtonAction,
                                          "Select axial slice view");
    
    m_volumePlaneAllToolButtonAction = new WorkbenchAction(WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ALL,
                                                           this);
    m_volumePlaneAllToolButtonAction->setToolTip("View the PARASAGITTAL, CORONAL, and AXIAL slices\n"
                                                 "Press arrow to display menu for layout selection");
    m_volumePlaneAllToolButtonAction->setCheckable(true);
    m_volumePlaneAllToolButtonAction->setMenu(createViewAllSlicesLayoutMenu(objectNamePrefix));
    m_volumePlaneAllToolButtonAction->setObjectName(objectNamePrefix
                                                    + "AllSlicesView");
    macroManager->addMacroSupportToObject(m_volumePlaneAllToolButtonAction,
                                          "Select all planes view");
    
    
    m_volumePlaneActionGroup = new QActionGroup(this);
    m_volumePlaneActionGroup->addAction(m_volumePlaneParasagittalToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneCoronalToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneAxialToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneAllToolButtonAction);
    m_volumePlaneActionGroup->setExclusive(true);
    QObject::connect(m_volumePlaneActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(volumePlaneActionGroupTriggered(QAction*)));
        
    QToolButton* volumePlaneParasagittalToolButton = new WorkbenchToolButton();
    volumePlaneParasagittalToolButton->setDefaultAction(m_volumePlaneParasagittalToolButtonAction);
    m_volumePlaneParasagittalToolButtonAction->setParent(volumePlaneParasagittalToolButton);
    
    QToolButton* volumePlaneCoronalToolButton = new WorkbenchToolButton();
    volumePlaneCoronalToolButton->setDefaultAction(m_volumePlaneCoronalToolButtonAction);
    m_volumePlaneCoronalToolButtonAction->setParent(volumePlaneCoronalToolButton);
    
    QToolButton* volumePlaneAxialToolButton = new WorkbenchToolButton();
    volumePlaneAxialToolButton->setDefaultAction(m_volumePlaneAxialToolButtonAction);
    m_volumePlaneAxialToolButtonAction->setParent(volumePlaneAxialToolButton);
    
    QToolButton* volumePlaneAllToolButton = new WorkbenchToolButton();
    volumePlaneAllToolButton->setDefaultAction(m_volumePlaneAllToolButtonAction);
    m_volumePlaneAllToolButtonAction->setParent(volumePlaneAllToolButton);
    
    WuQtUtilities::matchWidgetHeights(volumePlaneParasagittalToolButton,
                                      volumePlaneCoronalToolButton,
                                      volumePlaneAxialToolButton,
                                      volumePlaneAllToolButton);
    WuQtUtilities::matchWidgetWidths(volumePlaneParasagittalToolButton,
                                     volumePlaneCoronalToolButton,
                                     volumePlaneAxialToolButton,
                                     volumePlaneAllToolButton);
    
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
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(volumePlaneParasagittalToolButton, rowIndex, 0);
    gridLayout->addWidget(volumePlaneCoronalToolButton,      rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(volumePlaneAxialToolButton, rowIndex, 0);
    gridLayout->addWidget(volumePlaneAllToolButton,   rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(volumeCrosshairsToolButton, rowIndex, 0, Qt::AlignRight);
    gridLayout->addWidget(volumeCrosshairLabelsToolButton, rowIndex, 1, Qt::AlignLeft);
    
    m_volumePlaneWidgetGroup = new WuQWidgetObjectGroup(this);
    m_volumePlaneWidgetGroup->add(m_volumePlaneActionGroup);
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
    m_volumePlaneWidgetGroup->blockAllSignals(true);
    
    switch (browserTabContent->getVolumeSliceViewPlane()) {
        case VolumeSliceViewPlaneEnum::ALL:
            m_volumePlaneAllToolButtonAction->setChecked(true);
            break;
        case VolumeSliceViewPlaneEnum::AXIAL:
            m_volumePlaneAxialToolButtonAction->setChecked(true);
            break;
        case VolumeSliceViewPlaneEnum::CORONAL:
            m_volumePlaneCoronalToolButtonAction->setChecked(true);
            break;
        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
            m_volumePlaneParasagittalToolButtonAction->setChecked(true);
            break;
    }
    
    updateViewAllSlicesLayoutMenu(browserTabContent);
    
    m_volumeAxisCrosshairsToolButtonAction->setChecked(browserTabContent->isVolumeAxesCrosshairsDisplayed());
    m_volumeAxisCrosshairLabelsToolButtonAction->setChecked(browserTabContent->isVolumeAxesCrosshairLabelsDisplayed());
                                                            
    m_volumePlaneWidgetGroup->blockAllSignals(false);
}

/**
 * @return A new instance of the view all slices layout menu.
 * @param objectNamePrefix
 *     Prefix for object names in macro system
 */
QMenu*
BrainBrowserWindowToolBarSlicePlane::createViewAllSlicesLayoutMenu(const QString& objectNamePrefix)
{
    std::vector<VolumeSliceViewAllPlanesLayoutEnum::Enum> allLayouts;
    VolumeSliceViewAllPlanesLayoutEnum::getAllEnums(allLayouts);
    
    QMenu* menu = new QMenu(this);
    menu->setObjectName(objectNamePrefix
                        + "LayoutMenu");
    menu->setToolTip("Selects layout of volume slices (column, grid, row)");
    QActionGroup* actionGroup = new QActionGroup(this);
    
    for (auto layout : allLayouts) {
        QAction* action = menu->addAction(VolumeSliceViewAllPlanesLayoutEnum::toGuiName(layout));
        action->setData((int)VolumeSliceViewAllPlanesLayoutEnum::toIntegerCode(layout));
        action->setCheckable(true);
        action->setObjectName(objectNamePrefix
                              + ":Layout:"
                              + VolumeSliceViewAllPlanesLayoutEnum::toName(layout));
        m_viewAllSliceLayoutMenuActions.push_back(action);
        actionGroup->addAction(action);

        WuQMacroManager::instance()->addMacroSupportToObject(action,
                                                             "Select "
                                                             + VolumeSliceViewAllPlanesLayoutEnum::toGuiName(layout)
                                                             + " volume slice layout");
    }

    QObject::connect(menu, &QMenu::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::viewAllSliceLayoutMenuTriggered);
    return menu;
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
 * Gets called when the user selects an item on the view all slices layout menu.
 *
 * @param action
 *     Action of menu item selected.
 */
void
BrainBrowserWindowToolBarSlicePlane::viewAllSliceLayoutMenuTriggered(QAction* action)
{
    const int layoutInt = action->data().toInt();
    bool validFlag = false;
    const VolumeSliceViewAllPlanesLayoutEnum::Enum layout = VolumeSliceViewAllPlanesLayoutEnum::fromIntegerCode(layoutInt, &validFlag);
    if (validFlag) {
        BrowserTabContent* btc = getTabContentFromSelectedTab();
        
        btc->setVolumeSlicePlanesAllViewLayout(layout);
        
        m_parentToolBar->updateVolumeIndicesWidget(btc);
        updateGraphicsWindowAndYokedWindows();
    }
    else {
        CaretLogSevere("Invalid layout in menu item: "
                       + action->text());
    }
}

/**
 * Update the view all slices layout menu.
 */
void
BrainBrowserWindowToolBarSlicePlane::updateViewAllSlicesLayoutMenu(BrowserTabContent* browserTabContent)
{
    const VolumeSliceViewAllPlanesLayoutEnum::Enum layout = browserTabContent->getVolumeSlicePlanesAllViewLayout();
    const int layoutIntValue = VolumeSliceViewAllPlanesLayoutEnum::toIntegerCode(layout);
    
    for (auto action : m_viewAllSliceLayoutMenuActions) {
        if (action->data().toInt() == layoutIntValue) {
            action->setChecked(true);
            break;
        }
    }
}

/**
 * Called when volume slice plane button is clicked.
 */
void
BrainBrowserWindowToolBarSlicePlane::volumePlaneActionGroupTriggered(QAction* action)
{
    VolumeSliceViewPlaneEnum::Enum plane = VolumeSliceViewPlaneEnum::AXIAL;
    
    if (action == m_volumePlaneAllToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::ALL;
    }
    else if (action == m_volumePlaneAxialToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::AXIAL;
        
    }
    else if (action == m_volumePlaneCoronalToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::CORONAL;
        
    }
    else if (action == m_volumePlaneParasagittalToolButtonAction) {
        plane = VolumeSliceViewPlaneEnum::PARASAGITTAL;
    }
    else {
        CaretLogSevere("Invalid volume plane action: " + action->text());
    }
    
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    
    btc->setVolumeSliceViewPlane(plane);
    
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
