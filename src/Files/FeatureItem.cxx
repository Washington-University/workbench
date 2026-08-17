
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

#define __FEATURE_ITEM__DECLARE__
#include "FeatureItem.h"
#undef __FEATURE_ITEM__DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FeatureItem.h"
#include "FeatureLabel.h"
#include "FeatureLabelModel.h"
#include "FeatureItemModel.h"
#include "FeatureFile.h"
#include "FeaturePropertyValue.h"
#include "SceneClass.h"

using namespace caret;



/**
 * \class caret::FeatureItem
 * \brief Class for an feature that could be point, line, area, etc
 * \ingroup Files
 */

/**
 * Constructor.
 * @param featureType
 *    The type of the feature
 * @param fileNameNoPath
 *    Name of file (no path) from which feature was read
 * @param ijk
 *    IJK(s) for the feature
 * @param color
 *    Color of the feature
 * @param symbolSize
 *    Size of the symbol
 * @param propertieValues
 *    Propertry values for this feature
 */
FeatureItem::FeatureItem(const FeatureItemTypeEnum::Enum featureType,
                                               const AString& fileNameNoPath,
                                               const std::vector<Vector3D>& ijk,
                                               const QColor& color,
                                               const float symbolSize,
                                               const std::vector<const FeaturePropertyValue*>& propertyValues)
: FeatureBase(FeatureBase::BaseType::FEATURE_ITEM),
m_featureType(featureType),
m_fileNameNoPath(fileNameNoPath),
m_ijk(ijk),
m_color(color),
m_symbolSize(symbolSize),
m_propertyValues(propertyValues)
{
    switch (m_featureType) {
        case FeatureItemTypeEnum::INVALID:
            break;
        case FeatureItemTypeEnum::POINT:
            CaretAssert(m_ijk.size() == 1);
            break;
    }
    
    setFlags(Qt::ItemIsSelectable
             | Qt::ItemIsEnabled);
    
    setText(m_fileNameNoPath
            + " - "
            + FeatureItemTypeEnum::toGuiName(m_featureType)
            + " ("
            + AString::fromNumbers(m_ijk[0])
            + ")");
    setCheckable(true);
    setCheckState(Qt::Checked);
    
    QPixmap pixmap(12, 12);
    pixmap.fill(m_color);
    setIcon(pixmap);
}

/**
 * Destructor.
 */
