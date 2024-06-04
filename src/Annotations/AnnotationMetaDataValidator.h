#ifndef __ANNOTATION_SAMPLE_META_DATA_VALIDATOR_H__
#define __ANNOTATION_SAMPLE_META_DATA_VALIDATOR_H__

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

#include <memory>

#include "Annotation.h"
#include "GiftiMetaDataValidatorInterface.h"



namespace caret {

    class AnnotationMetaDataValidator : public GiftiMetaDataValidatorInterface {
        
    public:
        AnnotationMetaDataValidator(Annotation* annotation,
                                          const int32_t firstAllenSlabLeftHemisphereNumber,
                                          const int32_t firstAllenSlabRightHemisphereNumber,
                                          const int32_t lastAllenSlabRightHemisphereNumber);
        
        virtual ~AnnotationMetaDataValidator();
        
        AnnotationMetaDataValidator(const AnnotationMetaDataValidator&) = delete;

        AnnotationMetaDataValidator& operator=(const AnnotationMetaDataValidator&) = delete;
        
        /**
         * @return All known meta data names
         */
        virtual std::vector<AString> getAllMetaDataNames() const;
        
        /**
         * @return All names of metadata that are required
         */
        virtual std::vector<AString> getRequiredMetaDataNames() const;

        virtual bool checkMetaDataForErrors(const GiftiMetaData* metadata,
                                            AString& errorMessageOut) override;
        
        virtual bool checkMetaDataForWarnings(const GiftiMetaData* metadata,
                                              AString& warningMessageOut) override;
        
        
        std::vector<AString> m_allMetaDataNames;
        
        std::vector<AString> m_requiredMetaDataNames;
        
        // ADD_NEW_METHODS_HERE

    protected:
        Annotation* m_annotation = NULL;
        
        const int32_t m_firstAllenSlabLeftHemisphereNumber;
        
        const int32_t m_firstAllenSlabRightHemisphereNumber;
        
        const int32_t m_lastAllenSlabRightHemisphereNumber;

    private:
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SAMPLE_META_DATA_VALIDATOR_DECLARE__
#endif // __ANNOTATION_SAMPLE_META_DATA_VALIDATOR_DECLARE__

} // namespace
#endif  //__ANNOTATION_SAMPLE_META_DATA_VALIDATOR_H__
