
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
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

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
SceneableInterface(),
m_chartDataType(chartDataType)
{
    initializeMembersChartData();
}

/**
 * Destructor.
 */
ChartData::~ChartData()
{
    delete m_sceneAssistant;
    delete m_chartDataSource;
}

/**
 * Initialize members of a new instance.
 */
void
ChartData::initializeMembersChartData()
{
    m_chartDataSource = new ChartDataSource();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_chartDataSource",
                          "ChartDataSource",
                          m_chartDataSource);
}


/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartData::ChartData(const ChartData& obj)
: CaretObject(obj),
SceneableInterface(obj),
m_chartDataType(obj.m_chartDataType)
{
    initializeMembersChartData();
    
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
    m_chartDataType    = obj.m_chartDataType;
    *m_chartDataSource = *obj.m_chartDataSource;
}

/**
 * Create a new instance of a chart for the given data type.
 *
 * @param chartDataType
 *    Type of chart data.
 * @return
 *    Pointer to new instance.
 */
ChartData*
ChartData::newChartDataForChartDataType(const ChartDataTypeEnum::Enum chartDataType)
{
    ChartData* chartData = NULL;
    
    switch (chartDataType) {
        case ChartDataTypeEnum::CHART_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_ADJACENCY_MATRIX:
            CaretAssert(0);
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_DATA_SERIES:
            chartData = new ChartDataCartesian(chartDataType,
                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
            break;
        case ChartDataTypeEnum::CHART_DATA_TYPE_TIME_SERIES:
            chartData = new ChartDataCartesian(chartDataType,
                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME);
            break;
    }
    
    return chartData;
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
ChartData::saveToScene(const SceneAttributes* sceneAttributes,
                                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartData",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveSubClassDataToScene(sceneAttributes,
                            sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ChartData::restoreFromScene(const SceneAttributes* sceneAttributes,
                                               const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    restoreSubClassDataFromScene(sceneAttributes,
                                 sceneClass);
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
 * @return The source of the chart data (const method).
 */
const ChartDataSource*
ChartData::getChartDataSource() const
{
    return m_chartDataSource;
}

/**
 * @return The source of the chart data.
 */
ChartDataSource*
ChartData::getChartDataSource()
{
    return m_chartDataSource;
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

