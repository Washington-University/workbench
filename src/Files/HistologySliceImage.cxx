
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

#define __HISTOLOGY_SLICE_IMAGE_DECLARE__
#include "HistologySliceImage.h"
#undef __HISTOLOGY_SLICE_IMAGE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziImageFile.h"
#include "DataFileException.h"
#include "EventManager.h"
#include "ImageFile.h"
#include "MediaFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::HistologySliceImage 
 * \brief Contains an image and information related to the image
 * \ingroup Files
 */

/**
 * Constructor.
 * @param sceneName
 *    Namne of the scene
 * @param mediaFileName
 *    Name of media file containing image
 * @param distanceFileName
 *    Name of distance file
 * @param scaledToPlaneMatrix
 *    The scaled to plane matrix for this image
 * @param scaledToPlaneMatrixValidFlag
 *    Validity of scaled to plane matrix
 * @param toStereotaxicNonLinearTransform
 *    Non-linear transform for going to stereotaxic coordinates
 * @param fromStereotaxicNonLinearTransform
 *    Non-linear transform for going from stereotaxic coordinates
 */
HistologySliceImage::HistologySliceImage(const AString& sceneName,
                                         const AString& mediaFileName,
                                         const AString& distanceFileName,
                                         const Matrix4x4& scaledToPlaneMatrix,
                                         const bool scaledToPlaneMatrixValidFlag)
: CaretObject(),
m_sceneName(sceneName),
m_mediaFileName(mediaFileName),
m_distanceFileName(distanceFileName),
m_scaledToPlaneMatrix(scaledToPlaneMatrix),
m_scaledToPlaneMatrixValidFlag(scaledToPlaneMatrixValidFlag)
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
HistologySliceImage::~HistologySliceImage()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
HistologySliceImage::HistologySliceImage(const HistologySliceImage& obj)
: CaretObject(obj),
EventListenerInterface(),
SceneableInterface(obj)
{
    this->copyHelperHistologySliceImage(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
HistologySliceImage&
HistologySliceImage::operator=(const HistologySliceImage& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperHistologySliceImage(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
HistologySliceImage::copyHelperHistologySliceImage(const HistologySliceImage& /*obj*/)
{
    CaretAssertMessage(0, "Copying not supported");
}

/**
 * @return the Media File for this slice image
 */
MediaFile*
HistologySliceImage::getMediaFile()
{
    const MediaFile* mf(getMediaFilePrivate());
    return const_cast<MediaFile*>(mf);
}

/**
 * @return the Media File for this slice image, const method
 */
const MediaFile*
HistologySliceImage::getMediaFile() const
{
    return getMediaFilePrivate();
}

/**
 * @return The media file, will cause readiing of the media file the first time called
 */
const MediaFile*
HistologySliceImage::getMediaFilePrivate() const
{
    
    if (m_attemptedToReadMediaFileFlag) {
        return m_mediaFile.get();
    }
    
    m_attemptedToReadMediaFileFlag = true;
    
    bool validExtensionFlag(false);
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(m_mediaFileName,
                                                                                    &validExtensionFlag);
    if ( ! validExtensionFlag) {
        const AString msg("File extension is not recogonized by Workbench: "
                          + m_mediaFileName);
        CaretLogSevere(msg);
        return NULL;
    }
    
    switch (dataFileType) {
        case DataFileTypeEnum::CZI_IMAGE_FILE:
            try {
                std::unique_ptr<CziImageFile> cziImageFile(new CziImageFile(m_distanceFileName));
                cziImageFile->readFile(m_mediaFileName);
                m_mediaFile.reset(cziImageFile.release());
            }
            catch (const DataFileException& dfe) {
                const AString msg("Error while reading "
                                  + m_mediaFileName
                                  + ": "
                                  + dfe.whatString());
                CaretLogSevere(msg);
            }
            break;
        case DataFileTypeEnum::IMAGE:
            try {
                std::unique_ptr<ImageFile> imageFile(new ImageFile());
                imageFile->readFile(m_mediaFileName);
                m_mediaFile.reset(imageFile.release());
            }
            catch (const DataFileException& dfe) {
                const AString msg("Error while reading "
                                  + m_mediaFileName
                                  + ": "
                                  + dfe.whatString());
                CaretLogSevere(msg);
            }
            break;
        default:
            const AString msg("File extension is not supported for reading file "
                              + m_mediaFileName);
            CaretLogSevere(msg);
            break;
    }
    
    if (m_mediaFile) {
        m_mediaFile->setTransformMatrices(m_scaledToPlaneMatrix,
                                            m_scaledToPlaneMatrixValidFlag,
                                            m_planeToMillimetersMatrix,
                                            m_planeToMillimetersMatrixValidFlag,
                                          m_toStereotaxicNonLinearTransform,
                                          m_fromStereotaxicNonLinearTransform);
    }
    return m_mediaFile.get();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologySliceImage::toString() const
{
    AString s;
    s.appendWithNewLine("      Media File Name: " + m_mediaFileName);
    s.appendWithNewLine("      Distance File Name: " + m_distanceFileName);
    return s;
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
HistologySliceImage::stereotaxicXyzToPlaneXyz(const Vector3D& stereotaxicXyz,
                                              Vector3D& planeXyzOut) const
{
    const MediaFile* mediaFile(getMediaFile());
    if (mediaFile != NULL) {
        mediaFile->stereotaxicXyzToPlaneXyz(stereotaxicXyz,
                                            planeXyzOut);
        return true;
    }
    planeXyzOut.fill(0);
    return false;

}

/**
 * Set the plane to milimeters matrix
 * @param planeToMillimetersMatrix
 *    The plane to millimeters matrix
 * @param planeToMillimetersMatrixValidFlag
 *    Validity of plane to millimeters matrix
 */
void
HistologySliceImage::setPlaneToMillimetersMatrix(const Matrix4x4& planeToMillimetersMatrix,
                                                 const bool planeToMillimetersMatrixValidFlag,
                                                 std::shared_ptr<CziNonLinearTransform>& toStereotaxicNonLinearTransform,
                                                 std::shared_ptr<CziNonLinearTransform>& fromStereotaxicNonLinearTransform)
{
    m_planeToMillimetersMatrix          = planeToMillimetersMatrix;
    m_planeToMillimetersMatrixValidFlag = planeToMillimetersMatrixValidFlag;
    
    m_toStereotaxicNonLinearTransform   = toStereotaxicNonLinearTransform;
    m_fromStereotaxicNonLinearTransform = fromStereotaxicNonLinearTransform;
}

/**
 * @return names of all child files
 */
std::vector<AString>
HistologySliceImage::getChildDataFilePathNames() const
{
    std::vector<AString> names;
    names.push_back(m_mediaFileName);
    names.push_back(m_distanceFileName);
    return names;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
HistologySliceImage::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
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
HistologySliceImage::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "HistologySliceImage",
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
HistologySliceImage::restoreFromScene(const SceneAttributes* sceneAttributes,
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

