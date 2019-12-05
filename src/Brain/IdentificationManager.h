#ifndef __IDENTIFICATION_MANAGER_H__
#define __IDENTIFICATION_MANAGER_H__

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

#include <list>
#include <memory>

#include "CaretColorEnum.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {

    class CaretPreferences;
    class IdentificationFilter;
    class IdentificationHistoryManager;
    class IdentifiedItem;
    class IdentifiedItemNode;
    class IdentifiedItemVoxel;
    class SceneClassAssistant;
    
    class IdentificationManager : public SceneableInterface {
        
    public:
        IdentificationManager(const CaretPreferences* caretPreferences);
        
        virtual ~IdentificationManager();
        
        void addIdentifiedItem(IdentifiedItem* item);
        
        AString getIdentificationText() const;
        
        const IdentificationFilter* getIdentificationFilter() const;
        
        IdentificationFilter* getIdentificationFilter();
        
        const IdentificationHistoryManager* getIdentificationHistoryManager() const;
        
        IdentificationHistoryManager* getIdentificationHistoryManager();
        
        std::vector<IdentifiedItemNode> getNodeIdentifiedItemsForSurface(const StructureEnum::Enum structure,
                                                                         const int32_t surfaceNumberOfNodes) const;
        
        std::vector<IdentifiedItemVoxel> getIdentifiedItemsForVolume() const;
        
        void removeIdentifiedNodeItem(const StructureEnum::Enum structure,
                                      const int32_t surfaceNumberOfNodes,
                                      const int32_t nodeIndex);
        
        void removeIdentifiedVoxelItem(const float xyz[3]);
        
        void removeIdentificationText();
        
        void removeAllIdentifiedItems();
        
        void removeAllIdentifiedSymbols();
        
        bool isContralateralIdentificationEnabled() const;
        
        void setContralateralIdentificationEnabled(const bool enabled);
        
        float getIdentificationSymbolSize() const;
        
        void setIdentificationSymbolSize(const float symbolSize);
        
        float getMostRecentIdentificationSymbolSize() const;
        
        void setMostRecentIdentificationSymbolSize(const float symbolSize);
        
        CaretColorEnum::Enum getIdentificationSymbolColor() const;
        
        void setIdentificationSymbolColor(const CaretColorEnum::Enum color);
        
        CaretColorEnum::Enum getIdentificationContralateralSymbolColor() const;
        
        void setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color);
        
        bool isShowSurfaceIdentificationSymbols() const;
        
        void setShowSurfaceIdentificationSymbols(const bool showSurfaceIdentificationSymbols);
        
        bool isShowVolumeIdentificationSymbols() const;
        
        void setShowVolumeIdentificationSymbols(const bool showVolumeIdentificationSymbols);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        IdentificationManager(const IdentificationManager&);

        IdentificationManager& operator=(const IdentificationManager&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        void addIdentifiedItemPrivate(IdentifiedItem* item);
        
        // ADD_NEW_MEMBERS_HERE

        SceneClassAssistant* m_sceneAssistant;
        
        std::list<IdentifiedItem*> m_identifiedItems;
        
        AString m_previousIdentifiedItemsText;
        
        IdentifiedItem* m_mostRecentIdentifiedItem;
        
        bool m_contralateralIdentificationEnabled;
        
        float m_identifcationSymbolSize;
        
        float m_identifcationMostRecentSymbolSize;
        
        CaretColorEnum::Enum m_identificationSymbolColor;
        
        CaretColorEnum::Enum m_identificationContralateralSymbolColor;

        std::unique_ptr<IdentificationFilter> m_identificationFilter;
        
        std::unique_ptr<IdentificationHistoryManager> m_identificationHistoryManager;
        
        /** show surface identification symbols*/
        bool m_showSurfaceIdentificationSymbols;
        
        /** show volume identification symbols*/
        bool m_showVolumeIdentificationSymbols;
    };
    
#ifdef __IDENTIFICATION_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_MANAGER_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_MANAGER_H__
