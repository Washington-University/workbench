
/*LICENSE_START*/
/*
 * Copyright 2014 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __EVENT_CHARTS_NEW_NOTIFICATION_DECLARE__
#include "EventChartsNewNotification.h"
#undef __EVENT_CHARTS_NEW_NOTIFICATION_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartsNewNotification 
 * \brief This event provides new chart models that have become available.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventChartsNewNotification::EventChartsNewNotification(std::vector<QSharedPointer<ChartData> >& chartDatas)
: Event(EventTypeEnum::EVENT_CHARTS_NEW_NOTIFICATION)
{
    m_chartDatas = chartDatas;
}

/**
 * Destructor.
 */
EventChartsNewNotification::~EventChartsNewNotification()
{
}

/**
 * Get the chart data models that are valid for the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Chart data models for the given tab.
 */
std::vector<QSharedPointer<ChartData> >
EventChartsNewNotification::getChartDatasForTabIndex(const int32_t tabIndex) const
{
    return m_chartDatas;
}


