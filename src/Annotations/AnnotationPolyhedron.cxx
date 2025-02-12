
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
#include "HtmlStringBuilder.h"
#include "HtmlTableBuilder.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

static bool debugFlag = false;
    
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
    
    m_sceneAssistant.reset(new SceneClassAssistant());
    if (testProperty(Property::SCENE_CONTAINS_ATTRIBUTES)) {
    }
    resetProperty(Property::COPY_CUT_PASTE);
    
    /*
     * Initialize metadata with invalid dates
     */
    m_sampleMetaData.reset(new AnnotationSampleMetaData(getMetaData()));
    m_sampleMetaData->setActualSampleEditDate(AnnotationSampleMetaData::getInvalidDateInString());
    m_sampleMetaData->setDesiredSampleEditDate(AnnotationSampleMetaData::getInvalidDateInString());
    
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
        case AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE:
            if ( ! m_linkedPolyhedronIdentifier.isEmpty()) {
                EventAnnotationPolyhedronGetByLinkedIdentifier linkEvent(NULL,
                                                                         AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE,
                                                                         m_linkedPolyhedronIdentifier);
                EventManager::get()->sendEvent(linkEvent.getPointer());
                AnnotationPolyhedron* desiredPolyhedron(linkEvent.getPolyhedron());
                if (desiredPolyhedron != NULL) {
                    return desiredPolyhedron->getMetaData();
                }
                else {
                    CaretLogSevere("Failed to find matching polyhedron with link identifier="
                                   + m_linkedPolyhedronIdentifier);
                }
            }
            break;
        case AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE:
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
 * A desired sample is linked to an actual sample and vice versa
 */
AString
AnnotationPolyhedron::getLinkedPolyhedronIdentifier() const
{
    return m_linkedPolyhedronIdentifier;
}

/**
 * Set the linked polyhedron identifier
 * A desired sample is linked to an actual sample and vice versa
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
        case AnnotationPolyhedronTypeEnum::ACTUAL_SAMPLE:
            break;
        case AnnotationPolyhedronTypeEnum::DESIRED_SAMPLE:
            /*
             * Set linked identifier for DESIRED SAMPLE to date/time
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
        std::vector<GraphicsPolygonTessellator::Vertex> verticesEndOne;
        std::vector<GraphicsPolygonTessellator::Vertex> verticesEndTwo;
        for (int32_t i = 0; i < halfNumCoords; i++) {
            verticesEndOne.emplace_back(i, getCoordinate(i)->getXYZ());
            verticesEndTwo.emplace_back((i + halfNumCoords),
                                   getCoordinate(i + halfNumCoords)->getXYZ());
        }
        
        
        {
            std::vector<Triangle> triangles;
            tessellatePolygon(verticesEndOne, triangles);
            m_tessellatedTriangles.insert(m_tessellatedTriangles.end(),
                                triangles.begin(),
                                triangles.end());
        }
        
        {
            std::vector<Triangle> triangles;
            tessellatePolygon(verticesEndTwo, triangles);
            m_tessellatedTriangles.insert(m_tessellatedTriangles.end(),
                                triangles.begin(),
                                triangles.end());
        }
    }
    
    trianglesOut = m_tessellatedTriangles;
}

/**
 * Tessellate vertices in the polygon into triangles
 * @param polygon
 *    The input polygon
 * @param trianglesOut
 *    Output with triangles
 */
