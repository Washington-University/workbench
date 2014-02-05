
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

#define __CHART_AXIS_DECLARE__
#include "ChartAxis.h"
#undef __CHART_AXIS_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartAxis 
 * \brief Contains information about a chart axis.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartAxis::ChartAxis(const Axis axis)
: CaretObject(),
SceneableInterface(),
m_axis(axis)
{
    initializeMembersChartAxis();
}

/**
 * Destructor.
 */
ChartAxis::~ChartAxis()
{
    delete m_sceneAssistant;
}

/**
 * Initialize members of a new instance.
 */
void
ChartAxis::initializeMembersChartAxis()
{
    m_autoRangeScale = true;
    m_axisUnits      = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
    m_labelFontSize  = 12;
    m_visible        = false;
    m_minimumValue   = 0.0;
    m_maximumValue   = 0.0;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_autoRangeScale", &m_autoRangeScale);
    m_sceneAssistant->add("m_labelFontSize", &m_labelFontSize);
    m_sceneAssistant->add("m_visible", &m_visible);
    m_sceneAssistant->add("m_minimumValue", &m_minimumValue);
    m_sceneAssistant->add("m_maximumValue", &m_maximumValue);
    m_sceneAssistant->add("m_text", &m_text);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartAxis::ChartAxis(const ChartAxis& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    initializeMembersChartAxis();
    copyHelperChartAxis(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
ChartAxis&
ChartAxis::operator=(const ChartAxis& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartAxis(obj);
    }
    return *this;
    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
ChartAxis::copyHelperChartAxis(const ChartAxis& obj)
{
    m_axis           = obj.m_axis;
    m_text           = obj.m_text;
    m_axisUnits      = obj.m_axisUnits;
    m_maximumValue   = obj.m_maximumValue;
    m_minimumValue   = obj.m_minimumValue;
    m_labelFontSize  = obj.m_labelFontSize;
    m_visible        = obj.m_visible;
    m_autoRangeScale = obj.m_autoRangeScale;
}


/**
 * @return The location of the axis.
 */
ChartAxis::Axis
ChartAxis::getAxis() const
{
    return m_axis;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartAxis::toString() const
{
    return "ChartAxis";
}

/**
 * @return Text for axis label.
 */
AString
ChartAxis::getText() const
{
    return m_text;
}

/**
 * Set text for axis label.
 *
 * @param text
 *   New text for label.
 */
void
ChartAxis::setText(const AString& text)
{
    m_text = text;
}

/**
 * @return Axis Units.
 */
ChartAxisUnitsEnum::Enum
ChartAxis::getAxisUnits() const
{
    return m_axisUnits;
}

/**
 * Set the units for the axis.
 * 
 * @param axisUnits
 *    New value for axis units.
 */
void
ChartAxis::setAxisUnits(const ChartAxisUnitsEnum::Enum axisUnits)
{
    m_axisUnits = axisUnits;
}


/**
 * @return Minimum value for axis.
 */
float
ChartAxis::getMinimumValue() const
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
ChartAxis::setMinimumValue(const float minimumValue)
{
    m_minimumValue = minimumValue;
}

/**
 * @return Maximum value for axis.
 */
float
ChartAxis::getMaximumValue() const
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
ChartAxis::setMaximumValue(const float maximumValue)
{
    m_maximumValue = maximumValue;
}

/**
 * @return True if minimum/maximum values for axis are valid.
 */
bool
ChartAxis::isMinimumMaximumValueValid()
{
    if (m_maximumValue > m_minimumValue) {
        return true;
    }
    
    return false;
}

/**
 * @return Font size for the label's text.
 */
int32_t
ChartAxis::getLabelFontSize() const
{
    return m_labelFontSize;
}

/**
 * Set font size for label's text.
 *
 * @param fontSize
 *    New value for font size.
 */
void
ChartAxis::setLabelFontSize(const float fontSize)
{
    m_labelFontSize = fontSize;
}

/**
 * @return True if this axis should be displayed.
 */
bool
ChartAxis::isVisible() const
{
    return m_visible;
}

/**
 * Set this axis should be displayed.
 *
 * @param visible
 *     True if displayed, else false.
 */
void
ChartAxis::setVisible(const bool visible)
{
    m_visible = visible;
}

/**
 * Is auto range scale selected (scale matches data)
 */
bool
ChartAxis::isAutoRangeScale() const
{
    return m_autoRangeScale;
}

/**
 * Set auto range scale selected (scale matches data)
 *
 * @param autoRangeScale
 *    New status.
 */
void
ChartAxis::setAutoRangeScale(const bool autoRangeScale)
{
    m_autoRangeScale = autoRangeScale;
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
ChartAxis::saveToScene(const SceneAttributes* sceneAttributes,
                        const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartAxis",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
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
ChartAxis::restoreFromScene(const SceneAttributes* sceneAttributes,
                             const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}




