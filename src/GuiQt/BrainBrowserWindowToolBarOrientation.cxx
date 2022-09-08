
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_ORIENTATION_DECLARE__
#include "BrainBrowserWindowToolBarOrientation.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_ORIENTATION_DECLARE__

#include "CaretAssert.h"
using namespace caret;

#include <QAction>
#include <QGridLayout>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "Model.h"
#include "ModelHistology.h"
#include "ModelMedia.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "Surface.h"
#include "StructureEnum.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

    
/**
 * \class caret::BrainBrowserWindowToolBarOrientation 
 * \brief Toolbar component for orientation controls
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parentObjectName
 *     Name of the parent object
 * @param parentToolBar
 *     The parent toolbar
 */
BrainBrowserWindowToolBarOrientation::BrainBrowserWindowToolBarOrientation(const QString& parentObjectName,
                                                                           BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    CaretAssert(macroManager);
    const QString objectNamePrefix(parentObjectName
                                   + ":Orientation:");
    
    this->viewOrientationLeftIcon = WuQtUtilities::loadIcon(":/ToolBar/view-left.png");
    this->viewOrientationRightIcon = WuQtUtilities::loadIcon(":/ToolBar/view-right.png");
    this->viewOrientationAnteriorIcon = WuQtUtilities::loadIcon(":/ToolBar/view-anterior.png");
    this->viewOrientationPosteriorIcon = WuQtUtilities::loadIcon(":/ToolBar/view-posterior.png");
    this->viewOrientationDorsalIcon = WuQtUtilities::loadIcon(":/ToolBar/view-dorsal.png");
    this->viewOrientationVentralIcon = WuQtUtilities::loadIcon(":/ToolBar/view-ventral.png");
    this->viewOrientationLeftLateralIcon = WuQtUtilities::loadIcon(":/ToolBar/view-left-lateral.png");
    this->viewOrientationLeftMedialIcon = WuQtUtilities::loadIcon(":/ToolBar/view-left-medial.png");
    this->viewOrientationRightLateralIcon = WuQtUtilities::loadIcon(":/ToolBar/view-right-lateral.png");
    this->viewOrientationRightMedialIcon = WuQtUtilities::loadIcon(":/ToolBar/view-right-medial.png");
    
    this->orientationLeftOrLateralToolButtonAction = WuQtUtilities::createAction("L",
                                                                                 "View from a LEFT perspective",
                                                                                 this,
                                                                                 this,
                                                                                 SLOT(orientationLeftOrLateralToolButtonTriggered(bool)));
    if (this->viewOrientationLeftIcon != NULL) {
        this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationLeftIcon);
    }
    else {
        this->orientationLeftOrLateralToolButtonAction->setIconText("L");
    }
    this->orientationLeftOrLateralToolButtonAction->setObjectName(objectNamePrefix
                                                                  + "LeftOrLateralView");
    macroManager->addMacroSupportToObject(this->orientationLeftOrLateralToolButtonAction,
                                          "Select left or lateral orientation");
    
    this->orientationRightOrMedialToolButtonAction = WuQtUtilities::createAction("R",
                                                                                 "View from a RIGHT perspective",
                                                                                 this,
                                                                                 this,
                                                                                 SLOT(orientationRightOrMedialToolButtonTriggered(bool)));
    if (this->viewOrientationRightIcon != NULL) {
        this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationRightIcon);
    }
    else {
        this->orientationRightOrMedialToolButtonAction->setIconText("R");
    }
    this->orientationRightOrMedialToolButtonAction->setObjectName(objectNamePrefix
                                                                  + "RightOrMedialView");
    macroManager->addMacroSupportToObject(this->orientationRightOrMedialToolButtonAction,
                                          "Select right or medial orientation");
    
    this->orientationAnteriorToolButtonAction = WuQtUtilities::createAction("A",
                                                                            "View from an ANTERIOR perspective",
                                                                            this,
                                                                            this,
                                                                            SLOT(orientationAnteriorToolButtonTriggered(bool)));
    if (this->viewOrientationAnteriorIcon != NULL) {
        this->orientationAnteriorToolButtonAction->setIcon(*this->viewOrientationAnteriorIcon);
    }
    else {
        this->orientationAnteriorToolButtonAction->setIconText("A");
    }
    this->orientationAnteriorToolButtonAction->setObjectName(objectNamePrefix
                                                             + "AnteriorView");
    macroManager->addMacroSupportToObject(this->orientationAnteriorToolButtonAction,
                                          "Select anterior orientation");
    
    this->orientationPosteriorToolButtonAction = WuQtUtilities::createAction("P",
                                                                             "View from a POSTERIOR perspective",
                                                                             this,
                                                                             this,
                                                                             SLOT(orientationPosteriorToolButtonTriggered(bool)));
    if (this->viewOrientationPosteriorIcon != NULL) {
        this->orientationPosteriorToolButtonAction->setIcon(*this->viewOrientationPosteriorIcon);
    }
    else {
        this->orientationPosteriorToolButtonAction->setIconText("P");
    }
    this->orientationPosteriorToolButtonAction->setObjectName(objectNamePrefix
                                                              + "PosteriorView");
    macroManager->addMacroSupportToObject(this->orientationPosteriorToolButtonAction,
                                          "Select posterior orientation");
    
    this->orientationDorsalToolButtonAction = WuQtUtilities::createAction("D",
                                                                          "View from a DORSAL perspective",
                                                                          this,
                                                                          this,
                                                                          SLOT(orientationDorsalToolButtonTriggered(bool)));
    if (this->viewOrientationDorsalIcon != NULL) {
        this->orientationDorsalToolButtonAction->setIcon(*this->viewOrientationDorsalIcon);
    }
    else {
        this->orientationDorsalToolButtonAction->setIconText("D");
    }
    this->orientationDorsalToolButtonAction->setObjectName(objectNamePrefix
                                                           + "DorsalView");
    macroManager->addMacroSupportToObject(this->orientationDorsalToolButtonAction,
                                          "Select dorsal orientation");
    
    this->orientationVentralToolButtonAction = WuQtUtilities::createAction("V",
                                                                           "View from a VENTRAL perspective",
                                                                           this,
                                                                           this,
                                                                           SLOT(orientationVentralToolButtonTriggered(bool)));
    if (this->viewOrientationVentralIcon != NULL) {
        this->orientationVentralToolButtonAction->setIcon(*this->viewOrientationVentralIcon);
    }
    else {
        this->orientationVentralToolButtonAction->setIconText("V");
    }
    this->orientationVentralToolButtonAction->setObjectName(objectNamePrefix
                                                            + "VentralView");
    macroManager->addMacroSupportToObject(this->orientationVentralToolButtonAction,
                                          "Select ventral orientation");
    
    
    this->orientationLateralMedialToolButtonAction = WuQtUtilities::createAction("LM",
                                                                                 "View from a Lateral/Medial perspective",
                                                                                 this,
                                                                                 this,
                                                                                 SLOT(orientationLateralMedialToolButtonTriggered(bool)));
    this->orientationLateralMedialToolButtonAction->setObjectName(objectNamePrefix
                                                                  + "LateralMedialView");
    macroManager->addMacroSupportToObject(this->orientationLateralMedialToolButtonAction,
                                          "Select lateral/medial orientation");
    
    this->orientationDorsalVentralToolButtonAction = WuQtUtilities::createAction("DV",
                                                                                 "View from a Dorsal/Ventral perspective",
                                                                                 this,
                                                                                 this,
                                                                                 SLOT(orientationDorsalVentralToolButtonTriggered(bool)));
    this->orientationDorsalVentralToolButtonAction->setObjectName(objectNamePrefix
                                                                  + "DorsalVentralView");
    macroManager->addMacroSupportToObject(this->orientationDorsalVentralToolButtonAction,
                                          "Select dorsal/ventral orientation");
    
    this->orientationAnteriorPosteriorToolButtonAction = WuQtUtilities::createAction("AP",
                                                                                     "View from a Anterior/Posterior perspective",
                                                                                     this,
                                                                                     this,
                                                                                     SLOT(orientationAnteriorPosteriorToolButtonTriggered(bool)));
    this->orientationAnteriorPosteriorToolButtonAction->setObjectName(objectNamePrefix
                                                                      + "AnteriorPosteriorView");
    macroManager->addMacroSupportToObject(this->orientationAnteriorPosteriorToolButtonAction,
                                          "Select anterior/posterior orientation");
    
    this->orientationResetToolButtonAction = WuQtUtilities::createAction("R\nE\nS\nE\nT",
                                                                         "Reset the view to lateral and remove any panning or zooming",
                                                                         this,
                                                                         this,
                                                                         SLOT(orientationResetToolButtonTriggered(bool)));
    this->orientationResetToolButtonAction->setObjectName(objectNamePrefix
                                                          + "ResetView");
    macroManager->addMacroSupportToObject(this->orientationResetToolButtonAction,
                                          "Reset to default orientation");
    
    this->orientationLeftOrLateralToolButton = new QToolButton();
    this->orientationLeftOrLateralToolButton->setDefaultAction(this->orientationLeftOrLateralToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationLeftOrLateralToolButton);
    this->orientationLeftOrLateralToolButtonAction->setParent(this->orientationLeftOrLateralToolButton);
    
    this->orientationRightOrMedialToolButton = new QToolButton();
    this->orientationRightOrMedialToolButton->setDefaultAction(this->orientationRightOrMedialToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationRightOrMedialToolButton);
    orientationRightOrMedialToolButtonAction->setParent(orientationRightOrMedialToolButton);
    
    this->orientationAnteriorToolButton = new QToolButton();
    this->orientationAnteriorToolButton->setDefaultAction(this->orientationAnteriorToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationAnteriorToolButton);
    this->orientationAnteriorToolButtonAction->setParent(this->orientationAnteriorToolButton);
    
    this->orientationPosteriorToolButton = new QToolButton();
    this->orientationPosteriorToolButton->setDefaultAction(this->orientationPosteriorToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationPosteriorToolButton);
    this->orientationPosteriorToolButtonAction->setParent(this->orientationPosteriorToolButton);
    
    this->orientationDorsalToolButton = new QToolButton();
    this->orientationDorsalToolButton->setDefaultAction(this->orientationDorsalToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationDorsalToolButton);
    this->orientationDorsalToolButtonAction->setParent(this->orientationDorsalToolButton);
    
    this->orientationVentralToolButton = new QToolButton();
    this->orientationVentralToolButton->setDefaultAction(this->orientationVentralToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationVentralToolButton);
    this->orientationVentralToolButtonAction->setParent(this->orientationVentralToolButton);
    
    this->orientationLateralMedialToolButton = new QToolButton();
    this->orientationLateralMedialToolButton->setDefaultAction(this->orientationLateralMedialToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationLateralMedialToolButton);
    orientationLateralMedialToolButtonAction->setParent(orientationLateralMedialToolButton);
    
    this->orientationDorsalVentralToolButton = new QToolButton();
    this->orientationDorsalVentralToolButton->setDefaultAction(this->orientationDorsalVentralToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationDorsalVentralToolButton);
    orientationDorsalVentralToolButtonAction->setParent(orientationDorsalVentralToolButton);
    
    this->orientationAnteriorPosteriorToolButton = new QToolButton();
    this->orientationAnteriorPosteriorToolButton->setDefaultAction(this->orientationAnteriorPosteriorToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationAnteriorPosteriorToolButton);
    orientationAnteriorPosteriorToolButtonAction->setParent(orientationAnteriorPosteriorToolButton);
    
    WuQtUtilities::matchWidgetWidths(this->orientationLateralMedialToolButton,
                                     this->orientationDorsalVentralToolButton,
                                     this->orientationAnteriorPosteriorToolButton);
    
    QToolButton* orientationResetToolButton = new QToolButton();
    orientationResetToolButton->setDefaultAction(this->orientationResetToolButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(orientationResetToolButton);
    
    this->orientationCustomViewSelectToolButton = new QToolButton();
    this->orientationCustomViewSelectToolButton->setDefaultAction(getParentToolBar()->customViewAction);
    this->orientationCustomViewSelectToolButton->setSizePolicy(QSizePolicy::Minimum,
                                                               QSizePolicy::Fixed);
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationCustomViewSelectToolButton);
    
    QGridLayout* buttonGridLayout = new QGridLayout();
    buttonGridLayout->setColumnStretch(3, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(buttonGridLayout, 0, 0);
    buttonGridLayout->addWidget(this->orientationLeftOrLateralToolButton,      0, 0);
    buttonGridLayout->addWidget(this->orientationRightOrMedialToolButton,     0, 1);
    buttonGridLayout->addWidget(this->orientationDorsalToolButton,    1, 0);
    buttonGridLayout->addWidget(this->orientationVentralToolButton,   1, 1);
    buttonGridLayout->addWidget(this->orientationAnteriorToolButton,  2, 0);
    buttonGridLayout->addWidget(this->orientationPosteriorToolButton, 2, 1);
    buttonGridLayout->addWidget(this->orientationLateralMedialToolButton, 0, 2);
    buttonGridLayout->addWidget(this->orientationDorsalVentralToolButton, 1, 2);
    buttonGridLayout->addWidget(this->orientationAnteriorPosteriorToolButton, 2, 2);
    buttonGridLayout->addWidget(this->orientationCustomViewSelectToolButton, 3, 0, 1, 5, Qt::AlignHCenter);
    buttonGridLayout->addWidget(orientationResetToolButton, 0, 4, 3, 1);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(buttonGridLayout);
    
    addToWidgetGroup(this->orientationLeftOrLateralToolButtonAction);
    addToWidgetGroup(this->orientationRightOrMedialToolButtonAction);
    addToWidgetGroup(this->orientationAnteriorToolButtonAction);
    addToWidgetGroup(this->orientationPosteriorToolButtonAction);
    addToWidgetGroup(this->orientationDorsalToolButtonAction);
    addToWidgetGroup(this->orientationVentralToolButtonAction);
    addToWidgetGroup(this->orientationResetToolButtonAction);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarOrientation::~BrainBrowserWindowToolBarOrientation()
{
    if (this->viewOrientationLeftIcon != NULL) {
        delete this->viewOrientationLeftIcon;
        this->viewOrientationLeftIcon = NULL;
    }
    if (this->viewOrientationRightIcon != NULL) {
        delete this->viewOrientationRightIcon;
        this->viewOrientationRightIcon = NULL;
    }
    
    if (this->viewOrientationAnteriorIcon != NULL) {
        delete this->viewOrientationAnteriorIcon;
        this->viewOrientationAnteriorIcon = NULL;
    }
    
    if (this->viewOrientationPosteriorIcon != NULL) {
        delete this->viewOrientationPosteriorIcon;
        this->viewOrientationPosteriorIcon = NULL;
    }
    
    if (this->viewOrientationDorsalIcon != NULL) {
        delete this->viewOrientationDorsalIcon;
        this->viewOrientationDorsalIcon = NULL;
    }
    
    if (this->viewOrientationVentralIcon != NULL) {
        delete this->viewOrientationVentralIcon;
        this->viewOrientationVentralIcon = NULL;
    }
    
    if (this->viewOrientationLeftLateralIcon != NULL) {
        delete this->viewOrientationLeftLateralIcon;
        this->viewOrientationLeftLateralIcon = NULL;
    }
    
    if (this->viewOrientationLeftMedialIcon != NULL) {
        delete this->viewOrientationLeftMedialIcon;
        this->viewOrientationLeftMedialIcon = NULL;
    }
    
    if (this->viewOrientationRightLateralIcon != NULL) {
        delete this->viewOrientationRightLateralIcon;
        this->viewOrientationRightLateralIcon = NULL;
    }
    
    if (this->viewOrientationRightMedialIcon != NULL) {
        delete this->viewOrientationRightMedialIcon;
        this->viewOrientationRightMedialIcon = NULL;
    }
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarOrientation::updateContent(BrowserTabContent* browserTabContent)
{
    const int32_t tabIndex = browserTabContent->getTabNumber();
    
    blockAllSignals(true);
    
    const Model* mdc = getParentToolBar()->getDisplayedModel();
    if (mdc != NULL) {
        const ModelHistology* mh = dynamic_cast<const ModelHistology*>(mdc);
        const ModelMedia* mdm    = dynamic_cast<const ModelMedia*>(mdc);
        const ModelSurface* mdcs = dynamic_cast<const ModelSurface*>(mdc);
        const ModelSurfaceMontage* mdcsm = dynamic_cast<const ModelSurfaceMontage*>(mdc);
        const ModelVolume* mdcv = dynamic_cast<const ModelVolume*>(mdc);
        const ModelWholeBrain* mdcwb = dynamic_cast<const ModelWholeBrain*>(mdc);
        
        bool rightFlag = false;
        bool leftFlag = false;
        bool leftRightFlag = false;
        
        bool enableDualViewOrientationButtons = false;
        bool showDualViewOrientationButtons = false;
        bool showSingleViewOrientationButtons = false;
        
        if (mdcs != NULL) {
            const Surface* surface = mdcs->getSurface();
            const StructureEnum::Enum structure = surface->getStructure();
            if (StructureEnum::isLeft(structure)) {
                leftFlag = true;
            }
            else if (StructureEnum::isRight(structure)) {
                rightFlag = true;
            }
            else {
                leftRightFlag = true;
            }
            
            showSingleViewOrientationButtons = true;
        }
        else if (mdcsm != NULL) {
            AString latMedLeftRightText = "LM";
            AString latMedLeftRightToolTipText = "View from a Lateral/Medial perspective";
            switch (mdcsm->getSelectedConfigurationType(tabIndex)) {
                case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
                    latMedLeftRightText = "LR";
                    latMedLeftRightToolTipText = "View from a Right/Left Perspective";
                    enableDualViewOrientationButtons = true;
                    break;
                case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
                    enableDualViewOrientationButtons = true;
                    break;
                case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
                    break;
            }
            
            this->orientationLateralMedialToolButtonAction->setText(latMedLeftRightText);
            WuQtUtilities::setToolTipAndStatusTip(this->orientationLateralMedialToolButtonAction,
                                                  latMedLeftRightToolTipText);
            
            showDualViewOrientationButtons = true;
        }
        else if (mdcv != NULL) {
            /* nothing */
        }
        else if (mdcwb != NULL) {
            leftRightFlag = true;
            showSingleViewOrientationButtons = true;
        }
        else if (mh != NULL) {
            /* nothing */
        }
        else if (mdm != NULL) {
            /* nothing */
        }
        else {
            CaretAssertMessage(0, "Unknown model display controller type");
        }
        
        if (rightFlag || leftFlag) {
            if (rightFlag) {
                if (this->viewOrientationRightLateralIcon != NULL) {
                    this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationRightLateralIcon);
                }
                else {
                    this->orientationLeftOrLateralToolButtonAction->setIconText("L");
                }
                if (this->viewOrientationRightMedialIcon != NULL) {
                    this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationRightMedialIcon);
                }
                else {
                    this->orientationRightOrMedialToolButtonAction->setIconText("M");
                }
            }
            else if (leftFlag) {
                if (this->viewOrientationLeftLateralIcon != NULL) {
                    this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationLeftLateralIcon);
                }
                else {
                    this->orientationLeftOrLateralToolButtonAction->setIconText("L");
                }
                if (this->viewOrientationLeftMedialIcon != NULL) {
                    this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationLeftMedialIcon);
                }
                else {
                    this->orientationRightOrMedialToolButtonAction->setIconText("M");
                }
            }
            WuQtUtilities::setToolTipAndStatusTip(this->orientationLeftOrLateralToolButtonAction,
                                                  "View from a LATERAL perspective");
            WuQtUtilities::setToolTipAndStatusTip(this->orientationRightOrMedialToolButtonAction,
                                                  "View from a MEDIAL perspective");
        }
        else if (leftRightFlag) {
            if (this->viewOrientationLeftIcon != NULL) {
                this->orientationLeftOrLateralToolButtonAction->setIcon(*this->viewOrientationLeftIcon);
            }
            else {
                this->orientationLeftOrLateralToolButtonAction->setIconText("L");
            }
            if (this->viewOrientationRightIcon != NULL) {
                this->orientationRightOrMedialToolButtonAction->setIcon(*this->viewOrientationRightIcon);
            }
            else {
                this->orientationRightOrMedialToolButtonAction->setIconText("R");
            }
            WuQtUtilities::setToolTipAndStatusTip(this->orientationLeftOrLateralToolButtonAction,
                                                  "View from a LEFT perspective");
            WuQtUtilities::setToolTipAndStatusTip(this->orientationRightOrMedialToolButtonAction,
                                                  "View from a RIGHT perspective");
        }
        
        /*
         * The dual view buttons are not need for a flat map montage.
         * However, if they are hidden, their space is not reallocated and the
         * Reset button remains on the right and it looks weird.  So,
         * display them but disable them when a flat map montage.
         */
        this->orientationLateralMedialToolButton->setVisible(showDualViewOrientationButtons);
        this->orientationDorsalVentralToolButton->setVisible(showDualViewOrientationButtons);
        this->orientationAnteriorPosteriorToolButton->setVisible(showDualViewOrientationButtons);
        
        this->orientationLateralMedialToolButton->setEnabled(enableDualViewOrientationButtons);
        this->orientationDorsalVentralToolButton->setEnabled(enableDualViewOrientationButtons);
        this->orientationAnteriorPosteriorToolButton->setEnabled(enableDualViewOrientationButtons);
        
        
        this->orientationLeftOrLateralToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationRightOrMedialToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationDorsalToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationVentralToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationAnteriorToolButton->setVisible(showSingleViewOrientationButtons);
        this->orientationPosteriorToolButton->setVisible(showSingleViewOrientationButtons);
    }
    
    blockAllSignals(false);
}

/**
 * Called when orientation left or lateral button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationLeftOrLateralToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->leftView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation right or medial button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationRightOrMedialToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->rightView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation anterior button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationAnteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->anteriorView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation posterior button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationPosteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->posteriorView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation dorsal button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationDorsalToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->dorsalView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation ventral button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationVentralToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->ventralView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation reset button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationResetToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->resetView();
    
    Model* mdc = btc->getModelForDisplay();
    if (mdc != NULL) {
        getParentToolBar()->updateVolumeIndicesWidget(btc);
        getParentToolBar()->updateGraphicsWindowAndYokedWindows();
        if (btc->isMediaDisplayed()) {
            const bool repaintFlag(true);
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows(repaintFlag).getPointer());
        }
    }
}

/**
 * Called when orientation lateral/medial button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationLateralMedialToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->leftView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation dorsal/ventral button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationDorsalVentralToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->dorsalView();
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when orientation anterior/posterior button is pressed.
 */
void
BrainBrowserWindowToolBarOrientation::orientationAnteriorPosteriorToolButtonTriggered(bool /*checked*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->anteriorView();
    this->updateGraphicsWindowAndYokedWindows();
}
