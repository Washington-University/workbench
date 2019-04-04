
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

#define __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_VALUE_DECLARE__
#include "VolumeSurfaceOutlineModelCacheValue.h"
#undef __VOLUME_SURFACE_OUTLINE_MODEL_CACHE_VALUE_DECLARE__

#include "CaretAssert.h"
#include "GraphicsPrimitive.h"

using namespace caret;
    
/**
 * \class caret::VolumeSurfaceOutlineModelCacheValue 
 * \brief Data for a cached surface outline model
 * \ingroup Brain
 */

/**
 * Constructor.
 */
VolumeSurfaceOutlineModelCacheValue::VolumeSurfaceOutlineModelCacheValue()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineModelCacheValue::~VolumeSurfaceOutlineModelCacheValue()
{
    deletePrimitives();
}

/**
 * Delete the primitives.
 */
void
VolumeSurfaceOutlineModelCacheValue::deletePrimitives()
{
    for (auto p : m_contourLinePrimitives) {
        delete p;
    }
    m_contourLinePrimitives.clear();
}

/**
 * @return Reference to the graphics primitives;
 */
std::vector<GraphicsPrimitive*>
VolumeSurfaceOutlineModelCacheValue::getGraphicsPrimitives() const
{
    return m_contourLinePrimitives;
}

/**
 * Set the primitives in this cache.  Any current primitives are replaced and destroyed
 */
void
VolumeSurfaceOutlineModelCacheValue::setGraphicsPrimitive(const std::vector<GraphicsPrimitive*>& primitives)
{
    deletePrimitives();
    
    m_contourLinePrimitives = primitives;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSurfaceOutlineModelCacheValue::toString() const
{
    return "VolumeSurfaceOutlineModelCacheValue";
}

