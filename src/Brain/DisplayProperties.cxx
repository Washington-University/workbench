
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

#define __DISPLAY_PROPERTIES_DECLARE__
#include "DisplayProperties.h"
#undef __DISPLAY_PROPERTIES_DECLARE__

#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class DisplayProperties 
 * \brief Base class for all display properties.
 *
 * Base class for all display properties.
 */
/**
 * Constructor.
 */
DisplayProperties::DisplayProperties()
: CaretObject()
{
    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * Destructor.
 */
DisplayProperties::~DisplayProperties()
{
    delete m_sceneAssistant;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DisplayProperties::toString() const
{
    return "DisplayProperties";
}
