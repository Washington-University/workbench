
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
 * @return All substitution names for which values are requested.
 * This method is called by AnnotationTextSubstitutionFile's so 
 * that the file may provide substitution values.
 */
const std::vector<AString>&
EventAnnotationTextSubstitutionGet::getSubstitutionNames() const
{
    return m_substitutionNames;
}

/**
 * Get the substitution value for the given substitution name.
 * This method is called by the annotation drawing code to get
 * annotation text substitution values.
 *
 * @param substitutionValue
 *     The substitutionValue.
 */
AString
EventAnnotationTextSubstitutionGet::getSubstitutionValueForName(const AString& substitutionName) const
{
    AString value;
    
    const auto iter = m_substitutionNamesAndValues.find(substitutionName);
    if (iter != m_substitutionNamesAndValues.end()) {
        value = iter->second;
    }
    
    return value;
}

/**
 * Set the substitution value.
 * This method is called by AnnotationTextSubstitutionFile's so
 * that the file may provide substitution values.
 *
 * @param substitutionName
 *     The substitution name.
 * @param substitutionValue
 *     The substitution value.
 */
void
EventAnnotationTextSubstitutionGet::setSubstitutionValueForName(const AString& substitutionName,
                                                                const AString& substitutionValue)
{
    auto iter = m_substitutionNamesAndValues.find(substitutionName);
    if (iter != m_substitutionNamesAndValues.end()) {
        iter->second = substitutionValue;
    }
}

/**
 * Add a substitution name for which a substitution value is sought.
 * This method is called by the annotation drawing code to get
 * annotation text substitution values.
 *
 * @param substitutionName
 *     Name of the annotation substitution.
 */
void
EventAnnotationTextSubstitutionGet::addSubstitutionName(const AString& substitutionName)
{
    m_substitutionNames.push_back(substitutionName);
    m_substitutionNamesAndValues.insert(std::make_pair(substitutionName, ""));
}
