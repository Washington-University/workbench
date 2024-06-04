#ifndef __GIFTI_META_DATA_VALIDATOR_INTERFACE_H__
#define __GIFTI_META_DATA_VALIDATOR_INTERFACE_H__

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


/**
 * \class caret::GiftiMetaDataValidatorInterface
 * \brief Interface for validating gifti metadata
 * \ingroup Xml
 */


#include <memory>

#include "CaretObject.h"

namespace caret {

    class GiftiMetaData;
    
    class GiftiMetaDataValidatorInterface : public CaretObject {
        
    public:
        GiftiMetaDataValidatorInterface() { }
        
        virtual ~GiftiMetaDataValidatorInterface() { }
        
        GiftiMetaDataValidatorInterface(const GiftiMetaDataValidatorInterface&) = delete;

        GiftiMetaDataValidatorInterface& operator=(const GiftiMetaDataValidatorInterface&) = delete;
        
        /**
         * @return All known meta data names
         */
        virtual std::vector<AString> getAllMetaDataNames() const = 0;
        
        /**
         * @return All names of metadata that are required
         */
        virtual std::vector<AString> getRequiredMetaDataNames() const = 0;
        
        /**
         * Check the metadata for errors.  User must correct errors to proceed.
         * @param metadata
         *    Metadata for validation
         * @param errorMessageOut
         *    Output with description of any errors
         * @return
         *    True if there are no errors, else false.
         */
        virtual bool checkMetaDataForErrors(const GiftiMetaData* metadata,
                                            AString& errorMessageOut) = 0;
        
        /**
         * Check the metadata for warnings.  User is alerted and may continue without fixing warnings.
         * @param metadata
         *    Metadata for validation
         * @param warnningMessageOut
         *    Output with description of any warnings
         * @return
         *    True if there are no warnings, else false.
         */
        virtual bool checkMetaDataForWarnings(const GiftiMetaData* metadata,
                                              AString& warningMessageOut) = 0;
        
        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GIFTI_META_DATA_VALIDATOR_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GIFTI_META_DATA_VALIDATOR_INTERFACE_DECLARE__

} // namespace
#endif  //__GIFTI_META_DATA_VALIDATOR_INTERFACE_H__
