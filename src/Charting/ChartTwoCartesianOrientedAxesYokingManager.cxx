
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __CHART_TWO_AXES_YOKING_MANAGER_DECLARE__
#include "ChartTwoCartesianOrientedAxesYokingManager.h"
#undef __CHART_TWO_AXES_YOKING_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "ChartTwoAxisScaleRangeModeEnum.h"
#include "EventChartTwoCartesianOrientedAxesYoking.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoCartesianOrientedAxesYokingManager
 * \brief Yoking manager for chart axis, maintains yoked values
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoCartesianOrientedAxesYokingManager::ChartTwoCartesianOrientedAxesYokingManager()
: CaretObject()
{
    std::vector<ChartTwoAxisScaleRangeModeEnum::Enum> allRangeModes;
    ChartTwoAxisScaleRangeModeEnum::getAllEnums(allRangeModes);
    const int32_t numRangeModes = static_cast<int32_t>(allRangeModes.size());
    CaretAssert(numRangeModes > 0);
    
    m_horizontalMinimum.resize(numRangeModes, 0.0);
    m_horizontalMaximum.resize(numRangeModes, 1.0);
    m_verticalMinimum.resize(numRangeModes, 0.0);
    m_verticalMaximum.resize(numRangeModes, 1.0);
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_horizontalMinimum", &m_horizontalMinimum[0], m_horizontalMinimum.size(), 0.0);
    m_sceneAssistant->addArray("m_horizontalMaximum", &m_horizontalMaximum[0], m_horizontalMaximum.size(), 1.0);
    m_sceneAssistant->addArray("m_verticalMinimum",   &m_verticalMinimum[0],   m_verticalMinimum.size(),   0.0);
    m_sceneAssistant->addArray("m_verticalMaximum",   &m_verticalMaximum[0],   m_verticalMaximum.size(),   1.0);

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING);
}

/**
 * Destructor.
 */
