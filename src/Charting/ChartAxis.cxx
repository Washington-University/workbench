
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

#define __CHART_AXIS_DECLARE__
#include "ChartAxis.h"
#undef __CHART_AXIS_DECLARE__

#include "CaretAssert.h"
#include "ChartAxisCartesian.h"
#include "ChartScaleAutoRanging.h"
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
 *
 * @param axisType
 *    The axis type.
 * @param axisLocation.
 *    Axis location.
 */
ChartAxis::ChartAxis(const ChartAxisTypeEnum::Enum axisType,
                     const ChartAxisLocationEnum::Enum axisLocation)
: CaretObject(),
SceneableInterface(),
m_axisType(axisType),
m_axisLocation(axisLocation)
{
    initializeMembersChartAxis();
}

/**
 * Create and return an axis of the given type and at the given location.
 *
 * @param axisType
 *     Type of axis.
 * @param axisLocation
 *     Location of axis.
 * @return
 *     Axis that was created.
 */
ChartAxis*
ChartAxis::newChartAxisForTypeAndLocation(const ChartAxisTypeEnum::Enum axisType,
                                          const ChartAxisLocationEnum::Enum axisLocation)
{
    ChartAxis* axis = NULL;
    
    switch (axisType) {
        case ChartAxisTypeEnum::CHART_AXIS_TYPE_CARTESIAN:
            axis = new ChartAxisCartesian(axisLocation);
            break;
        case ChartAxisTypeEnum::CHART_AXIS_TYPE_NONE:
            CaretAssert(0);
            break;
    }
    
    return axis;
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
    m_parentChartModel = NULL;
    m_autoRangeScaleEnabled = true;
    m_axisUnits      = ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE;
    m_labelFontSize  = 12;
    m_visible        = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>("m_axisLocation",
                                                                              &m_axisLocation);
    m_sceneAssistant->add("m_autoRangeScaleEnabled", &m_autoRangeScaleEnabled);
    m_sceneAssistant->add("m_labelFontSize", &m_labelFontSize);
    m_sceneAssistant->add("m_visible", &m_visible);
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
    m_parentChartModel = NULL;
    m_axisType       = obj.m_axisType;
    m_axisLocation   = obj.m_axisLocation;
    m_text           = obj.m_text;
    m_axisUnits      = obj.m_axisUnits;
    m_labelFontSize  = obj.m_labelFontSize;
    m_visible        = obj.m_visible;
    m_autoRangeScaleEnabled = obj.m_autoRangeScaleEnabled;
}

/**
 * @return The type of the axis.
 */
ChartAxisTypeEnum::Enum
ChartAxis::getAxisType() const
{
    return m_axisType;
}

/**
 * @return The location of the axis.
 */
ChartAxisLocationEnum::Enum
ChartAxis::getAxisLocation() const
{
    return m_axisLocation;
}

/**
 * Set the parent chart model.
 *
 * @param parentChartModel
 *    Chart in which this axis is used.
 */
void
ChartAxis::setParentChartModel(ChartModel* parentChartModel)
{
    m_parentChartModel = parentChartModel;
}

/**
 * @return The chart model that uses this axis (may be NULL).
 */
ChartModel*
ChartAxis::getParentChartModel()
{
    return m_parentChartModel;
}

/**
 * @return The chart model that uses this axis (may be NULL).
 */
const ChartModel*
ChartAxis::getParentChartModel() const
{
    return m_parentChartModel;
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
 * Return the suffix for the axis units
 */
AString
ChartAxis::getAxisUnitsSuffix() const
{
    AString suffix;
    
    switch (m_axisUnits) {
        case ChartAxisUnitsEnum::CHART_AXIS_UNITS_NONE:
            break;
        case ChartAxisUnitsEnum::CHART_AXIS_UNITS_TIME_SECONDS:
            suffix = "s";
            break;
    }
    
    return suffix;
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
 * Is auto range scale enabled (scale matches data)
 */
bool
ChartAxis::isAutoRangeScaleEnabled() const
{
    return m_autoRangeScaleEnabled;
}

/**
 * Set auto range scale enabled (scale matches data)
 *
 * @param autoRangeScaleEnabled
 *    New status.
 */
void
ChartAxis::setAutoRangeScaleEnabled(const bool autoRangeScaleEnabled)
{
    m_autoRangeScaleEnabled = autoRangeScaleEnabled;
    
    if (m_autoRangeScaleEnabled) {
        updateForAutoRangeScale();
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
ChartAxis::saveToScene(const SceneAttributes* sceneAttributes,
                        const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartAxis",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
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
ChartAxis::restoreFromScene(const SceneAttributes* sceneAttributes,
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




