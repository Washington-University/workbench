#ifndef __FEATURE_ITEM_H__
#define __FEATURE_ITEM_H__

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



#include <memory>

#include <QColor>

#include "FeatureBase.h"
#include "FeatureKey.h"
#include "FeatureItemTypeEnum.h"
#include "SceneableInterface.h"
#include "Vector3D.h"


namespace caret {
    class CaretDataFileSelectionModel;
    class FeatureFile;
    class FeaturePropertyValue;
    
    class FeatureItem : public FeatureBase, public SceneableInterface {

    public:
        FeatureItem(const FeatureItemTypeEnum::Enum featureType,
                    const uint64_t groupID,
                    const uint64_t uniqueID,
                    const std::vector<Vector3D>& ijk,
                    const QColor& color,
                    const float symbolSize,
                    const std::vector<const FeaturePropertyValue*>& propertyValues);

        virtual ~FeatureItem();
        
        FeatureItem(const FeatureItem& obj) = delete;
        
        FeatureItem& operator=(const FeatureItem& obj) = delete;
        
        FeatureItemTypeEnum::Enum getType() const;
        
        bool isDisplayed() const;
        
        const FeatureKey& getFeatureKey() const;
        
        uint64_t getGroupID() const;
        
        AString getUniqueIdAsString() const;
        
        uint64_t getUniqueID() const;
        
        int32_t getNumberOfIJK() const;
        
        const Vector3D& getIJK(const int32_t index) const;
        
        const QColor& getColor() const;
        
        float getSymbolSize() const;

        AString getTypeName() const;
        
        void setUniqueID(const AString& uniqueID);
        
        void setUniqueID(const uint64_t uniqueID);
        
        virtual AString toString() const override;
        
        void getIdentificationText(std::vector<std::vector<AString>>& idTextOut,
                                   const FeatureFile* featureFile,
                                   const bool toolTipFlag) const;

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName) override;

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass) override;
        
        // ADD_NEW_METHODS_HERE
        

    private:
        void copyHelperFeatureItem(const FeatureItem& obj);
        
        FeatureItemTypeEnum::Enum m_featureType = FeatureItemTypeEnum::INVALID;
        
        FeatureKey m_featureKey;
        
        std::vector<Vector3D> m_ijk;
        
        QColor m_color;
        
        float m_symbolSize;
        
        std::vector<const FeaturePropertyValue*> m_propertyValues;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __FEATURE_ITEM__DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FEATURE_ITEM__DECLARE__
    
} // namespace
#endif  //__FEATURE_ITEM_H__

