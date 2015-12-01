
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
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
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
        at->setForegroundColor(CaretColorEnum::WHITE);
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
        at->setForegroundColor(CaretColorEnum::BLUE);
        at->setBackgroundColor(CaretColorEnum::GRAY);
        at->setFontPercentViewportSize(0.10);
        at->setBoldEnabled(true);
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
        at->setForegroundColor(CaretColorEnum::FUCHSIA);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontPercentViewportSize(0.08);
        at->setBoldEnabled(true);
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
        at->setForegroundColor(CaretColorEnum::PURPLE);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontPercentViewportSize(0.15);
        at->setBoldEnabled(false);
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
        at->setForegroundColor(CaretColorEnum::TEAL);
        at->setBackgroundColor(CaretColorEnum::SILVER);
        at->setFontPercentViewportSize(0.15);
        at->setBoldEnabled(false);
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
        at->setForegroundColor(CaretColorEnum::GREEN);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setWidth(0.2);
        at->setHeight(0.10);
        at->setRotationAngle(-20.0);
        at->setForegroundLineWidth(3.0);
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
        at->setForegroundColor(CaretColorEnum::RED);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationOval* at = new AnnotationOval(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        coord->setXYZ(0.5, 0.2, 0);
        at->setForegroundColor(CaretColorEnum::BLUE);
        at->setWidth(0.05);
        at->setHeight(0.1);
        at->setRotationAngle(30.0);
        at->setForegroundLineWidth(3.0);
        addAnnotationPrivate(at);
    }
    
    {
        AnnotationPercentSizeText* at = new AnnotationPercentSizeText(AnnotationAttributesDefaultTypeEnum::NORMAL);
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
        coord->setSurfaceSpace(StructureEnum::CORTEX_RIGHT, 32492, 7883, 20);
        at->setForegroundColor(CaretColorEnum::PURPLE);
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
        at->setForegroundColor(CaretColorEnum::TEAL);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontPercentViewportSize(0.12);
        at->setBoldEnabled(true);
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
    CaretDataFile::clear();
    
    clearPrivate();
}

/**
 * Clear the content of this file.
 */
void
AnnotationFile::clearPrivate()
{
    m_metadata->clear();

    m_annotations.clear();
    m_removedAnnotations.clear();
}


/**
 * Set the selection status of all annotations.
 *
 * @param selectedStatus
 *     New selection status for all annotations.
 */
void
AnnotationFile::setAllAnnotationsSelected(const bool selectedStatus)
{
    for (AnnotationIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        QSharedPointer<Annotation>& a = *iter;
        a->setSelected(selectedStatus);
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
    m_metadata.grabNew(new GiftiMetaData());
    m_sceneAssistant = new SceneClassAssistant();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE);
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
    return (m_annotations.empty());
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
    
    m_annotations.push_back(QSharedPointer<Annotation>(annotation));
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
AnnotationFile::addAnnotationDuringFileReading(Annotation* annotation)
{
    addAnnotationPrivate(annotation);
}

/**
 * @return True if the given annotation is in this file, else false.
 * 
 * @param annotation
 *     Annotation tested for inside this file.
 */
bool
AnnotationFile::containsAnnotation(const Annotation* annotation) const
{
    for (AnnotationConstIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        //        Annotation* annotationPointer = *iter;
        const QSharedPointer<Annotation>& annotationPointer = *iter;
        if (annotationPointer == annotation) {
            return true;
        }
    }
    
    return false;
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
            m_annotations.push_back(annotationPointer);
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
    for (AnnotationIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        QSharedPointer<Annotation>& annotationPointer = *iter;
        if (annotationPointer == annotation) {
            m_removedAnnotations.insert(annotationPointer);
            
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
 * @return All annotations in this file.
 */
//const std::vector<Annotation*>&
const std::vector<Annotation*>
AnnotationFile::getAllAnnotations() const
{
    std::vector<Annotation*> allAnnotations;
    
    for (AnnotationConstIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        //        Annotation* annotationPointer = *iter;
        const QSharedPointer<Annotation>& annotationPointer = *iter;
        allAnnotations.push_back(annotationPointer.data());
    }

    return allAnnotations;
}

/**
 * @return Number of annotations in the file.
 */
int32_t
AnnotationFile::getNumberOfAnnotations() const
{
    return m_annotations.size();
}

/**
 * Get the annotation at the given index.
 * 
 * @param index
 *     Index of the annotation.
 * @return
 *     Annotation at the given index.
 */
Annotation*
AnnotationFile::getAnnotation(const int32_t index)
{
    CaretAssertVectorIndex(m_annotations, index);
    return m_annotations[index].data();
}

/**
 * Get the annotation at the given index (const method).
 *
 * @param index
 *     Index of the annotation.
 * @return
 *     Annotation at the given index.
 */
const Annotation*
AnnotationFile::getAnnotation(const int32_t index) const
{
    CaretAssertVectorIndex(m_annotations, index);
    return m_annotations[index].data();
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
AnnotationFile::clearModified()
{
    CaretDataFile::clearModified();
    
    m_metadata->clearModified();
    
    for (AnnotationConstIterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        const QSharedPointer<Annotation>& annotationPointer = *iter;
        annotationPointer->clearModified();
    }
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

