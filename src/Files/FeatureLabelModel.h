#ifndef __FEATURE_LABEL_MODEL_H__
#define __FEATURE_LABEL_MODEL_H__

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

#include "SceneableInterface.h"


namespace caret {
    class FeatureLabel;
    class SceneClassAssistant;

    class FeatureLabelModel : public QStandardItemModel, public SceneableInterface {
        
        Q_OBJECT

    public:
        FeatureLabelModel(const AString& description);
        
        virtual ~FeatureLabelModel();
        
        FeatureLabelModel(const FeatureLabelModel&) = delete;

        FeatureLabelModel& operator=(const FeatureLabelModel&) = delete;

        AString getDescription() const;

        void addLabel(FeatureLabel* label);
        
        FeatureLabel* getLabelWithValue(const int32_t value);
        
        const FeatureLabel* getLabelWithValue(const int32_t value) const;
        
        void setAllLabelsDisplayed(const bool displayStatus);
        
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

        AString m_description;
        
        std::map<int32_t, FeatureLabel*> m_valueToLabelMap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FEATURE_LABEL_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FEATURE_LABEL_MODEL_DECLARE__

} // namespace
#endif  //__FEATURE_LABEL_MODEL_H__