void
AnnotationPolyhedron::tessellatePolygon(const std::vector<GraphicsPolygonTessellator::Vertex>& polygon,
                                        std::vector<Triangle>& trianglesOut) const
{
    trianglesOut.clear();
    
    AString errorMessage;
    GraphicsPolygonTessellator tess(polygon,
                                    m_planeOne.getNormalVector());
    std::vector<GraphicsPolygonTessellator::Vertex> triangleVertices;
    if (tess.tessellate(triangleVertices,
                        errorMessage)) {
        const int32_t numTriangles(triangleVertices.size() / 3);
        CaretAssert(static_cast<int32_t>(triangleVertices.size()) == (numTriangles * 3));
        for (int32_t i = 0; i < numTriangles; i++) {
            const int32_t i3(i * 3);
            trianglesOut.push_back(Triangle(triangleVertices[i3].m_xyz,
                                            triangleVertices[i3+1].m_xyz,
                                            triangleVertices[i3+2].m_xyz));
        }
        
        if (debugFlag) {
            std::cout << "Back from tessellator: " << std::endl;
            for (auto v : triangleVertices) {
                std::cout << v.m_vertexIndex << " " << v.m_xyz.toString() << std::endl;
            }
            std::cout << std::endl;
        }
    }
    else {
        CaretLogSevere("Tessellator Failed: "
                       + errorMessage);
    }
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
                                            (acOne->getXYZ().toString(6),
                                             + "   "
                                             + acTwo->getXYZ().toString(6)));
    }
    
    dataFileInformation.addNameAndValue("Sample Type: ",
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
        dataFileInformation.addNameAndValue("Polyhedron Volume ",
                                            AString::number(polyhedronVolume));
        dataFileInformation.addNameAndValue("Polyhedron End One Area ",
                                            AString::number(endOnePolygonArea));
        dataFileInformation.addNameAndValue("Polyhedron End Two Area ",
                                            AString::number(endTwoPolygonArea));
        dataFileInformation.addNameAndValue("Distance Between Ends ",
                                            AString::number(endToEndDistance));
        if ( ! warningMessage.isEmpty()) {
            dataFileInformation.addNameAndValue("Polyhedron Volume Warnings: ",
                                                warningMessage);
        }
    }
    else {
        dataFileInformation.addNameAndValue("Polyhedron Volume Failed: "
                                            ,errorMessage);
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
    const int32_t numberOfColumns(3);
    HtmlTableBuilder tableBuilder(HtmlTableBuilder::V4_01,
                                  numberOfColumns);
    
    AString abcdText, pointOnPlaneXyzText;
    if (m_planeOne.toAbcdAndPointXYZ(abcdText,
                                     pointOnPlaneXyzText)) {
        tableBuilder.addRow("Plane One",
                            abcdText,
                            pointOnPlaneXyzText);
    }
    else {
        tableBuilder.addRow("Plane One",
                            "Invalid");
    }

    if (m_planeTwo.toAbcdAndPointXYZ(abcdText,
                                     pointOnPlaneXyzText)) {
        tableBuilder.addRow("Plane Two",
                            abcdText,
                            pointOnPlaneXyzText);
    }
    else {
        tableBuilder.addRow("Plane Two",
                            "Invalid");
    }

    tableBuilder.addRow("Plane One Text",
                        m_planeOneNameStereotaxicXYZ.toString(6));
    tableBuilder.addRow("Plane Two Text",
                        m_planeTwoNameStereotaxicXYZ.toString(6));

    const int32_t numCoords(getNumberOfCoordinates() / 2);
    for (int32_t i = 0; i < numCoords; i++) {
        const AnnotationCoordinate* acOne(getCoordinate(i));
        const AnnotationCoordinate* acTwo(getCoordinate(i + numCoords));
        tableBuilder.addRow("Coord " + AString::number(i + 1),
                            acOne->getXYZ().toString(6),
                            acTwo->getXYZ().toString(6));
    }
    
    HtmlStringBuilder html;
    
    html.add(tableBuilder.getAsHtmlTable());
    
    html.add("Sample Type: " + AnnotationPolyhedronTypeEnum::toGuiName(getPolyhedronType()));
    html.addLineBreak();
    
    html.add(getMetadataInformationHtml());
    
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
        html.addLineBreak();
        html.addLineBreak();
        html.add("Polyhedron Volume "
                 + AString::number(polyhedronVolume));
        html.addLineBreak();
        html.addLineBreak();
        html.add("Polyhedron End One Area "
                 + AString::number(endOnePolygonArea));
        html.addLineBreak();
        html.add("Polyhedron End Two Area "
                 + AString::number(endTwoPolygonArea));
        html.addLineBreak();
        html.add("Distance Between Ends "
                 + AString::number(endToEndDistance));
        html.addLineBreak();
        if ( ! warningMessage.isEmpty()) {
            html.addLineBreak();
            html.add("Polyhedron Volume Warnings: "
                     + warningMessage);
        }
    }
    else {
        html.addLineBreak();
        html.add("Polyhedron Volume Failed: "
                 + errorMessage);
    }
    
    return html.toStringWithHtmlBody();
}

