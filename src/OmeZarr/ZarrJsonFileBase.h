#ifndef __ZARR_JSON_FILE_BASE_H__
#define __ZARR_JSON_FILE_BASE_H__

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
#include <string>

#include <nlohmann/json_fwd.hpp>

#include "CaretObject.h"
#include "FunctionResult.h"
#include "ZarrDriverTypeEnum.h"

namespace caret {

    class ZarrJsonFileBase : public CaretObject {
        
    public:
        ZarrJsonFileBase();
        
        virtual ~ZarrJsonFileBase();
        
        ZarrJsonFileBase(const ZarrJsonFileBase& obj);

        ZarrJsonFileBase& operator=(const ZarrJsonFileBase& obj);

        virtual void clear();
        
        std::string getJsonFilename() const;
        
        FunctionResult readFile(const ZarrDriverTypeEnum::Enum driverType,
                                const AString& zarrPath,
                                const AString& jsonFileRelativePath);
        
        AString getZarrPath() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    protected:
        /**
         * Must be override by child classes to read the file's content from JSON
         * @param json
         *    The JSON containing the file's content
         * @return
         *    Function result with Ok/Error.
         */
        virtual FunctionResult readContentFromJson(const nlohmann::json& json) = 0;
        
        FunctionResultValue<nlohmann::json> jsonError(const AString& errorMessage) const;
        
    private:
        void copyHelperZarrJsonFileBase(const ZarrJsonFileBase& obj);

        FunctionResultValue<nlohmann::json> readJsonFromFile(const ZarrDriverTypeEnum::Enum driverType,
                                                             const AString& zarrPath,
                                                             const AString& jsonFileRelativePath);
        
        AString m_zarrPath;
        
        std::string m_jsonFilename;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ZARR_JSON_FILE_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZARR_JSON_FILE_BASE_DECLARE__

} // namespace
#endif  //__ZARR_JSON_FILE_BASE_H__
