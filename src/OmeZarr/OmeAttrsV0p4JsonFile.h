#ifndef __OME_ATTRS_V0P4_JSON_FILE_H__
#define __OME_ATTRS_V0P4_JSON_FILE_H__

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

#include "OmeDimensionIndices.h"
#include "OmeVersionEnum.h"
#include "ZarrJsonFileBase.h"

namespace caret {
    class OmeAxis;
    class OmeDataSet;

    class OmeAttrsV0p4JsonFile : public ZarrJsonFileBase {
        
    public:
        OmeAttrsV0p4JsonFile();
        
        virtual ~OmeAttrsV0p4JsonFile();
        
        OmeAttrsV0p4JsonFile(const OmeAttrsV0p4JsonFile& obj) = delete;

        OmeAttrsV0p4JsonFile& operator=(const OmeAttrsV0p4JsonFile& obj) = delete;
        
        virtual void clear() override;
        
        QString getName() const;
        
        OmeVersionEnum::Enum getVersion() const;
        
        int32_t getNumberOfAxes() const;
        
        OmeAxis* getAxis(const int32_t index);
        
        const OmeAxis* getAxis(const int32_t index) const;
        
        int32_t getNumberOfDataSets() const;
        
        OmeDataSet* getDataSet(const int32_t index);
        
        const OmeDataSet* getDataSet(const int32_t index) const;
        
        const OmeDimensionIndices& getDimensionIndices() const;

        virtual AString toString() const override;
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual FunctionResult readContentFromJson(const nlohmann::json& json) override;
        
    private:
        FunctionResult parseZattsAxesJson(const nlohmann::json& json);
        
        FunctionResult parseZattsDatasetsJson(const nlohmann::json& json);
        
        std::vector<OmeAxis> m_axes;
        
        std::vector<std::unique_ptr<OmeDataSet>> m_dataSets;
        
        QString m_name;
        
        OmeVersionEnum::Enum m_version = OmeVersionEnum::UNKNOWN;
        
        OmeDimensionIndices m_dimensionIndices;

        // ADD_NEW_MEMBERS_HERE

    };
     
#ifdef __OME_ATTRS_V0P4_JSON_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_ATTRS_V0P4_JSON_FILE_DECLARE__

} // namespace
#endif  //__OME_ATTRS_V0P4_JSON_FILE_H__
