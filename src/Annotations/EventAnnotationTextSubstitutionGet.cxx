
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_DECLARE__
#include "EventAnnotationTextSubstitutionGet.h"
#undef __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationTextSubstitutionGet 
 * \brief Get the substitution text for a text annotation.
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
EventAnnotationTextSubstitutionGet::EventAnnotationTextSubstitutionGet()
: Event(EventTypeEnum::EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET)
{
    
}

/**
 * Destructor.
 */
EventAnnotationTextSubstitutionGet::~EventAnnotationTextSubstitutionGet()
{
}

/**
 * Add a substitution ID for gettting a substitution
 * @param substitutionID
 *    Substitution with file and column IDs
 */
void
EventAnnotationTextSubstitutionGet::addSubstitutionID(const AnnotationTextSubstitution& substitutionID)
{
    m_substitutionIDs.push_back(substitutionID);
    m_substitutionTexts.resize(m_substitutionIDs.size());
}

/**
 * @return The number of substitution idenfiers
 */
int32_t
EventAnnotationTextSubstitutionGet::getNumberOfSubstitutionIDs() const
{
    return m_substitutionIDs.size();
}

/**
 * @return The substitution identifier at the given index
 * @param index
 *    Index of the item
 */
const AnnotationTextSubstitution&
EventAnnotationTextSubstitutionGet::getSubstitutionID(const int32_t index) const
{
    CaretAssertVectorIndex(m_substitutionIDs, index);
    return m_substitutionIDs[index];
}

/**
 * Set the text value for the substitution ID at the give index
 * @param index
 *    Index of the item
 * @param textValue
 *    Text that is substituted into an annotation
 */
void
EventAnnotationTextSubstitutionGet::setSubstitutionTextValue(const int32_t index,
                                                             const AString textValue)
{
    CaretAssertVectorIndex(m_substitutionTexts, index);
    m_substitutionTexts[index] = textValue;
}

/**
 * @return The substitution text for the substitution ID at the given index
 * @param index
 *    Index of the substitution ID
 */
AString
EventAnnotationTextSubstitutionGet::getSubstitutionTextValue(const int32_t index) const
{
    AString textOut;
    CaretAssertVectorIndex(m_substitutionTexts, index);
    textOut = m_substitutionTexts[index];
    
    return textOut;
}
