
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

#define __ANNOTATION_FILE_DECLARE__
#include "AnnotationFile.h"
#undef __ANNOTATION_FILE_DECLARE__

#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFileXmlReader.h"
#include "AnnotationFileXmlWriter.h"
#include "AnnotationGroup.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "CaretLogger.h"
#include "DataFileContentCopyMoveParameters.h"
#include "DataFileException.h"
#include "DisplayGroupAndTabItemHelper.h"
#include "EventAnnotationAddToRemoveFromFile.h"
#include "EventAnnotationGroupGetWithKey.h"
#include "EventAnnotationGrouping.h"
#include "EventManager.h"
#include "GiftiMetaData.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationFile 
 * \brief File containing annotations.
 * \ingroup Files
 */

/**
 * Constructor for annotation file that saves annotations to a file.
 */
AnnotationFile::AnnotationFile()
: CaretDataFile(DataFileTypeEnum::ANNOTATION),
EventListenerInterface(),
 DataFileContentCopyMoveInterface(),
 DisplayGroupAndTabItemInterface(),
m_fileSubType(ANNOTATION_FILE_SAVE_TO_FILE)
{
    initializeAnnotationFile();
}

/**
 * Constructor for annotation file that accepts a sub file type
 * so that annotations may be saved to a file or a scene.
 *
 * This method is intended for use by the "Brain" for its
 * scene annotation file.
 *
 * @param fileSubType
 *     Type of saving of annotations.
 */
AnnotationFile::AnnotationFile(const AnnotationFileSubType fileSubType)
: CaretDataFile(DataFileTypeEnum::ANNOTATION),
m_fileSubType(fileSubType)
{
    initializeAnnotationFile();
}


/**
 * Destructor.
 */
AnnotationFile::~AnnotationFile()
{
    clearPrivate();
    
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_displayGroupAndTabItemHelper;
    delete m_sceneAssistant;
}

/**
 * Clear the content of this file.
 * This method is virtual so do not call from constructor/destructor.
 */
void
AnnotationFile::clear()
{
    const AString nameOfFile = getFileName();
    
    CaretDataFile::clear();
    
    clearPrivate();

    switch (m_fileSubType) {
        case ANNOTATION_FILE_SAVE_TO_FILE:
            break;
        case ANNOTATION_FILE_SAVE_TO_SCENE:
        {
            /*
             * Do not clear the name of the scene annotation file.
             */
            const bool modStatus = isModified();
            setFileName(nameOfFile);
            if ( ! modStatus) {
                clearModified();
            }
        }
            break;
    }
}

/**
 * Clear the content of this file.
 */
void
AnnotationFile::clearPrivate()
{
    m_metadata->clear();

    m_annotationGroups.clear();
    m_removedAnnotations.clear();
}


/**
 * Set the selection for editing status of all annotations.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param selectedStatus
 *     New selection status for all annotations.
 */
void
AnnotationFile::setAllAnnotationsSelectedForEditing(const int32_t windowIndex,
                                          const bool selectedStatus)
{
    std::vector<Annotation*> allAnnotations;
    getAllAnnotations(allAnnotations);
    for (std::vector<Annotation*>::iterator iter = allAnnotations.begin();
         iter != allAnnotations.end();
         iter++) {
        (*iter)->setSelectedForEditing(windowIndex, selectedStatus);
    }
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationFile::AnnotationFile(const AnnotationFile& obj)
: CaretDataFile(obj),
EventListenerInterface(),
DataFileContentCopyMoveInterface(),
DisplayGroupAndTabItemInterface(obj),
m_fileSubType(obj.m_fileSubType)
{
    initializeAnnotationFile();
    
    this->copyHelperAnnotationFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationFile&
AnnotationFile::operator=(const AnnotationFile& obj)
{
    if (this != &obj) {
        CaretDataFile::operator=(obj);
        this->copyHelperAnnotationFile(obj);
    }
    return *this;    
}

/**
 * Initialize an instance of an annotation file.
 */
void
AnnotationFile::initializeAnnotationFile()
{
    m_uniqueKeyGenerator = 0;
    
    m_metadata.grabNew(new GiftiMetaData());
    m_displayGroupAndTabItemHelper = new DisplayGroupAndTabItemHelper();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_displayGroupAndTabItemHelper",
                          "DisplayGroupAndTabItemHelper",
                          m_displayGroupAndTabItemHelper);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_GROUP_GET_WITH_KEY);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_GROUPING);
}

/**
 * Get the coordinate space annotation group with the given annotation's space and for tab/window
 * spaces, the tab/window index.  If the group does not exist, it will be created.
 *
 * @param annotation
 *    Annotation whose group is needed.
 * @return
 *    Group for the coordinateSpace.
 */
