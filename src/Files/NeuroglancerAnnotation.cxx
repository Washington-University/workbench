
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

#define __NEUROGLANCER_ANNOTATION_DECLARE__
#include "NeuroglancerAnnotation.h"
#undef __NEUROGLANCER_ANNOTATION_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "FeatureLabel.h"
#include "Matrix4x4.h"
#include "NeuroglancerAnnotationFile.h"
#include "NeuroglancerPropertyInfo.h"
#include "RgbaUnsignedByte.h"

using namespace caret;


    
/**
 * \class caret::NeuroglancerAnnotation 
 * \brief Contains one neuroglancer annotation
 * \ingroup Files
 */

/**
 * Constructor.
 * @param uniqueID
 *    UniqueID of this annotation
 * @param ijk
 *    IJK of the annotation
 * @param propertyValue
 *    Property values of this annotation
 */
NeuroglancerAnnotation::NeuroglancerAnnotation(const uint64_t& uniqueID,
                                               const Vector3D& ijk,
                                               const std::vector<int32_t>& propertyValues)
: m_uniqueID(uniqueID),
m_ijk(ijk),
m_xyz(ijk),
m_propertyValues(propertyValues)
{    
    m_rgba.setRgba(255, 0, 0, 255);
}

/**
 * Destructor.
 */
NeuroglancerAnnotation::~NeuroglancerAnnotation()
{
}

/**
 * @return IJK coordinate (from the file) of this annotation
 */
const Vector3D&
NeuroglancerAnnotation::getIJK() const
{
    return m_ijk;
}

/**
 * @return XYZ coordinate of this annotation.
 * Usually IJK * transform
 */
const Vector3D&
NeuroglancerAnnotation::getXYZ() const
{
    return m_xyz;
}

/**
 * Update the XYZ coordinates of this annotation by multiplying the
 * IJK by the given matrix.
 * @param ijkToXyzTransformationMatrix
 *    Transforms IJK to XYZ.
 */
void
NeuroglancerAnnotation::updateXYZ(const Matrix4x4& ijkToXyzTransformationMatrix)
{
    /*
     * It appears that IJK is same as Workbench XYZ.
     * Neuroglancer coordinates are (X, -Z, -Y) and that
     * is what the transform is for.
     */
    const bool ignoreTransformFlag(true);
    if (ignoreTransformFlag) {
        return;
    }
    m_xyz = m_ijk;
    ijkToXyzTransformationMatrix.multiplyPoint3(m_xyz);
}

/**
 * @return the Neuroglancer XYZ which is (IJK * transform).
 * The neuroglancer XYZ appears to be (X, -Z, -Y).
 * @param ijkToXyzTransformationMatrix
 *    Matrix that converts IJK to Neuroglancer XYZl.
 */
Vector3D
NeuroglancerAnnotation::getNeuroglancerXYZ(const Matrix4x4& ijkToXyzTransformationMatrix) const
{
    Vector3D xyz(m_ijk);
    ijkToXyzTransformationMatrix.multiplyPoint3(xyz);
    for (int32_t i = 0; i < 3; i++) {
        /*
         * Neuroglancer seems to round "towards zero"
         */
        float v(xyz[i]);
        if (v > 0.0) {
            v = std::floor(v);
        }
        else if (v < 0.0) {
            v = std::ceil(v);
        }
        xyz[i] = v;
    }
    return xyz;
}

/**
 * @return Unique ID for this annotation
 */
uint64_t
NeuroglancerAnnotation::getUniqueID() const
{
    return m_uniqueID;
}

/**
 * @return Unique ID for this annotation as a string
 */
AString
NeuroglancerAnnotation::getUniqueIdAsString() const
{
    return AString::number(m_uniqueID);
}

/**
 * Set the unique ID
 * @param uniqueID
 *    New value for unique ID
 */
void
NeuroglancerAnnotation::setUniqueID(const uint64_t& uniqueID)
{
    m_uniqueID = uniqueID;
}

/**
 * @return Number of properties
 */
int32_t
NeuroglancerAnnotation::getNumberOfProperties() const
{
    return m_propertyValues.size();
}

/**
 * @return Proprerty at given index as a 32-bit signed integer
 * @param propertyIndex
 *    Index of the property
 */
int32_t
NeuroglancerAnnotation::getPropertyAsInt(const int32_t propertyIndex) const
{
    CaretAssertVectorIndex(m_propertyValues, propertyIndex);
    return m_propertyValues[propertyIndex];
}

/**
 * @return Proprerty at given index as a 32-bit unsigned integer
 * @param propertyIndex
 *    Index of the property
 */
uint32_t
NeuroglancerAnnotation::getPropertyAsUnsignedInt(const int32_t propertyIndex) const
{
    const int32_t intValue(getPropertyAsInt(propertyIndex));
    const uint32_t* unsignedIntPointer(reinterpret_cast<const uint32_t*>(&intValue));
    return *unsignedIntPointer;
}

/**
 * @return Proprerty at given index as a 32-bit float
 * @param propertyIndex
 *    Index of the property
 */
float
NeuroglancerAnnotation::getPropertyAsFloat(const int32_t propertyIndex) const
{
    const int32_t intValue(getPropertyAsInt(propertyIndex));
    const float* floatPointer(reinterpret_cast<const float*>(&intValue));
    return *floatPointer;
}

/**
 * @return Proprerty at given index as RGBA
 * @param propertyIndex
 *    Index of the property
 */
RgbaUnsignedByte
NeuroglancerAnnotation::getPropertyAsRGBA(const int32_t propertyIndex) const
{
    RgbaUnsignedByte rgba;
    rgba.setUnsignedIntRGBA(getPropertyAsUnsignedInt(propertyIndex));
    return rgba;
}

