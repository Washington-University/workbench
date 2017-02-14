
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
    m_sceneAssistant->add("m_minimumValue",
                          &m_minimumValue);
    m_sceneAssistant->add("m_maximumValue",
                          &m_maximumValue);
    m_sceneAssistant->add("m_axisLabelsMinimumValue",
                          &m_axisLabelsMinimumValue);
    m_sceneAssistant->add("m_axisLabelsMaximumValue",
                          &m_axisLabelsMaximumValue);
    m_sceneAssistant->add("m_axisLabelsStepValue",
                          &m_axisLabelsStepValue);
    m_sceneAssistant->add("m_axisDigitsRightOfDecimal",
                          &m_axisDigitsRightOfDecimal);
    
    m_sceneAssistant->add<ChartTwoAxisScaleRangeModeEnum,ChartTwoAxisScaleRangeModeEnum::Enum>("m_scaleRangeMode",
                                                                                               &m_scaleRangeMode);
    m_sceneAssistant->add<ChartAxisUnitsEnum,ChartAxisUnitsEnum::Enum>("m_units",
                                                                       &m_units);
    m_sceneAssistant->add<NumericFormatModeEnum,NumericFormatModeEnum::Enum>("m_numericFormat",
                                                                             &m_numericFormat);
    m_sceneAssistant->add("m_numberOfSubdivisions",
                          &m_numberOfSubdivisions);
    m_sceneAssistant->add("m_labelText",
                          &m_labelText);
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
    m_minimumValue          = obj.m_minimumValue;
    m_maximumValue          = obj.m_maximumValue;
    m_axisLabelsMinimumValue          = obj.m_axisLabelsMinimumValue;
    m_axisLabelsMaximumValue          = obj.m_axisLabelsMaximumValue;
    m_axisLabelsStepValue   = obj.m_axisLabelsStepValue;
    m_axisDigitsRightOfDecimal = obj.m_axisDigitsRightOfDecimal;
    m_scaleRangeMode        = obj.m_scaleRangeMode;
    m_units                 = obj.m_units;
    m_numericFormat         = obj.m_numericFormat;
    m_numberOfSubdivisions  = obj.m_numberOfSubdivisions;
    m_labelText             = obj.m_labelText;
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
 * @return Number of subdivisions
 */
int32_t
ChartTwoCartesianAxis::getNumberOfSubdivisions() const
{
    return m_numberOfSubdivisions;
}

/**
 * Set Number of subdivisions
 * @param numberOfSubdivisions
 *    New value for Number of subdivisions
 */
