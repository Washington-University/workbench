
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

#include <QCheckBox>
#include <QGridLayout>
#include <QStackedWidget>
#include <QVBoxLayout>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_DECLARE__
#include "BrainBrowserWindowToolBarSurfaceMontage.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_MONTAGE_DECLARE__

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "ModelSurfaceMontage.h"
#include "SurfaceMontageConfigurationCerebellar.h"
#include "SurfaceMontageConfigurationCerebral.h"
#include "SurfaceMontageConfigurationFlatMaps.h"
#include "SurfaceMontageLayoutOrientationEnum.h"
#include "SurfaceSelectionModel.h"
#include "SurfaceSelectionViewController.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;



/**
 * \class caret::BrainBrowserWindowToolBarSurfaceMontage
 * \brief Surface Montage Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 * @param objectNamePrefix
 *    Prefix name for naming objects
 */
BrainBrowserWindowToolBarSurfaceMontage::BrainBrowserWindowToolBarSurfaceMontage(BrainBrowserWindowToolBar* parentToolBar,
                                                                                 const QString& objectNamePrefix)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    m_surfaceMontageConfigurationTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_surfaceMontageConfigurationTypeEnumComboBox->setup<SurfaceMontageConfigurationTypeEnum,SurfaceMontageConfigurationTypeEnum::Enum>();
    QObject::connect(m_surfaceMontageConfigurationTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(surfaceMontageConfigurationTypeEnumComboBoxItemActivated()));
    addToWidgetGroup(m_surfaceMontageConfigurationTypeEnumComboBox->getWidget());
    WuQtUtilities::setToolTipAndStatusTip(m_surfaceMontageConfigurationTypeEnumComboBox->getWidget(),
                                         ("Selects Surface Montage Configuration:\n"
                                          "   Cerebellar Cortex\n"
                                          "   Cerebral Cortex\n"
                                          "   Flat Maps"));
    m_surfaceMontageConfigurationTypeEnumComboBox->getComboBox()->setObjectName(objectNamePrefix
                                                                                + ":SurfaceMontageConfiguration");
    WuQMacroManager::instance()->addMacroSupportToObject(m_surfaceMontageConfigurationTypeEnumComboBox->getComboBox(),
                                                         "Select surface montage configuration");

    m_surfaceMontageLayoutOrientationEnumComboBox = new EnumComboBoxTemplate(this);
    m_surfaceMontageLayoutOrientationEnumComboBox->setup<SurfaceMontageLayoutOrientationEnum,SurfaceMontageLayoutOrientationEnum::Enum>();
    QObject::connect(m_surfaceMontageLayoutOrientationEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(surfaceMontageLayoutOrientationEnumComboBoxItemActivated()));
    addToWidgetGroup(m_surfaceMontageLayoutOrientationEnumComboBox->getWidget());
    WuQtUtilities::setToolTipAndStatusTip(m_surfaceMontageLayoutOrientationEnumComboBox->getWidget(),
                                         ("Selects Surface Layout:\n"
                                          "   Landscape (Layout left-to-right)\n"
                                          "   Portrait  (Layout top-to-bottom)"));
    m_surfaceMontageLayoutOrientationEnumComboBox->getComboBox()->setObjectName(objectNamePrefix
                                                                                + ":SurfaceMontageOrientation");
    WuQMacroManager::instance()->addMacroSupportToObject(m_surfaceMontageLayoutOrientationEnumComboBox->getComboBox(),
                                                         "Select surface montage layout");
    
    m_cerebellarComponent = new SurfaceMontageCerebellarComponent(this,
                                                                  objectNamePrefix);
    
    m_cerebralComponent = new SurfaceMontageCerebralComponent(this,
                                                              objectNamePrefix);
    
    m_flatMapsComponent = new SurfaceMontageFlatMapsComponent(this,
                                                              objectNamePrefix);
    
    QHBoxLayout* configOrientationLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(configOrientationLayout, 2, 0);
    configOrientationLayout->addStretch();
    configOrientationLayout->addWidget(m_surfaceMontageConfigurationTypeEnumComboBox->getWidget());
    configOrientationLayout->addStretch();
    configOrientationLayout->addSpacing(10);
    configOrientationLayout->addWidget(m_surfaceMontageLayoutOrientationEnumComboBox->getWidget());
    configOrientationLayout->addStretch();
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_cerebellarComponent);
    m_stackedWidget->addWidget(m_cerebralComponent);
    m_stackedWidget->addWidget(m_flatMapsComponent);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 0);
    layout->addLayout(configOrientationLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(m_stackedWidget);
    //    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarSurfaceMontage::~BrainBrowserWindowToolBarSurfaceMontage()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when the montage configuration is changed.
 */
void
BrainBrowserWindowToolBarSurfaceMontage::surfaceMontageConfigurationTypeEnumComboBoxItemActivated()
{
    m_parentToolBar->getTabContentFromSelectedTab();
    BrowserTabContent* btc = m_parentToolBar->getTabContentFromSelectedTab();
    const SurfaceMontageConfigurationTypeEnum::Enum configType = m_surfaceMontageConfigurationTypeEnumComboBox->getSelectedItem<SurfaceMontageConfigurationTypeEnum,SurfaceMontageConfigurationTypeEnum::Enum>();
    ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
    const int32_t tabIndex = btc->getTabNumber();
    msm->setSelectedConfigurationType(tabIndex, configType);
    
    updateContent(btc);
    invalidateColoringAndUpdateGraphicsWindow();
    m_parentToolBar->updateUserInterface();
}

/**
 * Called when the layout orientation value is changed.
 */
void
BrainBrowserWindowToolBarSurfaceMontage::surfaceMontageLayoutOrientationEnumComboBoxItemActivated()
{
    m_parentToolBar->getTabContentFromSelectedTab();
    BrowserTabContent* btc = m_parentToolBar->getTabContentFromSelectedTab();
    ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
    const int32_t tabIndex = btc->getTabNumber();
    SurfaceMontageConfigurationAbstract* selectedConfiguration = msm->getSelectedConfiguration(tabIndex);
    selectedConfiguration->setLayoutOrientation(m_surfaceMontageLayoutOrientationEnumComboBox->getSelectedItem<SurfaceMontageLayoutOrientationEnum, SurfaceMontageLayoutOrientationEnum::Enum>());

    invalidateColoringAndUpdateGraphicsWindow();
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarSurfaceMontage::updateContent(BrowserTabContent* browserTabContent)
{
    ModelSurfaceMontage* msm = browserTabContent->getDisplayedSurfaceMontageModel();
    if (msm == NULL) {
        return;
    }
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    
    std::vector<SurfaceMontageConfigurationTypeEnum::Enum> validConfigs;
    if (msm->getCerebellarConfiguration(tabIndex)->isValid()) {
        validConfigs.push_back(SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION);
    }
    if (msm->getCerebralConfiguration(tabIndex)->isValid()) {
        validConfigs.push_back(SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION);
    }
    if (msm->getFlatMapsConfiguration(tabIndex)->isValid()) {
        validConfigs.push_back(SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION);
    }
    
    m_surfaceMontageConfigurationTypeEnumComboBox->setupWithItems<SurfaceMontageConfigurationTypeEnum,SurfaceMontageConfigurationTypeEnum::Enum>(validConfigs);
    SurfaceMontageConfigurationAbstract* selectedConfiguration = msm->getSelectedConfiguration(tabIndex);
    m_surfaceMontageConfigurationTypeEnumComboBox->setSelectedItem<SurfaceMontageConfigurationTypeEnum,SurfaceMontageConfigurationTypeEnum::Enum>(msm->getSelectedConfigurationType(tabIndex));

    m_surfaceMontageLayoutOrientationEnumComboBox->setSelectedItem<SurfaceMontageLayoutOrientationEnum,SurfaceMontageLayoutOrientationEnum::Enum>(selectedConfiguration->getLayoutOrientation());
    
    switch (msm->getSelectedConfigurationType(tabIndex)) {
        case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
            m_cerebellarComponent->updateContent(browserTabContent);
            m_stackedWidget->setCurrentWidget(m_cerebellarComponent);
            break;
        case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
            m_cerebralComponent->updateContent(browserTabContent);
            m_stackedWidget->setCurrentWidget(m_cerebralComponent);
            break;
        case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
            m_flatMapsComponent->updateContent(browserTabContent);
            m_stackedWidget->setCurrentWidget(m_flatMapsComponent);
            break;
    }
}


/*
 ********************************************************************************************
 */

/**
 * \class caret::SurfaceMontageCerebralComponent
 * \brief Cerebral Surface Montage Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 * @param objectNamePrefix
 *    Prefix name for naming objects
 */
SurfaceMontageCerebralComponent::SurfaceMontageCerebralComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage,
                                                                 const QString& parentObjectNamePrefix)
: QWidget(parentToolBarMontage)
{
    const QString objectNamePrefix(parentObjectNamePrefix
                                   + ":SurfaceMontage");
    
    m_parentToolBarMontage = parentToolBarMontage;
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_leftCheckBox = new QCheckBox("Left");
    QObject::connect(m_leftCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_leftCheckBox->setObjectName(objectNamePrefix
                                  + ":EnableLeft");
    m_leftCheckBox->setToolTip("Enable Left Surfaces");
    macroManager->addMacroSupportToObject(m_leftCheckBox,
                                          "Enable left surface in cerebral montage");
    
    m_rightCheckBox = new QCheckBox("Right");
    QObject::connect(m_rightCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_rightCheckBox->setObjectName(objectNamePrefix
                                  + ":EnableRight");
    m_rightCheckBox->setToolTip("Enable Right Surface");
    macroManager->addMacroSupportToObject(m_rightCheckBox,
                                          "Enable right surface in cerebral montage");
    
    m_lateralCheckBox = new QCheckBox("Lateral");
    QObject::connect(m_lateralCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_lateralCheckBox->setObjectName(objectNamePrefix
                                  + ":EnableLateralView");
    m_lateralCheckBox->setToolTip("Enable Lateral View");
    macroManager->addMacroSupportToObject(m_lateralCheckBox,
                                          "Enable lateral view in cerebral montage");
    
    m_medialCheckBox = new QCheckBox("Medial");
    QObject::connect(m_medialCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_medialCheckBox->setObjectName(objectNamePrefix
                                  + ":EnableMedialView");
    m_medialCheckBox->setToolTip("Enable Medial View");
    macroManager->addMacroSupportToObject(m_medialCheckBox,
                                          "Enable medial view in cerebral montage");
    
    m_surfaceMontageFirstSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_surfaceMontageFirstSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_surfaceMontageFirstSurfaceCheckBox->setObjectName(objectNamePrefix
                                  + ":EnableFirstRowSurfaces");
    m_surfaceMontageFirstSurfaceCheckBox->setToolTip("Enable First Surfaces");
    macroManager->addMacroSupportToObject(m_surfaceMontageFirstSurfaceCheckBox,
                                          "Enable first surface row in cerebral montage");
    
    m_surfaceMontageSecondSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_surfaceMontageSecondSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_surfaceMontageSecondSurfaceCheckBox->setObjectName(objectNamePrefix
                                  + ":EnableSecondRowSurfaces");
    m_surfaceMontageSecondSurfaceCheckBox->setToolTip("Enable Second Surfaces");
    macroManager->addMacroSupportToObject(m_surfaceMontageSecondSurfaceCheckBox,
                                          "Enable second row in cerebral montage");
    
    m_leftSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                     objectNamePrefix
                                                                     + ":SurfaceLeftTop",
                                                                     "cerebral montage left top");
    QObject::connect(m_leftSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(leftSurfaceSelected(Surface*)));
    
    m_leftSecondSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                           objectNamePrefix
                                                                           + ":SurfaceLeftBottom",
                                                                           "cerebral montage left bottom");
    QObject::connect(m_leftSecondSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(leftSecondSurfaceSelected(Surface*)));
    
    m_rightSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                      objectNamePrefix
                                                                      + ":SurfaceRightTop",
                                                                      "cerebral montage right top");
    QObject::connect(m_rightSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(rightSurfaceSelected(Surface*)));
    
    m_rightSecondSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                            objectNamePrefix
                                                                            + ":SurfaceRightBottom",
                                                                            "cerebral montage right bottom");
    QObject::connect(m_rightSecondSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(rightSecondSurfaceSelected(Surface*)));
    
    QHBoxLayout* checkBoxLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(checkBoxLayout, 2, 0);
    checkBoxLayout->addWidget(m_leftCheckBox);
    checkBoxLayout->addSpacing(10);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_lateralCheckBox);
    checkBoxLayout->addSpacing(10);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_medialCheckBox);
    checkBoxLayout->addSpacing(10);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_rightCheckBox);
    
    int32_t columnIndex = 0;
    const int32_t COLUMN_ONE_TWO     = columnIndex++;
    const int32_t COLUMN_INDEX_LEFT  = columnIndex++;
    const int32_t COLUMN_INDEX_RIGHT = columnIndex++;
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(0,   0);
    layout->setColumnStretch(1, 100);
    layout->setColumnStretch(2, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    int row = layout->rowCount();
    layout->addWidget(m_surfaceMontageFirstSurfaceCheckBox, row, COLUMN_ONE_TWO);
    layout->addWidget(m_leftSurfaceViewController->getWidget(), row, COLUMN_INDEX_LEFT);
    layout->addWidget(m_rightSurfaceViewController->getWidget(), row, COLUMN_INDEX_RIGHT);
    row = layout->rowCount();
    layout->addWidget(m_surfaceMontageSecondSurfaceCheckBox, row, COLUMN_ONE_TWO);
    layout->addWidget(m_leftSecondSurfaceViewController->getWidget(), row, COLUMN_INDEX_LEFT);
    layout->addWidget(m_rightSecondSurfaceViewController->getWidget(), row, COLUMN_INDEX_RIGHT);
    row = layout->rowCount();
    layout->addLayout(checkBoxLayout,
                      row, COLUMN_INDEX_LEFT,
                      1, 2);
    row = layout->rowCount();
    
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_leftSurfaceViewController->getWidget());
    m_widgetGroup->add(m_leftSecondSurfaceViewController->getWidget());
    m_widgetGroup->add(m_rightSurfaceViewController->getWidget());
    m_widgetGroup->add(m_rightSecondSurfaceViewController->getWidget());
    m_widgetGroup->add(m_leftCheckBox);
    m_widgetGroup->add(m_rightCheckBox);
    m_widgetGroup->add(m_surfaceMontageFirstSurfaceCheckBox);
    m_widgetGroup->add(m_surfaceMontageSecondSurfaceCheckBox);
    m_widgetGroup->add(m_medialCheckBox);
    m_widgetGroup->add(m_lateralCheckBox);
    
    setFixedHeight(sizeHint().height());
}

