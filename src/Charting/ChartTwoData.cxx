
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

#define __CHART_TWO_DATA_DECLARE__
#include "ChartTwoData.h"
#undef __CHART_TWO_DATA_DECLARE__

#include "CaretAssert.h"
#include "ChartTwoDataCartesian.h"
#include "ChartDataSource.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SystemUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoData 
 * \brief Base class for chart data.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param chartDataType
 *   Type of chart data.
 */
ChartTwoData::ChartTwoData(const ChartTwoDataTypeEnum::Enum chartDataType)
: CaretObjectTracksModification(),
SceneableInterface(),
m_chartDataType(chartDataType)
{
    initializeMembersChartTwoData();
}

/**
 * Destructor.
 */
ChartTwoData::~ChartTwoData()
{
    delete m_sceneAssistant;
    delete m_chartDataSource;
}

/**
 * Initialize members of a new instance.
 */
void
ChartTwoData::initializeMembersChartTwoData()
{
    m_chartDataSource = new ChartDataSource();
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectionStatus[i] = true;
    }
    m_uniqueIdentifier = SystemUtilities::createUniqueID();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_chartDataSource",
                          "ChartDataSource",
                          m_chartDataSource);
    m_sceneAssistant->addTabIndexedBooleanArray("m_selectionStatus",
                                                m_selectionStatus);
    m_sceneAssistant->add("m_uniqueIdentifier",
                          &m_uniqueIdentifier);
}


/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoData::ChartTwoData(const ChartTwoData& obj)
: CaretObjectTracksModification(obj),
SceneableInterface(obj),
m_chartDataType(obj.m_chartDataType)
{
    initializeMembersChartTwoData();
    
    this->copyHelperChartTwoData(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoData&
ChartTwoData::operator=(const ChartTwoData& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperChartTwoData(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoData::copyHelperChartTwoData(const ChartTwoData& obj)
{
    CaretAssert(0);
    m_chartDataType    = obj.m_chartDataType;
    *m_chartDataSource = *obj.m_chartDataSource;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectionStatus[i] = obj.m_selectionStatus[i];
    }
}

/**
 * Create a new instance of a chart for the given data type.
 *
 * @param chartDataType
 *    Type of chart data.
 * @return
 *    Pointer to new instance.
 */
ChartTwoData*
ChartTwoData::newChartTwoDataForChartTwoDataType(const ChartTwoDataTypeEnum::Enum chartDataType)
{
    ChartTwoData* chartData = NULL;
    
    switch (chartDataType) {
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
            CaretAssert(0);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
            CaretAssert(0);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
            CaretAssert(0);
            break;
        case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
            chartData = new ChartTwoDataCartesian(chartDataType,
                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE,
                                               ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE);
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
ChartTwoData::saveToScene(const SceneAttributes* sceneAttributes,
                                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoData",
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
ChartTwoData::restoreFromScene(const SceneAttributes* sceneAttributes,
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
 * @return The chart data type.
 */
ChartTwoDataTypeEnum::Enum
ChartTwoData::getChartDataType() const
{
    return m_chartDataType;
}

/**
 * @return The source of the chart data (const method).
 */
const ChartDataSource*
ChartTwoData::getChartDataSource() const
{
    return m_chartDataSource;
}

/**
 * @return The source of the chart data.
 */
ChartDataSource*
ChartTwoData::getChartDataSource()
{
    return m_chartDataSource;
}

/**
 * @return The selection status in the given tab
 * @param tabIndex
 *    Index of the tab.
 */
bool
ChartTwoData::isSelected(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_selectionStatus, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_selectionStatus[tabIndex];
}

/**
 * Set the selection status.
 *
 * @param selectionStatus
 *    New selection status.
 */
void
ChartTwoData::setSelected(const int32_t tabIndex,
                       const bool selectionStatus)
{
    CaretAssertArrayIndex(m_selectionStatus, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    m_selectionStatus[tabIndex] = selectionStatus;
    
    /*
     * When selection status is true,
     * notify parent.
     */
//    if (m_selectionStatus[tabIndex]) {
//        if (m_parentChartModel != NULL) {
//            m_parentChartModel->childChartTwoDataSelectionChanged(this);
//        }
//    }
}

/**
 * Copy the selection status for all tabs from the given chart data to me.
 *
 * @param copyFrom
 *     Chart data from which selection status is copied.
 */
void
ChartTwoData::copySelectionStatusForAllTabs(const ChartTwoData* copyFrom)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectionStatus[i] = copyFrom->m_selectionStatus[i];
    }
}


/**
 * @return The Unique Identifier (UUID).
 */
AString
ChartTwoData::getUniqueIdentifier() const
{
    return m_uniqueIdentifier;
}

/**
 * Set the unique identifier.
 *
 * @param uniqueIdentifier
 *    The new unique identifier.
 */
void
ChartTwoData::setUniqueIdentifier(const AString& uniqueIdentifier)
{
    m_uniqueIdentifier = uniqueIdentifier;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoData::toString() const
{
    return "ChartTwoData";
}