ChartTwoCartesianOrientedAxesYokingManager::~ChartTwoCartesianOrientedAxesYokingManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoCartesianOrientedAxesYokingManager::toString() const
{
    return "ChartTwoCartesianOrientedAxesYokingManager";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ChartTwoCartesianOrientedAxesYokingManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_CHART_TWO_CARTESIAN_ORIENTED_AXES_YOKING) {
        auto chartEvent = dynamic_cast<EventChartTwoCartesianOrientedAxesYoking*>(event);
        CaretAssert(chartEvent);
        
        switch (chartEvent->getMode()) {
            case EventChartTwoCartesianOrientedAxesYoking::Mode::GET_MINIMUM_AND_MAXIMUM_VALUES:
            {
                const int32_t yokeIndex(static_cast<int32_t>(chartEvent->getYokingRangeMode()));
                switch (chartEvent->getAxisOrientation()) {
                    case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
                        CaretAssertVectorIndex(m_horizontalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_horizontalMaximum, yokeIndex);
                        chartEvent->setMinimumAndMaximumValues(m_horizontalMinimum[yokeIndex],
                                                               m_horizontalMaximum[yokeIndex]);
                        chartEvent->setEventProcessed();
                        break;
                    case ChartTwoAxisOrientationTypeEnum::VERTICAL:
                        CaretAssertVectorIndex(m_verticalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_verticalMaximum, yokeIndex);
                        chartEvent->setMinimumAndMaximumValues(m_verticalMinimum[yokeIndex],
                                                               m_verticalMaximum[yokeIndex]);
                        chartEvent->setEventProcessed();
                        break;
                }
            }
                break;
            case EventChartTwoCartesianOrientedAxesYoking::Mode::GET_YOKED_AXES:
                break;
            case EventChartTwoCartesianOrientedAxesYoking::Mode::SET_MINIMUM_AND_MAXIMUM_VALUES:
            {
                const int32_t yokeIndex(static_cast<int32_t>(chartEvent->getYokingRangeMode()));
                switch (chartEvent->getAxisOrientation()) {
                    case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
                    {
                        CaretAssertVectorIndex(m_horizontalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_horizontalMaximum, yokeIndex);
                        m_horizontalMinimum[yokeIndex] = chartEvent->getMinimumValue();
                        m_horizontalMaximum[yokeIndex] = chartEvent->getMaximumValue();
                        chartEvent->setEventProcessed();
                    }
                        break;
                    case ChartTwoAxisOrientationTypeEnum::VERTICAL:
                    {
                        CaretAssertVectorIndex(m_verticalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_verticalMaximum, yokeIndex);
                        m_verticalMinimum[yokeIndex] = chartEvent->getMinimumValue();
                        m_verticalMaximum[yokeIndex] = chartEvent->getMaximumValue();
                        chartEvent->setEventProcessed();
                    }
                        break;
                }
            }
                break;
            case EventChartTwoCartesianOrientedAxesYoking::Mode::SET_MAXIMUM_VALUE:
            {
                const int32_t yokeIndex(static_cast<int32_t>(chartEvent->getYokingRangeMode()));
                switch (chartEvent->getAxisOrientation()) {
                    case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
                        CaretAssertVectorIndex(m_horizontalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_horizontalMaximum, yokeIndex);
                        m_horizontalMaximum[yokeIndex] = chartEvent->getMaximumValue();
                        if (m_horizontalMinimum[yokeIndex] > m_horizontalMaximum[yokeIndex]) {
                            m_horizontalMinimum[yokeIndex] = m_horizontalMaximum[yokeIndex];
                        }
                        chartEvent->setEventProcessed();
                        break;
                    case ChartTwoAxisOrientationTypeEnum::VERTICAL:
                        CaretAssertVectorIndex(m_verticalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_verticalMaximum, yokeIndex);
                        m_verticalMaximum[yokeIndex] = chartEvent->getMaximumValue();
                        if (m_verticalMinimum[yokeIndex] > m_verticalMaximum[yokeIndex]) {
                            m_verticalMinimum[yokeIndex] = m_verticalMaximum[yokeIndex];
                        }
                        chartEvent->setEventProcessed();
                        break;
                }
            }
               break;
            case EventChartTwoCartesianOrientedAxesYoking::Mode::SET_MINIMUM_VALUE:
            {
                /*
                 * Note: "get" from event is loading into 'this' min/max
                 */
                const int32_t yokeIndex(static_cast<int32_t>(chartEvent->getYokingRangeMode()));
                switch (chartEvent->getAxisOrientation()) {
                    case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
                        CaretAssertVectorIndex(m_horizontalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_horizontalMaximum, yokeIndex);
                        m_horizontalMinimum[yokeIndex] = chartEvent->getMinimumValue();
                        if (m_horizontalMaximum[yokeIndex] < m_horizontalMinimum[yokeIndex]) {
                            m_horizontalMaximum[yokeIndex] = m_horizontalMinimum[yokeIndex];
                        }
                        chartEvent->setEventProcessed();
                        break;
                    case ChartTwoAxisOrientationTypeEnum::VERTICAL:
                        CaretAssertVectorIndex(m_verticalMinimum, yokeIndex);
                        CaretAssertVectorIndex(m_verticalMaximum, yokeIndex);
                        m_verticalMinimum[yokeIndex] = chartEvent->getMinimumValue();
                        if (m_verticalMaximum[yokeIndex] < m_verticalMinimum[yokeIndex]) {
                            m_verticalMaximum[yokeIndex] = m_verticalMinimum[yokeIndex];
                        }
                        chartEvent->setEventProcessed();
                        break;
                }
            }
                break;
        }

        event->setEventProcessed();
    }
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
ChartTwoCartesianOrientedAxesYokingManager::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoCartesianOrientedAxesYokingManager",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
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
ChartTwoCartesianOrientedAxesYokingManager::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

