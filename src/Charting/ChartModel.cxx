
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

#define __CHART_MODEL_DECLARE__
#include "ChartModel.h"
#undef __CHART_MODEL_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartAxis.h"
#include "ChartData.h"
#include "ChartDataCartesian.h"
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
 * @param chartSelectionMode
 *    The selection mode.
 */
ChartModel::ChartModel(const ChartVersionOneDataTypeEnum::Enum chartDataType,
                       const ChartSelectionModeEnum::Enum chartSelectionMode)
: CaretObject(),
SceneableInterface(),
m_chartDataType(chartDataType),
m_chartSelectionMode(chartSelectionMode)
{
    m_bottomAxis = NULL;
    m_leftAxis   = NULL;
    m_rightAxis  = NULL;
    m_topAxis    = NULL;
    
    switch (m_chartSelectionMode) {
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
            m_maximumNumberOfChartDatasToDisplay = 5;
            break;
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
            m_maximumNumberOfChartDatasToDisplay = 1;
            break;
    }
    
    m_averageChartDisplaySelected = false;
    
    m_sceneAssistant = new SceneClassAssistant();
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
    
    removeAllAxes();
    
    removeChartDataPrivate();
}

/**
 * Remove the data data.
 * NOTE: This method cannot be called by constructor/destructor since
 * it calls a virtual method.
 */
void
ChartModel::removeChartData()
{
    removeChartDataPrivate();
    
    updateAfterChartDataHasBeenAddedOrRemoved();
}

/**
 * Remove the data data.
 */
void
ChartModel::removeChartDataPrivate()
{
    m_chartDatas.clear();
}

/**
 * Remove all axes.
 */
void
ChartModel::removeAllAxes()
{
    if (m_bottomAxis != NULL) {
        delete m_bottomAxis;
        m_bottomAxis = NULL;
    }
    if (m_leftAxis != NULL) {
        delete m_leftAxis;
        m_leftAxis = NULL;
    }
    if (m_rightAxis != NULL) {
        delete m_rightAxis;
        m_rightAxis = NULL;
    }
    if (m_topAxis != NULL) {
        delete m_topAxis;
        m_topAxis = NULL;
    }
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
m_chartSelectionMode(obj.m_chartSelectionMode)
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
    m_chartSelectionMode   = obj.m_chartSelectionMode;
    m_maximumNumberOfChartDatasToDisplay = obj.m_maximumNumberOfChartDatasToDisplay;
    
    removeAllAxes();
    
    if (obj.m_leftAxis != NULL) {
        setLeftAxis(obj.m_leftAxis->clone());
    }
    
    if (obj.m_rightAxis != NULL) {
        setRightAxis(obj.m_rightAxis->clone());
    }
    
    if (obj.m_bottomAxis != NULL) {
        setBottomAxis(obj.m_bottomAxis->clone());
    }
    
    if (obj.m_topAxis != NULL) {
        setTopAxis(obj.m_topAxis->clone());
    }
    
    m_averageChartDisplaySelected = obj.m_averageChartDisplaySelected;
    
    removeChartData();
    
    m_chartDatas = obj.m_chartDatas;
    
    ChartData* selectedChartData = NULL;

    switch (m_chartSelectionMode) {
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
            break;
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
        {
            /*
             * If no item selected, choose oldest
             */
            if (selectedChartData == NULL) {
                const int32_t numData = static_cast<int32_t>(m_chartDatas.size());
                if (numData > 0) {
                    const int32_t lastIndex = numData - 1;
                    selectedChartData = m_chartDatas[lastIndex].data();
                }
            }
            
            if (selectedChartData != NULL) {
                /*
                 * Calling the setSelected method will ensure that 
                 * mutual exclusion for selection is maintained
                 */
//                selectedChartData->setSelected(true);
            }
        }
            break;
    }
    
    updateAfterChartDataHasBeenAddedOrRemoved();
}

/**
 * @return The chart data type.
 */
ChartVersionOneDataTypeEnum::Enum
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
ChartSelectionModeEnum::Enum
ChartModel::getChartSelectionMode() const
{
    return m_chartSelectionMode;
}

/**
 * Is this model empty (zero charts)?
 */
bool
ChartModel::isEmpty() const
{
    return m_chartDatas.empty();
}

/**
 * Add a chart model to this controller.
 *
 * @param chartData
 *     Model that is added.
 */
