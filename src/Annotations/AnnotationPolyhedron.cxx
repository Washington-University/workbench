
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_POLYHEDRON_DECLARE__
#include "AnnotationPolyhedron.h"
#undef __ANNOTATION_POLYHEDRON_DECLARE__

#include <QUuid>

#include <algorithm>
#include <cmath>

#include "AnnotationCoordinate.h"
#include "AnnotationFontAttributes.h"
#include "AnnotationSampleMetaData.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "EventAnnotationPolyhedronGetByLinkedIdentifier.h"
#include "EventManager.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "HtmlStringBuilder.h"
#include "HtmlTableBuilder.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class caret::AnnotationPolyhedron
 * \brief An annotation poly line
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param attributeDefaultType
 *    Type for attribute defaults
 */
AnnotationPolyhedron::AnnotationPolyhedron(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType)
: AnnotationMultiPairedCoordinateShape(AnnotationTypeEnum::POLYHEDRON,
                                       attributeDefaultType),
AnnotationFontAttributesInterface()
{
    initializeMembersAnnotationPolyhedron();
}

/**
 * Destructor.
 */
AnnotationPolyhedron::~AnnotationPolyhedron()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationPolyhedron::AnnotationPolyhedron(const AnnotationPolyhedron& obj)
: AnnotationMultiPairedCoordinateShape(obj),
AnnotationFontAttributesInterface()
{
    this->initializeMembersAnnotationPolyhedron();
    this->copyHelperAnnotationPolyhedron(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationPolyhedron&
AnnotationPolyhedron::operator=(const AnnotationPolyhedron& obj)
{
    if (this != &obj) {
        AnnotationMultiPairedCoordinateShape::operator=(obj);
        this->copyHelperAnnotationPolyhedron(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationPolyhedron::copyHelperAnnotationPolyhedron(const AnnotationPolyhedron& obj)
{
    m_planeOne = obj.m_planeOne;
    m_planeTwo = obj.m_planeTwo;
    m_planeOneNameStereotaxicXYZ = obj.m_planeOneNameStereotaxicXYZ;
    m_planeTwoNameStereotaxicXYZ = obj.m_planeTwoNameStereotaxicXYZ;
    *m_fontAttributes = *obj.m_fontAttributes;
    m_polyhedronType  = obj.m_polyhedronType;
    m_linkedPolyhedronIdentifier = obj.m_linkedPolyhedronIdentifier;
}

/**
 * Initialize a new instance of this class.
 */
void
AnnotationPolyhedron::initializeMembersAnnotationPolyhedron()
{
    /* Do not add font attribute to scene since it is written to file */
    m_fontAttributes.reset(new AnnotationFontAttributes(m_attributeDefaultType));
    
    /* no background color */
    m_fontAttributes->setTextBackgroundColor(CaretColorEnum::NONE);
    
    m_sceneAssistant.reset(new SceneClassAssistant());
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
    }
    resetProperty(Property::COPY_CUT_PASTE);
    
    /*
     * Initialize metadata with invalid dates
     */
    m_sampleMetaData.reset(new AnnotationSampleMetaData(getMetaData()));
    m_sampleMetaData->setRetrospectiveSampleEditDate(AnnotationSampleMetaData::getInvalidDateInString());
    m_sampleMetaData->setProspectiveSampleEditDate(AnnotationSampleMetaData::getInvalidDateInString());
    
    m_polyhedronType = AnnotationPolyhedronTypeEnum::INVALID;
    m_linkedPolyhedronIdentifier.clear();
}

/**
 * @return Cast to polyhedron (NULL if NOT polygon)
 */
AnnotationPolyhedron*
AnnotationPolyhedron::castToPolyhedron()
{
    return this;
}

/**
 * @return Cast to polyhedron (NULL if NOT polygon) const method
 */
const AnnotationPolyhedron*
AnnotationPolyhedron::castToPolyhedron() const
{
    return this;
}

/**
 * @return Pointer to the metadata
 */
GiftiMetaData*
AnnotationPolyhedron::getMetaData()
{
    switch (m_polyhedronType) {
        case AnnotationPolyhedronTypeEnum::INVALID:
            break;
        case AnnotationPolyhedronTypeEnum::RETROSPECTIVE_SAMPLE:
            if ( ! m_linkedPolyhedronIdentifier.isEmpty()) {
                EventAnnotationPolyhedronGetByLinkedIdentifier linkEvent(NULL,
                                                                         AnnotationPolyhedronTypeEnum::PROSPECTIVE_SAMPLE,
                                                                         m_linkedPolyhedronIdentifier);
                EventManager::get()->sendEvent(linkEvent.getPointer());
                AnnotationPolyhedron* prospectivePolyhedron(linkEvent.getPolyhedron());
                if (prospectivePolyhedron != NULL) {
                    return prospectivePolyhedron->getMetaData();
                }
                else {
                    CaretLogSevere("Failed to find matching polyhedron with link identifier="
                                   + m_linkedPolyhedronIdentifier);
                }
            }
            break;
        case AnnotationPolyhedronTypeEnum::PROSPECTIVE_SAMPLE:
            break;
    }
    return Annotation::getMetaData();
}

/**
 * @return Pointer to the metadata (const method)
 */
const GiftiMetaData*
AnnotationPolyhedron::getMetaData() const
{
    AnnotationPolyhedron* nonConstPolyhedron(const_cast<AnnotationPolyhedron*>(this));
    CaretAssert(nonConstPolyhedron);
    return nonConstPolyhedron->getMetaData();
}

/**
 * @return The linked polyhedron identifier
 * A prospective sample is linked to an retrospective sample and vice versa
 */
AString
AnnotationPolyhedron::getLinkedPolyhedronIdentifier() const
{
    return m_linkedPolyhedronIdentifier;
}

/**
 * Set the linked polyhedron identifier
 * A prospective sample is linked to an retrospective sample and vice versa
 * @param linkedPolyhedonIdentifier
 *    The identifier
 */
void
AnnotationPolyhedron::setLinkedPolyhedronIdentifier(const AString& linkedPolyhedronIdentifier)
{
    m_linkedPolyhedronIdentifier = linkedPolyhedronIdentifier;
}

/**
 * @return Pointer to the sample metadata
 */
AnnotationSampleMetaData*
AnnotationPolyhedron::getSampleMetaData()
{
    m_sampleMetaData->updateMetaData(getMetaData());
    return m_sampleMetaData.get();
}

/**
 * @return Pointer to the sample metadata (const method)
 */
const AnnotationSampleMetaData*
AnnotationPolyhedron::getSampleMetaData() const
{
    m_sampleMetaData->updateMetaData(getMetaData());
    return m_sampleMetaData.get();
}

/**
 * @return The polyhedron type
 */
AnnotationPolyhedronTypeEnum::Enum
AnnotationPolyhedron::getPolyhedronType() const
{
    return m_polyhedronType;
}

/**
 * Set the polyhedron type
 * @param polyhedronType
 *    Type of polyhedron
 */
void
AnnotationPolyhedron::setPolyhedronType(const AnnotationPolyhedronTypeEnum::Enum polyhedronType)
{
    m_polyhedronType = polyhedronType;
    
    switch (m_polyhedronType) {
        case AnnotationPolyhedronTypeEnum::INVALID:
            break;
        case AnnotationPolyhedronTypeEnum::RETROSPECTIVE_SAMPLE:
            break;
        case AnnotationPolyhedronTypeEnum::PROSPECTIVE_SAMPLE:
            /*
             * Set linked identifier for PROSPECTIVE SAMPLE to date/time
             */
            if (m_linkedPolyhedronIdentifier.isEmpty()) {
                m_linkedPolyhedronIdentifier = QUuid::createUuid().toString();
            }
            break;
    }
}

/**
 * Reset the coordinates so that they extend between the two planes
 * @param planeOne
 *    The first plane
 * @param planeTwo
 *    The second plane
 * @param errorMessageOut
 *    Output with error information
 * @return
 *    True if successful, false if error.
 */
bool
AnnotationPolyhedron::resetRangeToPlanes(const Plane& planeOne,
                                         const Plane& planeTwo,
                                         AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! planeOne.isValidPlane()) {
        errorMessageOut.appendWithNewLine("First plane is invalid.");
    }
    if ( ! planeTwo.isValidPlane()) {
        errorMessageOut.appendWithNewLine("Second plane is invalid.");
    }
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    const int32_t numCoordPairs(getNumberOfCoordinates() / 2);
    for (int32_t i = 0; i < numCoordPairs; i++) {
        const Vector3D xyzOne(getCoordinate(i)->getXYZ());
        const Vector3D xyzTwo(getCoordinate(i + numCoordPairs)->getXYZ());
        
        const Vector3D rayVector((xyzTwo - xyzOne).normal());
        
        Vector3D intersectionOneXYZ, intersectionTwoXYZ;
        float intersectionOneDistance(0.0), intersectionTwoDistance(0.0);
        if (planeOne.rayIntersection(xyzOne,
                                     rayVector,
                                     intersectionOneXYZ,
                                     intersectionOneDistance)
            && planeTwo.rayIntersection(xyzTwo,
                                        rayVector,
                                        intersectionTwoXYZ,
                                        intersectionTwoDistance)) {
            getCoordinate(i)->setXYZ(intersectionOneXYZ);
            getCoordinate(i + numCoordPairs)->setXYZ(intersectionTwoXYZ);
        }
    }
    
    m_planeOne = planeOne;
    m_planeTwo = planeTwo;
    
    const Vector3D nameOneToTwoVector((m_planeTwoNameStereotaxicXYZ
                                       - m_planeOneNameStereotaxicXYZ).normal());
    Vector3D newNameOneXYZ, newNameTwoXYZ;
    float distanceOne(0.0), distanceTwo(0.0);
    if (planeOne.rayIntersection(m_planeOneNameStereotaxicXYZ,
                                 nameOneToTwoVector,
                                 newNameOneXYZ,
                                 distanceOne)
        && planeTwo.rayIntersection(m_planeTwoNameStereotaxicXYZ,
                                    nameOneToTwoVector,
                                    newNameTwoXYZ,
                                    distanceTwo)) {
        m_planeOneNameStereotaxicXYZ = newNameOneXYZ;
        m_planeTwoNameStereotaxicXYZ = newNameTwoXYZ;
    }
    else {
        resetPlaneOneTwoNameStereotaxicXYZ();
    }
    
    setModified();
    
    return true;
}

/**
 * Update a coordinate and its paired coordinate while the annotation is being drawn
 * @param coordinateIndex
 *    Index of one of the coordinates in the pair
 * @param xyz
 *    New XYZ for coordinate
 */
void
AnnotationPolyhedron::updateCoordinatePairWhileBeingDrawn(const int32_t coordinateIndex,
                                                                       const Vector3D& xyz)
{
    /*
     * Coordinates are in pairs (first set followed by second set)
     */
    const int32_t fullNumCoords = static_cast<int32_t>(getNumberOfCoordinates());
    const int32_t halfNumCoords(fullNumCoords / 2);
    if (halfNumCoords < 1) {
        CaretLogSevere("Multi paired coordinate Shape has invalid number of coordinates="
                       + AString::number(fullNumCoords)
                       + "cannot insert new coordinates.");
        return;
    }
    
    if ((coordinateIndex < 0)
        || (coordinateIndex >= fullNumCoords)) {
        CaretLogSevere("Attempt to update coordinate at invalid index="
                       + AString::number(coordinateIndex));
        return;
    }
    
    if (m_planeOne.isValidPlane()
        && m_planeTwo.isValidPlane()) {
        /* OK */
    }
    else {
        CaretLogSevere("Attempt to update coordinate but plane(s) not valid.");
        return;
    }
    
    /*
     * Index one will be for first plane; index two for second plane
     */
    int32_t indexOne(-1);
    int32_t indexTwo(-1);
    if (coordinateIndex < halfNumCoords) {
        indexOne = coordinateIndex;
        indexTwo = coordinateIndex + halfNumCoords;
    }
    else {
        indexOne = coordinateIndex - halfNumCoords;
        indexTwo = coordinateIndex;
    }
    
    if ((indexOne >= 0)
        && (indexOne < fullNumCoords)
        && (indexTwo >= 0)
        && (indexTwo < fullNumCoords)) {
        /* OK */
    }
    else {
        CaretLogSevere("Failure to set coordinate indices correctly");
        return;
    }
    
    Vector3D xyzOne(getCoordinate(indexOne)->getXYZ());
    const float distanceToPlaneOne(m_planeOne.absoluteDistanceToPlane(xyzOne));
    const float distanceToPlaneTwo(m_planeTwo.absoluteDistanceToPlane(xyzOne));
    if (distanceToPlaneTwo < distanceToPlaneOne) {
        std::swap(indexOne,
                  indexTwo);
    }
    
    const Vector3D oneXYZ(m_planeOne.projectPointToPlane(xyz));
    getCoordinate(indexOne)->setXYZ(oneXYZ);
    
    const Vector3D twoXYZ(m_planeTwo.projectPointToPlane(xyz));
    getCoordinate(indexTwo)->setXYZ(twoXYZ);
}

/**
 * Update a coordinate BUT NOT its paired coordinate while the annotation is being drawn
 * @param coordinateIndex
 *    Index of one of the coordinates in the pair
 * @param xyz
 *    New XYZ for coordinate
 */
void
AnnotationPolyhedron::updateCoordinateWhileBeingDrawn(const int32_t coordinateIndex,
                                                      const Vector3D& xyz)
{
    /*
     * Coordinates are in pairs (first set followed by second set)
     */
    const int32_t fullNumCoords = static_cast<int32_t>(getNumberOfCoordinates());
    const int32_t halfNumCoords(fullNumCoords / 2);
    if (halfNumCoords < 1) {
        CaretLogSevere("Multi paired coordinate Shape has invalid number of coordinates="
                       + AString::number(fullNumCoords)
                       + "cannot insert new coordinates.");
        return;
    }
    
    if ((coordinateIndex < 0)
        || (coordinateIndex >= fullNumCoords)) {
        CaretLogSevere("Attempt to update coordinate at invalid index="
                       + AString::number(coordinateIndex));
        return;
    }
    
    if (m_planeOne.isValidPlane()
        && m_planeTwo.isValidPlane()) {
        /* OK */
    }
    else {
        CaretLogSevere("Attempt to update coordinate but plane(s) not valid.");
        return;
    }
    
    /*
     * Index one will be for first plane; index two for second plane
     */
    Plane plane;
    if (coordinateIndex < halfNumCoords) {
        plane = m_planeOne;
    }
    else {
        plane = m_planeTwo;
    }
    
    if ( ! plane.isValidPlane()) {
        CaretLogSevere("Plane for coordinate movement is invalid.");
        return;
    }
    
    if ((coordinateIndex >= 0)
        && (coordinateIndex < fullNumCoords)) {
        /* OK */
    }
    else {
        CaretLogSevere("Failure to set coordinate index correctly");
        return;
    }
    
    const Vector3D newXYZ(plane.projectPointToPlane(xyz));
    getCoordinate(coordinateIndex)->setXYZ(newXYZ);
}


/**
 * Apply (copy) for reset range from an undo/redo command
 * @param polyhedron
 *    The copy from polyhedron
 */
void
AnnotationPolyhedron::applyRedoUndoForResetRangeToPlane(const AnnotationPolyhedron* polyhedron)
{
    CaretAssert(polyhedron);
    m_planeOne = polyhedron->m_planeOne;
    m_planeTwo = polyhedron->m_planeTwo;
    m_planeOneNameStereotaxicXYZ = polyhedron->m_planeOneNameStereotaxicXYZ;
    m_planeTwoNameStereotaxicXYZ = polyhedron->m_planeTwoNameStereotaxicXYZ;
    std::vector<std::unique_ptr<AnnotationCoordinate>> coords(polyhedron->getCopyOfAllCoordinates());
    replaceAllCoordinatesNotConst(coords);
    setModified();
}

/**
 * @return The first plane from when polyhedron was drawn
 */
Plane
AnnotationPolyhedron::getPlaneOne() const
{
    return m_planeOne;
}

/**
 * @return The second plane from when polyhedron was drawn
 */
Plane
AnnotationPolyhedron::getPlaneTwo() const
{
    return m_planeTwo;
}

/**
 * Convert slices to millimeters
 * Slices  Millimeters
 *   -3        -2 x sliceThickness
 *   -2        -sliceThickness
 *   -1        0.0
 *   0         0.0
 *   1         0.0
 *   2         sliceThickness
 *   3         2 x sliceThickness
 *   etc      etc
 * @param sliceThickness
 *    Thickness of the slice, must be positive
 * @param numberOfSlices
 *    Number of slices, may be negative.
 * @return
 *    Millimeters
 */
float
AnnotationPolyhedron::slicesToMillimeters(const float sliceThickness,
                                          const float numberOfSlices)
{
    CaretAssert(sliceThickness > 0.0);
    
    float millimetersOut(0.0);
    if (sliceThickness > 0.0) {
        const float negFlag(numberOfSlices < 0.0);
        const float numSlices(std::fabs(numberOfSlices));
        
        if (numSlices > 1.0) {
            millimetersOut = (numSlices - 1.0) * sliceThickness;
        }
        
        if (negFlag) {
            millimetersOut = -millimetersOut;
        }
    }
    
    return millimetersOut;
}

/**
 * Convert slices to millimeters
 * Slices  Millimeters
 *   -3        -2 x sliceThickness
 *   -2        -sliceThickness
 *   -1        0.0
 *   0         0.0
 *   1         0.0
 *   2         sliceThickness
 *   3         2 x sliceThickness
 *   etc      etc
 * @param sliceThickness
 *    Thickness of the slice, must be positive
 * @param millimeters
 *    Number of millimeterfs
 * @return
 *    Number of slices
 */
float
AnnotationPolyhedron::millimetersToSlices(const float sliceThickness,
                                          const float millimeters)
{
    CaretAssert(sliceThickness > 0.0);

    float sliceOut(0.0);
    if (sliceThickness > 0.0) {
        const bool negFlag(millimeters < 0.0);
        const float mm(std::fabs(millimeters));
        
        sliceOut = (mm / sliceThickness) + 1.0;
        
        if (negFlag) {
            sliceOut = - sliceOut;
        }
    }
    
    return sliceOut;
}

/**
 * Set plane for the polyhedron
 * @param planeOne
 *    First slice plane from when annotation was created
 * @param planeTwo
 *    First slice plane from when annotation was created
 */
void
AnnotationPolyhedron::setPlanes(const Plane& planeOne,
                                const Plane& planeTwo)
{
    if (getNumberOfCoordinates() > 0) {
        CaretLogSevere("Changing plane of polyhedron after coordinates have been added.  "
                       "Plane should be set before adding any coordinates.");
    }
    m_planeOne = planeOne;
    m_planeTwo = planeTwo;
    setModified();
}

/**
 * @return Pointer to the font attributes
 */
AnnotationFontAttributes*
AnnotationPolyhedron::getFontAttributes()
{
    CaretAssert(m_fontAttributes);
    return m_fontAttributes.get();
}

/**
 * @return Pointer to the font attributes
 */
const AnnotationFontAttributes*
AnnotationPolyhedron::getFontAttributes() const
{
    CaretAssert(m_fontAttributes);
    return m_fontAttributes.get();
}

/**
 * Set values while reading file
 * @param plane
 *    The plane from when annotation was drawn
 * @param depth
 *    The depth value from when annotation was drawn
 */
void
AnnotationPolyhedron::setFromFileReading(const Plane& planeOne,
                                         const Plane& planeTwo)
{
    m_planeOne = planeOne;
    m_planeTwo = planeTwo;
    setModified();
}

/**
 * @return The font.
 */
AnnotationTextFontNameEnum::Enum
AnnotationPolyhedron::getFont() const
{
    return m_fontAttributes->getFont();
}

/**
 * Set the font for an annotation.
 
 * @param font
 *    Font for the annotation.
 */
void
AnnotationPolyhedron::setFont(const AnnotationTextFontNameEnum::Enum font)
{
    m_fontAttributes->setFont(font);
}

/**
 * @return Size of font as a percentage of the viewport height.
 *
 * Range is zero to one hundred.
 */
float
AnnotationPolyhedron::getFontPercentViewportSize() const
{
    return m_fontAttributes->getFontPercentViewportSize();
}

/**
 * Set the size of the font as a percentage of the viewport height.
 *
 * @param fontPercentViewportHeight
 *    New value for percentage of viewport height.
 *    Range is zero to one hundred.
 */
void
AnnotationPolyhedron::setFontPercentViewportSize(const float fontPercentViewportHeight)
{
    m_fontAttributes->setFontPercentViewportSize(fontPercentViewportHeight);
}

/**
 * @return The foreground color.
 */
CaretColorEnum::Enum
AnnotationPolyhedron::getTextColor() const
{
    return m_fontAttributes->getTextColor();
}

/**
 * Set the foreground color.
 *
 * @param color
 *     New value for foreground color.
 */
void
AnnotationPolyhedron::setTextColor(const CaretColorEnum::Enum color)
{
    m_fontAttributes->setTextColor(color);
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
AnnotationPolyhedron::getTextColorRGBA(float rgbaOut[4]) const
{
    m_fontAttributes->getTextColorRGBA(rgbaOut);
}

/**
 * Get the foreground color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
AnnotationPolyhedron::getTextColorRGBA(uint8_t rgbaOut[4]) const
{
    m_fontAttributes->getTextColorRGBA(rgbaOut);
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
AnnotationPolyhedron::getCustomTextColor(float rgbaOut[4]) const
{
    m_fontAttributes->getCustomTextColor(rgbaOut);
}

/**
 * Get the foreground color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
AnnotationPolyhedron::getCustomTextColor(uint8_t rgbaOut[4]) const
{
    m_fontAttributes->getCustomTextColor(rgbaOut);
}

/**
 * Set the foreground color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
AnnotationPolyhedron::setCustomTextColor(const float rgba[4])
{
    m_fontAttributes->setCustomTextColor(rgba);
}

/**
 * Set the foreground color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
AnnotationPolyhedron::setCustomTextColor(const uint8_t rgba[4])
{
    m_fontAttributes->setCustomTextColor(rgba);
}

/**
 * @return The background color.
 */
CaretColorEnum::Enum
AnnotationPolyhedron::getTextBackgroundColor() const
{
    return m_fontAttributes->getTextBackgroundColor();
}

/**
 * Set the background color.
 *
 * @param color
 *     New value for foreground color.
 */
void
AnnotationPolyhedron::setTextBackgroundColor(const CaretColorEnum::Enum color)
{
    m_fontAttributes->setTextBackgroundColor(color);
}

/**
 * Get the background color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0.0 to 1.0.
 */
void
AnnotationPolyhedron::getTextBackgroundColorRGBA(float rgbaOut[4]) const
{
    m_fontAttributes->getTextBackgroundColorRGBA(rgbaOut);
}

/**
 * Get the background color's RGBA components regardless of
 * coloring (custom color or a CaretColorEnum) selected by the user.
 *
 * @param rgbaOut
 *     RGBA components ranging 0 to 255.
 */
void
AnnotationPolyhedron::getTextBackgroundColorRGBA(uint8_t rgbaOut[4]) const
{
    m_fontAttributes->getTextBackgroundColorRGBA(rgbaOut);
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
AnnotationPolyhedron::getCustomTextBackgroundColor(float rgbaOut[4]) const
{
    m_fontAttributes->getCustomTextBackgroundColor(rgbaOut);
}

/**
 * Get the background color.
 *
 * @param rgbaOut
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
AnnotationPolyhedron::getCustomTextBackgroundColor(uint8_t rgbaOut[4]) const
{
    m_fontAttributes->getCustomTextBackgroundColor(rgbaOut);
}

/**
 * Set the background color with floats.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0.0, 1.0].
 */
void
AnnotationPolyhedron::setCustomTextBackgroundColor(const float rgba[4])
{
    m_fontAttributes->setCustomTextBackgroundColor(rgba);
}

/**
 * Set the background color with unsigned bytes.
 *
 * @param rgba
 *    RGBA components (red, green, blue, alpha) each of which ranges [0, 255].
 */
void
AnnotationPolyhedron::setCustomTextBackgroundColor(const uint8_t rgba[4])
{
    m_fontAttributes->setCustomTextBackgroundColor(rgba);
}

/**
 * @return
 *    Is bold enabled ?
 */
bool
AnnotationPolyhedron::isBoldStyleEnabled() const
{
    return m_fontAttributes->isBoldStyleEnabled();
}

/**
 * Set bold enabled.
 *
 * @param enabled
 *     New status for bold enabled.
 */
void
AnnotationPolyhedron::setBoldStyleEnabled(const bool enabled)
{
    m_fontAttributes->setBoldStyleEnabled(enabled);
}

/**
 * @return
 *    Is italic enabled ?
 */
bool
AnnotationPolyhedron::isItalicStyleEnabled() const
{
    return m_fontAttributes->isItalicStyleEnabled();
}

/**
 * Set italic enabled.
 *
 * @param enabled
 *     New status for italic enabled.
 */
void
AnnotationPolyhedron::setItalicStyleEnabled(const bool enabled)
{
    m_fontAttributes->setItalicStyleEnabled(enabled);
}

/**
 * @return
 *    Is underline enabled ?
 */
bool
AnnotationPolyhedron::isUnderlineStyleEnabled() const
{
    return m_fontAttributes->isUnderlineStyleEnabled();
}

/**
 * Set underline enabled.
 *
 * @param enabled
 *     New status for underline enabled.
 */
void
AnnotationPolyhedron::setUnderlineStyleEnabled(const bool enabled)
{
    m_fontAttributes->setUnderlineStyleEnabled(enabled);
}

/**
 * @return Is the font too small when it is last drawn
 * that may cause an OpenGL error and, as a result,
 * the text is not seen by the user.
 */
bool
AnnotationPolyhedron::isFontTooSmallWhenLastDrawn() const
{
    return m_fontAttributes->isFontTooSmallWhenLastDrawn();
}

void
AnnotationPolyhedron::setFontTooSmallWhenLastDrawn(const bool tooSmallFontFlag) const
{
    m_fontAttributes->setFontTooSmallWhenLastDrawn(tooSmallFontFlag);
}

/**
 * @param edgesOut
 *    Contains all edges from the polyhedron.  Edges connect a pair of vertices,
 *    one at each end of the polyhedron (connect the polygons)
 * @param trianglesOut
 *    Contains all triangles that form the ends of the polyhedron (the two polygons)
 */
void
AnnotationPolyhedron::getEdgesAndTriangles(std::vector<Edge>& edgesOut,
                                           std::vector<Triangle>& trianglesOut) const
{
    edgesOut.clear();
    trianglesOut.clear();
    
    /*
     * The tessellated triangles are cached to avoid recomputation of them.
     * A copy of the polyhedron's coordinates is also saved so that we
     * can detect when the coordinates change so that new tessellated
     * triangles can be generated.
     */
    const int32_t numCoordinates(getNumberOfCoordinates());
    if (numCoordinates != static_cast<int32_t>(m_tessellationPreviousXYZ.size())) {
        m_tessellatedTriangles.clear();
        m_tessellationPreviousXYZ.clear();
    }
    else if (numCoordinates > 0) {
        CaretAssert(numCoordinates == static_cast<int32_t>(m_tessellationPreviousXYZ.size()));
        
        for (int32_t i = 0; i < numCoordinates; i++) {
            if ( ! getCoordinate(i)->equalXYZ(m_tessellationPreviousXYZ[i])) {
                m_tessellatedTriangles.clear();
                m_tessellationPreviousXYZ.clear();
                break;
            }
        }
    }
    
    if (m_tessellatedTriangles.empty()) {
        m_tessellationPreviousXYZ.clear();
        for (int32_t i = 0; i < numCoordinates; i++) {
            m_tessellationPreviousXYZ.emplace_back(getCoordinate(i)->getXYZ());
        }

        const int32_t halfNumCoords(numCoordinates / 2);
        
        /*
         * Edges of polyhedron that connect the two polygons
         */
        for (int32_t i = 0; i < halfNumCoords; i++) {
            /*
             * Near face
             */
            const int32_t iNext((i == (halfNumCoords - 1)) ? 0 : i + 1);
            edgesOut.push_back(Edge(getCoordinate(i)->getXYZ(),
                                    getCoordinate(iNext)->getXYZ()));
            
            /*
             * Far face
             */
            const int32_t farOffset(halfNumCoords);
            edgesOut.push_back(Edge(getCoordinate(i + farOffset)->getXYZ(),
                                    getCoordinate(iNext + farOffset)->getXYZ()));
            
            /*
             * Triangles that make up sides of polyhedron
             */
            m_tessellatedTriangles.push_back(Triangle(getCoordinate(i)->getXYZ(),
                                                      getCoordinate(i + farOffset)->getXYZ(),
                                                      getCoordinate(iNext)->getXYZ()));
            m_tessellatedTriangles.push_back(Triangle(getCoordinate(iNext)->getXYZ(),
                                                      getCoordinate(i + farOffset)->getXYZ(),
                                                      getCoordinate(iNext + farOffset)->getXYZ()));
        }

        
        /*
         * Ends of polyhedron
         */
        std::vector<Vector3D> verticesEndOne;
        std::vector<Vector3D> verticesEndTwo;
        for (int32_t i = 0; i < halfNumCoords; i++) {
            verticesEndOne.push_back(getCoordinate(i)->getXYZ());
            verticesEndTwo.push_back(getCoordinate(i + halfNumCoords)->getXYZ());
        }
        
        const Vector3D endOneAvg(Vector3D::average(verticesEndOne));
        const Vector3D endTwoAvg(Vector3D::average(verticesEndTwo));
        const Vector3D endOneNormalVector((endOneAvg - endTwoAvg).normal());
        const Vector3D endTwoNormalVector(-endOneNormalVector);
        
        {
            std::vector<Vector3D> triangleVertices;
            const FunctionResult result(GraphicsUtilitiesOpenGL::tesselatePolygon(verticesEndOne,
                                                                                  endOneNormalVector,
                                                                                  triangleVertices));
            if (result.isOk()) {
                const int32_t numTriangles(triangleVertices.size() / 3);
                for (int32_t i = 0; i < numTriangles; i++) {
                    const int32_t i3(i*3);
                    m_tessellatedTriangles.emplace_back(triangleVertices[i3],
                                                        triangleVertices[i3+1],
                                                        triangleVertices[i3+2]);
                }
            }
            else {
                CaretLogSevere(result.getErrorMessage());
            }
        }
        
        {
            std::vector<Vector3D> triangleVertices;
            const FunctionResult result(GraphicsUtilitiesOpenGL::tesselatePolygon(verticesEndTwo,
                                                                                  endTwoNormalVector,
                                                                                  triangleVertices));
            if (result.isOk()) {
                const int32_t numTriangles(triangleVertices.size() / 3);
                for (int32_t i = 0; i < numTriangles; i++) {
                    const int32_t i3(i*3);
                    m_tessellatedTriangles.emplace_back(triangleVertices[i3],
                                                        triangleVertices[i3+1],
                                                        triangleVertices[i3+2]);
                }
            }
            else {
                CaretLogSevere(result.getErrorMessage());
            }
        }
    }
    
    trianglesOut = m_tessellatedTriangles;
}

/**
 * @return True if this instance is modified
 */
bool
AnnotationPolyhedron::isModified() const
{
    if (m_fontAttributes->isModified()) {
        return true;
    }
    return AnnotationMultiPairedCoordinateShape::isModified();
}

/**
 * Clear this instance's modification status
 */
void
AnnotationPolyhedron::clearModified()
{
    m_fontAttributes->clearModified();
    AnnotationMultiPairedCoordinateShape::clearModified();
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
AnnotationPolyhedron::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    AnnotationMultiPairedCoordinateShape::saveSubClassDataToScene(sceneAttributes,
                                                           sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
AnnotationPolyhedron::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                   const SceneClass* sceneClass)
{
    AnnotationMultiPairedCoordinateShape::restoreSubClassDataFromScene(sceneAttributes,
                                                  sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Add information about the content of this instance.
 *
 * @param dataFileInformation
 *     Will contain information about this instance.
 */
void
AnnotationPolyhedron::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const
{
    Annotation::addToDataFileContentInformation(dataFileInformation);

    AString abcdText, pointOnPlaneXyzText;
    if (m_planeOne.toAbcdAndPointXYZ(abcdText,
                                     pointOnPlaneXyzText)) {
        dataFileInformation.addNameAndValue("Plane One",
                                    (abcdText +
                                     + "  "
                                     + pointOnPlaneXyzText));
    }
    else {
        dataFileInformation.addNameAndValue("Plane One",
                                            "Invalid");
    }
    
    if (m_planeTwo.toAbcdAndPointXYZ(abcdText,
                                     pointOnPlaneXyzText)) {
        dataFileInformation.addNameAndValue("Plane Two",
                                            (abcdText +
                                             + "  "
                                             + pointOnPlaneXyzText));
    }
    else {
        dataFileInformation.addNameAndValue("Plane Two",
                                            "Invalid");
    }
    
    dataFileInformation.addNameAndValue("Plane One Text",
                                        m_planeOneNameStereotaxicXYZ.toString(6));
    dataFileInformation.addNameAndValue("Plane Two Text",
                                        m_planeTwoNameStereotaxicXYZ.toString(6));
    
    const int32_t numCoords(getNumberOfCoordinates() / 2);
    for (int32_t i = 0; i < numCoords; i++) {
        const AnnotationCoordinate* acOne(getCoordinate(i));
        const AnnotationCoordinate* acTwo(getCoordinate(i + numCoords));
        dataFileInformation.addNameAndValue(("Coord " + AString::number(i + 1)),
                                            (acOne->getXYZ().toString(6)
                                             + "   "
                                             + acTwo->getXYZ().toString(6)));
    }
    
    dataFileInformation.addNameAndValue("Sample Type",
                                        AnnotationPolyhedronTypeEnum::toGuiName(getPolyhedronType()));
    
    float endOnePolygonArea(0.0);
    float endTwoPolygonArea(0.0);
    float endToEndDistance(0.0);
    float polyhedronVolume(0.0);
    AString warningMessage;
    AString errorMessage;
    
    if (computePolyhedronVolume(polyhedronVolume,
                                endOnePolygonArea,
                                endTwoPolygonArea,
                                endToEndDistance,
                                warningMessage,
                                errorMessage)) {
        dataFileInformation.addNameAndValue("Polyhedron Volume",
                                            AString::number(polyhedronVolume));
        dataFileInformation.addNameAndValue("Polyhedron End One Area",
                                            AString::number(endOnePolygonArea));
        dataFileInformation.addNameAndValue("Polyhedron End Two Area",
                                            AString::number(endTwoPolygonArea));
        dataFileInformation.addNameAndValue("Distance Between Ends",
                                            AString::number(endToEndDistance));
        if ( ! warningMessage.isEmpty()) {
            dataFileInformation.addNameAndValue("Polyhedron Volume Warnings",
                                                warningMessage);
        }
    }
    else {
        dataFileInformation.addNameAndValue("Polyhedron Volume Failed"
                                            ,errorMessage);
    }
    
    const FunctionResultFloat curlVolumeResult(computePolyhedronVolumeCurlTheorem());
    if (curlVolumeResult.isOk()) {
        dataFileInformation.addNameAndValue("Polyhedron Volume (Curl Theorem)",
                                            curlVolumeResult.getValue());
        dataFileInformation.addNameAndValue("Curl Theorem",
                                            "https://mathworld.wolfram.com/PolyhedronVolume.html");
    }
    else {
        dataFileInformation.addNameAndValue("Polyhedron Volume (curl theorem)",
                                            curlVolumeResult.getErrorMessage());
    }
    
    getSampleMetaData()->addToDataFileContentInformation(this,
                                                         dataFileInformation);
}

/**
 * @return Information about the polyhedron in HTML format
 */
AString
AnnotationPolyhedron::getPolyhedronInformationHtml() const
{
    DataFileContentInformation dfci;
    addToDataFileContentInformation(dfci);
    AString htmlOut(dfci.getInformationInHtml());
    return htmlOut;
}

/**
 * Compute the volume and other measurements of the polyhedron VERSION 1
 * @param volumeOut
 *    Output containing the volume of the polyhedron
 * @param endOneAreaOut
 *    Output containing the area of one end of the polyhedron
 * @param endTwoAreaOut
 *    Output containing the area of the other end of the polyhedron
 * @param endToEndDistanceOut
 *    Output containing the distance between the two ends of the polyhedron
 * @param warningMessageOut
 *    Output containing containing possible problems even though there is not an error
 * @param errorMessageOut
 *    Output containing a message describing the fatal error
 * @return
 *    True if successful, otherwise false with errorMessageOut describing the error.
 */
bool
AnnotationPolyhedron::computePolyhedronVolume(float& volumeOut,
                                              float& endOneAreaOut,
                                              float& endTwoAreaOut,
                                              float& endToEndDistanceOut,
                                              AString& warningMessageOut,
                                              AString& errorMessageOut) const
{
    volumeOut = 0.0;
    endOneAreaOut = 0.0;
    endTwoAreaOut = 0.0;
    endToEndDistanceOut = 0.0;
    warningMessageOut = "";
    errorMessageOut   = "";
    
    const int32_t numCoords(getNumberOfCoordinates());
    const int32_t numCoordPairs(numCoords / 2);
    if (numCoordPairs < 3) {
        errorMessageOut = ("There must be at least 3 coordinate pairs to compute volume.  "
                           "This polyhedron contains "
                           + AString::number(numCoordPairs)
                           + " coordinate pairs.");
        return false;
    }
    
    if ( ! m_planeOne.isValidPlane()) {
        errorMessageOut = "First plane in polyhedron is invalid.";
        return false;
    }
    
    /*
     * Split coordinates into the two "end" polygons
     */
    std::vector<Vector3D> polygonOne;
    std::vector<Vector3D> polygonTwo;
    for (int32_t i = 0; i < numCoords; i++) {
        const Vector3D xyz(getCoordinate(i)->getXYZ());
        
        if (i < numCoordPairs) {
            polygonOne.push_back(xyz);
        }
        else {
            polygonTwo.push_back(xyz);
        }
    }
    
    CaretAssert(polygonOne.size() == polygonTwo.size());
    CaretAssert(numCoordPairs == static_cast<int32_t>(polygonOne.size()));
    
    if ( ! MathFunctions::arePointsCoplanar(polygonOne)) {
        warningMessageOut.appendWithNewLine("Coordinates in end one may not be coplanar");
    }
    if ( ! MathFunctions::arePointsCoplanar(polygonTwo)) {
        warningMessageOut.appendWithNewLine(" Coordinates in end two may not be coplanar");
    }
    
    /*
     * Normal vector for polyhedon end one to two
     */
    const Vector3D polyOneAvgXYZ(Vector3D::average(polygonOne));
    const Vector3D polyTwoAvgXYZ(Vector3D::average(polygonTwo));
    const Vector3D polyOneToTwoNormalVector((polyTwoAvgXYZ - polyOneAvgXYZ).normal());
    
    /*
     * Area of each end of polyhedron
     */
    endOneAreaOut = GraphicsUtilitiesOpenGL::computePolygonArea3D(polygonOne,
                                                                  polyOneToTwoNormalVector);
    endTwoAreaOut = GraphicsUtilitiesOpenGL::computePolygonArea3D(polygonTwo,
                                                                  polyOneToTwoNormalVector);

    /*
     * Step distance between each corresponding pair of coordiantes
     */
    const float numStepsFloat(250.0);
    std::vector<Vector3D> polygonStep;
    for (int32_t i = 0; i < numCoordPairs; i++) {
        CaretAssertVectorIndex(polygonOne, i);
        CaretAssertVectorIndex(polygonTwo, i);
        polygonStep.push_back((polygonTwo[i] - polygonOne[i]) / numStepsFloat);
        if (i == 0) {
            endToEndDistanceOut = (polygonTwo[i] - polygonOne[i]).length();
        }
    }
    CaretAssert(numCoordPairs == static_cast<int32_t>(polygonStep.size()));
    
    
    /*
     * Increment through the poyhedron from end to end
     * and compute volume of each sliver.
     */
    float polyhedronVolume(0.0);
    const int32_t numSteps(static_cast<int32_t>(numStepsFloat));
    for (int32_t iStep = 0; iStep < numSteps; iStep++) {
        /*
         * Coordinates for a 'slice' of the polyhedon
         */
        std::vector<Vector3D> p1;
        std::vector<Vector3D> p2;
        for (int32_t iCoord = 0; iCoord < numCoordPairs; iCoord++) {
            CaretAssertVectorIndex(polygonOne, iCoord);
            CaretAssertVectorIndex(polygonStep, iCoord);
            const Vector3D v1(polygonOne[iCoord]
                              + (iStep * polygonStep[iCoord]));
            const Vector3D v2(v1 + polygonStep[iCoord]);
            p1.push_back(v1);
            p2.push_back(v2);
        }
        
        /*
         * Thickness of the 'slice'
         */
        const Vector3D polygonOneAverageXYZ(Vector3D::average(p1));
        const Vector3D polygonTwoAverageXYZ(Vector3D::average(p2));
        const float thickness((polygonOneAverageXYZ - polygonTwoAverageXYZ).length());
        
        if (thickness > 0.0) {
            /*
             * Compute volume of 'slice' and add to polyhedron volume
             */
            const float areaOne(GraphicsUtilitiesOpenGL::computePolygonArea3D(p1,
                                                                              polyOneToTwoNormalVector));
            const float areaTwo(GraphicsUtilitiesOpenGL::computePolygonArea3D(p2,
                                                                              polyOneToTwoNormalVector));
            const float sliceVolume(((areaOne + areaTwo) / 2.0)
                                    * thickness);
            polyhedronVolume += sliceVolume;
        }
    }
    
    volumeOut = polyhedronVolume;
    
    return true;
}

/**
 * @return Function result containing compution of polyhedron volume using curl theorem
 * as described at https://mathworld.wolfram.com/PolyhedronVolume.html
 */
FunctionResultFloat
AnnotationPolyhedron::computePolyhedronVolumeCurlTheorem() const
{
    float volume(0.0);
    AString errorMessage;
    
    const int32_t numCoordinates(getNumberOfCoordinates());
    if (numCoordinates >= 6) {
        /*
         * Find Center of Gravity of all coordinates
         */
        Vector3D cog(0.0, 0.0, 0.0);
        for (int32_t i = 0; i < numCoordinates; i++) {
            cog += getCoordinate(i)->getXYZ();
        }
        cog /= static_cast<float>(numCoordinates);
            
        /*
         * Get the triangles (tessellators convert the
         * polygon ends into triangles).  Also includes
         * triangles formed by connecting the two
         * polygon ends.
         */
        std::vector<Edge> edges;
        std::vector<Triangle> triangles;
        getEdgesAndTriangles(edges,
                             triangles);
        
        /*
         * Compute the volume as described at 
         * https://mathworld.wolfram.com/PolyhedronVolume.html
         *
         * Formula => (1 / 6) * (Summation of each triangle 'a dot n')
         *    where 'a' is first vertex in triangle and 'n' is the
         *    normal vector defined as (b - a) X (c - a).
         */
        const int32_t numTriangles(triangles.size());
        for (int32_t i = 0; i < numTriangles; i++) {
            /*
             * Vertices of triangle
             */
            CaretAssertVectorIndex(triangles, i);
            Vector3D a(triangles[i].m_v1);
            Vector3D b(triangles[i].m_v2);
            Vector3D c(triangles[i].m_v3);
            
            /*
             * Test to force backwards oriented triangles
             */
            const bool testFlag(false);
            if (testFlag) {
                if ((i == 0) || (i == 4)) {
                    std::swap(a, c);
                }
            }
            /*
             * Test triangle has no area
             * (all vertices coincident)
             */
            const float distAB((a - b).length());
            const float distBC((b - c).length());
            const float tolerance(0.001);
            if ((distAB <= tolerance)
                && (distBC <= tolerance)) {
                continue;
            }
            
            /*
             * While the web page refers to it this as a 'normal' it
             * is just the cross product.  If one were to normalize
             * this value, the algorithm will fail.
             */
            const Vector3D bma(b - a);
            const Vector3D cma(c - a);
            Vector3D crossProduct(bma.cross(cma));
            
            /*
             * Compute normal vector pointing from center of polyhedron
             * through triangle's center-of-gravity (average of a, b, c)
             * This normal vector should point in roughly the same
             * direction as the triangle's normal vector.
             */
            const Vector3D triangleCOG((a + b + c) / 3.0);
            const Vector3D triangleCogNormal((triangleCOG - cog).normal());
            
            /*
             * The normal vector of the triangle should point OUT
             * of the polyhedron.
             *
             * If dot product is less than zero the triangle is oriented
             * incorrectly (pointing into the polyhedron) so
             * swap 'a' and 'c' and invert the triangle normal vector.
             */
            const Vector3D normalVector(crossProduct.normal());
            const float dotProd(normalVector.dot(triangleCogNormal));
            if (dotProd < 0) {
                std::swap(a, c);
                crossProduct *= -1.0;
            }
            
            /*
             * Add to volume summation
             */
            volume += (a.dot(crossProduct));
        }
        
        /*
         * Finalize volume
         */
        volume /= 6.0;
    }
    else {
        errorMessage = "Polyhedon contains less than 6 coordinates";
    }
    
    FunctionResultFloat result(volume,
                               errorMessage,
                               errorMessage.isEmpty());
    return result;
}

/**
 * @return XYZ for polyhedron name when drawn on first plane
 */
Vector3D
AnnotationPolyhedron::getPlaneOneNameStereotaxicXYZ() const
{
    return m_planeOneNameStereotaxicXYZ;
}

/**
 * @return XYZ for polyhedron name when drawn on second plane
 */
Vector3D
AnnotationPolyhedron::getPlaneTwoNameStereotaxicXYZ() const
{
    return m_planeTwoNameStereotaxicXYZ;
}

/**
 * Set the XYZ for polyhedron name when drawn on first plane
 * @param xyz
 *    New XYZ
 */
void
AnnotationPolyhedron::setPlaneOneNameStereotaxicXYZ(const Vector3D& xyz)
{
    if (xyz != m_planeOneNameStereotaxicXYZ) {
        m_planeOneNameStereotaxicXYZ = xyz;
        setModified();
    }
}

/**
 * Set the XYZ for polyhedron name when drawn on first plane
 * @param xyz
 *    New XYZ
 */
void
AnnotationPolyhedron::setPlaneTwoNameStereotaxicXYZ(const Vector3D& xyz)
{
    if (xyz != m_planeTwoNameStereotaxicXYZ) {
        m_planeTwoNameStereotaxicXYZ = xyz;
        setModified();
    }
}

/**
 * Reset the positions of the name stereotaxic coordinates using the
 * polyhedron coordinates.
 */
void
AnnotationPolyhedron::resetPlaneOneTwoNameStereotaxicXYZ()
{
    Vector3D nameOneXYZ(0.0, 0.0, 0.0);
    Vector3D nameTwoXYZ(0.0, 0.0, 0.0);
    
    const int32_t numCoordPairs(getNumberOfCoordinates() / 2);
    if (numCoordPairs >= 1) {
        for (int32_t i = 0; i < numCoordPairs; i++) {
            nameOneXYZ += getCoordinate(i)->getXYZ();
            nameTwoXYZ += getCoordinate(i + numCoordPairs)->getXYZ();
        }
        
        const float floatNumCoords(numCoordPairs);
        nameOneXYZ /= floatNumCoords;
        nameTwoXYZ /= floatNumCoords;
    }

    m_planeOneNameStereotaxicXYZ = nameOneXYZ;
    m_planeTwoNameStereotaxicXYZ = nameTwoXYZ;
}
