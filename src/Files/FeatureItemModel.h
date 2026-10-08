#ifndef __FEATURE_ITEM_MODEL_H__
#define __FEATURE_ITEM_MODEL_H__

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


#include <map>
#include <memory>

#include <QStandardItemModel>

#include "EventListenerInterface.h"
#include "FunctionResult.h"
#include "SceneableInterface.h"


namespace caret {
    class FeatureItem;
    class FeatureItemGroup;
    class SceneClassAssistant;

    class FeatureItemModel : public QStandardItemModel, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        FeatureItemModel();
        
        virtual ~FeatureItemModel();
        
        FeatureItemModel(const FeatureItemModel&) = delete;

        FeatureItemModel& operator=(const FeatureItemModel&) = delete;
        
        FunctionResult addFeature(QList<QStandardItem*>& featureAndProperties);
        
        FunctionResult addFeatureGroup(FeatureItemGroup* featureItemGroup);
        
        std::vector<const FeatureItemGroup*> getAllFeatureGroups() const;
        
        FeatureItemGroup* getFeatureItemGroupWithID(const uint64_t groupID);
        
        FeatureItem* getFeatureWithGroupAndUniqueID(const uint64_t groupID,
                                                    const uint64_t uniqueID);
        
        void setHeaderLabels(const QStringList& horizontalHeaderLabels);

        void setCheckedStatusOfAllItems(const bool checked);

        void updateCheckedStateOfAllItems();
        
        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

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

        /** Maps Group ID to FeatureItemGroup */
        std::map<uint64_t, FeatureItemGroup*> m_groupIdToFeatureGroupMap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FEATURE_ITEM_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FEATURE_ITEM_MODEL_DECLARE__

} // namespace
#endif  //__FEATURE_ITEM_MODEL_H__
