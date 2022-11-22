
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__
#include "BrainBrowserWindowToolBarTab.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "AnnotationScaleBar.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretUndoStack.h"
#include "ClippingPlanesWidget.h"
#include "EnumComboBoxTemplate.h"
#include "GuiManager.h"
#include "ScaleBarWidget.h"
#include "ViewingTransformations.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"
#include "YokingGroupEnum.h"
#include "WuQDataEntryDialog.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::BrainBrowserWindowToolBarTab 
 * \brief Tab section of Browser Window Toolbar.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of browser window.
 * @param toolBarLockWindowAndAllTabAspectRatioButton
 *     Button for locking window and all tab aspect ratio.
 * @param parentToolBar
 *     Parent toolbar.
 * @param objectNamePrefix
       Prefix for naming objects
 */
BrainBrowserWindowToolBarTab::BrainBrowserWindowToolBarTab(const int32_t browserWindowIndex,
                                                           QToolButton* toolBarLockWindowAndAllTabAspectRatioButton,
                                                           BrainBrowserWindowToolBar* parentToolBar,
                                                           const QString& objectNamePrefix)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_browserWindowIndex(browserWindowIndex),
m_parentToolBar(parentToolBar),
m_lockWindowAndAllTabAspectButton(toolBarLockWindowAndAllTabAspectRatioButton)
{
    m_objectNamePrefix = (objectNamePrefix
                          + ":Tab");
    setObjectName(m_objectNamePrefix);
    m_yokingGroupComboBox = new EnumComboBoxTemplate(this);
    m_yokingGroupComboBox->setup<YokingGroupEnum, YokingGroupEnum::Enum>();
    m_yokingGroupComboBox->getWidget()->setStatusTip("Select a yoking group (linked views)");
    m_yokingGroupComboBox->getWidget()->setToolTip(("Select a yoking group (linked views).\n"
                                                    "Models yoked to a group are displayed in the same view.\n"
                                                    "Surface Yoking is applied to Surface, Surface Montage\n"
                                                    "and Whole Brain.  Volume Yoking is applied to Volumes."));
    QComboBox* encapComboBox = m_yokingGroupComboBox->getComboBox();
    encapComboBox->setObjectName(m_objectNamePrefix
                                 + ":YokingGroup");
    WuQMacroManager::instance()->addMacroSupportToObject(encapComboBox,
                                                         "Select yoking group");
    
    m_yokeToLabel = new QLabel("Yoke");
    QObject::connect(m_yokingGroupComboBox, SIGNAL(itemActivated()),
                     this, SLOT(yokeToGroupComboBoxIndexChanged()));
    
    /*
     * Lighting action and button
     */
    QIcon shadingIcon;
    const bool shadingIconValid = WuQtUtilities::loadIcon(":/ToolBar/lighting.png",
                                                          shadingIcon);
    const AString lightToolTip = WuQtUtilities::createWordWrappedToolTipText("Enables shading on surfaces.  Shading affects "
                                                                             "palette colors (but not by much) and in rare circumstances it "
                                                                             "may be helpful to turn shading off.");

    m_lightingAction = new QAction(this);
    m_lightingAction->setCheckable(true);
    if (shadingIconValid) {
        m_lightingAction->setIcon(shadingIcon);
    }
    else {
        m_lightingAction->setText("L");
    }
    m_lightingAction->setToolTip(lightToolTip);
    m_lightingAction->setObjectName(m_objectNamePrefix
                                    + ":EnableShading");
    QObject::connect(m_lightingAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarTab::lightingEnabledCheckBoxChecked);
    WuQMacroManager::instance()->addMacroSupportToObject(m_lightingAction,
                                                         "Enable shading");

    QToolButton* lightingToolButton = new QToolButton();
    lightingToolButton->setDefaultAction(m_lightingAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(lightingToolButton);

    /*
     * Clipping action and button
     */
    QIcon clippingIcon;
    const bool clippingIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/clipping.png",
                            clippingIcon);
    
    m_clippingPlanesAction = new QAction(this);
    m_clippingPlanesAction->setCheckable(true);
    if (clippingIconValid) {
        m_clippingPlanesAction->setIcon(clippingIcon);
    }
    else {
        m_clippingPlanesAction->setText("C");
    }
    m_clippingPlanesAction->setToolTip("Enable clipping planes; click arrow to edit");
    m_clippingPlanesAction->setMenu(createClippingPlanesMenu());
    m_clippingPlanesAction->setObjectName(m_objectNamePrefix
                                          + ":ClippingPlanes");
    WuQMacroManager::instance()->addMacroSupportToObject(m_clippingPlanesAction,
                                                         "Enable clipping planes");
    QObject::connect(m_clippingPlanesAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarTab::clippingPlanesActionToggled);
    
    QToolButton* clippingPlanesToolButton = new QToolButton();
    clippingPlanesToolButton->setDefaultAction(m_clippingPlanesAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(clippingPlanesToolButton);

    /*
     * Scale bar action and button
     */
    QToolButton* scaleBarToolButton = new QToolButton();
    QPixmap scaleBarPixmap = createScaleBarPixmap(scaleBarToolButton);
    m_scaleBarAction = new QAction(this);
    m_scaleBarAction->setCheckable(true);
    m_scaleBarAction->setToolTip("Enable scale bar; click arrow to edit");
    m_scaleBarAction->setIcon(QIcon(scaleBarPixmap));
    m_scaleBarAction->setMenu(createScaleBarMenu());
    QObject::connect(m_scaleBarAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarTab::scaleBarActionToggled);

    scaleBarToolButton->setDefaultAction(m_scaleBarAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(scaleBarToolButton);

    m_macroRecordingLabel = new QLabel("");
    
    m_redoAction = WuQtUtilities::createAction("Redo",
                                               "Redo ToolTip",
                                               this,
                                               this,
                                               SLOT(redoActionTriggered()));
    QToolButton* redoToolButton = new QToolButton();
    redoToolButton->setDefaultAction(m_redoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(redoToolButton);
    m_redoAction->setObjectName(m_objectNamePrefix + ":Redo");
    WuQMacroManager::instance()->addMacroSupportToObject(m_redoAction, "Redo Image View");
    
    m_undoAction = WuQtUtilities::createAction("Undo",
                                               "Undo ToolTip",
                                               this,
                                               this,
                                               SLOT(undoActionTriggered()));
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(m_undoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(undoToolButton);
    m_undoAction->setObjectName(m_objectNamePrefix + ":Undo");
    WuQMacroManager::instance()->addMacroSupportToObject(m_undoAction, "Undo Image View");

    QHBoxLayout* redoUndoLayout(new QHBoxLayout());
    redoUndoLayout->addWidget(redoToolButton);
    redoUndoLayout->addWidget(undoToolButton);
    redoUndoLayout->addWidget(m_macroRecordingLabel);
    redoUndoLayout->addStretch();
    
    QHBoxLayout* yokeLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(yokeLayout, 2, 0);
    yokeLayout->addWidget(m_yokeToLabel);
    yokeLayout->addWidget(m_yokingGroupComboBox->getWidget());
    yokeLayout->addStretch();
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(buttonsLayout, 2, 2);
    buttonsLayout->addWidget(lightingToolButton);
    buttonsLayout->addWidget(clippingPlanesToolButton);
    buttonsLayout->addWidget(scaleBarToolButton);
    buttonsLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    layout->addLayout(yokeLayout);
    layout->addWidget(m_lockWindowAndAllTabAspectButton, 0, Qt::AlignLeft);
    layout->addLayout(buttonsLayout);
    layout->addLayout(redoUndoLayout);
//    layout->addWidget(m_macroRecordingLabel, 0, Qt::AlignLeft);
    
    addToWidgetGroup(m_yokeToLabel);
    addToWidgetGroup(m_yokingGroupComboBox->getWidget());
    addToWidgetGroup(lightingToolButton);
    addToWidgetGroup(clippingPlanesToolButton);
    addToWidgetGroup(scaleBarToolButton);
    addToWidgetGroup(m_macroRecordingLabel);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarTab::~BrainBrowserWindowToolBarTab()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarTab::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;
    if (browserTabContent == NULL) {
        return;
    }
    
    blockAllSignals(true);
    
    bool chartFlag = false;
    bool mediaFlag(false);
    switch (browserTabContent->getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            mediaFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            break;
    }
    
    if (chartFlag) {
        m_yokingGroupComboBox->setSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>(browserTabContent->getChartModelYokingGroup());
        m_yokeToLabel->setEnabled(false);
        m_yokingGroupComboBox->getWidget()->setEnabled(false);
    }
    else if (mediaFlag) {
        m_yokingGroupComboBox->setSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>(browserTabContent->getMediaModelYokingGroup());
        m_yokeToLabel->setEnabled(true);
        m_yokingGroupComboBox->getWidget()->setEnabled(true);
    }
    else {
        m_yokeToLabel->setEnabled(true);
        m_yokingGroupComboBox->getWidget()->setEnabled(true);
        m_yokingGroupComboBox->setSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>(browserTabContent->getBrainModelYokingGroup());
    }
    
    m_lightingAction->setChecked(browserTabContent->isLightingEnabled());
    
    m_macroRecordingLabel->setText("");
    switch (WuQMacroManager::instance()->getMode()) {
        case WuQMacroModeEnum::OFF:
            break;
        case WuQMacroModeEnum::RECORDING_INSERT_COMMANDS:
        case WuQMacroModeEnum::RECORDING_NEW_MACRO:
            m_macroRecordingLabel->setText("<html><font color=red>Macro</font></html>");
            break;
        case WuQMacroModeEnum::RUNNING:
            break;
    }
    
    m_clippingPlanesAction->setChecked(browserTabContent->isClippingPlanesEnabled());
    m_scaleBarAction->setChecked(browserTabContent->getScaleBar()->isDisplayed());
    
    m_redoAction->setEnabled(false);
    m_undoAction->setEnabled(false);
    
    if (browserTabContent != NULL) {
        CaretUndoStack* undoStack = getUndoStack();
        if (undoStack != NULL) {
            m_redoAction->setEnabled(undoStack->canRedo());
            m_redoAction->setToolTip(undoStack->redoText());
            
            m_undoAction->setEnabled(undoStack->canUndo());
            m_undoAction->setToolTip(undoStack->undoText());
        }
    }
    
    blockAllSignals(false);
}

/**
 * Called when lighting checkbox is checked by user
 *
 * @param checked
 *     New status of lighting.
 */
void
BrainBrowserWindowToolBarTab::lightingEnabledCheckBoxChecked(bool checked)
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    
    browserTabContent->setLightingEnabled(checked);
    this->updateGraphicsWindow();
}

/**
 * Called when window yoke to tab combo box is selected.
 */
void
BrainBrowserWindowToolBarTab::yokeToGroupComboBoxIndexChanged()
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent == NULL) {
        return;
    }
    
    bool chartFlag = false;
    bool mediaFlag = false;
    switch (browserTabContent->getSelectedModelType()) {
        case ModelTypeEnum::MODEL_TYPE_CHART:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            chartFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_HISTOLOGY:
            break;
        case  ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA:
            mediaFlag = true;
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            break;
    }
    
    YokingGroupEnum::Enum yokingGroup = m_yokingGroupComboBox->getSelectedItem<YokingGroupEnum, YokingGroupEnum::Enum>();
    if (chartFlag) {
        browserTabContent->setChartModelYokingGroup(yokingGroup);
    }
    else if (mediaFlag) {
        browserTabContent->setMediaModelYokingGroup(yokingGroup);
    }
    else {
        browserTabContent->setBrainModelYokingGroup(yokingGroup);
    }
    
    m_parentToolBar->updateToolBarComponents(browserTabContent);

    this->updateGraphicsWindow();
}

/**
 * @return Instance of the clipping menu
 */
QMenu*
BrainBrowserWindowToolBarTab::createClippingPlanesMenu()
{
    m_clippingPlanesWidget = new ClippingPlanesWidget(m_objectNamePrefix);
    QWidgetAction* clippingPlanesAction = new QWidgetAction(this);
    clippingPlanesAction->setDefaultWidget(m_clippingPlanesWidget);
    
    QMenu* menu = new QMenu(this);
    menu->addAction(clippingPlanesAction);
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBarTab::clippingPlanesMenuAboutToShow);
    return menu;
}

