
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
    m_stepValue      = obj.m_stepValue;
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
    m_stepValue      = 1.0;
    m_digitsRightOfDecimal = 1;
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_minimumValue", &m_minimumValue);
    m_sceneAssistant->add("m_maximumValue", &m_maximumValue);
    m_sceneAssistant->add("m_stepValue", &m_stepValue);
    m_sceneAssistant->add("m_digitsRightOfDecimal", &m_digitsRightOfDecimal);
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
 * @return The step value.
 */
float
ChartAxisCartesian::getStepValue() const
{
    return m_stepValue;
}

/**
 * @return Digits right of decimal for axis.
 */
int32_t
ChartAxisCartesian::getDigitsRightOfDecimal() const
{
    return m_digitsRightOfDecimal;
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
        
        m_minimumValue = scaleMin;
        m_maximumValue = scaleMax;
        m_stepValue    = scaleStep;
        m_digitsRightOfDecimal = digitsRightOfDecimal;
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
    
//    double scaleStep = 0.0;
//    double scaleMin  = 0.0;
//    double scaleMax  = 0.0;
//    int32_t digitsRightOfDecimal = 0;
//    
//    ChartScaleAutoRanging::createAutoScale(m_minimumValue,
//                                           m_maximumValue,
//                                           scaleMin,
//                                           scaleMax,
//                                           scaleStep,
//                                           digitsRightOfDecimal);
    
    const float numberOfTicks = 5;
    
    float dataStart = m_minimumValue;
    float dataRange = (m_maximumValue - m_minimumValue);
    if (isAutoRangeScaleEnabled()) {
//        dataStart = scaleMin;
//        dataRange = scaleMax - scaleMin;
    }
    if (dataRange <= 0.0) {
        return;
    }
    
    const float tickDataStep = dataRange / numberOfTicks;
    
    const float tickPixelStep = axisLengthInPixels / numberOfTicks;
    if (tickPixelStep <= 0.0) {
        return;
    }
    
    float labelOffset = 0.0;
    float labelValue  = dataStart;
    for (int32_t i = 0; i <= numberOfTicks; i++) {
        if (i > 0) {
            
        }
        labelOffsetInPixelsOut.push_back(labelOffset);
        labelTextOut.push_back(AString::number(labelValue, 'f', m_digitsRightOfDecimal));
        
        labelOffset += tickPixelStep;
        labelValue  += tickDataStep;
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

