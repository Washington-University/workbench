
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

#define __EVENT_ANNOTATION_BARS_GET_DECLARE__
#include "EventAnnotationBarsGet.h"
#undef __EVENT_ANNOTATION_BARS_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationBarsGet
 * \brief Event to get annontation color bars for tab(s).
 * \ingroup Brain
 */

/**
 * Constructor for getting annotation bars for ALL tabs.
 */
EventAnnotationBarsGet::EventAnnotationBarsGet()
: Event(EventTypeEnum::EVENT_ANNOTATION_BARS_GET),
m_allTabsFlag(true)
{
    
}

/**
 * Constructor for getting annotation bars for the given tab index.
 *
 * @param tabIndex
 *     Index of tab for which annotation bars are requested.
 */
EventAnnotationBarsGet::EventAnnotationBarsGet(const int32_t tabIndex)
: Event(EventTypeEnum::EVENT_ANNOTATION_BARS_GET),
m_allTabsFlag(false)
{
    m_tabIndices.insert(tabIndex);
}

/**
 * Constructor for getting annotation bars for the given tab indices.
 *
 * @param tabIndices
 *     Indices of tabs for which annotation bars are requested.  If
 *     the indices are empty no bars will be gotten.
 */
EventAnnotationBarsGet::EventAnnotationBarsGet(const std::vector<int32_t>& tabIndices)
: Event(EventTypeEnum::EVENT_ANNOTATION_BARS_GET),
m_allTabsFlag(false)
{
    m_tabIndices.insert(tabIndices.begin(),
                        tabIndices.end());
}

/**
 * Destructor.
 */
EventAnnotationBarsGet::~EventAnnotationBarsGet()
{
}

/**
 * Add annotation color bars.
 *
 * @param annotationColorBars
 *     Annotation color bars that are added.
 */
void
EventAnnotationBarsGet::addAnnotationColorBars(const std::vector<AnnotationColorBar*>& annotationColorBars)
{
    m_annotationColorBars.insert(m_annotationColorBars.end(),
                                 annotationColorBars.begin(),
                                 annotationColorBars.end());
}

/**
 * Add annotation scale bars.
 *
 * @param annotationScaleBar
 *     Annotation scale bar that is added.
 */
void
EventAnnotationBarsGet::addAnnotationScaleBar(AnnotationScaleBar* annotationScaleBar)
{
    m_annotationScaleBars.push_back(annotationScaleBar);
}


/**
 * Are annotation bars requested for the given tab index.
 *
 * @param tabIndex
 *     Index of tab.
 * @return
 *     True if annotation bars are requested for the given tab index, else false.
 */
bool
EventAnnotationBarsGet::isGetAnnotationColorBarsForTabIndex(const int32_t tabIndex)
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
EventAnnotationBarsGet::getAnnotationColorBars() const
{
    return m_annotationColorBars;
}

/**
 * @return The annotation scale bars after event completes.
 */
std::vector<AnnotationScaleBar*>
EventAnnotationBarsGet::getAnnotationScaleBars() const
{
    return m_annotationScaleBars;
}
