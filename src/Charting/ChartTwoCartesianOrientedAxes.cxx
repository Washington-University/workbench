
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

#define __CHART_TWO_CARTESIAN_ORIENTATED_AXES_DECLARE__
#include "ChartTwoCartesianOrientedAxes.h"
#undef __CHART_TWO_CARTESIAN_ORIENTATED_AXES_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartScaleAutoRanging.h"
#include "ChartTwoCartesianAxis.h"
#include "EventChartTwoAxisGetDataRange.h"
#include "EventChartTwoCartesianOrientedAxesYoking.h"
#include "EventManager.h"
#include "MathFunctions.h"
#include "NumericTextFormatting.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoCartesianOrientedAxes
 * \brief Axis orientation that contains two axes
 * \ingroup Charting
 */

/**
 * Constructor.
 * @param orientationType
 *    The orientation type
 */
ChartTwoCartesianOrientedAxes::ChartTwoCartesianOrientedAxes(const ChartTwoOverlaySet* parentChartOverlaySet,
                                                 const ChartTwoAxisOrientationTypeEnum::Enum orientationType)
: CaretObject(),
m_parentChartOverlaySet(parentChartOverlaySet),
m_orientationType(orientationType)
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    switch (m_orientationType) {
        case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
            m_leftOrBottomAxis.reset(new ChartTwoCartesianAxis(m_parentChartOverlaySet,
                                                               ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM));
            m_rightOrTopAxis.reset(new ChartTwoCartesianAxis(m_parentChartOverlaySet,
                                                             ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP));
            break;
        case ChartTwoAxisOrientationTypeEnum::VERTICAL:
            m_leftOrBottomAxis.reset(new ChartTwoCartesianAxis(m_parentChartOverlaySet,
                                                               ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT));
            m_rightOrTopAxis.reset(new ChartTwoCartesianAxis(m_parentChartOverlaySet,
                                                             ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT));
            break;
    }
    
    reset();
    
    m_sceneAssistant->add<ChartTwoAxisScaleRangeModeEnum,ChartTwoAxisScaleRangeModeEnum::Enum>("m_scaleRangeMode",
                                                                                               &m_scaleRangeMode);
    m_sceneAssistant->add("m_userScaleMinimumValue",
                          &m_userScaleMinimumValue);
    m_sceneAssistant->add("m_userScaleMaximumValue",
                          &m_userScaleMaximumValue);
    m_sceneAssistant->add("m_leftOrBottomAxis",
                          "ChartTwoCartesianAxis",
                          m_leftOrBottomAxis.get());
    m_sceneAssistant->add("m_rightOrTopAxis",
                          "ChartTwoCartesianAxis",
                          m_rightOrTopAxis.get());
    m_sceneAssistant->add("m_transformationEnabled",
                          &m_transformationEnabled);
}

/**
 * Destructor.
 */
