
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_VIEW_DECLARE__
#include "BrainBrowserWindowToolBarView.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VIEW_DECLARE__

#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarView 
 * \brief Toolbar component for selection of the View Mode
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentObjectName
 *     Name of the parent object
 * @param parentToolBar
 *     The parent toolbar
 */
BrainBrowserWindowToolBarView::BrainBrowserWindowToolBarView(const QString& parentObjectName,
                                                             BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    const QString objectNamePrefix(parentObjectName
                                   + ":ViewMode");
    
    this->viewModeChartOneRadioButton = new QRadioButton("Chart Old");
    this->viewModeChartOneRadioButton->setToolTip("Show Old Chart View");
    this->viewModeChartOneRadioButton->setObjectName(objectNamePrefix
                                                     + ":ChartOld");
    macroManager->addMacroSupportToObject(this->viewModeChartOneRadioButton,
                                          "Select Chart Old View");
    
    this->viewModeChartTwoRadioButton = new QRadioButton("Chart");
    this->viewModeChartTwoRadioButton->setToolTip("Show Chart View");
    this->viewModeChartTwoRadioButton->setObjectName(objectNamePrefix
                                                     + ":Chart");
    macroManager->addMacroSupportToObject(this->viewModeChartTwoRadioButton,
                                          "Select Chart View");
    
    this->viewModeSurfaceRadioButton = new QRadioButton("Surface");
    this->viewModeSurfaceRadioButton->setToolTip("Show Surface View");
    this->viewModeSurfaceRadioButton->setObjectName(objectNamePrefix
                                                    + ":Surface");
    macroManager->addMacroSupportToObject(this->viewModeSurfaceRadioButton,
                                          "Select surface view");
    
    this->viewModeSurfaceMontageRadioButton = new QRadioButton("Montage");
    this->viewModeSurfaceMontageRadioButton->setToolTip("Show Montage View");
    this->viewModeSurfaceMontageRadioButton->setObjectName(objectNamePrefix
                                                           + ":Montage");
    macroManager->addMacroSupportToObject(this->viewModeSurfaceMontageRadioButton,
                                          "Select surface montage view");
    
    this->viewModeVolumeRadioButton = new QRadioButton("Volume");
    this->viewModeVolumeRadioButton->setToolTip("Show Volume View");
    this->viewModeVolumeRadioButton->setObjectName(objectNamePrefix
                                                   + ":Volume");
    macroManager->addMacroSupportToObject(this->viewModeVolumeRadioButton,
                                          "Select volume view");
    
    this->viewModeWholeBrainRadioButton = new QRadioButton("All");
    this->viewModeWholeBrainRadioButton->setToolTip("Show All View");
    this->viewModeWholeBrainRadioButton->setObjectName(objectNamePrefix
                                                       + ":All");
    macroManager->addMacroSupportToObject(this->viewModeWholeBrainRadioButton,
                                          "Select all view");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
#ifdef CARET_OS_MACOSX
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 2);
#else
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 2);
#endif
    layout->addWidget(this->viewModeSurfaceMontageRadioButton);
    layout->addWidget(this->viewModeVolumeRadioButton);
    layout->addWidget(this->viewModeWholeBrainRadioButton);
    layout->addWidget(this->viewModeChartTwoRadioButton);
    layout->addWidget(this->viewModeSurfaceRadioButton);
    layout->addWidget(this->viewModeChartOneRadioButton);
    
    QButtonGroup* viewModeRadioButtonGroup = new QButtonGroup(this);
    viewModeRadioButtonGroup->addButton(this->viewModeChartOneRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeChartTwoRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeSurfaceRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeSurfaceMontageRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeVolumeRadioButton);
    viewModeRadioButtonGroup->addButton(this->viewModeWholeBrainRadioButton);
    QObject::connect(viewModeRadioButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
                     this, SLOT(viewModeRadioButtonClicked(QAbstractButton*)));
    
    addToWidgetGroup(this->viewModeChartOneRadioButton);
    addToWidgetGroup(this->viewModeChartTwoRadioButton);
    addToWidgetGroup(this->viewModeSurfaceRadioButton);
    addToWidgetGroup(this->viewModeSurfaceMontageRadioButton);
    addToWidgetGroup(this->viewModeVolumeRadioButton);
    addToWidgetGroup(this->viewModeWholeBrainRadioButton);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarView::~BrainBrowserWindowToolBarView()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarView::updateContent(BrowserTabContent* browserTabContent)
{
    ModelTypeEnum::Enum modelType = ModelTypeEnum::MODEL_TYPE_INVALID;
    if (browserTabContent != NULL) {
         modelType = browserTabContent->getSelectedModelType();
    }
    
    blockAllSignals(true);
    
    /*
     * Enable buttons for valid types
     */
    if (browserTabContent != NULL) {
        this->viewModeSurfaceRadioButton->setEnabled(browserTabContent->isSurfaceModelValid());
        this->viewModeSurfaceMontageRadioButton->setEnabled(browserTabContent->isSurfaceMontageModelValid());
        this->viewModeVolumeRadioButton->setEnabled(browserTabContent->isVolumeSliceModelValid());
        this->viewModeWholeBrainRadioButton->setEnabled(browserTabContent->isWholeBrainModelValid());
        this->viewModeChartOneRadioButton->setEnabled(browserTabContent->isChartOneModelValid());
        this->viewModeChartTwoRadioButton->setEnabled(browserTabContent->isChartTwoModelValid());
    }
    else {
        this->viewModeSurfaceRadioButton->setEnabled(false);
        this->viewModeSurfaceMontageRadioButton->setEnabled(false);
        this->viewModeVolumeRadioButton->setEnabled(false);
        this->viewModeWholeBrainRadioButton->setEnabled(false);
        this->viewModeChartOneRadioButton->setEnabled(false);
        this->viewModeChartTwoRadioButton->setEnabled(false);
    }
    
    switch (modelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            this->viewModeSurfaceRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            this->viewModeSurfaceMontageRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            this->viewModeVolumeRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            this->viewModeWholeBrainRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART:
            this->viewModeChartOneRadioButton->setChecked(true);
            break;
        case ModelTypeEnum::MODEL_TYPE_CHART_TWO:
            this->viewModeChartTwoRadioButton->setChecked(true);
            break;
    }
    
    blockAllSignals(false);
}

/**
 * Called when a view mode is selected.
 */
void
BrainBrowserWindowToolBarView::viewModeRadioButtonClicked(QAbstractButton*)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    if (btc == NULL) {
        return;
    }
    
    if (this->viewModeChartOneRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_CHART);
    }
    else if (this->viewModeChartTwoRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_CHART_TWO);
    }
    else if (this->viewModeSurfaceRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_SURFACE);
    }
    else if (this->viewModeSurfaceMontageRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE);
    }
    else if (this->viewModeVolumeRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES);
    }
    else if (this->viewModeWholeBrainRadioButton->isChecked()) {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN);
    }
    else {
        btc->setSelectedModelType(ModelTypeEnum::MODEL_TYPE_INVALID);
    }
    
    getParentToolBar()->updateToolBar();
    getParentToolBar()->updateTabName(-1);
    getParentToolBar()->updateToolBox();
    getParentToolBar()->emitViewModelChangedSignal();
    getParentToolBar()->updateGraphicsWindow();
}

