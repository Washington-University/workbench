
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_DECLARE__
#include "BrainBrowserWindowToolBarSurface.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_DECLARE__

#include <QLabel>
#include <QVBoxLayout>

#include "BrainBrowserWindowToolBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "ModelSurfaceSelector.h"
#include "StructureSurfaceSelectionControl.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarSurface 
 * \brief Toolbar component for Surface View mode surface selection
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parentObjectName
 *     Name of the parent object
 * @param parentToolBar
 *     The parent toolbar
 */
BrainBrowserWindowToolBarSurface::BrainBrowserWindowToolBarSurface(const QString& parentObjectName,
                                                                   BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    QLabel* structureSurfaceLabel = new QLabel("Brain Structure and Surface: ");
    
    /*
     * Note: Macro support is in StructureSurfaceSelectionControl
     */
    this->surfaceSurfaceSelectionControl = new StructureSurfaceSelectionControl(false,
                                                                                parentObjectName
                                                                                + ":Surface",
                                                                                "surface view",
                                                                                this);
    QObject::connect(this->surfaceSurfaceSelectionControl,
                     SIGNAL(selectionChanged(const StructureEnum::Enum,
                                             ModelSurface*)),
                     this,
                     SLOT(surfaceSelectionControlChanged(const StructureEnum::Enum,
                                                         ModelSurface*)));
    
    this->surfaceSurfaceSelectionControl->setMinimumWidth(150);
    this->surfaceSurfaceSelectionControl->setMaximumWidth(1200);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(structureSurfaceLabel);
    layout->addWidget(this->surfaceSurfaceSelectionControl);
    layout->addStretch();
    
    addToWidgetGroup(this->surfaceSurfaceSelectionControl);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarSurface::~BrainBrowserWindowToolBarSurface()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarSurface::updateContent(BrowserTabContent* browserTabContent)
{
    blockAllSignals(true);
    this->surfaceSurfaceSelectionControl->updateControl(browserTabContent->getSurfaceModelSelector());
    blockAllSignals(false);
}

/**
 * Called when a single surface control is changed.
 * @param structure
 *      Structure that is selected.
 * @param surfaceModel
 *     Model that is selected.
 */
void
BrainBrowserWindowToolBarSurface::surfaceSelectionControlChanged(
                                                          const StructureEnum::Enum structure,
                                                          ModelSurface* surfaceModel)
{
    if (surfaceModel != NULL) {
        BrowserTabContent* btc = this->getTabContentFromSelectedTab();
        ModelSurfaceSelector* surfaceModelSelector = btc->getSurfaceModelSelector();
        surfaceModelSelector->setSelectedStructure(structure);
        surfaceModelSelector->setSelectedSurfaceModel(surfaceModel);
        
        invalidateColoringAndUpdateGraphicsWindow();
        updateUserInterface();
    }
    
    getParentToolBar()->updateTabName(-1);
}

