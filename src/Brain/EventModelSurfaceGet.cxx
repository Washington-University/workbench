/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include "CaretAssert.h"
#include "EventModelSurfaceGet.h"

using namespace caret;

/**
 * Constructor.
 */
EventModelSurfaceGet::EventModelSurfaceGet(const Surface* surface)
: Event(EventTypeEnum::EVENT_MODEL_SURFACE_GET),
  surface(surface)
{
    CaretAssert(surface);
}

/**
 * Destructor.
 */
EventModelSurfaceGet::~EventModelSurfaceGet()
{
    
}

/**
 * @return The model surface that was found.
 */
ModelSurface* 
EventModelSurfaceGet::getModelSurface()
{
    return m_modelSurface;
}

/**
 * Set the model surface.
 * @param modelSurface
 *    Model surface that matches the specified surface.
 */
void
EventModelSurfaceGet::setModelSurface(ModelSurface* modelSurface)
{
    m_modelSurface = modelSurface;
}

/**
 * @return Returns the surface for which the model surface is requested.
 */
const Surface* 
EventModelSurfaceGet::getSurface() const
{
    return this->surface;
}


