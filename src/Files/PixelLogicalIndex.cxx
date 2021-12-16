
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
#include "PixelLogicalIndex.h"
#undef __PIXEL_INDEX_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::PixelLogicalIndex
 * \brief A logical pixel index
 * \ingroup Files
 *
 * A logical pixel index is used to index image data.  For all image data,
 * the origin is at the top-left corner.  For ordinary image files (JPG, PNG, etc)
 * the origin is (0, 0) with the "i" increasing left-to-right and "j" increasing
 * top to bottom.
 *
 * CZI files also have the origin at the top-left corner.  However, the origin
 * is typically non-zero values.
 */

/**
 * Constructor.
 */
PixelLogicalIndex::PixelLogicalIndex()
: CaretObject()
{
    initializeMembers();
    m_ijk[0] = 0;
    m_ijk[1] = 0;
    m_ijk[2] = 0;
}

/**
 * Constructor with pixel indices as floats
 * @param ijk
 *    Index i, J, K
 */
PixelLogicalIndex::PixelLogicalIndex(const float ijk[3])
: PixelLogicalIndex(ijk[0], ijk[1], ijk[2])
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
PixelLogicalIndex::PixelLogicalIndex(const float i,
                       const float j,
                       const float k)
: PixelLogicalIndex(static_cast<int64_t>(i),
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
PixelLogicalIndex::PixelLogicalIndex(const int32_t i,
                       const int32_t j,
                       const int32_t k)
: CaretObject()
{
    initializeMembers();
    m_ijk[0] = i;
    m_ijk[1] = j;
    m_ijk[2] = k;
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
PixelLogicalIndex::PixelLogicalIndex(const int64_t i,
                       const int64_t j,
                       const int64_t k)
: CaretObject()
{
    initializeMembers();
    m_ijk[0] = i;
    m_ijk[1] = j;
    m_ijk[2] = k;
}
///**
// * Constructor from a pixel coordinate
// * @param coordinate
// *    The pixel coordinate
// */
//PixelLogicalIndex::PixelLogicalIndex(const PixelCoordinate& coordinate)
//: CaretObject()
//{
//    initializeMembers();
//}

/**
 * Destructor.
 */
PixelLogicalIndex::~PixelLogicalIndex()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
PixelLogicalIndex::PixelLogicalIndex(const PixelLogicalIndex& obj)
: CaretObject(obj)
{
    initializeMembers();
    
    this->copyHelperPixelLogicalIndex(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
PixelLogicalIndex&
PixelLogicalIndex::operator=(const PixelLogicalIndex& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperPixelLogicalIndex(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
PixelLogicalIndex::copyHelperPixelLogicalIndex(const PixelLogicalIndex& obj)
{
    m_ijk = obj.m_ijk;
}

void
PixelLogicalIndex::initializeMembers()
{
    m_sceneAssistant.reset(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_ijk", &m_ijk[0], 3, 0);
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
PixelLogicalIndex::operator==(const PixelLogicalIndex& obj) const
{
    if (this == &obj) {
        return true;    
    }

    return (m_ijk == obj.m_ijk);
}

///**
// * @return True if the pixel index is valid (all IJK >= 0)
// */
//bool
//PixelLogicalIndex::isValid() const
//{
//    return ((getI() >= 0)
//            && (getJ() >= 0)
//            && (getK() >= 0));
//}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
PixelLogicalIndex::toString() const
{
    return ("PixelLogicalIndex = ("
            + AString::fromNumbers(m_ijk.data(), 3, ", ")
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
PixelLogicalIndex::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "PixelLogicalIndex",
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
PixelLogicalIndex::restoreFromScene(const SceneAttributes* sceneAttributes,
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

