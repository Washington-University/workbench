
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_POLYHEDRON_NAME_COMPONENT_SETTINGS_DECLARE__
#include "EventAnnotationPolyhedronNameComponentSettings.h"
#undef __EVENT_ANNOTATION_POLYHEDRON_NAME_COMPONENT_SETTINGS_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationPolyhedronNameComponentSettings 
 * \brief Event to get settings for components used in a polyhedron's name
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
EventAnnotationPolyhedronNameComponentSettings::EventAnnotationPolyhedronNameComponentSettings()
: Event(EventTypeEnum::EVENT_ANNOTATION_POLYHEDRON_NAME_COMPONENT_SETTINGS)
{
    
}

/**
 * Destructor.
 */
EventAnnotationPolyhedronNameComponentSettings::~EventAnnotationPolyhedronNameComponentSettings()
{
}

/**
 * @return Show polyhedron name
 */
bool 
EventAnnotationPolyhedronNameComponentSettings::isShowName() const
{
    return m_showNameFlag;
}

/**
 * @return Show polyhedron number
 */
bool
EventAnnotationPolyhedronNameComponentSettings::isShowNumber() const
{
    return m_showNumberFlag;
}

/**
 * @return Show show actual/desired suffix
 */
bool
EventAnnotationPolyhedronNameComponentSettings::isShowActualDesiredSuffix() const
{
    return m_showActualDesiredSuffixFlag;
}

/**
 * Show name
 * @param status
 *    New status
 */
void
EventAnnotationPolyhedronNameComponentSettings::setShowName(const bool status)
{
    m_showNameFlag = status;
}

/**
 * Show number
 * @param status
 *    New status
 */
void
EventAnnotationPolyhedronNameComponentSettings::setShowNumber(const bool status)
{
    m_showNumberFlag = status;
}

/**
 * Show actual/desired suffix
 * @param status
 *    New status
 */
void
EventAnnotationPolyhedronNameComponentSettings::setShowActualDesiredSuffix(const bool status)
{
    m_showActualDesiredSuffixFlag = status;
}
