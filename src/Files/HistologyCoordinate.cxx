
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

#define __HISTOLOGY_COORDINATE_DECLARE__
#include "HistologyCoordinate.h"
#undef __HISTOLOGY_COORDINATE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"
#include "HistologySlice.h"
#include "HistologySliceImage.h"
#include "HistologySlicesFile.h"
#include "MediaFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::HistologyCoordinate 
 * \brief Information about histology coordinate
 * \ingroup Files
 */

/**
 * Constructor.
 */
HistologyCoordinate::HistologyCoordinate()
: CaretObject()
{
    initializeMembers();
}


/**
 * Destructor.
 */
HistologyCoordinate::~HistologyCoordinate()
{
}

/**
 * New instance for identification.  slice number and stereotaxic coordinate will bet set.
 * @param histologySlicesFile
 *    The histology slices file (MUST be valid)
 * @param mediaFile
 *    The media file (may be NULL)
 * @param sliceIndex
 *    The slice index
 * @param planeXYZ
 *    The plane coordinate
 */
HistologyCoordinate
HistologyCoordinate::newInstanceIdentification(HistologySlicesFile* histologySlicesFile,
                                               MediaFile* mediaFile,
                                               const int32_t sliceIndex,
                                               const Vector3D& planeXYZ)
{
    CaretAssert(histologySlicesFile);
    CaretAssert(mediaFile);
    
    HistologyCoordinate hc;
    hc.setHistologySlicesFile(histologySlicesFile);
    hc.setMediaFile(mediaFile);
    hc.setSliceIndex(sliceIndex);
    hc.setSliceNumber(histologySlicesFile->getSliceNumberBySliceIndex(sliceIndex));
    hc.setPlaneXYZ(planeXYZ);
    hc.m_planeXY = planeXYZ;
    
//    if (mediaFile != NULL) {
//        Vector3D stereotaxicXYZ;
//        if (mediaFile->planeXyzToStereotaxicXyz(hc.m_planeXY,
//                                                stereotaxicXYZ)) {
//            hc.setStereotaxicXYZ(stereotaxicXYZ);
//        }
//    }
//    else {
        const HistologySlice* slice(histologySlicesFile->getHistologySliceByIndex(sliceIndex));
        if (slice != NULL) {
            Vector3D stereotaxicXYZ;
            if (slice->planeXyzToStereotaxicXyz(planeXYZ,
                                                stereotaxicXYZ)) {
                hc.setStereotaxicXYZ(stereotaxicXYZ);
            }
        }
//    }
    
    return hc;
}

/**
 * New instance for default slice at middle of the slices
 * @param histologySlicesFile
 *    The histology slices file (MUST be valid)
 */
HistologyCoordinate
HistologyCoordinate::newInstanceDefaultSlices(HistologySlicesFile* histologySlicesFile)
{
    if (histologySlicesFile == NULL) {
        HistologyCoordinate hc;
        return hc;
    }

    const int32_t numSlices(histologySlicesFile->getNumberOfHistologySlices());
    if (numSlices <= 0) {
        return HistologyCoordinate();
    }
    
    const int32_t sliceIndex(numSlices / 2);
    CaretAssert((sliceIndex >= 0)
                && (sliceIndex < numSlices));
    
    const HistologySlice* slice(histologySlicesFile->getHistologySliceByIndex(sliceIndex));
    if (slice != NULL) {
        BoundingBox bb(slice->getPlaneXyzBoundingBox());
        if (bb.isValid2D()) {
            Vector3D planeXYZ;
            bb.getCenter(planeXYZ);
            
            HistologyCoordinate hc;
            hc.setHistologySlicesFile(histologySlicesFile);
            hc.setSliceIndex(sliceIndex);
            hc.setSliceNumber(histologySlicesFile->getSliceNumberBySliceIndex(sliceIndex));
            hc.setPlaneXYZ(planeXYZ);
            
            Vector3D stereotaxicXYZ;
            if (slice->planeXyzToStereotaxicXyz(planeXYZ,
                                                stereotaxicXYZ)) {
                hc.setStereotaxicXYZ(stereotaxicXYZ);
            }

            return hc;
        }
    }

    HistologyCoordinate hc;
    return hc;
}

