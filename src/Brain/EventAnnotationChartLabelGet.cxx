
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_CHART_LABEL_GET_DECLARE__
#include "EventAnnotationChartLabelGet.h"
#undef __EVENT_ANNOTATION_CHART_LABEL_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationChartLabelGet 
 * \brief Event to get annotation chart labels
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventAnnotationChartLabelGet::EventAnnotationChartLabelGet()
: Event(EventTypeEnum::EVENT_ANNOTATION_CHART_LABEL_GET)
{
    
}

/**
 * Destructor.
 */
EventAnnotationChartLabelGet::~EventAnnotationChartLabelGet()
{
}

/**
 * Add a chart label.
 *
 * @param annotationChartLabel
 */
void
EventAnnotationChartLabelGet::addAnnotationChartLabel(AnnotationChartTwoAxisLabel* annotationChartLabel)
{
    m_annotationChartLabels.push_back(annotationChartLabel);
}

/**
 * @return The annotation chart labels.
 */
std::vector<AnnotationChartTwoAxisLabel*>
EventAnnotationChartLabelGet::getAnnotationChartLabels()
{
    return m_annotationChartLabels;
}