void
ChartModel::addChartData(const QSharedPointer<ChartData>& chartData)
{
    /*
     * If the display is limited to one chart and both the chart
     * in deque and the chart to add are both cartesian charts,
     * copy the chart color.
     */
    if (getMaximumNumberOfChartDatasToDisplay() == 1) {
        if ( ! m_chartDatas.empty()) {
            ChartData* firstChartData = getChartDataAtIndex(0);
            ChartDataCartesian* cartesianChart = dynamic_cast<ChartDataCartesian*>(firstChartData);
            if (cartesianChart != NULL) {
                ChartDataCartesian* newCartesianChart = dynamic_cast<ChartDataCartesian*>(chartData.data());
                if (newCartesianChart != NULL) {
                    newCartesianChart->setColor(cartesianChart->getColor());
                }
            }
        }
    }
    
    m_chartDatas.push_front(chartData);
    
    updateUsingMaximumNumberOfChartDatasToDisplay();
    
    updateAfterChartDataHasBeenAddedOrRemoved();
}

/**
 * Update so the number of charts is never greater than
 * the maximum.
 */
void
ChartModel::updateUsingMaximumNumberOfChartDatasToDisplay()
{
    /*
     * If needed, remove extra items at end of deque
     */
    const int32_t numToRemove = (static_cast<int32_t>(m_chartDatas.size())
                                 - m_maximumNumberOfChartDatasToDisplay);
    if (numToRemove > 0) {
        for (int32_t i = 0; i < numToRemove; i++) {
            m_chartDatas.pop_back();
        }
    }
    
    switch (m_chartSelectionMode) {
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
            break;
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
        {
            /*
             * See if any item is selected
             */
            bool haveSelectedItem = false;
            const int32_t numData = static_cast<int32_t>(m_chartDatas.size());
            for (int32_t i = 0; i < numData; i++) {
            }
            
            /*
             * If no item selected, selected oldest
             */
            if ( ! haveSelectedItem) {
                if (numData >= 0) {
                }
            }
        }
            break;
    }
}


/**
 * @return All chart datas (const method)
 */
