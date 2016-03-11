
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_GROUP_GET_WITH_KEY_DECLARE__
#include "EventAnnotationGroupGetWithKey.h"
#undef __EVENT_ANNOTATION_GROUP_GET_WITH_KEY_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationGroupGetWithKey 
 * \brief Get an annotation group using its group key.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param groupKey
 *     The annotation group's key.
 */
EventAnnotationGroupGetWithKey::EventAnnotationGroupGetWithKey(const AnnotationGroupKey& groupKey)
: Event(EventTypeEnum::EVENT_ANNOTATION_GROUP_GET_WITH_KEY),
m_groupKey(groupKey),
m_annotationGroup(NULL)
{

}

/**
 * Destructor.
 */
EventAnnotationGroupGetWithKey::~EventAnnotationGroupGetWithKey()
{
}

/**
 * @return The annotation group's key.
 */
const AnnotationGroupKey
EventAnnotationGroupGetWithKey::getGroupKey() const
{
    return m_groupKey;
}

/**
 * @return The annotation group (my be NULL if not found).
 */
AnnotationGroup*
EventAnnotationGroupGetWithKey::getAnnotationGroup() const
{
    return m_annotationGroup;
}

/**
 * Set the annotation group.
 *
 * @param annotationGroup
 *     The annotation group.
 */
void
EventAnnotationGroupGetWithKey::setAnnotationGroup(AnnotationGroup* annotationGroup)
{
    m_annotationGroup = annotationGroup;
}
