
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include <set>

#define __ANNOTATION_GROUP_DECLARE__
#include "AnnotationGroup.h"
#undef __ANNOTATION_GROUP_DECLARE__

#include "Annotation.h"
#include "AnnotationPointSizeText.h"
#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationGroup 
 * \brief Contains a group of annotations.
 * \ingroup Annotations
 */

/**
 * Constructor.
 *
 * @param annotationFile
 *     File to which this group belongs.
 * @param groupType
 *     Type of annotation group.
 * @param uniqueKey
 *     Unique key for this group.
 * @param coordinateSpace
 *     Annotation coordinate space for the group.
 * @param tabOrWindowIndex
 *     Index of tab or window for tab or window space.
 */
AnnotationGroup::AnnotationGroup(AnnotationFile* annotationFile,
                                 const AnnotationGroupTypeEnum::Enum groupType,
                                 const int32_t uniqueKey,
                                 const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                 const int32_t tabOrWindowIndex)
: CaretObjectTracksModification()
{
    CaretAssert(annotationFile);
    CaretAssert(groupType != AnnotationGroupTypeEnum::INVALID);
    CaretAssert(uniqueKey > 0);
    CaretAssert(coordinateSpace != AnnotationCoordinateSpaceEnum::PIXELS);
    
    initializeInstance();
    
    m_groupKey.setAnnotationFile(annotationFile);
    m_groupKey.setGroupType(groupType);
    
    switch (groupType) {
        case AnnotationGroupTypeEnum::INVALID:
            CaretAssertMessage(0, "Should never get here");
            break;
        case AnnotationGroupTypeEnum::SPACE:
            m_groupKey.setSpaceGroupUniqueKey(uniqueKey);
            break;
        case AnnotationGroupTypeEnum::USER:
            m_groupKey.setUserGroupUniqueKey(uniqueKey);
            break;
    }
    
    m_coordinateSpace  = coordinateSpace;
    m_tabOrWindowIndex = tabOrWindowIndex;
    
    switch (m_coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            CaretAssert((tabOrWindowIndex >= 0)
                        && (tabOrWindowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            CaretAssert((tabOrWindowIndex >= 0)
                        && (tabOrWindowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
            break;
    }
    
    setModified();
}

/**
 * Destructor.
 */
AnnotationGroup::~AnnotationGroup()
{
    m_annotations.clear();
    
    delete m_sceneAssistant;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationGroup::AnnotationGroup(const AnnotationGroup& obj)
: CaretObjectTracksModification(obj)
{
    initializeInstance();
    this->copyHelperAnnotationGroup(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
AnnotationGroup&
AnnotationGroup::operator=(const AnnotationGroup& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperAnnotationGroup(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationGroup::copyHelperAnnotationGroup(const AnnotationGroup& obj)
{
    m_groupKey         = obj.m_groupKey;
    m_coordinateSpace  = obj.m_coordinateSpace;
    m_name             = obj.m_name;
    m_tabOrWindowIndex = obj.m_tabOrWindowIndex;
    CaretAssertMessage(0, "What to do with annotations remove copy constructor/operator=");
}

/**
 * Initialize an instance.
 */
void
AnnotationGroup::initializeInstance()
{
    m_groupKey.reset();
    m_coordinateSpace  = AnnotationCoordinateSpaceEnum::PIXELS;
    m_name             = "";
    m_tabOrWindowIndex = -1;
    
    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * @return Is this group valid?
 */
bool
AnnotationGroup::isEmpty() const
{
    return (m_annotations.empty());
}

/**
 * @return The annotation group key.
 */
AnnotationGroupKey
AnnotationGroup::getAnnotationGroupKey() const
{
    return m_groupKey;
}

/**
 * @return Annotation file that contains this group.
 */
AnnotationFile*
AnnotationGroup::getAnnotationFile() const
{
    return m_groupKey.getAnnotationFile();
}

/**
 * @return The group type.
 */
AnnotationGroupTypeEnum::Enum
AnnotationGroup::getGroupType() const
{
    return m_groupKey.getGroupType();
}

/**
 * @return Coordinate space of the annotations.
 */
AnnotationCoordinateSpaceEnum::Enum
AnnotationGroup::getCoordinateSpace() const
{
    return m_coordinateSpace;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationGroup::toString() const
{
    return "AnnotationGroup";
}

/**
 * @return Name of the annotation group.
 */
AString
AnnotationGroup::getName() const
{
    return m_name;
}

/**
 * @return Index or tab or window for tab or window space.
 */
int32_t
AnnotationGroup::getTabOrWindowIndex() const
{
    return m_tabOrWindowIndex;
}

/**
 * @return Unique key displayed in annotation group name.
 */
int32_t
AnnotationGroup::getUniqueKey() const
{
    int32_t uniqueKey = -1;
    
    switch (m_groupKey.getGroupType()) {
        case AnnotationGroupTypeEnum::INVALID:
            CaretAssertMessage(0, "Should never get here");
            break;
        case AnnotationGroupTypeEnum::SPACE:
            uniqueKey = m_groupKey.getSpaceGroupUniqueKey();
            break;
        case AnnotationGroupTypeEnum::USER:
            uniqueKey = m_groupKey.getUserGroupUniqueKey();
            break;
    }
    
    return uniqueKey;
}

/**
 * Private method for adding annotations to this file.
 *
 * In the GUI, annotations are added using the AnnotationRedoUndoCommand
 * which allows undo/redo operations.
 *
 * @param annotation
 *     Annotation that is added.
 */
void
AnnotationGroup::addAnnotationPrivate(Annotation* annotation)
{
    if ( ! validateAddedAnnotation(annotation)) {
        delete annotation;
        return;
    }

    assignGroupKeyToAnnotation(annotation);
    
    m_annotations.push_back(QSharedPointer<Annotation>(annotation));
    setModified();
}

/**
 * Private method for adding annotations to this file using shared pointer.
 *
 * In the GUI, annotations are added using the AnnotationRedoUndoCommand
 * which allows undo/redo operations.
 *
 * @param annotation
 *     Annotation that is added.
 */
void
AnnotationGroup::addAnnotationPrivateSharedPointer(QSharedPointer<Annotation>& annotation)
{
    if ( ! validateAddedAnnotation(annotation.data())) {
        return;
    }
    
    assignGroupKeyToAnnotation(annotation.data());
    
    m_annotations.push_back(annotation);
    setModified();
}

/**
 * Assign group key for the given annotation.
 *
 * @param annotation
 *     The annotation.
 */
void
AnnotationGroup::assignGroupKeyToAnnotation(Annotation* annotation)
{
    annotation->setAnnotationGroupKey(m_groupKey);
    
//    AnnotationGroupKey annotationGroupKey;
//    annotationGroupKey.setAnnotationFile(m_groupKey.getAnnotationFile());
//    switch (m_groupKey.getGroupType()) {
//        case AnnotationGroupTypeEnum::INVALID:
//            CaretAssert(0);
//            break;
//        case AnnotationGroupTypeEnum::SPACE:
//            /*
//             * For space group, do not reset user group key
//             * since it is used for a regroup operation.
//             */
//            annotationGroupKey.setGroupType(AnnotationGroupTypeEnum::SPACE);
//            CaretAssert(m_groupKey.getSpaceGroupUniqueKey() > 0);
//            annotationGroupKey.setSpaceGroupUniqueKey(m_groupKey.getSpaceGroupUniqueKey());
//            break;
//        case AnnotationGroupTypeEnum::USER:
//            annotationGroupKey.setGroupType(AnnotationGroupTypeEnum::USER);
//            annotationGroupKey.setSpaceGroupUniqueKey(-1);
//            CaretAssert(m_groupKey.getUserGroupUniqueKey() > 0);
//            annotationGroupKey.setUserGroupUniqueKey(m_groupKey.getUserGroupUniqueKey());
//            break;
//    }
//    annotation->setAnnotationGroupKey(annotationGroupKey);
}

/**
 * Validate the annotation that is being added to this group.
 *
 * @param annotation
 *     Pointer to annotation.
 * @return
 *     True if the annotation is compatible with this group.
 */
bool
AnnotationGroup::validateAddedAnnotation(const Annotation* annotation)
{
    if (annotation->getType() == AnnotationTypeEnum::TEXT) {
        const AnnotationPointSizeText* pointSizeAnnotation = dynamic_cast<const AnnotationPointSizeText*>(annotation);
        if (pointSizeAnnotation != NULL) {
            CaretLogWarning("Point size text annotations are not supported in AnnotationGroup.  "
                            "The annotation has been discarded.");
            return false;
        }
    }
    
    const AnnotationCoordinateSpaceEnum::Enum space = annotation->getCoordinateSpace();
    if (space != m_coordinateSpace) {
        CaretLogSevere("Attempting to add annotation with non-matching coordinate space");
        CaretAssert(0);
        return false;
    }
    
    switch (space) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            if (m_tabOrWindowIndex != annotation->getTabIndex()) {
                CaretLogSevere("Attempting to add anntation with non-matching tab index: ");
                CaretAssert(0);
                return false;
            }
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (m_tabOrWindowIndex != annotation->getWindowIndex()) {
                CaretLogSevere("Attempting to add anntation with non-matching tab index: ");
                CaretAssert(0);
                return false;
            }
            break;
    }
    
    return true;
}

/**
 * @return The maximum unique key found in this group and
 * its annotations.
 */
int32_t
AnnotationGroup::getMaximumUniqueKey() const
{
    int32_t maxUniqueKey = getUniqueKey();
    
    for (AnnotationConstIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        maxUniqueKey = std::max(maxUniqueKey,
                                (*iter)->getUniqueKey());
    }
    
    return maxUniqueKey;
}

/**
 * Remove all annotations from this group.
 *
 * @param allRemovedAnnotationsOut
 *    Output containing all annotations from this group.
 */
void
AnnotationGroup::removeAllAnnotations(std::vector<QSharedPointer<Annotation> >& allRemovedAnnotationsOut)
{
    allRemovedAnnotationsOut = m_annotations;
    m_annotations.clear();
    setModified();
}


/**
 * Remove the annotation.  NOTE: The annotation is NOT deleted
 * but instead it is returned so that it can be 'undeleted'
 * or 're-pasted'.
 *
 * @param annotation
 *     Annotation that is removed.
 * @param removedAnnotationOut
 *     Shared pointer for annotation that was removed so
 *     the file can later undelete the annotation.  Only 
 *     valid when true is returned.
 * @return
 *     True if the annotation was removed, otherwise false.
 */
bool
AnnotationGroup::removeAnnotation(Annotation* annotation,
                                  QSharedPointer<Annotation>& removedAnnotationOut)
{
    removedAnnotationOut.clear();
    
    for (AnnotationIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        QSharedPointer<Annotation>& annotationPointer = *iter;
        if (annotationPointer == annotation) {
            removedAnnotationOut = annotationPointer;
            
            m_annotations.erase(iter);
            
            setModified();
            
            /*
             * Successfully removed
             */
            return true;
        }
    }
    
    /*
     * Annotation not in this file
     */
    return false;
}

/**
 * @return Number of annotations in this group.
 */
int32_t
AnnotationGroup::getNumberOfAnnotations() const
{
    return m_annotations.size();
}

/**
 * @param index
 *     Get the annotation at the given index.
 * @return
 *     Annotation at the given index.
 */
Annotation*
AnnotationGroup::getAnnotation(const int32_t index)
{
    CaretAssertVectorIndex(m_annotations, index);
    return m_annotations[index].data();
}

/**
 * @param index
 *     Get the annotation at the given index.
 * @return
 *     Annotation at the given index.
 */
const Annotation*
AnnotationGroup::getAnnotation(const int32_t index) const
{
    CaretAssertVectorIndex(m_annotations, index);
    return m_annotations[index].data();
}

/**
 * Get all annotations in this group.
 *
 * @param annotationsOut
 *    Output containing all annotations.
 */
void
AnnotationGroup::getAllAnnotations(std::vector<Annotation*>& annotationsOut) const
{
    annotationsOut.clear();

    for (AnnotationConstIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        annotationsOut.push_back((*iter).data());
    }
}

/**
 * Set the selection status for all annotations in this group
 * in the given window.
 *
 * @param windowIndex
 *     Index of window.
 * @param selectedStatus
 *     The selection status.
 */
void
AnnotationGroup::setAllAnnotationsSelected(const int32_t windowIndex,
                                           const bool selectedStatus)
{
    for (AnnotationIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        (*iter)->setSelected(windowIndex,
                             selectedStatus);
    }
}

/**
 * @return true if file is modified, else false.
 */
bool
AnnotationGroup::isModified() const
{
    for (AnnotationConstIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        const QSharedPointer<Annotation>& annotationPointer = *iter;
        if (annotationPointer->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modified status of this file.
 */
void
AnnotationGroup::clearModified()
{
    for (AnnotationIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        (*iter)->clearModified();
    }
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
AnnotationGroup::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationGroup",
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
AnnotationGroup::restoreFromScene(const SceneAttributes* sceneAttributes,
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

