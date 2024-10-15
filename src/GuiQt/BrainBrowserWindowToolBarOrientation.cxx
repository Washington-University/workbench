
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

using namespace caret;
#include "CaretAssert.h"

#include <QAction>
#include <QGridLayout>
#include <QStackedLayout>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretUndoStack.h"
#include "EventGraphicsPaintNowAllWindows.h"
#include "EventGraphicsPaintSoonAllWindows.h"
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
#include "ViewingTransformations.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
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
    
    this->orientationResetToolButtonAction = WuQtUtilities::createAction("Reset",
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
    WuQtUtilities::setToolButtonStyleForQt5Mac(this->orientationCustomViewSelectToolButton);
        
    m_singleViewWidget = new QWidget();
    QGridLayout* singleViewLayout = new QGridLayout(m_singleViewWidget);
    singleViewLayout->setHorizontalSpacing(0);
    singleViewLayout->setVerticalSpacing(0);
    singleViewLayout->setContentsMargins(0, 0, 0, 0);
    singleViewLayout->addWidget(this->orientationLeftOrLateralToolButton, 0, 0);
    singleViewLayout->addWidget(this->orientationDorsalToolButton, 0, 1);
    singleViewLayout->addWidget(this->orientationAnteriorToolButton, 0, 2);
    singleViewLayout->addWidget(this->orientationRightOrMedialToolButton, 1, 0);
    singleViewLayout->addWidget(this->orientationVentralToolButton, 1, 1);
    singleViewLayout->addWidget(this->orientationPosteriorToolButton, 1, 2);

    m_dualViewWidget = new QWidget();
    QGridLayout* dualViewLayout = new QGridLayout(m_dualViewWidget);
    dualViewLayout->setHorizontalSpacing(0);
    dualViewLayout->setVerticalSpacing(0);
    dualViewLayout->setContentsMargins(0, 0, 0, 0);
    dualViewLayout->addWidget(this->orientationLateralMedialToolButton, 0, 0);
    dualViewLayout->addWidget(this->orientationDorsalVentralToolButton, 0, 1);
    dualViewLayout->addWidget(this->orientationAnteriorPosteriorToolButton, 1, 0, 1, 2, Qt::AlignHCenter);

    m_emptyViewWidget = new QWidget();
    
    m_singleDualViewLayout = new QStackedLayout();
    m_singleDualViewLayout->addWidget(m_singleViewWidget);
    m_singleDualViewLayout->addWidget(m_dualViewWidget);
    m_singleDualViewLayout->addWidget(m_emptyViewWidget);
    
    /*
     * Redo and Undo
     */
    QIcon redoIcon;
    const bool redoIconValid(WuQtUtilities::loadIcon(":/ToolBar/redo.png",
                                                     redoIcon));
    QIcon undoIcon;
    const bool undoIconValid(WuQtUtilities::loadIcon(":/ToolBar/undo.png",
                                                     undoIcon));

    m_redoAction = new QAction();
    if (redoIconValid) {
        m_redoAction->setIcon(redoIcon);
    }
    else {
        m_redoAction->setText("R");
    }
    m_redoAction->setToolTip("Redo change in view");
    QObject::connect(m_redoAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarOrientation::redoActionTriggered);
    QToolButton* redoToolButton = new QToolButton();
    redoToolButton->setDefaultAction(m_redoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(redoToolButton);
    m_redoAction->setObjectName(objectNamePrefix + ":Redo");
    WuQMacroManager::instance()->addMacroSupportToObject(m_redoAction, "Redo Image View");
    
    m_undoAction = new QAction();
    if (undoIconValid) {
        m_undoAction->setIcon(undoIcon);
    }
    else {
        m_undoAction->setText("U");
    }
    m_undoAction->setToolTip("Undo change in view");
    QObject::connect(m_undoAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarOrientation::undoActionTriggered);
    QToolButton* undoToolButton = new QToolButton();
    undoToolButton->setDefaultAction(m_undoAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(undoToolButton);
    m_undoAction->setObjectName(objectNamePrefix + ":Undo");
    WuQMacroManager::instance()->addMacroSupportToObject(m_undoAction, "Undo Image View");

    /*
     * Select region
     */
    QIcon selectRegionIcon;
    const bool selectRegionIconValid(WuQtUtilities::loadIcon(":/ToolBar/select-region.png",
                                                             selectRegionIcon));
    m_selectRegionAction = new QAction();
    if (selectRegionIconValid) {
        m_selectRegionAction->setIcon(selectRegionIcon);
    }
    else {
        m_selectRegionAction->setText("SR");
    }
    m_selectRegionAction->setToolTip("<html>Select region by dragging mouse to form a rectangle</html>");
    m_selectRegionAction->setCheckable(true);
    QObject::connect(m_selectRegionAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarOrientation::selectRegionActionTriggered);
    QToolButton* selectRegionToolButton(new QToolButton());
    selectRegionToolButton->setDefaultAction(m_selectRegionAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(selectRegionToolButton);
    m_selectRegionAction->setObjectName(objectNamePrefix + ":SelectRegion");
    WuQMacroManager::instance()->addMacroSupportToObject(m_selectRegionAction, "Selection Region");
    
    QHBoxLayout* redoUndoLayout(new QHBoxLayout());
    WuQtUtilities::setLayoutSpacingAndMargins(redoUndoLayout, 0, 0);
    redoUndoLayout->addWidget(redoToolButton);
    redoUndoLayout->addWidget(undoToolButton);
    redoUndoLayout->addWidget(selectRegionToolButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(m_singleDualViewLayout, 0);
    layout->addWidget(this->orientationCustomViewSelectToolButton, 0, Qt::AlignHCenter);
    layout->addWidget(orientationResetToolButton, 0, Qt::AlignHCenter);
    layout->addLayout(redoUndoLayout);
    
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
    m_browserTabContent = browserTabContent;
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
        this->orientationLateralMedialToolButton->setEnabled(enableDualViewOrientationButtons);
        this->orientationDorsalVentralToolButton->setEnabled(enableDualViewOrientationButtons);
        this->orientationAnteriorPosteriorToolButton->setEnabled(enableDualViewOrientationButtons);

        if (showDualViewOrientationButtons) {
            m_singleDualViewLayout->setCurrentWidget(m_dualViewWidget);
        }
        else if (showSingleViewOrientationButtons) {
            m_singleDualViewLayout->setCurrentWidget(m_singleViewWidget);
        }
        else {
            m_singleDualViewLayout->setCurrentWidget(m_emptyViewWidget);
        }
    }
    
    m_redoAction->setEnabled(false);
    m_undoAction->setEnabled(false);
    
    if (browserTabContent != NULL) {
        CaretUndoStack* undoStack = getUndoStack();
        if (undoStack != NULL) {
            m_redoAction->setEnabled(undoStack->canRedo());
            m_redoAction->setToolTip("Redo " + undoStack->redoText());
            
            m_undoAction->setEnabled(undoStack->canUndo());
            m_undoAction->setToolTip("Undo " + undoStack->undoText());
        }
    }
    
    updateRegionSelectionAction();
    
    blockAllSignals(false);
}

void
BrainBrowserWindowToolBarOrientation::updateRegionSelectionAction()
{
    if (m_browserTabContent != NULL) {
        const std::vector<MouseLeftDragModeEnum::Enum> mouseDragModes(m_browserTabContent->getSupportedMouseLeftDragModes());
        bool enableSelectRegionFlag(false);
        for (const auto mdm : mouseDragModes) {
            switch (mdm) {
                case MouseLeftDragModeEnum::INVALID:
                    CaretAssert(0);
                    break;
                case MouseLeftDragModeEnum::DEFAULT:
                    break;
                case MouseLeftDragModeEnum::REGION_SELECTION:
                    enableSelectRegionFlag = true;
                    break;
            }
        }
        m_selectRegionAction->setEnabled(enableSelectRegionFlag);
        
        switch (m_browserTabContent->getMouseLeftDragMode()) {
            case MouseLeftDragModeEnum::INVALID:
                CaretAssert(0);
                break;
            case MouseLeftDragModeEnum::DEFAULT:
                m_selectRegionAction->setChecked(false);
                break;
            case MouseLeftDragModeEnum::REGION_SELECTION:
                m_selectRegionAction->setChecked(true);
                break;
        }
    }
    else {
        m_selectRegionAction->setEnabled(false);
    }
    
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
            EventManager::get()->sendEvent(EventGraphicsPaintNowAllWindows().getPointer());
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

/**
 * Gets called when the redo action is triggered
 */
void
BrainBrowserWindowToolBarOrientation::redoActionTriggered()
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
BrainBrowserWindowToolBarOrientation::undoActionTriggered()
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
 * Gets called when the select region action is triggered
 * @param checked
 *    New checked status
 */
void
BrainBrowserWindowToolBarOrientation::selectRegionActionTriggered(const bool checked)
{
    if (m_browserTabContent != NULL) {
        if (checked) {
            m_browserTabContent->setMouseLeftDragMode(MouseLeftDragModeEnum::REGION_SELECTION);
        }
        else {
            m_browserTabContent->setMouseLeftDragMode(MouseLeftDragModeEnum::DEFAULT);
        }
    }
    updateRegionSelectionAction();
}

/**
 * @return Undo stack for this tab or NULL if not valid
 */
CaretUndoStack*
BrainBrowserWindowToolBarOrientation::getUndoStack()
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

