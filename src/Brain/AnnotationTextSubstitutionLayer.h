#ifndef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_H__
#define __ANNOTATION_TEXT_SUBSTITUTION_LAYER_H__

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

#include <cstdint>
#include <memory>

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"
#include "SceneableInterface.h"

namespace caret {
    class AnnotationTextSubstitutionFile;
    class CaretDataFileSelectionModel;
    class SceneClassAssistant;
    
    class AnnotationTextSubstitutionLayer : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        AnnotationTextSubstitutionLayer();
        
        virtual ~AnnotationTextSubstitutionLayer();
        
        AnnotationTextSubstitutionLayer(const AnnotationTextSubstitutionLayer& obj);
        
        AnnotationTextSubstitutionLayer& operator=(const AnnotationTextSubstitutionLayer& obj);
        
        virtual void receiveEvent(Event* event) override;
        
        bool isSubstitutionsValid() const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        const std::vector<AString>& getAllGroupIdentifiers() const;
        
        AString getGroupIdentifer() const;
        
        void setGroupIdentifer(const AString& groupIdentifer);
        
        static AString getNoGroupIdentifierText();
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        int32_t getSubstitutionFileRowIndex() const;
        
        void setSubstitutionFileRowIndex(const int32_t substitutionFileRowIndex);
        
        AnnotationTextSubstitutionFile* getSelectedSubstitutionFile();
        
        const AnnotationTextSubstitutionFile* getSelectedSubstitutionFile() const;
        
        const CaretDataFileSelectionModel* getSubstitutionFileSelectionModel() const;
        
        CaretDataFileSelectionModel* getSubstitutionFileSelectionModel();
        
        void updateContent(const std::vector<AString>& groupIDs);
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;
        
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
        void copyHelperAnnotationTextSubstitutionLayer(const AnnotationTextSubstitutionLayer& obj);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        /** enabled status*/
        bool m_enabled = false;
        
        std::vector<AString> m_allGroupIdentifiers;
        
        /** Group identifier in annotation substution text*/
        AString m_groupIdentifer;
        
        /** map yoking group*/
        MapYokingGroupEnum::Enum m_mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
        
        /** selected row index in annotation substitution file*/
        int32_t m_substitutionFileRowIndex = 0;
        
        /** annotation file selection model*/
        std::unique_ptr<CaretDataFileSelectionModel> m_substitutionFileSelectionModel;
        
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_SUBSTITUTION_LAYER_DECLARE__
    
} // namespace
#endif  //__ANNOTATION_TEXT_SUBSTITUTION_LAYER_H__

