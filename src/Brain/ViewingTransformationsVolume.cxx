
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

#define __VIEWING_TRANSFORMATIONS_VOLUME_DECLARE__
#include "ViewingTransformationsVolume.h"
#undef __VIEWING_TRANSFORMATIONS_VOLUME_DECLARE__

#include "CaretAssert.h"
using namespace caret;

/**
 * \class caret::ViewingTransformationsVolume
 * \brief Viewing transformations (pan/rotate/zoom) for volume.
 * \ingroup Brain
 *
 * Extends ViewingTransformations with differences for volume viewing.
 */

/**
 * Constructor.
 */
ViewingTransformationsVolume::ViewingTransformationsVolume()
: ViewingTransformations()
{
    
}

/**
 * Destructor.
 */
ViewingTransformationsVolume::~ViewingTransformationsVolume()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ViewingTransformationsVolume::ViewingTransformationsVolume(const ViewingTransformationsVolume& obj)
: ViewingTransformations(obj)
{
    this->copyHelperViewingTransformationsVolume(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ViewingTransformationsVolume&
ViewingTransformationsVolume::operator=(const ViewingTransformationsVolume& obj)
{
    if (this != &obj) {
        ViewingTransformations::operator=(obj);
        this->copyHelperViewingTransformationsVolume(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ViewingTransformationsVolume::copyHelperViewingTransformationsVolume(const ViewingTransformationsVolume& /*obj*/)
{
    
}

/**
 * Reset the view to the default view for a VOLUME.
 */
void
ViewingTransformationsVolume::resetView()
{
    ViewingTransformations::resetView();
    m_rotationMatrix->identity();
}

/**
 * Set the view of the orthogonal volume to the given bounds
 * @param transform
 *    The object to window transform
 * @param selectionBounds
 *    The selection bounds
 * @param browserTabContent
 *    The content of the browser tab
 */
bool
ViewingTransformationsVolume::setOrthogonalViewToBounds(const GraphicsObjectToWindowTransform* transform,
                                                        const GraphicsRegionSelectionBox* selectionBounds,
                                                        const BrowserTabContent* browserTabContent)
{
    std::cout << "setOrthogonalViewToBounds called" << std::endl;
    return false;
}

