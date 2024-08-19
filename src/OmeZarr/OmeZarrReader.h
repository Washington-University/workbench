#ifndef __OME_ZARR_READER_H__
#define __OME_ZARR_READER_H__

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

#include "tensorstore/context.h"

#include "CaretObject.h"
#include "FunctionResult.h"
#include "OmeAxis.h"
#include "OmeDataSet.h"
#include "OmeVersionEnum.h"

namespace caret {
    class ZarrImageReader;

    class OmeZarrReader : public CaretObject {
        
    public:
        OmeZarrReader();
        
        virtual ~OmeZarrReader();
        
        OmeZarrReader(const OmeZarrReader& obj);
        
        OmeZarrReader& operator=(const OmeZarrReader& obj);

        FunctionResult initialize(const AString& omeZarrPath);

        int32_t getNumberOfDataSets() const;
        
        const OmeDataSet& getDataSet(const int32_t index) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperOmeZarrReader(const OmeZarrReader& obj);

        FunctionResult parseZattrsJson(const nlohmann::json& json);
        
        FunctionResult parseZattsAxesJson(const nlohmann::json& json);
        
        FunctionResult parseZattsDatasetsJson(const nlohmann::json& json);
        
        FunctionResult parseZattsCoordinateTransformationsJson(const nlohmann::json& json,
                                                               OmeDataSet& dataSet);
        
        void writeToQImage(const QString& filename,
                           const uint8_t* data,
                           const int32_t width,
                           const int32_t height,
                           const int32_t bytesPerPixel) const;
        
        void writeImageFromRawZarr(const uint8_t* data,
                                   const std::vector<int64_t>& offsets,
                                   const std::vector<int64_t>& lengths,
                                   const AString imageFilenamePrefix);
        
        tensorstore::Context m_tensorstoreContext;
        
        AString m_driverName;
        
        int32_t m_zarrFormatNumber = -1;
        
        OmeVersionEnum::Enum m_omeVersion = OmeVersionEnum::UNKNOWN;
        
        std::vector<OmeAxis> m_axes;
        
        std::vector<OmeDataSet> m_dataSets;
        
        int32_t m_dimensionIndexChannel = -1;
        
        int32_t m_dimensionIndexTime = -1;
        
        int32_t m_dimensionIndexX = -1;
        
        int32_t m_dimensionIndexY = -1;
        
        int32_t m_dimensionIndexZ = -1;
        
        std::unique_ptr<ZarrImageReader> m_zarrImageReader;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_ZARR_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_ZARR_READER_DECLARE__

} // namespace
#endif  //__OME_ZARR_READER_H__
