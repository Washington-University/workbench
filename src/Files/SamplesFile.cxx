
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
#include "AnnotationPolyhedron.h"
#include "AnnotationSampleMetaData.h"
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
                const AnnotationPolyhedron* polyhedron(ann->castToPolyhedron());
                CaretAssert(polyhedron);
                const AnnotationSampleMetaData* sampleMetaData(polyhedron->getSampleMetaData());
                if (slabID == sampleMetaData->getAllenSlabNumber()) {
                    const AString sampleNumberText(sampleMetaData->getSampleNumber());
                    if ( ! sampleNumberText.isEmpty()) {
                        bool validFlag(false);
                        const int32_t sampleNumber(sampleNumberText.toInt(&validFlag));
                        if (validFlag) {
                            existingSampleNumbers.insert(sampleNumber);
                        }
                    }
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

/**
 * @return All annotations for drawing (may be override by subclass
 * to change order of drawing).  This method returns the prospective samples
 * followed by the retrospective samples so that retrospective samples are drawn on
 * top of prospective samples.
 */
std::vector<Annotation*>
SamplesFile::getAllAnnotationsForDrawing() const
{
    std::vector<Annotation*> retrospectiveSamples;
    std::vector<Annotation*> prospectiveSamples;
    std::vector<Annotation*> otherSamples;

    std::vector<Annotation*> allAnnotations;
    getAllAnnotations(allAnnotations);
    
    for (Annotation* ann : allAnnotations) {
        AnnotationPolyhedron* poly(ann->castToPolyhedron());
        if (poly != NULL) {
            switch (poly->getPolyhedronType()) {
                case AnnotationPolyhedronTypeEnum::INVALID:
                    otherSamples.push_back(ann);
                    break;
                case AnnotationPolyhedronTypeEnum::RETROSPECTIVE_SAMPLE:
                    retrospectiveSamples.push_back(ann);
                    break;
                case AnnotationPolyhedronTypeEnum::PROSPECTIVE_SAMPLE:
                    prospectiveSamples.push_back(ann);
                    break;
            }
        }
    }

    std::vector<Annotation*> annotations(otherSamples.begin(),
                                         otherSamples.end());
    annotations.insert(annotations.end(),
                       prospectiveSamples.begin(),
                       prospectiveSamples.end());
    annotations.insert(annotations.end(),
                       retrospectiveSamples.begin(),
                       retrospectiveSamples.end());

    return annotations;
}
