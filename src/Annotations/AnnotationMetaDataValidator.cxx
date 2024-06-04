
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

#define __ANNOTATION_SAMPLE_META_DATA_VALIDATOR_DECLARE__
#include "AnnotationMetaDataValidator.h"
#undef __ANNOTATION_SAMPLE_META_DATA_VALIDATOR_DECLARE__

#include "Annotation.h"
#include "AnnotationMetaData.h"
#include "AnnotationMetaDataNames.h"
#include "CaretAssert.h"
#include "HemisphereEnum.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMetaDataValidator 
 * \brief Metadata validator for an annotation sample
 * \ingroup Annotations
 */

/**
 * Constructor.
 * @param annotation
 *    Annotation that is validated
 * @param firstAllenSlabLeftHemisphereNumber
 *    First valid number for a left hemisphere in the Allen Slab Number Box
 * @param firstAllenSlabRightHemisphereNumber
 *    First valid number for a right hemisphere in the Allen Slab Number Box
 * @param lastAllenSlabRightHemisphereNumber
 *    Last valid number for a right hemisphere in the Allen Slab Number Box
 */
AnnotationMetaDataValidator::AnnotationMetaDataValidator(Annotation* annotation,
                                                                     const int32_t firstAllenSlabLeftHemisphereNumber,
                                                                     const int32_t firstAllenSlabRightHemisphereNumber,
                                                                     const int32_t lastAllenSlabRightHemisphereNumber)
: GiftiMetaDataValidatorInterface(),
m_annotation(annotation),
m_firstAllenSlabLeftHemisphereNumber(firstAllenSlabLeftHemisphereNumber),
m_firstAllenSlabRightHemisphereNumber(firstAllenSlabRightHemisphereNumber),
m_lastAllenSlabRightHemisphereNumber(lastAllenSlabRightHemisphereNumber)
{
    CaretAssert(m_annotation);
    
    AnnotationMetaData* annotationMetaData(m_annotation->getMetaData());
    annotationMetaData->getMetaDataNamesForEditor(m_allMetaDataNames,
                                                  m_requiredMetaDataNames);
}

/**
 * Destructor.
 */
AnnotationMetaDataValidator::~AnnotationMetaDataValidator()
{
}

/**
 * @return All known meta data names
 */
std::vector<AString>
AnnotationMetaDataValidator::getAllMetaDataNames() const
{
    return m_allMetaDataNames;
}

/**
 * @return All names of metadata that are required
 */
std::vector<AString>
AnnotationMetaDataValidator::getRequiredMetaDataNames() const
{
    return m_requiredMetaDataNames;
}

/**
 * Check the metadata for errors.  User must correct errors to proceed.
 * @param metadata
 *    Metadata for validation
 * @param errorMessageOut
 *    Output with description of any errors
 * @return
 *    True if there are no errors, else false.
 */
bool
AnnotationMetaDataValidator::checkMetaDataForErrors(const GiftiMetaData* metadata,
                                                          AString& errorMessageOut)
{
    CaretAssert(metadata);
    errorMessageOut.clear();
    
    for (const auto& name : m_requiredMetaDataNames) {
        const AString& value(metadata->get(name).trimmed());
        if (value.isEmpty()) {
            errorMessageOut.appendWithNewLine("   " + name);
        }
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        errorMessageOut.insert(0, "Required metadata is invalid:\n");
    }
    
    return errorMessageOut.isEmpty();
}

/**
 * Check the metadata for warnings.  User is alerted and may continue without fixing warnings.
 * @param metadata
 *    Metadata for validation
 * @param warnningMessageOut
 *    Output with description of any warnings
 * @return
 *    True if there are no warnings, else false.
 */
bool
AnnotationMetaDataValidator::checkMetaDataForWarnings(const GiftiMetaData* metadata,
                                                            AString& warningMessageOut)
{
    CaretAssert(metadata);
    warningMessageOut.clear();
    
    const AString hemisphereString(metadata->get(AnnotationMetaDataNames::SAMPLES_HEMISPHERE));
    bool validHemFlag(false);
    const HemisphereEnum::Enum hemisphere(HemisphereEnum::fromGuiName(hemisphereString,
                                                                      &validHemFlag));
    
    bool okFlag(true);
    
    if (validHemFlag) {
        const int32_t allenSlabNumber(metadata->getInt(AnnotationMetaDataNames::SAMPLES_ALLEN_SLAB_NUMBER));
        
        AString hemMsg;
        switch (hemisphere) {
            case HemisphereEnum::BOTH:
                if ((allenSlabNumber < m_firstAllenSlabLeftHemisphereNumber)
                    || (allenSlabNumber > m_lastAllenSlabRightHemisphereNumber)) {
                    hemMsg = ("Allen Slab Number should be between (inclusively) "
                              + AString::number(m_firstAllenSlabLeftHemisphereNumber)
                               + " and "
                              + AString::number(m_lastAllenSlabRightHemisphereNumber)
                              + " for BOTH hemispheres.");
                    okFlag = false;
                }
                break;
            case HemisphereEnum::LEFT:
                if ((allenSlabNumber < m_firstAllenSlabLeftHemisphereNumber)
                    || (allenSlabNumber >= m_firstAllenSlabRightHemisphereNumber)) {
                    hemMsg = ("Allen Slab Number should be between (inclusively) "
                              + AString::number(m_firstAllenSlabLeftHemisphereNumber)
                              + " and "
                              + AString::number(m_firstAllenSlabRightHemisphereNumber)
                              + " for LEFT hemispheres.");
                    okFlag = false;
                }
                break;
            case HemisphereEnum::RIGHT:
                if ((allenSlabNumber < m_firstAllenSlabRightHemisphereNumber)
                    || (allenSlabNumber > m_lastAllenSlabRightHemisphereNumber)) {
                    hemMsg = ("Allen Slab Number should be between (inclusively) "
                              + AString::number(m_firstAllenSlabRightHemisphereNumber)
                              + " and "
                              + AString::number(m_lastAllenSlabRightHemisphereNumber)
                              + " for RIGHT hemispheres.");
                    okFlag = false;
                }
                break;
        }
        
        if ( ! hemMsg.isEmpty()) {
            warningMessageOut.appendWithNewLine(hemMsg);
        }
    }
    
    return okFlag;
}

