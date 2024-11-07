#ifndef __ZARR_V2_ARRAY_JSON_FILE_H__
#define __ZARR_V2_ARRAY_JSON_FILE_H__

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

#include "CaretObject.h"
#include "FunctionResult.h"

#include "ZarrDataTypeEnum.h"
#include "ZarrCompressorTypeEnum.h"
#include "ZarrDataTypeByteOrderEnum.h"
#include "ZarrDimensionSeparatorEnum.h"
#include "ZarrJsonFileBase.h"
#include "ZarrRowColumnMajorOrderTypeEnum.h"

namespace caret {

    class ZarrV2ArrayJsonFile : public ZarrJsonFileBase {
        
    public:
        ZarrV2ArrayJsonFile();
        
        virtual ~ZarrV2ArrayJsonFile();
        
        ZarrV2ArrayJsonFile(const ZarrV2ArrayJsonFile& obj);

        ZarrV2ArrayJsonFile& operator=(const ZarrV2ArrayJsonFile& obj);
        
        virtual void clear() override;
        
        std::vector<int64_t> getChunkSizes() const;
        
        void setChunkSizes(const std::vector<int64_t>& chunkSizes);
        
        std::vector<int64_t> getShapeSizes() const;
        
        void setShapeSizes(const std::vector<int64_t>& shapeSizes);
        
        int32_t getZarrFormat() const;
        
        void setZarrFormat(const int32_t zarrFormat);
        
        float getFillValue() const;
        
        void setFillValue(const float fillValue);
        
        ZarrDimensionSeparatorEnum::Enum getDimensionSeparator() const;
        
        void setDimensionSeparator(const ZarrDimensionSeparatorEnum::Enum dimensionSeparator);
        
        ZarrDataTypeEnum::Enum getDataType() const;
        
        void setDataType(const ZarrDataTypeEnum::Enum dataType);
        
        ZarrDataTypeByteOrderEnum::Enum getDataTypeByteOrder() const;
        
        void setDataTypeByteOrder(const ZarrDataTypeByteOrderEnum::Enum dataTypeByteOrder);
        
        int32_t getDataTypeLength() const;
        
        void setDataTypeLength(const int32_t dataTypeLength);
        
        ZarrRowColumnMajorOrderTypeEnum::Enum getRowColumnMajorOrderType() const;
        
        void setRowColumnMajorOrderType(const ZarrRowColumnMajorOrderTypeEnum::Enum rowColumnMajorOrderType);
        
        ZarrCompressorTypeEnum::Enum getCompressorType() const;
        
        const std::map<AString, AString>& getCompressorParameters() const;
        
        void setCompressorInfo(const ZarrCompressorTypeEnum::Enum compressorType,
                               const std::map<AString, AString>& compressorParameters);
        
        virtual AString toString() const;
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual FunctionResult readContentFromJson(const nlohmann::json& json) override;
        
    private:
        void copyHelperZarrV2ArrayJsonFile(const ZarrV2ArrayJsonFile& obj);

        std::vector<int64_t> m_chunkSizes;
        
        std::vector<int64_t> m_shapeSizes;
        
        int32_t m_zarrFormat = -1;
        
        float m_fillValue = -1;
        
        ZarrDimensionSeparatorEnum::Enum m_dimensionSeparator = ZarrDimensionSeparatorEnum::DOT;
        
        ZarrDataTypeEnum::Enum m_dataType = ZarrDataTypeEnum::UNKNOWN;
        
        ZarrDataTypeByteOrderEnum::Enum m_dataTypeByteOrder = ZarrDataTypeByteOrderEnum::UNKNOWN;
        
        int32_t m_dataTypeLength = -1;
        
        ZarrRowColumnMajorOrderTypeEnum::Enum m_rowColumnMajorOrderType = ZarrRowColumnMajorOrderTypeEnum::UNKNOWN;
        
        ZarrCompressorTypeEnum::Enum m_compressorType = ZarrCompressorTypeEnum::NO_COMPRESSOR;
        
        std::map<AString, AString> m_compressorParametersMap;
        // ADD_NEW_MEMBERS_HERE

    };
     
#ifdef __ZARR_V2_ARRAY_JSON_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZARR_V2_ARRAY_JSON_FILE_DECLARE__

} // namespace
#endif  //__ZARR_V2_ARRAY_JSON_FILE_H__
