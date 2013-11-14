
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
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "ModelSurfaceMontage.h"
#include "SurfaceMontageConfigurationCerebellar.h"
#include "SurfaceMontageConfigurationCerebral.h"
#include "SurfaceMontageConfigurationFlatMaps.h"
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
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_cerebralComponent = new SurfaceMontageCerebralComponent(this);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_cerebralComponent);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
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
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarSurfaceMontage::updateContent(BrowserTabContent* browserTabContent)
{
    m_cerebralComponent->updateContent(browserTabContent);
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
    
    m_surfaceMontageLeftCheckBox = new QCheckBox("Left");
    QObject::connect(m_surfaceMontageLeftCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(surfaceMontageCheckBoxSelected(bool)));
    
    m_surfaceMontageRightCheckBox = new QCheckBox("Right");
    QObject::connect(m_surfaceMontageRightCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(surfaceMontageCheckBoxSelected(bool)));
    
    m_lateralCheckBox = new QCheckBox("Lateral");
    QObject::connect(m_lateralCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(surfaceMontageCheckBoxSelected(bool)));
    
    m_medialCheckBox = new QCheckBox("Medial");
    QObject::connect(m_medialCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(surfaceMontageCheckBoxSelected(bool)));
    
    m_surfaceMontageFirstSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_surfaceMontageFirstSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(surfaceMontageCheckBoxSelected(bool)));
    
    m_surfaceMontageSecondSurfaceCheckBox = new QCheckBox(" ");
    QObject::connect(m_surfaceMontageSecondSurfaceCheckBox, SIGNAL(toggled(bool)),
                     this, SLOT(surfaceMontageCheckBoxSelected(bool)));
    
    m_surfaceMontageLeftSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_surfaceMontageLeftSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(surfaceMontageLeftSurfaceSelected(Surface*)));
    
    m_surfaceMontageLeftSecondSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_surfaceMontageLeftSecondSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(surfaceMontageLeftSecondSurfaceSelected(Surface*)));
    
    m_surfaceMontageRightSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_surfaceMontageRightSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(surfaceMontageRightSurfaceSelected(Surface*)));
    
    m_surfaceMontageRightSecondSurfaceViewController = new SurfaceSelectionViewController(this);
    QObject::connect(m_surfaceMontageRightSecondSurfaceViewController, SIGNAL(surfaceSelected(Surface*)),
                     this, SLOT(surfaceMontageRightSecondSurfaceSelected(Surface*)));
    
    QHBoxLayout* checkBoxLayout = new QHBoxLayout();
    checkBoxLayout->addWidget(m_surfaceMontageLeftCheckBox);
    checkBoxLayout->addSpacing(10);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_lateralCheckBox);
    checkBoxLayout->addSpacing(10);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_medialCheckBox);
    checkBoxLayout->addSpacing(10);
    checkBoxLayout->addStretch();
    checkBoxLayout->addWidget(m_surfaceMontageRightCheckBox);
    
    int32_t columnIndex = 0;
    const int32_t COLUMN_ONE_TWO     = columnIndex++;
    const int32_t COLUMN_INDEX_LEFT  = columnIndex++;
    const int32_t COLUMN_INDEX_RIGHT = columnIndex++;
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(0,   0);
    layout->setColumnStretch(1, 100);
    layout->setColumnStretch(2, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 2);
    int row = layout->rowCount();
    layout->addLayout(checkBoxLayout,
                      row, COLUMN_INDEX_LEFT,
                      1, 2);
    row = layout->rowCount();
    layout->addWidget(m_surfaceMontageFirstSurfaceCheckBox, row, COLUMN_ONE_TWO);
    layout->addWidget(m_surfaceMontageLeftSurfaceViewController->getWidget(), row, COLUMN_INDEX_LEFT);
    layout->addWidget(m_surfaceMontageRightSurfaceViewController->getWidget(), row, COLUMN_INDEX_RIGHT);
    row = layout->rowCount();
    layout->addWidget(m_surfaceMontageSecondSurfaceCheckBox, row, COLUMN_ONE_TWO);
    layout->addWidget(m_surfaceMontageLeftSecondSurfaceViewController->getWidget(), row, COLUMN_INDEX_LEFT);
    layout->addWidget(m_surfaceMontageRightSecondSurfaceViewController->getWidget(), row, COLUMN_INDEX_RIGHT);
    row = layout->rowCount();
    
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_surfaceMontageLeftSurfaceViewController->getWidget());
    m_widgetGroup->add(m_surfaceMontageLeftSecondSurfaceViewController->getWidget());
    m_widgetGroup->add(m_surfaceMontageRightSurfaceViewController->getWidget());
    m_widgetGroup->add(m_surfaceMontageRightSecondSurfaceViewController->getWidget());
    m_widgetGroup->add(m_surfaceMontageLeftCheckBox);
    m_widgetGroup->add(m_surfaceMontageRightCheckBox);
    m_widgetGroup->add(m_surfaceMontageFirstSurfaceCheckBox);
    m_widgetGroup->add(m_surfaceMontageSecondSurfaceCheckBox);
    m_widgetGroup->add(m_medialCheckBox);
    m_widgetGroup->add(m_lateralCheckBox);
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
    if (isHidden()) {
        return;
    }
    
    m_widgetGroup->blockAllSignals(true);
    
    const int32_t tabIndex = browserTabContent->getTabNumber();
    ModelSurfaceMontage* msm = browserTabContent->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
    
    m_surfaceMontageLeftCheckBox->setChecked(smcc->isLeftEnabled());
    m_surfaceMontageRightCheckBox->setChecked(smcc->isRightEnabled());
    m_surfaceMontageFirstSurfaceCheckBox->setChecked(smcc->isFirstSurfaceEnabled());
    m_surfaceMontageSecondSurfaceCheckBox->setChecked(smcc->isSecondSurfaceEnabled());
    m_lateralCheckBox->setChecked(smcc->isLateralEnabled());
    m_medialCheckBox->setChecked(smcc->isMedialEnabled());
    
    m_surfaceMontageLeftSurfaceViewController->updateControl(smcc->getLeftFirstSurfaceSelectionModel());
    m_surfaceMontageLeftSecondSurfaceViewController->updateControl(smcc->getLeftSecondSurfaceSelectionModel());
    m_surfaceMontageRightSurfaceViewController->updateControl(smcc->getRightFirstSurfaceSelectionModel());
    m_surfaceMontageRightSecondSurfaceViewController->updateControl(smcc->getRightSecondSurfaceSelectionModel());
    
    m_widgetGroup->blockAllSignals(false);
}

