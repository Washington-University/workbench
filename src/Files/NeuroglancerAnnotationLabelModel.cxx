
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __NEUROGLANCER_ANNOTATION_LABEL_MODEL_DECLARE__
#include "NeuroglancerAnnotationLabelModel.h"
#undef __NEUROGLANCER_ANNOTATION_LABEL_MODEL_DECLARE__

#include "CaretAssert.h"
#include "NeuroglancerAnnotationLabel.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::NeuroglancerAnnotationLabelModel 
 * \brief Model for a set of labels from an annotation property
 * \ingroup Files
 */

/**
 * Constructor.
 * @param description
 *    Description of the property containing the labels
 */
NeuroglancerAnnotationLabelModel::NeuroglancerAnnotationLabelModel(const AString& description)
: QStandardItemModel(),
m_description(description)
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
}

/**
 * Destructor.
 */
NeuroglancerAnnotationLabelModel::~NeuroglancerAnnotationLabelModel()
{
}

/**
 * @return Description of the model
 */
AString
NeuroglancerAnnotationLabelModel::getDescription() const
{
    return m_description;
}

/**
 * Add a label to this model
 * @param label
 *    The label.
 */
void
NeuroglancerAnnotationLabelModel::addLabel(NeuroglancerAnnotationLabel* label)
{
    appendRow(label);
    
    m_valueToLabelMap.insert(std::make_pair(label->getValue(),
                                            label));
}

/**
 * @return Label with the given value or NULL if no label with the index
 * @param value
 *    Value of the label
 */
NeuroglancerAnnotationLabel*
NeuroglancerAnnotationLabelModel::getLabelWithValue(const int32_t value)
{
    NeuroglancerAnnotationLabel* labelOut(NULL);
    
    const auto iter(m_valueToLabelMap.find(value));
    if (iter != m_valueToLabelMap.end()) {
        labelOut = iter->second;
    }
    
    return labelOut;
}

const NeuroglancerAnnotationLabel*
NeuroglancerAnnotationLabelModel::getLabelWithValue(const int32_t value) const
{
    NeuroglancerAnnotationLabel* labelOut(NULL);
    
    const auto iter(m_valueToLabelMap.find(value));
    if (iter != m_valueToLabelMap.end()) {
        labelOut = iter->second;
    }
    
    return labelOut;
}

/**
 * Set display status of all labels in this model
 * @param displayStatus
 *    If true, display all.
 */
void
NeuroglancerAnnotationLabelModel::setAllLabelsDisplayed(const bool displayStatus)
{
    const Qt::CheckState checkState(displayStatus
                                    ? Qt::Checked
                                    : Qt::Unchecked);
    const int32_t numLabels(rowCount());
    for (int32_t i = 0; i < numLabels; i++) {
        item(i)->setCheckState(checkState);
    }
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
NeuroglancerAnnotationLabelModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "NeuroglancerAnnotationLabelModel",
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
NeuroglancerAnnotationLabelModel::restoreFromScene(const SceneAttributes* sceneAttributes,
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