/**
 * Destructor.
 */
SurfaceMontageCerebralComponent::~SurfaceMontageCerebralComponent()
{
    
}

/**
 * Update the cerebral montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
SurfaceMontageCerebralComponent::updateContent(BrowserTabContent* browserTabContent)
{
    m_widgetGroup->blockAllSignals(true);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    ModelSurfaceMontage* msm = browserTabContent->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
    
    m_leftCheckBox->setChecked(smcc->isLeftEnabled());
    m_rightCheckBox->setChecked(smcc->isRightEnabled());
    m_surfaceMontageFirstSurfaceCheckBox->setChecked(smcc->isFirstSurfaceEnabled());
    m_surfaceMontageSecondSurfaceCheckBox->setChecked(smcc->isSecondSurfaceEnabled());
    m_lateralCheckBox->setChecked(smcc->isLateralEnabled());
    m_medialCheckBox->setChecked(smcc->isMedialEnabled());
    
    m_leftSurfaceViewController->updateControl(smcc->getLeftFirstSurfaceSelectionModel());
    m_leftSecondSurfaceViewController->updateControl(smcc->getLeftSecondSurfaceSelectionModel());
    m_rightSurfaceViewController->updateControl(smcc->getRightFirstSurfaceSelectionModel());
    m_rightSecondSurfaceViewController->updateControl(smcc->getRightSecondSurfaceSelectionModel());
    
    m_widgetGroup->blockAllSignals(false);
}

/**
 * Called when montage left first surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageCerebralComponent::leftSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
        smcc->getLeftFirstSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when montage left second surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageCerebralComponent::leftSecondSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
        smcc->getLeftSecondSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when montage right surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageCerebralComponent::rightSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
        smcc->getRightFirstSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when montage right second surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageCerebralComponent::rightSecondSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
        smcc->getRightSecondSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when surface montage checkbox is toggled.
 * @param status
 *    New status of check box.
 */
