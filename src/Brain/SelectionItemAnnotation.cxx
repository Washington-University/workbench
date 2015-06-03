
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

#include "CaretAssert.h"
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
 * Add a annotation to the selected annotations.
 *
 * @param annotationFile
 *     File containing the annotation.
 * @param annotation.
 *     Annotation that is added.
 */
void
SelectionItemAnnotation::setAnnotation(AnnotationFile* annotationFile,
                                       Annotation* annotation)
{
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    m_annotationFile = annotationFile;
    m_annotation     = annotation;
}
/**
 * Get a description of m_ object's content.
 * @return String describing m_ object's content.
 */
AString
SelectionItemAnnotation::toString() const
{
    AString text = SelectionItem::toString();
    text += ("Annotation: \n");
//    text += ("Surface: " + ((surface != NULL) ? surface->getFileNameNoPath() : "INVALID") + "\n");
//    text += ("Border File: " + ((borderFile != NULL) ? borderFile->getFileNameNoPath() : "INVALID") + "\n");
//    text += ("Border: " + ((border != NULL) ? border->getName() : "INVALID") + "\n");
//    text += ("Border Index: " + AString::number(borderIndex) + "\n");
//    text += ("Border Point Index: " + AString::number(borderPointIndex) + "\n");
    return text;
}