AnnotationGroup*
AnnotationFile::getSpaceAnnotationGroup(const Annotation* annotation)
{
    const AnnotationCoordinateSpaceEnum::Enum annotationSpace = annotation->getCoordinateSpace();
    int32_t annotationTabOrWindowIndex = -1;
    switch (annotationSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            annotationTabOrWindowIndex = annotation->getTabIndex();
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            annotationTabOrWindowIndex = annotation->getWindowIndex();
            break;
    }
    
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        AnnotationGroup* group = (*groupIter).data();
        if (group->getGroupType() == AnnotationGroupTypeEnum::SPACE) {
            if (group->getCoordinateSpace() == annotationSpace) {
                switch (annotationSpace) {
                    case AnnotationCoordinateSpaceEnum::PIXELS:
                        CaretAssert(0);
                        break;
                    case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    case AnnotationCoordinateSpaceEnum::SURFACE:
                        return group;
                        break;
                    case AnnotationCoordinateSpaceEnum::VIEWPORT:
                        CaretAssert(0);
                        break;
                    case AnnotationCoordinateSpaceEnum::TAB:
                    case AnnotationCoordinateSpaceEnum::WINDOW:
                        if (annotationTabOrWindowIndex == group->getTabOrWindowIndex()) {
                            return group;
                        }
                        break;
                }
            }
        }
    }
    
    switch (annotationSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            CaretAssert((annotationTabOrWindowIndex >= 0)
                        && (annotationTabOrWindowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            CaretAssert((annotationTabOrWindowIndex >= 0)
                        && (annotationTabOrWindowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
            break;
    }
    
    AnnotationGroup* group = new AnnotationGroup(this,
                                                 AnnotationGroupTypeEnum::SPACE,
                                                 generateUniqueKey(),
                                                 annotationSpace,
                                                 annotationTabOrWindowIndex);
    group->setItemParent(this);
    m_annotationGroups.push_back(QSharedPointer<AnnotationGroup>(group));
    
    return group;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationFile::copyHelperAnnotationFile(const AnnotationFile& obj)
{
    CaretAssertMessage(0, "Copying of annotation file not implemented.  "
                       "Will need to check subtype or have a 'clone' method' that each "
                       "subclass (AnnotationText) implements.");
    *m_displayGroupAndTabItemHelper = *obj.m_displayGroupAndTabItemHelper;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationFile::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE) {
        EventAnnotationAddToRemoveFromFile* annEvent = dynamic_cast<EventAnnotationAddToRemoveFromFile*>(event);
        CaretAssert(annEvent);
        
        AnnotationFile* annotationFile = annEvent->getAnnotationFile();
        Annotation* annotation = annEvent->getAnnotation();
        
        switch (annEvent->getMode()) {
            case EventAnnotationAddToRemoveFromFile::MODE_CREATE:
                if (annotationFile == this) {
                    if (restoreAnnotationAddIfNotFound(annotation)) {
                        annEvent->setSuccessful(true);
                    }
                }
                break;
            case EventAnnotationAddToRemoveFromFile::MODE_CUT:
                if (removeAnnotation(annotation)) {
                    annEvent->setSuccessful(true);
                }
                break;
            case EventAnnotationAddToRemoveFromFile::MODE_DELETE:
                if (removeAnnotation(annotation)) {
                    annEvent->setSuccessful(true);
                }
                break;
            case EventAnnotationAddToRemoveFromFile::MODE_PASTE:
                if (annotationFile == this) {
                    if (restoreAnnotationAddIfNotFound(annotation)) {
                        annEvent->setSuccessful(true);
                    }
                }
                break;
            case EventAnnotationAddToRemoveFromFile::MODE_UNCREATE:
                if (annotationFile == this) {
                    if (removeAnnotation(annotation)) {
                        annEvent->setSuccessful(true);
                    }
                }
                break;
            case EventAnnotationAddToRemoveFromFile::MODE_UNCUT:
                if (restoreAnnotation(annotation)) {
                    annEvent->setSuccessful(true);
                }
                break;
            case EventAnnotationAddToRemoveFromFile::MODE_UNDELETE:
                if (restoreAnnotation(annotation)) {
                    annEvent->setSuccessful(true);
                }
                break;
            case EventAnnotationAddToRemoveFromFile::MODE_UNPASTE:
                if (annotationFile == this) {
                    if (removeAnnotation(annotation)) {
                        annEvent->setSuccessful(true);
                    }
                }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_GROUP_GET_WITH_KEY) {
        EventAnnotationGroupGetWithKey* getGroupEvent = dynamic_cast<EventAnnotationGroupGetWithKey*>(event);
        CaretAssert(getGroupEvent);
        
        const AnnotationGroupKey groupKey = getGroupEvent->getGroupKey();
        
        for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
             groupIter != m_annotationGroups.end();
             groupIter++) {
            AnnotationGroup* group = (*groupIter).data();
            if (groupKey == group->getAnnotationGroupKey()) {
                getGroupEvent->setAnnotationGroup(group);
                getGroupEvent->setEventProcessed();
                return;
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_GROUPING) {
        EventAnnotationGrouping* groupEvent = dynamic_cast<EventAnnotationGrouping*>(event);
        CaretAssert(event);

        if (groupEvent->getAnnotationGroupKey().getAnnotationFile() == this) {
            switch (groupEvent->getMode()) {
                case EventAnnotationGrouping::MODE_INVALID:
                    break;
                case EventAnnotationGrouping::MODE_GROUP:
                    processGroupingAnnotations(groupEvent);
                    break;
                case EventAnnotationGrouping::MODE_REGROUP:
                    processRegroupingAnnotations(groupEvent);
                    break;
                case EventAnnotationGrouping::MODE_UNGROUP:
                    processUngroupingAnnotations(groupEvent);
                    break;
            }
        }
    }
}

/**
 * @return True if this file is empty, else false.
 */
bool
AnnotationFile::isEmpty() const
{
    for (AnnotationGroupConstIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        if ( ! (*groupIter)->isEmpty()) {
            return false;
        }
    }
    
    return true;
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
AnnotationFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
AnnotationFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
AnnotationFile::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
AnnotationFile::getFileMetaData() const
{
    return m_metadata;
}

/**
 * Add information about the content of this file.
 * 
 * @param dataFileInformation
 *     Will contain information about this file.
 */
void
AnnotationFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
}

/**
 * Private method for adding annotations to this file.
 *
 * In the GUI, annotations are added using the AnnotationRedoUndoCommand
 * which allows undo/redo operations.
 *
 * @param annotation
 *     Annotation that is added.
 * @param uniqueKey
 *     Unique key for the annotation.
 */
void
AnnotationFile::addAnnotationPrivate(Annotation* annotation,
                                     const int32_t uniqueKey)
{
    if (annotation->getType() == AnnotationTypeEnum::TEXT) {
        AnnotationPointSizeText* pointSizeAnnotation = dynamic_cast<AnnotationPointSizeText*>(annotation);
        if (pointSizeAnnotation != NULL) {
            CaretLogWarning("Point size text annotations are not supported in AnnotationFile.  "
                            "The annotation has been discarded.");
            delete annotation;
            return;
        }
    }

    CaretAssert(uniqueKey > 0);
    if (uniqueKey <= 0) {
        CaretLogSevere("invalid key less than zero.");
    }
    
    AnnotationGroup* group = getSpaceAnnotationGroup(annotation);
    CaretAssert(group);
    
    annotation->setUniqueKey(uniqueKey);
    
    group->addAnnotationPrivate(annotation);
    
    setModified();
}

void
AnnotationFile::addAnnotationPrivateSharedPointer(QSharedPointer<Annotation>& annotation,
                                                  const int32_t uniqueKey)
{
    if (annotation->getType() == AnnotationTypeEnum::TEXT) {
        AnnotationPointSizeText* pointSizeAnnotation = dynamic_cast<AnnotationPointSizeText*>(annotation.data());
        if (pointSizeAnnotation != NULL) {
            CaretLogWarning("Point size text annotations are not supported in AnnotationFile.  "
                            "The annotation has been discarded.");
            return;
        }
    }
    
    CaretAssert(uniqueKey > 0);
    if (uniqueKey <= 0) {
        CaretLogSevere("invalid key less than zero.");
    }
    
    AnnotationGroup* group = getSpaceAnnotationGroup(annotation.data());
    CaretAssert(group);
    
    annotation->setUniqueKey(uniqueKey);
    
    group->addAnnotationPrivateSharedPointer(annotation);
    
    setModified();
}

/**
 * Add an annotation to this file while the file is being read.
 * File will take ownership of the annotation.  
 *
 * In the GUI, annotations are added using the AnnotationRedoUndoCommand
 * which allows undo/redo operations.
 * 
 * @param annotation
 *     Annotation that is added.
 */
void
AnnotationFile::addAnnotationDuringFileVersionOneReading(Annotation* annotation)
{
    addAnnotationPrivate(annotation,
                         generateUniqueKey());
}

/**
 * Add a group while reading an annotation file.
 *
 * @param groupType
 *     Type of annotation group.
 * @param coordinateSpace
 *     Coordinate space of the group's annotaitons.
 * @param tabOrWindowIndex
 *     Tab or window index for groups in tab or window space.
 * @param uniqueKey
 *     Unique key for the annotation group.
 * @param annotations
 *     Annotation that are members of the group.
 * @throw DataFileException
 *     If there is an error.
 */
void
AnnotationFile::addAnnotationGroupDuringFileReading(const AnnotationGroupTypeEnum::Enum groupType,
                                                    const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                    const int32_t tabOrWindowIndex,
                                                    const int32_t uniqueKey,
                                                    const std::vector<Annotation*>& annotations)
{
    switch (groupType) {
        case AnnotationGroupTypeEnum::INVALID:
            throw DataFileException("INVALID group type is not allowed while annotation file.");
            break;
        case AnnotationGroupTypeEnum::SPACE:
            break;
        case AnnotationGroupTypeEnum::USER:
            break;
    }
    
    switch (coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            throw DataFileException("PIXELS coordinate space is not allowed for group while annotation file.");
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            throw DataFileException("VIEWPORT coordinate space is not allowed for group while annotation file.");
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
        case AnnotationCoordinateSpaceEnum::WINDOW:
            if ((tabOrWindowIndex < 0)
                || (tabOrWindowIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
                throw DataFileException("Invalid tab/window index for group while reading annotation file: "
                                        + QString::number(tabOrWindowIndex));
            }
            break;
    }
    
    if (uniqueKey <= 0) {
        throw DataFileException("Invalid unique key for group while reading annotation file: "
                                + AString::number(uniqueKey));
    }
    
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        QSharedPointer<AnnotationGroup> group = *groupIter;
        if (group->getUniqueKey() == uniqueKey) {
            throw DataFileException("More than one group using unique key "
                                    + AString::number(uniqueKey)
                                    + " while reading annotation file.");
        }

        /*
         * For default groups (NOT user groups), there may not be 
         * more than one group of each type.
         *
         * There is:
         *     - One group for stereotaxic
         *     - One group for surface
         *     - One group for each Tab
         *     - One group for each Window
         */
        if (group->getGroupType() == groupType) {
            if (groupType == AnnotationGroupTypeEnum::SPACE) {
                if (group->getCoordinateSpace() == coordinateSpace) {
                    switch (coordinateSpace) {
                        case AnnotationCoordinateSpaceEnum::PIXELS:
                            CaretAssert(0);
                            break;
                        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                        case AnnotationCoordinateSpaceEnum::SURFACE:
                            throw DataFileException("There is more than one annotation space group with space "
                                                    + AnnotationCoordinateSpaceEnum::toGuiName(coordinateSpace)
                                                    + ".  Only one space group for each space is allowed.");
                            break;
                        case AnnotationCoordinateSpaceEnum::TAB:
                            if (tabOrWindowIndex == group->getTabOrWindowIndex()) {
                                throw DataFileException("There is more than one annotation space group with space "
                                                        + AnnotationCoordinateSpaceEnum::toGuiName(coordinateSpace)
                                                        + " for tab "
                                                        + AString::number(tabOrWindowIndex)
                                                        + ".  Only one space group for each space is allowed.");
                            }
                            break;
                        case AnnotationCoordinateSpaceEnum::VIEWPORT:
                            CaretAssert(0);
                            break;
                        case AnnotationCoordinateSpaceEnum::WINDOW:
                            if (tabOrWindowIndex == group->getTabOrWindowIndex()) {
                                throw DataFileException("There is more than one annotation space group with space "
                                                        + AnnotationCoordinateSpaceEnum::toGuiName(coordinateSpace)
                                                        + " for window "
                                                        + AString::number(tabOrWindowIndex)
                                                        + ".  Only one space group for each space is allowed.");
                            }
                            break;
                    }
                }
            }
        }
    }
    
    AnnotationGroup* group = new AnnotationGroup(this,
                                                 groupType,
                                                 uniqueKey,
                                                 coordinateSpace,
                                                 tabOrWindowIndex);
    for (std::vector<Annotation*>::const_iterator annIter = annotations.begin();
         annIter != annotations.end();
         annIter++) {
        group->addAnnotationPrivate(*annIter);
    }
    
    group->setItemParent(this);
    
    m_annotationGroups.push_back(QSharedPointer<AnnotationGroup>(group));
}


/**
 * Restore an annotation that had been removed and possibly
 * add if it was not restored (probably creating a new
 * annotation).
 *
 * @param annotation
 *    Annotation that is restored.
 * @return
 *    True if the annotation was restored or added,
 *    otherwise false.
 */
bool
AnnotationFile::restoreAnnotationAddIfNotFound(Annotation* annotation)
{
    if (restoreAnnotation(annotation)) {
        return true;
    }
    
    addAnnotationPrivate(annotation,
                         generateUniqueKey());
    
    return true;
}


/**
 * Restore an annotation that had been removed.
 *
 * @param annotation
 *    Annotation that is restored.
 * @return
 *    True if the annotation was restored otherwise false.
 */
bool
AnnotationFile::restoreAnnotation(Annotation* annotation)
{
    for (std::set<QSharedPointer<Annotation> >::iterator iter = m_removedAnnotations.begin();
         iter != m_removedAnnotations.end();
         iter++) {
        QSharedPointer<Annotation> annotationPointer = *iter;
        if (annotationPointer.data() == annotation) {
            
            addAnnotationPrivateSharedPointer(annotationPointer,
                                              annotationPointer->getUniqueKey());
            
            m_removedAnnotations.erase(iter);
            setModified();
            
            /*
             * Successfully restored
             */
            return true;
        }
    }
    
    return false;
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
AnnotationFile::removeAnnotation(Annotation* annotation)
{
    
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        QSharedPointer<AnnotationGroup> group = *groupIter;
        QSharedPointer<Annotation> removedAnnotationPointer;
        if (group->removeAnnotation(annotation,
                                    removedAnnotationPointer)) {
            
            removedAnnotationPointer->invalidateAnnotationGroupKey();
            
            m_removedAnnotations.insert(removedAnnotationPointer);
            
            /*
             * Remove group if it is empty.
             */
            if (group->isEmpty()) {
                m_annotationGroups.erase(groupIter);
            }
            
            setModified();
            return true;
        }
    }
//    for (AnnotationIterator iter = m_annotations.begin();
//         iter != m_annotations.end();
//         iter++) {
//        QSharedPointer<Annotation>& annotationPointer = *iter;
//        if (annotationPointer == annotation) {
//            m_removedAnnotations.insert(annotationPointer);
//            
//            m_annotations.erase(iter);
//            
//            setModified();
//            
//            /*
//             * Successfully removed
//             */
//            return true;
//        }
//    }
    
    /*
     * Annotation not in this file
     */
    return false;
}

/**
 * Get all annotations in this file.
 * 
 * @param annotationsOut
 *    Output containing all annotations.
 */
void
AnnotationFile::getAllAnnotations(std::vector<Annotation*>& annotationsOut) const
{
    annotationsOut.clear();

    for (AnnotationGroupConstIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        std::vector<Annotation*> groupAnnotations;
        (*groupIter)->getAllAnnotations(groupAnnotations);
        annotationsOut.insert(annotationsOut.end(),
                              groupAnnotations.begin(),
                              groupAnnotations.end());
    }
}

/**
 * Get all annotation groups in this file.
 *
 * @param annotationGroupsOut
 *    Output containing all annotation groups.
 */
void
AnnotationFile::getAllAnnotationGroups(std::vector<AnnotationGroup*>& annotationGroupsOut) const
{
    annotationGroupsOut.clear();
    
    for (AnnotationGroupConstIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        annotationGroupsOut.push_back((*groupIter).data());
    }
}

/**
 * Group annotations.
 * 
 * @param groupingEvent
 *     The grouping event.
 */
void
AnnotationFile::processGroupingAnnotations(EventAnnotationGrouping* groupingEvent)
{
    CaretAssert(groupingEvent);
    AnnotationGroupKey spaceGroupKey = groupingEvent->getAnnotationGroupKey();
    
    
    std::vector<Annotation*> annotationsToGroup = groupingEvent->getAnnotations();
    if (annotationsToGroup.size() < 2) {
        groupingEvent->setErrorMessage("PROGRAM ERROR: Trying to group annotations less than two annotations");
        CaretAssert(0);
        return;
    }
    
    AnnotationGroupIterator spaceGroupIter = m_annotationGroups.end();
    for (spaceGroupIter = m_annotationGroups.begin();
         spaceGroupIter != m_annotationGroups.end();
         spaceGroupIter++) {
        AnnotationGroup* group = (*spaceGroupIter).data();
        if (group->getAnnotationGroupKey().getGroupType() == AnnotationGroupTypeEnum::SPACE) {
            if (group->containsAllAnnotation(annotationsToGroup)) {
                break;
            }
        }
    }
    
    if (spaceGroupIter == m_annotationGroups.end()) {
        groupingEvent->setErrorMessage("Did not find annotations in a space group.  This may occur when "
                                       "annotations have been ungrouped, an annotation is modified that moves "
                                       "it to a different group or the annotation is deleted, and there "
                                       "is an attempt to regroup the annotations.");
        return;
    }
    
    AnnotationGroup* spaceGroup = (*spaceGroupIter).data();
    CaretAssert(spaceGroup);

    
    
    
    
//    AnnotationGroupKey spaceGroupKey = groupingEvent->getAnnotationGroupKey();
//    
//    AnnotationGroup* spaceGroup = NULL;
//    AnnotationGroupIterator spaceGroupIter = m_annotationGroups.end();
//    for (spaceGroupIter = m_annotationGroups.begin();
//         spaceGroupIter != m_annotationGroups.end();
//         spaceGroupIter++) {
//        QSharedPointer<AnnotationGroup> groupPointer = *spaceGroupIter;
//        if (spaceGroupKey == groupPointer->getAnnotationGroupKey()) {
//            spaceGroup = groupPointer.data();
//            break;
//        }
//    }
//    
//    if (spaceGroup == NULL) {
//        groupingEvent->setErrorMessage("PROGRAM ERROR: Did not find space group for source of grouping annotations");
//        return;
//    }

    
    groupingEvent->setEventProcessed();
    
    if (spaceGroup->getGroupType() != AnnotationGroupTypeEnum::SPACE) {
        groupingEvent->setErrorMessage("PROGRAM ERROR: Trying to group annotations in a NON-space group");
        CaretAssert(0);
        return;
    }
    
    
    
    bool allValidFlag = true;
    std::vector<QSharedPointer<Annotation> > movedAnnotations;
    for (std::vector<Annotation*>::iterator annIter = annotationsToGroup.begin();
         annIter != annotationsToGroup.end();
         annIter++) {
        QSharedPointer<Annotation> annPtr;
        if (spaceGroup->removeAnnotation(*annIter, annPtr)) {
            movedAnnotations.push_back(annPtr);
        }
        else {
            allValidFlag = false;
            break;
        }
    }
    
    if ( ! allValidFlag) {
        for (std::vector<QSharedPointer<Annotation> >::iterator annPtrIter = movedAnnotations.begin();
             annPtrIter != movedAnnotations.end();
             annPtrIter++) {
            spaceGroup->addAnnotationPrivateSharedPointer(*annPtrIter);
        }
        
        groupingEvent->setErrorMessage("PROGRAM ERROR: Failed to remove an anntotation from its space group.");
        return;
    }
    
    AnnotationGroup* group = new AnnotationGroup(this,
                                                 AnnotationGroupTypeEnum::USER,
                                                 generateUniqueKey(),
                                                 spaceGroup->getCoordinateSpace(),
                                                 spaceGroup->getTabOrWindowIndex());
    
    for (std::vector<QSharedPointer<Annotation> >::iterator annPtrIter = movedAnnotations.begin();
         annPtrIter != movedAnnotations.end();
         annPtrIter++) {
        group->addAnnotationPrivateSharedPointer(*annPtrIter);
    }
    group->setItemParent(this);
    groupingEvent->setGroupKeyToWhichAnnotationsWereMoved(group->getAnnotationGroupKey());
    
    /*
     * If space group becomes empty, remove it.
     * Need to remove it before adding new group, otherwise
     * iterator will become invalid
     */
    if (spaceGroup->isEmpty()) {
        m_annotationGroups.erase(spaceGroupIter);
    }
    
    /*
     * Add new user group
     */
    m_annotationGroups.push_back(QSharedPointer<AnnotationGroup>(group));
    
    setModified();
}

/**
 * Group annotations.
 *
 * @param groupingEvent
 *     The grouping event.
 */
void
AnnotationFile::processUngroupingAnnotations(EventAnnotationGrouping* groupingEvent)
{
    CaretAssert(groupingEvent);
    
    AnnotationGroupKey userGroupKey = groupingEvent->getAnnotationGroupKey();
    
    AnnotationGroupIterator userGroupIter = m_annotationGroups.end();
    for (userGroupIter = m_annotationGroups.begin();
         userGroupIter != m_annotationGroups.end();
         userGroupIter++) {
        if (userGroupKey == (*userGroupIter)->getAnnotationGroupKey()) {
            break;
        }
    }

    if (userGroupIter == m_annotationGroups.end()) {
        groupingEvent->setErrorMessage("PROGRAM ERROR: Did not find group for ungrouping annotations");
        return;
    }
    
    AnnotationGroup* userGroup = (*userGroupIter).data();
    CaretAssert(userGroup);
    
    if (userGroup->getGroupType() != AnnotationGroupTypeEnum::USER) {
        groupingEvent->setErrorMessage("PROGRAM ERROR: Trying to ungroup annotations in a NON-user group");
        CaretAssert(0);
        return;
    }
    
    std::vector<QSharedPointer<Annotation> > allGroupAnnotations;
    userGroup->removeAllAnnotations(allGroupAnnotations);
    
    /**
     * Remove the group since it is empty.  Must be done before adding annotations back to
     * space group since the space group may not exist and if a new
     * space group is created the iterator will be invalid.
     */
    m_annotationGroups.erase(userGroupIter);
    
    for (std::vector<QSharedPointer<Annotation> >::iterator annIter = allGroupAnnotations.begin();
         annIter != allGroupAnnotations.end();
         annIter++) {
        QSharedPointer<Annotation> annPtr = *annIter;
        AnnotationGroup* spaceGroup = getSpaceAnnotationGroup(annPtr.data());
        spaceGroup->addAnnotationPrivateSharedPointer(annPtr);
    }
    
    groupingEvent->setEventProcessed();
    
    setModified();
}

/**
 * Group annotations.
 *
 * @param groupingEvent
 *     The grouping event.
 */
void
AnnotationFile::processRegroupingAnnotations(EventAnnotationGrouping* groupingEvent)
{
    CaretAssert(groupingEvent);
    
    /*
     * Unique key of group that annotations were once a member of
     */
    const int32_t userGroupUniqueKey = groupingEvent->getAnnotationGroupKey().getUserGroupUniqueKey();
    CaretAssert(userGroupUniqueKey > 0);
    
    std::vector<Annotation*> annotations;
    std::set<AnnotationGroup*> groups;
    
    /*
     * Find annotations in ONE space group that were
     * previously assigned to the previous user group.
     */
    //AnnotationGroupIterator spaceGroupIter = m_annotationGroups.end();
    for (AnnotationGroupIterator spaceGroupIter = m_annotationGroups.begin();
         spaceGroupIter != m_annotationGroups.end();
         spaceGroupIter++) {
        AnnotationGroup* group = (*spaceGroupIter).data();
        
        switch (group->getGroupType()) {
            case  AnnotationGroupTypeEnum::INVALID:
                break;
            case AnnotationGroupTypeEnum::SPACE:
            {
                std::vector<Annotation*> groupAnnotations;
                group->getAllAnnotations(groupAnnotations);
                
                for (std::vector<Annotation*>::iterator annIter = groupAnnotations.begin();
                     annIter != groupAnnotations.end();
                     annIter++) {
                    Annotation* ann = *annIter;
                    if (ann->getAnnotationGroupKey().getUserGroupUniqueKey() == userGroupUniqueKey) {
                        groups.insert(group);
                        annotations.push_back(ann);
                    }
                }
            }
                break;
            case AnnotationGroupTypeEnum::USER:
                break;
        }
    }

    const int32_t windowIndex = groupingEvent->getWindowIndex();
    
    /*
     * Annotations must be in one space group (same space!)
     */
    if (groups.size() == 1) {
        AnnotationGroup* spaceGroup = *(groups.begin());
        const int32_t numAnn = static_cast<int32_t>(annotations.size());
        if (numAnn > 1) {
            AnnotationGroup* group = new AnnotationGroup(this,
                                                         AnnotationGroupTypeEnum::USER,
                                                         reuseUniqueKeyOrGenerateNewUniqueKey(userGroupUniqueKey),
                                                         spaceGroup->getCoordinateSpace(),
                                                         spaceGroup->getTabOrWindowIndex());
            
            bool allValidFlag = true;
            std::vector<QSharedPointer<Annotation> > movedAnnotations;
            for (std::vector<Annotation*>::iterator annIter = annotations.begin();
                 annIter != annotations.end();
                 annIter++) {
                QSharedPointer<Annotation> annPtr;
                if (spaceGroup->removeAnnotation(*annIter, annPtr)) {
                    movedAnnotations.push_back(annPtr);
                }
                else {
                    allValidFlag = false;
                    break;
                }
            }
            
            for (std::vector<QSharedPointer<Annotation> >::iterator annPtrIter = movedAnnotations.begin();
                 annPtrIter != movedAnnotations.end();
                 annPtrIter++) {
                if ((windowIndex >= 0)
                    && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)) {
                    (*annPtrIter)->setSelectedForEditing(windowIndex,
                                               true);
                }
                group->addAnnotationPrivateSharedPointer(*annPtrIter);
            }
            group->setItemParent(this);
            
            m_annotationGroups.push_back(QSharedPointer<AnnotationGroup>(group));
            
            /*
             * If the space group is empty it is no longer needed
             * so delete it.
             */
            if (spaceGroup->isEmpty()) {
                for (AnnotationGroupIterator spaceGroupIter = m_annotationGroups.begin();
                     spaceGroupIter != m_annotationGroups.end();
                     spaceGroupIter++) {
                    if (spaceGroup == (*spaceGroupIter).data()) {
                        m_annotationGroups.erase(spaceGroupIter);
                        break;
                    }
                }
            }
            
            groupingEvent->setGroupKeyToWhichAnnotationsWereMoved(group->getAnnotationGroupKey());
            
            setModified();
        }
        else {
            groupingEvent->setErrorMessage("ERROR: Unable to regroup annotations due to only one annotation for regrouping.");
        }
    }
    else {
        groupingEvent->setErrorMessage("ERROR: Annotations that were in a previous group are no longer "
                                       "in the same space group.");
    }
}

/**
 * Clear drawn in window status for all annotations.
 */
void
AnnotationFile::clearAllAnnotationsDrawnInWindowStatus()
{
    std::vector<Annotation*> allAnnotations;
    getAllAnnotations(allAnnotations);
    
    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
         annIter != allAnnotations.end();
         annIter++) {
        (*annIter)->clearDrawnInWindowStatusForAllWindows();
    }
}

/**
 * Get annotations drawin in the given window.
 *
 * @param windowIndex
 *     Index of the window.
 * @param annotationsOut
 *     Output containing annotations with draw in window status set for
 *     the given window index.
 */
void
AnnotationFile::getAllAnnotationWithDrawnInWindowStatusSet(const int32_t windowIndex,
                                                           std::vector<Annotation*>& annotationsOut)
{
    std::vector<Annotation*> allAnnotations;
    getAllAnnotations(allAnnotations);
    
    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
         annIter != allAnnotations.end();
         annIter++) {
        if ((*annIter)->isDrawnInWindowStatus(windowIndex)) {
            annotationsOut.push_back(*annIter);
        }
    }
}

/**
 * @return true if file is modified, else false.
 */
bool
AnnotationFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    
    if (m_metadata->isModified()) {
        return true;
    }
    
    for (AnnotationGroupConstIterator iter = m_annotationGroups.begin();
         iter != m_annotationGroups.end();
         iter++) {
        if ((*iter)->isModified()) {
            return true;
        }
    }

    return false;
}

/**
 * Clear the modified status of this file.
 */
void
AnnotationFile::clearModified()
{
    CaretDataFile::clearModified();
    
    m_metadata->clearModified();
    
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        (*groupIter)->clearModified();
    }
}

