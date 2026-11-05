
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

#include "CaretAssert.h"
#include "Matrix4x4.h"
#include "NeuroglancerAnnotationFile.h"
#include "NeuroglancerPropertyInfo.h"

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
    Matrix4x4 m;
    m.setMatrixElement(0, 0, -1.0);
    m.setMatrixElement(0, 3, 48.0);

    m.setMatrixElement(1, 1, 0.0);
    m.setMatrixElement(1, 2, -1.0);
    m.setMatrixElement(1, 3, 48.0);

    m.setMatrixElement(2, 1, 1.0);
    m.setMatrixElement(2, 2, 0.0);
    m.setMatrixElement(2, 3, -64.0);
    
    m_xyz[0] = -(m_ijk[0] - 48);
    m_xyz[1] = m_ijk[1] + 64;
    m_xyz[2] = (m_ijk[2] + 48);
    static bool firstTimeFlag(true);
    if (firstTimeFlag) {
        firstTimeFlag = false;
        std::cout << "Matrix" << std::endl;
        std::cout << m.toFormattedString("   ") << std::endl << std::endl;
        std::cout << "IJK: " << m_xyz.toString() << std::endl;
        m.multiplyPoint3(m_xyz);
        std::cout << "XYZ: " << m_xyz.toString() << std::endl;
    }
    else {
        m.multiplyPoint3(m_xyz);
    }
    
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
int32_t
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
int32_t
NeuroglancerAnnotation::getPropertyAsFloat(const int32_t propertyIndex) const
{
    const int32_t intValue(getPropertyAsInt(propertyIndex));
    const float* floatPointer(reinterpret_cast<const float*>(&intValue));
    return *floatPointer;
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
    
    const AString featureName(getUniqueIdAsString());
    
    std::vector<AString> rowOne;
    rowOne.push_back(featureName);
    idTextOut.push_back(rowOne);
    
    std::vector<AString> rowTwo;
    rowTwo.push_back("IJK: "
                     + AString::fromNumbers(m_ijk));
    idTextOut.push_back(rowTwo);
    
    std::vector<AString> rowThree;
    rowThree.push_back("XYZ: "
                       + AString::fromNumbers(m_xyz, ",", 'f', 3));
    idTextOut.push_back(rowThree);

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
                    break;
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
