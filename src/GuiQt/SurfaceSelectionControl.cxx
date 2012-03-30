
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

#define __SURFACE_SELECTION_CONTROL_DECLARE__
#include "SurfaceSelectionControl.h"
#undef __SURFACE_SELECTION_CONTROL_DECLARE__

#include <QComboBox>

#include "Surface.h"
#include "SurfaceSelection.h"

using namespace caret;


    
/**
 * \class SurfaceSelectionControl 
 * \brief Control for selecting surfaces.
 *
 * Control for selecting surfaces.
 */
/**
 * Constructor.
 * @param parent
 *   The parent.
 * @param surfaceSelection
 *    Surface selection that is controlled through this control.
 */
SurfaceSelectionControl::SurfaceSelectionControl(QObject* parent,
                                                 SurfaceSelection* surfaceSelection)
: QObject(parent)
{
    this->initializeControl(surfaceSelection);
    this->thisInstanceOwnsSurfaceSelection = false;
}

/**
 * Constructor.
 * @param parent
 *   The parent.
 * @param brainStructure
 *   Allows selection of any surface with the specified brain structure.
 */
SurfaceSelectionControl::SurfaceSelectionControl(QObject* parent,
                                                 BrainStructure* brainStructure)
: QObject(parent)
{
    SurfaceSelection* ss = new SurfaceSelection(brainStructure);
    this->initializeControl(ss);
    this->thisInstanceOwnsSurfaceSelection = true;
}

/**
 * Help initialize an instance.
 */
void 
SurfaceSelectionControl::initializeControl(SurfaceSelection* surfaceSelection)
{
    this->surfaceComboBox = new QComboBox();
    QObject::connect(this->surfaceComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(comboBoxCurrentIndexChanged(int)));
    this->surfaceSelection = surfaceSelection;
}

/**
 * Destructor.
 */
SurfaceSelectionControl::~SurfaceSelectionControl()
{
    if (this->thisInstanceOwnsSurfaceSelection) {
        delete this->surfaceSelection;
    }
}

/**
 * Update the control.
 */
void 
SurfaceSelectionControl::updateControl()
{
    std::vector<Surface*> surfaces = this->surfaceSelection->getAvailableSurfaces();
    const Surface* selectedSurface = this->surfaceSelection->getSurface();
    
    this->surfaceComboBox->blockSignals(true);

    int32_t selectedIndex = 0;
    
    this->surfaceComboBox->clear();
    const int32_t numSurfaces = static_cast<int32_t>(surfaces.size());
    for (int32_t i = 0; i < numSurfaces; i++) {
        const int32_t indx = this->surfaceComboBox->count();
        this->surfaceComboBox->addItem(surfaces[i]->getFileNameNoPath());
        this->surfaceComboBox->setItemData(indx, qVariantFromValue((void*)surfaces[i]));
        
        if (surfaces[i] == selectedSurface) {
            selectedIndex = indx;
        }
    }
    
    if (numSurfaces > 0) {
        this->surfaceComboBox->setCurrentIndex(selectedIndex);
    }
    
    this->surfaceComboBox->blockSignals(false);
}

/**
 * @return the actual widget.
 */
QWidget* 
SurfaceSelectionControl::getWidget()
{
    return this->surfaceComboBox;
}

/**
 * @return the selected surface.
 */
Surface* 
SurfaceSelectionControl::getSurface()
{
    return this->surfaceSelection->getSurface();
}

/**
 * Set the selected surface.
 * @param surface
 *    The selected surface.
 */
void 
SurfaceSelectionControl::setSurface(Surface* surface)
{
    this->surfaceSelection->setSurface(surface);
    
    this->updateControl();
}

/**
 * Called when the item in the combo box is changed.
 * @param indx
 *    Index of item that was selected.
 */
void 
SurfaceSelectionControl::comboBoxCurrentIndexChanged(int indx)
{
    void* pointer = this->surfaceComboBox->itemData(indx).value<void*>();
    Surface* s = (Surface*)pointer;
    this->surfaceSelection->setSurface(s);
    
    emit surfaceSelected(s);
}
