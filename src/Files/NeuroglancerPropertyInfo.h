#ifndef __NEUROGLANCER_PROPERTY_INFO_H__
#define __NEUROGLANCER_PROPERTY_INFO_H__

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

#include "CaretObject.h"
#include "ComboBoxItemInterface.h"
#include "FeatureLabelModel.h"
#include "NeuroglancerPropertyDataTypeEnum.h"


namespace caret {

    class DataFileContentInformation;
    
    class NeuroglancerPropertyInfo : public ComboBoxItemInterface, public CaretObject {
        
    public:
        NeuroglancerPropertyInfo(const NeuroglancerPropertyDataTypeEnum::Enum dataType,
                                 const AString& description,
                                 const uint64_t uniqueID,
                                 const int32_t labelModelIndex);
        
        virtual ~NeuroglancerPropertyInfo();
        
        NeuroglancerPropertyInfo(const NeuroglancerPropertyInfo& obj) = delete;

        NeuroglancerPropertyInfo& operator=(const NeuroglancerPropertyInfo& obj) = delete;
        
        virtual AString comboBoxText() const override;

        NeuroglancerPropertyDataTypeEnum::Enum getDataType() const;
        
        AString getDescription() const;
        
        uint64_t getUniqueID() const;
        
        int32_t getLabelModelIndex() const;

        bool isLabels() const;
        
        FeatureLabelModel* getLabelModel() const;

        int32_t getPropertyIndex() const;
        
        void setPropertyIndex(const int32_t index);
        
        void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
//        void copyHelperNeuroglancerPropertyInfo(const NeuroglancerPropertyInfo& obj);

        void setLabelModel(FeatureLabelModel* labelModel);
        
        NeuroglancerPropertyDataTypeEnum::Enum m_dataType;
        
        AString m_description;
        
        uint64_t m_uniqueID;
        
        int32_t m_labelModelIndex;
        
        int32_t m_propertyIndex = -1;
        
        std::unique_ptr<FeatureLabelModel> m_labelModel;
        
        // ADD_NEW_MEMBERS_HERE

        friend class NeuroglancerAnnotationFileImporter;
    };
    
#ifdef __NEUROGLANCER_PROPERTY_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEUROGLANCER_PROPERTY_INFO_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_PROPERTY_INFO_H__