/**
 * Reuse the given unique key (if it is not used in a group or annotation
 * and is less than the current value of the unique key generator.  Otherwise,
 * generate a new unique key.
 *
 * @param reuseUniqueKey
 *     Key that is requested and tested to see if it can be used.
 * @return
 *     The input key if it can be reused, otherwise, a new key.
 */
int32_t
AnnotationFile::reuseUniqueKeyOrGenerateNewUniqueKey(const int32_t reuseUniqueKey)
{
    
    bool canBeReusedFlag = true;
    
    /*
     * Search the groups and the annotations within the groups to
     * see if the desired unique key is already used.
     */
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        AnnotationGroup* group = (*groupIter).data();
        if (group->getUniqueKey() == reuseUniqueKey) {
            canBeReusedFlag = false;
            break;
        }
        else {
            std::vector<Annotation*> groupAnnotations;
            group->getAllAnnotations(groupAnnotations);
            
            for (std::vector<Annotation*>::iterator annIter = groupAnnotations.begin();
                 annIter != groupAnnotations.end();
                 annIter++) {
                if ((*annIter)->getUniqueKey() == reuseUniqueKey) {
                    canBeReusedFlag = false;
                    break;
                }
            }
        }
    }
    
    /*
     * Check annotations that were deleted since they can be undeleted
     */
    if (canBeReusedFlag) {
        for (std::set<QSharedPointer<Annotation> >::iterator removedAnnIter = m_removedAnnotations.begin();
             removedAnnIter != m_removedAnnotations.end();
             removedAnnIter++) {
            if ((*removedAnnIter)->getUniqueKey() == reuseUniqueKey) {
                canBeReusedFlag = false;
            }
        }
    }
    
    int32_t outputUniqueKey = -1;
    
    if (canBeReusedFlag) {
        outputUniqueKey = reuseUniqueKey;
    }
    else {
        outputUniqueKey = generateUniqueKey();
    }
    
    return outputUniqueKey;
}


