
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

#define __CHART_MODEL_DECLARE__
#include "ChartModel.h"
#undef __CHART_MODEL_DECLARE__

#include "CaretAssert.h"
#include "ChartAxis.h"
#include "ChartData.h"
#include "ChartDataSource.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneStringArray.h"

using namespace caret;


    
/**
 * \class caret::ChartModel 
 * \brief Base class for chart model
 * \ingroup Charting
 *
 * Base class for chart model that displays chart data of the same type.
 * Some data types may require mututal exclusive display.
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *    Model type of chart that is managed.
 * @param supportsMultipleChartDisplayType
 *    Multiple chart support
 */
ChartModel::ChartModel(const ChartDataTypeEnum::Enum chartDataType,
                                                             const SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE supportsMultipleChartDisplayType)
: CaretObject(),
m_chartDataType(chartDataType),
m_supportsMultipleChartDisplayType(supportsMultipleChartDisplayType)
{
    m_bottomAxis = new ChartAxis(ChartAxis::AXIS_BOTTOM);
    m_leftAxis   = new ChartAxis(ChartAxis::AXIS_LEFT);
    m_rightAxis  = new ChartAxis(ChartAxis::AXIS_RIGHT);
    m_topAxis    = new ChartAxis(ChartAxis::AXIS_TOP);
    
    switch (m_supportsMultipleChartDisplayType) {
        case SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_NO:
            m_maximumNumberOfChartDatasToDisplay = 1;
            break;
        case SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_YES:
            m_maximumNumberOfChartDatasToDisplay = 5;
            break;
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_bottomAxis", "ChartAxis", m_bottomAxis);
    m_sceneAssistant->add("m_leftAxis", "ChartAxis", m_leftAxis);
    m_sceneAssistant->add("m_rightAxis", "ChartAxis", m_rightAxis);
    m_sceneAssistant->add("m_topAxis", "ChartAxis", m_topAxis);
    m_sceneAssistant->add("m_maximumNumberOfChartDatasToDisplay",
                          &m_maximumNumberOfChartDatasToDisplay);
}

/**
 * Destructor.
 */
ChartModel::~ChartModel()
{
    delete m_sceneAssistant;
    
    delete m_bottomAxis;
    delete m_leftAxis;
    delete m_rightAxis;
    delete m_topAxis;
    
    removeChartData();
}

/**
 * Remove the data data.
 */
void
ChartModel::removeChartData()
{
    for (std::deque<ChartData*>::iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        delete *iter;
    }
}


/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartModel::ChartModel(const ChartModel& obj)
: CaretObject(obj),
m_chartDataType(obj.m_chartDataType),
m_supportsMultipleChartDisplayType(obj.m_supportsMultipleChartDisplayType)
{
    this->copyHelperChartModel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
ChartModel&
ChartModel::operator=(const ChartModel& obj)
{
    if (this != &obj) {
        this->copyHelperChartModel(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
ChartModel::copyHelperChartModel(const ChartModel& obj)
{
    CaretAssert(0);
}

/**
 * @return The chart data type.
 */
ChartDataTypeEnum::Enum
ChartModel::getChartDataType() const
{
    return m_chartDataType;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartModel::toString() const
{
    return "ChartModel";
}

/**
 * @return Support for multiple chart display.  Some chart types allow
 * it and others do not.
 */
ChartModel::SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE
ChartModel::getSupportForMultipleChartDisplay() const
{
    return m_supportsMultipleChartDisplayType;
}

/**
 * Add a chart model to this controller.
 *
 * @param chartData
 *     Model that is added.
 */
void
ChartModel::addChartData(ChartData* chartData)
{
    CaretAssert(chartData);
    
    m_chartDatas.push_back(chartData->clone());
    
    if (static_cast<int32_t>(m_chartDatas.size()) > m_maximumNumberOfChartDatasToDisplay) {
        m_chartDatas.resize(m_maximumNumberOfChartDatasToDisplay);
    }
    
    resetAxesToDefaultRange();
}

/**
 * @return The chart data models that should be displayed.
 */
std::vector<ChartData*>
ChartModel::getChartDatasForDisplay() const
{
    std::vector<ChartData*> datasOut;
    
    int32_t counter = 0;
    
    for (std::deque<ChartData*>::const_iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        datasOut.push_back(*iter);
        
        counter++;
        if (counter >= m_maximumNumberOfChartDatasToDisplay) {
            break;
        }
    }
    
    return datasOut;
}


/**
 * @return The MAXIMUM number of chart models for display.
 *
 * NOTE: This value MAY BE GREATER than the actual number of chart models
 * that are available.
 */
int32_t
ChartModel::getMaximumNumberOfChartDatasToDisplay() const
{
    return m_maximumNumberOfChartDatasToDisplay;
}

/**
 * Set the number of most recent chart models for display.
 *
 * @param numberToDisplay
 *    New number of most recent models for display.
 */
void
ChartModel::setMaximumNumberOfChartDatasToDisplay(const int32_t numberToDisplay)
{
    CaretAssert(numberToDisplay > 0);
    
    switch (m_supportsMultipleChartDisplayType) {
        case SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_NO:
            m_maximumNumberOfChartDatasToDisplay = 1;
            break;
        case SUPPORTS_MULTIPLE_CHART_DISPLAY_TYPE_YES:
            m_maximumNumberOfChartDatasToDisplay = numberToDisplay;
            break;
    }
}

/**
 * @return Chart Axis for left.
 */
ChartAxis*
ChartModel::getLeftAxis()
{
    return m_leftAxis;
}

/**
 * @return Chart Axis for left (const method)
 */
const ChartAxis*
ChartModel::getLeftAxis() const
{
    return m_leftAxis;
}

/**
 * @return Chart Axis for right.
 */
ChartAxis*
ChartModel::getRightAxis()
{
    return m_rightAxis;
}

/**
 * @return Chart Axis for right (const method)
 */
const ChartAxis*
ChartModel::getRightAxis() const
{
    return m_rightAxis;
}

/**
 * @return Chart Bottom for bottom.
 */
ChartAxis*
ChartModel::getBottomAxis()
{
    return m_bottomAxis;
}

/**
 * @return Chart Axis for bottom (const method)
 */
const ChartAxis*
ChartModel::getBottomAxis() const
{
    return m_bottomAxis;
}

/**
 * @return Chart Axis for top.
 */
ChartAxis*
ChartModel::getTopAxis()
{
    return m_topAxis;
}

/**
 * @return Chart Axis for top (const method)
 */
const ChartAxis*
ChartModel::getTopAxis() const
{
    return m_topAxis;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
ChartModel::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    const int32_t numChartData = static_cast<int32_t>(m_chartDatas.size());
    
    if (numChartData <= 0) {
        return NULL;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    std::vector<SceneClass*> chartDataSceneClassVector;
    
    for (int32_t i = 0; i < numChartData; i++) {
        const AString name = ("m_chartDatas[" + AString::number(i) + "]");
        SceneClass* sc = m_chartDatas[i]->saveToScene(sceneAttributes,
                                                      name);
        chartDataSceneClassVector.push_back(sc);
    }

    SceneClassArray* chartDataArray = new SceneClassArray("chartDataArray",
                                                          chartDataSceneClassVector);
    sceneClass->addChild(chartDataArray);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void
ChartModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                          const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    removeChartData();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    /*
     * Restore my ChartData
     */
    const SceneClassArray* chartDataArray = sceneClass->getClassArray("chartDataArray");
    const int32_t numChartData = chartDataArray->getNumberOfArrayElements();
    for (int32_t i = 0; i < numChartData; i++) {
        const SceneClass* chartDataClass = chartDataArray->getClassAtIndex(i);
        ChartData* chartData = ChartData::newChartDataForChartDataType(m_chartDataType);
        chartData->restoreFromScene(sceneAttributes, chartDataClass);
        m_chartDatas.push_back(chartData);
    }
}
