
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __MEDIA_FILE_DECLARE__
#include "MediaFile.h"
#undef __MEDIA_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziUtilities.h"
#include "DataFileContentInformation.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::MediaFile 
 * \brief Base class for media type files (image, movie)
 * \ingroup Files
 */

/**
 * Constructor.
 * @param dataFileType
 *    Type of data file
 */
MediaFile::MediaFile(const DataFileTypeEnum::Enum dataFileType)
: CaretDataFile(dataFileType)
{
    switch (dataFileType) {
        case DataFileTypeEnum::CZI_IMAGE_FILE:
        case DataFileTypeEnum::IMAGE:
            break;
        default:
        {
            const AString msg("Invalid data file type="
                               + DataFileTypeEnum::toName(dataFileType)
                               + ".  Has new file type been added?");
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
        }
    }
    
    initializeMembersMediaFile();
    
}

/**
 * Copy constructor.
 * @param mediaFile
 *    Media file that is copied.
 */
MediaFile::MediaFile(const MediaFile& mediaFile)
: CaretDataFile(mediaFile)
{
    initializeMembersMediaFile();
}

/**
 * Destructor.
 */
MediaFile::~MediaFile()
{
}

/**
 * Initialize members of media file
 */
void
MediaFile::initializeMembersMediaFile()
{
    resetMatricesPrivate();
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
}


/**
 * @return Name of frame at given index.
 * @param frameIndex Index of the frame
 */
AString
MediaFile::getFrameName(const int32_t frameIndex) const
{
    CaretAssert((frameIndex >= 0) && (frameIndex < getNumberOfFrames()));
    const AString defaultFrameName(AString::number(frameIndex+1));
    return defaultFrameName;
}

/**
 * @return The units for the 'interval' between two consecutive frames.
 */
NiftiTimeUnitsEnum::Enum
MediaFile::getFrameIntervalUnits() const
{
    return NiftiTimeUnitsEnum::NIFTI_UNITS_UNKNOWN;
}

/**
 * Get the units value for the first frame and the
 * quantity of units between consecutive frames.  If the
 * units for the frame is unknown, value of one (1) are
 * returned for both output values.
 *
 * @param firstFrameUnitsValueOut
 *     Output containing units value for first frame.
 * @param frameIntervalStepValueOut
 *     Output containing number of units between consecutive frame.
 */
