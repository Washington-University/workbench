
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define ____NEUROGLANCER_ANNOTATION__DECLARE__
#include "NeuroglancerAnnotation.h"
#undef ____NEUROGLANCER_ANNOTATION__DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
using namespace caret;



/**
 * \class caret::NeuroglancerAnnotation
 * \brief Class for an annotation that could be point, line, area, etc
 * \ingroup Files
 */

/**
 * Constructor.
 * @param annotationType
 *    The type of the annotation
 * @param fileName
 *    Name of file (no path) from which annotation was read
 * @param xyz
 *    XYZ(s) for the annotation
 * @param color
 *    Color of the annotation
 */
NeuroglancerAnnotation::NeuroglancerAnnotation(const NeuroglancerAnnotationTypeEnum::Enum annotationType,
                                               const AString& fileName,
                                               const std::vector<Vector3D>& xyz,
                                               const CaretColor& color)
: CaretObject(),
m_annotationType(annotationType),
m_fileName(fileName),
m_xyz(xyz),
m_color(color)
{
    switch (m_annotationType) {
        case NeuroglancerAnnotationTypeEnum::INVALID:
            break;
        case NeuroglancerAnnotationTypeEnum::AXIS_ALIGNED_BOUNDING_BOX:
            CaretAssert(m_xyz.size() == 2);
            break;
        case NeuroglancerAnnotationTypeEnum::ELLIPSOID:
            CaretAssert(m_xyz.size() == 1);
            break;
        case NeuroglancerAnnotationTypeEnum::LINE:
            CaretAssert(m_xyz.size() >= 2);
            break;
        case NeuroglancerAnnotationTypeEnum::POINT:
            CaretAssert(m_xyz.size() == 1);
            break;
        case NeuroglancerAnnotationTypeEnum::POLYLINE:
            CaretAssert(m_xyz.size() >= 2);
            break;
    }
}

/**
 * Destructor.
 */
NeuroglancerAnnotation::~NeuroglancerAnnotation()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
NeuroglancerAnnotation::NeuroglancerAnnotation(const NeuroglancerAnnotation& obj)
: CaretObject(obj)
{
    this->copyHelperNeuroglancerAnnotation(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
NeuroglancerAnnotation&
NeuroglancerAnnotation::operator=(const NeuroglancerAnnotation& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperNeuroglancerAnnotation(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
NeuroglancerAnnotation::copyHelperNeuroglancerAnnotation(const NeuroglancerAnnotation& obj)
{
    m_annotationType = obj.m_annotationType;
    m_fileName    = obj.m_fileName;
    m_xyz         = obj.m_xyz;
    m_color       = obj.m_color;
}

/**
 * @return The annotation type
 */
NeuroglancerAnnotationTypeEnum::Enum
NeuroglancerAnnotation::getType() const
{
    return m_annotationType;
}

/**
 * @return Name of file from which annotation was read
 */
AString
NeuroglancerAnnotation::getFileName() const
{
    return m_fileName;
}

/**
 * @return Number of XYZ in the annotation
 */
int32_t
NeuroglancerAnnotation::getNumberOfXYZ() const
{
    return m_xyz.size();
}

/**
 * @return The color
 */
const CaretColor&
NeuroglancerAnnotation::getColor() const
{
    return m_color;
}

/**
 * @return XYZ at the given index
 * @param index
 *    The index
 */
const Vector3D&
NeuroglancerAnnotation::getXYZ(const int32_t index) const
{
    CaretAssertVectorIndex(m_xyz, index);
    return m_xyz[index];
}

/**
 * @return The size of the annotation
 */
float
NeuroglancerAnnotation::getSize() const
{
    return 1.0;
}

/**
 * @return The name of the type
 */
AString
NeuroglancerAnnotation::getTypeName() const
{
    return NeuroglancerAnnotationTypeEnum::toGuiName(m_annotationType);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
NeuroglancerAnnotation::toString() const
{
    AString xyzString;
    for (int32_t i = 0; i < getNumberOfXYZ(); i++) {
        if (i > 0) {
            xyzString += ", ";
        }
        xyzString += "(" + AString::fromNumbers(m_xyz[i]) + ")";
    }
    AString txt("type=" + getTypeName()
                + ", XYZ=" + xyzString
                + ", color=" + m_color.toString());
    return txt;
}


