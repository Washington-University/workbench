
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

#define __FEATURE_LABEL_MODEL_DECLARE__
#include "FeatureLabelModel.h"
#undef __FEATURE_LABEL_MODEL_DECLARE__

#include "CaretAssert.h"
#include "FeatureLabel.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"

using namespace caret;


    
/**
 * \class caret::FeatureLabelModel 
 * \brief Model for a set of labels from a feature property
 * \ingroup Files
 */

/**
 * Constructor.
 * @param description
 *    Description of the property containing the labels
 */
FeatureLabelModel::FeatureLabelModel(const AString& description)
: QStandardItemModel(),
m_description(description)
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
}

/**
 * Destructor.
 */
FeatureLabelModel::~FeatureLabelModel()
{
}

/**
 * @return Description of the model
 */
AString
FeatureLabelModel::getDescription() const
{
    return m_description;
}

/**
 * Add a label to this model
 * @param label
 *    The label.
 */
void
FeatureLabelModel::addLabel(FeatureLabel* label)
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
FeatureLabel*
FeatureLabelModel::getLabelWithValue(const int32_t value)
{
    FeatureLabel* labelOut(NULL);
    
    const auto iter(m_valueToLabelMap.find(value));
    if (iter != m_valueToLabelMap.end()) {
        labelOut = iter->second;
    }
    
    return labelOut;
}

const FeatureLabel*
FeatureLabelModel::getLabelWithValue(const int32_t value) const
{
    FeatureLabel* labelOut(NULL);
    
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
FeatureLabelModel::setAllLabelsDisplayed(const bool displayStatus)
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
FeatureLabelModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FeatureLabelModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    SceneObjectMapIntegerKey* labelsMap(new SceneObjectMapIntegerKey("labelsMap",
                                                                    SceneObjectDataTypeEnum::SCENE_CLASS));
    
    for (const auto& iter : m_valueToLabelMap) {
        const int32_t value(iter.first);
        const AString className("Label_"
                                + AString::number(value));
        labelsMap->addClass(value,
                            iter.second->saveToScene(sceneAttributes, className));
    }
    
    sceneClass->addChild(labelsMap);
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
FeatureLabelModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const SceneObjectMapIntegerKey* labelsMap(sceneClass->getMapIntegerKey("labelsMap"));
    if (labelsMap != NULL) {
        const std::vector<int32_t> allKeys(labelsMap->getKeys());
        for (const int32_t key : allKeys) {
            const SceneClass* sc(labelsMap->classValue(key));
            if (sc != NULL) {
                FeatureLabel* label(getLabelWithValue(key));
                if (label != NULL) {
                    label->restoreFromScene(sceneAttributes,
                                            sc);
                }
            }
        }
    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