void
MediaFile::getFrameIntervalStartAndStep(float& firstFrameUnitsValueOut,
                                          float& frameIntervalStepValueOut) const
{
    firstFrameUnitsValueOut   = 1.0;
    frameIntervalStepValueOut = 1.0;
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
MediaFile::getStructure() const
{
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
MediaFile::setStructure(const StructureEnum::Enum /*structure */)
{
    /* File does not support structures */
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
MediaFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    CaretDataFile::saveFileDataToScene(sceneAttributes,
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
MediaFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    CaretDataFile::restoreFileDataFromScene(sceneAttributes,
                                            sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return File casted to an media file (avoids use of dynamic_cast that can be slow)
 * Overidden in MediaFile
 */
MediaFile*
MediaFile::castToMediaFile()
{
    return this;
}

/**
 * @return File casted to an media file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const MediaFile*
MediaFile::castToMediaFile() const
{
    return this;
}

/**
 * @return A pixel index converted from a pixel logical index.
 * @param pixelLogicalIndex
 *    The logical pixel index.
 */
PixelIndex
MediaFile::pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const
{
    PixelIndex pixelIndex(pixelLogicalIndex.getI(),
                          pixelLogicalIndex.getJ(),
                          pixelLogicalIndex.getK());
    
    return pixelIndex;
}

/**
 * @return A pixel logical index converted from a pixel index.
 * @param pixelIndex
 *    The  pixel index.
 */
PixelLogicalIndex
MediaFile::pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const
{
    PixelLogicalIndex pixelLogicalIndex(pixelIndex.getI(),
                                        pixelIndex.getJ(),
                                        pixelIndex.getK());
    
    return pixelLogicalIndex;
}

/**
 * @return The Rect defining the plane coordinates.
 * Bounds are always parallel to X and Y axes.  If there is rotation due to
 * 'pixel to plane' matrix, bounds encloses image data.
 */
QRectF
MediaFile::getPlaneXyzRect() const
{
    return m_planeXyzRect;
}

/**
 * Convert a pixel index to a plane XYZ.  If not supported, output is same as input.
 * @param pixelIndex
 *    Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFile::pixelIndexToPlaneXYZ(const PixelIndex& pixelIndex,
                                  Vector3D& planeXyzOut) const
{
    if (m_pixelIndexToPlaneMatrixValidFlag) {
        /*
         * NOTE: Matrix has X and Y swapped
         */
        Vector3D xyz(pixelIndex.getJ(),
                     pixelIndex.getI(),
                     1.0);
        m_pixelIndexToPlaneMatrix.multiplyPoint3(xyz);
        planeXyzOut[0] = xyz[1];
        planeXyzOut[1] = xyz[0];
        planeXyzOut[2] = xyz[2];
        
        return true;
    }
    
    planeXyzOut[0] = pixelIndex.getI();
    planeXyzOut[1] = pixelIndex.getJ();
    planeXyzOut[2] = pixelIndex.getK();
    return false;
}

/**
 * Convert a pixel index to a plane XYZ.  If not supported, output is same as input.
 * @param pixelIndex
 *    Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFile::logicalPixelIndexToPlaneXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                         Vector3D& planeXyzOut) const
{
    PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    return pixelIndexToPlaneXYZ(pixelIndex,
                                planeXyzOut);
}

/**
 * Convert a pixel index to a plane XYZ.  If not supported, output is same as input.
 * @param logicalX
 *    logtical X Index of pixel
 * @param logicalY
 *    logtical Y Index of pixel
 * @param planeXyzOut
 *    Output with XYZ in plane
 * @return True if successful, else false.
 */
bool
MediaFile::logicalPixelIndexToPlaneXYZ(const float logicalX,
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
 * Convert a pixel XYZ to a pixel index.  If not supported, output is same as input.
 * @param planeXyz
 *     XYZ in plane
 * @param pixelIndexOut
 *    Index of pixel
 * @return True if successful, else false.
 */
bool
MediaFile::planeXyzToPixelIndex(const Vector3D& planeXyz,
                                  PixelIndex& pixelIndexOut) const
{
    if (m_planeToPixelIndexMatrixValidFlag) {
        /*
         * NOTE: Matrix has X and Y swapped
         */
        Vector3D xyz(planeXyz[1],
                     planeXyz[0],
                     planeXyz[2]);
        m_planeToPixelIndexMatrix.multiplyPoint3(xyz);
        pixelIndexOut.setIJK(xyz[1],
                             xyz[0],
                             0.0);
        return true;
    }
    
    pixelIndexOut.setIJK(planeXyz);
    return false;
}

/**
 * Convert a pixel XYZ to a logical pixel index.  If not supported, output is same as input.
 * @param planeXyz
 *     XYZ in plane
 * @param logicalPixelIndexOut
 *    Index of pixel
 * @return True if successful, else false.
 */
bool
MediaFile::planeXyzToLogicalPixelIndex(const Vector3D& planeXyz,
                                         PixelLogicalIndex& pixelLogicalIndexOut) const
{
    PixelIndex pixelIndex;
    const bool validFlag(planeXyzToPixelIndex(planeXyz,
                                              pixelIndex));
    pixelLogicalIndexOut = pixelIndexToPixelLogicalIndex(pixelIndex);
    return validFlag;
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
MediaFile::planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                    Vector3D& stereotaxicXyzOut) const
{
    if (m_planeToMillimetersMatrixValidFlag) {
        /*
         * Note: Matrix has X & Y swapped
         */
        Vector3D vec(planeXyz);
        if (m_planeMillimetersSwapFlag) {
            swapVectorXY(vec);
        }
        m_planeToMillimetersMatrix.multiplyPoint3(vec);
        if (m_planeMillimetersSwapFlag) {
            swapVectorXY(vec);
        }
        stereotaxicXyzOut = vec;
        return true;
    }
    
    stereotaxicXyzOut = Vector3D();
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
MediaFile::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                    Vector3D& planeXyzOut) const
{
    if (m_millimetersToPlaneMatrixValidFlag) {
        /*
         * Note: Matrix has X & Y swapped
         */
        Vector3D vec(stereotaxicXyz);
        if (m_planeMillimetersSwapFlag) {
            swapVectorXY(vec);
        }
        m_millimetersToPlaneMatrix.multiplyPoint3(vec);
        if (m_planeMillimetersSwapFlag) {
            swapVectorXY(vec);
        }
        planeXyzOut = vec;

        return true;
    }
    
    planeXyzOut = Vector3D();
    return false;
}

/**
 * Reset the matrices.
 */
void
MediaFile::resetMatricesPrivate()
{
    m_planeXyzRect = QRectF();
    m_planeBoundingBox.resetZeros();
    m_scaledToPlaneMatrix.identity();
    m_pixelIndexToPlaneMatrix.identity();
    m_planeToPixelIndexMatrix.identity();
    m_planeToMillimetersMatrix.identity();
    m_millimetersToPlaneMatrix.identity();
    
    m_planeXyzTopLeft     = Vector3D();
    m_planeXyzTopRight    = Vector3D();
    m_planeXyzBottomLeft  = Vector3D();
    m_planeXyzBottomRight = Vector3D();
    
    m_scaledToPlaneMatrixValidFlag      = false;
    m_pixelIndexToPlaneMatrixValidFlag  = false;
    m_planeToPixelIndexMatrixValidFlag  = false;
    m_planeToMillimetersMatrixValidFlag = false;
    m_millimetersToPlaneMatrixValidFlag = false;
}

/**
 * Reset the matrices.  This method is virtual so cannot call from constructor.
 */
void
MediaFile::resetMatrices()
{
    resetMatricesPrivate();
}

/**
 * @return True if scaled to plane matrix is valid
 */
bool
MediaFile::isScaledToPlaneMatrixValid() const
{
    return m_scaledToPlaneMatrixValidFlag;
}


/**
 * @return The scaled to plane matrix
 * converts normalized pixel (0 to 1) to plane coordinate
 * @param validFlagOut
 *    Pointer to boolean.  If not NULL, will be true if matrix is valid, else false.
 */
Matrix4x4
MediaFile::getScaledToPlaneMatrix(bool* validFlagOut) const
{
    if (validFlagOut != NULL) {
        *validFlagOut = m_scaledToPlaneMatrixValidFlag;
    }
    return m_scaledToPlaneMatrix;
}

/**
 * @return True if pixel index to plane matrix is valid
 */
bool
MediaFile::isPixelIndexToPlaneMatrixValid() const
{
    return m_pixelIndexToPlaneMatrixValidFlag;
}


/**
 * @return The pixel to plane matrix
 * Converts a pixel specified by [0 to 1] for X/Y to "plane coordinates"
 * @param validFlagOut
 *    Pointer to boolean.  If not NULL, will be true if matrix is valid, else false.
 */
Matrix4x4
MediaFile::getPixelIndexToPlaneMatrix(bool* validFlagOut) const
{
    if (validFlagOut != NULL) {
        *validFlagOut = m_pixelIndexToPlaneMatrixValidFlag;
    }
    return m_pixelIndexToPlaneMatrix;
}

/**
 * @return True if plane to pixel index matrix is valid
 */
bool
MediaFile::isPlaneToPixelIndexMatrixValid() const
{
    return m_planeToPixelIndexMatrixValidFlag;
}

/**
 * @return The plane to pixel matrix
 * Converts plane coordinate to a pixel specified by [0 to 1]
 * @param validFlagOut
 *    Pointer to boolean.  If not NULL, will be true if matrix is valid, else false.
 */
Matrix4x4
MediaFile::getPlaneToPixelIndexMatrix(bool* validFlagOut) const
{
    if (validFlagOut != NULL) {
        *validFlagOut = m_planeToPixelIndexMatrixValidFlag;
    }
   return m_planeToPixelIndexMatrix;
}

/**
 * @return True if plane to millimeters matrix is valid
 */
bool
MediaFile::isPlaneToMillimetersMatrixValid() const
{
    return m_planeToMillimetersMatrixValidFlag;
}

/**
 * @return Matrix that converts from plane coordiantes to millimeters
 * @param validFlagOut
 *    Pointer to boolean.  If not NULL, will be true if matrix is valid, else false.
 */
Matrix4x4
MediaFile::getPlaneToMillimetersMatrix(bool* validFlagOut) const
{
    if (validFlagOut != NULL) {
        *validFlagOut = m_planeToMillimetersMatrixValidFlag;;
    }
    return m_planeToMillimetersMatrix;
}

/**
 * @return True if millimeters to plane matrix is valid
 */
bool
MediaFile::isMillimetersToPlaneMatrixValid() const
{
    return m_millimetersToPlaneMatrixValidFlag;
}

/**
 * @return Matrix that converts from millimeters coordiantes to plane coordinates
 * @param validFlagOut
 *    Pointer to boolean.  If not NULL, will be true if matrix is valid, else false.
 */
Matrix4x4
MediaFile::getMillimetersToPlaneMatrix(bool* validFlagOut) const
{
    if (validFlagOut != NULL) {
        *validFlagOut = m_millimetersToPlaneMatrixValidFlag;;
    }
    return m_millimetersToPlaneMatrix;
}

/**
 * Set the matrix for display drawing.
 * @param scaledToPlaneMatrix
 *    The scaled to plane matrix.
 * @param planeToMillimetersMatrix
 *    Matrix for converting from plane coords to millimeter coords
 * @param matixValidFlag
 *    True if the matrix is valid.
 */
void
MediaFile::setScaledToPlaneMatrix(const Matrix4x4& scaledToPlaneMatrix,
                                  const Matrix4x4& planeToMillimetersMatrix,
                                  const bool matixValidFlag)
{
    resetMatrices();
    
    if ( ! matixValidFlag) {
        return;
    }
    
    m_planeToMillimetersMatrix = planeToMillimetersMatrix;
    m_planeToMillimetersMatrixValidFlag = true;
    
    m_millimetersToPlaneMatrix = m_planeToMillimetersMatrix;
    if (m_millimetersToPlaneMatrix.invert()) {
        m_millimetersToPlaneMatrixValidFlag = true;
    }
    
    /*
     * Translate by 1/2 pixel (move to center of pixel)
     */
    Matrix4x4 shiftMat;
    shiftMat.identity();
    shiftMat.setTranslation(0.5,
                            0.5,
                            0.0);
    
    /*
     * Matrix to convert pixel range so zero to one
     */
    const float scaleFactor(std::max(getWidth(), getHeight()));
    CaretAssert(scaleFactor >= 1.0);
    Matrix4x4 scaleMat;
    scaleMat.identity();
    scaleMat.scale((1.0 / scaleFactor),
                   (1.0 / scaleFactor),
                   1.0);
    
    m_scaledToPlaneMatrix = scaledToPlaneMatrix;
    m_scaledToPlaneMatrixValidFlag = true;
    
    Matrix4x4 indexToPlane = scaledToPlaneMatrix;
    indexToPlane.premultiply(scaleMat);
    indexToPlane.premultiply(shiftMat);
    
    
    const bool testFlag(false);
    if (testFlag) {
        std::cout << "---- File: " << getFileName() << std::endl;
        std::cout << "Shift Mat: " << shiftMat.toString() << std::endl;
        std::cout << "Scale Mat: " << scaleMat.toString() << std::endl;
        std::cout << "ScaledToPlane: " << scaledToPlaneMatrix.toString() << std::endl;
        std::cout << "PlaneToMM" << m_planeToMillimetersMatrix.toString() << std::endl;
        std::cout << "Index to Plane: " << indexToPlane.toString() << std::endl;
        std::cout << "Start Index to Plane ----------" << std::endl;
        indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, 0, 0, "top left");
        indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, getWidth() - 1, 0, "top right");
        indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, 0, getHeight() - 1, "bottom left");
        indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, getWidth() - 1, getHeight() - 1, "bottom right");
        
        const float w(getWidth() - 1);
        const float h(getHeight() - 1);
        lengthsTest(indexToPlane, 0, 0, w, 0, "top");
        lengthsTest(indexToPlane, 0, h, w, h, "bottom");
        lengthsTest(indexToPlane, 0, 0, 0, h, "left");
        lengthsTest(indexToPlane, 0, w, w, h, "right");
        
        std::cout << "   -- Separate ----------" << std::endl;
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, m_planeToMillimetersMatrix, 0, 0, "top left");
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, m_planeToMillimetersMatrix, getWidth() - 1, 0, "top right");
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, m_planeToMillimetersMatrix, 0, getHeight() - 1, "bottom left");
        indexToPlaneTest(scaledToPlaneMatrix, shiftMat, scaleMat, m_planeToMillimetersMatrix, getWidth() - 1, getHeight() - 1, "bottom right");
        std::cout << "   -------------------------------" << std::endl;
    }
    
    m_pixelIndexToPlaneMatrix = indexToPlane;
    m_pixelIndexToPlaneMatrixValidFlag = true;
    
    m_planeToPixelIndexMatrix = m_pixelIndexToPlaneMatrix;
    if (m_planeToPixelIndexMatrix.invert()) {
        m_planeToPixelIndexMatrixValidFlag = true;
    }
    
    if (m_pixelIndexToPlaneMatrixValidFlag) {
        const QRectF logicalRect(getLogicalBoundsRect());
        const PixelLogicalIndex topLeft(logicalRect.left(),
                                        logicalRect.top(),
                                        0);
        const PixelLogicalIndex topRight(logicalRect.right(),
                                         logicalRect.top(),
                                         0);
        const PixelLogicalIndex bottomLeft(logicalRect.left(),
                                           logicalRect.bottom(),
                                           0);
        const PixelLogicalIndex bottomRight(logicalRect.right(),
                                            logicalRect.bottom(),
                                            0);
        
        logicalPixelIndexToPlaneXYZ(topLeft,     m_planeXyzTopLeft);
        logicalPixelIndexToPlaneXYZ(topRight,    m_planeXyzTopRight);
        logicalPixelIndexToPlaneXYZ(bottomLeft,  m_planeXyzBottomLeft);
        logicalPixelIndexToPlaneXYZ(bottomRight, m_planeXyzBottomRight);
        
        m_planeBoundingBox.resetForUpdate();
        m_planeBoundingBox.update(m_planeXyzTopLeft);
        m_planeBoundingBox.update(m_planeXyzTopRight);
        m_planeBoundingBox.update(m_planeXyzBottomLeft);
        m_planeBoundingBox.update(m_planeXyzBottomRight);

        m_planeXyzRect.setLeft(m_planeBoundingBox.getMinX());
        m_planeXyzRect.setRight(m_planeBoundingBox.getMaxX());
        m_planeXyzRect.setTop(m_planeBoundingBox.getMinY());
        m_planeXyzRect.setBottom(m_planeBoundingBox.getMaxY());
        
        const bool testFlag2(false);
        if (testFlag2) {
            std::cout << "---- File: " << getFileName() << std::endl;
            indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, 0, 0, "top left");
            indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, getWidth() - 1, 0, "top right");
            indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, 0, getHeight() - 1, "bottom left");
            indexToPlaneTest(indexToPlane, m_planeToMillimetersMatrix, getWidth() - 1, getHeight() - 1, "bottom right");
            std::cout << "Plane bounding box: " << m_planeBoundingBox.toString() << std::endl;
            std::cout << "Plane XYZ Rect: " << CziUtilities::qRectToString(m_planeXyzRect) << std::endl;
        }
    }
    
    if (testFlag) {
        std::cout << "-------------------------------" << std::endl;
    }
}

