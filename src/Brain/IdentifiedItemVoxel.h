#ifndef __IDENTIFIED_ITEM_VOXEL_H__
#define __IDENTIFIED_ITEM_VOXEL_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "IdentificationSymbolSizeTypeEnum.h"
#include "IdentifiedItemBase.h"


namespace caret {
    class SceneClassAssistant;

    class IdentifiedItemVoxel : public IdentifiedItemBase {
        
    public:
        IdentifiedItemVoxel();

        IdentifiedItemVoxel(const AString& simpleText,
                            const AString& formattedText,
                            const float xyz[3]);
        
        virtual ~IdentifiedItemVoxel();
        
        IdentifiedItemVoxel(const IdentifiedItemVoxel& obj);

        IdentifiedItemVoxel& operator=(const IdentifiedItemVoxel& obj);

        virtual bool isValid() const override;
        
        void getXYZ(float xyzOut[3]) const;
        
        const float* getSymbolRGB() const;
        
        void getSymbolRGBA(uint8_t rgbaOut[4]) const;
        
        float getSymbolSize() const;
        
        void setSymbolRGB(const float* rgb);
        
        void setSymbolSize(const float symbolSize);
        
        IdentificationSymbolSizeTypeEnum::Enum getIdentificationSymbolSizeType() const;
        
        void setIdentificationSymbolSizeType(const IdentificationSymbolSizeTypeEnum::Enum sizeType);
        
        virtual AString toString() const override;
        

        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) override;

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) override;

    private:
        void copyHelperIdentifiedItemVoxel(const IdentifiedItemVoxel& obj);

        void initializeMembers();
        
        float m_xyz[3];
        
        float m_symbolRGB[3];
        
        float m_symbolSize;
        
        IdentificationSymbolSizeTypeEnum::Enum m_identificationSymbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;
        
        SceneClassAssistant* m_sceneAssistant;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IDENTIFIED_ITEM_VOXEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFIED_ITEM_VOXEL_DECLARE__

} // namespace
#endif  //__IDENTIFIED_ITEM_VOXEL_H__
