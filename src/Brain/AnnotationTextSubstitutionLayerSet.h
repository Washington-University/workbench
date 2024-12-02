#ifndef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_H__
#define __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_H__

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



#include <memory>

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class AnnotationTextSubstitutionLayer;
    class Brain;
    class DisplayPropertiesAnnotationTextSubstitution;
    class SceneClassAssistant;

    class AnnotationTextSubstitutionLayerSet : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        AnnotationTextSubstitutionLayerSet(Brain* brain);
        
        virtual ~AnnotationTextSubstitutionLayerSet();
        
        AnnotationTextSubstitutionLayerSet(const AnnotationTextSubstitutionLayerSet&) = delete;

        AnnotationTextSubstitutionLayerSet& operator=(const AnnotationTextSubstitutionLayerSet&) = delete;
        
        virtual void receiveEvent(Event* event) override;
        
        bool isSubstitutionsValid() const;
        
        int32_t getNumberOfLayers() const;
        
        void setNumberOfLayers(const int32_t numberOfLayers);
        
        AnnotationTextSubstitutionLayer* getLayer(const int32_t index);

        const AnnotationTextSubstitutionLayer* getLayer(const int32_t index) const;
        
        void reset();
        
        void updateContent();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        void restoreFromOldScene(DisplayPropertiesAnnotationTextSubstitution* dispProbsAnnSubs);
          
          
          
          
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
        Brain* m_brain;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::vector<std::unique_ptr<AnnotationTextSubstitutionLayer>> m_layers;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_DECLARE__
    static constexpr int32_t s_MAXIMUM_NUMBER_OF_LAYERS = 25;
    static constexpr int32_t s_MINIMUM_NUMBER_OF_LAYERS =  3;
#endif // __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_H__