void
MediaFile::indexToPlaneTest(const Matrix4x4& indexToPlane,
                            const Matrix4x4& planeToMM,
                            const int32_t i,
                            const int32_t j,
                            const AString& name)
{
    Vector3D ij(j,
                i,
                1.0f);
    
    Vector3D xy(ij);
    indexToPlane.multiplyPoint3(xy);
    float y = xy[0];  /* yes correct Y is first */
    float x = xy[1];
    std::cout << name << " plane i, j, k: " << i << ", " << j << " xyz: " << x << ", " << y << ", " << xy[2] << std::endl;
    
    planeToMM.multiplyPoint3(xy);
    y = xy[0];
    x = xy[1];
    std::cout << "         mm x, y, z: " << i << ", " << j << " xyz: " << x << ", " << y << ", " << xy[2] << std::endl;
}

void
MediaFile::lengthsTest(const Matrix4x4& indexToPlane,
                       const int32_t i1,
                       const int32_t j1,
                       const int32_t i2,
                       const int32_t j2,
                       const AString& name)
{
    const Vector3D v1Pixel(i1, j1, 0.0);
    const Vector3D v2Pixel(i2, j2, 0.0);
    Vector3D v1(v1Pixel);
    Vector3D v2(v2Pixel);
    indexToPlane.multiplyPoint3(v1);
    indexToPlane.multiplyPoint3(v2);
    const Vector3D dxy(v2 - v1);
    std::cout << name << " Index (" << AString::fromNumbers(v1Pixel) << ") to (" << AString::fromNumbers(v2Pixel) << ") "
    << " Plane XYZ ( " << AString::fromNumbers(v1) << ") to (" << AString::fromNumbers(v2) << ") length: "
    << dxy.length() << std::endl;
}