void
SurfaceMontageCerebralComponent::checkBoxSelected(bool /*status*/)
{
    BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
    smcc->setLeftEnabled(m_leftCheckBox->isChecked());
    smcc->setRightEnabled(m_rightCheckBox->isChecked());
    smcc->setFirstSurfaceEnabled(m_surfaceMontageFirstSurfaceCheckBox->isChecked());
    smcc->setSecondSurfaceEnabled(m_surfaceMontageSecondSurfaceCheckBox->isChecked());
    smcc->setLateralEnabled(m_lateralCheckBox->isChecked());
    smcc->setMedialEnabled(m_medialCheckBox->isChecked());
    
    m_parentToolBarMontage->updateUserInterface();
    m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
}


/*
 ********************************************************************************************
 */

/**
 * \class caret::SurfaceMontageCerebellarComponent
 * \brief Cerebellar Surface Montage Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 * @param objectNamePrefix
 *    Prefix name for naming objects
 */
SurfaceMontageCerebellarComponent::SurfaceMontageCerebellarComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage,
                                                                     const QString& parentObjectNamePrefix)
: QWidget(parentToolBarMontage)
{
    const QString objectNamePrefix(parentObjectNamePrefix
                                   + ":SurfaceMontageCerebellum");
    
    m_parentToolBarMontage = parentToolBarMontage;
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_dorsalCheckBox = new QCheckBox("Dorsal");
    QObject::connect(m_dorsalCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_dorsalCheckBox->setObjectName(objectNamePrefix
                                    + ":EnableDorsalView");
    m_dorsalCheckBox->setToolTip("Enable Dorsal View");
    macroManager->addMacroSupportToObject(m_dorsalCheckBox,
                                          "Enable dorsal view in cerebellar montage");
    
    m_ventralCheckBox = new QCheckBox("Ventral");
    QObject::connect(m_ventralCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_ventralCheckBox->setObjectName(objectNamePrefix
                                    + ":EnableVentralView");
    m_ventralCheckBox->setToolTip("Enable Ventral View");
    macroManager->addMacroSupportToObject(m_ventralCheckBox,
                                          "Enable ventral view in cerebellar montage");
    
    m_anteriorCheckBox = new QCheckBox("Anterior");
    QObject::connect(m_anteriorCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_anteriorCheckBox->setObjectName(objectNamePrefix
                                    + ":EnableAnteriorView");
    m_anteriorCheckBox->setToolTip("Enable Anterior View");
    macroManager->addMacroSupportToObject(m_anteriorCheckBox,
                                          "Enable anterior view in cerebellar montage");
    
    m_posteriorCheckBox = new QCheckBox("Posterior");
    QObject::connect(m_posteriorCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_posteriorCheckBox->setObjectName(objectNamePrefix
                                    + ":EnablePosteriorView");
    m_posteriorCheckBox->setToolTip("Enable Posterior View");
    macroManager->addMacroSupportToObject(m_posteriorCheckBox,
                                          "Enable posterior view in cerebellar montage");
    
    m_firstSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_firstSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_firstSurfaceCheckBox->setObjectName(objectNamePrefix
                                    + ":EnableFirstSurface");
    m_firstSurfaceCheckBox->setToolTip("Enable First Cerebellar Surface");
    macroManager->addMacroSupportToObject(m_firstSurfaceCheckBox,
                                          "Enable first in cerebellar montage");
    
    m_secondSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_secondSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_secondSurfaceCheckBox->setObjectName(objectNamePrefix
                                    + ":EnableSecondSurface");
    m_secondSurfaceCheckBox->setToolTip("Enable Second Cerebellar Surface");
    macroManager->addMacroSupportToObject(m_secondSurfaceCheckBox,
                                          "Enable second surface in cerebellar montage");
    
    m_firstSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                      objectNamePrefix
                                                                      + ":SurfaceFirst",
                                                                      "cerebellar monage first");
    QObject::connect(m_firstSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(firstSurfaceSelected(Surface*)));
    
    m_secondSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                       objectNamePrefix
                                                                       + ":SecondSurface",
                                                                       "cerebellar montage second");
    
    QObject::connect(m_secondSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(secondSurfaceSelected(Surface*)));
    
    QHBoxLayout* checkBoxLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(checkBoxLayout, 2, 0);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_dorsalCheckBox);
    checkBoxLayout->addSpacing(5);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_ventralCheckBox);
    checkBoxLayout->addSpacing(5);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_anteriorCheckBox);
    checkBoxLayout->addSpacing(5);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_posteriorCheckBox);
    checkBoxLayout->addStretch();
    
    int32_t columnIndex = 0;
    const int32_t COLUMN_CHECKBOX  = columnIndex++;
    const int32_t COLUMN_SELECTION = columnIndex++;
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(COLUMN_CHECKBOX,    0);
    layout->setColumnStretch(COLUMN_SELECTION, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    int row = layout->rowCount();
    layout->addWidget(m_firstSurfaceCheckBox, row, COLUMN_CHECKBOX);
    layout->addWidget(m_firstSurfaceViewController->getWidget(), row, COLUMN_SELECTION);
    row = layout->rowCount();
    layout->addWidget(m_secondSurfaceCheckBox, row, COLUMN_CHECKBOX);
    layout->addWidget(m_secondSurfaceViewController->getWidget(), row, COLUMN_SELECTION);
    row = layout->rowCount();
    layout->addLayout(checkBoxLayout,
                      row, COLUMN_CHECKBOX,
                      1, 2);
    row = layout->rowCount();
    
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_firstSurfaceViewController->getWidget());
    m_widgetGroup->add(m_secondSurfaceViewController->getWidget());
    m_widgetGroup->add(m_firstSurfaceCheckBox);
    m_widgetGroup->add(m_secondSurfaceCheckBox);
    m_widgetGroup->add(m_dorsalCheckBox);
    m_widgetGroup->add(m_ventralCheckBox);
    m_widgetGroup->add(m_anteriorCheckBox);
    m_widgetGroup->add(m_posteriorCheckBox);

    setFixedHeight(sizeHint().height());
}

