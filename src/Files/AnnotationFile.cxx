
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

#include "AnnotationArrow.h"
#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "EventAnnotation.h"
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
 * Constructor.
 */
AnnotationFile::AnnotationFile()
: CaretDataFile(DataFileTypeEnum::ANNOTATION)
{
    m_metadata.grabNew(new GiftiMetaData());
    m_sceneAssistant = new SceneClassAssistant();
    
    {
        AnnotationText* at = new AnnotationText();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
        at->setTabIndex(3);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        coord->setXYZ(0.4, 0.2, 0);
        at->setForegroundColor(CaretColorEnum::WHITE);
        at->setText("TAB-4-LEFT-MIDDLE,0.4, 0.2");
        addAnnotation(at);
    }
    
    {
        AnnotationText* at = new AnnotationText();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        coord->setXYZ(0.5, 0.9, 0);
        at->setForegroundColor(CaretColorEnum::BLUE);
        at->setBackgroundColor(CaretColorEnum::GRAY);
        at->setFontSize(AnnotationFontSizeEnum::SIZE20);
        at->setBoldEnabled(true);
        at->setText("WINDOW-CENTER-MIDDLE,0.5,0.9");
        addAnnotation(at);
    }
    
    {
        AnnotationText* at = new AnnotationText();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
        coord->setXYZ(0.1, 0.6, 0);
        at->setRotationAngle(30.0);
        at->setForegroundColor(CaretColorEnum::FUCHSIA);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontSize(AnnotationFontSizeEnum::SIZE16);
        at->setBoldEnabled(true);
        at->setText("Rotated");
        addAnnotation(at);
    }
    
    {
        AnnotationText* at = new AnnotationText();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        at->setOrientation(AnnotationTextOrientationEnum::STACKED);
        coord->setXYZ(0.9, 0.5, 0);
        at->setForegroundColor(CaretColorEnum::PURPLE);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontSize(AnnotationFontSizeEnum::SIZE24);
        at->setBoldEnabled(false);
        at->setText("STACKED-VERT-0.9-0.5");
        addAnnotation(at);
    }
    
    {
        AnnotationText* at = new AnnotationText();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
        at->setOrientation(AnnotationTextOrientationEnum::STACKED);
        coord->setXYZ(0.7, 0.7, 0);
        at->setForegroundColor(CaretColorEnum::TEAL);
        at->setBackgroundColor(CaretColorEnum::SILVER);
        at->setFontSize(AnnotationFontSizeEnum::SIZE24);
        at->setBoldEnabled(false);
        at->setText("STACK-ROT");
        at->setRotationAngle(-90.0);
        addAnnotation(at);
    }
    {
        AnnotationBox* at = new AnnotationBox();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        coord->setXYZ(0.5, 0.5, 0);
        at->setForegroundColor(CaretColorEnum::GREEN);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setWidth(0.2);
        at->setHeight(0.10);
        at->setRotationAngle(-20.0);
        at->setOutlineWidth(3.0);
        addAnnotation(at);
    }
    {
        AnnotationLine* at = new AnnotationLine();
        AnnotationCoordinate* startCoord = at->getStartCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        startCoord->setXYZ(0.1, 0.1, 0);
        AnnotationCoordinate* endCoord = at->getEndCoordinate();
        endCoord->setXYZ(0.2, 0.3, 0);
        at->setForegroundColor(CaretColorEnum::RED);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        addAnnotation(at);
    }

    {
        AnnotationArrow* at = new AnnotationArrow();
        AnnotationCoordinate* startCoord = at->getStartCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        startCoord->setXYZ(0.7, 0.1, 0);
        AnnotationCoordinate* endCoord = at->getEndCoordinate();
        endCoord->setXYZ(0.4, 0.3, 0);
        //endCoord->setXYZ(0.7, 0.4, 0);
        at->setForegroundColor(CaretColorEnum::GREEN);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setLineWidth(4.0);
        addAnnotation(at);
    }
    
    {
        AnnotationOval* at = new AnnotationOval();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setWindowIndex(0);
        coord->setXYZ(0.5, 0.2, 0);
        at->setForegroundColor(CaretColorEnum::BLUE);
        at->setWidth(0.05);
        at->setHeight(0.1);
        at->setRotationAngle(30.0);
        at->setOutlineWidth(3.0);
        addAnnotation(at);
    }
    
    {
        AnnotationText* at = new AnnotationText();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
        coord->setSurfaceSpace(StructureEnum::CORTEX_RIGHT, 32942, 7883);
        at->setForegroundColor(CaretColorEnum::PURPLE);
        at->setText("Vertex 7883");
        addAnnotation(at);
    }
    
    for (int32_t iTab = 0; iTab < 10; iTab++) {
        AnnotationText* at = new AnnotationText();
        AnnotationCoordinate* coord = at->getCoordinate();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
        at->setTabIndex(iTab);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::BOTTOM);
        at->setOrientation(AnnotationTextOrientationEnum::HORIZONTAL);
        coord->setXYZ(0.95, 0.05, 0);
        at->setForegroundColor(CaretColorEnum::TEAL);
        at->setBackgroundColor(CaretColorEnum::WHITE);
        at->setFontSize(AnnotationFontSizeEnum::SIZE18);
        at->setBoldEnabled(true);
        at->setText(AString::number(iTab + 1));
        addAnnotation(at);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION);
}

/**
 * Destructor.
 */
AnnotationFile::~AnnotationFile()
{
    for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        delete *iter;
    }
    m_annotations.clear();
    
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
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
    for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        Annotation* a = *iter;
        a->setSelected(selectedStatus);
    }
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationFile::AnnotationFile(const AnnotationFile& obj)
: CaretDataFile(obj)
{
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
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION) {
        EventAnnotation* annotationEvent = dynamic_cast<EventAnnotation*>(event);
        CaretAssert(annotationEvent);
        
        switch (annotationEvent->getMode()) {
            case EventAnnotation::MODE_INVALID:
                break;
            case EventAnnotation::MODE_ANNOTATION_EDIT:
                break;
            case EventAnnotation::MODE_DESELECT_ALL_ANNOTATIONS:
                setAllAnnotationsSelected(false);
                break;
        }
        
        annotationEvent->setEventProcessed();
    }
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
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
 * Add an annotation to this file.  File will take ownership
 * of the annotation.
 * 
 * @param annotation
 *     Annotation that is added.
 */
void
AnnotationFile::addAnnotation(Annotation* annotation)
{
    m_annotations.push_back(annotation);
    setModified();
}

/**
 * @return All annotations in this file.
 */
const std::vector<Annotation*>&
AnnotationFile::getAllAnnotations() const
{
    return m_annotations;
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
}

