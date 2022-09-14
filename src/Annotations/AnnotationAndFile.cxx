
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __ANNOTATION_AND_FILE_DECLARE__
#include "AnnotationAndFile.h"
#undef __ANNOTATION_AND_FILE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationAndFile 
 * \brief Simply contains pointer ot annotation and the file containing the annotation
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param annotation
 *     The annotation
 * @param file
 *     Annotation file containing the annotation
 * @param groupKey
 *     The group key for the annotation
 */
AnnotationAndFile::AnnotationAndFile(Annotation* annotation,
                                     AnnotationFile* file,
                                     const AnnotationGroupKey& groupKey)
: CaretObject(),
m_annotation(annotation),
m_file(file),
m_groupKey(groupKey)
{
}


/**
 * Destructor.
 */
AnnotationAndFile::~AnnotationAndFile()
{
    /* Do not delete annotation and file, we do not own them ! */
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationAndFile::AnnotationAndFile(const AnnotationAndFile& obj)
: CaretObject(obj)
{
    this->copyHelperAnnotationAndFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationAndFile&
AnnotationAndFile::operator=(const AnnotationAndFile& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationAndFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationAndFile::copyHelperAnnotationAndFile(const AnnotationAndFile& obj)
{
    m_annotation = obj.m_annotation;
    m_file       = obj.m_file;
    m_groupKey   = obj.m_groupKey;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
AnnotationAndFile::operator==(const AnnotationAndFile& obj) const
{
    if (this == &obj) {
        return true;    
    }

    /* Do not use group key in comparison */
    if ((m_annotation == obj.m_annotation)
        && (m_file == obj.m_file)) {
        return true;
    }
    return false;    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationAndFile::toString() const
{
    return "AnnotationAndFile";
}

