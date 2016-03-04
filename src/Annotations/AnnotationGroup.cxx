
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
 * @param groupType
 *     Type of annotation group.
 * @param annotationFile
 *     Annotation file that contains this group.
 * @param coordinateSpace
 *     Annotation coordinate space for the group.
 * @param tabOrWindowIndex
 *     Index of tab or window for tab or window space.
 */
AnnotationGroup::AnnotationGroup(AnnotationFile* annotationFile,
                                 const AnnotationGroupTypeEnum::Enum groupType,
                                 const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                 const int32_t tabOrWindowIndex)
: CaretObjectTracksModification()
{
    initializeInstance();
    
    m_annotationFile   = annotationFile;
    m_groupType        = groupType;
    m_coordinateSpace  = coordinateSpace;
    m_tabOrWindowIndex = tabOrWindowIndex;
    
    CaretAssert(m_annotationFile);
    CaretAssert(m_coordinateSpace != AnnotationCoordinateSpaceEnum::PIXELS);
    
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
    m_annotationFile   = obj.m_annotationFile;
    m_groupType        = obj.m_groupType;
    m_coordinateSpace  = obj.m_coordinateSpace;
    m_name             = obj.m_name;
    m_tabOrWindowIndex = obj.m_tabOrWindowIndex;
    m_uniqueKey        = obj.m_uniqueKey;
    CaretAssertMessage(0, "What to do with annotations remove copy constructor/operator=");
}

/**
 * Initialize an instance.
 */
void
AnnotationGroup::initializeInstance()
{
    m_annotationFile   = NULL;
    m_groupType        = AnnotationGroupTypeEnum::INVALID;
    m_coordinateSpace  = AnnotationCoordinateSpaceEnum::PIXELS;
    m_name             = "";
    m_tabOrWindowIndex = -1;
    m_uniqueKey        = -1;
    
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
 * @return Annotation file that contains this group.
 */
AnnotationFile*
AnnotationGroup::getAnnotationFile() const
{
    return m_annotationFile;
}

/**
 * @return The group type.
 */
AnnotationGroupTypeEnum::Enum
AnnotationGroup::getGroupType() const
{
    return m_groupType;
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
 * Set the unique key for this annotation group.  This method is
 * called when the annotation group is added to the annotation file.
 *
 * @param uniqueKey
 *     Unique key displayed in an annotation group name.
 */
void
AnnotationGroup::setUniqueKey(const int32_t uniqueKey)
{
    m_uniqueKey = uniqueKey;

    m_name = ("Group "
              + AString::number(m_uniqueKey));
}

/**
 * @return Unique key displayed in annotation group name.
 */
int32_t
AnnotationGroup::getUniqueKey() const
{
    return m_uniqueKey;
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
    if (annotation->getType() == AnnotationTypeEnum::TEXT) {
        AnnotationPointSizeText* pointSizeAnnotation = dynamic_cast<AnnotationPointSizeText*>(annotation);
        if (pointSizeAnnotation != NULL) {
            CaretLogWarning("Point size text annotations are not supported in AnnotationGroup.  "
                            "The annotation has been discarded.");
            delete annotation;
            return;
        }
    }
    
    const AnnotationCoordinateSpaceEnum::Enum space = annotation->getCoordinateSpace();
    if (space != m_coordinateSpace) {
        CaretLogSevere("Attempting to add anntation with non-matching coordinate space");
        CaretAssert(0);
        return;
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
                return;
            }
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if (m_tabOrWindowIndex != annotation->getWindowIndex()) {
                CaretLogSevere("Attempting to add anntation with non-matching tab index: ");
                CaretAssert(0);
                return;
            }
            break;
    }

    m_annotations.push_back(QSharedPointer<Annotation>(annotation));
    setModified();
}

/**
 * Remove the annotation.  NOTE: The annotation is NOT deleted
 * but instead it is saved so that it can be 'undeleted'
 * or 're-pasted'.
 *
 * @param annotation
 *     Annotation that is removed.
 * @return
 *     True if the annotation was removed, otherwise false.
 */
bool
AnnotationGroup::removeAnnotation(Annotation* annotation)
{
    for (AnnotationIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        QSharedPointer<Annotation>& annotationPointer = *iter;
        if (annotationPointer == annotation) {
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

