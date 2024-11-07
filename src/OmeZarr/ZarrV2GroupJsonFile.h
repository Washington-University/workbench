#ifndef __ZARR_V2_GROUP_JSON_FILE_H__
#define __ZARR_V2_GROUP_JSON_FILE_H__

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

#include "ZarrJsonFileBase.h"

namespace caret {

    class ZarrV2GroupJsonFile : public ZarrJsonFileBase {
        
    public:
        ZarrV2GroupJsonFile();
        
        virtual ~ZarrV2GroupJsonFile();
        
        ZarrV2GroupJsonFile(const ZarrV2GroupJsonFile& obj);

        ZarrV2GroupJsonFile& operator=(const ZarrV2GroupJsonFile& obj);
        
        virtual void clear() override;
        
        int32_t getZarrFormat() const;
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual FunctionResult readContentFromJson(const nlohmann::json& json) override;
        
    private:
        void copyHelperZarrV2GroupJsonFile(const ZarrV2GroupJsonFile& obj);

        int32_t m_zarrFormat = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
     
#ifdef __ZARR_V2_GROUP_JSON_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZARR_V2_GROUP_JSON_FILE_DECLARE__

} // namespace
#endif  //__ZARR_V2_GROUP_JSON_FILE_H__
