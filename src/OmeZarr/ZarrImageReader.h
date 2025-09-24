#ifndef __ZARR_IMAGE_READER_H__
#define __ZARR_IMAGE_READER_H__

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



#include <cinttypes>
#include <memory>
#include <vector>

#include <xtensor.hpp>

#include "CaretObject.h"
#include "FunctionResult.h"
#include "ZarrDataTypeEnum.h"
#include "ZarrDriverTypeEnum.h"

namespace caret {

    class OmeAttrsV0p4JsonFile;
    class ZarrV2ArrayJsonFile;
    class ZarrV2GroupJsonFile;
    
    class ZarrImageReader : public CaretObject {
        
    public:
        ZarrImageReader();
        
        virtual ~ZarrImageReader();
        
        ZarrImageReader(const ZarrImageReader& obj) = delete;

        ZarrImageReader& operator=(const ZarrImageReader& obj) = delete;
        
        FunctionResult initialize(const ZarrDriverTypeEnum::Enum driverType,
                                  const AString& zarrPath,
                                  const AString& relativePath);
        
        std::vector<int64_t> getShapeSizes() const;
        
        ZarrDataTypeEnum::Enum getDataType() const;
        
        FunctionResultValue<xt::xarray<uint8_t>*> readData(const std::vector<int64_t>& dimOffsets,
                                                           const std::vector<int64_t>& dimLengths);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        enum class Status {
            INITIALIZATION_FAILED,
            INITIALIZATION_SUCCESSFUL,
            UNINITIALIZED
        };
        
        void copyHelperZarrImageReader(const ZarrImageReader& obj);

        FunctionResultValue<xt::xarray<uint8_t>*> readLocalFile(const AString& zarrPath,
                                                                const AString& relativePath,
                                                                const std::vector<int64_t>& dimOffsets,
                                                                const std::vector<int64_t>& dimLengths);

        FunctionResultValue<xt::xarray<uint8_t>*> readDataErrorResult(const AString& errorMessage);
        
        ZarrDriverTypeEnum::Enum m_driverType = ZarrDriverTypeEnum::INVALID;
        
        AString m_zarrPath;
        
        AString m_relativePath;
        
        Status m_status = Status::UNINITIALIZED;
        
        std::unique_ptr<ZarrV2ArrayJsonFile> m_zarrayFile;
        
        /** file dimensions*/
        std::vector<int64_t> m_shapeSizes;
        
        /** data type*/
        ZarrDataTypeEnum::Enum m_dataType;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ZARR_IMAGE_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZARR_IMAGE_READER_DECLARE__

} // namespace
#endif  //__ZARR_IMAGE_READER_H__