/**
 * Called when montage left surface is selected.
 * @param surface
 *    Surface that was selected.
 */
void
SurfaceMontageCerebralComponent::surfaceMontageLeftSurfaceSelected(Surface* surface)
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
SurfaceMontageCerebralComponent::surfaceMontageLeftSecondSurfaceSelected(Surface* surface)
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
SurfaceMontageCerebralComponent::surfaceMontageRightSurfaceSelected(Surface* surface)
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
SurfaceMontageCerebralComponent::surfaceMontageRightSecondSurfaceSelected(Surface* surface)
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
SurfaceMontageCerebralComponent::surfaceMontageCheckBoxSelected(bool /*status*/)
{
    BrowserTabContent* btc = m_parentToolBarMontage->getTabContentFromSelectedTab();
    const int32_t tabIndex = btc->getTabNumber();
    ModelSurfaceMontage* msm = btc->getDisplayedSurfaceMontageModel();
    SurfaceMontageConfigurationCerebral* smcc = msm->getCerebralConfiguration(tabIndex);
    smcc->setLeftEnabled(m_surfaceMontageLeftCheckBox->isChecked());
    smcc->setRightEnabled(m_surfaceMontageRightCheckBox->isChecked());
    smcc->setFirstSurfaceEnabled(m_surfaceMontageFirstSurfaceCheckBox->isChecked());
    smcc->setSecondSurfaceEnabled(m_surfaceMontageSecondSurfaceCheckBox->isChecked());
    smcc->setLateralEnabled(m_lateralCheckBox->isChecked());
    smcc->setMedialEnabled(m_medialCheckBox->isChecked());
    
    m_parentToolBarMontage->updateUserInterface();
    m_parentToolBarMontage->invalidateColoringAndUpdateGraphicsWindow();
}

