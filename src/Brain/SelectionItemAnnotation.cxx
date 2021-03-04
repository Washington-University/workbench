
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

#define __SELECTION_ITEM_ANNOTATION_DECLARE__
#include "SelectionItemAnnotation.h"
#undef __SELECTION_ITEM_ANNOTATION_DECLARE__

#include "AnnotationText.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::SelectionItemAnnotation 
 * \brief Contains information about a selected annotation.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemAnnotation::SelectionItemAnnotation()
: SelectionItem(SelectionItemDataTypeEnum::ANNOTATION)
{
}

/**
 * Destructor.
 */
SelectionItemAnnotation::~SelectionItemAnnotation()
{
}

/**
 * Reset this selection item.
 */
void
SelectionItemAnnotation::reset()
{
    SelectionItem::reset();
    
    /*
     * Just have pointers to the annotations.
     * We do not 'own' them.
     */
    m_annotationFile = NULL;
    m_annotation     = NULL;
    m_sizingHandle   = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE;
    m_polyLineCoordinateIndex = -1;
}

/**
 * @return Is this selected item valid?
 */
bool
SelectionItemAnnotation::isValid() const
{
    return (m_annotation != NULL);
}

/**
 * @return The selected annotation.
 */
Annotation*
SelectionItemAnnotation::getAnnotation() const
{
    return m_annotation;
}

/**
 * @return The file containing the selected annotation.
 */
AnnotationFile*
SelectionItemAnnotation::getAnnotationFile() const
{
    return m_annotationFile;
}

/**
 * @return Sizing handle selected in the selected annotation.
 */
AnnotationSizingHandleTypeEnum::Enum
SelectionItemAnnotation::getSizingHandle() const
{
    return m_sizingHandle;
}

/**
 * @return Index of poly line coordinate
 */
int32_t
SelectionItemAnnotation::getPolyLineCoordinateIndex() const
{
    return m_polyLineCoordinateIndex;
}

/**
 * @return The annotation's coordinates converted to window coordinates
 */
std::vector<Vector3D>
SelectionItemAnnotation::getAnnotationCoordsInWindowXYZ() const
{
    return m_coordsInWindowXYZ;
}


/**
 * Add a annotation to the selected annotations.
 *
 * @param annotationFile
 *     File containing the annotation.
 * @param annotation.
 *     Annotation that is added.
 * @param annotationSizingHandle
 *     Sizing handle that is selected.
 * @param polyLineCoordinateIndex
 *     Index of poly line coordinate
 * @param coordsInWindowXYZ
 *     Coordinates convertex to window XYZ
 */
void
SelectionItemAnnotation::setAnnotation(AnnotationFile* annotationFile,
                                       Annotation* annotation,
                                       const AnnotationSizingHandleTypeEnum::Enum annotationSizingHandle,
                                       const int32_t polyLineCoordinateIndex,
                                       const std::vector<Vector3D>& coordsInWindowXYZ)
{
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    m_annotationFile = annotationFile;
    m_annotation     = annotation;
    m_sizingHandle   = annotationSizingHandle;
    m_polyLineCoordinateIndex = polyLineCoordinateIndex;
    m_coordsInWindowXYZ       = coordsInWindowXYZ;
    
    if (annotation != NULL) {
        bool validateFlag(false);
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
                validateFlag = true;
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                break;
            case AnnotationTypeEnum::IMAGE:
                validateFlag = true;
                break;
            case AnnotationTypeEnum::LINE:
                validateFlag = true;
                break;
            case AnnotationTypeEnum::OVAL:
                validateFlag = true;
                break;
            case AnnotationTypeEnum::POLYGON:
                validateFlag = true;
                break;
            case AnnotationTypeEnum::POLYLINE:
                validateFlag = true;
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
            case AnnotationTypeEnum::TEXT:
                validateFlag = true;
                break;
        }
        if (validateFlag) {
            if (annotation->getNumberOfCoordinates() != static_cast<int32_t>(coordsInWindowXYZ.size())) {
                CaretLogSevere("Selection failed for annotation "
                               + annotation->toString()
                               + " with "
                               + AString::number(annotation->getNumberOfCoordinates())
                               + " coordinates but only "
                               + AString::number(coordsInWindowXYZ.size())
                               + " coordinates have valid window positions.");
            }
        }
    }
}
/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemAnnotation::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Annotation type=" + AnnotationTypeEnum::toGuiName(m_annotation->getType())
             + "   sizeHandleType=" + AnnotationSizingHandleTypeEnum::toGuiName(m_sizingHandle)
             + "   m_polyLineCoordinateIndex=" + AString::number(m_polyLineCoordinateIndex));
    
    AnnotationText* textAnn = dynamic_cast<AnnotationText*>(m_annotation);
    if (textAnn != NULL) {
        text += ("   text=" + textAnn->getText());
    }
//    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
//    text += ("Border File: " + ((borderFile != NULL) ? borderFile->getFileNameNoPath() : "INVALID") + "\n");
//    text += ("Border: " + ((border != NULL) ? border->getName() : "INVALID") + "\n");
//    text += ("Border Index: " + AString::number(borderIndex) + "\n");
//    text += ("Border Point Index: " + AString::number(borderPointIndex) + "\n");
    return text;
}
