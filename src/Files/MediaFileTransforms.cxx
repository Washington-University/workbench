
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __MEDIA_FILE_TRANSFORMS_DECLARE__
#include "MediaFileTransforms.h"
#undef __MEDIA_FILE_TRANSFORMS_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziNonLinearTransform.h"
#include "MediaFile.h"
#include "PixelIndex.h"
#include "PixelLogicalIndex.h"
#include "Plane.h"
#include "Vector3D.h"

using namespace caret;

static const bool debugFlag(false);

    
/**
 * \class caret::MediaFileTransforms 
 * \brief Transformations used in and by media files
 * \ingroup Files
 */

/**
 * Constructor of invalid instance
 */
MediaFileTransforms::MediaFileTransforms()
: CaretObject()
{

}

/**
 * Copy constructor
 * @param mft
 *    Instance copied to this
 */
MediaFileTransforms::MediaFileTransforms(const MediaFileTransforms& mft)
: CaretObject(mft)
{
    copyHelper(mft);
}


/**
 * Constructor.
 * @param parentMediaFile
 *    Media file that owns this instance
 */
MediaFileTransforms::MediaFileTransforms(const Inputs& inputs)
: CaretObject(),
m_inputs(inputs)
{
    if ( ! m_inputs.m_logicalBoundsRect.isValid()) {
        CaretLogSevere("Input logical bounds rectangle is invalid");
        CaretAssert(0);
    }
    else {
        /*testTransforms();*/
    }
}

/**
 * Destructor.
 */
MediaFileTransforms::~MediaFileTransforms()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
MediaFileTransforms::toString() const
{
    return "MediaFileTransforms";
}

/**
 * Assignment operator
 * @param mft
 *    Instance to copy
 * @return
 *    This
 */
MediaFileTransforms&
MediaFileTransforms::operator=(const MediaFileTransforms& mft)
{
    if (this != &mft) {
        copyHelper(mft);
    }
    
    return *this;
}

/**
 * Copies other instance to this
 * @param mft
 *    Instance to copy
 */
void
MediaFileTransforms::copyHelper(const MediaFileTransforms& mft)
{
    m_inputs = mft.m_inputs;
}


/**
 * @return A pixel index converted from a pixel logical index.
 * @param pixelLogicalIndex
 *    The logical pixel index.
 */
PixelIndex
MediaFileTransforms::logicalPixelIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const
{
    PixelIndex pixelIndex(pixelLogicalIndex.getI() - m_inputs.m_logicalBoundsRect.x(),
                          pixelLogicalIndex.getJ() - m_inputs.m_logicalBoundsRect.y(),
                          pixelLogicalIndex.getK());
    
    return pixelIndex;
}

/**
 * Convert a pixel index to a plane XYZ.
 * @param pixelIndex
 *    Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::logicalPixelIndexToPlaneXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                       Vector3D& planeXyzOut) const
{
    PixelIndex pixelIndex(logicalPixelIndexToPixelIndex(pixelLogicalIndex));
    return pixelIndexToPlaneXYZ(pixelIndex,
                                planeXyzOut);
}

/**
 * Convert a pixel index to a plane XYZ.
 * @param logicalX
 *    logtical X Index of pixel
 * @param logicalY
 *    logtical Y Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::logicalPixelIndexToPlaneXYZ(const float logicalX,
                                       const float logicalY,
                                       Vector3D& planeXyzOut) const
{
    const PixelLogicalIndex logicalIndex(logicalX,
                                         logicalY,
                                         0.0);
    return logicalPixelIndexToPlaneXYZ(logicalIndex,
                                       planeXyzOut);
}

/**
 * Convert a pixel index to a plane XYZ.
 * @param pixelIndex
 *    Index of pixel
 * @param stereotaxicXyzOut
 *    Output with XYZ in stereotaxic
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::logicalPixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                             Vector3D& stereotaxicXyzOut) const
{
    
    PixelIndex pixelIndex(logicalPixelIndexToPixelIndex(pixelLogicalIndex));
    return pixelIndexToStereotaxicXYZ(pixelIndex,
                                      stereotaxicXyzOut);
}

/**
 * Convert a pixel index to a stereotaxic  XYZ.  
 * @param logicalX
 *    logtical X Index of pixel
 * @param logicalY
 *    logtical Y Index of pixel
 * @param stereotaxicXyzOut
 *    Output with XYZ in stereotaxic
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::logicalPixelIndexToStereotaxicXYZ(const float logicalX,
                                             const float logicalY,
                                             Vector3D& stereotaxicXyzOut) const
{
    const PixelLogicalIndex logicalIndex(logicalX,
                                         logicalY,
                                         0.0);
    return logicalPixelIndexToStereotaxicXYZ(logicalIndex,
                                             stereotaxicXyzOut);
}

/**
 * @return A pixel logical index converted from a pixel index.
 * @param pixelIndex
 *    The  pixel index.
 */
