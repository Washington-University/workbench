
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
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QToolButton>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
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
 */
BrainBrowserWindowToolBarSlicePlane::BrainBrowserWindowToolBarSlicePlane(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QIcon parasagittalIcon;
    const bool parasagittalIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/view-plane-parasagittal.png",
                            parasagittalIcon);
    
    QIcon coronalIcon;
    const bool coronalIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/view-plane-coronal.png",
                            coronalIcon);
    
    QIcon axialIcon;
    const bool axialIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/view-plane-axial.png",
                            axialIcon);
    
    m_volumePlaneParasagittalToolButtonAction =
    WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::PARASAGITTAL),
                                "View the PARASAGITTAL slice",
                                this);
    m_volumePlaneParasagittalToolButtonAction->setCheckable(true);
    if (parasagittalIconValid) {
        m_volumePlaneParasagittalToolButtonAction->setIcon(parasagittalIcon);
    }
    
    m_volumePlaneCoronalToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::CORONAL),
                                                                           "View the CORONAL slice",
                                                                           this);
    m_volumePlaneCoronalToolButtonAction->setCheckable(true);
    if (coronalIconValid) {
        m_volumePlaneCoronalToolButtonAction->setIcon(coronalIcon);
    }
    
    m_volumePlaneAxialToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::AXIAL),
                                                                         "View the AXIAL slice",
                                                                         this);
    m_volumePlaneAxialToolButtonAction->setCheckable(true);
    if (axialIconValid) {
        m_volumePlaneAxialToolButtonAction->setIcon(axialIcon);
    }
    
    m_volumePlaneAllToolButtonAction = WuQtUtilities::createAction(VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::ALL),
                                                                   "View the PARASAGITTAL, CORONAL, and AXIAL slices\n"
                                                                   "Press arrow to display menu for layout selection",
                                                                       this);
    m_volumePlaneAllToolButtonAction->setCheckable(true);
    m_volumePlaneAllToolButtonAction->setMenu(createViewAllSlicesLayoutMenu());
    
    
    m_volumePlaneActionGroup = new QActionGroup(this);
    m_volumePlaneActionGroup->addAction(m_volumePlaneParasagittalToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneCoronalToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneAxialToolButtonAction);
    m_volumePlaneActionGroup->addAction(m_volumePlaneAllToolButtonAction);
    m_volumePlaneActionGroup->setExclusive(true);
    QObject::connect(m_volumePlaneActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(volumePlaneActionGroupTriggered(QAction*)));
    
    
    m_volumePlaneResetToolButtonAction = WuQtUtilities::createAction("Reset",
                                                                         "Reset to remove panning, zooming, and/or oblique rotation",
                                                                         this,
                                                                         this,
                                                                         SLOT(volumePlaneResetToolButtonTriggered(bool)));
    
    
    QToolButton* volumePlaneParasagittalToolButton = new QToolButton();
    volumePlaneParasagittalToolButton->setDefaultAction(m_volumePlaneParasagittalToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumePlaneParasagittalToolButton);
    
    QToolButton* volumePlaneCoronalToolButton = new QToolButton();
    volumePlaneCoronalToolButton->setDefaultAction(m_volumePlaneCoronalToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumePlaneCoronalToolButton);
    
    QToolButton* volumePlaneAxialToolButton = new QToolButton();
    volumePlaneAxialToolButton->setDefaultAction(m_volumePlaneAxialToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumePlaneAxialToolButton);
    
    QToolButton* volumePlaneAllToolButton = new QToolButton();
    volumePlaneAllToolButton->setDefaultAction(m_volumePlaneAllToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumePlaneAllToolButton);
    
    QToolButton* volumePlaneResetToolButton = new QToolButton();
    volumePlaneResetToolButton->setDefaultAction(m_volumePlaneResetToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumePlaneResetToolButton);
    
    WuQtUtilities::matchWidgetHeights(volumePlaneParasagittalToolButton,
                                      volumePlaneCoronalToolButton,
                                      volumePlaneAxialToolButton,
                                      volumePlaneAllToolButton);
    WuQtUtilities::matchWidgetWidths(volumePlaneParasagittalToolButton,
                                     volumePlaneCoronalToolButton,
                                     volumePlaneAxialToolButton,
                                     volumePlaneAllToolButton);
    
    QToolButton* slicePlaneCustomToolButton = new QToolButton();
    slicePlaneCustomToolButton->setDefaultAction(m_parentToolBar->customViewAction);
    slicePlaneCustomToolButton->setSizePolicy(QSizePolicy::Minimum,
                                              QSizePolicy::Fixed);
    WuQtUtilities::setToolButtonStyleForQt5Mac(slicePlaneCustomToolButton);
    
    m_volumeAxisCrosshairsToolButtonAction = new QAction("");
    m_volumeAxisCrosshairsToolButtonAction->setCheckable(true);
    m_volumeAxisCrosshairsToolButtonAction->setToolTip("Show crosshairs on slice planes");
    QObject::connect(m_volumeAxisCrosshairsToolButtonAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::volumeAxisCrosshairsTriggered);
    QToolButton* volumeCrosshairsToolButton = new QToolButton();
    QPixmap xhairPixmap = createCrosshairsIcon(volumeCrosshairsToolButton);
    volumeCrosshairsToolButton->setDefaultAction(m_volumeAxisCrosshairsToolButtonAction);
    m_volumeAxisCrosshairsToolButtonAction->setIcon(QIcon(xhairPixmap));
    volumeCrosshairsToolButton->setIconSize(xhairPixmap.size());
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumeCrosshairsToolButton);
    
    m_volumeAxisCrosshairLabelsToolButtonAction = new QAction("");
    m_volumeAxisCrosshairLabelsToolButtonAction->setCheckable(true);
    m_volumeAxisCrosshairLabelsToolButtonAction->setToolTip("Show crosshair slice plane labels");
    QObject::connect(m_volumeAxisCrosshairLabelsToolButtonAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::volumeAxisCrosshairLabelsTriggered);
    QToolButton* volumeCrosshairLabelsToolButton = new QToolButton();
    volumeCrosshairLabelsToolButton->setDefaultAction(m_volumeAxisCrosshairLabelsToolButtonAction);
    QPixmap labelsPixmap = createCrosshairLabelsIcon(volumeCrosshairLabelsToolButton);
    m_volumeAxisCrosshairLabelsToolButtonAction->setIcon(QIcon(labelsPixmap));
    volumeCrosshairLabelsToolButton->setIconSize(labelsPixmap.size());
    WuQtUtilities::setToolButtonStyleForQt5Mac(volumeCrosshairLabelsToolButton);
    
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(volumePlaneParasagittalToolButton, rowIndex, 0);
    gridLayout->addWidget(volumePlaneCoronalToolButton,      rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(volumePlaneAxialToolButton, rowIndex, 0);
    gridLayout->addWidget(volumePlaneAllToolButton,   rowIndex, 1);
    rowIndex++;
    gridLayout->addWidget(volumePlaneResetToolButton, rowIndex, 0, 1, 2, Qt::AlignHCenter);
    rowIndex++;
    gridLayout->addWidget(slicePlaneCustomToolButton, rowIndex, 0, 1, 2, Qt::AlignHCenter);
    rowIndex++;
    gridLayout->addWidget(volumeCrosshairsToolButton, rowIndex, 0, Qt::AlignRight);
    gridLayout->addWidget(volumeCrosshairLabelsToolButton, rowIndex, 1, Qt::AlignLeft);
    
    m_volumePlaneWidgetGroup = new WuQWidgetObjectGroup(this);
    m_volumePlaneWidgetGroup->add(m_volumePlaneActionGroup);
    m_volumePlaneWidgetGroup->add(m_volumePlaneResetToolButtonAction);
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
    
    switch (browserTabContent->getSliceViewPlane()) {
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
 */
QMenu*
BrainBrowserWindowToolBarSlicePlane::createViewAllSlicesLayoutMenu()
{
    std::vector<VolumeSliceViewAllPlanesLayoutEnum::Enum> allLayouts;
    VolumeSliceViewAllPlanesLayoutEnum::getAllEnums(allLayouts);
    
    QMenu* menu = new QMenu();
    QActionGroup* actionGroup = new QActionGroup(this);
    
    for (auto layout : allLayouts) {
        QAction* action = menu->addAction(VolumeSliceViewAllPlanesLayoutEnum::toGuiName(layout));
        action->setData((int)VolumeSliceViewAllPlanesLayoutEnum::toIntegerCode(layout));
        action->setCheckable(true);
        m_viewAllSliceLayoutMenuActions.push_back(action);
        
        actionGroup->addAction(action);
    }

    QObject::connect(menu, &QMenu::triggered,
                     this, &BrainBrowserWindowToolBarSlicePlane::viewAllSliceLayoutMenuTriggered);
    return menu;
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
        
        btc->setSlicePlanesAllViewLayout(layout);
        
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
    const VolumeSliceViewAllPlanesLayoutEnum::Enum layout = browserTabContent->getSlicePlanesAllViewLayout();
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
    
    btc->setSliceViewPlane(plane);
    
    m_parentToolBar->updateVolumeIndicesWidget(btc);
    updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when volume reset slice view button is pressed.
 */
void
BrainBrowserWindowToolBarSlicePlane::volumePlaneResetToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = getTabContentFromSelectedTab();
    btc->resetView();
    
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
 * Create a pixmap for the crosshairs button.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @return
 *    The pixmap.
 */
QPixmap
BrainBrowserWindowToolBarSlicePlane::createCrosshairsIcon(const QWidget* widget)
{
    CaretAssert(widget);
    const float pixmapSize = 22.0;
    
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter(widget,
                                                                                            pixmap);
    const int startXY = 4;
    const int endXY   = 10;
    QPen pen(painter->pen());
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(-startXY, 0, -endXY, 0);
    painter->drawLine( startXY, 0,  endXY, 0);
    painter->drawLine(0, -startXY, 0, -endXY);
    painter->drawLine(0,  startXY, 0,  endXY);
    
    return pixmap;
}

/**
 * Create a pixmap for the crosshairs button.
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @return
 *    The pixmap.
 */
QPixmap
BrainBrowserWindowToolBarSlicePlane::createCrosshairLabelsIcon(const QWidget* widget)
{
    CaretAssert(widget);
    
    const float pixmapSize = 22.0f;
    const float fullXY = pixmapSize;
    const float halfXY = fullXY / 2.0f;
    
    const float boxWH = 8.0f;
    const float halfBoxWH = boxWH / 2.0f;

    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(widget,
                                                                                pixmap);
    QFont font = painter->font();
    font.setPixelSize(10);//9);
    painter->setFont(font);
    
    const float edgeOffset = 1.0f;
    painter->drawText(QRectF(edgeOffset, halfXY - halfBoxWH, boxWH, boxWH),
                      "L", QTextOption(Qt::AlignCenter)); //Qt::AlignLeft | Qt::AlignTop));
    painter->drawText(QRectF(fullXY - boxWH - edgeOffset + 1, halfXY - halfBoxWH - 1, boxWH, boxWH),
                      "R", QTextOption(Qt::AlignCenter)); //Qt::AlignLeft | Qt::AlignTop));

    painter->drawText(QRectF(halfXY - halfBoxWH, edgeOffset, boxWH, boxWH),
                      "T", QTextOption(Qt::AlignCenter)); //Qt::AlignLeft | Qt::AlignTop));
    painter->drawText(QRectF(halfXY - halfBoxWH + 1, fullXY - boxWH - edgeOffset, boxWH, boxWH),
                      "B", QTextOption(Qt::AlignCenter)); //Qt::AlignLeft | Qt::AlignTop));
    
    return pixmap;
}
