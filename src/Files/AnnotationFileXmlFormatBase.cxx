
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

#define __ANNOTATION_FILE_XML_FORMAT_BASE_DECLARE__
#include "AnnotationFileXmlFormatBase.h"
#undef __ANNOTATION_FILE_XML_FORMAT_BASE_DECLARE__

#include <QFileInfo>

#include "AnnotationFile.h"
#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationFileXmlFormatBase 
 * \brief Base class for Annotation File XML reader/writer.
 * \ingroup Files
 *
 * This class is the base class for the Annotation File XML format
 * reader and writer.  It contains XML tags used by the reader and 
 * writer.
 */

/**
 * Constructor.
 */
AnnotationFileXmlFormatBase::AnnotationFileXmlFormatBase()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
AnnotationFileXmlFormatBase::~AnnotationFileXmlFormatBase()
{
}

/**
 * Set the annotation file directory from annotation file name
 * @param annotationFileName
 *    Name of annotation file
 */
void
AnnotationFileXmlFormatBase::setAnnotationFileDirectory(const AString& annotationFileName)
{
    QFileInfo fileInfo(annotationFileName);
    m_annotationFileDirectory = fileInfo.absoluteDir();
}

/**
 * @return The annotation file's directory
 */
const QDir&
AnnotationFileXmlFormatBase::getAnnotationFileDirectory() const
{
    return m_annotationFileDirectory;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationFileXmlFormatBase::toString() const
{
    return "AnnotationFileXmlFormatBase";
}