PixelLogicalIndex
MediaFileTransforms::pixelIndexToLogicalPixelIndex(const PixelIndex& pixelIndex) const
{
    PixelLogicalIndex pixelLogicalIndex(pixelIndex.getI() +  m_inputs.m_logicalBoundsRect.x(),
                                        pixelIndex.getJ() +  m_inputs.m_logicalBoundsRect.y(),
                                        pixelIndex.getK());
    
    return pixelLogicalIndex;
}

/**
 * Convert a pixel index to a plane XYZ.
 * @param pixelIndex
 *    Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::pixelIndexToPlaneXYZ(const PixelIndex& pixelIndex,
                                Vector3D& planeXyzOut) const
{
    if (m_inputs.m_pixelIndexToPlaneMatrixValidFlag) {
        Vector3D xyz(pixelIndex.getI(),
                     pixelIndex.getJ(),
                     0.0);
        m_inputs.m_pixelIndexToPlaneMatrix.multiplyPoint3(xyz);
        planeXyzOut = xyz;
        
        return true;
    }
    
    planeXyzOut = Vector3D();
    return false;
}

/**
 * convert a pixel index to a stereotaxic coordinate
 * @param pixelIndex
 *    Pixel index
 * @param xyzOut
 *    Output with the XYZ coordinate
 * @return
 *    True if conversion successful, else false.
 */
bool
MediaFileTransforms::pixelIndexToStereotaxicXYZ(const PixelIndex& pixelIndex,
                                                Vector3D& xyzOut) const
{
    Vector3D planeXYZ;
    if (pixelIndexToPlaneXYZ(pixelIndex, planeXYZ)) {
        if (planeXyzToStereotaxicXyz(planeXYZ, xyzOut)) {
            return true;
        }
    }
    
    xyzOut = Vector3D();
    return false;
}

/**
 * Convert a pixel XYZ to a logical pixel index.
 * @param planeXyz
 *     XYZ in plane
 * @param logicalPixelIndexOut
 *    Index of pixel
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::planeXyzToLogicalPixelIndex(const Vector3D& planeXyz,
                                       PixelLogicalIndex& pixelLogicalIndexOut) const
{
    PixelIndex pixelIndex;
    const bool validFlag(planeXyzToPixelIndex(planeXyz,
                                              pixelIndex));
    pixelLogicalIndexOut = pixelIndexToLogicalPixelIndex(pixelIndex);
    return validFlag;
}

/**
 * Convert a pixel XYZ to a pixel index.
 * @param planeXyz
 *     XYZ in plane
 * @param pixelIndexOut
 *    Index of pixel
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::planeXyzToPixelIndex(const Vector3D& planeXyz,
                                PixelIndex& pixelIndexOut) const
{
    if (m_inputs.m_planeToPixelIndexMatrixValidFlag) {
        Vector3D xyz(planeXyz);
        m_inputs.m_planeToPixelIndexMatrix.multiplyPoint3(xyz);
        pixelIndexOut.setIJK(xyz[0],
                             xyz[1],
                             0.0);
        return true;
    }
    
    pixelIndexOut = PixelIndex();
    return false;
}

/**
 * Convert a plane XYZ to stereotaxic XYZ
 * @param planeXyz
 *     XYZ in plane
 * @param stereotaxicXyzOut
 *    Output with stereotaxic XYZ
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                              Vector3D& stereotaxicXyzOut) const
{
    Vector3D dummyXYZ;
    return planeXyzToStereotaxicXyz(planeXyz,
                                    dummyXYZ,
                                    stereotaxicXyzOut);
}

/**
 * Convert a plane XYZ to stereotaxic XYZ
 * @param planeXyz
 *     XYZ in plane
 * @param stereotaxicNoNonLinearXyzOut
 *    Output with stereotaxic XYZ but without non-linear offset
 * @param stereotaxicXyzOut
 *    Output with stereotaxic XYZ
 * @return True if successful, else false.
 */