/**
 * @return Proprerty for indexing a label table
 * @param propertyIndex
 *    Index of the property
 */
int32_t
NeuroglancerAnnotation::getPropertyAsLabelIndex(const int32_t propertyIndex) const
{
    return static_cast<int32_t>(getPropertyAsUnsignedInt(propertyIndex));
}

/**
 * @return RGBA coloring for this annotation
 */
const uint8_t*
NeuroglancerAnnotation::getRgba() const
{
    return m_rgba.getRgbaPointer();
}

/**
 * Set the RGBA coloring from the property at the given index
 * @param propertyIndex
 *    Index of the property
 */
void
NeuroglancerAnnotation::setRgbaFromPropertyAtIndex(const int32_t propertyIndex)
{
    CaretAssertVectorIndex(m_propertyValues, propertyIndex);
    const uint32_t* unsignedIntRGBA = reinterpret_cast<const uint32_t*>(&m_propertyValues[propertyIndex]);
    m_rgba.setUnsignedIntRGBA(*unsignedIntRGBA);
}

/**
 * Get identification text for this feature
 * @param idTextOut
 *    Rows of text for display
 * @param neuroAnnFile
 *    Neuroglancer annotation file containing this feature
 * @param toolTipFlag
 *    If true, text is for tooltip
 */
void
NeuroglancerAnnotation::getIdentificationText(std::vector<std::vector<AString>>& idTextOut,
                                              const NeuroglancerAnnotationFile* neuroAnnFile,
                                              const bool toolTipFlag) const
{
    idTextOut.clear();
    
    const AString annName("Neuroglancer: "
                          + getUniqueIdAsString());

    std::vector<AString> rowOne;
    rowOne.push_back(annName);
    idTextOut.push_back(rowOne);
    
    std::vector<AString> rowTwo;
    rowTwo.push_back("IJK: "
                     + AString::fromNumbers(m_ijk, ",", 'f', 3));
    idTextOut.push_back(rowTwo);
    
    const Vector3D neuroglancerXYZ(getNeuroglancerXYZ(neuroAnnFile->getIjkToXyzTransformationMatrix()));
    std::vector<AString> rowThree;
    rowThree.push_back("Neuroglancer XYZ: "
                       + AString::fromNumbers(neuroglancerXYZ, ",", 'f', 0));
    idTextOut.push_back(rowThree);

    std::vector<AString> rowFour;
    rowFour.push_back("Workbench XYZ: "
                       + AString::fromNumbers(m_xyz, ",", 'f', 3));
    idTextOut.push_back(rowFour);
    
    if ( ! toolTipFlag) {
        const std::vector<const NeuroglancerPropertyInfo*> allPropInfo(neuroAnnFile->getAllPropertyInfos());
        const int32_t numProps(allPropInfo.size());
        for (int32_t iProp = 0; iProp < numProps; iProp++) {
            CaretAssertVectorIndex(allPropInfo, iProp);
            const NeuroglancerPropertyInfo* pi(allPropInfo[iProp]);
            AString valueText;
            switch (pi->getDataType()) {
                case NeuroglancerPropertyDataTypeEnum::INVALID:
                    break;
                case NeuroglancerPropertyDataTypeEnum::FLOAT:
                    valueText = AString::number(getPropertyAsFloat(iProp));
                    break;
                case NeuroglancerPropertyDataTypeEnum::INTEGER:
                    valueText = AString::number(getPropertyAsInt(iProp));
                    break;
                case NeuroglancerPropertyDataTypeEnum::UNSIGNED_INTEGER:
                    valueText = AString::number(getPropertyAsUnsignedInt(iProp));
                    break;
                case NeuroglancerPropertyDataTypeEnum::RGBA:
                    valueText = getPropertyAsRGBA(iProp).toString();
                    break;
            }
            
            if (pi->isLabels()) {
                const int32_t propertyIndex(pi->getPropertyIndex());
                const int32_t labelIndex(getPropertyAsLabelIndex(propertyIndex));
                FeatureLabelModel* labelModel(pi->getLabelModel());
                if (labelModel != NULL) {
                    FeatureLabel* label(labelModel->getLabelWithValue(labelIndex));
                    if (label != NULL) {
                        valueText = label->text();
                    }
                }
            }
            
            if ( ! valueText.isEmpty()) {
                idTextOut.push_back( { pi->getDescription(), valueText } );
            }
        }
    }
}

/**
 * @return True if this annotation is displayed
 */
bool
NeuroglancerAnnotation::isDisplayed() const
{
    return m_displayedFlag;
}

/**
 * Set display status for this annotation
 * @param status
 *    New displayed status
 */
void
NeuroglancerAnnotation::setDisplayed(const bool status)
{
    m_displayedFlag = status;
}

/**
 * Update the displayed status using the given label selections
 * @param allPropertyInfos
 *    All property infos
 */
void
NeuroglancerAnnotation::updateDisplayedStatus(const std::vector<const NeuroglancerPropertyInfo*>& allPropertyInfos)
{
    /*
     * Assume displayed until a label that is off is found
     */
    m_displayedFlag = true;
    
    for (const NeuroglancerPropertyInfo* npi : allPropertyInfos) {
        CaretAssert(npi);
        if (npi->isLabels()) {
            if ( ! npi->isLabelAtIndexEnabled(getPropertyAsLabelIndex(npi->getPropertyIndex()))) {
                m_displayedFlag = false;
                return;
            }
        }
    }
}

