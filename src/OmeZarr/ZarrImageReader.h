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

#ifdef WORKBENCH_HAVE_TENSOR_STORE
#include "tensorstore/context.h"
#include "tensorstore/open.h"
#endif

#include "CaretObject.h"
#include "FunctionResult.h"
#include "ZarrDataTypeEnum.h"

namespace caret {

    class ZarrImageReader : public CaretObject {
        
    public:
        ZarrImageReader();
        
        virtual ~ZarrImageReader();
        
        ZarrImageReader(const ZarrImageReader& obj);

        ZarrImageReader& operator=(const ZarrImageReader& obj);
        
        FunctionResult readZarrayFile(tensorstore::Context& context,
                                      const AString& zarrayFilename,
                                      const AString& driverName);

        static FunctionResultString getDriverNameFromFilename(const AString& filename);
        
        static FunctionResultInt32 readZgroupFileZarrFormat(tensorstore::Context& context,
                                                            const AString& driverName,
                                                            const AString& zarrFilename);
        
        static FunctionResultValue<nlohmann::json> readJson(tensorstore::Context& context,
                                                            const AString& driverName,
                                                            const AString& zarrFilename,
                                                            const AString& fileInZarrFilename);
        
        static FunctionResultValue<nlohmann::json> readZattrsFileJson(tensorstore::Context& context,
                                                                      const AString& driverName,
                                                                      const AString& zarrFilename);
        
        std::vector<int64_t> getDimensions() const;
        
        void setDimensions(const std::vector<int64_t> dimensions);
        
        ZarrDataTypeEnum::Enum getDataType() const;
        
        void setDataType(const ZarrDataTypeEnum::Enum dataType);

        FunctionResultValue<unsigned char*> readData(const std::vector<int64_t>& dimOffsets,
                                                     const std::vector<int64_t>& dimLengths,
                                                     const ZarrDataTypeEnum::Enum dataTypeToRead);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperZarrImageReader(const ZarrImageReader& obj);

        /** file dimensions*/
        std::vector<int64_t> m_dimensions;
        
        /** data type*/
        ZarrDataTypeEnum::Enum m_dataType;
        
        /** The 'store' for reading the tensorstore file */
        tensorstore::TensorStore<> m_store;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ZARR_IMAGE_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZARR_IMAGE_READER_DECLARE__

} // namespace
#endif  //__ZARR_IMAGE_READER_H__