bool
MediaFileTransforms::planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                              Vector3D& stereotaxicNoNonLinearXyzOut,
                                              Vector3D& stereotaxicXyzOut) const
{
    if (m_inputs.m_planeToMillimetersMatrixValidFlag) {
        /*
         * Get the non-linear offset that is an offset in plane coordinates
         */
        Vector3D nonLinearOffsetXYZ(0.0, 0.0, 0.0);
        if (m_inputs.m_toStereotaxicNonLinearTransform) {
            m_inputs.m_toStereotaxicNonLinearTransform->getNonLinearOffset(planeXyz,
                                                                           nonLinearOffsetXYZ);
        }

        /*
         * Add non-linear offset to plane coordinate
         */
        const Vector3D newPlaneXYZ(planeXyz + nonLinearOffsetXYZ);
        
        /*
         * Transform plane coordinate to stereotaxic (mm) coordinate
         */
        stereotaxicXyzOut = newPlaneXYZ;
        m_inputs.m_planeToMillimetersMatrix.multiplyPoint3(stereotaxicXyzOut);
        
        stereotaxicNoNonLinearXyzOut = planeXyz;
        m_inputs.m_planeToMillimetersMatrix.multiplyPoint3(stereotaxicNoNonLinearXyzOut);
        if (debugFlag) {
            std::cout << "To stereotaxic (no non-linear): " << AString::fromNumbers(stereotaxicNoNonLinearXyzOut) << std::endl;
            std::cout << "               With non-linear: " << AString::fromNumbers(stereotaxicXyzOut) << std::endl;
            const float distance((stereotaxicXyzOut - stereotaxicNoNonLinearXyzOut).length());
            std::cout << "                      Distance: " << distance << "mm" << std::endl;
            std::cout << "                     With File: " << m_inputs.m_toStereotaxicNonLinearTransform->getFilename() << std::endl;
        }

        return true;
    }
    
    stereotaxicXyzOut = Vector3D();
    return false;
}

/**
 * Convert a stereotaxic xyz coordinate to a pixel index
 * @param xyz
 *    The coordinate
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param pixelLogicalIndexOut
 *    Output logical pixel index
 * @return
 *    True if successful, else false.
 */
bool
MediaFileTransforms::stereotaxicXyzToLogicalPixelIndex(const Vector3D& xyz,
                                             PixelLogicalIndex& pixelLogicalIndexOut) const
{
    PixelIndex pixelIndex;
    if (stereotaxicXyzToPixelIndex(xyz, pixelIndex)) {
        pixelLogicalIndexOut = pixelIndexToLogicalPixelIndex(pixelIndex);
        return true;
    }
    
    pixelLogicalIndexOut = PixelLogicalIndex();
    return false;
}

/**
 * Convert a stereotaxic xyz coordinate to a pixel index
 * @param xyz
 *    The coordinate
 * @param pixelIndexOut
 *    Output pixel index
 * @return
 *    True if successful, else false.
 */
bool
MediaFileTransforms::stereotaxicXyzToPixelIndex(const Vector3D& xyz,
                                      PixelIndex& pixelIndexOut) const
{
    Vector3D planeXYZ;
    if (stereotaxicXyzToPlaneXyz(xyz, planeXYZ)) {
        if (planeXyzToPixelIndex(planeXYZ, pixelIndexOut)) {
            return true;
        }
    }
    
    pixelIndexOut = PixelIndex();
    return false;
}