/**
 * New instance for identification.  slice number and stereotaxic coordinate will bet set.
 * @param histologySlicesFile
 *    The histology slices file (MUST be valid)
 * @param mediaFile
 *    The media file (may be NULL)
 * @param sliceIndex
 *    The slice index
 * @param planeXYZ
 *    The plane coordinate
 */
HistologyCoordinate
HistologyCoordinate::newInstancePlaneXYZChanged(HistologySlicesFile* histologySlicesFile,
                                               const int32_t sliceIndex,
                                               const Vector3D& planeXYZ)
{
    CaretAssert(histologySlicesFile);
    
    HistologyCoordinate hc;
    hc.setHistologySlicesFile(histologySlicesFile);
    hc.setMediaFile(NULL);
    hc.setSliceIndex(sliceIndex);
    hc.setSliceNumber(histologySlicesFile->getSliceNumberBySliceIndex(sliceIndex));
    hc.setPlaneXYZ(planeXYZ);
    hc.m_planeXY = planeXYZ;
    
    const HistologySlice* slice(histologySlicesFile->getHistologySliceByIndex(sliceIndex));
    if (slice != NULL) {
        Vector3D stereotaxicXYZ;
        if (slice->planeXyzToStereotaxicXyz(planeXYZ,
                                            stereotaxicXYZ)) {
            hc.setStereotaxicXYZ(stereotaxicXYZ);
        }
    }
    
    return hc;
}

/**
 * New instance for closest slices to the stereotaxic  coordinate
 * @param histologySlicesFile
 *    The histology slices file (MUST be valid)
 * @param stereotaxicXYZ
 *    The stereotaxic coordinate
 */
HistologyCoordinate
HistologyCoordinate::newInstanceStereotaxicXYZ(HistologySlicesFile* histologySlicesFile,
                                               const Vector3D& stereotaxicXYZ)
{
    if (histologySlicesFile != NULL) {
        float mmDistanceToSlice(0.0);
        Vector3D nearestOnSliceStereotaxicXYZ;
        
        const HistologySlice* nearestSlice(histologySlicesFile->getSliceNearestStereotaxicXyz(stereotaxicXYZ,
                                                                                              mmDistanceToSlice,
                                                                                              nearestOnSliceStereotaxicXYZ));
        if (nearestSlice != NULL) {
            Vector3D planeXYZ;
            nearestSlice->stereotaxicXyzToPlaneXyz(nearestOnSliceStereotaxicXYZ,
                                                   planeXYZ);
            const int32_t sliceNumber(nearestSlice->getSliceNumber());
            HistologyCoordinate hc;
            hc.setHistologySlicesFile(histologySlicesFile);
            hc.setSliceIndex(histologySlicesFile->getSliceIndexFromSliceNumber(sliceNumber));
            hc.setSliceNumber(sliceNumber);
            hc.setPlaneXYZ(planeXYZ);
            hc.setStereotaxicXYZ(nearestOnSliceStereotaxicXYZ);
            
            return hc;
        }
    }
    
    HistologyCoordinate hc;
    return hc;
}

/**
 * New instance for closest slices to the stereotaxic  coordinate
 * @param histologySlicesFile
 *    The histology slices file (MUST be valid)
 * @param stereotaxicXYZ
 *    The stereotaxic coordinate
 */
