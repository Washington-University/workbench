
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __ANNOTATION_SAMPLE_META_DATA_DECLARE__
#include "AnnotationSampleMetaData.h"
#undef __ANNOTATION_SAMPLE_META_DATA_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AnnotationSampleMetaData 
 * \brief Wraps GIFTI metadata for use as metadata by an annotation sample
 * \ingroup Annotations
 */

/**
 * Constructor.
 */
AnnotationSampleMetaData::AnnotationSampleMetaData()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
AnnotationSampleMetaData::~AnnotationSampleMetaData()
{
}

/**
 * @return All valid values for Allen Tissue Type
 */
std::vector<AString> 
AnnotationSampleMetaData::getAllValidAllenTissueTypeValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("BS");
    values.push_back("CB");
    values.push_back("CX");

    return values;
}

/**
 * @return All valid values for Hemisphere
 */
std::vector<AString>
AnnotationSampleMetaData::getAllValidHemisphereValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("left");
    values.push_back("right");
    values.push_back("both");

    return values;
}


/**
 * @return All valid values for Location
 */
std::vector<AString>
AnnotationSampleMetaData::getAllValidLocationValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("Actual");
    values.push_back("Desired");

    return values;
}

/**
 * @return All valid values for Sample Type
 */

std::vector<AString> AnnotationSampleMetaData::getAllValidSampleTypeValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("Slap polyhedron");
    values.push_back("polygon");

    return values;
}

/**
 * @return All valid values for Slab Face
 */
std::vector<AString>
AnnotationSampleMetaData::getAllValidSlabFaceValues()
{
    std::vector<AString> values;
    
    values.push_back("");
    values.push_back("Anterior");
    values.push_back("Posterior");
    values.push_back("Inferior");
    values.push_back("Superior");
    values.push_back("Left");
    values.push_back("Right");
    values.push_back("Other");

    return values;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
AnnotationSampleMetaData::AnnotationSampleMetaData(const AnnotationSampleMetaData& obj)
: CaretObject(obj)
{
    this->copyHelperAnnotationSampleMetaData(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
AnnotationSampleMetaData&
AnnotationSampleMetaData::operator=(const AnnotationSampleMetaData& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperAnnotationSampleMetaData(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
AnnotationSampleMetaData::copyHelperAnnotationSampleMetaData(const AnnotationSampleMetaData& obj)
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationSampleMetaData::toString() const
{
    return "AnnotationSampleMetaData";
}

