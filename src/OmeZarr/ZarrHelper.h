#ifndef __ZARR_HELPER_H__
#define __ZARR_HELPER_H__

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

#include <nlohmann/json.hpp>

#include "CaretObject.h"
#include "FunctionResult.h"
#include "ZarrDriverTypeEnum.h"


namespace caret {

    class OmeAttrsV0p4JsonFile;
    class ZarrV2ArrayJsonFile;
    class ZarrV2GroupJsonFile;
    
    class ZarrHelper : public CaretObject {
        
    public:
        ZarrHelper();
        
        virtual ~ZarrHelper();
        
        ZarrHelper(const ZarrHelper&) = delete;

        ZarrHelper& operator=(const ZarrHelper&) = delete;
        
        static FunctionResultValue<ZarrV2ArrayJsonFile*> readZarrV2ArrayJsonFile(const ZarrDriverTypeEnum::Enum driverType,
                                                                                 const AString& zarrPath,
                                                                                 const AString& relativePath);
        
        static FunctionResultValue<ZarrDriverTypeEnum::Enum> getDriverTypeFromFilename(const AString& filename);
        
        static FunctionResultValue<ZarrV2GroupJsonFile*> readZarrV2GroupJsonFile(const ZarrDriverTypeEnum::Enum driverType,
                                                                                 const AString& zarrPath);
        
        static FunctionResultValue<OmeAttrsV0p4JsonFile*> readOmeAttrsV0p4JsonFile(const ZarrDriverTypeEnum::Enum driverType,
                                                                                           const AString& zarrPath);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        static FunctionResultValue<nlohmann::json> jsonError(const AString& filename,
                                                             const AString& errorMessage);
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ZARR_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZARR_HELPER_DECLARE__

} // namespace
#endif  //__ZARR_HELPER_H__
