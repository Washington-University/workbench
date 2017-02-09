
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __EVENT_CHART_OVERLAY_VALIDATE_DECLARE__
#include "EventChartOverlayValidate.h"
#undef __EVENT_CHART_OVERLAY_VALIDATE_DECLARE__

#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartOverlayValidate 
 * \brief Test a chart overlay for validity (it exists).
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param chartOverlay
 *     Chart overlay for verification.
 */
EventChartOverlayValidate::EventChartOverlayValidate(const ChartTwoOverlay* chartOverlay)
: Event(EventTypeEnum::EVENT_CHART_OVERLAY_VALIDATE),
  m_chartOverlay(chartOverlay)
{
    m_valid = false;
}

/**
 * Destructor.
 */
EventChartOverlayValidate::~EventChartOverlayValidate()
{
    
}

/**
 * @return true if the chart overlay was found to be valid.
 */
bool
EventChartOverlayValidate::isValidChartOverlay() const
{
    return m_valid;
}

/**
 * Set the validity if the given overlay is the overlay
 * that was passed to the constructor.
 *
 * @param chartOverlay
 *    Chart overlay tested for match.
 */
void
EventChartOverlayValidate::testValidChartOverlay(const ChartTwoOverlay* chartOverlay)
{
    if (m_chartOverlay == chartOverlay) {
        m_valid = true;
    }
}

