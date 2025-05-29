
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

#define __HISTOLOGY_SLICE_DECLARE__
#include "HistologySlice.h"
#undef __HISTOLOGY_SLICE_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "HistologySliceImage.h"
#include "CziDistanceFile.h"
#include "CziNonLinearTransform.h"
#include "DataFileContentInformation.h"
#include "GraphicsRegionSelectionBox.h"
#include "HistologyCoordinate.h"
#include "HistologySlicesFile.h"
#include "MathFunctions.h"
#include "MediaFile.h"
#include "Plane.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::HistologySlice 
 * \brief Contains images for a slice
 * \ingroup Files
 */

/**
 * Constructor.
 * @param sliceIndex
 *    Index of the slice
 * @param sliceName
 *    Number of the slice
 * @param MRIToHistWarpFileName,
 *    Name of MRI to Hist File
 * @param histToMRIWarpFileName,
 *    Name of Hist to MRI File
 * @param planeToMillimetersMatrix,
 *    The plane to millimeters matrix
 * @param planeToMillimetersMatrixValidFlag
 *    Validity of plane to millimeters matrix
 */
HistologySlice::HistologySlice(const int32_t sliceIndex,
                               const AString& sliceName,
                               const AString& MRIToHistWarpFileName,
                               const AString& histToMRIWarpFileName,
                               const Matrix4x4& planeToMillimetersMatrix,
                               const bool planeToMillimetersMatrixValidFlag)
