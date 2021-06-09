
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

#define __DEFAULT_VIEW_TRANSFORM_DECLARE__
#include "DefaultViewTransform.h"
#undef __DEFAULT_VIEW_TRANSFORM_DECLARE__

#include "CaretAssert.h"
using namespace caret;



/**
 * \class caret::DefaultViewTransform
 * \brief Parameters for a default view so object is centered and fills viewport
 * \ingroup Common
 */

/**
 * Constructor.
 */
DefaultViewTransform::DefaultViewTransform()
: CaretObject()
{
    reset();
}

/**
 * Destructor.
 */
DefaultViewTransform::~DefaultViewTransform()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DefaultViewTransform::DefaultViewTransform(const DefaultViewTransform& obj)
: CaretObject(obj)
{
    this->copyHelperDefaultViewTransform(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
DefaultViewTransform&
DefaultViewTransform::operator=(const DefaultViewTransform& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperDefaultViewTransform(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
DefaultViewTransform::copyHelperDefaultViewTransform(const DefaultViewTransform& obj)
{
    m_scaling     = obj.m_scaling;
    m_translation = obj.m_translation;
}

/**
 * Reset to defaults.
 */
void
DefaultViewTransform::reset()
{
    m_scaling = 1.0;
    m_translation.fill(0.0);
}

/**
 * @return The scaling.
 */
float
DefaultViewTransform::getScaling() const
{
    return m_scaling;
}

/**
 * Set the scaling.
 * @param scaling
 *    New scaling value.
 */
void
DefaultViewTransform::setScaling(const float scaling)
{
    m_scaling = scaling;
}

/**
 * @return The translation
 */
std::array<float, 3>
DefaultViewTransform::getTranslation() const
{
    return m_translation;
}

/**
 * Set the translation.
 * @param X
 *    New X value.
 * @param Y
 *    New Y value.
 * @param Z
 *    New Z value.
 */
void
DefaultViewTransform::setTranslation(const float x,
                                     const float y,
                                     const float z)
{
    m_translation[0] = x;
    m_translation[1] = y;
    m_translation[2] = z;
}

/*
 * Set the Translation
 * @param translation
 *   New translation
 */
void
DefaultViewTransform::setTranslation(const std::array<float, 3>& translation)
{
    m_translation = translation;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
DefaultViewTransform::toString() const
{
    return ("Scaling = " + AString::number(m_scaling)
            + " Translation = " + AString::fromNumbers(m_translation.data(), 3, ", "));
}


