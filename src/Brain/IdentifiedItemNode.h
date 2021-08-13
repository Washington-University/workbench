#ifndef __IDENTIFIED_ITEM_NODE_H__
#define __IDENTIFIED_ITEM_NODE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
#include "StructureEnum.h"

namespace caret {

    class IdentifiedItemNode : public IdentifiedItemBase {
        
    public:
        IdentifiedItemNode();
        
        IdentifiedItemNode(const AString& simpleText,
                           const AString& formattedText,
                           const StructureEnum::Enum structure,
                           const int32_t surfaceNumberOfNodes,
                           const int32_t nodeIndex);
        
        virtual ~IdentifiedItemNode();
        
        IdentifiedItemNode(const IdentifiedItemNode& obj);

        IdentifiedItemNode& operator=(const IdentifiedItemNode& obj);
        
        // ADD_NEW_METHODS_HERE

        virtual bool isValid() const;
        
//        AString getText() const;
        
        StructureEnum::Enum getStructure() const;
        
        StructureEnum::Enum getContralateralStructure() const;
        
        void setContralateralStructure(const StructureEnum::Enum contralateralStructure);

        int32_t getSurfaceNumberOfNodes() const;
        
        int32_t getNodeIndex() const;
        
        const float* getSymbolRGB() const;
        
        const float* getContralateralSymbolRGB() const;
        
        void getSymbolRGBA(uint8_t rgbaOut[4]) const;
        
        void getContralateralSymbolRGB(uint8_t rgbaOut[4]) const;
        
        float getSymbolSize() const;
        
        void setSymbolRGB(const float* rgb);
        
        void setContralateralSymbolRGB(const float* rgb);
        
        void setSymbolSize(const float symbolSize);
        
        IdentificationSymbolSizeTypeEnum::Enum getIdentificationSymbolSizeType() const;
        
        void setIdentificationSymbolSizeType(const IdentificationSymbolSizeTypeEnum::Enum sizeType);
        
        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        void copyHelperIdentifiedItemNode(const IdentifiedItemNode& obj);

        void initializeMembers();
        
        // ADD_NEW_MEMBERS_HERE
        
        StructureEnum::Enum m_structure;
        
        StructureEnum::Enum m_contralateralStructure;
        
        int32_t m_surfaceNumberOfNodes;
        
        int32_t m_nodeIndex;
        
        float m_symbolRGB[3];
        
        float m_contralateralSymbolRGB[3];
        
        float m_symbolSize;
        
        IdentificationSymbolSizeTypeEnum::Enum m_identificationSymbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;
        
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __IDENTIFIED_ITEM_NODE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFIED_ITEM_NODE_DECLARE__

} // namespace
#endif  //__IDENTIFIED_ITEM_NODE_H__