: CaretObject(),
m_sliceIndex(sliceIndex),
m_sliceName(sliceName),
m_MRIToHistWarpFileName(MRIToHistWarpFileName),
m_histToMRIWarpFileName(histToMRIWarpFileName),
m_planeToMillimetersMatrix(planeToMillimetersMatrix),
m_planeToMillimetersMatrixValidFlag(planeToMillimetersMatrixValidFlag)
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_fromStereotaxicNonLinearTransform.reset(new CziNonLinearTransform(CziNonLinearTransform::Mode::FROM_MILLIMETERS,
                                                                        m_histToMRIWarpFileName));
    m_toStereotaxicNonLinearTransform.reset(new CziNonLinearTransform(CziNonLinearTransform::Mode::TO_MILLIMETERS,
                                                                      m_MRIToHistWarpFileName));
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
HistologySlice::~HistologySlice()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
HistologySlice::HistologySlice(const HistologySlice& obj)
: CaretObject(obj),
EventListenerInterface(),
SceneableInterface(obj)
{
    this->copyHelperHistologySlice(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
HistologySlice&
HistologySlice::operator=(const HistologySlice& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperHistologySlice(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
HistologySlice::copyHelperHistologySlice(const HistologySlice& /*obj*/)
{
    CaretAssertMessage(0, "Copying not supported");
    m_stereotaxicXyzBoundingBoxValidFlag = false;
    m_planeXyzBoundingBoxValidFlag       = false;
}

/**
 * @return Index of the slice
 */
int32_t
HistologySlice::getSliceIndex() const
{
    return m_sliceIndex;
}

/**
 * @return Number of the slice
 */
AString
HistologySlice::getSliceName() const
{
    return m_sliceName;
}

/**
 * Add an histology slice image
 * @param histologySliceImage
 *    Image to add.  This instance will take ownership of the image.
 */
void
HistologySlice::addHistologySliceImage(HistologySliceImage* histologySliceImage)
{
    CaretAssert(histologySliceImage);
    histologySliceImage->setPlaneToMillimetersMatrix(m_planeToMillimetersMatrix,
                                                     m_planeToMillimetersMatrixValidFlag,
                                                     m_toStereotaxicNonLinearTransform,
                                                     m_fromStereotaxicNonLinearTransform);
    std::unique_ptr<HistologySliceImage> ptr(histologySliceImage);
    m_histologySliceImages.push_back(std::move(ptr));
    
    m_stereotaxicXyzBoundingBoxValidFlag = false;
    m_planeXyzBoundingBoxValidFlag       = false;
}

/**
 * @return Number of histology slice images
 */
int32_t
HistologySlice::getNumberOfHistologySliceImages() const
{
    return m_histologySliceImages.size();
}

/**
 * @return Histology slice image at the given index
 * @param index
 *    Index of the image
 */
HistologySliceImage*
HistologySlice::getHistologySliceImage(const int32_t index)
{
    CaretAssertVectorIndex(m_histologySliceImages, index);
    return m_histologySliceImages[index].get();
}

/**
 * @return Histology slice image at the given index, const method
 * @param index
 *    Index of the image
 */
const HistologySliceImage*
HistologySlice::getHistologySliceImage(const int32_t index) const
{
    CaretAssertVectorIndex(m_histologySliceImages, index);
    return m_histologySliceImages[index].get();
}

/**
 * @return stereotaxic BoundingBox for the slice (bounding box of all images in slice)
 */
BoundingBox
HistologySlice::getStereotaxicXyzBoundingBox() const
{
    if ( ! m_stereotaxicXyzBoundingBoxValidFlag) {
        m_stereotaxicXyzBoundingBox.resetForUpdate();
        
        for (auto& slice : m_histologySliceImages) {
            const MediaFile* mf(slice->getMediaFile());
            if (mf != NULL) {
                BoundingBox bb(mf->getStereotaxicXyzBoundingBox());
                m_stereotaxicXyzBoundingBox.unionOperation(bb);
            }
        }
        m_stereotaxicXyzBoundingBoxValidFlag = true;
    }
    
    return m_stereotaxicXyzBoundingBox;
}

/**
 * @return Plane BoundingBox for the slice (bounding box of all images in slice)
 */
BoundingBox
HistologySlice::getPlaneXyzBoundingBox() const
{
    if ( ! m_planeXyzBoundingBoxValidFlag) {
        m_planeXyzBoundingBox.resetForUpdate();
        
        for (auto& slice : m_histologySliceImages) {
            const MediaFile* mf(slice->getMediaFile());
            if (mf != NULL) {
                BoundingBox bb(mf->getPlaneXyzBoundingBox());
                m_planeXyzBoundingBox.unionOperation(bb);
            }
        }
        m_planeXyzBoundingBoxValidFlag = true;
    }
    
    return m_planeXyzBoundingBox;
}

/**
 * @return Factor for converting a millimeters size to a plane size
 */
float
HistologySlice::getMillimetersToPlaneFactor() const
{
    if (m_MillimetersToPlaneFactor < 0.0) {
        const BoundingBox planeBoundingBox(getPlaneXyzBoundingBox());
        const BoundingBox stereotaxicBoundingBox(getStereotaxicXyzBoundingBox());
        const float planeLength(std::sqrt((planeBoundingBox.getDifferenceX() * planeBoundingBox.getDifferenceX())
                                          + (planeBoundingBox.getDifferenceY() * planeBoundingBox.getDifferenceY())
                                          + (planeBoundingBox.getDifferenceZ() * planeBoundingBox.getDifferenceZ())));
        const float stereotaxicLength(std::sqrt((stereotaxicBoundingBox.getDifferenceX() * stereotaxicBoundingBox.getDifferenceX())
                                                + (stereotaxicBoundingBox.getDifferenceY() * stereotaxicBoundingBox.getDifferenceY())
                                                + (stereotaxicBoundingBox.getDifferenceZ() * stereotaxicBoundingBox.getDifferenceZ())));
        
        m_MillimetersToPlaneFactor = 1.0;
        
        if (stereotaxicLength > 0.0) {
            m_MillimetersToPlaneFactor = (planeLength
                                          / stereotaxicLength);
        }
    }
    
    return m_MillimetersToPlaneFactor;
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
HistologySlice::planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                         Vector3D& stereotaxicXyzOut) const
{
    Vector3D unusedXyz;
    return planeXyzToStereotaxicXyz(planeXyz,
                                    unusedXyz,
                                    stereotaxicXyzOut);
}

/**
 * Convert a plane XYZ to stereotaxic XYZ
 * @param planeXyz
 *     XYZ in plane
 * @param stereotaxicNoNonLinearXyzOut
 *    Output with stereotaxic XYZ but non-linear offset
 * @param stereotaxicWithNonLinearXyzOut
 *    Output with stereotaxic XYZ but with non-linear offset
 * @return True if successful, else false.
 */
bool
HistologySlice::planeXyzToStereotaxicXyz(const Vector3D& planeXyz,
                                         Vector3D& stereotaxicNoNonLinearXyzOut,
                                         Vector3D& stereotaxicWithNonLinearXyzOut) const
{
    const HistologySliceImage* firstSliceImage(getHistologySliceImage(0));
    if (firstSliceImage != NULL) {
        const MediaFile* mediaFile(firstSliceImage->getMediaFile());
        if (mediaFile->planeXyzToStereotaxicXyz(planeXyz,
                                                stereotaxicNoNonLinearXyzOut,
                                                stereotaxicWithNonLinearXyzOut)) {
            return true;
        }
    }
    stereotaxicNoNonLinearXyzOut.fill(0);
    stereotaxicWithNonLinearXyzOut.fill(0);
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
HistologySlice::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                         Vector3D& planeXyzOut) const
{
    Vector3D unsusedXyz;
    return stereotaxicXyzToPlaneXyz(stereotaxicXyz,
                                    unsusedXyz,
                                    planeXyzOut);
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
HistologySlice::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                         Vector3D& planeNoNonLinearXyzOut,
                                         Vector3D& planeWithNonLinearXyzOut) const
{
    /*
     * Stereotaxic <-> Plane is same for all images
     */
    const HistologySliceImage* firstSliceImage(getHistologySliceImage(0));
    if (firstSliceImage != NULL) {
        const MediaFile* mediaFile(firstSliceImage->getMediaFile());
        if (mediaFile->stereotaxicXyzToPlaneXyz(stereotaxicXyz,
                                                planeNoNonLinearXyzOut,
                                                planeWithNonLinearXyzOut)) {
            return true;
        }
    }
    planeNoNonLinearXyzOut.fill(0);
    planeWithNonLinearXyzOut.fill(0);
    return false;
}

/**
 * Convert a region from plane coordinates to stereotaxic coordinates
 */
std::unique_ptr<GraphicsRegionSelectionBox>
HistologySlice::planeRegionToStereotaxicRegion(const GraphicsRegionSelectionBox* planeRegion) const
{
    CaretAssert(planeRegion);
    
    std::unique_ptr<GraphicsRegionSelectionBox> regionOut;
    
    if (planeRegion != NULL) {
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0), minZ(0.0), maxZ(0.0);
        if (planeRegion->getBounds(minX, minY, minZ, maxX, maxY, maxZ)) {
            Vector3D planeTopLeftXYZ(minX, minY, 0.0);
            Vector3D planeBottomLeftXYZ(minX, maxY, 0.0);
            Vector3D planeTopRightXYZ(maxX, minY, 0.0);
            Vector3D planeBottomRightXYZ(maxX, maxY, 0.0);
            Vector3D blXYZ;
            Vector3D tlXYZ;
            Vector3D trXYZ;
            Vector3D brXYZ;
            if (planeXyzToStereotaxicXyz(planeBottomLeftXYZ, blXYZ)
                && planeXyzToStereotaxicXyz(planeTopLeftXYZ, tlXYZ)
                && planeXyzToStereotaxicXyz(planeTopRightXYZ, trXYZ)
                && planeXyzToStereotaxicXyz(planeBottomRightXYZ, brXYZ)) {
                /*
                 * Note: For histology viewport, origin is top left
                 */
                float vpMinX, vpMinY, vpMaxX, vpMaxY;
                planeRegion->getViewportBounds(vpMinX, vpMinY, vpMaxX, vpMaxY);
                regionOut.reset(new GraphicsRegionSelectionBox);
                regionOut->initialize(blXYZ[0], blXYZ[1], blXYZ[2], vpMinX, vpMaxY);
                regionOut->update(brXYZ[0], brXYZ[1], brXYZ[2], vpMaxX, vpMaxY);
                regionOut->update(tlXYZ[0], tlXYZ[1], tlXYZ[2], vpMinX, vpMinY);
                regionOut->update(trXYZ[0], trXYZ[1], trXYZ[2], vpMaxX, vpMinY);
            }
        }
    }
    
    return regionOut;
}

/**
 * @return Plane for stereotaxic coordinates
 */
const Plane&
HistologySlice::getStereotaxicPlane() const
{
    if ( ! m_stereotaxicPlaneValidFlag) {
        if (getNumberOfHistologySliceImages() > 0) {
            /*
             * Use plane from first image as all images in slice should
             * be close to being in the same plane
             */
            const HistologySliceImage* histologyImage(getHistologySliceImage(0));
            CaretAssert(histologyImage);
            const MediaFile* mediaFile(histologyImage->getMediaFile());
            if (mediaFile != NULL) {
                const Plane* plane(mediaFile->getStereotaxicImagePlane());
                if (plane->isValidPlane()) {
                    m_stereotaxicPlane = *plane;
                    m_stereotaxicPlaneValidFlag = true;
                }
            }
        }
    }
    
    return m_stereotaxicPlane;
}

/**
 * Project the givent stereotaxic coordinate onto the slice
 * @param stereotaxicXYZ
 *    Input stereotaxic coordinate
 * @param stereotaxicOnSliceXYZ
 *    Output stereotaxic coordinate projected to the slice
 * @return
 *    True if successful, else false
 */
bool
HistologySlice::projectStereotaxicXyzToSlice(const Vector3D stereotaxicXYZ,
                                            Vector3D& stereotaxicOnSliceXYZ) const
{
    const Plane plane(getStereotaxicPlane());
    if (plane.isValidPlane()) {
        plane.projectPointToPlane(stereotaxicXYZ,
                                  stereotaxicOnSliceXYZ);
        return true;
    }
    return false;
}

/**
 * Project the givent stereotaxic coordinate onto the slice
 * @param stereotaxicXYZ
 *    Input stereotaxic coordinate
 * @param stereotaxicOnSliceXYZ
 *    Output stereotaxic coordinate projected to the slice
 * @param stereotaxicDistanceToSliceOut
 *    Distance to slice
 * @param planeOnSliceXYZ
 *    Plane coordinate of point on slice plane
 * @return
 *    True if successful, else false
 */
bool
HistologySlice::projectStereotaxicXyzToSlice(const Vector3D& stereotaxicXYZ,
                                             Vector3D& stereotaxicOnSliceXYZ,
                                             float& stereotaxicDistanceToSliceOut,
                                             Vector3D& planeOnSliceXYZ) const
{
    if (projectStereotaxicXyzToSlice(stereotaxicXYZ,
                                     stereotaxicOnSliceXYZ)) {
        if (stereotaxicXyzToPlaneXyz(stereotaxicOnSliceXYZ,
                                     planeOnSliceXYZ)) {
            stereotaxicDistanceToSliceOut = (stereotaxicXYZ - stereotaxicOnSliceXYZ).length();
            return true;
        }
    }

    return false;
}

/**
 * @return Plane for plane  coordinates
 */
const Plane&
HistologySlice::getPlaneXyzPlane() const
{
    if ( ! m_planeXyzPlaneValidFlag) {
        if (getNumberOfHistologySliceImages() > 0) {
            /*
             * Use plane from first image as all images in slice should
             * be close to being in the same plane
             */
            const HistologySliceImage* histologyImage(getHistologySliceImage(0));
            CaretAssert(histologyImage);
            const MediaFile* mediaFile(histologyImage->getMediaFile());
            if (mediaFile != NULL) {
                const Plane* plane(mediaFile->getPlaneCoordinatesPlane());
                if (plane->isValidPlane()) {
                    m_planeXyzPlane = *plane;
                    m_planeXyzPlaneValidFlag = true;
                }
            }
        }
    }
    
    return m_planeXyzPlane;
}

/**
 * Find the media file in the histology slice images
 * @param mediaFileName
 *    Name of media file
 * @return
 *    Media file with the given name or NULL if not found
 */
MediaFile*
HistologySlice::findMediaFileWithName(const AString& mediaFileName) const
{
    for (auto& hsi : m_histologySliceImages) {
        MediaFile* mf(hsi->getMediaFile());
        if (mf != NULL) {
            if (mf->getFileName() == mediaFileName) {
                return mf;
            }
        }
    }
    
    return NULL;
}

/**
 * @return Index of media file in the images for this slice
 * @param mediaFileName
 *    Name of the media file
 */
int32_t
HistologySlice::getIndexOfMediaFileWithName(const AString& mediaFileName) const
{
    const int numImages(m_histologySliceImages.size());
    for (int32_t i = 0; i < numImages; i++) {
        CaretAssertVectorIndex(m_histologySliceImages, i);
        CaretAssert(m_histologySliceImages[i]);
        const MediaFile* mf(m_histologySliceImages[i]->getMediaFile());
        if (mf != NULL) {
            if (mf->getFileName() == mediaFileName) {
                return i;
            }
        }
    }
    return -1;
}

/**
 * Get the identification text for the given histology coordinate.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param histologyCoordinate
 *    The histology coordinate
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
HistologySlice::getIdentificationText(const int32_t tabIndex,
                                      const HistologyCoordinate& histologyCoordinate,
                                      std::vector<AString>& columnOneTextOut,
                                      std::vector<AString>& columnTwoTextOut,
                                      std::vector<AString>& toolTipTextOut) const
{
    std::vector<AString> columnOneText, columnTwoText, toolTipText;

    std::vector<int32_t> frameIndicesVector { 0 };
    const MediaFile* mediaFile(findMediaFileWithName(histologyCoordinate.getHistologyMediaFileName()));
    if (mediaFile != NULL) {
        mediaFile->getPixelPlaneIdentificationTextForHistology(tabIndex,
                                                               frameIndicesVector,
                                                               histologyCoordinate.getPlaneXYZ(),
                                                               columnOneText,
                                                               columnTwoText,
                                                               toolTipText);
    }
    else {
        CaretLogWarning("Unable to find media file with name "
                        + histologyCoordinate.getHistologyMediaFileName()
                        + " for generating identification text");
    }

    const int32_t numColOne(columnOneText.size());
    const int32_t numColTwo(columnTwoText.size());
    const int32_t maxNum(std::max(numColOne, numColTwo));
    for (int32_t i = 0; i < maxNum; i++) {
        AString colOne;
        AString colTwo;
        if (i < numColOne) {
            CaretAssertVectorIndex(columnOneText, i);
            colOne = columnOneText[i];
        }
        if (i < numColTwo) {
            CaretAssertVectorIndex(columnTwoText, i);
            colTwo = columnTwoText[i];
        }
        columnOneTextOut.push_back(colOne);
        columnTwoTextOut.push_back(colTwo);
    }
    
    toolTipTextOut = toolTipText;
    
    if (s_debugFlag) {
        idDevelopment(histologyCoordinate);
    }
}

void
HistologySlice::idDevelopment(const HistologyCoordinate& histologyCoordinate) const
{
    bool firstFlag(true);
    for (const auto& hsi : m_histologySliceImages) {
        AString distanceInfo;
        hsi->getDistanceInfo(histologyCoordinate,
                             distanceInfo);
        if ( ! distanceInfo.isEmpty()) {
            if (firstFlag) {
                firstFlag = false;
                std::cout << std::endl;
            }
            std::cout << distanceInfo << std::endl << std::flush;
        }
    }
}

/**
 * Create the texture images for making overlap
 * @param errorMessageOut
 *    Contains error information if failure to create masking textures
 * @return
 *    True if successful, else false.
 */
bool
HistologySlice::createOverlapMaskingTextures(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (m_maskingTexturesCreatedFlag) {
        return true;
    }
    m_maskingTexturesCreatedFlag = true;
    
    std::vector<const CziDistanceFile*> distanceFiles;
    
    const int32_t numSliceImages(m_histologySliceImages.size());
    for (int32_t iImage = 0; iImage < numSliceImages; iImage++) {
        CaretAssertVectorIndex(m_histologySliceImages, iImage);
        CaretAssert(m_histologySliceImages[iImage]);
        const CziDistanceFile* df(m_histologySliceImages[iImage]->getDistanceFile());
        if (df != NULL) {
            distanceFiles.push_back(df);
        }
    }
    
    if (distanceFiles.size() != m_histologySliceImages.size()) {
        errorMessageOut = "Not all images in slice have distance files";
        return false;
    }

    std::vector<GraphicsPrimitiveV3fT2f*> primitives;
    const bool resultFlag(CziDistanceFile::createMaskingPrimitives(distanceFiles,
                                                                   primitives,
                                                                   errorMessageOut));
    if (resultFlag) {
        if (primitives.size() == m_histologySliceImages.size()) {
            for (int32_t iImage = 0; iImage < numSliceImages; iImage++) {
                CaretAssertVectorIndex(m_histologySliceImages, iImage);
                CaretAssert(m_histologySliceImages[iImage]);
                CaretAssertVectorIndex(primitives, iImage);
                m_histologySliceImages[iImage]->setStencilMaskingImagePrimitive(primitives[iImage]);
            }
        }
    }
    return resultFlag;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologySlice::toString() const
{
    AString s("Histology slice "
              + getSliceName());
    s.appendWithNewLine("   MRI to Hist File: " + m_MRIToHistWarpFileName);
    s.appendWithNewLine("   Hist to MRI File: " + m_histToMRIWarpFileName);
    const int32_t numImages(getNumberOfHistologySliceImages());
    for (int32_t i = 0; i < numImages; i++) {
        s.appendWithNewLine(getHistologySliceImage(i)->toString());
    }
    return s;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
HistologySlice::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * @return Names (absolute path) of all child data files of this file.
 * This includes the CZI Image Files, Distance File, and the Non-Linear
 * Transform Files.
 */
std::vector<AString>
HistologySlice::getChildDataFilePathNames() const
{
    std::vector<AString> childDataFilePathNames;
    childDataFilePathNames.push_back(m_MRIToHistWarpFileName);
    childDataFilePathNames.push_back(m_histToMRIWarpFileName);
    
    for (const auto& image : m_histologySliceImages) {
        std::vector<AString> names(image->getChildDataFilePathNames());
        childDataFilePathNames.insert(childDataFilePathNames.end(),
                                      names.begin(), names.end());
    }

    return childDataFilePathNames;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
HistologySlice::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologySlice",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
HistologySlice::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * Add information about the slice to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
HistologySlice::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    /*
     * Note: Slice indices range 0 to N-1 but we display as 1 to N
     */
    dataFileInformation.addNameAndValue("Slice Index", getSliceIndex() + 1);
    dataFileInformation.addNameAndValue("Slice Number", getSliceName());
    
    const AString mprAngleTitle("MPR Rotation Angles");
    Vector3D mprAngles;
    if (getSliceRotationAngles(mprAngles)) {
        dataFileInformation.addNameAndValue(mprAngleTitle, mprAngles.toString());
    }
    else {
        dataFileInformation.addNameAndValue(mprAngleTitle, "Invalid");
    }
    const int32_t numImages(getNumberOfHistologySliceImages());
    for (int32_t jImage = 0; jImage < numImages; jImage++) {
        const HistologySliceImage* image(getHistologySliceImage(jImage));
        const MediaFile* mediaFile(image->getMediaFile());
        CaretAssert(mediaFile);
        dataFileInformation.addNameAndValue("Image",
                                            (mediaFile->getFileNameNoPath()
                                             + " ("
                                             + mediaFile->getFilePath()
                                             + ")"));
        dataFileInformation.addNameAndValue("Stereotaxic Plane Equation",
                                            mediaFile->getStereotaxicImagePlane()->toString());
        const_cast<MediaFile*>(mediaFile)->addPlaneCoordsToDataFileContentInformation(dataFileInformation);
    }
    dataFileInformation.addNameAndValue(" ", " ");
    dataFileInformation.addNameAndValue(" ", " ");
    dataFileInformation.addNameAndValue(" ", " ");
    dataFileInformation.addNameAndValue("=====", "File Info");
    dataFileInformation.addNameAndValue(" ", " ");
    for (int32_t jImage = 0; jImage < numImages; jImage++) {
        const HistologySliceImage* image(getHistologySliceImage(jImage));
        const MediaFile* mediaFile(image->getMediaFile());
        CaretAssert(mediaFile);
        const_cast<MediaFile*>(mediaFile)->addToDataFileContentInformation(dataFileInformation);
    }
}

/**
 * Get the rotations angles of the slice.  These angles are derived from
 * the 'Plane to Millimeters' matrix and are used to orient the corresponding
 * volume slice in MPR viewing mode.
 * @param rotationsOut
 *    X, Y, Z rotations angles output
 * @return
 *    True if the rotation angles are valid, else false.
 */
bool
HistologySlice::getSliceRotationAngles(Vector3D& rotationsOut) const
{
    if ( ! m_planeToMillimetersMatrixValidFlag) {
        rotationsOut.fill(0.0);
        return false;
    }
    
    if ( ! m_mprVolumeRotationAnglesComputedFlag) {
        m_mprVolumeRotationAnglesComputedFlag = true;
        
        const Vector3D v1(m_planeToMillimetersMatrix.getBasisVector(0));
        const Vector3D v2(m_planeToMillimetersMatrix.getBasisVector(1));
        Vector3D crossvec(v1.cross(v2));
        if (crossvec[1] < 0.0) {
            CaretLogFine("Radiological orientation when computing Histology MPR Volume Rotation for "
                         + getSliceName());
            crossvec = -crossvec;
        }
        
        const Vector3D normalVector(crossvec.normal());
        const float rotationX(MathFunctions::toDegrees(std::asin(normalVector[2])));
        const float rotationZ(-MathFunctions::toDegrees(std::atan2(normalVector[0],
                                                                   normalVector[1])));
        m_mprVolumeRotationAngles[0] = rotationX;
        m_mprVolumeRotationAngles[1] = 0.0;
        m_mprVolumeRotationAngles[2] = rotationZ;
        m_mprVolumeRotationAnglesValidFlag = true;
    }
    
    rotationsOut = m_mprVolumeRotationAngles;
    return m_mprVolumeRotationAnglesValidFlag;
}

/**
 * Get the orientation labels for the slice..  The rotation of the slice's axis are compared
 * to the standard orientations to determine the text (or no text) that is displayed.
 * @param leftScreenLabelTextOut
 *    Label for left side
 * @param rightScreenLabelTextOut
 *    Label for right side
 * @param bottomScreenLabelTextOut
 *    Label for bottom side
 * @param topScreenLabelTextOut
 *    Label for top side
 */
void
HistologySlice::getAxisLabels(const bool flipLeftRightFlag,
                              AString& leftScreenLabelTextOut,
                              AString& rightScreenLabelTextOut,
                              AString& bottomScreenLabelTextOut,
                              AString& topScreenLabelTextOut) const
{
    leftScreenLabelTextOut   = "";
    rightScreenLabelTextOut  = "";
    bottomScreenLabelTextOut = "";
    topScreenLabelTextOut    = "";
    
    const int32_t numImages(getNumberOfHistologySliceImages());
    if (numImages < 1) {
        return;
    }
    const HistologySliceImage* hsi(getHistologySliceImage(0));
    CaretAssert(hsi);
    const MediaFile* mediaFile(hsi->getMediaFile());
    CaretAssert(mediaFile);
    
    /*
     * Get the basis vector.
     * May need to flip the first vector if slice is left/right flipped
     * Flip second vector since plane Y origin is at top
     */
    std::array<Vector3D, 2> allBasisVectors {
        (m_planeToMillimetersMatrix.getBasisVectorNormalized(0)
         * (flipLeftRightFlag ? -1.0 : 1.0)),
        (m_planeToMillimetersMatrix.getBasisVectorNormalized(1) * -1.0)
    };
    
    class VectorAndLabel {
    public:
        VectorAndLabel(const AString& label,
                       const AString& oppositeLabel,
                       const float x,
                       const float y,
                       const float z)
        : m_label(label),
        m_oppositeLabel(oppositeLabel),
        m_vector(x, y, z) { }
        
        AString m_label;
        AString m_oppositeLabel;
        Vector3D m_vector;
    };
    
    std::vector<VectorAndLabel> allVectorsAndLabels;
    allVectorsAndLabels.emplace_back("L", "R",  1.0,  0.0,  0.0);
    allVectorsAndLabels.emplace_back("R", "L", -1.0,  0.0,  0.0);
    allVectorsAndLabels.emplace_back("A", "P",  0.0, -1.0,  0.0);
    allVectorsAndLabels.emplace_back("P", "A",  0.0,  1.0,  0.0);
    allVectorsAndLabels.emplace_back("I", "S",  0.0,  0.0,  1.0);
    allVectorsAndLabels.emplace_back("S", "I",  0.0,  0.0, -1.0);
    
    
    for (int32_t iBasis = 0; iBasis < static_cast<int32_t>(allBasisVectors.size()); iBasis++) {
        CaretAssertVectorIndex(allBasisVectors, iBasis);
        const Vector3D& basisVector(allBasisVectors[iBasis]);
        for (const VectorAndLabel& vectorAndLabel : allVectorsAndLabels) {
            const float dotValue(vectorAndLabel.m_vector.dot(basisVector));
            AString labelTemp;
            if (dotValue >= 0.866) {  /* cosine(30 degrees) = 0.866 */
                switch (iBasis) {
                    case 0:
                        leftScreenLabelTextOut  = vectorAndLabel.m_label;
                        rightScreenLabelTextOut = vectorAndLabel.m_oppositeLabel;
                        break;
                    case 1:
                        bottomScreenLabelTextOut = vectorAndLabel.m_label;
                        topScreenLabelTextOut    = vectorAndLabel.m_oppositeLabel;
                        break;
                }
            }
        }
    }
}
