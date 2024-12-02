
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __ANNOTATION_TEXT_SUBSTITUTION_DECLARE__
#include "AnnotationTextSubstitution.h"
#undef __ANNOTATION_TEXT_SUBSTITUTION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationTextSubstitution 
 * \brief Identifies a text substitution in a text annotation
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param groupID
 *    The group identifier
 * @param columnID
 *    The column identifier
 * @param startIndex
 *    Start of the substitution in the text annotation
 * @param length
 *    Length of the substitution in the text annotation
 */
AnnotationTextSubstitution::AnnotationTextSubstitution(const AString& groupID,
                                                       const AString& columnID,
                                                       const int32_t startIndex,
                                                       const int32_t length)
: CaretObject(),
m_groupID(groupID),
m_columnID(columnID),
m_startIndex(startIndex),
m_length(length)
{
    
}

/**
 * Destructor.
 */
AnnotationTextSubstitution::~AnnotationTextSubstitution()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationTextSubstitution::AnnotationTextSubstitution(const AnnotationTextSubstitution& obj)
: CaretObject(obj)
{
    this->copyHelperAnnotationTextSubstitution(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationTextSubstitution&
AnnotationTextSubstitution::operator=(const AnnotationTextSubstitution& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationTextSubstitution(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationTextSubstitution::copyHelperAnnotationTextSubstitution(const AnnotationTextSubstitution& obj)
{
    m_groupID    = obj.m_groupID;
    m_columnID   = obj.m_columnID;
    m_startIndex = obj.m_startIndex;
    m_length     = obj.m_length;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
AnnotationTextSubstitution::operator==(const AnnotationTextSubstitution& obj) const
{
    if (this == &obj) {
        return true;    
    }

    /*
     * Equality is same group ID and column ID
     */
    if ((m_groupID == obj.m_groupID)
        && (m_columnID == obj.m_columnID)) {
        return true;
    }

    return false;    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationTextSubstitution::toString() const
{
    AString txt("GroupID=" + m_groupID
                + " ColumnID=" + m_columnID
                + " StartIndex=" + AString::number(m_startIndex)
                + " Length=" + AString::number(m_length));
    return txt;
}