/**
 * Destructor.
 */
SurfaceMontageCerebellarComponent::~SurfaceMontageCerebellarComponent()
{
    
}

/**
 * Update the cerebellar montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
SurfaceMontageCerebellarComponent::updateContent(BrowserTabContent* browserTabContent)
{
    m_widgetGroup->blockAllSignals(true);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    ModelSurfaceMontage* msm = browserTabContent->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationCerebellar* smcc = msm->getCerebellarConfiguration(tabIndex);
    
    m_firstSurfaceCheckBox->setChecked(smcc->isFirstSurfaceEnabled());
    m_secondSurfaceCheckBox->setChecked(smcc->isSecondSurfaceEnabled());
    
    m_dorsalCheckBox->setChecked(smcc->isDorsalEnabled());
    m_ventralCheckBox->setChecked(smcc->isVentralEnabled());
    m_anteriorCheckBox->setChecked(smcc->isAnteriorEnabled());
    m_posteriorCheckBox->setChecked(smcc->isPosteriorEnabled());
    
    m_firstSurfaceViewController->updateControl(smcc->getFirstSurfaceSelectionModel());
    m_secondSurfaceViewController->updateControl(smcc->getSecondSurfaceSelectionModel());
    
    m_widgetGroup->blockAllSignals(false);
}

/**
 * Called when first cerebellar surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageCerebellarComponent::firstSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationCerebellar* smcc = msm->getCerebellarConfiguration(tabIndex);
        smcc->getFirstSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when second cerebellar surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageCerebellarComponent::secondSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationCerebellar* smcc = msm->getCerebellarConfiguration(tabIndex);
        smcc->getSecondSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called cerebellar surface montage checkbox is toggled.
 * @param status
 *    New status of check box.
 */
