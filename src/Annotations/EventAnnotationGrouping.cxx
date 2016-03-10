
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

#define __EVENT_ANNOTATION_GROUPING_DECLARE__
#include "EventAnnotationGrouping.h"
#undef __EVENT_ANNOTATION_GROUPING_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationGrouping 
 * \brief Event for performing an annotation grouping operation.
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
EventAnnotationGrouping::EventAnnotationGrouping()
: Event(EventTypeEnum::EVENT_ANNOTATION_GROUPING),
m_mode(MODE_INVALID),
m_annotationGroup(NULL)
{
}

/**
 * Destructor.
 */
EventAnnotationGrouping::~EventAnnotationGrouping()
{
}

/**
 * Set the mode to group annotations.
 *
 * @param spaceGroup
 *     The annotation space group whose annotations are moved into a user group.
 * @param annotations
 *     Annotations move to user group.
 */
void
EventAnnotationGrouping::setModeGroupAnnotations(const AnnotationGroup* spaceGroup,
                                                 std::vector<Annotation*>& annotations)
{
    m_mode            = MODE_GROUP;
    m_annotationGroup = const_cast<AnnotationGroup*>(spaceGroup);
    m_annotations     = annotations;
}

/*
 * @return The mode.
 */
EventAnnotationGrouping::Mode
EventAnnotationGrouping::getMode() const
{
    return m_mode;
}

/*
 * @return The annotation group.
 */
AnnotationGroup*
EventAnnotationGrouping::getAnnotationGroup() const
{
    return m_annotationGroup;
}

/*
 * @return The annotations.
 */
std::vector<Annotation*>
EventAnnotationGrouping::getAnnotations() const
{
    return m_annotations;
}