void
MediaFile::indexToPlaneTest(const Matrix4x4& scaledToPlane,
                            const Matrix4x4& shiftMat,
                            const Matrix4x4& scaleMat,
                            const Matrix4x4& planeToMM,
                            const int32_t i,
                            const int32_t j,
                            const AString& name)
{
    const Vector3D ij(j,
                      i,
                      1.0f);
    Vector3D xy(j,
                i,
                1.0f);
    
    std::cout << "Input (i, j): " << i << ", " << j << " " << name << std::endl;
    shiftMat.multiplyPoint3(xy);
    std::cout << "   After Shift (x, y, z): " << xy[1] << ", " << xy[0] << ", " << xy[2] <<std::endl;
    scaleMat.multiplyPoint3(xy);
    std::cout << "   After scale (x, y, z): " << xy[1] << ", " << xy[0] << ", " << xy[2] <<std::endl;
    scaledToPlane.multiplyPoint3(xy);
    std::cout << "   Result (x, y, z): " << xy[1] << ", " << xy[0] << ", " << xy[2] <<std::endl;
    
    planeToMM.multiplyPoint3(xy);
    std::cout << "   Result Spatial (x, y, z): " << xy[1] << "mm, " << xy[0] << ", " << xy[2] << "mm" << std::endl;
}