/**
 * @return A new unique key (each annotation and
 * annotation group in the file contains a unique key.
 */
int32_t
AnnotationFile::generateUniqueKey()
{
    if (m_uniqueKeyGenerator < 0) {
        m_uniqueKeyGenerator = 0;
    }
    
    m_uniqueKeyGenerator++;
    
    return m_uniqueKeyGenerator;
}

/**
 * Update the unqiue keys after reading the file
 * Older files did not have unique keys
 */
void
AnnotationFile::updateUniqueKeysAfterReadingFile()
{

    /*
     * Find maximum unique identifier from annotations AND annotation groups
     */
    int32_t maximumKeyFound = 0;
    
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        maximumKeyFound = std::max(maximumKeyFound,
                                   (*groupIter)->getMaximumUniqueKey());
    }
    
    m_uniqueKeyGenerator = maximumKeyFound;
}

/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
AnnotationFile::readFile(const AString& filename)
{
    clear();
    
    checkFileReadability(filename);
    
    AnnotationFileXmlReader reader;
    reader.readFile(filename,
                    this);

    updateUniqueKeysAfterReadingFile();
    
    setFileName(filename);
    
    clearModified();
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
AnnotationFile::writeFile(const AString& filename)
{
    if (!(filename.endsWith(".annot") || filename.endsWith(".wb_annot")))
    {
        CaretLogWarning("annotation file '" + filename + "' should be saved ending in .annot");
    }
    checkFileWritability(filename);
    
    setFileName(filename);

    AnnotationFileXmlWriter writer;
    writer.writeFile(this);
    
    clearModified();
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
AnnotationFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                    SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    switch (m_fileSubType) {
        case ANNOTATION_FILE_SAVE_TO_FILE:
            break;
        case ANNOTATION_FILE_SAVE_TO_SCENE:
            if ( ! isEmpty()) {
                try {
                    AnnotationFileXmlWriter writer;
                    QString fileContentInString;
                    writer.writeFileToString(this,
                                             fileContentInString);
                    sceneClass->addString("AnnotationFileContent",
                                          fileContentInString);
                }
                catch (const DataFileException& dfe) {
                    sceneAttributes->addToErrorMessage(dfe.whatString());
                }
            }
            break;
    }
    
    /*
     * Save groups to scene
     */
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        AnnotationGroup* group = (*groupIter).data();
        const int32_t uniqueKey = group->getUniqueKey();
        SceneClass* groupClass = group->saveToScene(sceneAttributes,
                           AnnotationGroup::getSceneClassNameForAnnotationUniqueKey(uniqueKey));
        sceneClass->addClass(groupClass);
    }
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
AnnotationFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                         const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    switch (m_fileSubType) {
        case ANNOTATION_FILE_SAVE_TO_FILE:
            break;
        case ANNOTATION_FILE_SAVE_TO_SCENE:
            QString fileContentInString = sceneClass->getStringValue("AnnotationFileContent");
            if ( ! fileContentInString.isEmpty()) {
                try {
                    AnnotationFileXmlReader reader;
                    reader.readFileFromString(fileContentInString,
                                              this);
                    updateUniqueKeysAfterReadingFile();
                    clearModified();
                }
                catch (const DataFileException& dfe) {
                    sceneAttributes->addToErrorMessage(dfe.whatString());
                }
            }
            break;
    }
    
    /*
     * Restore groups from scene
     */
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        AnnotationGroup* group = (*groupIter).data();
        const int32_t uniqueKey = group->getUniqueKey();
        const SceneClass* annClass = sceneClass->getClass(AnnotationGroup::getSceneClassNameForAnnotationUniqueKey(uniqueKey));
        if (sceneClass != NULL) {
            group->restoreFromScene(sceneAttributes,
                                               annClass);
        }
    }
}

