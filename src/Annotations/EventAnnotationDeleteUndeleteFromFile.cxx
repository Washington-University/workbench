
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

#define __EVENT_ANNOTATION_DELETE_UNDELETE_FROM_FILE_DECLARE__
#include "EventAnnotationDeleteUndeleteFromFile.h"
#undef __EVENT_ANNOTATION_DELETE_UNDELETE_FROM_FILE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationDeleteUndeleteFromFile 
 * \brief Request that an annotation be delete from or undeleted from an annotation file
 * \ingroup Annotations
 *
 * Annotations may be deleted by the user but the user may request to undelete an
 * annotation.  This event is received by an annotation file and the file will
 * then delete or undelete the annotation.  When the file receives a request to
 * delete the annotation, the file will delete the annotation from its valid
 * annotations but it does NOT destroy the annotation.  Instead, the file will
 * save the annotation so that the user can request undeletion of the annotation.
 * With this logic, the pointer can be used to request undeletion.
 */

/**
 * Constructor.
 *
 * @param mode
 *    Mode (delete/undelete)
 * @param annotation
 *    Annotation that is deleted or undeleted
 */
EventAnnotationDeleteUndeleteFromFile::EventAnnotationDeleteUndeleteFromFile(const Mode mode,
                                                                             Annotation* annotation)
: Event(EventTypeEnum::EVENT_ANNOTATION_DELETE_UNDELETE_FROM_FILE),
m_mode(mode),
m_annotation(annotation),
m_successFlag(false)
{
    CaretAssert(annotation);
}

/**
 * Destructor.
 */
EventAnnotationDeleteUndeleteFromFile::~EventAnnotationDeleteUndeleteFromFile()
{
}

/**
 * @return the mode (delete/undelete)
 */
EventAnnotationDeleteUndeleteFromFile::Mode
EventAnnotationDeleteUndeleteFromFile::getMode() const
{
    return m_mode;
}

/**
 * @return Pointer to annotation that is deleted or undeleted.
 */
Annotation*
EventAnnotationDeleteUndeleteFromFile::getAnnotation() const
{
    return m_annotation;
}

/**
 * @return True if deletion or undeletion of the annotation was successful.
 */
bool
EventAnnotationDeleteUndeleteFromFile::isSuccessful() const
{
    return m_successFlag;
}

/**
 * Set the success status for deletion or undeletion of the annotation.
 *
 * @param status
 *     New status indicating success.
 */
void
EventAnnotationDeleteUndeleteFromFile::setSuccessful(const bool status)
{
    m_successFlag = status;
}
