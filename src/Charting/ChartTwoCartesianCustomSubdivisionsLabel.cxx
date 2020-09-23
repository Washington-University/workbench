
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

#define __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_LABEL_DECLARE__
#include "ChartTwoCartesianCustomSubdivisionsLabel.h"
#undef __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_LABEL_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoCartesianCustomSubdivisionsLabel
 * \brief Axis label for custom cartesian axis
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoCartesianCustomSubdivisionsLabel::ChartTwoCartesianCustomSubdivisionsLabel()
: CaretObject()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_numericValue",
                          &m_numericValue);
    m_sceneAssistant->add("m_customLabelText",
                          &m_customLabelText);

}

/**
 * Destructor.
 */
ChartTwoCartesianCustomSubdivisionsLabel::~ChartTwoCartesianCustomSubdivisionsLabel()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoCartesianCustomSubdivisionsLabel::ChartTwoCartesianCustomSubdivisionsLabel(const ChartTwoCartesianCustomSubdivisionsLabel& obj)
: CaretObject(obj)
{
    this->copyHelperChartTwoCartesianCustomSubdivisionsLabel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoCartesianCustomSubdivisionsLabel&
ChartTwoCartesianCustomSubdivisionsLabel::operator=(const ChartTwoCartesianCustomSubdivisionsLabel& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartTwoCartesianCustomSubdivisionsLabel(obj);
    }
    return *this;    
}

/**
 * Less than operator for sorting by numeric value
 * @param customAxisLabel
 *    Label for comparison
 * @return True if 'this' is less than given label using numeric value
 */
bool
ChartTwoCartesianCustomSubdivisionsLabel::operator<(const ChartTwoCartesianCustomSubdivisionsLabel& customAxisLabel) const
{
    return (m_numericValue < customAxisLabel.m_numericValue);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoCartesianCustomSubdivisionsLabel::copyHelperChartTwoCartesianCustomSubdivisionsLabel(const ChartTwoCartesianCustomSubdivisionsLabel& obj)
{
    m_numericValue    = obj.m_numericValue;
    m_customLabelText = obj.m_customLabelText;
}

/**
 * @return The numeric value
 */
float
ChartTwoCartesianCustomSubdivisionsLabel::getNumericValue() const
{
    return m_numericValue;
}

/**
 * Set the numeric value
 * @param numericValue
 *    New numeric value
 */
void
ChartTwoCartesianCustomSubdivisionsLabel::setNumericValue(const float numericValue)
{
    m_numericValue = numericValue;
}

/**
 * @return The custom text
 */
AString
ChartTwoCartesianCustomSubdivisionsLabel::getCustomText() const
{
    return m_customLabelText;
}

/**
 * Set the custom text
 * @param customText
 *    New custom text for label
 */
void
ChartTwoCartesianCustomSubdivisionsLabel::setCustomText(const AString& customText)
{
    m_customLabelText = customText;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoCartesianCustomSubdivisionsLabel::toString() const
{
    return "ChartTwoCartesianCustomSubdivisionsLabel";
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
ChartTwoCartesianCustomSubdivisionsLabel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoCartesianCustomSubdivisionsLabel",
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
ChartTwoCartesianCustomSubdivisionsLabel::restoreFromScene(const SceneAttributes* sceneAttributes,
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

