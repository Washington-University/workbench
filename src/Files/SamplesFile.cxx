
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

#include "Annotation.h"
#include "AnnotationMetaData.h"
#include "AnnotationMetaDataNames.h"
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

/**
 * Generate a sample number for the given slab ID.  The sample number will be the first  positive integer that is not
 * used by an annotation that uses the slabID.
 * @param annotationFiles
 *    File to search for slabID and samle numbers
 * @param slabID
 *    The slab ID
 */
AString
SamplesFile::generateSampleNumberFromSlabID(std::vector<SamplesFile*> samplesFiles,
                                               const AString& slabID)
{
    if (slabID.isEmpty()) {
        return "";
    }
    
    
    /*
     * Get sample numbers used by a SlabID
     */
    std::set<int32_t> existingSampleNumbers;
    for (const SamplesFile* annFile : samplesFiles) {
        std::vector<Annotation*> allAnnotations;
        annFile->getAllAnnotations(allAnnotations);
        
        for (const Annotation* ann : allAnnotations) {
            CaretAssert(ann);
            if (ann->getType() == AnnotationTypeEnum::POLYHEDRON) {
                if (slabID == ann->getMetaData()->get(AnnotationMetaDataNames::SAMPLES_ALLEN_SLAB_NUMBER)) {
                    const int32_t sampleNumber(ann->getMetaData()->getInt(AnnotationMetaDataNames::SAMPLES_SAMPLE_NUMBER));
                    existingSampleNumbers.insert(sampleNumber);
                }
            }
        }
    }
    
    /*
     * Find an unused sample number
     */
    AString sampleNumberOut;
    for (int32_t i = 1; i < 9999999; i++) {
        if (existingSampleNumbers.find(i) == existingSampleNumbers.end()) {
            sampleNumberOut = AString::number(i);
            break;
        }
    }
    return sampleNumberOut;
}