FeatureItem::~FeatureItem()
{
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
FeatureItem::copyHelperFeatureItem(const FeatureItem& obj)
{
    m_featureType = obj.m_featureType;
    m_fileNameNoPath = obj.m_fileNameNoPath;
    m_ijk            = obj.m_ijk;
    m_color          = obj.m_color;
    m_symbolSize     = obj.m_symbolSize;
    m_propertyValues = obj.m_propertyValues;
}

/**
 * @return The feature type
 */
FeatureItemTypeEnum::Enum
FeatureItem::getType() const
{
    return m_featureType;
}

/**
 * @return True if the feature is displayed
 */
bool
FeatureItem::isDisplayed() const
{
    /*
     * Is feature checkbox off
     */
    if (checkState() != Qt::Checked) {
        return false;
    }
    
    /*
     * Are there any labels with their checkbox off?
     */
    for (const FeaturePropertyValue* propertyValue  : m_propertyValues) {
        const FeatureLabelModel* labelModel(propertyValue->getLabelModel());
        if (labelModel != NULL) {
            const FeatureLabel* label(labelModel->getLabelWithValue(propertyValue->getValue().toInt()));
            if (label != NULL) {
                if (label->checkState() != Qt::Checked) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

/**
 * @return Name of file without path from which feature was read
 */
AString
FeatureItem::getFileNameNoPath() const
{
    return m_fileNameNoPath;
}

/**
 * @return Number of IJK in the feature
 */
int32_t
FeatureItem::getNumberOfIJK() const
{
    return m_ijk.size();
}

/**
 * @return The color
 */
const QColor&
FeatureItem::getColor() const
{
    return m_color;
}

/**
 * @return IJK at the given index
 * @param index
 *    The index
 */
const Vector3D&
FeatureItem::getIJK(const int32_t index) const
{
    CaretAssertVectorIndex(m_ijk, index);
    return m_ijk[index];
}

/**
 * @return The size of the feature
 */
float
FeatureItem::getSymbolSize() const
{
    return m_symbolSize;
}

/**
 * @return The name of the type
 */
AString
FeatureItem::getTypeName() const
{
    return FeatureItemTypeEnum::toGuiName(m_featureType);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
FeatureItem::toString() const
{
    AString ijkString;
    for (int32_t i = 0; i < getNumberOfIJK(); i++) {
        if (i > 0) {
            ijkString += ", ";
        }
        ijkString += "(" + AString::fromNumbers(m_ijk[i]) + ")";
    }
    AString txt("type=" + getTypeName()
                + ", IJK=" + ijkString
                + ", color=" + FeaturePropertyValue::QColorToString(m_color));
    return txt;
}

/**
 * Get identification text for this feature
 * @param idTextOut
 *    Rows of text for display
 * @param featureFile
 *    Feature file containing this feature
 * @param featureIndex
 *    Index of this feature in the feature file
 * @param toolTipFlag
 *    If true, text is for tooltip
 */
void
FeatureItem::getIdentificationText(std::vector<std::vector<AString>>& idTextOut,
                                              const FeatureFile* featureFile,
                                              const int32_t featureIndex,
                                              const bool toolTipFlag) const
{
    idTextOut.clear();
    
    const AString featureName(FeatureItemTypeEnum::toGuiName(getType()));

    if (toolTipFlag) {
        std::vector<AString> rowOne;
        rowOne.push_back(featureName);
        idTextOut.push_back(rowOne);
        
        const int32_t iCoord(0);
        const Vector3D ijk(getIJK(iCoord));
        std::vector<AString> rowTwo;
        rowTwo.push_back("IJK: "
                         + AString::fromNumbers(ijk));
        idTextOut.push_back(rowTwo);
        
        const Vector3D xyz(featureFile->featureIJKtoXYZ(this,
                                                        iCoord));
        std::vector<AString> rowThree;
        rowThree.push_back("XYZ: "
                           + AString::fromNumbers(xyz, ",", 'f', 3));
        idTextOut.push_back(rowThree);
    }
    else {
        std::vector<AString> rowOne;
        rowOne.push_back(featureName);
        idTextOut.push_back(rowOne);
        
        const int32_t numIJK(getNumberOfIJK());
        for (int32_t iCoord = 0; iCoord < numIJK; iCoord++) {
            const AString indexString((iCoord > 0)
                                      ? (" " + AString::number(iCoord+1))
                                      : "");
            const Vector3D ijk(getIJK(iCoord));
            std::vector<AString> rowIJKXYZ;
            rowIJKXYZ.push_back("IJK"
                                + indexString
                                + ": "
                                + AString::fromNumbers(ijk));
            
            if (featureFile != NULL) {
                const Vector3D xyz(featureFile->featureIJKtoXYZ(this,
                                                                iCoord));
                rowIJKXYZ.push_back("XYZ"
                                    + indexString
                                    + ": "
                                    + AString::fromNumbers(xyz, ",", 'f', 3));
            }
            idTextOut.push_back(rowIJKXYZ);
        }
        
        for (const auto& propVal : m_propertyValues) {
            if (propVal->getDataType() == FeaturePropertyDataTypeEnum::LABEL) {
                std::vector<AString> row {
                    propVal->getDescription(),
                    propVal->getLabelText()
                };
                idTextOut.push_back(row);
            }
        }
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
FeatureItem::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                    const AString& instanceName)
{
    SceneClass* sceneClass(new SceneClass(instanceName,
                                          "FeatureItem",
                                          1));
    const bool checkedFlag(checkState() == Qt::Checked);
    sceneClass->addBoolean("checkedFlag",
                           checkedFlag);
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
FeatureItem::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                         const SceneClass* sceneClass)
{
    const bool defaultValue(true);
    const bool checkedFlag(sceneClass->getBooleanValue("checkedFlag",
                                                       defaultValue));
    if (checkedFlag) {
        setCheckState(Qt::Checked);
    }
    else {
        setCheckState(Qt::Unchecked);
    }
}