/**
 * Converrt a stereotaxic coordinate to a plane coordinate
 * @param stereotaxicXyz
 *    Input stereotaxic coordinate
 * @param planeXyzOut
 *    Output plane coordinate
 * @return True if successful, else false
 */
bool
MediaFileTransforms::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeXyzOut) const
{
    Vector3D dummyXYZ;
    return stereotaxicXyzToPlaneXyz(stereotaxicXyz,
                                    dummyXYZ,
                                    planeXyzOut);
}

/**
 * Converrt a stereotaxic coordinate to a plane coordinate
 * @param stereotaxicXyz
 *    Input stereotaxic coordinate
 * @param planeNoNonLinearXyzOut
 *    Output plane coordinate without any non-linear correction
 * @param planeXyzOut
 *    Output plane coordinate
 * @return True if successful, else false
 */
bool
MediaFileTransforms::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeNoNonLinearXyzOut,
                                              Vector3D& planeXyzOut) const
{
    if (m_inputs.m_millimetersToPlaneMatrixValidFlag) {
        if (debugFlag) {
            const Matrix4x4& m(m_inputs.m_planeToMillimetersMatrix);
            const Vector3D planeivec(m.getMatrixElement(0, 0),
                                     m.getMatrixElement(1, 0),
                                     m.getMatrixElement(2, 0));

            const Vector3D planejvec(m.getMatrixElement(0, 1),
                                     m.getMatrixElement(1, 1),
                                     m.getMatrixElement(2, 1));

            const Vector3D planeorigin(m.getMatrixElement(0, 3),
                                       m.getMatrixElement(1, 3),
                                       m.getMatrixElement(2, 3));
            const Vector3D invplaneivec(planeivec / planeivec.lengthsquared());
            const Vector3D invplanejvec(planejvec / planejvec.lengthsquared());
            const float planepointi((stereotaxicXyz - planeorigin).dot(invplaneivec));
            const float planepointj((stereotaxicXyz - planeorigin).dot(invplanejvec));

            
            std::cout << "Plane to MM: " << m_inputs.m_planeToMillimetersMatrix.toString() << std::endl;
            std::cout << "plainpointi, j: " << planepointi << ", " << planepointj << std::endl;
        }

        
        /*
         * Transform stereotaxic coordinate to a plane coordinate
         */
        Vector3D planeXYZ(stereotaxicXyz);
        m_inputs.m_millimetersToPlaneMatrix.multiplyPoint3(planeXYZ);
        if (debugFlag) {
            std::cout << "plane xyz from matrix: " << AString::fromNumbers(planeXYZ) << std::endl;
        }
        planeNoNonLinearXyzOut = planeXYZ;
        planeNoNonLinearXyzOut[2] = m_inputs.m_planeZ;
        
        /*
         * Sanity check
         */
        if (debugFlag) {
            if (m_inputs.m_invertedPlaneToMillimetersMatrixValidFlag) {
                Vector3D xyz(stereotaxicXyz);
                m_inputs.m_invertedPlaneToMillimetersMatrix.multiplyPoint3(xyz);
                std::cout << "Inverted plane to mm matrix: " << AString::fromNumbers(xyz) << std::endl;
            }
        }

        /*
         * Get the non-linear offset that is an offset in plane coordinates
         */
        Vector3D nonLinearOffsetXYZ;
        if (m_inputs.m_fromStereotaxicNonLinearTransform) {
            m_inputs.m_fromStereotaxicNonLinearTransform->getNonLinearOffset(planeXYZ,
                                                                             nonLinearOffsetXYZ);
        }
        
        /*
         * Final plane coordinate (plane + plane non-linear offset)
         */
        planeXYZ[2] = m_inputs.m_planeZ;
        planeXyzOut = planeXYZ + nonLinearOffsetXYZ;
        
        if (debugFlag) {
            std::cout << "To Plane (no non-linear): " << AString::fromNumbers(planeXYZ) << std::endl;
            std::cout << "         With non-linear: " << AString::fromNumbers(planeXyzOut) << std::endl;
            const float distance(nonLinearOffsetXYZ.length());
            std::cout << "                Distance: " << distance << "" << std::endl;
            std::cout << "               With File: " << m_inputs.m_fromStereotaxicNonLinearTransform->getFilename() << std::endl;
        }

        return true;
    }
    
    planeXyzOut = Vector3D();
    planeNoNonLinearXyzOut = Vector3D();
    return false;
}

