
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

#define __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_ALL_GROUP_I_DS_DECLARE__
#include "EventAnnotationTextSubstitutionGetAllGroupIDs.h"
#undef __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_ALL_GROUP_I_DS_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationTextSubstitutionGetAllGroupIDs 
 * \brief Event to get all text substitutions group identifiers
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
EventAnnotationTextSubstitutionGetAllGroupIDs::EventAnnotationTextSubstitutionGetAllGroupIDs()
: Event(EventTypeEnum::EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_ALL_GROUP_IDS)
{
    
}

/**
 * Destructor.
 */
EventAnnotationTextSubstitutionGetAllGroupIDs::~EventAnnotationTextSubstitutionGetAllGroupIDs()
{
}

/**
 * Add a text substitution group identifier
 * @param groupID
 *    The group identifier
 */
void
EventAnnotationTextSubstitutionGetAllGroupIDs::addGroupID(const AString& groupID)
{
    if ( ! groupID.isEmpty()) {
        m_groupIDs.insert(groupID);
    }
}

/**
 * @return A Vector containing all group IDs.
 */
std::vector<AString>
EventAnnotationTextSubstitutionGetAllGroupIDs::getAllGroupIDs() const
{
    std::vector<AString> ids(m_groupIDs.begin(),
                             m_groupIDs.end());
    return ids;
}
