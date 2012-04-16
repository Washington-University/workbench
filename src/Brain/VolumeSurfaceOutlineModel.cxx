
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

#define __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__
#include "VolumeSurfaceOutlineModel.h"
#undef __VOLUME_SURFACE_OUTLINE_MODEL_DECLARE__

#include "SurfaceSelectionModel.h"
#include "SurfaceTypeEnum.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"

using namespace caret;


    
/**
 * \class VolumeSurfaceOutlineSelection 
 * \brief Controls display of a volume surface outline.
 *
 * Controls display of a volume surface outline.
 */
/**
 * Constructor.
 */
VolumeSurfaceOutlineModel::VolumeSurfaceOutlineModel()
: CaretObject()
{
    this->displayed = false;
    this->thickness = 5.0;
    this->surfaceSelectionModel = new SurfaceSelectionModel(SurfaceTypeEnum::ANATOMICAL);
    this->colorOrTabModel = new VolumeSurfaceOutlineColorOrTabModel();
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineModel::~VolumeSurfaceOutlineModel()
{
    delete this->surfaceSelectionModel;
    delete this->colorOrTabModel;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSurfaceOutlineModel::toString() const
{
    return "VolumeSurfaceOutlineSelection";
}

/**
 * @return Is this surface outline displayed?
 */
bool 
VolumeSurfaceOutlineModel::isDisplayed() const
{
    return this->displayed;
}

/**
 * Set the display status of the surface outline.
 * @param displayed
 *    New display status.
 */
void 
VolumeSurfaceOutlineModel::setDisplayed(const bool displayed)
{
    this->displayed = displayed;
}

/**
 * @return Thickness for drawing surface.
 */
float 
VolumeSurfaceOutlineModel::getThickness() const
{
    return this->thickness;
}

/**
 * Set the thickness for drawing the surface.
 * @param thickness
 *    New value for thickness.
 */
void 
VolumeSurfaceOutlineModel::setThickness(const float thickness)
{
    this->thickness = thickness;
}

/**
 * @return  The surface selector used to select the surface.
 */
SurfaceSelectionModel* 
VolumeSurfaceOutlineModel::getSurfaceSelectionModel()
{
    return this->surfaceSelectionModel;
}

/**
 * @return  Get the selected surface.
 */
const Surface* 
VolumeSurfaceOutlineModel::getSurface() const
{
    return this->surfaceSelectionModel->getSurface();
}

/**
 * @return  Get the selected surface.
 */
Surface* 
VolumeSurfaceOutlineModel::getSurface()
{
    return this->surfaceSelectionModel->getSurface();
}

/**
 * @return The model for color or tab selection.
 */
VolumeSurfaceOutlineColorOrTabModel* 
VolumeSurfaceOutlineModel::getColorOrTabModel()
{
    return this->colorOrTabModel;
}

/**
 * @return The model for color or tab selection.
 */
const VolumeSurfaceOutlineColorOrTabModel* 
VolumeSurfaceOutlineModel::getColorOrTabModel() const
{
    return this->colorOrTabModel;
}


