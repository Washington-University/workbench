
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __PIXEL_COORDINATE_DECLARE__
#include "PixelCoordinate.h"
#undef __PIXEL_COORDINATE_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::PixelCoordinate 
 * \brief Pixel coordinate
 * \ingroup Files
 */

/**
 * Constructor.
 */
PixelCoordinate::PixelCoordinate()
: CaretObject(),
SceneableInterface()
{
    initializeMembers();
}

/**
 * Constructor.
 * @param xyz
 *    Coordinates in std::array
 */
PixelCoordinate::PixelCoordinate(const std::array<float,3>& xyz)
: CaretObject(),
SceneableInterface()
{
    initializeMembers();
    m_pixelCoord[0] = xyz[0];
    m_pixelCoord[1] = xyz[1];
    m_pixelCoord[2] = xyz[2];
}

/**
 * Constructor.
 * @param x
 *    X-coordinate
 * @param y
 *    Y-coordinate
 * @param z
 *    Z-coordinate
 */
PixelCoordinate::PixelCoordinate(const float x,
                                 const float y,
                                 const float z)
: CaretObject(),
SceneableInterface()
{
    initializeMembers();
    m_pixelCoord[0] = x;
    m_pixelCoord[1] = y;
    m_pixelCoord[2] = z;
}

/**
 * Initialize members of this instance
 */
void
PixelCoordinate::initializeMembers()
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_pixelCoord", m_pixelCoord, 3, 0.0);
}

/**
 * Destructor.
 */
PixelCoordinate::~PixelCoordinate()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
PixelCoordinate::PixelCoordinate(const PixelCoordinate& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    initializeMembers();
    this->copyHelperPixelCoordinate(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
PixelCoordinate&
PixelCoordinate::operator=(const PixelCoordinate& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperPixelCoordinate(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
PixelCoordinate::copyHelperPixelCoordinate(const PixelCoordinate& obj)
{
    m_pixelCoord = obj.m_pixelCoord;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
PixelCoordinate::operator==(const PixelCoordinate& obj) const
{
    if (this == &obj) {
        return true;    
    }

    /* perform equality testing HERE and return true if equal ! */

    return false;    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PixelCoordinate::toString() const
{
    return ("PixelCoordinate = ("
            + AString::fromNumbers(m_pixelCoord, 3, ", ")
            + ")");
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
PixelCoordinate::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "PixelCoordinate",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
PixelCoordinate::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

