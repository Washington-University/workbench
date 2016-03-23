
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
#include "DisplayGroupAndTabItemHelper.h"
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
    
    delete m_displayGroupAndTabItemHelper;
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
    *m_displayGroupAndTabItemHelper = *obj.m_displayGroupAndTabItemHelper;
    
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
    
    m_displayGroupAndTabItemHelper = new DisplayGroupAndTabItemHelper();
    
    m_sceneAssistant = new SceneClassAssistant();

    m_sceneAssistant->add("m_displayGroupAndTabItemHelper",
                          "DisplayGroupAndTabItemHelper",
                          m_displayGroupAndTabItemHelper);
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
    if (m_name.isEmpty()) {
        
        
        AString spaceName = AnnotationCoordinateSpaceEnum::toGuiName(m_coordinateSpace);
        switch (m_coordinateSpace) {
            case AnnotationCoordinateSpaceEnum::PIXELS:
                CaretAssertMessage(0, "Should never be pixels");
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                spaceName.append(" "
                                 + AString::number(getTabOrWindowIndex() + 1));
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                spaceName.append(" "
                                 + AString::number(getTabOrWindowIndex() + 1));
                break;
        }
        
        switch (m_groupKey.getGroupType()) {
            case AnnotationGroupTypeEnum::INVALID:
                CaretAssertMessage(0, "Should never get here");
                break;
            case AnnotationGroupTypeEnum::SPACE:
                m_name.append("Space Group "
                              + spaceName);
                break;
            case AnnotationGroupTypeEnum::USER:
                m_name.append("User Group "
                              + AString::number(m_groupKey.getUserGroupUniqueKey())
                              + " "
                              + spaceName);
                break;
        }
    }
    
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
    annotation->setItemParent(this);
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
            annotation->setItemParent(NULL);
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
 * Set the selection for editing status for all annotations in this group
 * in the given window.
 *
 * @param windowIndex
 *     Index of window.
 * @param selectedStatus
 *     The selection status.
 */
