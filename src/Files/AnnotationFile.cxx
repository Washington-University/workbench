
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
#include "AnnotationText.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "DataFileException.h"
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
    
    const bool addExampleDataFlag = false;
    if (addExampleDataFlag) {
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
            at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
            at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
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
            at->setForegroundLineWidth(3.0);
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
            AnnotationOval* at = new AnnotationOval();
            AnnotationCoordinate* coord = at->getCoordinate();
            at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
            at->setWindowIndex(0);
            coord->setXYZ(0.5, 0.2, 0);
            at->setForegroundColor(CaretColorEnum::BLUE);
            at->setWidth(0.05);
            at->setHeight(0.1);
            at->setRotationAngle(30.0);
            at->setForegroundLineWidth(3.0);
            addAnnotation(at);
        }
        
        {
            AnnotationText* at = new AnnotationText();
            AnnotationCoordinate* coord = at->getCoordinate();
            at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
            coord->setSurfaceSpace(StructureEnum::CORTEX_RIGHT, 32492, 7883, 20);
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
    }
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

    for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        delete *iter;
    }
    m_annotations.clear();
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
: CaretDataFile(obj),
EventListenerInterface()
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
AnnotationFile::receiveEvent(Event* /*event*/)
{
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
 * Remove and destroy the given annotation from this file.
 * If the annotation is not found in this file,
 * no action is taken (returns false).  Since the annotation will
 * be destroyed NEVER use the pointer passed to this
 * method after this method completes (if true returned).
 *
 * @param annotation
 *     Annotation for deletion.
 * @return
 *     True if the annotation was in this file and was 
 *     was deleted, else false.
 */
bool
AnnotationFile::removeAnnotation(const Annotation* annotation)
{
    for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        Annotation* annotationPointer = *iter;
        if (annotationPointer == annotation) {
            m_annotations.erase(iter);
            delete annotationPointer;
            
            setModified();
            
            return true;
        }
    }
    
    /*
     * Annotation was not in this file.
     */
    return false;
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
    return m_annotations[index];
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
    return m_annotations[index];
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
    
    for (std::vector<Annotation*>::const_iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        const Annotation* annotationPointer = *iter;
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
    
    for (std::vector<Annotation*>::const_iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        Annotation* annotationPointer = *iter;
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
    
//    if ( ! isEmpty()) {
//        try {
//        AnnotationFileXmlWriter writer;
//        QString fileContentInString;
//        writer.writeFileToString(this,
//                                 fileContentInString);
//        sceneClass->addString("AnnotationFileContent",
//                              fileContentInString);
//        }
//        catch (const DataFileException& dfe) {
//            sceneAttributes->addToErrorMessage(dfe.whatString());
//        }
//    }
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
    
//    QString fileContentInString = sceneClass->getStringValue("AnnotationFileContent");
//    if ( ! fileContentInString.isEmpty()) {
//        try {
//            AnnotationFileXmlReader reader;
//            reader.readFileFromString(fileContentInString,
//                                      this);
//            
//            clearModified();
//        }
//        catch (const DataFileException& dfe) {
//            sceneAttributes->addToErrorMessage(dfe.whatString());
//        }
//    }
}

