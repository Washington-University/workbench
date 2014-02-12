
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
#include "CaretLogger.h"
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
 * @param selectionMode
 *    The selection mode.
 */
ChartModel::ChartModel(const ChartDataTypeEnum::Enum chartDataType,
                       const SelectionMode selectionMode)
: CaretObject(),
SceneableInterface(),
m_chartDataType(chartDataType),
m_selectionMode(selectionMode)
{
    m_bottomAxis = new ChartAxis(ChartAxis::AXIS_BOTTOM);
    m_leftAxis   = new ChartAxis(ChartAxis::AXIS_LEFT);
    m_rightAxis  = new ChartAxis(ChartAxis::AXIS_RIGHT);
    m_topAxis    = new ChartAxis(ChartAxis::AXIS_TOP);
    
    switch (m_selectionMode) {
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_YES:
            m_maximumNumberOfChartDatasToDisplay = 1;
            break;
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_NO:
            m_maximumNumberOfChartDatasToDisplay = 5;
            break;
    }
    
    m_averageChartDisplaySelected = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_bottomAxis", "ChartAxis", m_bottomAxis);
    m_sceneAssistant->add("m_leftAxis", "ChartAxis", m_leftAxis);
    m_sceneAssistant->add("m_rightAxis", "ChartAxis", m_rightAxis);
    m_sceneAssistant->add("m_topAxis", "ChartAxis", m_topAxis);
    m_sceneAssistant->add("m_maximumNumberOfChartDatasToDisplay",
                          &m_maximumNumberOfChartDatasToDisplay);
    m_sceneAssistant->add("m_averageChartDisplaySelected",
                          &m_averageChartDisplaySelected);
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
    m_chartDatas.clear();
}


/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartModel::ChartModel(const ChartModel& obj)
: CaretObject(obj),
SceneableInterface(obj),
m_chartDataType(obj.m_chartDataType),
m_selectionMode(obj.m_selectionMode)
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
    m_chartDataType = obj.m_chartDataType;
    m_selectionMode   = obj.m_selectionMode;
    m_maximumNumberOfChartDatasToDisplay = obj.m_maximumNumberOfChartDatasToDisplay;
    *m_leftAxis   = *obj.m_leftAxis;
    *m_rightAxis  = *obj.m_rightAxis;
    *m_bottomAxis = *obj.m_bottomAxis;
    *m_topAxis    = *obj.m_topAxis;
    m_averageChartDisplaySelected = obj.m_averageChartDisplaySelected;
    
    removeChartData();
    
    for (std::deque<ChartData*>::const_iterator iter = obj.m_chartDatas.begin();
         iter != obj.m_chartDatas.end();
         iter++) {
        ChartData* cd = *iter;
        m_chartDatas.push_back(cd->clone());
    }
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
ChartModel::SelectionMode
ChartModel::getSelectionMode() const
{
    return m_selectionMode;
}

/**
 * Add a chart model to this controller.
 *
 * @param chartDataIn
 *     Model that is added.
 */
void
ChartModel::addChartData(const ChartData* chartDataIn)
{
    CaretAssert(chartDataIn);
    
    ChartData* chartData = chartDataIn->clone();
    
    switch (m_selectionMode) {
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_YES:
            chartData->setSelected(false);
            break;
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_NO:
            chartData->setSelected(true);
            break;
    }
    
    m_chartDatas.push_front(chartData);
    
    const int32_t numChartData = static_cast<int32_t>(m_chartDatas.size());
    
    /*
     * If needed, remove extra items at end of deque
     */
    bool selectedItemWasRemoved = false;
    const int32_t numToRemove = numChartData - m_maximumNumberOfChartDatasToDisplay;
    if (numToRemove > 0) {
        for (int32_t i = 0; i < numToRemove; i++) {
            ChartData* cd = m_chartDatas.back();
            if (cd->isSelected()) {
                selectedItemWasRemoved = true;
            }
            m_chartDatas.pop_back();
            delete cd;
        }
    }

    if (selectedItemWasRemoved) {
        switch (m_selectionMode) {
            case SELECTION_MODE_MUTUALLY_EXCLUSIVE_YES:
            {
                bool haveSelectedItem = false;
                const int32_t numData = static_cast<int32_t>(m_chartDatas.size());
                for (int32_t i = 0; i < numData; i++) {
                    if (m_chartDatas[i]->isSelected()) {
                        haveSelectedItem = true;
                        break;
                    }
                }
                
                if ( ! haveSelectedItem) {
                    const int32_t lastIndex = numData - 1;
                    if (numData >= 0) {
                        m_chartDatas[lastIndex]->setSelected(true);
                    }
                }
            }
                break;
            case SELECTION_MODE_MUTUALLY_EXCLUSIVE_NO:
                break;
        }
    }
    
    resetAxesToDefaultRange();
}