/**
 * @return Pointer to DataFile that implements this interface
 */
DataFile*
AnnotationFile::getAsDataFile()
{
    DataFile* dataFile = dynamic_cast<DataFile*>(this);
    CaretAssert(this);
    return dataFile;
}

/**
 * Append content from the given data file copy/move interface to this instance
 *
 * @param copyMoveParameters
 *     Parameters used for copy/move.
 * @throws DataFileException
 *     If there is an error.
 */
void
AnnotationFile::appendContentFromDataFile(const DataFileContentCopyMoveParameters& copyMoveParameters)
{
    const AnnotationFile* copyFromFile = dynamic_cast<const AnnotationFile*>(copyMoveParameters.getDataFileCopyMoveInterfaceToCopy());
    if (copyFromFile == NULL) {
        throw DataFileException("Trying to copy content to annotation file from a file that is not an "
                                "annotation file.");
    }
    
    const bool selectedAnnotationsFlag = copyMoveParameters.isOptionSelectedItems();
    const int32_t windowIndex = copyMoveParameters.getWindowIndex();
    if (selectedAnnotationsFlag) {
        if (windowIndex < 0) {
            throw DataFileException("Requested copying of selected annotations but window index is invalid.");
        }
    }
    
    std::vector<AnnotationGroup*> annotationGroups;
    copyFromFile->getAllAnnotationGroups(annotationGroups);
    
    for (std::vector<AnnotationGroup*>::iterator groupIter = annotationGroups.begin();
         groupIter != annotationGroups.end();
         groupIter++) {
        const AnnotationGroup* groupToCopy = *groupIter;
        CaretAssert(groupToCopy);

        /*
         * Find that annotations that are to be copied.
         */
        const int32_t numAnnInGroup = groupToCopy->getNumberOfAnnotations();
        std::vector<const Annotation*> annotationsToCopy;
        for (int32_t ia = 0; ia < numAnnInGroup; ia++) {
            const Annotation* annToCopy = groupToCopy->getAnnotation(ia);
            CaretAssert(annToCopy);
            
            bool copyFlag = true;
            if (selectedAnnotationsFlag) {
                if (annToCopy->isSelectedForEditing(windowIndex)) {
                    /* Nothing */
                }
                else {
                    copyFlag = false;
                }
            }
            
            if (copyFlag) {
                annotationsToCopy.push_back(annToCopy);
            }
        }
        
        /*
         * If there are are annotations to copy,
         * clone them and add to the proper group.
         */
        if ( ! annotationsToCopy.empty()) {
            AnnotationGroup* group = NULL;
            switch (groupToCopy->getGroupType()) {
                case AnnotationGroupTypeEnum::INVALID:
                    break;
                case AnnotationGroupTypeEnum::SPACE:
                    /*
                     * All annotations in a group are in identical spaces
                     * so use the first annotation to find the space group.
                     */
                    group = getSpaceAnnotationGroup(groupToCopy->getAnnotation(0));
                    break;
                case AnnotationGroupTypeEnum::USER:
                    /*
                     * Create a new user group
                     */
                    group = new AnnotationGroup(this,
                                                AnnotationGroupTypeEnum::USER,
                                                generateUniqueKey(),
                                                groupToCopy->getCoordinateSpace(),
                                                groupToCopy->getTabOrWindowIndex());
                    group->setItemParent(this);
                    m_annotationGroups.push_back(QSharedPointer<AnnotationGroup>(group));
                    break;
            }
            
            CaretAssert(group);
            
            /*
             * Copy annotations and add them to the group.
             */
            for (std::vector<const Annotation*>::const_iterator iter = annotationsToCopy.begin();
                 iter != annotationsToCopy.end();
                 iter++) {
                const Annotation* annToCopy = *iter;
                Annotation* clonedAnn = annToCopy->clone();
                clonedAnn->setUniqueKey(generateUniqueKey());
                group->addAnnotationPrivate(clonedAnn);
            }
        }
    }
}

