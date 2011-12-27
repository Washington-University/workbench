
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __DISPLAY_CONTROL_VOLUME_SURFACE_OUTLINE_PAGE_DECLARE__
#include "DisplayControlVolumeSurfaceOutlinePage.h"
#undef __DISPLAY_CONTROL_VOLUME_SURFACE_OUTLINE_PAGE_DECLARE__

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "Brain.h"
#include "BrainOpenGL.h"
#include "CaretColorEnumSelectionControl.h"
#include "DisplayPropertiesVolume.h"
#include "GuiManager.h"
#include "SurfaceSelectionControl.h"
#include "VolumeSurfaceOutlineSelection.h"

using namespace caret;

    
/**
 * \class DisplayControlVolumeSurfaceOutlinePage 
 * \brief Page for control of volume surface outlines.
 *
 * Page for control of volume surface outlines.
 */

/**
 * Constructor.
 */
DisplayControlVolumeSurfaceOutlinePage::DisplayControlVolumeSurfaceOutlinePage()
: MultiPageDialogPage("Volume Surface Outline")
{
}

/**
 * Destructor.
 */
DisplayControlVolumeSurfaceOutlinePage::~DisplayControlVolumeSurfaceOutlinePage()
{
    
}

/**
 * Is the page valid?
 */
bool 
DisplayControlVolumeSurfaceOutlinePage::isPageValid()
{
    return true;
}

/**
 * Apply the pages content.
 */
void 
DisplayControlVolumeSurfaceOutlinePage::applyPageContent()
{
    const int32_t numOutlines = static_cast<int32_t>(this->outlineWidgets.size());
    for (int32_t i = 0; i < numOutlines; i++) {
        this->outlineWidgets[i]->applyWidget();
    }
    
    this->updateAllGraphics();
}

/**
 * Will be called to request that the page be constructed.
 */
QWidget* 
DisplayControlVolumeSurfaceOutlinePage::createPageContent()
{
    int32_t columnCounter = 0;
    const int32_t COLUMN_SHOW      = columnCounter++;
    const int32_t COLUMN_COLOR     = columnCounter++;
    const int32_t COLUMN_THICKNESS = columnCounter++;
    const int32_t COLUMN_SURFACE   = columnCounter++;
    
    QLabel* columnShowLabel      = new QLabel("Show");
    QLabel* columnColorLabel     = new QLabel("Color");
    QLabel* columnThicknessLabel = new QLabel("Thickness");
    QLabel* columnSurfaceLabel   = new QLabel("Surface");
    
    QWidget* w = new QWidget();
    QGridLayout* layout = new QGridLayout(w);
    layout->setColumnStretch(COLUMN_SHOW, 0);
    layout->setColumnStretch(COLUMN_COLOR, 0);
    layout->setColumnStretch(COLUMN_THICKNESS, 0);
    layout->setColumnStretch(COLUMN_SURFACE, 100);
    
    layout->addWidget(columnShowLabel, 0, COLUMN_SHOW);
    layout->addWidget(columnColorLabel, 0, COLUMN_COLOR);
    layout->addWidget(columnThicknessLabel, 0, COLUMN_THICKNESS);
    layout->addWidget(columnSurfaceLabel, 0, COLUMN_SURFACE);
    
    for (int32_t i = 0; i < DisplayPropertiesVolume::MAXIMUM_NUMBER_OF_SURFACE_OUTLINES; i++) {
        OutlineWidget* outlineWidget = new OutlineWidget(this,
                                                         i);
        this->outlineWidgets.push_back(outlineWidget);

        const int32_t row = layout->rowCount();
        layout->addWidget(outlineWidget->selectionCheckBox, row, COLUMN_SHOW);
        layout->addWidget(outlineWidget->colorSelectionControl->getWidget(), row, COLUMN_COLOR);
        layout->addWidget(outlineWidget->thicknessSpinBox, row, COLUMN_THICKNESS);
        layout->addWidget(outlineWidget->surfaceSelectionControl->getWidget(), row, COLUMN_SURFACE);
        
    }
    
    return w;
}

/**
 * Update the contents of the page.
 */
void 
DisplayControlVolumeSurfaceOutlinePage::updatePageContent()
{
    const int32_t numOutlines = static_cast<int32_t>(this->outlineWidgets.size());
    for (int32_t i = 0; i < numOutlines; i++) {
        this->outlineWidgets[i]->updateWidget();
    }
}

//===================================================================================

/**
 * Constructor.
 * @param parentPage
 *    Parent page to which signals are connected.
 * @param indx
 *    Index for volume surface outline in this widget.
 */
OutlineWidget::OutlineWidget(DisplayControlVolumeSurfaceOutlinePage* parentPage,
                             const int32_t outlineIndex)
: CaretObject()
{
    this->outlineIndex = outlineIndex;
    
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesVolume* dpv = brain->getDisplayPropertiesVolume();
    VolumeSurfaceOutlineSelection* vsos = dpv->getSurfaceOutlineSelection(this->outlineIndex);
    
    
    this->selectionCheckBox = new QCheckBox(" ");
    parentPage->addWidget(this->selectionCheckBox, 
                          true);
    
    this->colorSelectionControl = new CaretColorEnumSelectionControl(true);
    parentPage->addWidget(this->colorSelectionControl, 
                          true);
    
    float minLineWidth, maxLineWidth;
    BrainOpenGL::getMinMaxLineWidth(minLineWidth, 
                                    maxLineWidth);
    this->thicknessSpinBox = new QDoubleSpinBox();
    this->thicknessSpinBox->setRange(minLineWidth, 
                                     maxLineWidth);
    this->thicknessSpinBox->setSingleStep(1.0);
    parentPage->addWidget(this->thicknessSpinBox, 
                          true);
    
    this->surfaceSelectionControl = new SurfaceSelectionControl(vsos->getSurfaceSelection());
    parentPage->addWidget(this->surfaceSelectionControl, 
                          true);
    
}

/**
 * Destructor.
 */
OutlineWidget::~OutlineWidget()
{
    
}

/**
 * Apply contents of widget.
 */
void 
OutlineWidget::applyWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesVolume* dpv = brain->getDisplayPropertiesVolume();
    VolumeSurfaceOutlineSelection* vsos = dpv->getSurfaceOutlineSelection(this->outlineIndex);
    
    vsos->setDisplayed(this->selectionCheckBox->isChecked());
    vsos->setColor(this->colorSelectionControl->getSelectedColor());
    vsos->setThickness(this->thicknessSpinBox->value());
}

/**
 * Update the widget.
 */
void 
OutlineWidget::updateWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesVolume* dpv = brain->getDisplayPropertiesVolume();
    VolumeSurfaceOutlineSelection* vsos = dpv->getSurfaceOutlineSelection(this->outlineIndex);
    
    this->selectionCheckBox->setChecked(vsos->isDisplayed());
    this->colorSelectionControl->setSelectedColor(vsos->getColor());
    this->thicknessSpinBox->setValue(vsos->getThickness());
    this->surfaceSelectionControl->updateControl();
}
