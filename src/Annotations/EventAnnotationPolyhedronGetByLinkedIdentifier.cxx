
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER_DECLARE__
#include "EventAnnotationPolyhedronGetByLinkedIdentifier.h"
#undef __EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationPolyhedronGetByLinkedIdentifier 
 * \brief Get a polyhedron with the matching linked identifier
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param annotationFile
 *    Optional annotation file (annotation may be in any file if this is NULL)
 * @param polyhedronType
 *    Type of polyhedron for matching
 * @param linkedIdentifier
 *    Linked identifier that must match
 */
EventAnnotationPolyhedronGetByLinkedIdentifier::EventAnnotationPolyhedronGetByLinkedIdentifier(const AnnotationFile* annotationFile,
                                                                                               const AnnotationPolyhedronTypeEnum::Enum polyhedronType,
                                                                                               const AString& linkedIdentifier)
: Event(EventTypeEnum::EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER),
m_annotationFile(annotationFile),
m_polyhedronType(polyhedronType),
m_linkedIdentifier(linkedIdentifier)
{
    CaretAssert(m_polyhedronType != AnnotationPolyhedronTypeEnum::INVALID);
    CaretAssert( ! m_linkedIdentifier.isEmpty());
}

/**
 * Destructor.
 */
EventAnnotationPolyhedronGetByLinkedIdentifier::~EventAnnotationPolyhedronGetByLinkedIdentifier()
{
}

/**
 * @return Annotation file that must contain the annotation with matching linked identifier
 * If NULL, any file is allowed.
 */
const AnnotationFile*
EventAnnotationPolyhedronGetByLinkedIdentifier::getAnnotationFile() const
{
    return m_annotationFile;
}

/**
 * @return Type of polyhedron searching for
 */
AnnotationPolyhedronTypeEnum::Enum
EventAnnotationPolyhedronGetByLinkedIdentifier::getPolyhedronType() const
{
    return m_polyhedronType;
}

/**
 * @return Linked identifier searching for
 */
AString
EventAnnotationPolyhedronGetByLinkedIdentifier::getLinkedIdentifier() const
{
    return m_linkedIdentifier;
}

/**
 * @return Matching polyhedron that was found or NULL if not found
 */
AnnotationPolyhedron*
EventAnnotationPolyhedronGetByLinkedIdentifier::getPolyhedron() const
{
    return m_annotationPolyhedron;
}

/**
 * Set the matching polyhedron that was found
 * @param polyhedron
 */
void
EventAnnotationPolyhedronGetByLinkedIdentifier::setPolyhedron(AnnotationPolyhedron* polyhedron)
{
    if (m_annotationPolyhedron == NULL) {
        m_annotationPolyhedron = polyhedron;
    }
    else {
        CaretLogSevere("Program Error: More than one polyhedron with type="
                       + AnnotationPolyhedronTypeEnum::toName(m_polyhedronType)
                       + " was found with linked identifier="
                       + m_linkedIdentifier);
    }
}
