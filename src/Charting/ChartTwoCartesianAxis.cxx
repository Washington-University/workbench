
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __CHART_TWO_CARTESIAN_AXIS_DECLARE__
#include "ChartTwoCartesianAxis.h"
#undef __CHART_TWO_CARTESIAN_AXIS_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartScaleAutoRanging.h"
#include "MathFunctions.h"
#include "NumericTextFormatting.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoCartesianAxis 
 * \brief Chart Two Cartesian Axis Attributes.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param axisLocation
 *     Location of the axis (left, right, bottom, top)
 */
ChartTwoCartesianAxis::ChartTwoCartesianAxis(const ChartAxisLocationEnum::Enum axisLocation)
: CaretObject(),
m_axisLocation(axisLocation)
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_userScaleMinimumValue",
                          &m_userScaleMinimumValue);
    m_sceneAssistant->add("m_userScaleMaximumValue",
                          &m_userScaleMaximumValue);
    m_sceneAssistant->add("m_axisLabelsStepValue",
                          &m_axisLabelsStepValue);
    m_sceneAssistant->add("m_userDigitsRightOfDecimal",
                          &m_userDigitsRightOfDecimal);
    
    m_sceneAssistant->add<ChartTwoAxisScaleRangeModeEnum,ChartTwoAxisScaleRangeModeEnum::Enum>("m_scaleRangeMode",
                                                                                               &m_scaleRangeMode);
    m_sceneAssistant->add<ChartAxisUnitsEnum,ChartAxisUnitsEnum::Enum>("m_units",
                                                                       &m_units);
    m_sceneAssistant->add<NumericFormatModeEnum,NumericFormatModeEnum::Enum>("m_userNumericFormat",
                                                                             &m_userNumericFormat);
    m_sceneAssistant->add("m_autoSubdivisionsEnabled",
                          &m_autoSubdivisionsEnabled);
    m_sceneAssistant->add("m_userNumberOfSubdivisions",
                          &m_userNumberOfSubdivisions);
    m_sceneAssistant->add("m_axisTitle",
                          &m_axisTitle);
    m_sceneAssistant->add("m_visible",
                          &m_visible);
    m_sceneAssistant->add("m_showTickmarks",
                          &m_showTickmarks);
}

/**
 * Destructor.
 */
