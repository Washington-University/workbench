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
#include "IdentificationSymbolSizeTypeEnum.h"
#include "IdentifiedItemUniversalTypeEnum.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {

    class CaretPreferences;
    class IdentificationFilter;
    class IdentificationHistoryManager;
    class IdentifiedItemUniversal;
    class SceneClassAssistant;
    class SelectionItem;

    class IdentificationManager : public SceneableInterface {
        
    public:
        IdentificationManager(const CaretPreferences* caretPreferences);
        
        virtual ~IdentificationManager();
        
        void addIdentifiedItem(IdentifiedItemUniversal* item);
        
        AString getIdentificationText() const;
        
        const IdentificationFilter* getIdentificationFilter() const;
        
        IdentificationFilter* getIdentificationFilter();
        
        const IdentificationHistoryManager* getIdentificationHistoryManager() const;
        
        IdentificationHistoryManager* getIdentificationHistoryManager();
        
        std::vector<const IdentifiedItemUniversal*> getIdentifiedItems() const;
        
        void getIdentifiedItemColorAndSize(const IdentifiedItemUniversal* item,
                                           const IdentifiedItemUniversalTypeEnum::Enum drawingOnType,
                                           const float referenceHeight,
                                           const bool contralateralSurfaceFlag,
                                           std::array<uint8_t, 4>& rgbaOut,
                                           float& symbolDiameterOut) const;
        
        bool removeIdentifiedItem(const SelectionItem* selectedItem);
        
        void removeIdentificationText();
        
        void removeAllIdentifiedItems();
        
        void removeAllIdentifiedSymbols();
        
        bool isContralateralIdentificationEnabled() const;
        
        void setContralateralIdentificationEnabled(const bool enabled);
        
        IdentificationSymbolSizeTypeEnum::Enum getIdentificationSymbolSizeType() const;
        
        void setIdentificationSymbolSizeType(const IdentificationSymbolSizeTypeEnum::Enum sizeType);
        
        float getIdentificationSymbolSize() const;
        
        void setIdentificationSymbolSize(const float symbolSize);
        
        float getMostRecentIdentificationSymbolSize() const;
        
        void setMostRecentIdentificationSymbolSize(const float symbolSize);
        
        float getIdentificationSymbolPercentageSize() const;
        
        void setIdentificationSymbolPercentageSize(const float symbolSize);
        
        float getMostRecentIdentificationSymbolPercentageSize() const;
        
        void setMostRecentIdentificationSymbolPercentageSize(const float symbolSize);
        
        CaretColorEnum::Enum getIdentificationSymbolColor() const;
        
        void setIdentificationSymbolColor(const CaretColorEnum::Enum color);
        
        CaretColorEnum::Enum getIdentificationContralateralSymbolColor() const;
        
        void setIdentificationContralateralSymbolColor(const CaretColorEnum::Enum color);
        
        bool isShowMediaIdentificationSymbols() const;
        
        void setShowMediaIdentificationSymbols(const bool showMediaIdenficationSymbols);
        
        bool isShowSurfaceIdentificationSymbols() const;
        
        void setShowSurfaceIdentificationSymbols(const bool showSurfaceIdentificationSymbols);
        
        bool isShowVolumeIdentificationSymbols() const;
        
        void setShowVolumeIdentificationSymbols(const bool showVolumeIdentificationSymbols);
        
        bool isShowOtherTypeIdentificationSymbols() const;
        
        void setShowOtherTypeIdentificationSymbols(const bool showOtherTypeIdentificationSymbols);
        
        float getChartLineLayerSymbolSize() const;
        
        void setChartLineLayerSymbolSize(const float symbolSize);
        
        float getChartLineLayerToolTipTextSize() const;
        
        void setChartLineLayerToolTipTextSize(const float textSize);

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

        void addIdentifiedItemPrivate(IdentifiedItemUniversal* item,
                                      const bool restoringSceneFlag);
        
        // ADD_NEW_MEMBERS_HERE

        SceneClassAssistant* m_sceneAssistant;
        
        std::list<std::unique_ptr<IdentifiedItemUniversal>> m_identifiedItems;
        
        AString m_previousIdentifiedItemsText;
        
        IdentifiedItemUniversal* m_mostRecentIdentifiedItem = NULL;
        
        bool m_contralateralIdentificationEnabled;
        
        IdentificationSymbolSizeTypeEnum::Enum m_identificationSymbolSizeType = IdentificationSymbolSizeTypeEnum::MILLIMETERS;
        
        float m_identifcationSymbolSize;
        
        float m_identifcationMostRecentSymbolSize;
        
        float m_identifcationSymbolPercentageSize;
        
        float m_identifcationMostRecentSymbolPercentageSize;
        
        CaretColorEnum::Enum m_identificationSymbolColor;
        
        CaretColorEnum::Enum m_identificationContralateralSymbolColor;

        float m_chartLineLayerSymbolSize = 2.0;
        
        float m_chartLineLayerToolTipTextSize = 2.0;
        
        std::unique_ptr<IdentificationFilter> m_identificationFilter;
        
        std::unique_ptr<IdentificationHistoryManager> m_identificationHistoryManager;
        
        /** show media identification symbols*/
        bool m_showMediaIdentificationSymbols;

        /** show surface identification symbols*/
        bool m_showSurfaceIdentificationSymbols;
        
        /** show volume identification symbols*/
        bool m_showVolumeIdentificationSymbols;
        
        /** show identification symbols from other types (eg: volume symbols on surfaces)*/
        bool m_showOtherTypeIdentificationSymbols;
    };
    
#ifdef __IDENTIFICATION_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_MANAGER_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_MANAGER_H__