/**
 * @return All chart datas (const method)
 */
std::vector<const ChartData*>
ChartModel::getAllChartDatas() const
{
    std::vector<const ChartData*> datasOut;
    
//    int32_t counter = 0;
    
    for (std::deque<ChartData*>::const_iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        datasOut.push_back(*iter);
        
//        counter++;
//        if (counter >= m_maximumNumberOfChartDatasToDisplay) {
//            break;
//        }
    }
    
    return datasOut;
}

/**
 * @return All chart datas.
 */
std::vector<ChartData*>
ChartModel::getAllChartDatas()
{
    std::vector<ChartData*> datasOut;
    
    //    int32_t counter = 0;
    
    for (std::deque<ChartData*>::const_iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        datasOut.push_back(*iter);
        
        //        counter++;
        //        if (counter >= m_maximumNumberOfChartDatasToDisplay) {
        //            break;
        //        }
    }
    
    return datasOut;
}

/**
 * @return All SELECTED chart datas.
 */
std::vector<const ChartData*>
ChartModel::getAllSelectedChartDatas() const
{
    std::vector<const ChartData*> datasOut;
    
    //    int32_t counter = 0;
    
    for (std::deque<ChartData*>::const_iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        ChartData* cd = *iter;
        if (isChartDataSelected(cd)) {
            datasOut.push_back(cd);
        }
        
        //        counter++;
        //        if (counter >= m_maximumNumberOfChartDatasToDisplay) {
        //            break;
        //        }
    }
    
    return datasOut;
}

/**
 * @return Is average chart data display selected.
 * NOTE: Not all charts support an average.
 */
bool
ChartModel::isAverageChartDisplaySelected() const
{
    if (isAverageChartDisplaySupported()) {
        return m_averageChartDisplaySelected;
    }
    return false;
}

/**
 * Set the average chart data selected.  
 * NOTE: Not all charts support an average.
 *
 * @param selected
 *    New status.
 */
void
ChartModel::setAverageChartDisplaySelected(const bool selected)
{
    m_averageChartDisplaySelected = selected;
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
    
    switch (m_selectionMode) {
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_YES:
            m_maximumNumberOfChartDatasToDisplay = 1;
            break;
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_NO:
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
 * Set chart data selection status.
 *
 * @param chartData
 *     Chart data that has its selection status set.
 * @param selectionStatus
 *     New status.
 */
void
ChartModel::setChartDataSelected(ChartData* chartData,
                                 const bool selectionStatus)
{
    switch (m_selectionMode) {
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_YES:
        {
            bool found = false;
            const int32_t numChartData = static_cast<int32_t>(m_chartDatas.size());
            for (int32_t i = 0; i < numChartData; i++) {
                ChartData* cd = m_chartDatas[i];
                if (cd == chartData) {
                    cd->setSelected(selectionStatus);
                    found = true;
                }
                else {
                    cd->setSelected(false);
                }
            }
            
            if ( ! found) {
                if (numChartData > 0) {
                    CaretLogSevere("Attempt to set selection status of chart data not in model.");
                    m_chartDatas[0]->setSelected(true);
                }
            }
        }
            break;
        case SELECTION_MODE_MUTUALLY_EXCLUSIVE_NO:
        {
            bool found = false;
            const int32_t numChartData = static_cast<int32_t>(m_chartDatas.size());
            for (int32_t i = 0; i < numChartData; i++) {
                ChartData* cd = m_chartDatas[i];
                if (cd == chartData) {
                    cd->setSelected(selectionStatus);
                    found = true;
                    break;
                }
            }
            
            if ( ! found) {
                if (numChartData > 0) {
                    CaretLogSevere("Attempt to set selection status of chart data not in model.");
                }
            }
        }
            break;
    }
}

/**
 * Is the chart data item selected? 
 *
 * @param chartData
 *     Chart data tested for selection status.
 *
 * @return 
 *     True if item is selected, else false.
 */
bool
ChartModel::isChartDataSelected(const ChartData* chartData) const
{
    bool selectionStatus = false;
    
    bool found = false;
    
    const int32_t numChartData = static_cast<int32_t>(m_chartDatas.size());
    for (int32_t i = 0; i < numChartData; i++) {
        ChartData* cd = m_chartDatas[i];
        if (cd == chartData) {
            selectionStatus = cd->isSelected();
            found = true;
            break;
        }
    }
    
    if ( ! found) {
        if (numChartData > 0) {
            CaretLogSevere("Attempt to get selection status of chart data not in model.");
        }
    }
    
    return selectionStatus;
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
