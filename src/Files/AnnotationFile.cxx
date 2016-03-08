
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
#include "DataFileException.h"
#include "EventAnnotationAddToRemoveFromFile.h"
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
DataFileContentCopyMoveInterface(),
m_fileSubType(ANNOTATION_FILE_SAVE_TO_FILE)
{
    initializeAnnotationFile();
    
    const bool addExampleDataFlag = false;
    if (addExampleDataFlag) {
        createExampleAnnotations();
    }
}

/**
 * Create example annotations for testing.
 */
void
AnnotationFile::createExampleAnnotations()
{
    {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
        at->setTabIndex(3);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        coord->setXYZ(0.4, 0.2, 0);
        at->setLineColor(CaretColorEnum::WHITE);
        at->setText("TAB-4-LEFT-MIDDLE,0.4, 0.2");
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        coord->setXYZ(0.5, 0.9, 0);
        at->setLineColor(CaretColorEnum::BLUE);
        at->setBackgroundColor(CaretColorEnum::GRAY);
        at->setFontPercentViewportSize(0.10);
        at->setBoldStyleEnabled(true);
        at->setText("WINDOW-CENTER-MIDDLE,0.5,0.9");
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
        coord->setXYZ(0.1, 0.6, 0);
        at->setRotationAngle(30.0);
        at->setLineColor(CaretColorEnum::FUCHSIA);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontPercentViewportSize(0.08);
        at->setBoldStyleEnabled(true);
        at->setText("Rotated");
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        at->setOrientation(AnnotationTextOrientationEnum::STACKED);
        coord->setXYZ(0.9, 0.5, 0);
        at->setLineColor(CaretColorEnum::PURPLE);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontPercentViewportSize(0.15);
        at->setBoldStyleEnabled(false);
        at->setText("STACKED-VERT-0.9-0.5");
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        at->setOrientation(AnnotationTextOrientationEnum::STACKED);
        coord->setXYZ(0.7, 0.7, 0);
        at->setLineColor(CaretColorEnum::TEAL);
        at->setBackgroundColor(CaretColorEnum::SILVER);
        at->setFontPercentViewportSize(0.15);
        at->setBoldStyleEnabled(false);
        at->setText("STACK-ROT");
        at->setRotationAngle(-90.0);
        addAnnotationPrivate(at);
    }
    {
        AnnotationBox* at = new AnnotationBox(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        coord->setXYZ(0.5, 0.5, 0);
        at->setLineColor(CaretColorEnum::GREEN);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setWidth(0.2);
        at->setHeight(0.10);
        at->setRotationAngle(-20.0);
        at->setLineWidth(3.0);
        addAnnotationPrivate(at);
    }
    {
        AnnotationLine* at = new AnnotationLine(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* startCoord = at->getStartCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        startCoord->setXYZ(0.1, 0.1, 0);
        AnnotationCoordinate* endCoord = at->getEndCoordinate();
        endCoord->setXYZ(0.2, 0.3, 0);
        at->setLineColor(CaretColorEnum::RED);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationOval* at = new AnnotationOval(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        coord->setXYZ(0.5, 0.2, 0);
        at->setLineColor(CaretColorEnum::BLUE);
        at->setWidth(0.05);
        at->setHeight(0.1);
        at->setRotationAngle(30.0);
        at->setLineWidth(3.0);
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
        coord->setSurfaceSpace(StructureEnum::CORTEX_RIGHT, 32492, 7883, 20, AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX);
        at->setLineColor(CaretColorEnum::PURPLE);
        at->setText("Vertex 7883");
        addAnnotationPrivate(at);
    }
    
    for (int32_t iTab = 0; iTab < 10; iTab++) {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
        at->setTabIndex(iTab);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
        at->setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
        coord->setXYZ(0.95, 0.05, 0);
        at->setLineColor(CaretColorEnum::TEAL);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontPercentViewportSize(0.12);
        at->setBoldStyleEnabled(true);
        at->setText(AString::number(iTab + 1));
        addAnnotationPrivate(at);
    }
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
 * Set the selection status of all annotations.
 *
 * @param windowIndex
 *     Index of window for annotation selection.
 * @param selectedStatus
 *     New selection status for all annotations.
 */
void
AnnotationFile::setAllAnnotationsSelected(const int32_t windowIndex,
                                          const bool selectedStatus)
{
    std::vector<Annotation*> allAnnotations;
    getAllAnnotations(allAnnotations);
    for (std::vector<Annotation*>::iterator iter = allAnnotations.begin();
         iter != allAnnotations.end();
         iter++) {
        (*iter)->setSelected(windowIndex, selectedStatus);
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
    m_sceneAssistant = new SceneClassAssistant();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE);
}

/**
 * Get the coordinate space annotation group with the given space and for tab/window
 * spaces, the tab/window index.  If the group does not exist, it will be created.
 *
 * @param coordinateSpace
 *    Space for the group.
 * @param tabOrWindowIndex
 *    Index of tab/window for tab/window coordinateSpaces.
 * @return
 *    Group for the coordinateSpace.
 */
AnnotationGroup*
AnnotationFile::getSpaceAnnotationGroup(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                        const int32_t tabOrWindowIndex)
{
    for (AnnotationGroupIterator groupIter = m_annotationGroups.begin();
         groupIter != m_annotationGroups.end();
         groupIter++) {
        AnnotationGroup* group = (*groupIter).data();
        if (group->getCoordinateSpace() == coordinateSpace) {
            switch (coordinateSpace) {
                case AnnotationCoordinateSpaceEnum::PIXELS:
                    CaretAssert(0);
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    return group;
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                case AnnotationCoordinateSpaceEnum::WINDOW:
                    if (tabOrWindowIndex == group->getTabOrWindowIndex()) {
                        return group;
                    }
                    break;
            }
        }
    }
    
    switch (coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssert(0);
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            CaretAssert((tabOrWindowIndex >= 0)
                        && (tabOrWindowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS));
        case AnnotationCoordinateSpaceEnum::WINDOW:
            CaretAssert((tabOrWindowIndex >= 0)
                        && (tabOrWindowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS));
            break;
    }
    
    AnnotationGroup* group = new AnnotationGroup(this,
                                                 AnnotationGroupTypeEnum::SPACE,
                                                 coordinateSpace,
                                                 tabOrWindowIndex);
    group->setUniqueKey(generateUniqueKey());
    
    m_annotationGroups.push_back(QSharedPointer<AnnotationGroup>(group));
    
    return group;
}



/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
AnnotationFile::copyHelperAnnotationFile(const AnnotationFile& /* obj */)
{
    CaretAssertMessage(0, "Copying of annotation file not implemented.  "
                       "Will need to check subtype or have a 'clone' method' that each "
                       "subclass (AnnotationText) implements.");
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
 */
void
AnnotationFile::addAnnotationPrivate(Annotation* annotation)
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
    
    const AnnotationCoordinateSpaceEnum::Enum coordinateSpace = annotation->getCoordinateSpace();
    int32_t tabOrWindowIndex = -1;
    switch (coordinateSpace) {
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            tabOrWindowIndex = annotation->getTabIndex();
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            tabOrWindowIndex = annotation->getWindowIndex();
            break;
    }
    
    AnnotationGroup* group = getSpaceAnnotationGroup(annotation->getCoordinateSpace(),
                                                     tabOrWindowIndex);
    CaretAssert(group);
    
    annotation->setUniqueKey(generateUniqueKey());
    
    group->addAnnotationPrivate(annotation);
    
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
    addAnnotationPrivate(annotation);
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

        if (group->getGroupType() == groupType) {
            if (groupType == AnnotationGroupTypeEnum::SPACE) {
                if (group->getCoordinateSpace() == coordinateSpace) {
                    throw DataFileException("There is more than one annotation space group with space "
                                            + AnnotationCoordinateSpaceEnum::toGuiName(coordinateSpace)
                                            + ".  Only one space group for each space is allowed.");
                }
            }
        }
    }
    
    AnnotationGroup* group = new AnnotationGroup(this,
                                                 groupType,
                                                 coordinateSpace,
                                                 tabOrWindowIndex);
    group->setUniqueKey(uniqueKey);
    for (std::vector<Annotation*>::const_iterator annIter = annotations.begin();
         annIter != annotations.end();
         annIter++) {
        group->addAnnotationPrivate(*annIter);
    }
    
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
    
    addAnnotationPrivate(annotation);
    
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
        if (annotationPointer == annotation) {
            CaretAssertMessage(0, "Need to add annotation to group but unique key does not need to be set");
//            m_annotations.push_back(annotationPointer);
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
        if (group->removeAnnotation(annotation)) {
            CaretAssertMessage(0, "Need to move annotation from group to removed annotations");
//            m_removedAnnotations.insert(annotationPointer);
//            
//            m_annotations.erase(iter);
            
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
                    
                    clearModified();
                }
                catch (const DataFileException& dfe) {
                    sceneAttributes->addToErrorMessage(dfe.whatString());
                }
            }
            break;
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
 * @param dataFileCopyMoveInterface
 *     From which content is copied.
 * @throws DataFileException
 *     If there is an error.
 */
void
AnnotationFile::appendContentFromDataFile(const DataFileContentCopyMoveInterface* dataFileCopyMoveInterface)
{
    const AnnotationFile* copyFromFile = dynamic_cast<const AnnotationFile*>(dataFileCopyMoveInterface);
    if (copyFromFile == NULL) {
        throw DataFileException("Trying to copy content to annotation file from a file that is not an "
                                "annotation file.");
    }
    
    CaretAssertMessage(0, "Need to copy annotation groups.  Should be able to copy groups with their unique keys since new file");
//    for (AnnotationIterator annIter = m_annotations.begin();
//         annIter != m_annotations.end();
//         annIter++) {
//        const Annotation* ann = (*annIter).data();
//        CaretAssert(ann);
//        Annotation* annCopy = ann->clone();
//        CaretAssert(annCopy);
//        addAnnotationPrivate(annCopy);
//    }
}

/**
 * @return A new instance of the same file type.  File is empty.
 */
DataFileContentCopyMoveInterface*
AnnotationFile::newInstanceOfDataFile() const
{
    return new AnnotationFile();
}

