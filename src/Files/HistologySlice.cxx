
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

#include "CaretAssert.h"
#include "EventManager.h"
#include "HistologySliceImage.h"
#include "MediaFile.h"
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
 * @param sliceNumber
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
HistologySlice::HistologySlice(const int32_t sliceNumber,
                               const AString& MRIToHistWarpFileName,
                               const AString& histToMRIWarpFileName,
                               const Matrix4x4& planeToMillimetersMatrix,
                               const bool planeToMillimetersMatrixValidFlag)
: CaretObject(),
m_sliceNumber(sliceNumber),
m_MRIToHistWarpFileName(MRIToHistWarpFileName),
m_histToMRIWarpFileName(histToMRIWarpFileName),
m_planeToMillimetersMatrix(planeToMillimetersMatrix),
m_planeToMillimetersMatrixValidFlag(planeToMillimetersMatrixValidFlag)
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
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
: CaretObject(obj)
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
}

/**
 * @return Number of the slice
 */
int32_t
HistologySlice::getSliceNumber() const
{
    return m_sliceNumber;
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
                                                     m_planeToMillimetersMatrixValidFlag);
    std::unique_ptr<HistologySliceImage> ptr(histologySliceImage);
    m_histologySliceImages.push_back(std::move(ptr));
    
    m_stereotaxicXyzBoundingBoxValidFlag = false;
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
 * @return The plane to millimeters matrix
 */
Matrix4x4
HistologySlice::getPlaneToMillimetersMatrix() const
{
    return m_planeToMillimetersMatrix;
}

/**
 * @return Is the plane to millimeters matrix valid?
 */
bool
HistologySlice::isPlaneToMillimetersMatrixValid() const
{
    return m_planeToMillimetersMatrixValidFlag;
}

/**
 * @return BoundingBox for the slice (bounding box of all images in slice)
 */
BoundingBox
HistologySlice::getStereotaxicXyzBoundingBox() const
{
    if ( ! m_stereotaxicXyzBoundingBoxValidFlag) {
        m_stereotaxicXyzBoundingBox.resetForUpdate();
        
        for (auto& slice : m_histologySliceImages) {
            BoundingBox bb(slice->getMediaFile()->getStereotaxicXyzBoundingBox());
            m_stereotaxicXyzBoundingBox.unionOperation(bb);
        }
        m_stereotaxicXyzBoundingBoxValidFlag = true;
    }
    
    return m_stereotaxicXyzBoundingBox;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
HistologySlice::toString() const
{
    AString s("Histology slice "
              + AString::number(getSliceNumber()));
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