HistologyCoordinate
HistologyCoordinate::newInstanceSliceIndexChanged(HistologySlicesFile* histologySlicesFile,
                                                  const HistologyCoordinate& histologyCoordinate,
                                                  const int32_t sliceIndex)
{
    if ( ! histologyCoordinate.isValid()) {
        const AString msg("Histology coordinate must be valid when slice index changed");
        CaretLogSevere(msg);
        CaretAssertMessage(0, msg);
        HistologyCoordinate hc;
        return hc;
    }
    
    const HistologySlice* histologySlice(histologySlicesFile->getHistologySliceByIndex(sliceIndex));
    if (histologySlice == NULL) {
        const AString msg("Invalid slice index=" + AString::number(sliceIndex));
        CaretLogSevere(msg);
        CaretAssertMessage(0, msg);
        HistologyCoordinate hc;
        return hc;
    }
    
    const Plane plane(histologySlice->getPlaneXyzPlane());
    if (plane.isValidPlane()) {
        Vector3D newPlaneXYZ;
        /*
         * Move previous plane coord to the new slice's plane
         */
        plane.projectPointToPlane(histologyCoordinate.getPlaneXYZ(),
                                  newPlaneXYZ);

        Vector3D newStereotaxicXYZ;
        histologySlice->planeXyzToStereotaxicXyz(newPlaneXYZ,
                                                 newStereotaxicXYZ);

        HistologyCoordinate hc;
        hc.setHistologySlicesFile(histologySlicesFile);
        hc.setSliceIndex(sliceIndex);
        hc.setSliceNumber(histologySlicesFile->getSliceNumberBySliceIndex(sliceIndex));
        hc.setPlaneXYZ(newPlaneXYZ);
        hc.setStereotaxicXYZ(newStereotaxicXYZ);
        return hc;
    }
 
    HistologyCoordinate hc;
    return hc;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
HistologyCoordinate::HistologyCoordinate(const HistologyCoordinate& obj)
: CaretObject(obj)
{
    initializeMembers();
    this->copyHelperHistologyCoordinate(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
HistologyCoordinate&
HistologyCoordinate::operator=(const HistologyCoordinate& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperHistologyCoordinate(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
HistologyCoordinate::copyHelperHistologyCoordinate(const HistologyCoordinate& obj)
{
    m_stereotaxicXYZ                  = obj.m_stereotaxicXYZ;
    m_histologySlicesFileName         = obj.m_histologySlicesFileName;
    m_histologyMediaFileName          = obj.m_histologyMediaFileName;
    m_sliceIndex                      = obj.m_sliceIndex;
    m_planeXY                         = obj.m_planeXY;
    m_stereotaxicXYZValid             = obj.m_stereotaxicXYZValid;
    m_planeXYValid                    = obj.m_planeXYValid;
    m_sliceIndexValid                 = obj.m_sliceIndexValid;
    m_sliceNumber                     = obj.m_sliceNumber;
    m_sliceNumberValid                = obj.m_sliceNumberValid;
}

/**
 * Initialize members of an instance
 */
void
HistologyCoordinate::initializeMembers()
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_stereotaxicXYZ",
                               m_stereotaxicXYZ,
                               3,
                               0.0);
    m_sceneAssistant->add("m_stereotaxicXYZValid",
                          &m_stereotaxicXYZValid);
    m_sceneAssistant->addArray("m_planeXY",
                               m_planeXY,
                               3,
                               0.0);
    m_sceneAssistant->add("m_planeXYValid",
                          &m_planeXYValid);
    m_sceneAssistant->add("m_histologyMediaFileName",
                          &m_histologyMediaFileName);
    m_sceneAssistant->add("m_sliceIndex",
                          &m_sliceIndex);
    m_sceneAssistant->add("m_sliceIndexValid",
                          &m_sliceIndexValid);
    m_sceneAssistant->add("m_sliceNumber",
                          &m_sliceNumber);
    m_sceneAssistant->add("m_sliceNumberValid",
                          &m_sliceNumberValid);
}

/**
 * Copy the yoked settings of the coordinate
 * @param histologySlicesFile
 *    The histology slices file
 * @param histologyCoordinate
 *    Coordinate that is copied.
 */
void
HistologyCoordinate::copyYokedSettings(const HistologySlicesFile* histologySlicesFile,
                                       const HistologyCoordinate& histologyCoordinate)
{
    if (&histologyCoordinate == this) {
        return;
    }
    
    if (histologyCoordinate.isValid()
        && histologyCoordinate.isStereotaxicXYZValid()) {
        HistologyCoordinate newCoord(HistologyCoordinate::newInstanceStereotaxicXYZ(const_cast<HistologySlicesFile*>(histologySlicesFile),
                                                                                    histologyCoordinate.getStereotaxicXYZ()));
        *this = newCoord;
    }
    else {
        CaretLogSevere("Histology coordinate must be valid with valid stereotaxic coordinates");
    }
}

/**
 * @param is this coordinate valid?
 */
bool
HistologyCoordinate::isValid() const
{
    if ( ( ! m_histologySlicesFileName.isEmpty())
        && (m_planeXYValid)) {
        return true;
    }
    
    return false;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologyCoordinate::toString() const
{
    return "HistologyCoordinate";
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
HistologyCoordinate::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologyCoordinate",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    sceneClass->addPathName("m_histologySlicesFileName",
                            m_histologySlicesFileName);
    
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
HistologyCoordinate::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    m_histologySlicesFileName = sceneClass->getPathNameValue("m_histologySlicesFileName", "");
}

/**
 * Set the histology slices file.
 * @param histologySlicesFile
 *    Pointer to histology slices file.
 */
void
HistologyCoordinate::setHistologySlicesFile(HistologySlicesFile* histologySlicesFile)
{
    if (histologySlicesFile != NULL) {
        setHistologySlicesFileName(histologySlicesFile->getFileName());
    }
}

/**
 * @return stereotaxic coordinate
 */
Vector3D
HistologyCoordinate::getStereotaxicXYZ() const
{
    return m_stereotaxicXYZ;
}

/**
 * Set the media file.
 * @param mediaFile
 *    Pointer to media file.
 */
void
HistologyCoordinate::setMediaFile(MediaFile* mediaFile)
{
    if (mediaFile != NULL) {
        setHistologyMediaFileName(mediaFile->getFileName());
    }
}

/**
 * Set the stereotaxic coordinate and validity of it
 * @param xyz
 *    New stereotaxic coordinate
 */
void
HistologyCoordinate::setStereotaxicXYZ(const Vector3D& xyz)
{
    m_stereotaxicXYZ = xyz;
    m_stereotaxicXYZValid = true;
}

/**
 * @return name of histology slices file
 */
AString
HistologyCoordinate::getHistologySlicesFileName() const
{
    return m_histologySlicesFileName;
}

/**
 * Set name of histology slices file
 *
 * @param histologySlicesFileName
 *    New value for name of histology slices file
 */
void
HistologyCoordinate::setHistologySlicesFileName(const AString& histologySlicesFileName)
{
    m_histologySlicesFileName = histologySlicesFileName;
}

/**
 * @return name of histology media file
 */
AString
HistologyCoordinate::getHistologyMediaFileName() const
{
    return m_histologyMediaFileName;
}

/**
 * Set name of histology media file
 *
 * @param histologyMediaFileName
 *    New value for name of histology media file
 */
void
HistologyCoordinate::setHistologyMediaFileName(const AString& histologyMediaFileName)
{
    m_histologyMediaFileName = histologyMediaFileName;
}

/**
 * @return index of slice
 */
int64_t
HistologyCoordinate::getSliceIndex() const
{
    return m_sliceIndex;
}

/**
 * Set index of slice.  Will also set slice number if histology file is valid and slice index is valid for the file
 *
 * @param sliceIndex
 *    New value for index of slice
 */
void
HistologyCoordinate::setSliceIndex(const int64_t sliceIndex)
{
    m_sliceIndex = sliceIndex;
    m_sliceIndexValid = true;
}

/**
 * @return plane XYZ coordinate
 */
Vector3D
HistologyCoordinate::getPlaneXYZ() const
{
    return m_planeXY;
}

/**
 * Set the plane coordinate and validity of it
 * @param xyz
 *    New plane coordinate
 */
void
HistologyCoordinate::setPlaneXYZ(const Vector3D& xyz)
{
    m_planeXY = xyz;
    m_planeXYValid = true;
}

/**
 * @return validity of stereotaxic XYZ
 */
bool
HistologyCoordinate::isStereotaxicXYZValid() const
{
    return m_stereotaxicXYZValid;
}

/**
 * @return validity of plane XY
 */
bool
HistologyCoordinate::isPlaneXYValid() const
{
    return m_planeXYValid;
}

/**
 * @return validity of histology slices file name
 */
bool
HistologyCoordinate::isHistologySlicesFileNameValid() const
{
    return ( ! m_histologySlicesFileName.isEmpty());
}

/**
 * @return validity of histology media file name
 */
bool
HistologyCoordinate::isHistologyMediaFileNameValid() const
{
    return ( ! m_histologyMediaFileName.isEmpty());
}

/**
 * @return validity of slice index
 */
bool
HistologyCoordinate::isSliceIndexValid() const
{
    return m_sliceIndexValid;
}

/**
 * @return number of slice
 */
int64_t
HistologyCoordinate::getSliceNumber() const
{
    return m_sliceNumber;
}

/**
 * Set number of slice
 *
 * @param sliceNumber
 *    New value for number of slice
 */
void
HistologyCoordinate::setSliceNumber(const int64_t sliceNumber)
{
    m_sliceNumber = sliceNumber;
    m_sliceNumberValid = true;
}

/**
 * @return validity of slice number
 */
bool
HistologyCoordinate::isSliceNumberValid() const
{
    return m_sliceNumberValid;
}

