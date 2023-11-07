
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

#include <cmath>

#include "AnnotationCoordinate.h"
#include "AnnotationFontAttributes.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
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
    m_plane = obj.m_plane;
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
 * Finish creation of a polyhedron using the given depth
 * @param plane
 *    Plane of slice
 * @param polyhedronDepth
 *    Depth of polyhedron
 * @param errorMessageOut
 *    Error information output
 * @return True if successful, else false and see errorMessageOut
 */
bool
AnnotationPolyhedron::finishNewPolyhedron(const Plane& plane,
                                          const float polyhedronDepth,
                                          AString& errorMessageOut)
{
    if (getCoordinateSpace() == AnnotationCoordinateSpaceEnum::STEREOTAXIC) {
        CaretLogSevere("Should not call this function when stereotaxic drawing");
    }
    
    errorMessageOut.clear();
    
    const int32_t numCoords(getNumberOfCoordinates());
    if (numCoords < 3) {
        errorMessageOut = ("Polyhedron must contain at least three coordinates.");
        return false;
    }
    
    if ( ! plane.isValidPlane()) {
        errorMessageOut = ("Plane for volume slice is invalid.");
        return false;
    }
    
    m_plane = plane;
    
    const bool debugFlag(false);
    
    const Vector3D normalVector(plane.getNormalVector());
    if (debugFlag) std::cout << "Normal Vector: " << normalVector.toString() << std::endl;
    const Vector3D offsetVector(normalVector * polyhedronDepth);
    for (int32_t i = 0; i < numCoords; i++) {
        AnnotationCoordinate* newCoord(new AnnotationCoordinate(*getCoordinate(i)));
        Vector3D xyz;
        newCoord->getXYZ(xyz);
        if (debugFlag) std::cout << "   Orig: " << xyz.toString();
        xyz += offsetVector;
        newCoord->setXYZ(xyz);
        if (debugFlag) std::cout << ", New: " << xyz.toString() << std::endl;
        addCoordinate(newCoord);
    }
    
    if (debugFlag) {
        std::cout << "Created annotation: " << this->toString() << std::endl;
        const int32_t n(getNumberOfCoordinates());
        for (int32_t i = 0; i < n; i++) {
            Vector3D xyz;
            getCoordinate(i)->getXYZ(xyz);
            std::cout << "   " << i << ": " << xyz.toString() << std::endl;
        }
    }
    
    setDrawingNewAnnotationStatus(false);
    
    return true;
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
    
    setModified();
    
    return true;
}


/**
 * @return The plane from when polyhedron was drawn
 */
Plane
AnnotationPolyhedron::getPlane() const
{
    return m_plane;
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
 * Set plane for the polyhedron SHOULD BE DONE BEFORE ADDING ANY COORDINATES
 * @param plane
 *    New plane
 */
void
AnnotationPolyhedron::setPlane(const Plane& plane)
{
    if (getNumberOfCoordinates() > 0) {
        CaretLogSevere("Changing plane of polyhedron after coordinates have been added.  "
                       "Plane should be set before adding any coordinates.");
    }
    m_plane = plane;
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
AnnotationPolyhedron::setFromFileReading(const Plane& plane)
{
    m_plane = plane;
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
