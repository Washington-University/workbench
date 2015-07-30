
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __ANNOTATION_UNDO_COMMAND_DECLARE__
#include "AnnotationRedoUndoCommand.h"
#undef __ANNOTATION_UNDO_COMMAND_DECLARE__

#include "AnnotationLine.h"
#include "AnnotationText.h"
#include "AnnotationTwoDimensionalShape.h"
#include "EventAnnotationDeleteUndeleteFromFile.h"
#include "EventManager.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::AnnotationRedoUndoCommand 
 * \brief Command pattern for annotation modifications that undo and redo.
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
AnnotationRedoUndoCommand::AnnotationRedoUndoCommand()
: CaretUndoCommand()
{
    m_mode       = AnnotationRedoUndoCommandModeEnum::INVALID;
    m_sortedFlag = false;
}

/**
 * Destructor.
 */
AnnotationRedoUndoCommand::~AnnotationRedoUndoCommand()
{
    for (std::vector<AnnotationMemento*>::iterator iter = m_annotationMementos.begin();
         iter != m_annotationMementos.end();
         iter++) {
        delete *iter;
    }
}

/**
 * Apply a redo or undo command.
 *
 * @param annotation
 *     The annotation being redone or undone.
 * @param  annotationValue
 *     The redo or undo value for the annotation.
 */
