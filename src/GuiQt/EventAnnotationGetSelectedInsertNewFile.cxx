
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE_DECLARE__
#include "EventAnnotationGetSelectedInsertNewFile.h"
#undef __EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAnnotationGetSelectedInsertNewFile 
 * \brief Get annotation file selected in the insert new section of the toolbar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param userInputMode
 *    The user input mode
 */
EventAnnotationGetSelectedInsertNewFile::EventAnnotationGetSelectedInsertNewFile(const UserInputModeEnum::Enum userInputMode)
: Event(EventTypeEnum::EVENT_ANNOTATION_GET_SELECTED_INSERT_NEW_FILE),
m_userInputMode(userInputMode)
{
    
}

/**
 * Destructor.
 */
EventAnnotationGetSelectedInsertNewFile::~EventAnnotationGetSelectedInsertNewFile()
{
}

/**
 * @return The user input mode
 */
UserInputModeEnum::Enum
EventAnnotationGetSelectedInsertNewFile::getUserInputMode() const
{
    return m_userInputMode;
}

/**
 * @return The annotation file
 */
AnnotationFile*
EventAnnotationGetSelectedInsertNewFile::getAnnotationFile() const
{
    return m_annotationFile;
}

/**
 * Set the annotation file
 * @param annotationFile
 *    The annotation file
 */
void
EventAnnotationGetSelectedInsertNewFile::setAnnotationFile(AnnotationFile* annotationFile)
{
    m_annotationFile = annotationFile;
}