/**
 * @return True if plane to millimeters matrix is valid
 */
bool
MediaFileTransforms::isPlaneToMillimetersMatrixValid() const
{
    return m_inputs.m_planeToMillimetersMatrixValidFlag;
}

/**
 * @return Matrix that converts from plane coordiantes to millimeters
 */
Matrix4x4
MediaFileTransforms::getPlaneToMillimetersMatrix() const
{
    return m_inputs.m_planeToMillimetersMatrix;
}

void
MediaFileTransforms::testTransforms()
{
    const Vector3D planeA(125, 250, m_inputs.m_planeZ);
    Vector3D xyz;
    Vector3D xyzWithNonLinearOffset;
    if (planeXyzToStereotaxicXyz(planeA, xyz, xyzWithNonLinearOffset)) {
        Vector3D backToPlaneA;
        if (stereotaxicXyzToPlaneXyz(xyz, backToPlaneA)) {
            const float dist((planeA - backToPlaneA).length());
            if (dist > 0.0005) {
                CaretLogSevere("Plane to xyz and back to plane failed");
            }
            std::cout << "Plane to xyz and back to plane length: " << dist << std::endl;
        }
        
        const Vector3D tl(0, 0, 0);
        const Vector3D bl(0, 100, 0);
        const Vector3D br(100, 100, 0);
        Vector3D tlXYZ, blXYZ, brXYZ, dummyXYZ;
        if (planeXyzToStereotaxicXyz(tl, tlXYZ, dummyXYZ)
            && planeXyzToStereotaxicXyz(bl, blXYZ, dummyXYZ)
            && planeXyzToStereotaxicXyz(br, brXYZ, dummyXYZ)) {
            Plane plane(tlXYZ, blXYZ, brXYZ);
            if (plane.isValidPlane()) {
                Vector3D normalVector;
                plane.getNormalVector(normalVector);
                const Vector3D offsetVector(normalVector * 10.0);
                const Vector3D offsetXYZ(xyz + offsetVector);
                Vector3D offsetPlaneXYZ;
                Vector3D withNonLinearOffsetXYZ;
                if (stereotaxicXyzToPlaneXyz(offsetXYZ, offsetPlaneXYZ, withNonLinearOffsetXYZ)) {
                    const float dist((planeA - offsetPlaneXYZ).length());
                    if (dist > 0.0005) {
                        CaretLogSevere("Plane to xyz (with offset from plane) and back to plane failed.");
                    }
                    std::cout << "Plane to xyz (with offset from plane) and back to plane: " << dist << std::endl;
                }
            }
        }
    }
}

/**
 * Inputs constructor.  Computes additional matrices.
 */
MediaFileTransforms::Inputs::Inputs(const QRectF& logicalBoundsRect,
                                    const Matrix4x4& pixelIndexToPlaneMatrix,
                                    const bool pixelIndexToPlaneMatrixValidFlag,
                                    const Matrix4x4& planeToMillimetersMatrix,
                                    const bool planeToMillimetersMatrixValidFlag,
                                    std::shared_ptr<CziNonLinearTransform>& toStereotaxicNonLinearTransform,
                                    std::shared_ptr<CziNonLinearTransform>& fromStereotaxicNonLinearTransform)