void
AnnotationRedoUndoCommand::applyRedoOrUndo(Annotation* annotation,
                                           const Annotation* annotationValue) const
{
    CaretAssert(annotation);
    const AnnotationTypeEnum::Enum annType = annotation->getType();
   
    switch (m_mode) {
        case AnnotationRedoUndoCommandModeEnum::INVALID:
            break;
        case AnnotationRedoUndoCommandModeEnum::COLOR_BACKGROUND:
        {
            CaretAssert(annotationValue);
            annotation->setBackgroundColor(annotationValue->getBackgroundColor());
            float rgba[4];
            annotationValue->getCustomBackgroundColor(rgba);
            annotation->setCustomBackgroundColor(rgba);
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::COLOR_FOREGROUND:
        {
            CaretAssert(annotationValue);
            annotation->setForegroundColor(annotationValue->getForegroundColor());
            float rgba[4];
            annotationValue->getCustomForegroundColor(rgba);
            annotation->setCustomForegroundColor(rgba);
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::COORDINATE_ONE:
        {
            AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
            AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);

            const AnnotationOneDimensionalShape* valueOneDimShape = dynamic_cast<const AnnotationOneDimensionalShape*>(annotationValue);
            const AnnotationTwoDimensionalShape* valueTwoDimShape = dynamic_cast<const AnnotationTwoDimensionalShape*>(annotationValue);
            
            if ((oneDimShape != NULL)
                && (valueOneDimShape != NULL)) {
                AnnotationCoordinate* coordDest = oneDimShape->getStartCoordinate();
                const AnnotationCoordinate* coordFrom = valueOneDimShape->getStartCoordinate();
                *coordDest = *coordFrom;
            }
            else if ((twoDimShape != NULL)
                     && (valueTwoDimShape != NULL)) {
                AnnotationCoordinate* coordDest = twoDimShape->getCoordinate();
                const AnnotationCoordinate* coordFrom = valueTwoDimShape->getCoordinate();
                *coordDest = *coordFrom;
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::COORDINATE_ONE_AND_TWO:
        {
            AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
            
            const AnnotationOneDimensionalShape* valueOneDimShape = dynamic_cast<const AnnotationOneDimensionalShape*>(annotationValue);
            
            if ((oneDimShape != NULL)
                && (valueOneDimShape != NULL)) {
                AnnotationCoordinate* coordDestStart = oneDimShape->getStartCoordinate();
                const AnnotationCoordinate* coordFromStart = valueOneDimShape->getStartCoordinate();
                *coordDestStart = *coordFromStart;

                AnnotationCoordinate* coordDestEnd = oneDimShape->getEndCoordinate();
                const AnnotationCoordinate* coordFromEnd = valueOneDimShape->getEndCoordinate();
                *coordDestEnd = *coordFromEnd;
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::COORDINATE_TWO:
        {
            AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
            const AnnotationOneDimensionalShape* valueOneDimShape = dynamic_cast<const AnnotationOneDimensionalShape*>(annotationValue);
            
            if ((oneDimShape != NULL)
                && (valueOneDimShape != NULL)) {
                AnnotationCoordinate* coordDest = oneDimShape->getEndCoordinate();
                const AnnotationCoordinate* coordFrom = valueOneDimShape->getEndCoordinate();
                *coordDest = *coordFrom;
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS:
            CaretAssertMessage(0, "This mode is handle in the redo() and undo() functions.");
            break;
        case AnnotationRedoUndoCommandModeEnum::LINE_ARROW_START:
            if ((annType == AnnotationTypeEnum::LINE)
                && (annotationValue->getType() == AnnotationTypeEnum::LINE)) {
                AnnotationLine* line = dynamic_cast<AnnotationLine*>(annotation);
                CaretAssert(line);
                const AnnotationLine* lineValue = dynamic_cast<const AnnotationLine*>(annotationValue);
                line->setDisplayStartArrow(lineValue->isDisplayStartArrow());
            }
            else {
                CaretAssert(0);
            }
            break;
        case AnnotationRedoUndoCommandModeEnum::LINE_ARROW_END:
            if ((annType == AnnotationTypeEnum::LINE)
                && (annotationValue->getType() == AnnotationTypeEnum::LINE)) {
                AnnotationLine* line = dynamic_cast<AnnotationLine*>(annotation);
                CaretAssert(line);
                const AnnotationLine* lineValue = dynamic_cast<const AnnotationLine*>(annotationValue);
                line->setDisplayEndArrow(lineValue->isDisplayEndArrow());
            }
            else {
                CaretAssert(0);
            }
            break;
        case AnnotationRedoUndoCommandModeEnum::LINE_WIDTH_FOREGROUND:
            if (annotation->isForegroundLineWidthSupported()) {
                //annotation->setForegroundLineWidth(value.m_floatValue);
                annotation->setForegroundLineWidth(annotationValue->getForegroundLineWidth());
            }
            break;
        case AnnotationRedoUndoCommandModeEnum::LOCATION_AND_SIZE:
            annotation->applyCoordinatesSizeAndRotationFromOther(annotationValue);
            break;
        case AnnotationRedoUndoCommandModeEnum::ROTATION_ANGLE:
            {
                AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
                const AnnotationTwoDimensionalShape* twoDimValue = dynamic_cast<const AnnotationTwoDimensionalShape*>(annotationValue);
                if ((twoDimAnn != NULL)
                    && (twoDimValue != NULL)) {
                    twoDimAnn->setRotationAngle(twoDimValue->getRotationAngle());
                }
                else {
                    CaretAssert(0);
                }
            }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_ALIGNMENT_HORIZONTAL:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setHorizontalAlignment(textAnnValue->getHorizontalAlignment());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_ALIGNMENT_VERTICAL:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setVerticalAlignment(textAnnValue->getVerticalAlignment());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_CHARACTERS:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setText(textAnnValue->getText());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_CONNECT_TO_BRAINORDINATE:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setConnectToBrainordinate(textAnnValue->getConnectToBrainordinate());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_BOLD:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setBoldEnabled(textAnnValue->isBoldEnabled());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_ITALIC:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setItalicEnabled(textAnnValue->isItalicEnabled());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_NAME:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setFont(textAnnValue->getFont());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_SIZE:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setFontSize(textAnnValue->getFontSize());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_UNDERLINE:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setUnderlineEnabled(textAnnValue->isUnderlineEnabled());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_ORIENTATION:
        {
            AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
            const AnnotationText* textAnnValue = dynamic_cast<const AnnotationText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setOrientation(textAnnValue->getOrientation());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TWO_DIM_HEIGHT:
        {
            AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
            const AnnotationTwoDimensionalShape* twoDimValue = dynamic_cast<const AnnotationTwoDimensionalShape*>(annotationValue);
            if ((twoDimAnn != NULL)
                && (twoDimValue != NULL)) {
                twoDimAnn->setHeight(twoDimValue->getHeight());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TWO_DIM_WIDTH:
        {
            AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
            const AnnotationTwoDimensionalShape* twoDimValue = dynamic_cast<const AnnotationTwoDimensionalShape*>(annotationValue);
            if ((twoDimAnn != NULL)
                && (twoDimValue != NULL)) {
                twoDimAnn->setWidth(twoDimValue->getWidth());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
    }
}

/**
 * Operation that "redoes" the command.
 */
void
AnnotationRedoUndoCommand::redo()
{
    for (std::vector<AnnotationMemento*>::iterator iter = m_annotationMementos.begin();
         iter != m_annotationMementos.end();
         iter++) {
        const AnnotationMemento* annMem = *iter;
        CaretAssert(annMem);
        
        if (m_mode == AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS) {
            EventAnnotationDeleteUndeleteFromFile event(EventAnnotationDeleteUndeleteFromFile::MODE_DELETE,
                                                        annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
        }
        else {
            applyRedoOrUndo(annMem->m_annotation,
                            annMem->m_redoAnnotation);
        }
    }
}

/**
 * Operation that "undoes" the command.
 */
void
AnnotationRedoUndoCommand::undo()
{
    for (std::vector<AnnotationMemento*>::iterator iter = m_annotationMementos.begin();
         iter != m_annotationMementos.end();
         iter++) {
        const AnnotationMemento* annMem = *iter;
        CaretAssert(annMem);
        
        if (m_mode == AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS) {
            EventAnnotationDeleteUndeleteFromFile event(EventAnnotationDeleteUndeleteFromFile::MODE_UNDELETE,
                                                        annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
        }
        else {
            applyRedoOrUndo(annMem->m_annotation,
                            annMem->m_undoAnnotation);
        }
    }
}

/**
 * @return Number of modified annotations.
 */
int32_t
AnnotationRedoUndoCommand::count() const
{
    return m_annotationMementos.size();
}

/**
 * Attempts to merge this command with command. Returns true on success; otherwise returns false.
 *
 * If this function returns true, calling this command's redo() must have the same effect as
 * redoing both this command and command. Similarly, calling this command's undo() must have
 * the same effect as undoing command and this command.
 *
 * @return True if the given command was merged with this command, else false.
 */
bool
AnnotationRedoUndoCommand::mergeWith(const CaretUndoCommand* command)
{
    const AnnotationRedoUndoCommand* otherCommand = dynamic_cast<const AnnotationRedoUndoCommand*>(command);
    CaretAssert(otherCommand);
    
    /*
     * Assume compatibility if same mode and applied to same number of annotations.
     */
    if (m_mode != otherCommand->m_mode) {
        return false;
    }
    
    if (m_annotationMementos.size() != otherCommand->m_annotationMementos.size()) {
        return false;
    }
    
    /*
     * Sort mementos so that the standard library's equal() function
     * can compare the annotation pointers
     */
    sortAnnotationMementos();
    otherCommand->sortAnnotationMementos();
    
    /*
     * Compare to verify that the corresponding annotation mementos
     * are the same.
     */
    if ( ! std::equal(m_annotationMementos.begin(),
                      m_annotationMementos.end(),
                      otherCommand->m_annotationMementos.begin(),
                      AnnotationRedoUndoCommand::equalAnnotationMemento)) {
        return false;
    }
    
    const int32_t numAnnMem = static_cast<int32_t>(m_annotationMementos.size());
    for (int32_t i = 0; i < numAnnMem; i++) {
        CaretAssertVectorIndex(m_annotationMementos, i);
        CaretAssertVectorIndex(otherCommand->m_annotationMementos, i);
        delete m_annotationMementos[i]->m_redoAnnotation;
        m_annotationMementos[i]->m_redoAnnotation = otherCommand->m_annotationMementos[i]->m_redoAnnotation->clone();
    }
    
    return true;
}


/**
 * Set them mode to first coordinate and create the redo/undo instances.
 *
 * @param coordinate
 *     New value of the coordinate.
 * @param annotations
 *     Annotations that receive this new coordinate.
 */
void
AnnotationRedoUndoCommand::setModeCoordinateOne(const AnnotationCoordinate& coordinate,
                                                const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::COORDINATE_ONE;
    setDescription("Coordinate");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        Annotation* redoAnnotation = annotation->clone();
        AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(redoAnnotation);
        AnnotationTwoDimensionalShape* twoDimShape = dynamic_cast<AnnotationTwoDimensionalShape*>(redoAnnotation);
        
        AnnotationCoordinate* redoCoordinate = NULL;
        if (oneDimShape != NULL) {
            redoCoordinate = oneDimShape->getStartCoordinate();
        }
        else if (twoDimShape != NULL) {
            redoCoordinate = twoDimShape->getCoordinate();
        }
        
        if (redoCoordinate != NULL) {
            *redoCoordinate = coordinate;
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
            
        }
        else {
            delete redoAnnotation;
        }
    }
}

/**
 * Set them mode to second coordinate and create the redo/undo instances.
 *
 * @param coordinateOne
 *     New value of the coordinate one.
 * @param coordinateTwo
 *     New value of the coordinate two.
 * @param annotations
 *     Annotations that receive this new coordinate.
 */
void
AnnotationRedoUndoCommand::setModeCoordinateOneAndTwo(const AnnotationCoordinate& coordinateOne,
                                                      const AnnotationCoordinate& coordinateTwo,
                                                      const std::vector<Annotation*> annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::COORDINATE_ONE_AND_TWO;
    setDescription("Coordinate");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        Annotation* redoAnnotation = annotation->clone();
        AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(redoAnnotation);
        
        if (oneDimShape != NULL) {
            AnnotationCoordinate* redoCoordinateOne = oneDimShape->getStartCoordinate();
            CaretAssert(redoCoordinateOne);
            AnnotationCoordinate* redoCoordinateTwo = oneDimShape->getEndCoordinate();
            CaretAssert(redoCoordinateTwo);

            *redoCoordinateOne = coordinateOne;
            *redoCoordinateTwo = coordinateTwo;
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
        else {
            delete redoAnnotation;
        }
    }
}

/**
 * Set them mode to location and size of annotations (coords, size, space, window, tab)
 *
 * @param annotationsBeforeMoveAndResize
 *     Annotations before move and resize has been applied.
 * @param annotationsAfterMoveAndResize
 *     Corresponding annotations after move and resize has been applied.
 */
void
AnnotationRedoUndoCommand::setModeLocationAndSize(const std::vector<Annotation*>& annotationsBeforeMoveAndResize,
                                                  const std::vector<Annotation*>& annotationsAfterMoveAndResize)
{
    const int32_t numAnnotations = static_cast<int32_t>(annotationsBeforeMoveAndResize.size());
    if (numAnnotations != static_cast<int32_t>(annotationsAfterMoveAndResize.size())) {
        CaretAssertMessage(0, "Number of annotations must be the same");
        CaretLogSevere("Number of annotations must be the same");
        return;
    }
    
    m_mode = AnnotationRedoUndoCommandModeEnum::LOCATION_AND_SIZE;
    setDescription("Reshape Annotations");
    
    for (int32_t i = 0; i < numAnnotations; i++) {
        CaretAssertVectorIndex(annotationsBeforeMoveAndResize, i);
        CaretAssertVectorIndex(annotationsAfterMoveAndResize, i);
        Annotation* annBefore = annotationsBeforeMoveAndResize[i];
        const Annotation* annAfter = annotationsAfterMoveAndResize[i];
        CaretAssert(annBefore);
        CaretAssert(annAfter);
        CaretAssert(annBefore->getType() == annAfter->getType());
        
        Annotation* undoAnnotation = annBefore->clone();
        Annotation* redoAnnotation = annAfter->clone();
        
        AnnotationMemento* am = new AnnotationMemento(annBefore,
                                                      redoAnnotation,
                                                      undoAnnotation);
        m_annotationMementos.push_back(am);
    }
}


/**
 * Set them mode to second coordinate and create the redo/undo instances.
 *
 * @param coordinate
 *     New value of the coordinate.
 * @param annotations
 *     Annotations that receive this new coordinate.
 */
void
AnnotationRedoUndoCommand::setModeCoordinateTwo(const AnnotationCoordinate& coordinate,
                                                const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::COORDINATE_TWO;
    setDescription("Coordinate");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        Annotation* redoAnnotation = annotation->clone();
        AnnotationOneDimensionalShape* oneDimShape = dynamic_cast<AnnotationOneDimensionalShape*>(redoAnnotation);
        
        AnnotationCoordinate* redoCoordinate = NULL;
        if (oneDimShape != NULL) {
            redoCoordinate = oneDimShape->getEndCoordinate();
        }
        
        if (redoCoordinate != NULL) {
            *redoCoordinate = coordinate;
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
            
        }
        else {
            delete redoAnnotation;
        }
    }
}

/**
 * Set them mode to line arrow start and create the redo/undo instances.
 *
 * @param newStatus
 *     New status of line arrow at start of line.
 * @param annotations
 *     Annotation that receive this new color.
 */
void
AnnotationRedoUndoCommand::setModeLineArrowStart(const bool newStatus,
                           const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::LINE_ARROW_START;
    setDescription("Line Arrow Start");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::LINE) {
            AnnotationLine* line = dynamic_cast<AnnotationLine*>(annotation);
            CaretAssert(line);
            AnnotationLine* redoAnnotation = dynamic_cast<AnnotationLine*>(annotation->clone());
            redoAnnotation->setDisplayStartArrow(newStatus);
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set them mode to line arrow end and create the redo/undo instances.
 *
 * @param newStatus
 *     New status of line arrow at end of line.
 * @param annotations
 *     Annotation that receive this new color.
 */
void
AnnotationRedoUndoCommand::setModeLineArrowEnd(const bool newStatus,
                         const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::LINE_ARROW_END;
    setDescription("Line Arrow End");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::LINE) {
            AnnotationLine* line = dynamic_cast<AnnotationLine*>(annotation);
            CaretAssert(line);
            AnnotationLine* redoAnnotation = dynamic_cast<AnnotationLine*>(annotation->clone());
            redoAnnotation->setDisplayEndArrow(newStatus);
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set them mode to line width and create the redo/undo instances.
 *
 * @param newLineWidth
 *     New line width.
 * @param annotations
 *     Annotation that receive this new line width.
 */
void
AnnotationRedoUndoCommand::setModeLineWidthForeground(const float newLineWidth,
                                                      const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::LINE_WIDTH_FOREGROUND;
    setDescription("Foreground Line Width");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        Annotation* redoAnnotation = annotation->clone();
        redoAnnotation->setForegroundLineWidth(newLineWidth);
        Annotation* undoAnnotation = annotation->clone();
        
        AnnotationMemento* am = new AnnotationMemento(annotation,
                                                      redoAnnotation,
                                                      undoAnnotation);
        m_annotationMementos.push_back(am);
    }
}

/**
 * Set the mode to color background and create the undo/redo instances.
 *
 * @param color
 *     The new background color.
 * @param customColor
 *     The new custom background color.
 * @param annotations
 *     Annotation that receive this new color.
 */
void
AnnotationRedoUndoCommand::setModeColorBackground(const CaretColorEnum::Enum color,
                                              const float customColor[4],
                                              const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::COLOR_BACKGROUND;
    setDescription("Background Color");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        Annotation* redoAnnotation = annotation->clone();
        redoAnnotation->setBackgroundColor(color);
        redoAnnotation->setCustomBackgroundColor(customColor);
        Annotation* undoAnnotation = annotation->clone();
        
        AnnotationMemento* am = new AnnotationMemento(annotation,
                                                      redoAnnotation,
                                                      undoAnnotation);
        m_annotationMementos.push_back(am);
    }
}

/**
 * Set the mode to color foreground and create the undo/redo instances.
 *
 * @param color
 *     The new foreground color.
 * @param customColor
 *     The new custom foreground color.
 * @param annotations
 *     Annotation that receive this new color.
 */
void
AnnotationRedoUndoCommand::setModeColorForeground(const CaretColorEnum::Enum color,
                            const float customColor[4],
                            const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::COLOR_FOREGROUND;
    setDescription("Foreground Color");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        
        Annotation* redoAnnotation = annotation->clone();
        redoAnnotation->setForegroundColor(color);
        redoAnnotation->setCustomForegroundColor(customColor);
        Annotation* undoAnnotation = annotation->clone();
        
        AnnotationMemento* am = new AnnotationMemento(annotation,
                                                       redoAnnotation,
                                                       undoAnnotation);
        
        m_annotationMementos.push_back(am);
    }
}

/**
 * Set the mode to delete annotations and create the undo/redo instances.
 *
 * @param annotations
 *     Annotation that are deleted.
 */
void
AnnotationRedoUndoCommand::setModeDeleteAnnotations(const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS;
    setDescription("Delete Annotations");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        /*
         * NOTE: We only need the pointer since the file containing
         * the annotation will handle delete/undelete of the 
         * annotation.  If we don't use NULL for the redo and
         * undo annotations, copies of the annotation would be 
         * needed since the AnnotationMemento will delete
         * the redo and undo annotations when it is deleted.
         */
        AnnotationMemento* am = new AnnotationMemento(annotation,
                                                      NULL,
                                                      NULL);
        
        m_annotationMementos.push_back(am);
    }
}

/**
 * Set the mode to rotation angle and create the undo/redo instances
 *
 * @param newRotationAngle
 *     The new rotation angle
 * @param annotations
 *     Annotations that receive this new rotation angle
 */
void
AnnotationRedoUndoCommand::setModeRotationAngle(const float newRotationAngle,
                                                const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::ROTATION_ANGLE;
    setDescription("Rotation Angle");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
        if (twoDimAnn != NULL) {
            AnnotationTwoDimensionalShape* redoAnnotation = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setRotationAngle(newRotationAngle);
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
    
}


/**
 * Set the mode to horizontal text alignment and create the undo/redo instances
 *
 * @param newHorizontalAlignment
 *     The new horizontal alignment
 * @param annotations
 *     Annotation that receive this new text.
 */
void
AnnotationRedoUndoCommand::setModeTextAlignmentHorizontal(const AnnotationTextAlignHorizontalEnum::Enum newHorizontalAlignment,
                                                          const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_ALIGNMENT_HORIZONTAL;
    setDescription("Text Horizontal Alignment");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setHorizontalAlignment(newHorizontalAlignment);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to vertical text alignment and create the undo/redo instances
 *
 * @param newVerticalAlignment
 *     The new vertical alignment
 * @param annotations
 *     Annotation that receive this new text.
 */
void
AnnotationRedoUndoCommand::setModeTextAlignmentVertical(const AnnotationTextAlignVerticalEnum::Enum newVerticalAlignment,
                                                        const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_ALIGNMENT_VERTICAL;
    setDescription("Text Vertical Alignment");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setVerticalAlignment(newVerticalAlignment);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}


/**
 * Set the mode to text characters and create the undo/redo instances.
 *
 * @param text
 *     The text .
 * @param annotations
 *     Annotation that receive this new text.
 */
void
AnnotationRedoUndoCommand::setModeTextCharacters(const AString& text,
                           const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_CHARACTERS;
    setDescription("Text Characters");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setText(text);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to text connect to brainordinate and create the undo/redo instances.
 *
 * @param newConnectType
 *     New value for connect to brainordinate.
 * @param annotations
 *     Annotation that receive this new status.
 */
void
AnnotationRedoUndoCommand::setModeTextConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum newConnectType,
                                                             const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_CONNECT_TO_BRAINORDINATE;
    setDescription("Text Connect to Brainordinate");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setConnectToBrainordinate(newConnectType);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to text font bold and create the undo/redo instances.
 *
 * @param newStatus
 *     New status for font bold.
 * @param annotations
 *     Annotation that receive this new status.
 */
void
AnnotationRedoUndoCommand::setModeTextFontBold(const bool newStatus,
                                               const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_BOLD;
    setDescription("Text Bold");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setBoldEnabled(newStatus);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to text font italic and create the undo/redo instances.
 *
 * @param newStatus
 *     New status for font italic.
 * @param annotations
 *     Annotation that receive this new status.
 */
void
AnnotationRedoUndoCommand::setModeTextFontItalic(const bool newStatus,
                                                 const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_ITALIC;
    setDescription("Text Italic");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setItalicEnabled(newStatus);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to font name and create the undo/redo instances.
 *
 * @param newFontName
 *     New font name
 * @param annotations
 *     Annotation that receive this font name
 */
void
AnnotationRedoUndoCommand::setModeTextFontName(const AnnotationFontNameEnum::Enum newFontName,
                                               const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_NAME;
    setDescription("Text Font Name");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setFont(newFontName);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to font size and create the undo/redo instances.
 *
 * @param newFontSize
 *     New font size
 * @param annotations
 *     Annotation that receive this font size
 */
void
AnnotationRedoUndoCommand::setModeTextFontSize(const AnnotationFontSizeEnum::Enum newFontSize,
                                               const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_SIZE;
    setDescription("Text Font Size");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setFontSize(newFontSize);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}


/**
 * Set the mode to text font underline and create the undo/redo instances.
 *
 * @param newStatus
 *     New status for font undeline.
 * @param annotations
 *     Annotation that receive this new status.
 */
void
AnnotationRedoUndoCommand::setModeTextFontUnderline(const bool newStatus,
                                                    const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_UNDERLINE;
    setDescription("Text Underline");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setUnderlineEnabled(newStatus);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to vertical text alignment and create the undo/redo instances
 *
 * @param newVerticalAlignment
 *     The new vertical alignment
 * @param annotations
 *     Annotation that receive this new text.
 */
void
AnnotationRedoUndoCommand::setModeTextOrientation(const AnnotationTextOrientationEnum::Enum newTextOrientation,
                                                  const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_ORIENTATION;
    setDescription("Text Orientation");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationText* redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setOrientation(newTextOrientation);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to two dim height and create the undo/redo instances
 *
 * @param newHeight
 *     The new height
 * @param annotations
 *     Annotation that receive this new text.
 */
void
AnnotationRedoUndoCommand::setModeTwoDimHeight(const float newHeight,
                                               const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TWO_DIM_HEIGHT;
    setDescription("Height");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);

        AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
        if (twoDimAnn != NULL) {
            AnnotationTwoDimensionalShape* redoAnnotation = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setHeight(newHeight);
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to two dim width and create the undo/redo instances
 *
 * @param newWidth
 *     The new vertical alignment
 * @param annotations
 *     Annotation that receive this new text.
 */
void
AnnotationRedoUndoCommand::setModeTwoDimWidth(const float newWidth,
                                              const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TWO_DIM_WIDTH;
    setDescription("Width");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
        if (twoDimAnn != NULL) {
            AnnotationTwoDimensionalShape* redoAnnotation = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setWidth(newWidth);
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Compare the two annotation mementos using the annotation pointer.
 *
 * @param am1
 *     Left side for comparison.
 * @param am2
 *     Right side for comparison.
 * @return
 *     True if left side's annotation pointer is equal to the right side's annotation pointer.
 */
bool
AnnotationRedoUndoCommand::equalAnnotationMemento(const AnnotationMemento* am1,
                                                  const AnnotationMemento* am2)
{
    CaretAssert(am1);
    CaretAssert(am2);

    return (am1->m_annotation == am2->m_annotation);
}



/**
 * Compare the two annotation mementos using the annotation pointer.
 * 
 * @param am1
 *     Left side for comparison.
 * @param am2
 *     Right side for comparison.
 * @return
 *     True if left side's annotation pointer is less than right side's annotation pointer.
 */
bool
AnnotationRedoUndoCommand::lessThanAnnotationMemento(const AnnotationMemento* am1,
                                                     const AnnotationMemento* am2)
{
    CaretAssert(am1);
    CaretAssert(am2);
    
    return (am1->m_annotation < am2->m_annotation);
}

/**
 * Sort the annotation mementos in this redo/undo command.
 * The mergeWith() method needs to compare the mementos 
 * in two command and when the mementos are sorted,
 * it allows faster comparison.
 */
void
AnnotationRedoUndoCommand::sortAnnotationMementos() const
{
    if (m_sortedFlag) {
        return;
    }
    
    std::sort(m_annotationMementos.begin(),
              m_annotationMementos.end(),
              AnnotationRedoUndoCommand::lessThanAnnotationMemento);
    
    m_sortedFlag = true;
}