void
SurfaceMontageCerebellarComponent::checkBoxSelected(bool /*status*/)
{
    BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationCerebellar* smcc = msm->getCerebellarConfiguration(tabIndex);
    
    smcc->setDorsalEnabled(m_dorsalCheckBox->isChecked());
    smcc->setVentralEnabled(m_ventralCheckBox->isChecked());
    smcc->setAnteriorEnabled(m_anteriorCheckBox->isChecked());
    smcc->setPosteriorEnabled(m_posteriorCheckBox->isChecked());
    
    smcc->setFirstSurfaceEnabled(m_firstSurfaceCheckBox->isChecked());
    smcc->setSecondSurfaceEnabled(m_secondSurfaceCheckBox->isChecked());
    
    m_parentToolBarMontage->updateUserInterface();
    m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
}



/*
 ********************************************************************************************
 */

/**
 * \class caret::SurfaceMontageFlatMapsComponent
 * \brief Flat Surface Montage Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 * @param objectNamePrefix
 *    Prefix name for naming objects
 */
SurfaceMontageFlatMapsComponent::SurfaceMontageFlatMapsComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage,
                                                                 const QString& parentObjectNamePrefix)
: QWidget(parentToolBarMontage)
{
    const QString objectNamePrefix(parentObjectNamePrefix
                                   + ":SurfaceMontageFlat");
    
    m_parentToolBarMontage = parentToolBarMontage;
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_leftSurfaceCheckBox = new QCheckBox("Left");
    QObject::connect(m_leftSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_leftSurfaceCheckBox->setObjectName(objectNamePrefix
                                         + ":EnableLeft");
    m_leftSurfaceCheckBox->setToolTip("Enable Left Flat Surface");
    macroManager->addMacroSupportToObject(m_leftSurfaceCheckBox,
                                          "Enable left surface in flat montage");
    
    m_rightSurfaceCheckBox = new QCheckBox("Right");
    QObject::connect(m_rightSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_rightSurfaceCheckBox->setObjectName(objectNamePrefix
                                         + ":EnableRight");
    m_rightSurfaceCheckBox->setToolTip("Enable Right Flat Surface");
    macroManager->addMacroSupportToObject(m_rightSurfaceCheckBox,
                                          "Enable right surface in flat montage");
    
    m_cerebellumSurfaceCheckBox = new QCheckBox("Cerebellum ");
    QObject::connect(m_cerebellumSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    m_cerebellumSurfaceCheckBox->setObjectName(objectNamePrefix
                                         + ":EnableCerebellum");
    m_cerebellumSurfaceCheckBox->setToolTip("Enable Cerebellum Flat Surface");
    macroManager->addMacroSupportToObject(m_cerebellumSurfaceCheckBox,
                                          "Enable cerebellar surface in flat montage");
    
    m_leftSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                     objectNamePrefix
                                                                     + ":LeftFlatSurface",
                                                                     "montage flat left");
    QObject::connect(m_leftSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(leftSurfaceSelected(Surface*)));
    
    m_rightSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                      objectNamePrefix
                                                                      + ":RightFlatSurface",
                                                                      "montage flat right");
    QObject::connect(m_rightSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(rightSurfaceSelected(Surface*)));
    
    m_cerebellumSurfaceViewController = new SurfaceSelectionViewController(this,
                                                                           objectNamePrefix
                                                                           + ":CerebellumSurface",
                                                                           "montage flat cerebellum");
    QObject::connect(m_cerebellumSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(cerebellumSurfaceSelected(Surface*)));
    
    int32_t columnIndex = 0;
    const int32_t COLUMN_CHECKBOX  = columnIndex++;
    const int32_t COLUMN_SELECTION = columnIndex++;
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(COLUMN_CHECKBOX,    0);
    layout->setColumnStretch(COLUMN_SELECTION, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 2);
    int row = layout->rowCount();
    layout->addWidget(m_leftSurfaceCheckBox, row, COLUMN_CHECKBOX);
    layout->addWidget(m_leftSurfaceViewController->getWidget(), row, COLUMN_SELECTION);
    row = layout->rowCount();
    layout->addWidget(m_rightSurfaceCheckBox, row, COLUMN_CHECKBOX);
    layout->addWidget(m_rightSurfaceViewController->getWidget(), row, COLUMN_SELECTION);
    row = layout->rowCount();
    layout->addWidget(m_cerebellumSurfaceCheckBox, row, COLUMN_CHECKBOX);
    layout->addWidget(m_cerebellumSurfaceViewController->getWidget(), row, COLUMN_SELECTION);
    row = layout->rowCount();
    
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_leftSurfaceViewController->getWidget());
    m_widgetGroup->add(m_rightSurfaceViewController->getWidget());
    m_widgetGroup->add(m_cerebellumSurfaceViewController->getWidget());
    m_widgetGroup->add(m_leftSurfaceCheckBox);
    m_widgetGroup->add(m_rightSurfaceCheckBox);
    m_widgetGroup->add(m_cerebellumSurfaceCheckBox);
        
    setFixedHeight(sizeHint().height());
}

