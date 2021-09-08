
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

#define __PIXEL_INDEX_DECLARE__
#include "PixelIndex.h"
#undef __PIXEL_INDEX_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::PixelIndex 
 * \brief A pixel index
 * \ingroup Files
 */

/**
 * Constructor.
 */
PixelIndex::PixelIndex()
: CaretObject()
{
    initializeMembers();
    m_pixelIndices.m_ijk[0] = 0;
    m_pixelIndices.m_ijk[1] = 0;
    m_pixelIndices.m_ijk[2] = 0;
}

/**
 * Constructor with pixel indices as floats
 * @param ijk
 *    Index i, J, K
 */
PixelIndex::PixelIndex(const float ijk[3])
: PixelIndex(ijk[0], ijk[1], ijk[2])
{
}

/**
 * Constructor with pixel indices as floats
 * @param i
 *    Index i
 * @param j
 *    Index j
 * @param k
 *    Index k
 */
PixelIndex::PixelIndex(const float i,
                       const float j,
                       const float k)
: PixelIndex(static_cast<int64_t>(i),
             static_cast<int64_t>(j),
             static_cast<int64_t>(k))
{
}


/**
 * Constructor with pixel indices
 * @param i
 *    Index i
 * @param j
 *    Index j
 * @param k
 *    Index k
 */
PixelIndex::PixelIndex(const int64_t i,
                       const int64_t j,
                       const int64_t k)
: CaretObject()
{
    initializeMembers();
    m_pixelIndices.m_ijk[0] = i;
    m_pixelIndices.m_ijk[1] = j;
    m_pixelIndices.m_ijk[2] = k;
}

///**
// * Constructor from a pixel coordinate
// * @param coordinate
// *    The pixel coordinate
// */
//PixelIndex::PixelIndex(const PixelCoordinate& coordinate)
//: CaretObject()
//{
//    initializeMembers();
//}

/**
 * Destructor.
 */
PixelIndex::~PixelIndex()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
PixelIndex::PixelIndex(const PixelIndex& obj)
: CaretObject(obj)
{
    initializeMembers();
    
    this->copyHelperPixelIndex(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
PixelIndex&
PixelIndex::operator=(const PixelIndex& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperPixelIndex(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
PixelIndex::copyHelperPixelIndex(const PixelIndex& obj)
{
    m_pixelIndices = obj.m_pixelIndices;
}

void
PixelIndex::initializeMembers()
{
    m_sceneAssistant.reset(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_ijk", m_pixelIndices.m_ijk, 3, 0);
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
PixelIndex::operator==(const PixelIndex& obj) const
{
    if (this == &obj) {
        return true;    
    }

    return (m_pixelIndices == obj.m_pixelIndices);
}

/**
 * @return True if the pixel index is valid (all IJK >= 0)
 */
bool
PixelIndex::isValid() const
{
    return ((getI() >= 0)
            && (getJ() >= 0)
            && (getK() >= 0));
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PixelIndex::toString() const
{
    return ("PixelIndex = ("
            + AString::fromNumbers(getIJK(), 3, ", ")
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
PixelIndex::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "PixelIndex",
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
PixelIndex::restoreFromScene(const SceneAttributes* sceneAttributes,
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