ChartTwoCartesianOrientedAxes::~ChartTwoCartesianOrientedAxes()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoCartesianOrientedAxes::ChartTwoCartesianOrientedAxes(const ChartTwoCartesianOrientedAxes& obj)
: CaretObject(obj),
SceneableInterface(obj),
m_orientationType(obj.m_orientationType)
{
    this->copyHelperChartTwoCartesianOrientedAxes(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoCartesianOrientedAxes&
ChartTwoCartesianOrientedAxes::operator=(const ChartTwoCartesianOrientedAxes& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartTwoCartesianOrientedAxes(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoCartesianOrientedAxes::copyHelperChartTwoCartesianOrientedAxes(const ChartTwoCartesianOrientedAxes& obj)
{
    m_scaleRangeMode          = obj.m_scaleRangeMode;
    m_userScaleMinimumValue   = obj.m_userScaleMinimumValue;
    m_userScaleMaximumValue   = obj.m_userScaleMaximumValue;
    *m_leftOrBottomAxis       = *m_leftOrBottomAxis;
    *m_rightOrTopAxis         = *m_rightOrTopAxis;
    m_transformationEnabled   = obj.m_transformationEnabled;
}

/*
 * Reset axis to its defaults
 */
void
ChartTwoCartesianOrientedAxes::reset()
{
    m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::AUTO;
    m_userScaleMinimumValue = -100.0f;
    m_userScaleMaximumValue = 100.0f;
    m_transformationEnabled = false;
    
    m_leftOrBottomAxis->reset();
    m_rightOrTopAxis->reset();
}


/**
 * @return The orientation type
 */
ChartTwoAxisOrientationTypeEnum::Enum
ChartTwoCartesianOrientedAxes::getOrientationType()
{
    return m_orientationType;
}

/**
 * @return Scale Range Mode
 */
ChartTwoAxisScaleRangeModeEnum::Enum
ChartTwoCartesianOrientedAxes::getScaleRangeMode() const
{
    return m_scaleRangeMode;
}

/**
 * Copy the left/bottom and right/top axes
 * @param axes
 *    Axes to copy to 'this'
 */
void
ChartTwoCartesianOrientedAxes::copyAxes(const ChartTwoCartesianOrientedAxes* axes)
{
    *m_leftOrBottomAxis = *axes->m_leftOrBottomAxis;
    *m_rightOrTopAxis   = *axes->m_rightOrTopAxis;
}


/**
 * @return The left axis (vertical orientation) or bottom axis (horizontal orientation)
 */
ChartTwoCartesianAxis*
ChartTwoCartesianOrientedAxes::getLeftOrBottomAxis()
{
    return m_leftOrBottomAxis.get();
}

/**
 * @return The left axis (vertical orientation) or bottom axis (horizontal orientation) const method
 */
const ChartTwoCartesianAxis*
ChartTwoCartesianOrientedAxes::getLeftOrBottomAxis() const
{
    return m_leftOrBottomAxis.get();
}

/**
 * @return The right axis (vertical orientation) or top axis (horizontal orientation)
 */
ChartTwoCartesianAxis*
ChartTwoCartesianOrientedAxes::getRightOrTopAxis()
{
    return m_rightOrTopAxis.get();
}

/**
 * @return The right axis (vertical orientation) or top axis (horizontal orientation), const method
 */
const ChartTwoCartesianAxis*
ChartTwoCartesianOrientedAxes::getRightOrTopAxis() const
{
    return m_rightOrTopAxis.get();
}

void
ChartTwoCartesianOrientedAxes::initializeScaleRangeMode(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode)
{
    m_scaleRangeMode = scaleRangeMode;
}

/**
 * Set Scale Range Mode when called from GUI
 *
 * @param scaleRangeMode
 *    New value for Scale Range Mode
 */
void
ChartTwoCartesianOrientedAxes::setScaleRangeModeFromGUI(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode)
{
    /*
     * If yoking is changing to yoking mode and was non-yoking mode
     */
    if (ChartTwoAxisScaleRangeModeEnum::isYokingRangeMode(scaleRangeMode)) {
        if ( ! ChartTwoAxisScaleRangeModeEnum::isYokingRangeMode(m_scaleRangeMode)) {
            /*
             * See if any axes are yoked to the range (yoking) mode
             */
            std::vector<ChartTwoCartesianOrientedAxes*> yokedAxes = EventChartTwoCartesianOrientedAxesYoking::getYokedAxes(m_orientationType,
                                                                                                                           scaleRangeMode);
            
            m_scaleRangeMode = scaleRangeMode;
            
            /*
             * No other axes yoked, then set yoking min/max in the yoking manager
             */
            if (yokedAxes.empty()) {
                /*
                 * Since no axes yoked, initialize yoked range with "this" range
                 */
                EventChartTwoCartesianOrientedAxesYoking::setMinMaxValues(m_orientationType,
                                                                          m_scaleRangeMode,
                                                                          m_userScaleMinimumValue,
                                                                          m_userScaleMaximumValue);
            }
        }
    }
    m_scaleRangeMode = scaleRangeMode;
    
    updateMinMaxValuesForYoking();
}

/*
 * Get the minimum and maximum data values for this axis
 * @param minimumValueOut
 *   Minimum value out
 * @param maximumValueOut
 *   Maximum value out
 */
void
ChartTwoCartesianOrientedAxes::getDataRange(float& minimumValueOut,
                                            float& maximumValueOut) const
{
    EventChartTwoAxisGetDataRange rangeEvent(m_parentChartOverlaySet,
                                             m_orientationType);
    EventManager::get()->sendEvent(rangeEvent.getPointer());
    
    if ( ! rangeEvent.getMinimumAndMaximumValues(minimumValueOut,
                                                 maximumValueOut)) {
        minimumValueOut = 0.0f;
        maximumValueOut = 0.0f;
    }
}

/**
 * @return User scale's minimum value
 */
float
ChartTwoCartesianOrientedAxes::getUserScaleMinimumValue() const
{
    updateMinMaxValuesForYoking();
    
    return m_userScaleMinimumValue;
}

/**
 * @return User scale's maximum value
 */
float
ChartTwoCartesianOrientedAxes::getUserScaleMaximumValue() const
{
    updateMinMaxValuesForYoking();
    
    return m_userScaleMaximumValue;
}

/**
 * Update the minimum and maximum values if yoking is on
 */
void
ChartTwoCartesianOrientedAxes::updateMinMaxValuesForYoking() const
{
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AUTO:
        {
            float minValue(0.0), maxValue(0.0);
            getDataRange(minValue,
                         maxValue);
            float dummyStepValue(0.0);
            int32_t dummyDigitsRightOfDecimal(0);
            getAutoRangeMinimumAndMaximum(minValue, maxValue,
                                          m_userScaleMinimumValue, m_userScaleMaximumValue,
                                          dummyStepValue, dummyDigitsRightOfDecimal);
        }
            break;
        case ChartTwoAxisScaleRangeModeEnum::DATA:
        {
            getDataRange(m_userScaleMinimumValue,
                         m_userScaleMaximumValue);
        }
            break;
        case ChartTwoAxisScaleRangeModeEnum::USER:
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_A:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_B:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_C:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_D:
        {
            /*
             * When yoked, use min/max that is stored in the yoking manager
             */
            float minValue(0.0), maxValue(0.0);
            EventChartTwoCartesianOrientedAxesYoking::getMinMaxValues(m_orientationType,
                                                                      m_scaleRangeMode,
                                                                      minValue,
                                                                      maxValue);
            m_userScaleMinimumValue = minValue;
            m_userScaleMaximumValue = maxValue;
        }
            break;
    }
}

/**
 * Set User scale's maximum value
 * @param userScaleMaximumValue
 *    New value for User scale's maximum value
 */
void
ChartTwoCartesianOrientedAxes::setUserScaleMaximumValueFromGUI(const float userScaleMaximumValue)
{
    if (ChartTwoAxisScaleRangeModeEnum::isYokingRangeMode(m_scaleRangeMode)) {
        /*
         * When yoked, min/max are stored in yoking manager
         */
        EventChartTwoCartesianOrientedAxesYoking::setMaximumValue(m_orientationType,
                                                                  m_scaleRangeMode,
                                                                  userScaleMaximumValue);
        return;
    }
    
    m_userScaleMaximumValue = userScaleMaximumValue;
    if (m_userScaleMaximumValue < m_userScaleMinimumValue) {
        m_userScaleMinimumValue = m_userScaleMaximumValue;
    }
    updateRangeModeAfterMinimumOrMaximumChanged();
}

/**
 * Set User scale's minimum value
 * @param userScaleMinimumValue
 *    New value for User scale's minimum value
 */
void
ChartTwoCartesianOrientedAxes::setUserScaleMinimumValueFromGUI(const float userScaleMinimumValue)
{
    if (ChartTwoAxisScaleRangeModeEnum::isYokingRangeMode(m_scaleRangeMode)) {
        /*
         * When yoked, min/max are stored in yoking manager
         */
        EventChartTwoCartesianOrientedAxesYoking::setMinimumValue(m_orientationType,
                                                                  m_scaleRangeMode,
                                                                  userScaleMinimumValue);
        return;
    }
    
    m_userScaleMinimumValue = userScaleMinimumValue;
    if (m_userScaleMinimumValue > m_userScaleMaximumValue) {
        m_userScaleMaximumValue = m_userScaleMinimumValue;
    }
    updateRangeModeAfterMinimumOrMaximumChanged();
}

/**
 * Called to possibly change the range scale
 */
void
ChartTwoCartesianOrientedAxes::updateRangeModeAfterMinimumOrMaximumChanged()
{
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AUTO:
            m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::USER;
            break;
        case ChartTwoAxisScaleRangeModeEnum::DATA:
            m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::USER;
            break;
        case ChartTwoAxisScaleRangeModeEnum::USER:
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_A:
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_B:
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_C:
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_D:
            break;
    }
}

/*
 * Reset the user scale range.
 * For AUTO and DATA, no action is taken since they already use the data range.
 * For USER, the range is reset to the data range.
 * For YOKING, the range is set to the minimum and maximum from all axes yoked to the group.
 */
void
ChartTwoCartesianOrientedAxes::resetUserScaleRange()
{
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AUTO:
            break;
        case ChartTwoAxisScaleRangeModeEnum::DATA:
            break;
        case ChartTwoAxisScaleRangeModeEnum::USER:
        {
            /*
             * Reset to range of data
             */
            float minValue(0.0), maxValue(0.0);
            getDataRange(minValue, maxValue);
            m_userScaleMinimumValue = minValue;
            m_userScaleMaximumValue = maxValue;
        }
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_A:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_B:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_C:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_D:
        {
            /*
             * Get range of all yoked charts
             */
            float minValue(0.0), maxValue(0.0);
            if (EventChartTwoCartesianOrientedAxesYoking::getDataRangeMinMaxValues(m_orientationType,
                                                                                   m_scaleRangeMode,
                                                                                   minValue,
                                                                                   maxValue)) {
                /*
                 * Set yoked range
                 */
                EventChartTwoCartesianOrientedAxesYoking::setMinMaxValues(m_orientationType,
                                                                          m_scaleRangeMode,
                                                                          minValue,
                                                                          maxValue);
            }
        }
            break;
    }
}


/**
 * Set the range mode and user scale from an old scene
 * @param scaleRangeMode
 *    Range mode
 * @param minimumValue
 *    Minimum user scale
 * @param maximumValue
 *    Maximum user scale
 */
void
ChartTwoCartesianOrientedAxes::setRangeModeAndUserScaleFromVersionOneScene(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode,
                                                                           const float userScaleMinimumValue,
                                                                           const float userScaleMaximumValue)
{
    /*
     * Note: version one did not have yoking
     */
    m_scaleRangeMode        = scaleRangeMode;
    m_userScaleMinimumValue = userScaleMinimumValue;
    m_userScaleMaximumValue = userScaleMaximumValue;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoCartesianOrientedAxes::toString() const
{
    return "ChartTwoCartesianOrientedAxes";
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
ChartTwoCartesianOrientedAxes::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoCartesianOrientedAxes",
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
ChartTwoCartesianOrientedAxes::restoreFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass)
{
    reset();
    
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * Given the bounds of the data, determine the auto range minimum and maximum values.
 *
 * @param minimumValue
 *     Minimum data value
 * @param maximumValue
 *     Maximum data value
 * @param minimumOut
 *     Output minimum value for autoranging.
 * @param maximumOut
 *     Output maximum value for autoranging.
 * @param stepValueOut
 *     Output step value for scale.
 * @param digitsRightOfDecimalOut
 *     Output with digits right of decimal.
 * @return
 *     True if output values are valid, else false.
 */
bool
ChartTwoCartesianOrientedAxes::getAutoRangeMinimumAndMaximum(const float minimumValue,
                                                             const float maximumValue,
                                                             float& minimumOut,
                                                             float& maximumOut,
                                                             float& stepValueOut,
                                                             int32_t& digitsRightOfDecimalOut) const
{
    float minValue = minimumValue;
    float maxValue = maximumValue;
    
    if (maxValue > minValue) {
        double scaleStep = 0.0;
        double scaleMin  = 0.0;
        double scaleMax  = 0.0;
        int32_t digitsRightOfDecimal = 0;
        
        ChartScaleAutoRanging::createAutoScale(minValue,
                                               maxValue,
                                               scaleMin,
                                               scaleMax,
                                               scaleStep,
                                               digitsRightOfDecimal);
        minimumOut   = scaleMin;
        maximumOut   = scaleMax;
        stepValueOut = scaleStep;
        digitsRightOfDecimalOut   = digitsRightOfDecimal;
        
        return true;
    }
    
    return false;
}

/**
 * Get the axis scale text values and their positions for drawing the scale.
 *
 * @param minimumDataValue
 *     Minimum data value
 * @param maximumDataValue
 *     Maximum data value
 * @param axisLength
 *     Length of axis (no specific unit type is assumed)
 * @param minimumOut
 *     Output minimum value for autoranging.
 * @param maximumOut
 *     Output maximum value for autoranging.
 * @param scaleValuesOffsetInPixelsOut
 *     Output containing offset in pixels for the scale values.
 * @param scaleValuesOut
 *     Output containing text for scale values.
 * @return
 *     True if output data is valid, else false.
 */
bool
ChartTwoCartesianOrientedAxes::getScaleValuesAndOffsets(const ChartTwoCartesianAxis* cartesianAxis,
                                                        const float minimumDataValue,
                                                        const float maximumDataValue,
                                                        const float axisLength,
                                                        float& minimumOut,
                                                        float& maximumOut,
                                                        std::vector<float>& scaleValuesOffsetInPixelsOut,
                                                        std::vector<AString>& scaleValuesOut) const
{
    CaretAssert(cartesianAxis);
    
    float minimumValue = minimumDataValue;
    float maximumValue = maximumDataValue;
    
    minimumOut = 0.0;
    maximumOut = 0.0;
    scaleValuesOffsetInPixelsOut.clear();
    scaleValuesOut.clear();
    
    if (axisLength <= 0.0) {
        CaretAssert(0);
        return false;
    }
    
    float labelsStart = 0.0;
    float labelsEnd   = 0.0;
    float labelsStep  = 1.0;
    int32_t labelsDigitsRightOfDecimal = 0;
    
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AUTO:
            break;
        case ChartTwoAxisScaleRangeModeEnum::DATA:
            break;
        case ChartTwoAxisScaleRangeModeEnum::USER:
            minimumValue = m_userScaleMinimumValue;
            maximumValue = m_userScaleMaximumValue;
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_A:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_B:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_C:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_D:
            updateMinMaxValuesForYoking();
            minimumValue = m_userScaleMinimumValue;
            maximumValue = m_userScaleMaximumValue;
            break;
    }
    
    if ( ! getAutoRangeMinimumAndMaximum(minimumValue,
                                         maximumValue,
                                         labelsStart,
                                         labelsEnd,
                                         labelsStep,
                                         labelsDigitsRightOfDecimal)) {
        return false;
    }
    
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AUTO:
            m_userScaleMinimumValue = labelsStart;
            m_userScaleMaximumValue = labelsEnd;
            break;
        case ChartTwoAxisScaleRangeModeEnum::DATA:
        {
            const double range = maximumDataValue - minimumDataValue;
            if (range > 0.0) {
                const int32_t numSteps = MathFunctions::round((labelsEnd - labelsStart) / labelsStep);
                if (numSteps > 0) {
                    labelsStart = minimumDataValue;
                    labelsEnd   = maximumDataValue;
                    labelsStep  = range / numSteps;
                    m_userScaleMinimumValue = labelsStart;
                    m_userScaleMaximumValue = labelsEnd;
                }
            }
        }
            break;
        case ChartTwoAxisScaleRangeModeEnum::USER:
        {
            const double range = m_userScaleMaximumValue - m_userScaleMinimumValue;
            if (range > 0.0) {
                const int32_t numSteps = MathFunctions::round((labelsEnd - labelsStart) / labelsStep);
                if (numSteps > 0) {
                    labelsStart = m_userScaleMinimumValue;
                    labelsEnd   = m_userScaleMaximumValue;
                    labelsStep  = range / numSteps;
                    m_userScaleMinimumValue = labelsStart;
                    m_userScaleMaximumValue = labelsEnd;
                }
            }
        }
            break;
        case ChartTwoAxisScaleRangeModeEnum::YOKE_A:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_B:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_C:
        case ChartTwoAxisScaleRangeModeEnum::YOKE_D:
        {
            const double range = m_userScaleMaximumValue - m_userScaleMinimumValue;
            if (range > 0.0) {
                const int32_t numSteps = MathFunctions::round((labelsEnd - labelsStart) / labelsStep);
                if (numSteps > 0) {
                    labelsStart = m_userScaleMinimumValue;
                    labelsEnd   = m_userScaleMaximumValue;
                    labelsStep  = range / numSteps;
                    m_userScaleMinimumValue = labelsStart;
                    m_userScaleMaximumValue = labelsEnd;
                }
            }
        }
            break;
    }
    
    switch (cartesianAxis->getNumericSubdivsionsMode()) {
        case ChartTwoNumericSubdivisionsModeEnum::AUTO:
            break;
        case ChartTwoNumericSubdivisionsModeEnum::USER:
        {
            const float labelsRange = labelsEnd - labelsStart;
            if (labelsRange <= 0.0) {
                return false;
            }
            const float dividend = (1.0 + cartesianAxis->getUserNumberOfSubdivisions());
            labelsStep = labelsRange / dividend;
        }
            break;
    }
    
    minimumOut = labelsStart;
    maximumOut = labelsEnd;
    
    /*
     * If the "labels end" or "labels start" value is not valid (infinity or not-a-number) there
     * are invalid values in the data and will cause the labels processing later
     * in this method to fail.  So, alert the user that there is a problem in
     * the data.
     *
     * A set is used to track those models for which the user has
     * already been alerted.  Otherwise, the alert message will be
     * displayed every time this method is called (which is many) and
     * the user will receive endless pop-ups.
     */
    if ( (! MathFunctions::isNumeric(labelsStart))
        || (! MathFunctions::isNumeric(labelsEnd))) {
        const AString msg("Invalid numbers (infinity or not-a-number) found when trying to create chart.  "
                          "Run \"wb_command -file-information\" on files being charted to find the file "
                          "that contains invalid data so that the file can be fixed.");
        CaretLogWarning(msg);
        return false;
    }
    
    float labelsRange = (labelsEnd - labelsStart);
    if (labelsRange <= 0.0) {
        return false;
    }
    
    const float tickLabelsStep = labelsStep;
    if (tickLabelsStep <= 0.0) {
        return false;
    }
    
    const float onePercentRange = labelsRange * 0.01f;
    
    std::vector<float> labelNumericValues;
    
    float labelValue  = labelsStart;
    while (labelValue <= labelsEnd) {
        float labelParametricValue = (labelValue - labelsStart) / labelsRange;
        
        float labelValueForText = labelValue;
        
        if (labelsRange >= 10.0) {
            /*
             * Is this the first label?
             */
            if (labelValue <= labelsStart) {
                /*
                 * Handles case when the minimum DATA value is just a little
                 * bit greater than the minimum value for axis labels such
                 * as in Data-Series data when the minimum data value is "1"
                 * and the minimum axis label value is "0".  Without this
                 * code no value is displayed at the left edge of the axis.
                 */
                if (labelParametricValue < 0.0) {
                    const float nextParametricValue = ((labelValue + tickLabelsStep) - labelsStart) / labelsRange;
                    if (nextParametricValue > 0.05) {
                        labelParametricValue = 0.0;
                        labelValueForText = labelsStart;
                    }
                }
            }
            
            if (labelParametricValue < 0.0) {
                if (labelParametricValue >= -0.01) {
                    labelParametricValue = 0.0;
                }
            }
            
            /*
             * Is this the last label?
             */
            if (labelValue >= labelsEnd) {
                /*
                 * Like above, ensures a value is displayed at the right
                 * edge of the axis.
                 */
                if (labelParametricValue > 1.0) {
                    const float prevParametricValue = ((labelValue - tickLabelsStep) - labelsStart) / labelsRange;
                    if (prevParametricValue < 0.95) {
                        labelParametricValue = 1.0;
                        labelValueForText = labelsEnd;
                    }
                }
            }
            
            if (labelParametricValue > 1.0) {
                if (labelParametricValue < 1.01) {
                    labelParametricValue = 1.0;
                }
            }
        }
        
        if ((labelParametricValue >= 0.0)
            && (labelParametricValue <= 1.0)) {
            const float labelPixelsPosition = axisLength * labelParametricValue;
            labelNumericValues.push_back(labelValueForText);
            scaleValuesOffsetInPixelsOut.push_back(labelPixelsPosition);
        }
        
        labelValue  += tickLabelsStep;
        
        /*
         * It is possible that 'labelValue' may be slightly greater than 'labelsEnd'
         * for the last label which results in the last label not displayed.
         * So, if the 'labelValue' is slightly greater than 'labelsEnd',
         * limit 'labelValue' so that the label at the end of the data range
         * is displayed.
         *
         * Example: labelValue = 73.9500046
         *          labelsEnd  = 73.9499969
         */
        if (labelValue > labelsEnd) {
            const float diff = labelValue - labelsEnd;
            if (diff < onePercentRange) {
                labelValue = labelsEnd;
            }
        }
    }
    
    const int32_t numValues = static_cast<int32_t>(labelNumericValues.size());
    if (numValues > 0) {
        scaleValuesOut.resize(numValues);
        NumericTextFormatting::formatValueRange(cartesianAxis->getUserNumericFormat(),
                                                cartesianAxis->getUserDigitsRightOfDecimal(),
                                                &labelNumericValues[0],
                                                &scaleValuesOut[0],
                                                labelNumericValues.size());
    }
    
    CaretAssert(scaleValuesOffsetInPixelsOut.size() == scaleValuesOut.size());
    return ( ! scaleValuesOut.empty());
}

/**
 * @return True if transformations are enabled
 */
bool
ChartTwoCartesianOrientedAxes::isTransformationEnabled() const
{
    return m_transformationEnabled;
}

/**
 * Set transformations enabled
 * @param enabled
 *    New enabled status for transformations
 */
void
ChartTwoCartesianOrientedAxes::setTransformationEnabled(const bool enabled)
{
    m_transformationEnabled = enabled;
}

/**
 * Convert the given viewport value to a percentage of the viewport, multiply the
 * percentage of the viewport by the range of the data, and return the data value.
 * @param viewport
 *    The viewport
 * @param viewportValue
 *    Value in viewport space
 * @return Data value multiplied by percentage of viewport
 */
float
ChartTwoCartesianOrientedAxes::getDataPercentageFromPercentageOfViewport(const int32_t viewport[4],
                                                                    const float viewportValue) const
{
    float dataValueOut(0.0);
    
    /*
     * Range of displayed data
     */
    const float dataRange(getUserScaleMaximumValue() - getUserScaleMinimumValue());
    if (dataRange <= 0.0) {
        return dataValueOut;
    }
    
    /*
     * Size of viewport
     */
    float viewportSize(0.0);
    switch (m_orientationType) {
        case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
            viewportSize = viewport[2];
            break;
        case ChartTwoAxisOrientationTypeEnum::VERTICAL:
            viewportSize = viewport[3];
            break;
    }
    /*
     * Percentage of viewport
     */
    const float viewportPercentage((viewportSize > 0)
                                   ? (viewportValue / viewportSize)
                                   : 0.0);

    dataValueOut = (dataRange * viewportPercentage);
    
    return dataValueOut;
}

/**
 * Apply mouse translation to the current chart's axes
 * @param viewport
 *    Viewport containing chart
 * @param mouseDX
 *   The change in mouse X
 * @param mouseDY
 *   The change in mouse Y
 */
void
ChartTwoCartesianOrientedAxes::applyMouseTranslation(const int32_t viewport[4],
                                                     const float mouseDX,
                                                     const float mouseDY)
{
    if ( ! m_transformationEnabled) {
        return;
    }
    
    float mouseDeltaXY(0.0);
    switch (m_orientationType) {
        case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
            mouseDeltaXY = mouseDX;
            break;
        case ChartTwoAxisOrientationTypeEnum::VERTICAL:
            mouseDeltaXY = mouseDY;
            break;
    }
    if (mouseDeltaXY == 0.0) {
        return;
    }
    
    float deltaData = getDataPercentageFromPercentageOfViewport(viewport,
                                                           mouseDeltaXY);
    if (deltaData != 0.0) {
        setUserScaleMinimumValueFromGUI(getUserScaleMinimumValue() - deltaData);
        setUserScaleMaximumValueFromGUI(getUserScaleMaximumValue() - deltaData);
    }
}

/**
 * Apply mouse scaling to the current chart's axes
 * @param viewport
 *    Viewport containing chart
 * @param mouseXY
 *    Position of the mouse along axis
 * @param mouseDY
 *   The change in mouse Y
 */
void
ChartTwoCartesianOrientedAxes::applyMouseScaling(const int32_t viewport[4],
                                                 const float mouseXY,
                                                 const float mouseDY)
{
    if ( ! m_transformationEnabled) {
        return;
    }
    
    if (mouseDY == 0.0) {
        return;
    }
    
    /*
     * Want to scale about the position of the mouse
     */
    float percentMin(1.0);
    switch (m_orientationType) {
        case ChartTwoAxisOrientationTypeEnum::HORIZONTAL:
            percentMin = (mouseXY / viewport[2]);
            break;
        case ChartTwoAxisOrientationTypeEnum::VERTICAL:
            percentMin = (mouseXY / viewport[3]);
            break;
    }
    const float percentMax(1.0 - percentMin);

    const float accelerate(15.0);
    const float deltaData = (accelerate * getDataPercentageFromPercentageOfViewport(viewport,
                                                                                   mouseDY));
    float deltaMin(deltaData * percentMin);
    float deltaMax(deltaData * percentMax);
    const float newMin(getUserScaleMinimumValue() + deltaMin);
    const float newMax(getUserScaleMaximumValue() - deltaMax);
    if (newMax > newMin) {
        setUserScaleMinimumValueFromGUI(newMin);
        setUserScaleMaximumValueFromGUI(newMax);
    }
}

/**
 * @return A percentage of the data's range
 * @param percentage
 *   The percentage ranging [0.0, 100.0]
 */
float
ChartTwoCartesianOrientedAxes::getPercentageOfDataRange(const float percentage) const
{
    float dataMin(0.0), dataMax(0.0);
    getDataRange(dataMin, dataMax);
    const float percent((dataMax - dataMin) * (percentage / 100.0));
    return percent;
}

