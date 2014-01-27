
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

#define __CHART_DATA_DECLARE__
#include "ChartData.h"
#undef __CHART_DATA_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ChartData 
 * \brief Base class for chart data.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *   Type of chart model.
 */
ChartData::ChartData(const ChartDataTypeEnum::Enum chartDataType)
: CaretObject(),
m_chartDataType(chartDataType)
{
    
}

/**
 * Destructor.
 */
ChartData::~ChartData()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartData::ChartData(const ChartData& obj)
: CaretObject(obj),
m_chartDataType(obj.m_chartDataType)
{
    this->copyHelperChartData(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartData&
ChartData::operator=(const ChartData& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartData(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartData::copyHelperChartData(const ChartData& obj)
{
    CaretAssert(0);
}

/**
 * @return The chart data model type.
 */
ChartDataTypeEnum::Enum
ChartData::getChartDataType() const
{
    return m_chartDataType;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartData::toString() const
{
    return "ChartData";
}

