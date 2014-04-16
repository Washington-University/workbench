
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

#define __CHART_AXIS_CARTESIAN_DECLARE__
#include "ChartAxisCartesian.h"
#undef __CHART_AXIS_CARTESIAN_DECLARE__

#include "CaretAssert.h"
#include "ChartModelCartesian.h"
#include "ChartScaleAutoRanging.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartAxisCartesian 
 * \brief Axes for Cartesian Data.
 * \ingroup Charting
 */

/**
 * Constructor.
 *
 * @param axisLocation.
 *    Axis location.
 */
ChartAxisCartesian::ChartAxisCartesian(const ChartAxisLocationEnum::Enum axisLocation)
: ChartAxis(ChartAxisTypeEnum::CHART_AXIS_TYPE_CARTESIAN,
            axisLocation)
{
    initializeMembersChartAxisCartesian();
    
}

/**
 * Destructor.
 */
ChartAxisCartesian::~ChartAxisCartesian()
{
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartAxisCartesian::ChartAxisCartesian(const ChartAxisCartesian& obj)
: ChartAxis(obj)
{
    initializeMembersChartAxisCartesian();
    this->copyHelperChartAxisCartesian(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartAxisCartesian&
ChartAxisCartesian::operator=(const ChartAxisCartesian& obj)
{
    if (this != &obj) {
        ChartAxis::operator=(obj);
        this->copyHelperChartAxisCartesian(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartAxisCartesian::copyHelperChartAxisCartesian(const ChartAxisCartesian& obj)
{
    m_maximumValue   = obj.m_maximumValue;
    m_minimumValue   = obj.m_minimumValue;
    m_digitsRightOfDecimal = obj.m_digitsRightOfDecimal;
    
}

/**
 * Initialize class members.
 */
void
ChartAxisCartesian::initializeMembersChartAxisCartesian()
{
    m_minimumValue   = 0.0;
    m_maximumValue   = 1.0;
    m_digitsRightOfDecimal = 1;
    m_axisLabelsMinimumValue = 0.0;
    m_axisLabelsMaximumValue = 1.0;
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_minimumValue", &m_minimumValue);
    m_sceneAssistant->add("m_maximumValue", &m_maximumValue);
    m_sceneAssistant->add("m_digitsRightOfDecimal", &m_digitsRightOfDecimal);
    m_sceneAssistant->add("m_axisLabelsMinimumValue", &m_axisLabelsMinimumValue);
    m_sceneAssistant->add("m_axisLabelsMaximumValue", &m_axisLabelsMaximumValue);
}

/**
 * @return Minimum value for axis.
 */
float
ChartAxisCartesian::getMinimumValue() const
{
    return m_minimumValue;
}

/**
 * Set minimum value for axis.
 *
 * @param minimumValue
 *    New minimum value for axis.
 */
void
ChartAxisCartesian::setMinimumValue(const float minimumValue)
{
    m_minimumValue = minimumValue;
}

/**
 * @return Maximum value for axis.
 */
float
ChartAxisCartesian::getMaximumValue() const
{
    return m_maximumValue;
}

/**
 * Set maximum value for axis.
 *
 * @param maximumValue
 *    New maximum value for axis.
 */
void
ChartAxisCartesian::setMaximumValue(const float maximumValue)
{
    m_maximumValue = maximumValue;
}

/**
 * Update for auto range scale.
 */
void
ChartAxisCartesian::updateForAutoRangeScale()
{
    if (isAutoRangeScaleEnabled()) {
        const ChartModel* chartModel = getParentChartModel();
        CaretAssert(chartModel);
        const ChartModelCartesian* chartModelCartesian = dynamic_cast<const ChartModelCartesian*>(chartModel);
        
        float minX, maxX, minY, maxY;
        chartModelCartesian->getBounds(minX, maxX, minY, maxY);
        
        float minValue = m_minimumValue;
        float maxValue = m_maximumValue;
        switch (getAxisLocation()) {
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_BOTTOM:
                minValue = minX;
                maxValue = maxX;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT:
                minValue = minY;
                maxValue = maxY;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_RIGHT:
                minValue = minY;
                maxValue = maxY;
                break;
            case ChartAxisLocationEnum::CHART_AXIS_LOCATION_TOP:
                minValue = minX;
                maxValue = maxX;
                break;
        }
        
        m_minimumValue = minValue;
        m_maximumValue = maxValue;
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
    m_digitsRightOfDecimal = digitsRightOfDecimal;

    /**
     * Use auto-scaled range for left and right axis
     */
    if (isAutoRangeScaleEnabled()) {
        switch (getAxisLocation()) {
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
        }
    }
}

/**
 * Get the axis labels and their positions for drawing the scale.
 *
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
ChartAxisCartesian::getLabelsAndPositions(const float axisLengthInPixels,
                                          const float /*fontSizeInPixels*/,
                                          std::vector<float>& labelOffsetInPixelsOut,
                                          std::vector<AString>& labelTextOut)
{
    labelOffsetInPixelsOut.clear();
    labelTextOut.clear();
 
    if (axisLengthInPixels < 25.0) {
        return;
    }
    
    updateForAutoRangeScale();
    
    const float numberOfTicks = 5;
    
    float dataStart = m_minimumValue;
    float dataEnd   = m_maximumValue;
    float dataRange = (m_maximumValue - m_minimumValue);
    if (dataRange <= 0.0) {
        return;
    }
    
    float labelsStart = m_axisLabelsMinimumValue;
    float labelsRange = (m_axisLabelsMaximumValue - m_axisLabelsMinimumValue);
    if (labelsRange <= 0.0) {
        return;
    }
    const float tickLabelsStep = labelsRange / numberOfTicks;
    if (tickLabelsStep <= 0.0) {
        return;
    }
    
    float labelValue  = labelsStart;
    for (int32_t i = 0; i <= numberOfTicks; i++) {
        float labelParametricValue = (labelValue - dataStart) / dataRange;
        
        float labelValueForText = labelValue;
        
        if (dataRange >= 10.0) {
            if (i == 0) {
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
            
            if (i == numberOfTicks) {
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
            const AString labelText = AString::number(labelValueForText, 'f', m_digitsRightOfDecimal);
            
            labelOffsetInPixelsOut.push_back(labelPixelsPosition);
            labelTextOut.push_back(labelText);
        }
        else {
//            std::cout << "Label value=" << labelValue << " parametric=" << labelParametricValue << " failed." << std::endl;
        }
        
        labelValue  += tickLabelsStep;
    }
}

/**
 * At times a copy of chart data will be needed BUT it must be
 * the proper subclass so copy constructor and assignment operator
 * will no function when this abstract, base class is used.  Each
 * subclass will override this method so that the returned class
 * is of the proper type.
 *
 * @return Copy of this instance that is the actual subclass.
 */
ChartAxis*
ChartAxisCartesian::clone() const
{
    ChartAxisCartesian* cloneCopy = new ChartAxisCartesian(*this);
    return cloneCopy;
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
ChartAxisCartesian::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ChartAxisCartesian::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

