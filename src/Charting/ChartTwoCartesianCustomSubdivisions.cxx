
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

#define __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_DECLARE__
#include "ChartTwoCartesianCustomSubdivisions.h"
#undef __CHART_TWO_CARTESIAN_CUSTOM_SUBDIVISIONS_DECLARE__

#include "CaretAssert.h"
#include "CaretResult.h"
#include "ChartTwoCartesianCustomSubdivisionsLabel.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoCartesianCustomSubdivisions
 * \brief Custom axis for cartesian charts
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoCartesianCustomSubdivisions::ChartTwoCartesianCustomSubdivisions()
: CaretObject()
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    reset();
}

/**
 * Destructor.
 */
ChartTwoCartesianCustomSubdivisions::~ChartTwoCartesianCustomSubdivisions()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoCartesianCustomSubdivisions::ChartTwoCartesianCustomSubdivisions(const ChartTwoCartesianCustomSubdivisions& obj)
: CaretObject(obj),
SceneableInterface(obj)
{
    this->copyHelperChartTwoCartesianCustomSubdivisions(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoCartesianCustomSubdivisions&
ChartTwoCartesianCustomSubdivisions::operator=(const ChartTwoCartesianCustomSubdivisions& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperChartTwoCartesianCustomSubdivisions(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoCartesianCustomSubdivisions::copyHelperChartTwoCartesianCustomSubdivisions(const ChartTwoCartesianCustomSubdivisions& obj)
{
    m_labels.clear();
    
    for (const auto& label : obj.m_labels) {
        addLabel(new ChartTwoCartesianCustomSubdivisionsLabel(*label));
    }
}

/**
 * @return Number of labels
 */
int32_t
ChartTwoCartesianCustomSubdivisions::getNumberOfLabels() const
{
    return m_labels.size();
}

/**
 * @return Numeric value for label at the given index
 * @param index
 *    Index of label
 */
float
ChartTwoCartesianCustomSubdivisions::getLabelNumericValue(const int32_t index) const
{
    return getLabel(index)->getNumericValue();
}

/**
 * Set the numeric value for the label at the given index
 * @param index
 *    Index of label
 * @param value
 *    New value for label
 */
void
ChartTwoCartesianCustomSubdivisions::setLabelNumericValue(const int32_t index,
                                                          const float value)
{
    getLabel(index)->setNumericValue(value);
}

/**
 * @return Text for label at the given index
 * @param index
 *    Index of label
 */
AString
ChartTwoCartesianCustomSubdivisions::getLabelText(const int32_t index) const
{
    return getLabel(index)->getCustomText();
}

/**
 * Set the text value for the label at the given index
 * @param index
 *    Index of label
 *    @param text
 * New text for label
 */
void
ChartTwoCartesianCustomSubdivisions::setLabelText(const int32_t index,
                                                  const AString& text)
{
    getLabel(index)->setCustomText(text);
}

/**
 * Sort the label's in descending order using the numeric values.
 * @return True if the order of the labels is changed.
 */
bool
ChartTwoCartesianCustomSubdivisions::sortLabelsByNumericValue()
{
    /*
     * Avoid sorting if labels are in descending order
     * using the label's value
     */
    bool needSorting = false;
    const int32_t numLabels = static_cast<int32_t>(m_labels.size());
    if (numLabels > 1) {
        for (int32_t i = 0; i < (numLabels - 1); i++) {
            CaretAssertVectorIndex(m_labels, (i + 1));
            if (m_labels[i + 1]->getNumericValue() > m_labels[i]->getNumericValue()) {
                needSorting = true;
                break;
            }
        }
    }
    
    if ( ! needSorting) {
        return false;
    }
    
    std::sort(m_labels.begin(),
              m_labels.end(),
              [](const std::unique_ptr<ChartTwoCartesianCustomSubdivisionsLabel>& lhs,
                 const std::unique_ptr<ChartTwoCartesianCustomSubdivisionsLabel>& rhs) -> bool
              { return ( ! (lhs->getNumericValue() > rhs->getNumericValue())); } );
    
    return true;
}


/**
 * Add a label.  This instance will take ownership of the given label.
 * Label will be inserted at the bottom (end).  It may be necessary
 * to sort the labels after calling this method.
 * @param label
 *    Label that is added.
 */
void
ChartTwoCartesianCustomSubdivisions::addLabel(ChartTwoCartesianCustomSubdivisionsLabel* label)
{
    CaretAssert(label);
    std::unique_ptr<ChartTwoCartesianCustomSubdivisionsLabel> ptr(label);
    m_labels.push_back(std::move(ptr));
}

/**
 * Reset to default of two labels
 */
void
ChartTwoCartesianCustomSubdivisions::reset()
{
    ChartTwoCartesianCustomSubdivisionsLabel* labelOne = new ChartTwoCartesianCustomSubdivisionsLabel();
    labelOne->setCustomText("1.0");
    labelOne->setNumericValue(1.0);
    addLabel(labelOne);
    
    ChartTwoCartesianCustomSubdivisionsLabel* labelTwo = new ChartTwoCartesianCustomSubdivisionsLabel();
    labelTwo->setCustomText("0.0");
    labelTwo->setNumericValue(0.0);
    addLabel(labelTwo);
}

/**
 * @return Label at the given index
 * @param index
 *    Index of label
 */
ChartTwoCartesianCustomSubdivisionsLabel*
ChartTwoCartesianCustomSubdivisions::getLabel(const int32_t index)
{
    CaretAssertVectorIndex(m_labels, index);
    return m_labels[index].get();
}

/**
 * @return Label at the given index (const method)
 * @param index
 *    Index of label
 */
const ChartTwoCartesianCustomSubdivisionsLabel*
ChartTwoCartesianCustomSubdivisions::getLabel(const int32_t index) const
{
    CaretAssertVectorIndex(m_labels, index);
    return m_labels[index].get();
}

/**
 * @return Label at the given index of -1 if not found
 * @param label
 *    Label for which index is sought
 */
int32_t
ChartTwoCartesianCustomSubdivisions::getIndexOfLabel(const ChartTwoCartesianCustomSubdivisionsLabel* label)
{
    CaretAssert(label);
    const int32_t numLabels = static_cast<int32_t>(m_labels.size());
    for (int32_t i = 0; i < numLabels; i++) {
        CaretAssertVectorIndex(m_labels, i);
        if (m_labels[i].get() == label) {
            return i;
        }
    }
    return -1;
}

/**
 * Insert a lable above (lower index) the given index.  Index 0 is at the top.
 * @param index
 *    Index of label that has label inserted above it (lower index)
 * @return a CaretResult indicating success or error
 */
std::unique_ptr<CaretResult>
ChartTwoCartesianCustomSubdivisions::insertLabelAbove(const int32_t index)
{
    if ( ! isValidLabelIndex(index)) {
        return CaretResult::newInstanceError("Invalid index for inserting label above.");
    }
    
    float value(0.0);
    if (index == 0) {
        CaretAssertVectorIndex(m_labels, index);
        value = m_labels[index]->getNumericValue() + 1.0;
    }
    else {
        CaretAssertVectorIndex(m_labels, index);
        CaretAssertVectorIndex(m_labels, index - 1);
        value = ((m_labels[index]->getNumericValue()
                  + m_labels[index - 1]->getNumericValue())
                 / 2.0);
    }
    
    std::unique_ptr<ChartTwoCartesianCustomSubdivisionsLabel> ptr(new ChartTwoCartesianCustomSubdivisionsLabel());
    ptr->setNumericValue(value);
    ptr->setCustomText(QString::number(value, 'f'));
    m_labels.insert(m_labels.begin() + index,
                    std::move(ptr));
    
    return CaretResult::newInstanceSuccess();
}

/**
 * Insert a lable below (higher index) the given index.  Index 0 is at the top.
 * @param index
 *    Index of label that has label inserted below it (higher index)
 * @return a CaretResult indicating success or error
 */
std::unique_ptr<CaretResult>
ChartTwoCartesianCustomSubdivisions::insertLabelBelow(const int32_t index)
{
    if ( ! isValidLabelIndex(index)) {
        return CaretResult::newInstanceError("Invalid index for inserting label below.");
    }
    
    const int32_t lastIndex(getNumberOfLabels() - 1);
    float value(0.0);
    if (index == lastIndex) {
        CaretAssertVectorIndex(m_labels, index);
        value = m_labels[index]->getNumericValue() - 1.0;
    }
    else {
        CaretAssertVectorIndex(m_labels, index);
        CaretAssertVectorIndex(m_labels, index + 1);
        value = ((m_labels[index]->getNumericValue()
                  + m_labels[index + 1]->getNumericValue())
                 / 2.0);
    }
    
    std::unique_ptr<ChartTwoCartesianCustomSubdivisionsLabel> ptr(new ChartTwoCartesianCustomSubdivisionsLabel());
    ptr->setNumericValue(value);
    ptr->setCustomText(QString::number(value, 'f'));
    m_labels.insert(m_labels.begin() + index + 1,
                    std::move(ptr));
    
    return CaretResult::newInstanceSuccess();
}


/**
 * Remove the given label
 * @param label
 *    Label to remove
 * @return a CaretResult indicating success or error
 */
std::unique_ptr<CaretResult>
ChartTwoCartesianCustomSubdivisions::removeLabel(ChartTwoCartesianCustomSubdivisionsLabel* label)
{
    CaretAssert(label);
    const int32_t index = getIndexOfLabel(label);
    if (index >= 0) {
        return removeLabelAtIndex(index);
    }
    return CaretResult::newInstanceError("Label not found, unable to remove");
}

/**
 * Remove the label at the given index
 * @param index
 *    Index of label to remove
 * @return a CaretResult indicating success or error
 */
std::unique_ptr<CaretResult>
ChartTwoCartesianCustomSubdivisions::removeLabelAtIndex(const int32_t index)
{
    if (m_labels.size() <= 2) {
        return CaretResult::newInstanceError("Cannot remove label (minimum number of labels has been reached)");
    }
    
    if (isValidLabelIndex(index)) {
        m_labels.erase(m_labels.begin() + index);
        return CaretResult::newInstanceSuccess();
    }
    
    return CaretResult::newInstanceError("Invalid index for removing label");
}

/**
 * Get the valid range of values for a spin box containing the label value at the given index
 * @param index
 *    Index of the label
 * @param rangeMinimumOut
 *    Minimum value allowed in spin box
 * @param rangeMaximumOut
 *    Maximum value allowed in spin box
 */
void
ChartTwoCartesianCustomSubdivisions::getRangeForLabelAtIndex(const int32_t index,
                                                             float& rangeMinimumOut,
                                                             float& rangeMaximumout) const
{
    rangeMaximumout = std::numeric_limits<float>::max();
    rangeMinimumOut = -rangeMaximumout;
    
    const int32_t numLabels = getNumberOfLabels();
    if (numLabels <= 1) {
        return;
    }
    
    /*
     * Values are descending starting at index zero
     */
    if (index < (numLabels - 1)) {
        const ChartTwoCartesianCustomSubdivisionsLabel* labelBelow = getLabel(index + 1);
        rangeMinimumOut = labelBelow->getNumericValue();
    }
    if (index > 0) {
        const ChartTwoCartesianCustomSubdivisionsLabel* labelAbove = getLabel(index - 1);
        rangeMaximumout = labelAbove->getNumericValue();
    }
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ChartTwoCartesianCustomSubdivisions::toString() const
{
    return "ChartTwoCartesianCustomSubdivisions";
}

/**
 * @return True if the given label index is valid, else false.
 * @param index
 *    Index of the label.
 */
bool
ChartTwoCartesianCustomSubdivisions::isValidLabelIndex(const int32_t index) const
{
    if ((index >= 0)
        && (index < static_cast<int32_t>(m_labels.size()))) {
        return true;
    }
    return false;
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
ChartTwoCartesianCustomSubdivisions::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "ChartTwoCartesianCustomSubdivisions",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    if ( ! m_labels.empty()) {
        std::vector<SceneClass*> labelsVector;
        for (const auto& label : m_labels) {
            labelsVector.push_back(label->saveToScene(sceneAttributes,
                                                      "LabelInstance"));
        }
        SceneClassArray* sceneClassArray = new SceneClassArray("m_labels",
                                                               labelsVector);
        sceneClass->addChild(sceneClassArray);
    }

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
ChartTwoCartesianCustomSubdivisions::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    m_labels.clear();
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    const SceneClassArray* labelsArray = sceneClass->getClassArray("m_labels");
    if (labelsArray != NULL) {
        const int32_t numLabels = labelsArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numLabels; i++) {
            const SceneClass* labelClass = labelsArray->getClassAtIndex(i);
            ChartTwoCartesianCustomSubdivisionsLabel* label = new ChartTwoCartesianCustomSubdivisionsLabel();
            label->restoreFromScene(sceneAttributes, labelClass);
            addLabel(label);
        }
    }
    
    if (m_labels.size() < 2) {
        reset();
    }
    
    sortLabelsByNumericValue();
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