void
AnnotationGroup::setAllAnnotationsSelectedForEditing(const int32_t windowIndex,
                                           const bool selectedStatus)
{
    for (AnnotationIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        (*iter)->setSelectedForEditing(windowIndex,
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
 * @return name for an annotation saved to scene.
 *
 * @param uniqueKey
 *     Unique key of the annotation in the file.
 */
AString
AnnotationGroup::getSceneClassNameForAnnotationUniqueKey(const int32_t uniqueKey)
{
    return ("Ann-" + AString::number(uniqueKey));
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
    
    const int32_t annCount = getNumberOfAnnotations();
    for (int32_t i = 0; i < annCount; i++) {
        const int32_t uniqueKey = getAnnotation(i)->getUniqueKey();
        SceneClass* annClass = getAnnotation(i)->saveToScene(sceneAttributes,
                                      getSceneClassNameForAnnotationUniqueKey(uniqueKey));
        sceneClass->addClass(annClass);
    }
    
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

    const int32_t annCount = getNumberOfAnnotations();
    for (int32_t i = 0; i < annCount; i++) {
        const int32_t uniqueKey = getAnnotation(i)->getUniqueKey();
        const SceneClass* annClass = sceneClass->getClass(getSceneClassNameForAnnotationUniqueKey(uniqueKey));
        if (sceneClass != NULL) {
            getAnnotation(i)->restoreFromScene(sceneAttributes,
                                               annClass);
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * @return Number of children.
 */
int32_t
AnnotationGroup::getNumberOfItemChildren() const
{
    return m_annotations.size();
}

/**
 * Get child at the given index.
 *
 * @param index
 *     Index of the child.
 * @return
 *     Child at the given index.
 */
DisplayGroupAndTabItemInterface*
AnnotationGroup::getItemChild(const int32_t index) const
{
    CaretAssertVectorIndex(m_annotations,
                           index);
    return m_annotations[index].data();
}

/**
 * @return Children of this item.
 */
std::vector<DisplayGroupAndTabItemInterface*>
AnnotationGroup::getItemChildren() const
{
    std::vector<DisplayGroupAndTabItemInterface*> children;
    
    for (AnnotationConstIterator annIter = m_annotations.begin();
         annIter != m_annotations.end();
         annIter++) {
        children.push_back((*annIter).data());
    }
    
    return children;
}

/**
 * @return Parent of this item.
 */
DisplayGroupAndTabItemInterface*
AnnotationGroup::getItemParent() const
{
    return m_displayGroupAndTabItemHelper->getParent();
}

/**
 * Set the parent of this item.
 *
 * @param itemParent
 *     Parent of this item.
 */
void
AnnotationGroup::setItemParent(DisplayGroupAndTabItemInterface* itemParent)
{
    m_displayGroupAndTabItemHelper->setParent(itemParent);
}

/**
 * @return Name of this item.
 */
AString
AnnotationGroup::getItemName() const
{
    return getName();
}

/**
 * Get the icon color for this item.  Icon is filled with background
 * color, outline color is drawn around edges, and text color is small
 * square in center.  For any colors that do not apply, use an alpha
 * value (last element) of zero.
 *
 * @param backgroundRgbaOut
 *     Red, green, blue, alpha components for background ranging [0, 1].
 * @param outlineRgbaOut
 *     Red, green, blue, alpha components for outline ranging [0, 1].
 * @param textRgbaOut
 *     Red, green, blue, alpha components for text ranging [0, 1].
 */
void
AnnotationGroup::getItemIconColorsRGBA(float backgroundRgbaOut[4],
                                       float outlineRgbaOut[4],
                                       float textRgbaOut[4]) const
{
    backgroundRgbaOut[0] = 0.0;
    backgroundRgbaOut[1] = 0.0;
    backgroundRgbaOut[2] = 0.0;
    backgroundRgbaOut[3] = 0.0;

    outlineRgbaOut[0] = 0.0;
    outlineRgbaOut[1] = 0.0;
    outlineRgbaOut[2] = 0.0;
    outlineRgbaOut[3] = 0.0;

    textRgbaOut[0] = 0.0;
    textRgbaOut[1] = 0.0;
    textRgbaOut[2] = 0.0;
    textRgbaOut[3] = 0.0;
}

/**
 * @return This item can be expanded.
 */
bool
AnnotationGroup::isItemExpandable() const
{
    return true;
}

/**
 * @return Is this item expanded in the given display group/tab?
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 */
bool
AnnotationGroup::isItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex) const
{
    if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW) {
       return  m_displayGroupAndTabItemHelper->isExpandedInWindow(m_tabOrWindowIndex);
    }
    
    return m_displayGroupAndTabItemHelper->isExpanded(displayGroup,
                                                      tabIndex);
}

/**
 * Set this item's expanded status in the given display group/tab.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     New expanded status.
 */
void
AnnotationGroup::setItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const bool status)
{
    if (m_coordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW) {
        m_displayGroupAndTabItemHelper->setExpandedInWindow(m_tabOrWindowIndex,
                                                            status);
    }
    else {
        m_displayGroupAndTabItemHelper->setExpanded(displayGroup,
                                                    tabIndex,
                                                    status);
    }
}

/**
 * Get display selection status in the given display group/tab?
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 */
TriStateSelectionStatusEnum::Enum
AnnotationGroup::getItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex) const
{
    TriStateSelectionStatusEnum::Enum status = TriStateSelectionStatusEnum::UNSELECTED;
    
    const int numChildren = getNumberOfAnnotations();
    if (numChildren > 0) {
        int32_t selectedCount = 0;
        for (int32_t i = 0; i < numChildren; i++) {
            CaretAssertVectorIndex(m_annotations, i);
            switch (m_annotations[i]->getItemDisplaySelected(displayGroup,
                                                      tabIndex)) {
                case TriStateSelectionStatusEnum::PARTIALLY_SELECTED:
                    CaretAssertMessage(0, "Annotation should never be partially selected.");
                    break;
                case TriStateSelectionStatusEnum::SELECTED:
                    selectedCount++;
                    break;
                case TriStateSelectionStatusEnum::UNSELECTED:
                    break;
            }
        }
        
        if (selectedCount == numChildren) {
            status = TriStateSelectionStatusEnum::SELECTED;
        }
        else if (selectedCount > 0) {
            status = TriStateSelectionStatusEnum::PARTIALLY_SELECTED;
        }
    }
    
    return status;
}

/**
 * Set display this item selected in the given display group/tab.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     New selection status.
 */
void
AnnotationGroup::setItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const TriStateSelectionStatusEnum::Enum status)
{
    switch (status) {
        case TriStateSelectionStatusEnum::PARTIALLY_SELECTED:
            CaretAssertMessage(0, "Annotation group should never be set to partially selected.");
            return;
            break;
        case TriStateSelectionStatusEnum::SELECTED:
            break;
        case TriStateSelectionStatusEnum::UNSELECTED:
            break;
    }
    
    /*
     * Note: An annotation group's selection status is based
     * of the the group's annotations so we do not need to set
     * an explicit selection status for the group.
     */
    
    DisplayGroupAndTabItemInterface::setChildrenDisplaySelectedHelper(this,
                                                                displayGroup,
                                                                tabIndex,
                                                                status);
}