ChartTwoCartesianAxis::~ChartTwoCartesianAxis()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoCartesianAxis::ChartTwoCartesianAxis(const ChartTwoCartesianAxis& obj)
: CaretObject(obj),
m_axisLocation(obj.m_axisLocation)
{
    this->copyHelperChartTwoCartesianAxis(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoCartesianAxis&
ChartTwoCartesianAxis::operator=(const ChartTwoCartesianAxis& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartTwoCartesianAxis(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoCartesianAxis::copyHelperChartTwoCartesianAxis(const ChartTwoCartesianAxis& obj)
{
    CaretAssert(m_axisLocation == obj.m_axisLocation);
    
    m_userScaleMinimumValue = obj.m_userScaleMinimumValue;
    m_userScaleMaximumValue = obj.m_userScaleMaximumValue;
    m_axisLabelsStepValue   = obj.m_axisLabelsStepValue;
    m_userDigitsRightOfDecimal = obj.m_userDigitsRightOfDecimal;
    m_scaleRangeMode        = obj.m_scaleRangeMode;
    m_units                 = obj.m_units;
    m_userNumericFormat         = obj.m_userNumericFormat;
    m_autoSubdivisionsEnabled = obj.m_autoSubdivisionsEnabled;
    m_userNumberOfSubdivisions  = obj.m_userNumberOfSubdivisions;
    m_axisTitle             = obj.m_axisTitle;
    m_visible               = obj.m_visible;
    m_showTickmarks         = obj.m_showTickmarks;
}

/**
 * @return The axis location (left, right, bottom, top)
 */
ChartAxisLocationEnum::Enum
ChartTwoCartesianAxis::getAxisLocation() const
{
    return m_axisLocation;
}

/**
 * @return User's digits right of decimal point.
 */
int32_t
ChartTwoCartesianAxis::getUserDigitsRightOfDecimal() const
{
    return m_userDigitsRightOfDecimal;
}

/**
 * Set user's digits right of decimal point.
 *
 * @param digitsRightOfDecimal
 *    Numer of digits right of decimal point.
 */
void
ChartTwoCartesianAxis::setUserDigitsRightOfDecimal(const int32_t digitsRightOfDecimal)
{
    m_userDigitsRightOfDecimal = digitsRightOfDecimal;
}


/**
 * @return User's number of subdivisions.
 */
int32_t
ChartTwoCartesianAxis::getUserNumberOfSubdivisions() const
{
    return m_userNumberOfSubdivisions;
}

/**
 * Set User's number of subdivisions
 * @param numberOfSubdivisions
 *    New value for Number of subdivisions.
 */
void
ChartTwoCartesianAxis::setUserNumberOfSubdivisions(const int32_t numberOfSubdivisions)
{
    m_userNumberOfSubdivisions = numberOfSubdivisions;
}

/**
 * @return auto subdivisions enabled
 */
bool
ChartTwoCartesianAxis::isAutoSubdivisionsEnabled() const
{
    return m_autoSubdivisionsEnabled;
}

/**
 * Set auto subdivisions enabled
 *
 * @param autoSubdivisionsEnabled
 *    New value for auto subdivisions enabled
 */
void
ChartTwoCartesianAxis::setAutoSubdivisionsEnabled(const bool autoSubdivisionsEnabled)
{
    m_autoSubdivisionsEnabled = autoSubdivisionsEnabled;
}

/**
 * @return User scale's minimum value
 */
float
ChartTwoCartesianAxis::getUserScaleMinimumValue() const
{
    return m_userScaleMinimumValue;
}

/**
 * @return User scale's maximum value
 */
float
ChartTwoCartesianAxis::getUserScaleMaximumValue() const
{
    return m_userScaleMaximumValue;
}

/**
 * Set User scale's maximum value
 * @param userScaleMaximumValue
 *    New value for User scale's maximum value
 */
void
ChartTwoCartesianAxis::setUserScaleMaximumValue(const float userScaleMaximumValue)
{
    m_userScaleMaximumValue = userScaleMaximumValue;
}

/**
 * Set User scale's minimum value
 * @param userScaleMinimumValue
 *    New value for User scale's minimum value
 */
void
ChartTwoCartesianAxis::setUserScaleMinimumValue(const float userScaleMinimumValue)
{
    m_userScaleMinimumValue = userScaleMinimumValue;
}

/**
 * @return Is the axis visible
 */
bool
ChartTwoCartesianAxis::isVisible() const
{
    return m_visible;
}

/**
 * Set visibility for the axis
 *
 * @param visible
 *    New value for axis visibility
 */
void
ChartTwoCartesianAxis::setVisible(const bool visible)
{
    m_visible = visible;
}

/**
 * @return Show axis tickmarks
 */
bool
ChartTwoCartesianAxis::isShowTickmarks() const
{
    return m_showTickmarks;
}

/**
 * Set Show axis tickmarks
 * @param showTickmarks
 *    New value for Show axis tickmarks
 */
void
ChartTwoCartesianAxis::setShowTickmarks(const bool showTickmarks)
{
    m_showTickmarks = showTickmarks;
}

/**
 * @return Scale Range Mode
 */
ChartTwoAxisScaleRangeModeEnum::Enum
ChartTwoCartesianAxis::getScaleRangeMode() const
{
    return m_scaleRangeMode;
}

/**
 * Set Scale Range Mode
 *
 * @param scaleRangeMode
 *    New value for Scale Range Mode
 */
void
ChartTwoCartesianAxis::setScaleRangeMode(const ChartTwoAxisScaleRangeModeEnum::Enum scaleRangeMode)
{
    m_scaleRangeMode = scaleRangeMode;
}

/**
 * @return Axis units
 */
ChartAxisUnitsEnum::Enum
ChartTwoCartesianAxis::getUnits() const
{
    return m_units;
}

/**
 * Set Axis units
 *
 * @param units
 *    New value for Axis units
 */
void
ChartTwoCartesianAxis::setUnits(const ChartAxisUnitsEnum::Enum units)
{
    m_units = units;
}

/**
 * @return User's umeric format mode
 */
NumericFormatModeEnum::Enum
ChartTwoCartesianAxis::getUserNumericFormat() const
{
    return m_userNumericFormat;
}

/**
 * Set User's Numeric format mode
 *
 * @param numericFormat
 *    New value for Numeric format mode
 */
void
ChartTwoCartesianAxis::setUserNumericFormat(const NumericFormatModeEnum::Enum numericFormat)
{
    m_userNumericFormat = numericFormat;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoCartesianAxis::toString() const
{
    return "ChartTwoCartesianAxis";
}

/**
 * @return Title for axis
 */
AString
ChartTwoCartesianAxis::getAxisTitle() const
{
    return m_axisTitle;
}

/**
 * Set title for axis
 *
 * @param axisTitle
 *    New value for axis title
 */
void
ChartTwoCartesianAxis::setAxisTitle(const AString& axisTitle)
{
    m_axisTitle = axisTitle;
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
ChartTwoCartesianAxis::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoCartesianAxis",
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
ChartTwoCartesianAxis::restoreFromScene(const SceneAttributes* sceneAttributes,
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

/**
 * Given the bounds of the data, determine the auto range minimum and maximum values.
 *
 * @param dataBounds
 *     Bounds of the data.
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
ChartTwoCartesianAxis::getAutoRangeMinimumAndMaximum(const float dataBounds[4],
                                                     float& minimumOut,
                                                     float& maximumOut,
                                                     float& stepValueOut,
                                                     int32_t& digitsRightOfDecimalOut) const
{
    float minValue =  1.0;
    float maxValue = -1.0;
    
    switch (m_axisLocation) {
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
            minValue = dataBounds[0];
            maxValue = dataBounds[1];
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
            minValue = dataBounds[2];
            maxValue = dataBounds[3];
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
            minValue = dataBounds[2];
            maxValue = dataBounds[3];
            break;
        case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
            minValue = dataBounds[0];
            maxValue = dataBounds[1];
            break;
    }
    
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
 * @param dataBoundsIn
 *     Bounds of data [minX, maxX, minY, maxY].
 * @param axisLengthInPixels
 *     Length of axis in pixels.
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
ChartTwoCartesianAxis::getScaleValuesAndOffsets(const float dataBoundsIn[4],
                                                const float axisLengthInPixels,
                                                float& minimumOut,
                                                float& maximumOut,
                                                std::vector<float>& scaleValuesOffsetInPixelsOut,
                                                std::vector<AString>& scaleValuesOut) const
{
    float dataBounds[4] = {
        dataBoundsIn[0],
        dataBoundsIn[1],
        dataBoundsIn[2],
        dataBoundsIn[3],
    };
    
    minimumOut = 0.0;
    maximumOut = 0.0;
    scaleValuesOffsetInPixelsOut.clear();
    scaleValuesOut.clear();
    
    if (axisLengthInPixels < 25.0) {
        return false;
    }
    
    float labelsStart = 0.0;
    float labelsEnd   = 0.0;
    float labelsStep  = 1.0;
    int32_t labelsDigitsRightOfDecimal = 0;
    
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_AUTO:
            break;
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_USER:
            switch (m_axisLocation) {
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                    dataBounds[0] = m_userScaleMinimumValue;
                    dataBounds[1] = m_userScaleMaximumValue;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                    dataBounds[2] = m_userScaleMinimumValue;
                    dataBounds[3] = m_userScaleMaximumValue;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                    dataBounds[2] = m_userScaleMinimumValue;
                    dataBounds[3] = m_userScaleMaximumValue;
                    break;
                case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                    dataBounds[0] = m_userScaleMinimumValue;
                    dataBounds[1] = m_userScaleMaximumValue;
                    break;
            }
            break;
    }

    if ( ! getAutoRangeMinimumAndMaximum(dataBounds,
                                         labelsStart,
                                         labelsEnd,
                                         labelsStep,
                                         labelsDigitsRightOfDecimal)) {
        return false;
    }
    
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_AUTO:
            break;
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_USER:
            labelsStart = m_userScaleMinimumValue;
            labelsEnd   = m_userScaleMaximumValue;
            break;
    }
    
    if ( ! m_autoSubdivisionsEnabled) {
        const float labelsRange = labelsEnd - labelsStart;
        if (labelsRange <= 0.0) {
            return false;
        }
        const float dividend = (1.0 + m_userNumberOfSubdivisions);
        labelsStep = labelsRange / dividend;
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
            const float labelPixelsPosition = axisLengthInPixels * labelParametricValue;
            labelNumericValues.push_back(labelValueForText);
            scaleValuesOffsetInPixelsOut.push_back(labelPixelsPosition);
            
//            const AString labelText = AString::number(labelValueForText, 'f', labelsDigitsRightOfDecimal);
//            labelTextOut.push_back(labelText);
        }
        else {
            //            std::cout << "Label value=" << labelValue << " parametric=" << labelParametricValue << " failed." << std::endl;
        }
        
        labelValue  += tickLabelsStep;
    }
    
    const int32_t numValues = static_cast<int32_t>(labelNumericValues.size());
    if (numValues > 0) {
        scaleValuesOut.resize(numValues);
        NumericTextFormatting::formatValueRange(m_userNumericFormat,
                                                m_userDigitsRightOfDecimal,
                                                &labelNumericValues[0],
                                                &scaleValuesOut[0],
                                                labelNumericValues.size());
    }
    
    CaretAssert(scaleValuesOffsetInPixelsOut.size() == scaleValuesOut.size());
    return ( ! scaleValuesOut.empty());
}