/**
 * Add information about plane coordinates to the data file information
 */
void
MediaFile::addPlaneCoordsToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    if (m_pixelIndexToPlaneMatrixValidFlag) {
        dataFileInformation.addNameAndValue("Plane Top Left",     m_planeXyzTopLeft);
        dataFileInformation.addNameAndValue("Plane Bottom Left",  m_planeXyzBottomLeft);
        dataFileInformation.addNameAndValue("Plane Bottom Right", m_planeXyzBottomRight);
        dataFileInformation.addNameAndValue("Plane Top Right",    m_planeXyzTopRight);
        
        Vector3D mmTopLeft;
        Vector3D mmTopRight;
        Vector3D mmBottomLeft;
        Vector3D mmBottomRight;
        if (planeXyzToStereotaxicXyz(m_planeXyzTopLeft, mmTopLeft)
            && planeXyzToStereotaxicXyz(m_planeXyzBottomLeft, mmBottomLeft)
            && planeXyzToStereotaxicXyz(m_planeXyzBottomRight, mmBottomRight)
            && planeXyzToStereotaxicXyz(m_planeXyzTopRight, mmTopRight)) {
            dataFileInformation.addNameAndValue("MM Top Left",     mmTopLeft);
            dataFileInformation.addNameAndValue("MM Bottom Left",  mmBottomLeft);
            dataFileInformation.addNameAndValue("MM Bottom Right", mmBottomRight);
            dataFileInformation.addNameAndValue("MM Top Right",    mmTopRight);
        }
    }
    else {
        dataFileInformation.addNameAndValue("Pixe to Plane Matrix", "Invalid");
    }
}