: m_logicalBoundsRect(logicalBoundsRect),
m_pixelIndexToPlaneMatrix(pixelIndexToPlaneMatrix),
m_pixelIndexToPlaneMatrixValidFlag(pixelIndexToPlaneMatrixValidFlag),
m_planeToMillimetersMatrix(planeToMillimetersMatrix),
m_planeToMillimetersMatrixValidFlag(planeToMillimetersMatrixValidFlag),
m_toStereotaxicNonLinearTransform(toStereotaxicNonLinearTransform),
m_fromStereotaxicNonLinearTransform(fromStereotaxicNonLinearTransform)
{
    if (m_pixelIndexToPlaneMatrixValidFlag) {
        /*
         * 'Plane to Pixel' is inversion of 'Pixel to Plane'
         */
        //TSC: 2D to 2D matrices should not be invertible as a 3D matrix, force dummy third dim to identity before inversion
        m_pixelIndexToPlaneMatrix.setMatrixElement(2, 0, 0.0);
        m_pixelIndexToPlaneMatrix.setMatrixElement(2, 1, 0.0);
        m_pixelIndexToPlaneMatrix.setMatrixElement(2, 2, 1.0);
        m_pixelIndexToPlaneMatrix.setMatrixElement(2, 3, 0.0);
        //std::cout << m_pixelIndexToPlaneMatrix.toString() << std::endl;
        m_planeToPixelIndexMatrix = m_pixelIndexToPlaneMatrix;
        m_planeToPixelIndexMatrixValidFlag = m_planeToPixelIndexMatrix.invert();
        if ( ! m_planeToPixelIndexMatrixValidFlag) {
            CaretLogSevere("Inversion of pixel to plane matrix failed");
        }
    }
    
    if (m_planeToMillimetersMatrixValidFlag) {
        /*
         * Inverting the plane to millimeters matrix will not work for millimeters
         * to plane conversion if point is not on image plane.  But we will
         * compute it and use it for sanity checks during development.
         */
        //TSC: XML reader should not spit out an invertible matrix when reading a 2D to 3D matrix, because that needs math, do the math here instead
        Vector3D ivec(m_planeToMillimetersMatrix.getMatrixElement(0, 0),
                      m_planeToMillimetersMatrix.getMatrixElement(1, 0),
                      m_planeToMillimetersMatrix.getMatrixElement(2, 0));
        Vector3D jvec(m_planeToMillimetersMatrix.getMatrixElement(0, 1),
                      m_planeToMillimetersMatrix.getMatrixElement(1, 1),
                      m_planeToMillimetersMatrix.getMatrixElement(2, 1));
        Vector3D kvec = ivec.cross(jvec).normal();
        m_planeToMillimetersMatrix.setMatrixElement(0, 2, kvec[0]);
        m_planeToMillimetersMatrix.setMatrixElement(1, 2, kvec[1]);
        m_planeToMillimetersMatrix.setMatrixElement(2, 2, kvec[2]);
        //std::cout << m_planeToMillimetersMatrix.toString() << std::endl;
        m_invertedPlaneToMillimetersMatrix = m_planeToMillimetersMatrix;
        m_invertedPlaneToMillimetersMatrixValidFlag = m_invertedPlaneToMillimetersMatrix.invert();
        if ( ! m_invertedPlaneToMillimetersMatrixValidFlag) {
            CaretLogSevere("Inversion of plane to millimeters matrix failed");
        }

        /*
         * Compute the millimeters to plane matrix
         */
        m_millimetersToPlaneMatrixValidFlag = computeMillimetersToPlaneMatrix(m_planeToMillimetersMatrix,
                                                                              m_millimetersToPlaneMatrix);
        /*
         * In event computation of millimeters to plane matrix fails,
         * invert plane to millimeters matrix but it will not work
         * correctly if a point is not on the plane of the image
         */
        
        if ( ! m_millimetersToPlaneMatrixValidFlag) {
            /*
             * 'Millimeter to Plane' matrix is inversion of 'Plane to Millimeters'
             */
            m_millimetersToPlaneMatrix = m_invertedPlaneToMillimetersMatrix;
            m_millimetersToPlaneMatrixValidFlag = m_invertedPlaneToMillimetersMatrixValidFlag;
        }
    }

    /*
     * Computer plane's Z coordinate
     */
    Vector3D pixel(0.0, 0.0, 0.0);
    if (m_pixelIndexToPlaneMatrixValidFlag) {
        m_pixelIndexToPlaneMatrix.multiplyPoint3(pixel);
        m_planeZ = pixel[2];
    }
}