std::vector<const ChartData*>
ChartModel::getAllChartDatas() const
{
    std::vector<const ChartData*> datasOut;
    
    for (std::deque<QSharedPointer<ChartData> >::const_iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        datasOut.push_back(iter->data());
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
    
    for (std::deque<QSharedPointer<ChartData> >::const_iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        datasOut.push_back(iter->data());
    }
    
    return datasOut;
}

/**
 * @return All SELECTED chart datas in the given tab.
 * @param tabIndex
 *     Index of tab.
 */
std::vector<const ChartData*>
ChartModel::getAllSelectedChartDatas(const int32_t tabIndex) const
{
    std::vector<const ChartData*> datasOut;
    
    for (std::deque<QSharedPointer<ChartData> >::const_iterator iter = m_chartDatas.begin();
         iter != m_chartDatas.end();
         iter++) {
        ChartData* cd = iter->data();
        if (cd->isSelected(tabIndex)) {
            datasOut.push_back(cd);
        }
    }
    
    return datasOut;
}

/**
 * @return Number of chart data.
 */
int32_t
ChartModel::getNumberOfChartData() const
{
    return m_chartDatas.size();
}

/**
 * Get the chart data at the given index.
 *
 * @param chartDataIndex
 *    Index of desired chart data.
 * @return
 *    ChartData at the given index.
 */
ChartData*
ChartModel::getChartDataAtIndex(const int32_t chartDataIndex)
{
    CaretAssertVectorIndex(m_chartDatas,
                           chartDataIndex);
    return m_chartDatas[chartDataIndex].data();
}

/**
 * Get the chart data at the given index (const method).
 *
 * @param chartDataIndex
 *    Index of desired chart data.
 * @return
 *    ChartData at the given index.
 */
const ChartData*
ChartModel::getChartDataAtIndex(const int32_t chartDataIndex) const
{
    CaretAssertVectorIndex(m_chartDatas,
                           chartDataIndex);
    return m_chartDatas[chartDataIndex].data();
}

/**
 * Move the chart data at the given index by swapping with the chart
 * data at (chartDataIndex - 1).
 *
 * @param chartDataIndex
 *    Index of the chart data.
 */
void
ChartModel::moveChartDataAtIndexToOneLowerIndex(const int32_t chartDataIndex)
{
    CaretAssertVectorIndex(m_chartDatas,
                           chartDataIndex);
    
    if (chartDataIndex > 0) {
        std::swap(m_chartDatas[chartDataIndex],
                  m_chartDatas[chartDataIndex - 1]);
        updateAfterChartDataHasBeenAddedOrRemoved();
    }
}

/**
 * Move the chart data at the given index by swapping with the chart
 * data at (chartDataIndex + 1).
 *
 * @param chartDataIndex
 *    Index of the chart data.
 */
void
ChartModel::moveChartDataAtIndexToOneHigherIndex(const int32_t chartDataIndex)
{
    CaretAssertVectorIndex(m_chartDatas,
                           chartDataIndex);
    
    if (chartDataIndex < (static_cast<int32_t>(m_chartDatas.size()) - 1)) {
        std::swap(m_chartDatas[chartDataIndex],
                  m_chartDatas[chartDataIndex + 1]);
        updateAfterChartDataHasBeenAddedOrRemoved();
    }
}

/**
 * Remove the chart data at the given index.
 *
 * @param chartDataIndex
 *    Index of the chart data.
 */
void
ChartModel::removeChartAtIndex(const int32_t chartDataIndex)
{
    CaretAssertVectorIndex(m_chartDatas,
                           chartDataIndex);
    
    const int32_t lastIndex = m_chartDatas.size() - 1;
    
    for (int32_t i = chartDataIndex; i < lastIndex; i++) {
        CaretAssertVectorIndex(m_chartDatas, (i + 1));
        m_chartDatas[i] = m_chartDatas[i + 1];
    }
    
    m_chartDatas.resize(m_chartDatas.size() - 1);
    
    updateAfterChartDataHasBeenAddedOrRemoved();
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
    
    switch (m_chartSelectionMode) {
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
            m_maximumNumberOfChartDatasToDisplay = numberToDisplay;
            break;
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
            m_maximumNumberOfChartDatasToDisplay = 1;
            break;
    }

    updateUsingMaximumNumberOfChartDatasToDisplay();
    
    updateAfterChartDataHasBeenAddedOrRemoved();
}

/**
 * @return Chart Axis for left.  NULL if axis not valid.
 */
ChartAxis*
ChartModel::getLeftAxis()
{
    return m_leftAxis;
}

/**
 * @return Chart Axis for left (const method).  NULL if axis not valid.
 */
const ChartAxis*
ChartModel::getLeftAxis() const
{
    return m_leftAxis;
}

/**
 * Set the bottom axis.  Replaces current axis.
 *
 * @param bottomAxis
 *    New bottom axis.
 */
void
ChartModel::setBottomAxis(ChartAxis* bottomAxis)
{
    if (m_bottomAxis != NULL) {
        delete m_bottomAxis;
    }
    m_bottomAxis = bottomAxis;
    m_bottomAxis->setParentChartModel(this);
}

/**
 * @return Chart Axis for right.  NULL if axis not valid.
 */
ChartAxis*
ChartModel::getRightAxis()
{
    return m_rightAxis;
}

/**
 * @return Chart Axis for right (const method).  NULL if axis not valid.
 */
const ChartAxis*
ChartModel::getRightAxis() const
{
    return m_rightAxis;
}

/**
 * Set the right axis.  Replaces current axis.
 *
 * @param rightAxis
 *    New right axis.
 */
void
ChartModel::setRightAxis(ChartAxis* rightAxis)
{
    if (m_rightAxis != NULL) {
        delete m_rightAxis;
    }
    m_rightAxis = rightAxis;
    m_rightAxis->setParentChartModel(this);
}


/**
 * @return Chart Bottom for bottom.  NULL if axis not valid.
 */
ChartAxis*
ChartModel::getBottomAxis()
{
    return m_bottomAxis;
}

/**
 * @return Chart Axis for bottom (const method).  NULL if axis not valid.
 */
const ChartAxis*
ChartModel::getBottomAxis() const
{
    return m_bottomAxis;
}

/**
 * Set the left axis.  Replaces current axis.
 *
 * @param leftAxis
 *    New left axis.
 */
void
ChartModel::setLeftAxis(ChartAxis* leftAxis)
{
    if (m_leftAxis != NULL) {
        delete m_leftAxis;
    }
    m_leftAxis = leftAxis;
    m_leftAxis->setParentChartModel(this);
}

/**
 * @return Chart Axis for top.  NULL if axis not valid.
 */
ChartAxis*
ChartModel::getTopAxis()
{
    return m_topAxis;
}

/**
 * @return Chart Axis for top (const method).  NULL if axis not valid.
 */
const ChartAxis*
ChartModel::getTopAxis() const
{
    return m_topAxis;
}

/**
 * Set the top axis.  Replaces current axis.
 *
 * @param topAxis
 *    New top axis.
 */
void
ChartModel::setTopAxis(ChartAxis* topAxis)
{
    if (m_topAxis != NULL) {
        delete m_topAxis;
    }
    m_topAxis = topAxis;
    m_topAxis->setParentChartModel(this);
}

/**
 * Called by child ChartData when its selection status changes.
 *
 * If the selection mode is mutually exclusive, this method
 * ensures that no more than one child is selected.
 *
 * If the selection mode is NOT mutually exclusive, no 
 * action is taken.
 */
void
ChartModel::childChartDataSelectionChanged(ChartData* /*childChartData*/)
{
    switch (m_chartSelectionMode) {
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_ANY:
            break;
        case ChartSelectionModeEnum::CHART_SELECTION_MODE_SINGLE:
            //        if (childChartData->isSelected()) {
            //            const int32_t numChartData = static_cast<int32_t>(m_chartDatas.size());
            //            for (int32_t i = 0; i < numChartData; i++) {
            //                ChartData* cd = m_chartDatas[i];
            //                if (cd != childChartData) {
            //                    cd->setSelected(false);
            //                }
            //            }
            //        }
            break;
    }
}

/**
 * Update after chart data has been added or removed.
 */
void
ChartModel::updateAfterChartDataHasBeenAddedOrRemoved()
{
    if (m_bottomAxis != NULL) {
        m_bottomAxis->updateForAutoRangeScale();
    }
    if (m_leftAxis != NULL) {
        m_leftAxis->updateForAutoRangeScale();
    }
    if (m_rightAxis != NULL) {
        m_rightAxis->updateForAutoRangeScale();
    }
    if (m_topAxis != NULL) {
        m_topAxis->updateForAutoRangeScale();
    }
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
                                            2);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if (numChartData > 0) {
        std::vector<AString> chartDataUniqueIDs;
        for (int32_t i = 0; i < numChartData; i++) {
            chartDataUniqueIDs.push_back(m_chartDatas[i]->getUniqueIdentifier());
        }
        
        sceneClass->addStringArray("chartUniqueIDsArray",
                                   &chartDataUniqueIDs[0],
                                   numChartData);
    }
    
    if (m_bottomAxis != NULL) {
        sceneClass->addEnumeratedType<ChartAxisTypeEnum,ChartAxisTypeEnum::Enum>("bottomAxisType",
                                                                                 m_bottomAxis->getAxisType());
        sceneClass->addChild(m_bottomAxis->saveToScene(sceneAttributes,
                                                     "m_bottomAxis"));
    }
    
    if (m_leftAxis != NULL) {
        sceneClass->addEnumeratedType<ChartAxisTypeEnum,ChartAxisTypeEnum::Enum>("leftAxisType",
                                                                                 m_leftAxis->getAxisType());
        sceneClass->addChild(m_leftAxis->saveToScene(sceneAttributes,
                                                     "m_leftAxis"));
    }
    
    if (m_rightAxis != NULL) {
        sceneClass->addEnumeratedType<ChartAxisTypeEnum,ChartAxisTypeEnum::Enum>("rightAxisType",
                                                                                 m_rightAxis->getAxisType());
        sceneClass->addChild(m_rightAxis->saveToScene(sceneAttributes,
                                                     "m_rightAxis"));
    }
    
    if (m_topAxis != NULL) {
        sceneClass->addEnumeratedType<ChartAxisTypeEnum,ChartAxisTypeEnum::Enum>("topAxisType",
                                                                                 m_topAxis->getAxisType());
        sceneClass->addChild(m_topAxis->saveToScene(sceneAttributes,
                                                     "m_topAxis"));
    }
    
    saveSubClassDataToScene(sceneAttributes,
                            sceneClass);
    
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
    
    const int32_t versionNumber = sceneClass->getVersionNumber();
    
    removeChartData();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    removeAllAxes();
    
    /*
     * Restore bottom axis
     */
    const ChartAxisTypeEnum::Enum bottomAxisType =
    sceneClass->getEnumeratedTypeValue<ChartAxisTypeEnum, ChartAxisTypeEnum::Enum>("bottomAxisType",
                                                                                   ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE);
    if (bottomAxisType != ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE) {
        ChartAxis* axis = ChartAxis::newChartAxisForTypeAndLocation(bottomAxisType, ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM);
        axis->restoreFromScene(sceneAttributes,
                               sceneClass->getClass("m_bottomAxis"));
        setBottomAxis(axis);
    }
    
    /*
     * Restore left axis
     */
    const ChartAxisTypeEnum::Enum leftAxisType =
    sceneClass->getEnumeratedTypeValue<ChartAxisTypeEnum, ChartAxisTypeEnum::Enum>("leftAxisType",
                                                                                   ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE);
    if (leftAxisType != ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE) {
        ChartAxis* axis = ChartAxis::newChartAxisForTypeAndLocation(leftAxisType, ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT);
        axis->restoreFromScene(sceneAttributes,
                               sceneClass->getClass("m_leftAxis"));
        setLeftAxis(axis);
    }
    
    if (versionNumber >= 2) {
        /*
         * Restore right axis
         */
        const ChartAxisTypeEnum::Enum rightAxisType =
        sceneClass->getEnumeratedTypeValue<ChartAxisTypeEnum, ChartAxisTypeEnum::Enum>("rightAxisType",
                                                                                       ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE);
        if (rightAxisType != ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE) {
            ChartAxis* axis = ChartAxis::newChartAxisForTypeAndLocation(rightAxisType, ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT);
            axis->restoreFromScene(sceneAttributes,
                                   sceneClass->getClass("m_rightAxis"));
            setRightAxis(axis);
        }
        
        /*
         * Restore top axis
         */
        const ChartAxisTypeEnum::Enum topAxisType =
        sceneClass->getEnumeratedTypeValue<ChartAxisTypeEnum, ChartAxisTypeEnum::Enum>("topAxisType",
                                                                                       ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE);
        if (topAxisType != ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE) {
            ChartAxis* axis = ChartAxis::newChartAxisForTypeAndLocation(topAxisType, ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP);
            axis->restoreFromScene(sceneAttributes,
                                   sceneClass->getClass("m_topAxis"));
            setTopAxis(axis);
        }
    }
    
    /*
     * Restore unique IDs of ChartData
     */
    m_chartDataUniqueIDsRestoredFromScene.clear();
    const ScenePrimitiveArray* chartUniqueIDsArray = sceneClass->getPrimitiveArray("chartUniqueIDsArray");
    if (chartUniqueIDsArray != NULL) {
        const int32_t numElements = chartUniqueIDsArray->getNumberOfArrayElements();
        m_chartDataUniqueIDsRestoredFromScene.resize(numElements);
        chartUniqueIDsArray->stringValues(m_chartDataUniqueIDsRestoredFromScene,
                                          "");

        /*
         * Need to reverse order so that multiple charts are restored
         * in the correct order.  Done here rather than when saving
         * scenes so that old scenes are restored correctly.
         */
        if ( ! m_chartDataUniqueIDsRestoredFromScene.empty()) {
            std::reverse(m_chartDataUniqueIDsRestoredFromScene.begin(),
                         m_chartDataUniqueIDsRestoredFromScene.end());
        }
    }
    
    restoreSubClassDataFromScene(sceneAttributes,
                                 sceneClass);
}