/**
 * @return Return a rectangle that defines the bounds of the media data
 */
QRectF
MediaFile::getLogicalBoundsRect() const
{
    QRectF rect(0, 0, getWidth(), getHeight());
    return rect;
}



/**
 * @return True if this media supports coordinates
 */
bool
MediaFile::isPlaneXyzSupported() const
{
    return m_pixelIndexToPlaneMatrixValidFlag;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzBottomLeft() const
{
    return m_planeXyzBottomLeft;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzBottomRight() const
{
    return m_planeXyzBottomRight;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzTopRight() const
{
    return m_planeXyzTopRight;
}

/**
 * @return Coordinate at bottom left of media
 */
Vector3D
MediaFile::getPlaneXyzTopLeft() const
{
    return m_planeXyzTopLeft;
}

/**
 * @return Bounding box of coordinates
 */
BoundingBox
MediaFile::getPlaneXyzBoundingBox() const

{
    return m_planeBoundingBox;
}

/**
 * Get the identification text for the pixel at the given pixel index with origin at bottom left.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param frameIndex
 *    Indics of the frames
 * @param pixelLogicalIndex
 *    Logical pixel index
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
MediaFile::getPixelPlaneIdentificationTextForFrames(const int32_t tabIndex,
                                                         const std::vector<int32_t>& frameIndices,
                                                         const Vector3D& planeCoordinate,
                                                         std::vector<AString>& columnOneTextOut,
                                                         std::vector<AString>& columnTwoTextOut,
                                                         std::vector<AString>& toolTipTextOut) const
{
    columnOneTextOut.clear();
    columnTwoTextOut.clear();
    toolTipTextOut.clear();
    
    PixelLogicalIndex pixelLogicalIndex;
    if ( ! planeXyzToLogicalPixelIndex(planeCoordinate,
                                       pixelLogicalIndex)) {
        return;
    }
    std::vector<int32_t> validFrameIndices;
    for (int32_t frameIndex : frameIndices) {
        if (isPixelIndexInFrameValid(frameIndex,
                                     pixelLogicalIndex)) {
            validFrameIndices.push_back(frameIndex);
        }
    }
    if (validFrameIndices.empty()) {
        return;
    }
    
    
    std::vector<AString> leftRgbaText;
    std::vector<AString> rightRgbaText;
    for (int32_t frameIndex : validFrameIndices) {
        uint8_t rgba[4];
        const bool rgbaValidFlag = getPixelRGBA(tabIndex,
                                                frameIndex,
                                                pixelLogicalIndex,
                                                rgba);
        if (rgbaValidFlag) {
            leftRgbaText.push_back("Scene "
                                   + AString::number(frameIndex + 1));
            rightRgbaText.push_back("RGBA ("
                                    + (rgbaValidFlag
                                       ? AString::fromNumbers(rgba, 4, ",")
                                       : "Invalid")
                                    + ")");
        }
    }
    CaretAssert(leftRgbaText.size() == rightRgbaText.size());
    const int32_t numRgbaText(static_cast<int32_t>(leftRgbaText.size()));
    
    const PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    const int64_t fullResPixelI(pixelIndex.getI());
    const int64_t fullResPixelJ(pixelIndex.getJ());
    const AString pixelText("Pixel IJ ("
                            + AString::number(fullResPixelI)
                            + ","
                            + AString::number(fullResPixelJ)
                            + ")");
    
    const AString logicalText("Logical IJ ("
                              + AString::number(pixelLogicalIndex.getI(), 'f', 3)
                              + ","
                              + AString::number(pixelLogicalIndex.getJ(), 'f', 3)
                              + ")");
    
    const AString planeText("Plane XYZ ("
                            + AString::fromNumbers(planeCoordinate)
                            + ")");
    
    Vector3D stereoXYZ;
    AString mmText;
    if (planeXyzToStereotaxicXyz(planeCoordinate, stereoXYZ)) {
        mmText = ("Stereotaxicz XYZ ("
                  + AString::fromNumbers(stereoXYZ)
                  + "mm");
    }

    columnOneTextOut.push_back("Filename");
    columnTwoTextOut.push_back(getFileNameNoPath());
    
    columnOneTextOut.push_back(pixelText);
    columnTwoTextOut.push_back(logicalText);
    
    columnOneTextOut.push_back(mmText);
    columnTwoTextOut.push_back(planeText);
    
    Vector3D xyz;
    if (pixelIndexToStereotaxicXYZ(pixelLogicalIndex, false, xyz)) {
        columnOneTextOut.push_back("Stereotaxic XYZ");
        columnTwoTextOut.push_back(AString::fromNumbers(xyz, 3, ", "));
    }
    
    if (pixelIndexToStereotaxicXYZ(pixelLogicalIndex, true, xyz)) {
        columnOneTextOut.push_back("Stereotaxic XYZ with NIFTI warping");
        columnTwoTextOut.push_back(AString::fromNumbers(xyz, 3, ", "));
    }
    
    for (int32_t i = 0; i < numRgbaText; i++) {
        CaretAssertVectorIndex(leftRgbaText, i);
        CaretAssertVectorIndex(rightRgbaText, i);
        toolTipTextOut.push_back(leftRgbaText[i]
                                 + ": "
                                 + rightRgbaText[i]);
        columnOneTextOut.push_back(leftRgbaText[i]);
        columnTwoTextOut.push_back(rightRgbaText[i]);
    }
    toolTipTextOut.push_back(pixelText);
    toolTipTextOut.push_back(logicalText);
    toolTipTextOut.push_back(mmText);
    
    CaretAssert(columnOneTextOut.size() == columnTwoTextOut.size());
}

/**
 * Converrt a plane rectangle to a logical rectangle
 * @param planeRect
 *    The plane rectangle
 * @return
 *    The logical rectangle
 */
QRectF
MediaFile::planeRectToLogicalRect(const QRectF& planeRect) const
{
    QRectF rect;
    
    const Vector3D topLeftCoord(planeRect.left(),
                                planeRect.top(),
                                0.0);
    const Vector3D bottomRightCoord(planeRect.right(),
                                    planeRect.bottom(),
                                    0.0);
    
    PixelLogicalIndex topLeftPixel;
    PixelLogicalIndex bottomRightPixel;
    if (planeXyzToLogicalPixelIndex(topLeftCoord, topLeftPixel)
        && planeXyzToLogicalPixelIndex(bottomRightCoord, bottomRightPixel)) {
        rect.setCoords(topLeftPixel.getI(), topLeftPixel.getJ(),
                       bottomRightPixel.getI(), bottomRightPixel.getJ());
    }
    return rect;
}

/**
 * Convert a logical rect to a plane rect
 * @param logicalRect
 *    The logical rectangle
 * @return
 *    The plane rectangle
 */
QRectF
MediaFile::logicalRectToPlaneRect(const QRectF& logicalRect) const
{
    QRectF rect;
    
    const PixelLogicalIndex topLeftPixel(logicalRect.left(),
                                         logicalRect.top(),
                                         0.0f);
    const PixelLogicalIndex bottomRightPixel(logicalRect.right(),
                                             logicalRect.bottom(),
                                             0.0f);
    Vector3D topLeftCoord;
    Vector3D bottomRightCoord;
    if (logicalPixelIndexToPlaneXYZ(topLeftPixel, topLeftCoord)
        && logicalPixelIndexToPlaneXYZ(bottomRightPixel, bottomRightCoord)) {
        rect.setCoords(topLeftCoord[0], topLeftCoord[1],
                       bottomRightCoord[0], bottomRightCoord[1]);
    }
    return rect;
}