/**
 * @return A new instance of the same file type.  File is empty.
 */
DataFileContentCopyMoveInterface*
AnnotationFile::newInstanceOfDataFile() const
{
    return new AnnotationFile();
}

/**
 * @return Number of children.
 */
int32_t
AnnotationFile::getNumberOfItemChildren() const
{
    return m_annotationGroups.size();
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
AnnotationFile::getItemChild(const int32_t index) const
{
    CaretAssertVectorIndex(m_annotationGroups,
                           index);
    return m_annotationGroups[index].data();
}

/**
 * @return Children of this item.
 */
std::vector<DisplayGroupAndTabItemInterface*>
AnnotationFile::getItemChildren() const
{
    std::vector<DisplayGroupAndTabItemInterface*> children;
    
    for (AnnotationGroupConstIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        children.push_back((*groupIter).data());
    }
    
    return children;
}


/**
 * @return Parent of this item.
 */
DisplayGroupAndTabItemInterface*
AnnotationFile::getItemParent() const
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
AnnotationFile::setItemParent(DisplayGroupAndTabItemInterface* itemParent)
{
    m_displayGroupAndTabItemHelper->setParent(itemParent);
}

/**
 * @return Name of this item.
 */
AString
AnnotationFile::getItemName() const
{
    return getFileNameNoPath();
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
AnnotationFile::getItemIconColorsRGBA(float backgroundRgbaOut[4],
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
AnnotationFile::isItemExpandable() const
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
AnnotationFile::isItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                               const int32_t tabIndex) const
{
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
AnnotationFile::setItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex,
                                const bool status)
{
    m_displayGroupAndTabItemHelper->setExpanded(displayGroup,
                                                tabIndex,
                                                status);
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
AnnotationFile::getItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex) const
{
    TriStateSelectionStatusEnum::Enum status = TriStateSelectionStatusEnum::UNSELECTED;
    
    const int numChildren = static_cast<int32_t>(m_annotationGroups.size());
    if (numChildren > 0) {
        int32_t selectedCount = 0;
        int32_t partialSelectedCount = 0;
        for (int32_t i = 0; i < numChildren; i++) {
            CaretAssertVectorIndex(m_annotationGroups, i);
            switch (m_annotationGroups[i]->getItemDisplaySelected(displayGroup,
                                                           tabIndex)) {
                case TriStateSelectionStatusEnum::PARTIALLY_SELECTED:
                    partialSelectedCount++;
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
        else if ((selectedCount > 0)
                 || (partialSelectedCount > 0)) {
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
AnnotationFile::setItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex,
                                const TriStateSelectionStatusEnum::Enum status)
{
//    m_displayGroupAndTabItemHelper->setSelected(displayGroup,
//                                                tabIndex,
//                                                status);
    /*
     * Note: An annotation file's selection status is based
     * of the the file's annotation groups so we do not need to set
     * an explicit selection status for the file.
     */
    
    DisplayGroupAndTabItemInterface::setChildrenDisplaySelectedHelper(this,
                                                                      displayGroup,
                                                                      tabIndex,
                                                                      status);
    DisplayGroupAndTabItemInterface::setChildrenDisplaySelectedHelper(this,
                                                                displayGroup,
                                                                tabIndex,
                                                                status);
}

/**
 * Is this item selected for editing in the given window?
 *
 * @param windowIndex
 *     Index of the window.
 * @return
 *     Selection status.
 */
bool
AnnotationFile::isItemSelectedForEditingInWindow(const int32_t /*windowIndex*/)
{
    /*
     * The annotation file is never selected for editing.
     */
    return false;
}