/**
 * Called when clipping planes action is toggled
 * @param checked
 *     New checked status
 */
void
BrainBrowserWindowToolBarTab::clippingPlanesActionToggled(bool checked)
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        browserTabContent->setClippingPlanesEnabledAndEnablePlanes(checked);
        updateContent(browserTabContent);
        updateGraphicsWindow();
    }
}

/**
 * Called when clipping menu is about to show
 */
void
BrainBrowserWindowToolBarTab::clippingPlanesMenuAboutToShow()
{
    int32_t tabIndex = -1;
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        tabIndex = browserTabContent->getTabNumber();
    }

    m_clippingPlanesWidget->updateContent(tabIndex);
}

/**
 * @return Instance of the scale bar menu
 */
QMenu*
BrainBrowserWindowToolBarTab::createScaleBarMenu()
{
    m_scaleBarWidget = new ScaleBarWidget(m_objectNamePrefix);
    QWidgetAction* scaleBarAction = new QWidgetAction(this);
    scaleBarAction->setDefaultWidget(m_scaleBarWidget);
    
    QMenu* menu = new QMenu(this);
    menu->addAction(scaleBarAction);
    QObject::connect(menu, &QMenu::aboutToShow,
                     this, &BrainBrowserWindowToolBarTab::scaleBarMenuAboutToShow);
    return menu;
}

