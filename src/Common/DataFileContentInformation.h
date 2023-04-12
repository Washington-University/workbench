#ifndef __DATA_FILE_CONTENT_INFORMATION_H__
#define __DATA_FILE_CONTENT_INFORMATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <map>

#include "CaretObject.h"

namespace caret {

    class Vector3D;
    
    class DataFileContentInformation : public CaretObject {
        
    public:
        /**
         * Option flags for commands.
         * All options are off by default.
         * If an option should be on by default, call setOptionFlag()
         * from this class' constructor.
         */
        enum OptionFlag {
            /**
             * Show information about each map for files that support maps.
             */
            OPTION_SHOW_MAP_INFORMATION,
            /*
             * Show detailed information about CIFTI Label Mappings
             */
            OPTION_SHOW_CIFTI_LABEL_MAPPING
        };
        
        DataFileContentInformation();
        
        virtual ~DataFileContentInformation();
        
        void addNameAndValue(const AString& name,
                             const char* value);
        
        void addNameAndValue(const AString& name,
                             const AString& value);
        
        void addNameAndValue(const AString& name,
                             const int32_t value);
        
        void addNameAndValue(const AString& name,
                             const int64_t value);
        
        void addNameAndValue(const AString& name,
                             const double value,
                             const int32_t precision = 3);
        
        void addNameAndValue(const AString& name,
                             const Vector3D& vector);
        
        void addNameAndValue(const AString& name,
                             const bool value);
        
        void addText(const AString& text);
        
        AString getInformationInString() const;
        
        bool isOptionFlag(const OptionFlag optionFlag) const;
        
        void setOptionFlag(const OptionFlag optionFlag,
                           const bool flagValue);
        
    private:
        DataFileContentInformation(const DataFileContentInformation&);

        DataFileContentInformation& operator=(const DataFileContentInformation&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        std::vector<std::pair<AString, AString> > m_namesAndValues;
        
        AString m_text;
        
        std::map<OptionFlag, bool> m_optionFlags;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_CONTENT_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_CONTENT_INFORMATION_DECLARE__

} // namespace
#endif  //__DATA_FILE_CONTENT_INFORMATION_H__