void
ChartTwoCartesianAxis::setNumberOfSubdivisions(const int32_t numberOfSubdivisions)
{
    m_numberOfSubdivisions = numberOfSubdivisions;
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
 * @return The minimum value for the axis.
 */
float
ChartTwoCartesianAxis::getMinimumValue() const
{
    return m_minimumValue;
}

/**
 * @return The maximum value for the axis.
 */
float
ChartTwoCartesianAxis::getMaximumValue() const
{
    return m_maximumValue;
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
 * @return Numeric format mode
 */
NumericFormatModeEnum::Enum
ChartTwoCartesianAxis::getNumericFormat() const
{
    return m_numericFormat;
}

/**
 * Set Numeric format mode
 *
 * @param numericFormat
 *    New value for Numeric format mode
 */
void
ChartTwoCartesianAxis::setNumericFormat(const NumericFormatModeEnum::Enum numericFormat)
{
    m_numericFormat = numericFormat;
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
 * @return Text for axis label
 */
AString
ChartTwoCartesianAxis::getLabelText() const
{
    return m_labelText;
}

/**
 * Set Text for axis label
 *
 * @param labelText
 *    New value for Text for axis label
 */
void
ChartTwoCartesianAxis::setLabelText(const AString& labelText)
{
    m_labelText = labelText;
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
 * Update for auto range scale.
 *
 * @param dataBounds
 *     Bounds of data [minX, maxX, minY, maxY].
 */
void
ChartTwoCartesianAxis::updateForAutoRangeScale(const float dataBounds[4])
{
    float minValue = m_minimumValue;
    float maxValue = m_maximumValue;
    
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_AUTO:
        {
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
            
            m_minimumValue = minValue;
            m_maximumValue = maxValue;
        }
            break;
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_USER:
            break;
    }
    
    double scaleStep = 0.0;
    double scaleMin  = 0.0;
    double scaleMax  = 0.0;
    int32_t digitsRightOfDecimal = 0;
    
    ChartScaleAutoRanging::createAutoScale(m_minimumValue,
                                           m_maximumValue,
                                           scaleMin,
                                           scaleMax,
                                           scaleStep,
                                           digitsRightOfDecimal);
    m_axisLabelsMinimumValue = scaleMin;
    m_axisLabelsMaximumValue = scaleMax;
    m_axisLabelsStepValue    = scaleStep;
    m_axisDigitsRightOfDecimal   = digitsRightOfDecimal;
    
    /**
     * Use auto-scaled range for left and right axis
     */
    switch (m_scaleRangeMode) {
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_AUTO:
        switch (m_axisLocation) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                m_minimumValue = m_axisLabelsMinimumValue;
                m_maximumValue = m_axisLabelsMaximumValue;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                m_minimumValue = m_axisLabelsMinimumValue;
                m_maximumValue = m_axisLabelsMaximumValue;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                break;
            break;
        }
        case ChartTwoAxisScaleRangeModeEnum::AXIS_DATA_RANGE_USER:
            break;
    }
}

/**
 * Get the axis labels and their positions for drawing the scale.
 *
 * @param dataBounds
 *     Bounds of data [minX, maxX, minY, maxY].
 * @param axisLengthInPixels
 *   Length of axis in pixels.
 * @param fontSizeInPixels
 *   Size of the font in pixels.
 * @param labelOffsetInPixelsOut
 *   Output containing offset in pixels for the scale labels.
 * @param labelTextOut
 *   Output containing text for scale labels.
 */
void
ChartTwoCartesianAxis::getLabelsAndPositions(const float dataBounds[4],
                                             const float axisLengthInPixels,
                                             const float /*fontSizeInPixels*/,
                                             std::vector<float>& labelOffsetInPixelsOut,
                                             std::vector<AString>& labelTextOut)
{
    labelOffsetInPixelsOut.clear();
    labelTextOut.clear();
    
    if (axisLengthInPixels < 25.0) {
        return;
    }
    
    updateForAutoRangeScale(dataBounds);
    
    float dataStart = m_minimumValue;
    float dataEnd   = m_maximumValue;
    float dataRange = (m_maximumValue - m_minimumValue);
    if (dataRange <= 0.0) {
        return;
    }
    
    float labelsStart = m_axisLabelsMinimumValue;
    float labelsEnd   = m_axisLabelsMaximumValue;
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
        return;
    }
    
    float labelsRange = (m_axisLabelsMaximumValue - m_axisLabelsMinimumValue);
    if (labelsRange <= 0.0) {
        return;
    }
    
    const float tickLabelsStep = m_axisLabelsStepValue;
    if (tickLabelsStep <= 0.0) {
        return;
    }
    
    
    float labelValue  = labelsStart;
    while (labelValue <= labelsEnd) {
        float labelParametricValue = (labelValue - dataStart) / dataRange;
        
        float labelValueForText = labelValue;
        
        if (dataRange >= 10.0) {
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
                    const float nextParametricValue = ((labelValue + tickLabelsStep) - dataStart) / dataRange;
                    if (nextParametricValue > 0.05) {
                        labelParametricValue = 0.0;
                        labelValueForText = dataStart;
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
                    const float prevParametricValue = ((labelValue - tickLabelsStep) - dataStart) / dataRange;
                    if (prevParametricValue < 0.95) {
                        labelParametricValue = 1.0;
                        labelValueForText = dataEnd;
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
            const AString labelText = AString::number(labelValueForText, 'f', m_axisDigitsRightOfDecimal);
            
            labelOffsetInPixelsOut.push_back(labelPixelsPosition);
            labelTextOut.push_back(labelText);
        }
        else {
            //            std::cout << "Label value=" << labelValue << " parametric=" << labelParametricValue << " failed." << std::endl;
        }
        
        labelValue  += tickLabelsStep;
    }
}
