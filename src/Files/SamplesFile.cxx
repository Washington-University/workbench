
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

#define __SAMPLES_FILE_DECLARE__
#include "SamplesFile.h"
#undef __SAMPLES_FILE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SamplesFile 
 * \brief File for samples that are a specific annotation (polyhedron)
 * \ingroup Files
 */

/**
 * Constructor.
 */
SamplesFile::SamplesFile()
: AnnotationFile(DataFileTypeEnum::SAMPLES,
                 AnnotationFile::SAMPLES_FILE_SAVE_TO_FILE)
{
    
}

/**
 * Destructor.
 */
SamplesFile::~SamplesFile()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SamplesFile::SamplesFile(const SamplesFile& obj)
: AnnotationFile(obj)
{
    this->copyHelperSamplesFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SamplesFile&
SamplesFile::operator=(const SamplesFile& obj)
{
    if (this != &obj) {
        AnnotationFile::operator=(obj);
        this->copyHelperSamplesFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SamplesFile::copyHelperSamplesFile(const SamplesFile& /*obj*/)
{
    
}

