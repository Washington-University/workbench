
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

#define ____NEUROGLANCER_ANNOTATION__DECLARE__
#include "NeuroglancerAnnotation.h"
#undef ____NEUROGLANCER_ANNOTATION__DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "NeuroglancerAnnotationsFile.h"
#include "NeuroglancerAnnotationPropertyValue.h"

using namespace caret;



/**
 * \class caret::NeuroglancerAnnotation
 * \brief Class for an annotation that could be point, line, area, etc
 * \ingroup Files
 */

/**
 * Constructor.
 * @param annotationType
 *    The type of the annotation
 * @param fileName
 *    Name of file (no path) from which annotation was read
 * @param ijk
 *    IJK(s) for the annotation
 * @param color
 *    Color of the annotation
 * @param symbolSize
 *    Size of the symbol
 * @param propertieValues
 *    Propertry values for this annotation
 */
NeuroglancerAnnotation::NeuroglancerAnnotation(const NeuroglancerAnnotationTypeEnum::Enum annotationType,
                                               const AString& fileName,
                                               const std::vector<Vector3D>& ijk,
                                               const QColor& color,
                                               const float symbolSize,
                                               const std::vector<const NeuroglancerAnnotationPropertyValue*>& propertyValues)
: NeuroglancerAnnotationBase(NeuroglancerAnnotationBase::BaseType::ANNOTATION),
m_annotationType(annotationType),
m_fileName(fileName),
m_ijk(ijk),
m_color(color),
m_symbolSize(symbolSize),
m_propertyValues(propertyValues)
{
    switch (m_annotationType) {
        case NeuroglancerAnnotationTypeEnum::INVALID:
            break;
        case NeuroglancerAnnotationTypeEnum::AXIS_ALIGNED_BOUNDING_BOX:
            CaretAssert(m_ijk.size() == 2);
            break;
        case NeuroglancerAnnotationTypeEnum::ELLIPSOID:
            CaretAssert(m_ijk.size() == 1);
            break;
        case NeuroglancerAnnotationTypeEnum::LINE:
            CaretAssert(m_ijk.size() >= 2);
            break;
        case NeuroglancerAnnotationTypeEnum::POINT:
            CaretAssert(m_ijk.size() == 1);
            break;
        case NeuroglancerAnnotationTypeEnum::POLYLINE:
            CaretAssert(m_ijk.size() >= 2);
            break;
    }
    
    setFlags(Qt::ItemIsSelectable
             | Qt::ItemIsEnabled);
    
    setText(fileName
            + " - "
            + NeuroglancerAnnotationTypeEnum::toGuiName(m_annotationType)
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
NeuroglancerAnnotation::~NeuroglancerAnnotation()
{
    std::cout << "Annotation destroyed" << std::endl;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
NeuroglancerAnnotation::copyHelperNeuroglancerAnnotation(const NeuroglancerAnnotation& obj)
{
    m_annotationType = obj.m_annotationType;
    m_fileName       = obj.m_fileName;
    m_ijk            = obj.m_ijk;
    m_color          = obj.m_color;
    m_symbolSize     = obj.m_symbolSize;
    m_propertyValues = obj.m_propertyValues;
}

/**
 * @return The annotation type
 */
NeuroglancerAnnotationTypeEnum::Enum
NeuroglancerAnnotation::getType() const
{
    return m_annotationType;
}

/**
 * @return Name of file from which annotation was read
 */
AString
NeuroglancerAnnotation::getFileName() const
{
    return m_fileName;
}

/**
 * @return Number of IJK in the annotation
 */
int32_t
NeuroglancerAnnotation::getNumberOfIJK() const
{
    return m_ijk.size();
}

/**
 * @return The color
 */
const QColor&
NeuroglancerAnnotation::getColor() const
{
    return m_color;
}

/**
 * @return IJK at the given index
 * @param index
 *    The index
 */
const Vector3D&
NeuroglancerAnnotation::getIJK(const int32_t index) const
{
    CaretAssertVectorIndex(m_ijk, index);
    return m_ijk[index];
}

/**
 * @return The size of the annotation
 */
float
NeuroglancerAnnotation::getSymbolSize() const
{
    return m_symbolSize;
}

/**
 * @return The name of the type
 */
AString
NeuroglancerAnnotation::getTypeName() const
{
    return NeuroglancerAnnotationTypeEnum::toGuiName(m_annotationType);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
NeuroglancerAnnotation::toString() const
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
                + ", color=" + NeuroglancerAnnotationPropertyValue::QColorToString(m_color));
    return txt;
}

/**
 * Get identification text for this annotation
 * @param idTextOut
 *    Rows of text for display
 * @param neuroglancerAnnotationFile
 *    Neuroglancer annotation file containing this annotation
 * @param annotationIndex
 *    Index of this annotation in the neuroglancer annotation file
 * @param toolTipFlag
 *    If true, text is for tooltip
 */
void
NeuroglancerAnnotation::getIdentificationText(std::vector<std::vector<AString>>& idTextOut,
                                              const NeuroglancerAnnotationsFile* neuroglancerAnnotationFile,
                                              const int32_t annotationIndex,
                                              const bool toolTipFlag) const
{
    idTextOut.clear();
    
    const AString neuroAnnName("Neuro Ann "
                               + text());
    if (toolTipFlag) {
        std::vector<AString> rowOne;
        rowOne.push_back(neuroAnnName);
        idTextOut.push_back(rowOne);
        
        const int32_t iCoord(0);
        const Vector3D ijk(getIJK(iCoord));
        std::vector<AString> rowTwo;
        rowTwo.push_back("IJK: "
                         + AString::fromNumbers(ijk));
        idTextOut.push_back(rowTwo);
        
        if (neuroglancerAnnotationFile != NULL) {
            const Vector3D xyz(neuroglancerAnnotationFile->getAnnotationCoordinateXYZ(annotationIndex,
                                                                                      iCoord));
            std::vector<AString> rowThree;
            rowThree.push_back("XYZ: "
                               + AString::fromNumbers(xyz, ",", 'f', 3));
            idTextOut.push_back(rowThree);
        }
    }
    else {
        std::vector<AString> rowOne;
        rowOne.push_back(neuroAnnName);
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
            
            if (neuroglancerAnnotationFile != NULL) {
                const Vector3D xyz(neuroglancerAnnotationFile->getAnnotationCoordinateXYZ(annotationIndex,
                                                                                          iCoord));
                rowIJKXYZ.push_back("XYZ"
                                    + indexString
                                    + ": "
                                    + AString::fromNumbers(xyz, ",", 'f', 3));
            }
            idTextOut.push_back(rowIJKXYZ);
        }
        
        for (const auto& propVal : m_propertyValues) {
            if (propVal->getDataType() == NeuroglancerAnnotationPropertyDataTypeEnum::LABEL) {
                std::vector<AString> row {
                    propVal->getDescription(),
                    propVal->getLabelText()
                };
                idTextOut.push_back(row);
            }
        }
    }
}



