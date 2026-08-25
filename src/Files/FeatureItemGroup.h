#ifndef __FEATURE_ITEM_GROUP_H__
#define __FEATURE_ITEM_GROUP_H__

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

#include "FeatureBase.h"
#include "FunctionResult.h"
#include "SceneableInterface.h"


namespace caret {
    class FeatureItem;
    class SceneClassAssistant;

    class FeatureItemGroup : public FeatureBase, public SceneableInterface {
        
    public:
        static uint64_t getDefaultGroupID();

        FeatureItemGroup(const uint64_t groupID);
        
        virtual ~FeatureItemGroup();
        
        FeatureItemGroup(const FeatureItemGroup&) = delete;

        FeatureItemGroup& operator=(const FeatureItemGroup&) = delete;
        
        FunctionResult addFeature(QList<QStandardItem*>& featureAndProperties);
        
        uint64_t getGroupID() const;

        const std::vector<const FeatureItem*>& getAllFeatures() const;
        
        std::vector<const uint64_t>& getAllFeatureUniqueIDs() const;
        
        FeatureItem* getFeatureWithUniqueID(const uint64_t uniqueID);

        AString toString() const;

        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        uint64_t m_groupID;
        
        std::map<uint64_t, FeatureItem*> m_uniqueIdToFeatureMap;
        
        /* lazy initialized */
        mutable std::vector<const FeatureItem*> m_allFeatures;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FEATURE_ITEM_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FEATURE_ITEM_GROUP_DECLARE__

} // namespace
#endif  //__FEATURE_ITEM_GROUP_H__
