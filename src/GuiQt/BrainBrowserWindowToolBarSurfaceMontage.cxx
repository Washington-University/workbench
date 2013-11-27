
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
 */
BrainBrowserWindowToolBarSurfaceMontage::BrainBrowserWindowToolBarSurfaceMontage(BrainBrowserWindowToolBar* parentToolBar)
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

    m_surfaceMontageLayoutOrientationEnumComboBox = new EnumComboBoxTemplate(this);
    m_surfaceMontageLayoutOrientationEnumComboBox->setup<SurfaceMontageLayoutOrientationEnum,SurfaceMontageLayoutOrientationEnum::Enum>();
    QObject::connect(m_surfaceMontageLayoutOrientationEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(surfaceMontageLayoutOrientationEnumComboBoxItemActivated()));
    addToWidgetGroup(m_surfaceMontageLayoutOrientationEnumComboBox->getWidget());
    WuQtUtilities::setToolTipAndStatusTip(m_surfaceMontageLayoutOrientationEnumComboBox->getWidget(),
                                         ("Selects Surface Layout:\n"
                                          "   Landscape (Layout left-to-right)\n"
                                          "   Portrait  (Layout top-to-bottom)"));
    
    m_cerebellarComponent = new SurfaceMontageCerebellarComponent(this);
    
    m_cerebralComponent = new SurfaceMontageCerebralComponent(this);
    
    m_flatMapsComponent = new SurfaceMontageFlatMapsComponent(this);
    
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
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    m_surfaceMontageConfigurationTypeEnumComboBox->setSelectedItem<SurfaceMontageConfigurationTypeEnum,SurfaceMontageConfigurationTypeEnum::Enum>(msm->getSelectedConfigurationType(tabIndex));
    
    SurfaceMontageConfigurationAbstract* selectedConfiguration = msm->getSelectedConfiguration(tabIndex);
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
 */
SurfaceMontageCerebralComponent::SurfaceMontageCerebralComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage)
: QWidget(parentToolBarMontage)
{
    m_parentToolBarMontage = parentToolBarMontage;
    
    m_leftCheckBox = new QCheckBox("Left");
    QObject::connect(m_leftCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_rightCheckBox = new QCheckBox("Right");
    QObject::connect(m_rightCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_lateralCheckBox = new QCheckBox("Lateral");
    QObject::connect(m_lateralCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_medialCheckBox = new QCheckBox("Medial");
    QObject::connect(m_medialCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_surfaceMontageFirstSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_surfaceMontageFirstSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_surfaceMontageSecondSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_surfaceMontageSecondSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_leftSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_leftSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(leftSurfaceSelected(Surface*)));
    
    m_leftSecondSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_leftSecondSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(leftSecondSurfaceSelected(Surface*)));
    
    m_rightSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_rightSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(rightSurfaceSelected(Surface*)));
    
    m_rightSecondSurfaceViewController = new SurfaceSelectionViewController(this);
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
 */
SurfaceMontageCerebellarComponent::SurfaceMontageCerebellarComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage)
: QWidget(parentToolBarMontage)
{
    m_parentToolBarMontage = parentToolBarMontage;
    
    m_dorsalCheckBox = new QCheckBox("Dorsal");
    QObject::connect(m_dorsalCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_ventralCheckBox = new QCheckBox("Ventral");
    QObject::connect(m_ventralCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_anteriorCheckBox = new QCheckBox("Anterior");
    QObject::connect(m_anteriorCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_posteriorCheckBox = new QCheckBox("Posterior");
    QObject::connect(m_posteriorCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_firstSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_firstSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_secondSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_secondSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_firstSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_firstSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(firstSurfaceSelected(Surface*)));
    
    m_secondSurfaceViewController = new SurfaceSelectionViewController(this);
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
 */
SurfaceMontageFlatMapsComponent::SurfaceMontageFlatMapsComponent(BrainBrowserWindowToolBarSurfaceMontage* parentToolBarMontage)
: QWidget(parentToolBarMontage)
{
    m_parentToolBarMontage = parentToolBarMontage;
    
    m_leftSurfaceCheckBox = new QCheckBox("Left");
    QObject::connect(m_leftSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_rightSurfaceCheckBox = new QCheckBox("Right");
    QObject::connect(m_rightSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_cerebellumSurfaceCheckBox = new QCheckBox("Cerebellum ");
    QObject::connect(m_cerebellumSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(checkBoxSelected(bool)));
    
    m_leftSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_leftSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(leftSurfaceSelected(Surface*)));
    
    m_rightSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_rightSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(rightSurfaceSelected(Surface*)));
    
    m_cerebellumSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_cerebellumSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(cerebellumSurfaceSelected(Surface*)));
    
//    QHBoxLayout* checkBoxLayout = new QHBoxLayout();
//    WuQtUtilities::setLayoutSpacingAndMargins(checkBoxLayout, 2, 0);
//    checkBoxLayout->addStretch();
//    checkBoxLayout->addWidget(m_leftSurfaceCheckBox);
//    checkBoxLayout->addSpacing(5);
//    checkBoxLayout->addStretch();
//    checkBoxLayout->addWidget(m_rightSurfaceCheckBox);
//    checkBoxLayout->addSpacing(5);
//    checkBoxLayout->addStretch();
//    checkBoxLayout->addWidget(m_cerebellumSurfaceCheckBox);
//    checkBoxLayout->addStretch();
    
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
//    layout->addLayout(checkBoxLayout,
//                      row, COLUMN_CHECKBOX,
//                      1, 2);
//    row = layout->rowCount();
    
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