/**
 * Destructor.
 */
SurfaceMontageFlatMapsComponent::~SurfaceMontageFlatMapsComponent()
{
    
}

/**
 * Update the flat maps montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
SurfaceMontageFlatMapsComponent::updateContent(BrowserTabContent* browserTabContent)
{
    m_widgetGroup->blockAllSignals(true);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    ModelSurfaceMontage* msm = browserTabContent->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationFlatMaps* smcc = msm->getFlatMapsConfiguration(tabIndex);
    
    m_leftSurfaceCheckBox->setChecked(smcc->isLeftEnabled());
    m_rightSurfaceCheckBox->setChecked(smcc->isRightEnabled());
    m_cerebellumSurfaceCheckBox->setChecked(smcc->isCerebellumEnabled());
    
    m_leftSurfaceViewController->updateControl(smcc->getLeftSurfaceSelectionModel());
    m_rightSurfaceViewController->updateControl(smcc->getRightSurfaceSelectionModel());
    m_cerebellumSurfaceViewController->updateControl(smcc->getCerebellumSurfaceSelectionModel());
    
    m_widgetGroup->blockAllSignals(false);
}

/**
 * Called when flat left surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageFlatMapsComponent::leftSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationFlatMaps* smcc = msm->getFlatMapsConfiguration(tabIndex);
        smcc->getLeftSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when flat right surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageFlatMapsComponent::rightSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationFlatMaps* smcc = msm->getFlatMapsConfiguration(tabIndex);
        smcc->getRightSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when flat cerebellum surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageFlatMapsComponent::cerebellumSurfaceSelected(Surface* surface)
{
    if (surface != NULL) {
        BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
        const int32_t tabIndex = btc->getTabNumber();
        ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
        SurfaceMontageConfigurationFlatMaps* smcc = msm->getFlatMapsConfiguration(tabIndex);
        smcc->getCerebellumSurfaceSelectionModel()->setSurface(surface);
        m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
    }
}

/**
 * Called when flat maps surface montage checkbox is toggled.
 * @param status
 *    New status of check box.
 */
void
SurfaceMontageFlatMapsComponent::checkBoxSelected(bool /*status*/)
{
    BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationFlatMaps* smcc = msm->getFlatMapsConfiguration(tabIndex);
    
    smcc->setLeftEnabled(m_leftSurfaceCheckBox->isChecked());
    smcc->setRightEnabled(m_rightSurfaceCheckBox->isChecked());
    smcc->setCerebellumEnabled(m_cerebellumSurfaceCheckBox->isChecked());
    
    m_parentToolBarMontage->updateUserInterface();
    m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
}

