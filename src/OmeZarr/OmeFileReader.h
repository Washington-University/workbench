#ifndef __OME_FILE_READER_H__
#define __OME_FILE_READER_H__

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
#include "OmeAxis.h"
#include "OmeDataSet.h"
#include "OmeDimensionIndices.h"
#include "OmeVersionEnum.h"
#include "ZarrDriverTypeEnum.h"

namespace caret {
    class OmeImage;
    class OmeAttrsV0p4JsonFile;
    class ZarrV2GroupJsonFile;
    class ZarrImageReader;

    class OmeFileReader : public CaretObject {
        
    public:
        OmeFileReader();
        
        virtual ~OmeFileReader();
        
        OmeFileReader(const OmeFileReader& obj) = delete;
        
        OmeFileReader& operator=(const OmeFileReader& obj) = delete;

        FunctionResult initialize(const AString& omeZarrPath);

        const OmeAttrsV0p4JsonFile* getZAttrs() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperOmeFileReader(const OmeFileReader& obj);

        FunctionResultValue<OmeImage*> readImageData(ZarrImageReader* zarrImageReader,
                                                         const std::vector<int64_t>& dimOffsets,
                                                         const std::vector<int64_t>& dimLengths);
                
        ZarrDriverTypeEnum::Enum m_driverType = ZarrDriverTypeEnum::INVALID;
        
        std::unique_ptr<ZarrV2GroupJsonFile> m_zarrGroupFile;
        
        std::unique_ptr<OmeAttrsV0p4JsonFile> m_omeZAttrs;
        
        int32_t m_zarrFormatNumber = -1;
        
        OmeVersionEnum::Enum m_omeVersion = OmeVersionEnum::UNKNOWN;
        
        OmeDimensionIndices m_dimensionIndices;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_FILE_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_FILE_READER_DECLARE__

} // namespace
#endif  //__OME_FILE_READER_H__