/**
 * Restore chart data from scene by matching unique identifiers from charts
 *
 * @param restoredChartData
 *     Chart data restored from scenes.
 */
void
ChartModel::restoreChartDataFromScene(const SceneAttributes* sceneAttributes,
                                      std::vector<QSharedPointer<ChartData> >& restoredChartData)
{
    const int32_t numChartUniqueIDsFromScene = static_cast<int32_t>(m_chartDataUniqueIDsRestoredFromScene.size());
    if (numChartUniqueIDsFromScene > 0) {
        std::vector<QSharedPointer<ChartData> > chartDataVector(numChartUniqueIDsFromScene);
        
        /*
         * Need to keep chart data in same order as when scene was created
         */
        for (int32_t i = 0; i < numChartUniqueIDsFromScene; i++) {
            for (std::vector<QSharedPointer<ChartData> >::iterator chartIter = restoredChartData.begin();
                 chartIter != restoredChartData.end();
                 chartIter++) {
                QSharedPointer<ChartData> cd = *chartIter;
                if (cd->getUniqueIdentifier() == m_chartDataUniqueIDsRestoredFromScene[i]) {
                    if (cd->getChartDataType() == m_chartDataType) {
                        chartDataVector[i] = cd;
                        break;
                    }
                }
            }
        }
        
        /*
         * Add the restored chart data
         */
        for (int32_t i = 0; i < numChartUniqueIDsFromScene; i++) {
            QSharedPointer<ChartData> cd = chartDataVector[i];
            if (cd.isNull()) {
                const AString msg("Failed to restore chart with Unique ID: "
                                  + m_chartDataUniqueIDsRestoredFromScene[i]);
                sceneAttributes->addToErrorMessage(msg);
                CaretLogSevere(msg);
            }
            else {
                addChartData(cd);
            }
        }
    }
    
    m_chartDataUniqueIDsRestoredFromScene.clear();

    updateUsingMaximumNumberOfChartDatasToDisplay();
    
    updateAfterChartDataHasBeenAddedOrRemoved();
}


