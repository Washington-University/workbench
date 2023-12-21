
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

#include <algorithm>
#include <cmath>

#include "AnnotationCoordinate.h"
#include "AnnotationFontAttributes.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiMetaData.h"
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
 * Update a coordinate while the annotation is being drawn
 * @param coordinateIndex
 *    Index of the coordinate
 * @param xyz
 *    New XYZ for coordinate
 */
void
AnnotationPolyhedron::updateCoordinatesWhileBeingDrawn(const int32_t coordinateIndex,
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
 *    Contains all edges from the polyhedron
 * @param trianglesOut
 *    Contains all triangles from sides
 */
void
AnnotationPolyhedron::getEdgesAndTriangles(std::vector<Edge>& edgesOut,
                                           std::vector<Triangle>& trianglesOut) const
{
    edgesOut.clear();
    
    const int32_t halfNumCoords(getNumberOfCoordinates() / 2);
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
         * Triangles
         */
        trianglesOut.push_back(Triangle(getCoordinate(i)->getXYZ(),
                                        getCoordinate(i + farOffset)->getXYZ(),
                                        getCoordinate(iNext)->getXYZ()));
        trianglesOut.push_back(Triangle(getCoordinate(iNext)->getXYZ(),
                                        getCoordinate(i + farOffset)->getXYZ(),
                                        getCoordinate(iNext + farOffset)->getXYZ()));
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
    const bool polyhedronSamplesFlag(true);
    std::vector<AString> metaDataNames;
    std::vector<AString> requiredMetaDataNames;
    Annotation::getDefaultMetaDataNamesForType(getType(),
                                               polyhedronSamplesFlag,
                                               metaDataNames,
                                               requiredMetaDataNames);

    return getMetaData()->toFormattedHtml(metaDataNames);
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
