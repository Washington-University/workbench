
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_COLOR_BAR_GET_DECLARE__
#include "EventAnnotationColorBarGet.h"
#undef __EVENT_ANNOTATION_COLOR_BAR_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationColorBarGet 
 * \brief Event to get annontation color bars for tab(s).
 * \ingroup Brain
 */

/**
 * Constructor for getting annotations for ALL tabs.
 */
EventAnnotationColorBarGet::EventAnnotationColorBarGet()
: Event(EventTypeEnum::EVENT_ANNOTATION_COLOR_BAR_GET),
m_allTabsFlag(true)
{
    
}

/**
 * Constructor for getting annotations for the given tab index.
 *
 * @param tabIndex
 *     Index of tab for which color bars are requested.
 */
EventAnnotationColorBarGet::EventAnnotationColorBarGet(const int32_t tabIndex)
: Event(EventTypeEnum::EVENT_ANNOTATION_COLOR_BAR_GET),
m_allTabsFlag(false)
{
    m_tabIndices.insert(tabIndex);
}

/**
 * Constructor for getting annotations for the given tab indices.
 *
 * @param tabIndices
 *     Indices of tabs for which color bars are requested.  If 
 *     the indices are empty no colorbars will be gotten.
 */
EventAnnotationColorBarGet::EventAnnotationColorBarGet(const std::vector<int32_t>& tabIndices)
: Event(EventTypeEnum::EVENT_ANNOTATION_COLOR_BAR_GET),
m_allTabsFlag(false)
{
    m_tabIndices.insert(tabIndices.begin(),
                        tabIndices.end());
}

/**
 * Destructor.
 */
EventAnnotationColorBarGet::~EventAnnotationColorBarGet()
{
}

/**
 * Add annotation color bars.
 * 
 * @param annotationColorBars
 *     Annotation color bars that are added.
 */
void
EventAnnotationColorBarGet::addAnnotationColorBars(const std::vector<AnnotationColorBar*>& annotationColorBars)
{
    m_annotationColorBars.insert(m_annotationColorBars.end(),
                                 annotationColorBars.begin(),
                                 annotationColorBars.end());
}

/**
 * Are annotation color bars requested for the given tab index.
 *
 * @param tabIndex
 *     Index of tab.
 * @return
 *     True if annotations are requested for the given tab index, else false.
 */
bool
EventAnnotationColorBarGet::isGetAnnotationColorBarsForTabIndex(const int32_t tabIndex)
{
    if (m_allTabsFlag) {
        return true;
    }
    else if (m_tabIndices.find(tabIndex) != m_tabIndices.end()) {
        return true;
    }
    
    return false;
}

/**
 * @return The annotation color bars after event completes.
 */
std::vector<AnnotationColorBar*>
EventAnnotationColorBarGet::getAnnotationColorBars() const
{
    return m_annotationColorBars;
}

