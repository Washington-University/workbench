
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

#define __NEUROGLANCER_PROPERTY_INFO_DECLARE__
#include "NeuroglancerPropertyInfo.h"
#undef __NEUROGLANCER_PROPERTY_INFO_DECLARE__

#include "CaretAssert.h"
#include "DataFileContentInformation.h"
#include "FeatureLabel.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::NeuroglancerPropertyInfo 
 * \brief Information about a neuroglancer property
 * \ingroup Files
 */

/**
 * Constructor.
 * @param dataType
 *    Type of data in property
 * @param description
 *    Description of the property
 * @param uniqueID
 *    Unique ID of the property
 * @param labelModelIndex
 *    Index into label model or negative if data type is not Label
 */
NeuroglancerPropertyInfo::NeuroglancerPropertyInfo(const NeuroglancerPropertyDataTypeEnum::Enum dataType,
                                                   const AString& description,
                                                   const uint64_t uniqueID,
                                                   const int32_t labelModelIndex)
: ComboBoxItemInterface(),
CaretObject(),
m_dataType(dataType),
m_description(description),
m_uniqueID(uniqueID),
m_labelModelIndex(labelModelIndex)
{
    
}

/**
 * Destructor.
 */
NeuroglancerPropertyInfo::~NeuroglancerPropertyInfo()
{
}

/**
 * @return Text for this item displayed in combo box
 * MUST OVERRIDE THIS METHOD
 */
AString
NeuroglancerPropertyInfo::comboBoxText() const
{
    return m_description;
}

/**
 * @return Data type of property
 */
NeuroglancerPropertyDataTypeEnum::Enum
NeuroglancerPropertyInfo::getDataType() const
{
    return m_dataType;
}

/**
 * @return Description of property
 */
AString
NeuroglancerPropertyInfo::getDescription() const
{
    return m_description;
}

/**
 * @return Unique ID of property
 */
uint64_t
NeuroglancerPropertyInfo::getUniqueID() const
{
    return m_uniqueID;
}

/**
 * @return Index of label model or -1 if not a label data type
 */
int32_t
NeuroglancerPropertyInfo::getLabelModelIndex() const
{
    return m_labelModelIndex;
}

/**
 * @return True if this property contains label data
 */
bool
NeuroglancerPropertyInfo::isLabels() const
{
    return (m_labelModel != NULL);
}

/**
 * @return Label model for this property.
 * This should not be called if the property is not a label.
 */
FeatureLabelModel*
NeuroglancerPropertyInfo::getLabelModel() const
{
    CaretAssert(m_labelModel);
    return m_labelModel.get();
}

/**
 * Set the label model
 * @param labelModel
 *   Label model for this property (may be NULL)
 */
void
NeuroglancerPropertyInfo::setLabelModel(FeatureLabelModel* labelModel)
{
    m_labelModel.reset(labelModel);
}

/**
 * @return Index of this property in the neuroglancer annotation file
 */
int32_t
NeuroglancerPropertyInfo::getPropertyIndex() const
{
    return m_propertyIndex;
}

/**
 * Set the index of this property in the neuroglancer annotation file
 * @param index
 *     Index of the property
 */
void
NeuroglancerPropertyInfo::setPropertyIndex(const int32_t index)
{
    m_propertyIndex = index;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
NeuroglancerPropertyInfo::toString() const
{
    return ("m_dataType=" + NeuroglancerPropertyDataTypeEnum::toName(m_dataType)
            + ", m_description=" + m_description
            + ", m_labelModelIndex=" + AString::number(m_labelModelIndex)
            + ", m_uniqueID=" + AString::number(m_uniqueID));
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
NeuroglancerPropertyInfo::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const
{
    dataFileInformation.addNameAndValue("Property Description",
                                        m_description);
    
    AString typeName(NeuroglancerPropertyDataTypeEnum::toName(m_dataType));
    if (isLabels()) {
        typeName = "Labels";
    }
    dataFileInformation.addNameAndValue("   Type",
                                        typeName);
}

/**
 * @return True if label at given index is enabled
 * @param labelIndex
 *    Index of label
 */
bool
NeuroglancerPropertyInfo::isLabelAtIndexEnabled(const int32_t labelIndex) const
{
    return (m_labelModel->getLabelWithValue(labelIndex)->checkState() == Qt::Checked);
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
NeuroglancerPropertyInfo::saveToScene(const SceneAttributes* sceneAttributes,
                                      const AString& instanceName)
{
    SceneClass* sceneClass(NULL);
    if (isLabels()) {
        sceneClass = new SceneClass(instanceName,
                                    "NeuroglancerPropertyInfo",
                                    1);

        CaretAssert(m_labelModel);
        SceneClass* labelModelSceneClass(m_labelModel->saveToScene(sceneAttributes,
                                                                   "m_labelModel"));
        if (labelModelSceneClass != NULL) {
            sceneClass->addClass(labelModelSceneClass);
        }
    }

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
NeuroglancerPropertyInfo::restoreFromScene(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    const SceneClass* labelModelSceneClass(sceneClass->getClass("m_labelModel"));
    if (labelModelSceneClass != NULL) {
        m_labelModel->restoreFromScene(sceneAttributes,
                                       labelModelSceneClass);
    }
}
