
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

#define __VIEWING_TRANSFORMATIONS_CEREBELLUM_DECLARE__
#include "ViewingTransformationsCerebellum.h"
#undef __VIEWING_TRANSFORMATIONS_CEREBELLUM_DECLARE__

#include "CaretAssert.h"
using namespace caret;

/**
 * \class caret::ViewingTransformationsCerebellum 
 * \brief Viewing transformations (pan/rotate/zoom) for cerebellum.
 * \ingroup Brain
 *
 * Extends ViewingTransformations with differences for cerebellum viewing.
 */

/**
 * Constructor.
 */
ViewingTransformationsCerebellum::ViewingTransformationsCerebellum()
: ViewingTransformations()
{
    
}

/**
 * Destructor.
 */
ViewingTransformationsCerebellum::~ViewingTransformationsCerebellum()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ViewingTransformationsCerebellum::ViewingTransformationsCerebellum(const ViewingTransformationsCerebellum& obj)
: ViewingTransformations(obj)
{
    this->copyHelperViewingTransformationsCerebellum(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ViewingTransformationsCerebellum&
ViewingTransformationsCerebellum::operator=(const ViewingTransformationsCerebellum& obj)
{
    if (this != &obj) {
        ViewingTransformations::operator=(obj);
        this->copyHelperViewingTransformationsCerebellum(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ViewingTransformationsCerebellum::copyHelperViewingTransformationsCerebellum(const ViewingTransformationsCerebellum& /*obj*/)
{
    /* nothing to copy */
}

/**
 * Reset the view to the default view.
 */
void
ViewingTransformationsCerebellum::resetView()
{
    ViewingTransformations::resetView();
    dorsalView();
}

/**
 * set to a dorsal view.
 */
void
ViewingTransformationsCerebellum::dorsalView()
{
    m_rotationMatrix->setRotation(0.0, 0.0, 0.0);
}

/**
 * set to a ventral view.
 */
void
ViewingTransformationsCerebellum::ventralView()
{
    m_rotationMatrix->setRotation(0.0, 180.0, 180.0);
}