/**
 * Called when scale bar action is toggled
 * @param checked
 *     New checked status
 */
void
BrainBrowserWindowToolBarTab::scaleBarActionToggled(bool checked)
{
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        browserTabContent->getScaleBar()->setDisplayed(checked);
        updateContent(browserTabContent);
        updateGraphicsWindow();
    }
}

/**
 * Called when scale bar is about to show
 */
void
BrainBrowserWindowToolBarTab::scaleBarMenuAboutToShow()
{
    BrowserTabContent* browserTabContent = this->getTabContentFromSelectedTab();
    m_scaleBarWidget->updateContent(browserTabContent);
}

/**
 * @return Undo stack for this tab or NULL if not valid
 */
CaretUndoStack*
BrainBrowserWindowToolBarTab::getUndoStack()
{
    CaretUndoStack* undoStack(NULL);
    if (m_browserTabContent != NULL) {
            ViewingTransformations* viewingTransform(m_browserTabContent->getViewingTransformation());
            if (viewingTransform != NULL) {
                undoStack = viewingTransform->getRedoUndoStack();
            }
    }
    return undoStack;
}

/**
 * Gets called when the redo action is triggered
 */
void
BrainBrowserWindowToolBarTab::redoActionTriggered()
{
    CaretUndoStack* undoStack = getUndoStack();
    AString errorMessage;
    if ( ! undoStack->redo(errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    updateGraphicsWindowAndYokedWindows();
    updateContent(m_browserTabContent);
}

/**
 * Gets called when the undo action is triggered
 */
void
BrainBrowserWindowToolBarTab::undoActionTriggered()
{
    CaretUndoStack* undoStack = getUndoStack();
    AString errorMessage;
    if ( ! undoStack->undo(errorMessage)) {
        WuQMessageBox::errorOk(this,
                               errorMessage);
    }
    
    updateGraphicsWindowAndYokedWindows();
    updateContent(m_browserTabContent);
}


/**
 * Create an ruler icon for Scale Bar tool button
 *
 * @param widget
 *    To color the pixmap with backround and foreground,
 *    the palette from the given widget is used.
 * @return
 *    Pixmap with icon
 */

QPixmap
BrainBrowserWindowToolBarTab::createScaleBarPixmap(const QWidget* widget)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    float width  = 24.0;
    float height = 24.0;
    float margin = 1.0;
    
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget,
                                                                                                pixmap);
    
    const float leftX(margin);
    const float rightX(width - margin);
    const float centerX((leftX + rightX) / 2.0);
    const float leftCenterX((leftX + centerX) / 2.0);
    const float rightCenterX((rightX + centerX) / 2.0);
    const float y(margin + 2.0);
    const float smallTickHeight(8.0);
    const float mediumTickHeight(smallTickHeight * 1.3);
    const float bigTickHeight(smallTickHeight * 1.65);
    
    QPen pen = painter->pen();
    pen.setWidth(4);
    painter->setPen(pen);
    
    /* bottom line */
    painter->drawLine(QLineF(leftX, y,
                             rightX, y));
    
    pen.setWidth(2);
    painter->setPen(pen);
    
    /* left tick */
    painter->drawLine(QLineF(leftX, y,
                             leftX, y + bigTickHeight));
                      
    /* right tick */
    painter->drawLine(QLineF(rightX, margin,
                             rightX, y + bigTickHeight));
    
    /* center tick */
    painter->drawLine(QLineF(centerX, y,
                             centerX, y + mediumTickHeight));

    /* left-center tick */
    painter->drawLine(QLineF(leftCenterX, y,
                             leftCenterX, y + smallTickHeight));

    /* right-center tick */
    painter->drawLine(QLineF(rightCenterX, y,
                             rightCenterX, y + smallTickHeight));
    
    return pixmap;
}