/**
 * Compute the 'millimeters to plane matrix'
 * @param planeToMillimetersMatrix
 *   The plane to millimeters matrix
 * @param millimetersToPlaneMatrixOut
 * @return True if the output matrix is valid, else false.
 */
bool
MediaFileTransforms::Inputs::computeMillimetersToPlaneMatrix(const Matrix4x4& planeToMillimetersMatrix,
                                                             Matrix4x4& millimetersToPlaneMatrixOut) const
{
    /*
     * We cannot simply invert the planeToMillimetersMatrix as points
     * in stereotaxic coordinates may not be on the image's plane.
     *
     * From Tim Coalson:
     * There is a more general solution for inverting the plane2mm matrix
     * (which would be necessary if we included stretching components from
     * regressing the affine from the warpfield): insert a 3rd dimension
     * from the cross product of the two existing dimensions, and then do
     * a standard 3D matrix inversion:
     *    planeivec = [plane2mm(1, 1), plane2mm(2, 1), plane2mm(3, 1)]
     *    planejvec = [plane2mm(1, 2), plane2mm(2, 2), plane2mm(3, 2)]
     *    newkvec = planeivec.cross(planejvec).normal()
     *    plane3d = [[plane2mm(1:3, 1:2), newkvec(:), plane2mm(1:3, 3)]; 0 0 0 1]
     */
    Matrix4x4 plane3d;
    const bool swapRowColFlag(false);
    if (swapRowColFlag) {
        /*
         * This matrix seems to work when we go from
         * plane to stereotaxic back to plane (the
         * plane coordinates match).
         */
        const Matrix4x4& m(planeToMillimetersMatrix);
        const Vector3D planeivec(m.getMatrixElement(0, 0),
                                 m.getMatrixElement(0, 1),
                                 m.getMatrixElement(0, 2));
        
        const Vector3D planejvec(m.getMatrixElement(1, 0),
                                 m.getMatrixElement(1, 1),
                                 m.getMatrixElement(1, 2));
        const Vector3D newkvec(planeivec.cross(planejvec).normal());
        
        plane3d = planeToMillimetersMatrix;
        plane3d.setMatrixElement(2, 0, newkvec[0]);
        plane3d.setMatrixElement(2, 1, newkvec[1]);
        plane3d.setMatrixElement(2, 2, newkvec[2]);
        if (debugFlag) {
            std::cout << "xxx plane3d: " << plane3d.toString() << std::endl;
            std::cout << "xxx planeToMM: " << m_planeToMillimetersMatrix.toString() << std::endl;
        }
    }
    else {
        const Matrix4x4& m(planeToMillimetersMatrix);
        const Vector3D planeivec(m.getMatrixElement(0, 0),
                                 m.getMatrixElement(1, 0),
                                 m.getMatrixElement(2, 0));
        
        const Vector3D planejvec(m.getMatrixElement(0, 1),
                                 m.getMatrixElement(1, 1),
                                 m.getMatrixElement(2, 1));
        const Vector3D newkvec(planeivec.cross(planejvec).normal());
        
        plane3d = planeToMillimetersMatrix;
        plane3d.setMatrixElement(0, 2, newkvec[0]);
        plane3d.setMatrixElement(1, 2, newkvec[1]);
        plane3d.setMatrixElement(2, 2, newkvec[2]);
        if (debugFlag) {
            std::cout << "plane3d: " << plane3d.toString() << std::endl;
            std::cout << "planeToMM: " << m_planeToMillimetersMatrix.toString() << std::endl;
        }
    }
    
    /*
     * Invert the plane3d matrix to get the
     * millimeters to plane matrix
     */
    millimetersToPlaneMatrixOut = plane3d;
    if (millimetersToPlaneMatrixOut.invert()) {
        return true;
    }
    
    CaretLogSevere("Computation of millimeters to plane matrix failed");
    millimetersToPlaneMatrixOut.identity();
    return false;
}

