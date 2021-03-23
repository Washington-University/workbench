
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

#include <cmath>

#include "AnnotationBrowserTab.h"
#include "AnnotationFontAttributesInterface.h"
#include "AnnotationLine.h"
#include "AnnotationMultiCoordinateShape.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationPointSizeText.h"
#include "AnnotationStackingOrderOperation.h"
#include "AnnotationOneCoordinateShape.h"
#include "CaretLogger.h"
#include "EventAnnotationAddToRemoveFromFile.h"
#include "EventAnnotationGrouping.h"
#include "EventAnnotationValidate.h"
#include "EventGetViewportSize.h"
#include "EventManager.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"

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
    m_annotationGroupMemento = NULL;
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
    
    if (m_annotationGroupMemento != NULL) {
        delete m_annotationGroupMemento;
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
    
    /*
     * Test the validity of the annotation (it still exists) to
     * avoid a crash.
     */
    EventAnnotationValidate validateEvent(annotation);
    EventManager::get()->sendEvent(validateEvent.getPointer());
    if ( ! validateEvent.isAnnotationValid()) {
        CaretLogFine("Failed to validate annotation for redo/undo.  Annotation was likely deleted (not an error).");
        return;
    }
    
    
    const AnnotationTypeEnum::Enum annType = annotation->getType();
   
    switch (m_mode) {
        case AnnotationRedoUndoCommandModeEnum::INVALID:
            break;
        case AnnotationRedoUndoCommandModeEnum::BROWSER_TAB_BACKGROUND:
        {
            CaretAssert(annotation);
            CaretAssert(annotationValue);
            
            AnnotationBrowserTab* annotationBrowserTab = dynamic_cast<AnnotationBrowserTab*>(annotation);
            const AnnotationBrowserTab* annotationBrowserTabValue = dynamic_cast<const AnnotationBrowserTab*>(annotationValue);
            CaretAssert(annotationBrowserTab);
            CaretAssert(annotationBrowserTabValue);
            
            const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType = annotationBrowserTabValue->getBackgroundType();
            annotationBrowserTab->setBackgroundType(backgroundType);
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::BOUNDS_2D_ALL:
        case AnnotationRedoUndoCommandModeEnum::BOUNDS_2D_SINGLE:
        {
            CaretAssert(annotation);
            CaretAssert(annotationValue);
            
            AnnotationBrowserTab* annotationBrowserTab = dynamic_cast<AnnotationBrowserTab*>(annotation);
            const AnnotationBrowserTab* annotationBrowserTabValue = dynamic_cast<const AnnotationBrowserTab*>(annotationValue);
            CaretAssert(annotationBrowserTab);
            CaretAssert(annotationBrowserTabValue);
            
            float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
            annotationBrowserTabValue->getBounds2D(minX, maxX, minY, maxY);
            annotationBrowserTab->setBounds2D(minX, maxX, minY, maxY);
        }
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
            annotation->setLineColor(annotationValue->getLineColor());
            float rgba[4];
            annotationValue->getCustomLineColor(rgba);
            annotation->setCustomLineColor(rgba);
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::COORDINATE_ONE:
        {
            AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation);
            AnnotationOneCoordinateShape* twoDimShape = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);

            const AnnotationTwoCoordinateShape* valueOneDimShape = dynamic_cast<const AnnotationTwoCoordinateShape*>(annotationValue);
            const AnnotationOneCoordinateShape* valueTwoDimShape = dynamic_cast<const AnnotationOneCoordinateShape*>(annotationValue);
            
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
            AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation);
            
            const AnnotationTwoCoordinateShape* valueOneDimShape = dynamic_cast<const AnnotationTwoCoordinateShape*>(annotationValue);
            
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
            AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation);
            const AnnotationTwoCoordinateShape* valueOneDimShape = dynamic_cast<const AnnotationTwoCoordinateShape*>(annotationValue);
            
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
        case AnnotationRedoUndoCommandModeEnum::COORDINATE_MULTI:
        {
            AnnotationMultiCoordinateShape* multiCoordShape = dynamic_cast<AnnotationMultiCoordinateShape*>(annotation);
            const AnnotationMultiCoordinateShape* valueMultiCoordShape = dynamic_cast<const AnnotationMultiCoordinateShape*>(annotationValue);
            
            if ((multiCoordShape != NULL)
                && (valueMultiCoordShape != NULL)) {
                std::vector<std::unique_ptr<AnnotationCoordinate>> coords(valueMultiCoordShape->getCopyOfAllCoordinates());
                multiCoordShape->replaceAllCoordinatesNotConst(coords);
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::CREATE_ANNOTATION:
            CaretAssertMessage(0, ("This mode "
                                   + AnnotationRedoUndoCommandModeEnum::toName(m_mode)
                                   + " is handle in the redo() and undo() functions."));
            break;
        case AnnotationRedoUndoCommandModeEnum::CUT_ANNOTATION:
            CaretAssertMessage(0, ("This mode "
                                   + AnnotationRedoUndoCommandModeEnum::toName(m_mode)
                                   + " is handle in the redo() and undo() functions."));
            break;
        case AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS:
            CaretAssertMessage(0, ("This mode "
                                   + AnnotationRedoUndoCommandModeEnum::toName(m_mode)
                                   + " is handle in the redo() and undo() functions."));
            break;
        case AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATION:
            CaretAssertMessage(0, ("This mode "
                                   + AnnotationRedoUndoCommandModeEnum::toName(m_mode)
                                   + " is handle in the redo() and undo() functions."));
            break;
        case AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATIONS:
            CaretAssertMessage(0, ("This mode "
                                   + AnnotationRedoUndoCommandModeEnum::toName(m_mode)
                                   + " is handle in the redo() and undo() functions."));
            break;
        case AnnotationRedoUndoCommandModeEnum::GROUPING_GROUP:
            CaretAssert(0);
            break;
        case AnnotationRedoUndoCommandModeEnum::GROUPING_REGROUP:
            CaretAssert(0);
            break;
        case AnnotationRedoUndoCommandModeEnum::GROUPING_UNGROUP:
            CaretAssert(0);
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
            if (annotation->testProperty(Annotation::Property::LINE_THICKNESS)) {
                annotation->setLineWidthPercentage(annotationValue->getLineWidthPercentage());
            }
            break;
        case AnnotationRedoUndoCommandModeEnum::LOCATION_AND_SIZE:
            annotation->applyCoordinatesSizeAndRotationFromOther(annotationValue);
            break;
        case AnnotationRedoUndoCommandModeEnum::PASTE_ANNOTATION:
            CaretAssertMessage(0, ("This mode "
                                   + AnnotationRedoUndoCommandModeEnum::toName(m_mode)
                                   + " is handle in the redo() and undo() functions."));
            break;
        case AnnotationRedoUndoCommandModeEnum::MULTI_COORD_INSERT_COORDINATE:
        case AnnotationRedoUndoCommandModeEnum::MULTI_COORD_REMOVE_COORDINATE:
        {
            AnnotationMultiCoordinateShape* multiCoordAnn = annotation->castToMultiCoordinateShape();
            const AnnotationMultiCoordinateShape* multiCoordAnnValue = annotationValue->castToMultiCoordinateShape();
            if ((multiCoordAnn != NULL)
                && (multiCoordAnnValue != NULL)) {
                const std::vector<std::unique_ptr<AnnotationCoordinate>> allCoords(multiCoordAnnValue->getCopyOfAllCoordinates());
                multiCoordAnn->replaceAllCoordinatesNotConst(allCoords);
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::ROTATION_ANGLE:
            {
                AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);
                const AnnotationOneCoordinateShape* twoDimValue = dynamic_cast<const AnnotationOneCoordinateShape*>(annotationValue);
                if ((twoDimAnn != NULL)
                    && (twoDimValue != NULL)) {
                    twoDimAnn->setRotationAngle(twoDimValue->getRotationAngle());
                }
                else {
                    AnnotationTwoCoordinateShape* oneDimAnn = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation);
                    const AnnotationTwoCoordinateShape* oneDimValue = dynamic_cast<const AnnotationTwoCoordinateShape*>(annotationValue);
                    if ((oneDimAnn != NULL)
                        && (oneDimValue != NULL)) {
                        oneDimAnn->getStartCoordinate()->setXYZ(oneDimValue->getStartCoordinate()->getXYZ());
                        oneDimAnn->getEndCoordinate()->setXYZ(oneDimValue->getEndCoordinate()->getXYZ());
                    }
                }
            }
            break;
        case AnnotationRedoUndoCommandModeEnum::STACKING_ORDER_ANNOTATIONS:
        {
            const AnnotationTwoCoordinateShape* oneDimAnn = annotationValue->castToTwoCoordinateShape();
            const AnnotationOneCoordinateShape* twoDimAnn = annotationValue->castToOneCoordinateShape();
            if (oneDimAnn != NULL) {
                float xyz[3];
                oneDimAnn->getStartCoordinate()->getXYZ(xyz);
                const float z1 = xyz[2];
                oneDimAnn->getEndCoordinate()->getXYZ(xyz);
                const float z2 = xyz[2];
                
                AnnotationTwoCoordinateShape* ann = annotation->castToTwoCoordinateShape();
                if (ann != NULL) {
                    ann->getStartCoordinate()->getXYZ(xyz);
                    xyz[2] = z1;
                    ann->getStartCoordinate()->setXYZ(xyz);
                    
                    ann->getEndCoordinate()->getXYZ(xyz);
                    xyz[2] = z2;
                    ann->getEndCoordinate()->setXYZ(xyz);
                }
            }
            else if (twoDimAnn != NULL) {
                float xyz[3];
                twoDimAnn->getCoordinate()->getXYZ(xyz);
                const float z = xyz[2];
                
                AnnotationOneCoordinateShape* ann = annotation->castToOneCoordinateShape();
                if (ann != NULL) {
                    ann->getCoordinate()->getXYZ(xyz);
                    xyz[2] = z;
                    ann->getCoordinate()->setXYZ(xyz);
                }
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::STACKING_ORDER_BROWSER_TAB:
        {
            annotation->setStackingOrder(annotationValue->getStackingOrder());
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
        case AnnotationRedoUndoCommandModeEnum::TEXT_COLOR:
        {
            AnnotationFontAttributesInterface* textAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
            const AnnotationFontAttributesInterface* textAnnValue = dynamic_cast<const AnnotationFontAttributesInterface*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setTextColor(textAnnValue->getTextColor());
                float rgba[4];
                textAnnValue->getCustomTextColor(rgba);
                textAnn->setCustomTextColor(rgba);
            }
            else {
                CaretAssert(0);
            }
            
            CaretAssert(annotationValue);
            annotation->setLineColor(annotationValue->getLineColor());
            float rgba[4];
            annotationValue->getCustomLineColor(rgba);
            annotation->setCustomLineColor(rgba);
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
            AnnotationFontAttributesInterface* textAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
            const AnnotationFontAttributesInterface* textAnnValue = dynamic_cast<const AnnotationFontAttributesInterface*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setBoldStyleEnabled(textAnnValue->isBoldStyleEnabled());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_ITALIC:
        {
            AnnotationFontAttributesInterface* textAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
            const AnnotationFontAttributesInterface* textAnnValue = dynamic_cast<const AnnotationFontAttributesInterface*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setItalicStyleEnabled(textAnnValue->isItalicStyleEnabled());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_NAME:
        {
            AnnotationFontAttributesInterface* textAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
            const AnnotationFontAttributesInterface* textAnnValue = dynamic_cast<const AnnotationFontAttributesInterface*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setFont(textAnnValue->getFont());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_PERCENT_SIZE:
        {
            AnnotationFontAttributesInterface* textAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
            const AnnotationFontAttributesInterface* textAnnValue = dynamic_cast<const AnnotationFontAttributesInterface*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setFontPercentViewportSize(textAnnValue->getFontPercentViewportSize());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_POINT_SIZE:
        {
            AnnotationPointSizeText* textAnn = dynamic_cast<AnnotationPointSizeText*>(annotation);
            const AnnotationPointSizeText* textAnnValue = dynamic_cast<const AnnotationPointSizeText*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setFontPointSize(textAnnValue->getFontPointSize());
            }
            else {
                CaretAssert(0);
            }
        }
            break;
        case AnnotationRedoUndoCommandModeEnum::TEXT_FONT_UNDERLINE:
        {
            AnnotationFontAttributesInterface* textAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
            const AnnotationFontAttributesInterface* textAnnValue = dynamic_cast<const AnnotationFontAttributesInterface*>(annotationValue);
            if ((textAnn != NULL)
                && (textAnnValue != NULL)) {
                textAnn->setUnderlineStyleEnabled(textAnnValue->isUnderlineStyleEnabled());
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
            AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);
            const AnnotationOneCoordinateShape* twoDimValue = dynamic_cast<const AnnotationOneCoordinateShape*>(annotationValue);
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
            AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);
            const AnnotationOneCoordinateShape* twoDimValue = dynamic_cast<const AnnotationOneCoordinateShape*>(annotationValue);
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
 *
 * @param errorMessageOut
 *     Output containing error message.
 * @return
 *     True if the command executed successfully, else false.
 */
bool
AnnotationRedoUndoCommand::redo(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (m_mode == AnnotationRedoUndoCommandModeEnum::GROUPING_GROUP) {
        CaretAssert(m_annotationGroupMemento);
        EventAnnotationGrouping groupEvent;
        groupEvent.setModeGroupAnnotations(getWindowIndex(),
                                           m_annotationGroupMemento->m_annotationGroupKey,
                                           m_annotationGroupMemento->m_annotations);
        EventManager::get()->sendEvent(groupEvent.getPointer());
        
        m_annotationGroupMemento->setUndoAnnotationGroupKey(groupEvent.getGroupKeyToWhichAnnotationsWereMoved());
        
        if (groupEvent.isError()) {
            errorMessageOut = groupEvent.getErrorMessage();
            return false;
        }
        return true;
    }
    else if (m_mode == AnnotationRedoUndoCommandModeEnum::GROUPING_UNGROUP) {
        CaretAssert(m_annotationGroupMemento);
        
        EventAnnotationGrouping groupEvent;
        groupEvent.setModeUngroupAnnotations(getWindowIndex(),
                                             m_annotationGroupMemento->m_annotationGroupKey);
        
        EventManager::get()->sendEvent(groupEvent.getPointer());
        
        if (groupEvent.isError()) {
            errorMessageOut = groupEvent.getErrorMessage();
            return false;
        }
        return true;
    }
    else if (m_mode == AnnotationRedoUndoCommandModeEnum::GROUPING_REGROUP) {
        CaretAssert(m_annotationGroupMemento);
        
        EventAnnotationGrouping groupEvent;
        groupEvent.setModeRegroupAnnotations(getWindowIndex(),
                                             m_annotationGroupMemento->m_annotationGroupKey);
        
        EventManager::get()->sendEvent(groupEvent.getPointer());
        
        m_annotationGroupMemento->setUndoAnnotationGroupKey(groupEvent.getGroupKeyToWhichAnnotationsWereMoved());
        
        if (groupEvent.isError()) {
            errorMessageOut = groupEvent.getErrorMessage();
            return false;
        }
        return true;
    }
    
    bool validFlag = true;
    
    for (std::vector<AnnotationMemento*>::iterator iter = m_annotationMementos.begin();
         iter != m_annotationMementos.end();
         iter++) {
        const AnnotationMemento* annMem = *iter;
        CaretAssert(annMem);
        
        if (m_mode == AnnotationRedoUndoCommandModeEnum::CREATE_ANNOTATION) {
            EventAnnotationAddToRemoveFromFile event(EventAnnotationAddToRemoveFromFile::MODE_CREATE,
                                                     annMem->m_annotationFile,
                                                     annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
            
            if (event.isError()) {
                errorMessageOut.appendWithNewLine(event.getErrorMessage());
                validFlag = false;
            }
        }
        else if (m_mode == AnnotationRedoUndoCommandModeEnum::CUT_ANNOTATION) {
            EventAnnotationAddToRemoveFromFile event(EventAnnotationAddToRemoveFromFile::MODE_CUT,
                                                     annMem->m_annotationFile,
                                                     annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
            
            if (event.isError()) {
                errorMessageOut.appendWithNewLine(event.getErrorMessage());
                validFlag = false;
            }
        }
        else if (m_mode == AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS) {
            EventAnnotationAddToRemoveFromFile event(EventAnnotationAddToRemoveFromFile::MODE_DELETE,
                                                     annMem->m_annotationFile,
                                                     annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
            
            if (event.isError()) {
                errorMessageOut.appendWithNewLine(event.getErrorMessage());
                validFlag = false;
            }
        }
        else if ((m_mode == AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATION)
                 || (m_mode == AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATIONS)) {
            EventAnnotationAddToRemoveFromFile duplicateEvent(EventAnnotationAddToRemoveFromFile::MODE_DUPLICATE,
                                                          annMem->m_annotationFile,
                                                          annMem->m_annotation);
            EventManager::get()->sendEvent(duplicateEvent.getPointer());
            
            if (duplicateEvent.isError()) {
                errorMessageOut.appendWithNewLine(duplicateEvent.getErrorMessage());
                validFlag = false;
            }
        }
        else if (m_mode == AnnotationRedoUndoCommandModeEnum::PASTE_ANNOTATION) {
            EventAnnotationAddToRemoveFromFile pasteEvent(EventAnnotationAddToRemoveFromFile::MODE_PASTE,
                                                           annMem->m_annotationFile,
                                                           annMem->m_annotation);
            EventManager::get()->sendEvent(pasteEvent.getPointer());
            
            if (pasteEvent.isError()) {
                errorMessageOut.appendWithNewLine(pasteEvent.getErrorMessage());
                validFlag = false;
            }
        }
        else {
            /*
             * Note: Does not report any errors
             */
            applyRedoOrUndo(annMem->m_annotation,
                            annMem->m_redoAnnotation);
        }
    }
    
    return validFlag;
}

/**
 * Operation that "undoes" the command.
 *
 * @param errorMessageOut
 *     Output containing error message.
 * @return
 *     True if the command executed successfully, else false. */
bool
AnnotationRedoUndoCommand::undo(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (m_mode == AnnotationRedoUndoCommandModeEnum::GROUPING_GROUP) {
        CaretAssert(m_annotationGroupMemento);
        EventAnnotationGrouping groupEvent;
        groupEvent.setModeUngroupAnnotations(getWindowIndex(),
                                             m_annotationGroupMemento->m_undoAnnotationGroupKey);
        EventManager::get()->sendEvent(groupEvent.getPointer());
        
        if (groupEvent.isError()) {
            errorMessageOut = groupEvent.getErrorMessage();
            return false;
        }
        return true;
    }
    else if (m_mode == AnnotationRedoUndoCommandModeEnum::GROUPING_UNGROUP) {
        CaretAssert(m_annotationGroupMemento);
        
        EventAnnotationGrouping groupEvent;
        groupEvent.setModeRegroupAnnotations(getWindowIndex(),
                                             m_annotationGroupMemento->m_annotationGroupKey);
        
        EventManager::get()->sendEvent(groupEvent.getPointer());
        
        if (groupEvent.isError()) {
            errorMessageOut = groupEvent.getErrorMessage();
            return false;
        }
        return true;
    }
    else if (m_mode == AnnotationRedoUndoCommandModeEnum::GROUPING_REGROUP) {
        CaretAssert(m_annotationGroupMemento);
        
        EventAnnotationGrouping groupEvent;
        groupEvent.setModeUngroupAnnotations(getWindowIndex(),
                                             m_annotationGroupMemento->m_undoAnnotationGroupKey);
        
        EventManager::get()->sendEvent(groupEvent.getPointer());
        
        m_annotationGroupMemento->setUndoAnnotationGroupKey(groupEvent.getGroupKeyToWhichAnnotationsWereMoved());
        
        
        if (groupEvent.isError()) {
            errorMessageOut = groupEvent.getErrorMessage();
            return false;
        }
        return true;
    }
    
    bool validFlag = true;
    
    for (std::vector<AnnotationMemento*>::iterator iter = m_annotationMementos.begin();
         iter != m_annotationMementos.end();
         iter++) {
        const AnnotationMemento* annMem = *iter;
        CaretAssert(annMem);
        
        if (m_mode == AnnotationRedoUndoCommandModeEnum::CREATE_ANNOTATION) {
            EventAnnotationAddToRemoveFromFile event(EventAnnotationAddToRemoveFromFile::MODE_UNCREATE,
                                                     annMem->m_annotationFile,
                                                     annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
            
            if (event.isError()) {
                errorMessageOut.appendWithNewLine(event.getErrorMessage());
                validFlag = false;
            }
        }
        else if (m_mode == AnnotationRedoUndoCommandModeEnum::CUT_ANNOTATION) {
            EventAnnotationAddToRemoveFromFile event(EventAnnotationAddToRemoveFromFile::MODE_UNCUT,
                                                     annMem->m_annotationFile,
                                                     annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
            
            if (event.isError()) {
                errorMessageOut.appendWithNewLine(event.getErrorMessage());
                validFlag = false;
            }
        }
        else if (m_mode == AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS) {
            EventAnnotationAddToRemoveFromFile event(EventAnnotationAddToRemoveFromFile::MODE_UNDELETE,
                                                     annMem->m_annotationFile,
                                                     annMem->m_annotation);
            EventManager::get()->sendEvent(event.getPointer());
            
            if (event.isError()) {
                errorMessageOut.appendWithNewLine(event.getErrorMessage());
                validFlag = false;
            }
        }
        else if ((m_mode == AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATION)
                 || (m_mode == AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATIONS)) {
            EventAnnotationAddToRemoveFromFile duplicateEvent(EventAnnotationAddToRemoveFromFile::MODE_UNDUPLICATE,
                                                              annMem->m_annotationFile,
                                                              annMem->m_annotation);
            EventManager::get()->sendEvent(duplicateEvent.getPointer());
            
            if (duplicateEvent.isError()) {
                errorMessageOut.appendWithNewLine(duplicateEvent.getErrorMessage());
                validFlag = false;
            }
        }
        else if (m_mode == AnnotationRedoUndoCommandModeEnum::PASTE_ANNOTATION) {
            EventAnnotationAddToRemoveFromFile pasteEvent(EventAnnotationAddToRemoveFromFile::MODE_UNPASTE,
                                                           annMem->m_annotationFile,
                                                           annMem->m_annotation);
            EventManager::get()->sendEvent(pasteEvent.getPointer());
            
            if (pasteEvent.isError()) {
                errorMessageOut.appendWithNewLine(pasteEvent.getErrorMessage());
                validFlag = false;
            }
        }
        else {
            applyRedoOrUndo(annMem->m_annotation,
                            annMem->m_undoAnnotation);
        }
    }
    
    return validFlag;
}

/**
 * @return Is the command valid?
 */
bool
AnnotationRedoUndoCommand::isValid() const
{
    if (m_annotationMementos.size() > 0) {
        return true;
    }
    else if (m_annotationGroupMemento != NULL) {
        return true;
    }
    
    return false;
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
 * Set them mode to all bounds and create the redo/undo instances.
 *
 * @param minX
 *     New minimum x-value of the annotation
 * @param maxX
 *     New maximum x-value of the annotation
 * @param minY
 *     New minimum y-value of the annotation
 * @param maxY
 *     New minimum y-value of the annotation
 * @param annotation
 *     Annotation that receive this new bounds (browser tabs only).
 */
void
AnnotationRedoUndoCommand::setBoundsAll(const float minX,
                                        const float maxX,
                                        const float minY,
                                        const float maxY,
                                        Annotation* annotation)
{
    CaretAssert(annotation);
    
    AnnotationBrowserTab* browserTabAnnotation = dynamic_cast<AnnotationBrowserTab*>(annotation);
    if (browserTabAnnotation != NULL) {
        m_mode = AnnotationRedoUndoCommandModeEnum::BOUNDS_2D_ALL;
        setDescription("Bounds 2D ALL");
        
        AnnotationBrowserTab* redoAnnotation = dynamic_cast<AnnotationBrowserTab*>(browserTabAnnotation->clone());
        CaretAssert(redoAnnotation);
        redoAnnotation->setBounds2D(minX, maxX, minY, maxY);
        
        Annotation* undoAnnotation = browserTabAnnotation->clone();
        AnnotationMemento* am = new AnnotationMemento(annotation,
                                                      redoAnnotation,
                                                      undoAnnotation);
        m_annotationMementos.push_back(am);
    }
    else {
        CaretLogWarning("Annotation for bounds setting must be a Browser Tab Annotation");
    }
}

/**
 * Set background type and create the redo/undo instances.
 *
 * @param backgroundType
 *     New value for background type
 * @param annotations
 *     Annotations that receive this background type (browser tabs only).
 */
void
AnnotationRedoUndoCommand::setBrowserTabBackground(const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType,
                                                   const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::BROWSER_TAB_BACKGROUND;
    setDescription("Browser Tab Background");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        AnnotationBrowserTab* browserTabAnnotation = dynamic_cast<AnnotationBrowserTab*>(annotation);
        if (browserTabAnnotation != NULL) {
            AnnotationBrowserTab* redoAnnotation = dynamic_cast<AnnotationBrowserTab*>(browserTabAnnotation->clone());
            CaretAssert(redoAnnotation);
            
            redoAnnotation->setBackgroundType(backgroundType);
            
            Annotation* undoAnnotation = browserTabAnnotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
        else {
            CaretLogWarning("Annotation for background type must be a Browser Tab Annotation");
        }
    }
}

/**
 * Set them mode to x-minimum and create the redo/undo instances.
 *
 * @param newMinX
 *     New minimum x-value of the annotation
 * @param annotations
 *     Annotations that receive this new bounds (browser tabs only).
 */
void
AnnotationRedoUndoCommand::setBoundsMinX2D(const float newMinX,
                                           const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::BOUNDS_2D_SINGLE;
    setDescription("Bounds 2D X-Min");
    setBounds2DHelper(BoundsType2D::MIN_X,
                      newMinX,
                      annotations);
}

/**
 * Set them mode to x-maximum and create the redo/undo instances.
 *
 * @param newMaxX
 *     New minimum x-value of the annotation
 * @param annotations
 *     Annotations that receive this new bounds (browser tabs only).
 */
void
AnnotationRedoUndoCommand::setBoundsMaxX2D(const float newMaxX,
                                           const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::BOUNDS_2D_SINGLE;
    setDescription("Bounds 2D X-Max");
    setBounds2DHelper(BoundsType2D::MAX_X,
                      newMaxX,
                      annotations);
}

/**
 * Set them mode to y-minimum and create the redo/undo instances.
 *
 * @param newMinY
 *     New minimum y-value of the annotation
 * @param annotations
 *     Annotations that receive this new bounds (browser tabs only).
 */
void
AnnotationRedoUndoCommand::setBoundsMinY2D(const float newMinY,
                                           const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::BOUNDS_2D_SINGLE;
    setDescription("Bounds 2D Y-Min");
    setBounds2DHelper(BoundsType2D::MIN_Y,
                      newMinY,
                      annotations);
}

/**
 * Set them mode to y-maximum and create the redo/undo instances.
 *
 * @param newMaxY
 *     New maximum y-value of the annotation
 * @param annotations
 *     Annotations that receive this new bounds (browser tabs only).
 */
void
AnnotationRedoUndoCommand::setBoundsMaxY2D(const float newMaxY,
                                           const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::BOUNDS_2D_SINGLE;
    setDescription("Bounds 2D Y-Max");
    setBounds2DHelper(BoundsType2D::MAX_Y,
                      newMaxY,
                      annotations);
}

/**
 * Helper for bounds 2D
 *
 * @param boundsType
 *     The bounds value bing set
 * @param value
 *     New value
 * @param annotation
 *     Annotations that have bounds changed.
 */
void
AnnotationRedoUndoCommand::setBounds2DHelper(const BoundsType2D boundsType,
                                             const float value,
                                             const std::vector<Annotation*>& annotations)
{
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        AnnotationBrowserTab* browserTabAnnotation = dynamic_cast<AnnotationBrowserTab*>(annotation);
        if (browserTabAnnotation != NULL) {
            AnnotationBrowserTab* redoAnnotation = dynamic_cast<AnnotationBrowserTab*>(browserTabAnnotation->clone());
            CaretAssert(redoAnnotation);
            float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
            redoAnnotation->getBounds2D(minX, maxX, minY, maxY);
            
            switch (boundsType) {
                case BoundsType2D::MIN_X:
                    minX = value;
                    break;
                case BoundsType2D::MAX_X:
                    maxX = value;
                    break;
                case BoundsType2D::MIN_Y:
                    minY = value;
                    break;
                case BoundsType2D::MAX_Y:
                    maxY = value;
                    break;
            }
            
            redoAnnotation->setBounds2D(minX, maxX, minY, maxY);
            
            Annotation* undoAnnotation = browserTabAnnotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
        else {
            CaretLogWarning("Annotation for bounds setting must be a Browser Tab Annotation");
        }
    }
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
        AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(redoAnnotation);
        AnnotationOneCoordinateShape* twoDimShape = dynamic_cast<AnnotationOneCoordinateShape*>(redoAnnotation);
        
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
        AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(redoAnnotation);
        
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
    if (numAnnotations == 1) {
        setDescription("Reshape Annotation");
    }
    else {
        setDescription("Reshape Annotations");
    }
    
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
        AnnotationTwoCoordinateShape* oneDimShape = dynamic_cast<AnnotationTwoCoordinateShape*>(redoAnnotation);
        
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
 * Set them mode to second coordinate and create the redo/undo instances.
 *
 * @param coordinate
 *     New value of the coordinate.
 * @param annotations
 *     Annotations that receive this new coordinate.
 */
void
AnnotationRedoUndoCommand::setModeCoordinateMulti(const std::vector<std::unique_ptr<const AnnotationCoordinate>>& coordinates,
                                                  const std::vector<Annotation*>& annotations)
{
    std::vector<AnnotationMultiCoordinateShape*> multiCoordAnns;
    for (auto& ann : annotations) {
        CaretAssert(ann);
        AnnotationMultiCoordinateShape* mc = ann->castToMultiCoordinateShape();
        if (mc != NULL) {
            multiCoordAnns.push_back(mc);
        }
        else {
            CaretLogWarning("Attempting to apply set multi-coords on annotation that is does not support multi-coordinates: "
                            + ann->toString());
        }
    }
    if (multiCoordAnns.empty()) {
        CaretLogWarning("No multi-coord annotations for setting coordinates");
        return;
    }
    
    m_mode        = AnnotationRedoUndoCommandModeEnum::COORDINATE_MULTI;
    setDescription("Set coordinates for multi-coordinate annotations");
    
    for (auto& ann : multiCoordAnns) {
        CaretAssert(ann);
        AnnotationMultiCoordinateShape* redoAnnotation = dynamic_cast<AnnotationMultiCoordinateShape*>(ann->clone());
        ann->replaceAllCoordinates(coordinates);
        Annotation* undoAnnotation = ann->clone();
        AnnotationMemento* am = new AnnotationMemento(ann,
                                                      redoAnnotation,
                                                      undoAnnotation);
        m_annotationMementos.push_back(am);
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
AnnotationRedoUndoCommand::setModeLineWidth(const float newLineWidth,
                                            const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::LINE_WIDTH_FOREGROUND;
    setDescription("Line Width");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        Annotation* redoAnnotation = annotation->clone();
        redoAnnotation->setLineWidthPercentage(newLineWidth);
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
 * Set the mode to line color and create the undo/redo instances.
 *
 * @param color
 *     The new line color.
 * @param customColor
 *     The new custom line color.
 * @param annotations
 *     Annotation that receive this new color.
 */
void
AnnotationRedoUndoCommand::setModeColorLine(const CaretColorEnum::Enum color,
                            const float customColor[4],
                            const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::COLOR_FOREGROUND;
    setDescription("Line Color");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        
        Annotation* redoAnnotation = annotation->clone();
        redoAnnotation->setLineColor(color);
        redoAnnotation->setCustomLineColor(customColor);
        Annotation* undoAnnotation = annotation->clone();
        
        AnnotationMemento* am = new AnnotationMemento(annotation,
                                                       redoAnnotation,
                                                       undoAnnotation);
        
        m_annotationMementos.push_back(am);
    }
}

/**
 * Set the mode to add new annotation and create the undo/redo instances.
 *
 * @param annotationFile
 *     File to which annotation is pasted.
 * @param annotations
 *     Annotation that are pasted.
 */
void
AnnotationRedoUndoCommand::setModeCreateAnnotation(AnnotationFile* annotationFile,
                                                   Annotation* annotation)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::CREATE_ANNOTATION;
    setDescription("Create Annotation");
    
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    
    /*
     * NOTE: We only need the pointer since the file containing
     * the annotation will handle delete/undelete of the
     * annotation.  If we don't use NULL for the redo and
     * undo annotations, copies of the annotation would be
     * needed since the AnnotationMemento will delete
     * the redo and undo annotations when it is deleted.
     */
    AnnotationMemento* am = new AnnotationMemento(annotationFile,
                                                  annotation,
                                                  NULL,
                                                  NULL);
    
    m_annotationMementos.push_back(am);
}


/**
 * Set the mode to cut annotations and create the undo/redo instances.
 *
 * @param annotations
 *     Annotation that are cut.
 */
void
AnnotationRedoUndoCommand::setModeCutAnnotations(const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::CUT_ANNOTATION;
    setDescription("Cut Annotation");
    
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
 * Set the mode to delete annotations and create the undo/redo instances.
 *
 * @param annotations
 *     Annotation that are deleted.
 */
void
AnnotationRedoUndoCommand::setModeDeleteAnnotations(const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::DELETE_ANNOTATIONS;
    if (annotations.size() > 1) {
        setDescription("Delete Annotations");
    }
    else {
        setDescription("Delete Annotation");
    }
    
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
 * Set the mode to move annotations into a user group.
 *
 * @param annotationGroupKey
 *     Group key for source of annotations.
 * @param annotations
 *     Annotations that will be moved to a user group.
 */
void
AnnotationRedoUndoCommand::setModeGroupingGroupAnnotations(const AnnotationGroupKey& annotationGroupKey,
                                                           const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::GROUPING_GROUP;
    setDescription("Group Annotations");
    m_annotationGroupMemento = new AnnotationGroupMemento(annotationGroupKey,
                                                          annotations);
}


/**
 * Set the mode to remove annotations from a user group.
 *
 * @param annotationGroupKey
 *     Group key of user group containing annotations.
 */
void
AnnotationRedoUndoCommand::setModeGroupingUngroupAnnotations(const AnnotationGroupKey& annotationGroupKey)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::GROUPING_UNGROUP;
    setDescription("Ungroup Annotations");
    std::vector<Annotation*> emptyAnnotations;
    m_annotationGroupMemento = new AnnotationGroupMemento(annotationGroupKey,
                                                          emptyAnnotations);
}

/**
 * Set the mode to move annotations back to a previous user group.
 *
 * @param annotationGroupKey
 *     Group key of previous user group.
 */
void
AnnotationRedoUndoCommand::setModeGroupingRegroupAnnotations(const AnnotationGroupKey& annotationGroupKey)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::GROUPING_REGROUP;
    setDescription("Regroup Annotations");
    std::vector<Annotation*> emptyAnnotations;
    m_annotationGroupMemento = new AnnotationGroupMemento(annotationGroupKey,
                                                          emptyAnnotations);
}

/**
 * Set the mode to paste annotations and create the undo/redo instances.
 *
 * @param annotationFile
 *     File to which annotation is pasted.
 * @param annotations
 *     Annotation that are pasted.
 */
void
AnnotationRedoUndoCommand::setModePasteAnnotation(AnnotationFile* annotationFile,
                                                  Annotation* annotation)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::PASTE_ANNOTATION;
    setDescription("Paste Annotation");
    
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    
    /*
     * NOTE: We only need the pointer since the file containing
     * the annotation will handle delete/undelete of the
     * annotation.  If we don't use NULL for the redo and
     * undo annotations, copies of the annotation would be
     * needed since the AnnotationMemento will delete
     * the redo and undo annotations when it is deleted.
     */
    AnnotationMemento* am = new AnnotationMemento(annotationFile,
                                                  annotation,
                                                  NULL,
                                                  NULL);
    
    m_annotationMementos.push_back(am);
}

/**
 * Set the mode to duplicate annotations and create the undo/redo instances.
 *
 * @param annotationFile
 *     File to which annotation is pasted.
 * @param annotations
 *     The new anntotation duplicated from existing annotation.
 */
void
AnnotationRedoUndoCommand::setModeDuplicateAnnotation(AnnotationFile* annotationFile,
                                                  Annotation* annotation)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATION;
    setDescription("Duplicate Annotation");
    
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    
    /*
     * NOTE: We only need the pointer since the file containing
     * the annotation will handle delete/undelete of the
     * annotation.  If we don't use NULL for the redo and
     * undo annotations, copies of the annotation would be
     * needed since the AnnotationMemento will delete
     * the redo and undo annotations when it is deleted.
     */
    AnnotationMemento* am = new AnnotationMemento(annotationFile,
                                                  annotation,
                                                  NULL,
                                                  NULL);
    
    m_annotationMementos.push_back(am);
}

/**
 * Set the mode to duplicate annotations and create the undo/redo instances.
 *
 * @param fileAndAnnotations
 *       Pairs with file and annotation
 */
void
AnnotationRedoUndoCommand::setModeDuplicateAnnotations(std::vector<std::pair<AnnotationFile*, Annotation*>>& fileAndAnnotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::DUPLICATE_ANNOTATIONS;
    setDescription("Duplicate Annotations");
    
    CaretAssert(fileAndAnnotations.size() > 0);
    
    for (auto& fileAnn : fileAndAnnotations) {
        AnnotationFile* file(fileAnn.first);
        Annotation*     annotation(fileAnn.second);
        CaretAssert(file);
        CaretAssert(annotation);
        
        /*
         * NOTE: We only need the pointer since the file containing
         * the annotation will handle delete/undelete of the
         * annotation.  If we don't use NULL for the redo and
         * undo annotations, copies of the annotation would be
         * needed since the AnnotationMemento will delete
         * the redo and undo annotations when it is deleted.
         */
        AnnotationMemento* am = new AnnotationMemento(file,
                                                      annotation,
                                                      NULL,
                                                      NULL);
        
        m_annotationMementos.push_back(am);
    }
}


/**
 * Set the mode to insert a polyline vertex
 *
 * @param insertAfterCoordinateIndex
 *     Insert the new coordinate after this coordinate
 * @param normalizedDistanceToNextCoordinate
 *     Normalized Distance to next coordinate
 * @param surfaceSpaceNewVertexIndex
 *     Vertex index for a surface space annotation
 * @param annotation
 *     Annotation that receives a new vertex
 */
void
AnnotationRedoUndoCommand::setModeMultiCoordAnnInsertCoordinate(const int32_t insertAfterCoordinateIndex,
                                                                const float normalizedDistanceToNextCoordinate,
                                                                const int32_t surfaceSpaceNewVertexIndex,
                                                                Annotation* annotation)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::MULTI_COORD_INSERT_COORDINATE;
    setDescription("Insert Polyline Coordinate");
    
    Annotation* redoAnnotation = annotation->clone();
    CaretAssert(redoAnnotation);
    
    AnnotationMultiCoordinateShape* multiCoordShape(redoAnnotation->castToMultiCoordinateShape());
    CaretAssert(multiCoordShape);
    multiCoordShape->insertCoordinate(insertAfterCoordinateIndex,
                                      surfaceSpaceNewVertexIndex,
                                      normalizedDistanceToNextCoordinate);
    
    Annotation* undoAnnotation = annotation->clone();
    AnnotationMemento* am = new AnnotationMemento(annotation,
                                                  redoAnnotation,
                                                  undoAnnotation);
    m_annotationMementos.push_back(am);
}

/**
 * Set the mode to remove a polyline vertex
 *
 * @param coordinateIndex
 *     Remove the vertex at this index
 * @param annotation
 *     Annotation that has vertex removed
 */
void
AnnotationRedoUndoCommand::setModeMultiCoordAnnRemoveCoordinate(const int32_t coordinateIndex,
                                                                Annotation* annotation)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::MULTI_COORD_REMOVE_COORDINATE;
    setDescription("Remove Polyline Coordinate");
    
    Annotation* redoAnnotation = annotation->clone();
    CaretAssert(redoAnnotation);
    
    AnnotationMultiCoordinateShape* multiCoordShape(redoAnnotation->castToMultiCoordinateShape());
    CaretAssert(multiCoordShape);
    multiCoordShape->removeCoordinateAtIndex(coordinateIndex);
    
    Annotation* undoAnnotation = annotation->clone();
    AnnotationMemento* am = new AnnotationMemento(annotation,
                                                  redoAnnotation,
                                                  undoAnnotation);
    m_annotationMementos.push_back(am);
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
        
        AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);
        AnnotationTwoCoordinateShape* oneDimAnn = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation);
        if (twoDimAnn != NULL) {
            AnnotationOneCoordinateShape* redoAnnotation = dynamic_cast<AnnotationOneCoordinateShape*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setRotationAngle(newRotationAngle);
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
        else if (oneDimAnn != NULL) {
            int32_t viewport[4] = { 0, 0, 0, 0 };
            bool viewportValid = false;
            switch (oneDimAnn->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::CHART:
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case  AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                {
                    const int tabIndex = oneDimAnn->getTabIndex();
                    EventGetViewportSize vpSizeEvent(EventGetViewportSize::MODE_TAB_AFTER_MARGINS_INDEX,
                                                     tabIndex);
                    EventManager::get()->sendEvent(vpSizeEvent.getPointer());
                    if (vpSizeEvent.isViewportSizeValid()) {
                        vpSizeEvent.getViewportSize(viewport);
                        viewportValid = true;
                    }
                }
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                {
                    const int windowIndex = oneDimAnn->getWindowIndex();
                    EventGetViewportSize vpSizeEvent(EventGetViewportSize::MODE_WINDOW_INDEX,
                                                     windowIndex);
                    EventManager::get()->sendEvent(vpSizeEvent.getPointer());
                    if (vpSizeEvent.isViewportSizeValid()) {
                        vpSizeEvent.getViewportSize(viewport);
                        viewportValid = true;
                    }
                }
                    break;
            }
            
            if (viewportValid) {
                const float vpWidth  = viewport[2];
                const float vpHeight = viewport[3];
                
                float annOneX = 0.0;
                float annOneY = 0.0;
                float annTwoX = 0.0;
                float annTwoY = 0.0;
                oneDimAnn->getStartCoordinate()->getViewportXY(vpWidth, vpHeight, annOneX, annOneY);
                oneDimAnn->getEndCoordinate()->getViewportXY(vpWidth, vpHeight, annTwoX, annTwoY);
                
                const bool rotateAroundMiddleFlag = true;
                if (rotateAroundMiddleFlag) {

                    AnnotationTwoCoordinateShape* redoAnnotation = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation->clone());
                    CaretAssert(redoAnnotation);
                    redoAnnotation->setRotationAngle(vpWidth, vpHeight, newRotationAngle);
                    Annotation* undoAnnotation = annotation->clone();
                    AnnotationMemento* am = new AnnotationMemento(annotation,
                                                                  redoAnnotation,
                                                                  undoAnnotation);
                    m_annotationMementos.push_back(am);
                }
                else {
                    const float vpOneXYZ[3] = { annOneX, annOneY, 0.0 };
                    const float vpTwoXYZ[3] = { annTwoX, annTwoY, 0.0 };
                    const float length = MathFunctions::distance3D(vpOneXYZ, vpTwoXYZ);
                    const float angleRadians = MathFunctions::toRadians(-newRotationAngle);
                    const float dy = length * std::sin(angleRadians);
                    const float dx = length * std::cos(angleRadians);
                    annTwoX = annOneX + dx;
                    annTwoY = annOneY + dy;
                    
                    
                    AnnotationTwoCoordinateShape* redoAnnotation = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation->clone());
                    CaretAssert(redoAnnotation);
                    redoAnnotation->getEndCoordinate()->setXYZFromViewportXYZ(vpWidth, vpHeight, annTwoX, annTwoY);
                    Annotation* undoAnnotation = annotation->clone();
                    AnnotationMemento* am = new AnnotationMemento(annotation,
                                                                  redoAnnotation,
                                                                  undoAnnotation);
                    m_annotationMementos.push_back(am);
                }
            }
        }
    }
    
}

/**
 * Set the mode to stacking order and created the undo/redo instances
 *
 * @param newStackingOrder
 *     New stacking order value
 * @param annotations
 *     Annotations that receive the new stacking order
 */
void
AnnotationRedoUndoCommand::setModeStackingOrderBrowserTab(const int32_t newStackingOrder,
                                                          const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::STACKING_ORDER_BROWSER_TAB;
    setDescription("Stacking Order");
    
    for (auto ann : annotations) {
        CaretAssert(ann);
        Annotation* redoAnnotation = ann->clone();
        CaretAssert(redoAnnotation);
        redoAnnotation->setStackingOrder(newStackingOrder);
        
        Annotation* undoAnnotation = ann->clone();
        AnnotationMemento* am = new AnnotationMemento(ann,
                                                      redoAnnotation,
                                                      undoAnnotation);
        m_annotationMementos.push_back(am);
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
        
        AnnotationText* redoAnnotation = NULL;
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                break;
            case AnnotationTypeEnum::IMAGE:
                break;
            case AnnotationTypeEnum::LINE:
                break;
            case AnnotationTypeEnum::OVAL:
                break;
            case AnnotationTypeEnum::POLYGON:
                break;
            case AnnotationTypeEnum::POLYLINE:
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::TEXT:
                redoAnnotation = dynamic_cast<AnnotationText*>(annotation->clone());
                break;
        }
        
        /*
         * This method should only get call for text-type annotations.
         */
        CaretAssert(redoAnnotation);
        
        if (redoAnnotation != NULL) {
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
 * Set the mode to text color and create the undo/redo instances.
 *
 * @param color
 *     The color enum.
 * @param customColor
 *     The custom color components.
 * @param annotations
 *     Annotation that receive this new color.
 */
void
AnnotationRedoUndoCommand::setModeTextColor(const CaretColorEnum::Enum color,
                                            const float customColor[4],
                                            const std::vector<Annotation*>& annotations)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::TEXT_COLOR;
    setDescription("Text Color");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        AnnotationFontAttributesInterface* fontStyleAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
        if (fontStyleAnn != NULL) {
            Annotation* redoAnnotation = annotation->clone();
            AnnotationFontAttributesInterface* redoAnnotationFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(redoAnnotation);
            CaretAssert(redoAnnotationFontStyle);
            redoAnnotationFontStyle->setTextColor(color);
            redoAnnotationFontStyle->setCustomTextColor(customColor);
            
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
        
        AnnotationFontAttributesInterface* fontStyleAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
        if (fontStyleAnn != NULL) {
            Annotation* redoAnnotation = annotation->clone();
            AnnotationFontAttributesInterface* redoAnnotationFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(redoAnnotation);
            CaretAssert(redoAnnotationFontStyle);
            redoAnnotationFontStyle->setBoldStyleEnabled(newStatus);
            
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
        
        AnnotationFontAttributesInterface* fontStyleAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
        if (fontStyleAnn != NULL) {
            Annotation* redoAnnotation = annotation->clone();
            AnnotationFontAttributesInterface* redoAnnotationFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(redoAnnotation);
            CaretAssert(redoAnnotationFontStyle);
            redoAnnotationFontStyle->setItalicStyleEnabled(newStatus);
            
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
AnnotationRedoUndoCommand::setModeTextFontName(const AnnotationTextFontNameEnum::Enum newFontName,
                                               const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_NAME;
    setDescription("Text Font Name");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        AnnotationFontAttributesInterface* fontStyleAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
        if (fontStyleAnn != NULL) {
            Annotation* redoAnnotation = annotation->clone();
            AnnotationFontAttributesInterface* redoAnnotationFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(redoAnnotation);
            CaretAssert(redoAnnotationFontStyle);
            redoAnnotationFontStyle->setFont(newFontName);
            
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
 * @param newFontPointSize
 *     New font point size
 * @param annotations
 *     Annotation that receive this font size
 */
void
AnnotationRedoUndoCommand::setModeTextFontPointSize(const AnnotationTextFontPointSizeEnum::Enum newFontPointSize,
                                                    const std::vector<Annotation*>& annotations)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_POINT_SIZE;
    setDescription("Text Font Point Size");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            AnnotationPointSizeText* redoAnnotation = dynamic_cast<AnnotationPointSizeText*>(annotation->clone());
            CaretAssert(redoAnnotation);
            redoAnnotation->setFontPointSize(newFontPointSize);
            
            Annotation* undoAnnotation = annotation->clone();
            AnnotationMemento* am = new AnnotationMemento(annotation,
                                                          redoAnnotation,
                                                          undoAnnotation);
            m_annotationMementos.push_back(am);
        }
    }
}

/**
 * Set the mode to font percent size and create the undo/redo instances.
 *
 * @param newFontPercentSize
 *     New font percentage size
 * @param annotations
 *     Annotation that receive this font size
 */
void
AnnotationRedoUndoCommand::setModeTextFontPercentSize(const float newFontPercentSize,
                                                      const std::vector<Annotation*>& annotations,
                                                      const float surfaceSpaceRowCount)
{
    m_mode        = AnnotationRedoUndoCommandModeEnum::TEXT_FONT_PERCENT_SIZE;
    setDescription("Text Font Percent Size");
    
    for (std::vector<Annotation*>::const_iterator iter = annotations.begin();
         iter != annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        AnnotationFontAttributesInterface* fontStyleAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
        if (fontStyleAnn != NULL) {
            Annotation* redoAnnotation = annotation->clone();
            AnnotationFontAttributesInterface* redoAnnotationFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(redoAnnotation);
            CaretAssert(redoAnnotationFontStyle);
            
            float percentSize = newFontPercentSize;
            switch (redoAnnotation->getCoordinateSpace()) {
                case AnnotationCoordinateSpaceEnum::CHART:
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    percentSize *= surfaceSpaceRowCount;
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    percentSize *= surfaceSpaceRowCount;
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case  AnnotationCoordinateSpaceEnum::WINDOW:
                    break;
            }
            
            redoAnnotationFontStyle->setFontPercentViewportSize(percentSize);
            
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
        
        AnnotationFontAttributesInterface* fontStyleAnn = dynamic_cast<AnnotationFontAttributesInterface*>(annotation);
        if (fontStyleAnn != NULL) {
            Annotation* redoAnnotation = annotation->clone();
            AnnotationFontAttributesInterface* redoAnnotationFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(redoAnnotation);
            CaretAssert(redoAnnotationFontStyle);
            redoAnnotationFontStyle->setUnderlineStyleEnabled(newStatus);
            
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

        AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);
        if (twoDimAnn != NULL) {
            AnnotationOneCoordinateShape* redoAnnotation = dynamic_cast<AnnotationOneCoordinateShape*>(annotation->clone());
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
        
        AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);
        if (twoDimAnn != NULL) {
            AnnotationOneCoordinateShape* redoAnnotation = dynamic_cast<AnnotationOneCoordinateShape*>(annotation->clone());
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
 * Set the mode to and process an annotation stacking order change (but NOT for browser tab annotations)
 *
 * @param annotations
 *     The annotations that are reordered
 * @param stackingOrders
 *     New stacking orders for each of the annotations
 * @param orderType
 *     Type of ordering
 */
void
AnnotationRedoUndoCommand::setModeStackingOrderAnnotations(const std::vector<Annotation*>& annotations,
                                                           const std::vector<float>& stackingOrders,
                                                           const AnnotationStackingOrderTypeEnum::Enum orderType)
{
    m_mode = AnnotationRedoUndoCommandModeEnum::STACKING_ORDER_ANNOTATIONS;
    setDescription(AnnotationStackingOrderTypeEnum::toGuiName(orderType));
    
    CaretAssert(annotations.size() == stackingOrders.size());
    
    const int32_t numAnn = static_cast<int32_t>(annotations.size());
    for (int32_t i = 0; i < numAnn; i++) {
        CaretAssertVectorIndex(annotations, i);
        CaretAssertVectorIndex(stackingOrders, i);

        const float newZ = stackingOrders[i];
        
        Annotation* redoAnnotation = annotations[i]->clone();
        CaretAssert(redoAnnotation);
        AnnotationTwoCoordinateShape* oneDimAnn = redoAnnotation->castToTwoCoordinateShape();
        AnnotationOneCoordinateShape* twoDimAnn = redoAnnotation->castToOneCoordinateShape();
        if (oneDimAnn != NULL) {
            float xyz[3];
            oneDimAnn->getStartCoordinate()->getXYZ(xyz);
            xyz[2] = newZ;
            oneDimAnn->getStartCoordinate()->setXYZ(xyz);
            oneDimAnn->getEndCoordinate()->getXYZ(xyz);
            xyz[2] = newZ;
            oneDimAnn->getEndCoordinate()->setXYZ(xyz);
        }
        else if (twoDimAnn != NULL) {
            float xyz[3];
            twoDimAnn->getCoordinate()->getXYZ(xyz);
            xyz[2] = newZ;
            twoDimAnn->getCoordinate()->setXYZ(xyz);
        }
        else {
            CaretAssert(0);
        }
        
        Annotation* undoAnnotation = annotations[i]->clone();
        
        AnnotationMemento* am = new AnnotationMemento(annotations[i],
                                                      redoAnnotation,
                                                      undoAnnotation);
        m_annotationMementos.push_back(am);
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



