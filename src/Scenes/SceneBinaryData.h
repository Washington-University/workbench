#ifndef __SCENE_BINARY_DATA_H__
#define __SCENE_BINARY_DATA_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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


#include <cstdint>
#include <memory>

#include "FunctionResult.h"
#include "SceneBinaryEncodingTypeEnum.h"
#include "SceneObject.h"

namespace caret {

    class SceneBinaryData : public SceneObject {
        
    public:
        SceneBinaryData(const AString& name,
                        const AString& binaryDataEncodedAsText,
                        const SceneBinaryEncodingTypeEnum::Enum encodingType,
                        const int64_t uncompressedNumberOfBytes);
        
        SceneBinaryData(const AString& name,
                        const std::vector<float>& floatVector);
        
        SceneBinaryData(const AString& name,
                        const uint8_t* binaryData,
                        const int64_t numberOfBytes);
        
        virtual ~SceneBinaryData();
        
        SceneBinaryData(const SceneBinaryData& rhs);

        SceneBinaryData& operator=(const SceneBinaryData& obj) = delete;
        
        virtual SceneObject* clone() const;
        
        virtual SceneBinaryData* castToSceneBinaryData() override;
        
        virtual const SceneBinaryData* castToSceneBinaryData() const override;
        
        SceneBinaryEncodingTypeEnum::Enum getBinaryEncoding() const;
        
        AString getBinaryDataEncodedAsText() const;
        
        int64_t getBinaryUncompressedNumberOfBytes() const;
        
        std::vector<float> getAsFloatVector() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSceneBinaryData(const SceneBinaryData& obj);

        void reset();
        
        void encodeAsText(const uint8_t* dataPointer,
                          const int64_t numberOfBytes,
                          const SceneBinaryEncodingTypeEnum::Enum encodingType);
        
        FunctionResult decodeFromText(uint8_t* dataPointer) const;
        

        SceneBinaryEncodingTypeEnum::Enum m_binaryEncoding = SceneBinaryEncodingTypeEnum::GZIP_BASE64;

        AString m_binaryDataEncodedAsText;
        
        int64_t m_binaryUncompressedNumberOfBytes = 0;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_BINARY_DATA_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_BINARY_DATA_DECLARE__

} // namespace
#endif  //__SCENE_BINARY_DATA_H__
