
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
#include "ChartTwoCartesianCustomSubdivisions.h"
#include "ChartTwoOverlaySetInterface.h"
#include "EventManager.h"
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
 * @param parentChartOverlaySet
 *     Parent of this axis. (may be NULL)
 * @param axisLocation
 *     Location of the axis (left, right, bottom, top)
 * @param displayGroupAxes
 *     Axes for display groups
 */
ChartTwoCartesianAxis::ChartTwoCartesianAxis(const ChartTwoOverlaySetInterface* parentChartOverlaySetInterface,
                                             const ChartAxisLocationEnum::Enum axisLocation,
                                             std::vector<ChartTwoCartesianAxis*>& displayGroupAxes)
: CaretObject(),
SceneableInterface(),
m_parentChartOverlaySetInterface(parentChartOverlaySetInterface),
m_axisLocation(axisLocation),
m_displayGroupAxes(displayGroupAxes)
{
    m_customSubdivisions.reset(new ChartTwoCartesianCustomSubdivisions());
    
    reset();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                   &m_displayGroup);
    m_sceneAssistant->add("m_displayedByUser",
                          &m_displayedByUser);
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
    m_sceneAssistant->add<CaretUnitsTypeEnum,CaretUnitsTypeEnum::Enum>("m_units",
                                                                       &m_units);
    m_sceneAssistant->add<NumericFormatModeEnum,NumericFormatModeEnum::Enum>("m_userNumericFormat",
                                                                             &m_userNumericFormat);
    m_sceneAssistant->add<ChartTwoNumericSubdivisionsModeEnum,ChartTwoNumericSubdivisionsModeEnum::Enum>("m_numericSubdivsionsMode",
                                                                             &m_numericSubdivsionsMode);
    m_sceneAssistant->add("m_userNumberOfSubdivisions",
                          &m_userNumberOfSubdivisions);
    m_sceneAssistant->add("m_showTickmarks",
                          &m_showTickmarks);
    m_sceneAssistant->add("m_showLabel",
                          &m_showLabel);
    m_sceneAssistant->add("m_titleOverlayIndex",
                          &m_titleOverlayIndex);
    m_sceneAssistant->add("m_labelTextSize",
                          &m_labelTextSize);
    m_sceneAssistant->add("m_numericsTextSize",
                          &m_numericsTextSize);
    m_sceneAssistant->add("m_numericsTextDisplayed",
                          &m_numericsTextDisplayed);
    m_sceneAssistant->add("m_numericsTextRotated",
                          &m_numericsTextRotated);
    m_sceneAssistant->add("m_paddingSize",
                          &m_paddingSize);
    m_sceneAssistant->add("m_customSubdivisions",
                          "ChartTwoCartesianCustomSubdivisions",
                          m_customSubdivisions.get());
    m_sceneAssistant->add<ChartTwoCartesianSubdivisionsModeEnum,ChartTwoCartesianSubdivisionsModeEnum::Enum>("m_subdivisionsMode",
                                                                                                             &m_subdivisionsMode);
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
SceneableInterface(obj),
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
    
    /*
     * Do not copy:
     * m_displayGroup
     */
    m_userScaleMinimumValue     = obj.m_userScaleMinimumValue;
    m_userScaleMaximumValue     = obj.m_userScaleMaximumValue;
    m_axisLabelsStepValue       = obj.m_axisLabelsStepValue;
    m_userDigitsRightOfDecimal  = obj.m_userDigitsRightOfDecimal;
    m_titleOverlayIndex         = obj.m_titleOverlayIndex;
    m_scaleRangeMode            = obj.m_scaleRangeMode;
    m_units                     = obj.m_units;
    m_userNumericFormat         = obj.m_userNumericFormat;
    m_numericSubdivsionsMode    = obj.m_numericSubdivsionsMode;
    m_userNumberOfSubdivisions  = obj.m_userNumberOfSubdivisions;
    m_labelTextSize             = obj.m_labelTextSize;
    m_numericsTextSize          = obj.m_numericsTextSize;
    m_numericsTextDisplayed     = obj.m_numericsTextDisplayed;
    m_numericsTextRotated       = obj.m_numericsTextRotated;
    m_paddingSize               = obj.m_paddingSize;
    m_showTickmarks             = obj.m_showTickmarks;
    m_showLabel                 = obj.m_showLabel;    
    m_displayedByUser           = obj.m_displayedByUser;
    *m_customSubdivisions       = *obj.m_customSubdivisions;
    m_subdivisionsMode          = obj.m_subdivisionsMode;
}

