
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __EXAMPLE_SCENE_INFO_DECLARE__
#include "ExampleSceneInfo.h"
#undef __EXAMPLE_SCENE_INFO_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ExampleSceneInfo 
 * \brief Contains info about examples scenes
 * \ingroup Common
 */

/**
 * Constructor.
 * @param filename
 *    Name of file containing the scene
 * @param name
 *    Name of scene
 * @param description
 *    Description of scene
 */
ExampleSceneInfo::ExampleSceneInfo(const AString& filename,
                                   const AString& name,
                                   const AString& description)
: CaretObject(),
m_filename(filename),
m_name(name),
m_description(description)
{
    
}

/**
 * Destructor.
 */
ExampleSceneInfo::~ExampleSceneInfo()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ExampleSceneInfo::ExampleSceneInfo(const ExampleSceneInfo& obj)
: CaretObject(obj)
{
    this->copyHelperExampleSceneInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ExampleSceneInfo&
ExampleSceneInfo::operator=(const ExampleSceneInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperExampleSceneInfo(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ExampleSceneInfo::copyHelperExampleSceneInfo(const ExampleSceneInfo& obj)
{
    m_filename    = obj.m_filename;
    m_name        = obj.m_name;
    m_description = obj.m_description;
}

/**
 * @return Name of scene file containing the scene
 */
AString
ExampleSceneInfo::getFilename() const
{
    return m_filename;
}

/**
 * @return Name of the scene
 */
AString
ExampleSceneInfo::getName() const
{
    return m_name;
}

/**
 * @return Description of scene
 */
AString
ExampleSceneInfo::getDescription() const
{
    return m_description;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ExampleSceneInfo::toString() const
{
    return "ExampleSceneInfo";
}