/**
 * @return metadata for polyhedron in HTML format
 */
AString
AnnotationPolyhedron::getMetadataInformationHtml() const
{
    return getSampleMetaData()->toFormattedHtml(this);
}

/**
 * Compute the volume and other measurements of the polyhedron
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
     * Get matrix that rotates plane to align with the Z-axis
     */
    const Vector3D zAxis(0.0, 0.0, 1.0);
    const Matrix4x4 matrix(Matrix4x4::rotationTo(m_planeOne.getNormalVector(),
                                                 zAxis));
    
    /*
     * Rotate coordinates to align with Z-axis
     * and split coordinates into the two "end" polygons
     */
    std::vector<Vector3D> polygonOne;
    std::vector<Vector3D> polygonTwo;
    for (int32_t i = 0; i < numCoords; i++) {
        const Vector3D xyzOrig(getCoordinate(i)->getXYZ());
        Vector3D xyz(xyzOrig);
        matrix.multiplyPoint3(xyz);
        
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
     * Absolute distance between ends
     */
    const float absDeltaZ(std::fabs(polygonTwo[0][2] - polygonOne[0][2]));
    
    const int32_t numSteps(static_cast<int32_t>(absDeltaZ * 50.0));
    const float stepPercentage(1.0 / static_cast<float>(numSteps));
    
    /*
     * Z-coordinates should be same in each polygon
     */
    const float stepDelta(absDeltaZ / static_cast<float>(numSteps));
    
    /*
     * Compute the step, in XYZ, for each vertex
     * between the two "end" polygons
     */
    std::vector<Vector3D> stepXYZ;
    for (int32_t i = 0; i < numCoordPairs; i++) {
        CaretAssertVectorIndex(polygonOne, i);
        CaretAssertVectorIndex(polygonTwo, i);
        const Vector3D dxyz(polygonTwo[i] - polygonOne[i]);
        const float stepDistance(dxyz.length() * stepPercentage);
        stepXYZ.push_back(dxyz.normal() * stepDistance);
    }
    
    endOneAreaOut = MathFunctions::polygonArea(polygonOne);
    endTwoAreaOut = MathFunctions::polygonArea(polygonTwo);
    endToEndDistanceOut = absDeltaZ;
    
    /*
     * Increment through the poyhedron from end to end
     * and compute volume of each sliver.
     */
    float volume(0.0);
    for (int32_t iStep = 0; iStep < numSteps; iStep++) {
        std::vector<Vector3D> p1;
        std::vector<Vector3D> p2;
        for (int32_t iCoord = 0; iCoord < numCoordPairs; iCoord++) {
            CaretAssertVectorIndex(polygonOne, iCoord);
            CaretAssertVectorIndex(stepXYZ, iCoord);
            const Vector3D v1(polygonOne[iCoord]
                              + (iStep * stepXYZ[iCoord]));
            const Vector3D v2(v1 + stepXYZ[iCoord]);
            p1.push_back(v1);
            p2.push_back(v2);
        }
        
        const float areaOne(MathFunctions::polygonArea(p1));
        const float areaTwo(MathFunctions::polygonArea(p2));
        const float stepVolume(((areaOne + areaTwo) / 2.0)
                               * stepDelta);
        volume += stepVolume;
    }
    
    volumeOut = volume;
    
    return true;
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