/*
 * Reset axis to its defaults
 */
void
ChartTwoCartesianAxis::reset()
{
    m_displayGroup = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    m_userScaleMinimumValue = -100.0f;
    m_userScaleMaximumValue = 100.0f;
    m_axisLabelsStepValue = 1.0f;
    m_userDigitsRightOfDecimal = 1;
    m_titleOverlayIndex = 0;
    m_scaleRangeMode = ChartTwoAxisScaleRangeModeEnum::AUTO;
    m_units = CaretUnitsTypeEnum::NONE;
    m_userNumericFormat = NumericFormatModeEnum::AUTO;
    m_numericSubdivsionsMode = ChartTwoNumericSubdivisionsModeEnum::AUTO;
    m_userNumberOfSubdivisions = 2;
    m_labelTextSize = 2.5f;
    m_numericsTextSize = 2.5f;
    m_numericsTextDisplayed = true;
    m_numericsTextRotated = false;
    m_paddingSize = 0.0f;
    m_showTickmarks = true;
    m_showLabel = true;
    m_displayedByUser = true;
    m_customSubdivisions->reset();
    m_subdivisionsMode = ChartTwoCartesianSubdivisionsModeEnum::STANDARD;
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
 * @return display the axis selected by user
 */
bool
ChartTwoCartesianAxis::isDisplayedByUser() const
{
    return m_displayedByUser;
}

/**
 * Set display the axis by user
 * @param displayedByUser
 *    New value for display the axis
 */
void
ChartTwoCartesianAxis::setDisplayedByUser(const bool displayedByUser)
{
    m_displayedByUser = displayedByUser;
}

/**
 * @return The display group
 */
DisplayGroupEnum::Enum
ChartTwoCartesianAxis::getDisplayGroup() const
{
    return m_displayGroup;
}

/**
 * Set the display group
 * @param displayGroup
 *    New display group
 */
void
ChartTwoCartesianAxis::setDisplayGroup(const DisplayGroupEnum::Enum displayGroup)
{
    m_displayGroup = displayGroup;
}


/**
 * Get the range of data for this axis.
 *
 * @param rangeMinimumOut
 *     Minimum value allowed.
 * @param rangeMaximumOut
 *     Maximum value allowed.
 */
void
ChartTwoCartesianAxis::getDataRange(float& rangeMinimumOut,
                                    float& rangeMaximumOut) const
{
    CaretAssert(m_parentChartOverlaySetInterface);
    if (m_parentChartOverlaySetInterface != NULL) {
        if ( ! m_parentChartOverlaySetInterface->getDataRangeForAxis(m_axisLocation,
                                                                     rangeMinimumOut,
                                                                     rangeMaximumOut)) {
            rangeMinimumOut = 0.0;
            rangeMaximumOut = 0.0;
        }
    }
}

/**
 * @return User's digits right of decimal point.
 */
int32_t
ChartTwoCartesianAxis::getUserDigitsRightOfDecimal() const
{
    return getDisplayGroupOrThisAxis()->m_userDigitsRightOfDecimal;
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
    getDisplayGroupOrThisAxis()->m_userDigitsRightOfDecimal = digitsRightOfDecimal;
}


/**
 * @return User's number of subdivisions.
 */
int32_t
ChartTwoCartesianAxis::getUserNumberOfSubdivisions() const
{
    return getDisplayGroupOrThisAxis()->m_userNumberOfSubdivisions;
}

/**
 * Set User's number of subdivisions
 * @param numberOfSubdivisions
 *    New value for Number of subdivisions.
 */
void
ChartTwoCartesianAxis::setUserNumberOfSubdivisions(const int32_t numberOfSubdivisions)
{
    getDisplayGroupOrThisAxis()->m_userNumberOfSubdivisions = numberOfSubdivisions;
}

/**
 * @return numeric subdivisions mode
 */
ChartTwoNumericSubdivisionsModeEnum::Enum
ChartTwoCartesianAxis::getNumericSubdivsionsMode() const
{
    return getDisplayGroupOrThisAxis()->m_numericSubdivsionsMode;
}

/**
 * Set numeric subdivisions mode
 *
 * @param numericSubdivsionsMode
 *     New value for numeric subdivisions mode
 */
void
ChartTwoCartesianAxis::setNumericSubdivsionsMode(const ChartTwoNumericSubdivisionsModeEnum::Enum numericSubdivsionsMode)
{
    getDisplayGroupOrThisAxis()->m_numericSubdivsionsMode = numericSubdivsionsMode;
}

/**
 * @return The custom subdivisions
 */
ChartTwoCartesianCustomSubdivisions*
ChartTwoCartesianAxis::getCustomSubdivisions()
{
    return getDisplayGroupOrThisAxis()->m_customSubdivisions.get();
}

/**
 * @return The custom subdivisions (const method)
 */
const ChartTwoCartesianCustomSubdivisions*
ChartTwoCartesianAxis::getCustomSubdivisions() const
{
    return getDisplayGroupOrThisAxis()->m_customSubdivisions.get();
}

/**
 * @return The subdivisions mode
 */
ChartTwoCartesianSubdivisionsModeEnum::Enum
ChartTwoCartesianAxis::getSubdivisionsMode() const
{
    return getDisplayGroupOrThisAxis()->m_subdivisionsMode;
}

/**
 * Set the subdivisions mode
 * @param subdivisionsMode
 *    New subdivisions mode
 */
void
ChartTwoCartesianAxis::setSubdivisionsMode(const ChartTwoCartesianSubdivisionsModeEnum::Enum subdivisionsMode)
{
    getDisplayGroupOrThisAxis()->m_subdivisionsMode = subdivisionsMode;
}

/**
 * @return User scale's minimum value form scene
 */
float
ChartTwoCartesianAxis::getSceneUserScaleMinimumValue() const
{
    return m_userScaleMinimumValue;
}

/**
 * @return User scale's maximum value from scene
 */
float
ChartTwoCartesianAxis::getSceneUserScaleMaximumValue() const
{
    return m_userScaleMaximumValue;
}

/**
 * @return Show axis tickmarks
 */
bool
ChartTwoCartesianAxis::isShowTickmarks() const
{
    return getDisplayGroupOrThisAxis()->m_showTickmarks;
}

/**
 * Set Show axis tickmarks
 * @param showTickmarks
 *    New value for Show axis tickmarks
 */
void
ChartTwoCartesianAxis::setShowTickmarks(const bool showTickmarks)
{
    getDisplayGroupOrThisAxis()->m_showTickmarks = showTickmarks;
}

/**
 * @return show axis label
 */
bool
ChartTwoCartesianAxis::isShowLabel() const
{
    return getDisplayGroupOrThisAxis()->m_showLabel;
}

/**
 * Set show axis label
 * @param showLabel
 *    New value for show axis label
 */
void
ChartTwoCartesianAxis::setShowLabel(const bool showLabel)
{
    getDisplayGroupOrThisAxis()->m_showLabel = showLabel;
}

/**
 * @return Scale Range Mode from Scene
 */
ChartTwoAxisScaleRangeModeEnum::Enum
ChartTwoCartesianAxis::getSceneScaleRangeMode() const
{
    return m_scaleRangeMode;
}

/**
 * @return Axis units
 */
CaretUnitsTypeEnum::Enum
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
ChartTwoCartesianAxis::setUnits(const CaretUnitsTypeEnum::Enum units)
{
    m_units = units;
}

/**
 * @return User's umeric format mode
 */
NumericFormatModeEnum::Enum
ChartTwoCartesianAxis::getUserNumericFormat() const
{
    return getDisplayGroupOrThisAxis()->m_userNumericFormat;
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
    getDisplayGroupOrThisAxis()->m_userNumericFormat = numericFormat;
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
 * @return Index of overlay that supplies the label.
 *
 * @param maximumNumberOfOverlays
 *     Maximum number of allowable overlays.
 */
int32_t
ChartTwoCartesianAxis::getLabelOverlayIndex(const int32_t maximumNumberOfOverlays) const
{
    if (m_titleOverlayIndex < 0) {
        m_titleOverlayIndex = 0;
    }
    else if (m_titleOverlayIndex >= maximumNumberOfOverlays) {
        m_titleOverlayIndex = 0;
    }
    
    return m_titleOverlayIndex;
}

/**
 * Set the index of the overlay that supplies the title.
 * 
 * @param labelOverlayIndex
 *     New value for label overlay index.
 */
void
ChartTwoCartesianAxis::setLabelOverlayIndex(const int32_t labelOverlayIndex)
{
    m_titleOverlayIndex = labelOverlayIndex;
}

/**
 * @return size of label text
 */
float
ChartTwoCartesianAxis::getLabelTextSize() const
{
    return getDisplayGroupOrThisAxis()->m_labelTextSize;
}

/**
 * Set size of label text
 *
 * @param labelTextSize
 *    New value for size of label text
 */
void
ChartTwoCartesianAxis::setLabelTextSize(const float labelTextSize)
{
    getDisplayGroupOrThisAxis()->m_labelTextSize = labelTextSize;
}

/**
 * @return size of numerics text
 */
float
ChartTwoCartesianAxis::getNumericsTextSize() const
{
    return getDisplayGroupOrThisAxis()->m_numericsTextSize;
}

/**
 * Set size of numerics text
 *
 * @param numericsTextSize
 *    New value for size of numerics text
 */
void
ChartTwoCartesianAxis::setNumericsTextSize(const float numericsTextSize)
{
    getDisplayGroupOrThisAxis()->m_numericsTextSize = numericsTextSize;
}

/**
 * @return display numeric text in scale
 */
bool
ChartTwoCartesianAxis::isNumericsTextDisplayed() const
{
    return getDisplayGroupOrThisAxis()->m_numericsTextDisplayed;
}

/**
 * Set display numeric text in scale
 *
 * @param numericsTextDisplayed
 *    New value for display numeric text in scale
 */
void
ChartTwoCartesianAxis::setNumericsTextDisplayed(const bool numericsTextDisplayed)
{
    getDisplayGroupOrThisAxis()->m_numericsTextDisplayed = numericsTextDisplayed;
}

/**
 * @return rotate numeric text
 */
bool
ChartTwoCartesianAxis::isNumericsTextRotated() const
{
    return getDisplayGroupOrThisAxis()->m_numericsTextRotated;
}

/**
 * Set rotate numeric text
 *
 * @param numericsTextRotated
 *    New value for rotate numeric text
 */
void
ChartTwoCartesianAxis::setNumericsTextRotated(const bool numericsTextRotated)
{
    getDisplayGroupOrThisAxis()->m_numericsTextRotated = numericsTextRotated;
}

/**
 * @return size of padding
 */
float
ChartTwoCartesianAxis::getPaddingSize() const
{
    return getDisplayGroupOrThisAxis()->m_paddingSize;
}

/**
 * Set size of padding
 *
 * @param paddingSize
 *    New value for size of padding
 */
void
ChartTwoCartesianAxis::setPaddingSize(const float paddingSize)
{
    getDisplayGroupOrThisAxis()->m_paddingSize = paddingSize;
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
    
    /* may not be in older scenes */
    m_displayGroup          = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    m_displayedByUser       = true;
    m_numericsTextDisplayed = true;
    m_numericsTextRotated   = false;
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
}

/**
 * @return Axis from current display group selection.  When the
 * active display group is 'TAB', 'this' axis is returned.  When a
 * group is selected, the axis for that group's attributes is returned.
 */
ChartTwoCartesianAxis*
ChartTwoCartesianAxis::getDisplayGroupOrThisAxis()
{
    ChartTwoCartesianAxis* axis(this);
    
    /*
     * if parent overlay set interface is NULL, then
     * 'this' is a display group axes
     */
    if (m_parentChartOverlaySetInterface != NULL) {
        switch (m_displayGroup) {
            case DisplayGroupEnum::DISPLAY_GROUP_A:
            case DisplayGroupEnum::DISPLAY_GROUP_B:
            case DisplayGroupEnum::DISPLAY_GROUP_C:
            case DisplayGroupEnum::DISPLAY_GROUP_D:
            {
                const int32_t index(DisplayGroupEnum::toIntegerCode(m_displayGroup));
                CaretAssertVectorIndex(m_displayGroupAxes, index);
                axis = m_displayGroupAxes[index];
            }
                break;
            case DisplayGroupEnum::DISPLAY_GROUP_TAB:
                axis = this;
                break;
        }
    }
    CaretAssert(axis);
    
    return axis;
}

/**
 * @return Axis from current display group selection.  When the
 * active display group is 'TAB', 'this' axis is returned.  When a
 * group is selected, the axis for that group's attributes is returned.
 */
const ChartTwoCartesianAxis*
ChartTwoCartesianAxis::getDisplayGroupOrThisAxis() const
{
    /* klude to avoid duplicating method */
    ChartTwoCartesianAxis* nonConstThis(const_cast<ChartTwoCartesianAxis*>(this));
    return nonConstThis->getDisplayGroupOrThisAxis();
}
