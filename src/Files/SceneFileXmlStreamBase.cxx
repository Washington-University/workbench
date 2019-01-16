
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

#define __SCENE_FILE_XML_STREAM_BASE_DECLARE__
#include "SceneFileXmlStreamBase.h"
#undef __SCENE_FILE_XML_STREAM_BASE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SceneFileXmlStreamBase 
 * \brief Base class for Scene File XML Stream Reader and Writer
 * \ingroup Files
 */

/**
 * Constructor.
 */
SceneFileXmlStreamBase::SceneFileXmlStreamBase()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
SceneFileXmlStreamBase::~SceneFileXmlStreamBase()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SceneFileXmlStreamBase::toString() const
{
    return "SceneFileXmlStreamBase";
}

