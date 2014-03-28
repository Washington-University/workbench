#ifndef __CLIPPING_PLANE_GROUP_H__
#define __CLIPPING_PLANE_GROUP_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "CaretObject.h"

#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class ClippingPlaneGroup : public CaretObject, public SceneableInterface {
        
    public:
        ClippingPlaneGroup();
        
        virtual ~ClippingPlaneGroup();
        
        ClippingPlaneGroup(const ClippingPlaneGroup& obj);

        ClippingPlaneGroup& operator=(const ClippingPlaneGroup& obj);
        
        void resetToDefaultValues();
        
        void getTranslation(float translation[3]) const;
        
        void getRotation(float rotation[4][4]) const;
        
        void getThickness(float thickness[3]) const;
        
        void getSelectionStatus(bool selectionStatus[3]) const;
        
        void setTranslation(const float translation[3]);
        
        void setRotation(const float rotation[4][4]);
        
        void setThickness(const float thickness[3]);

        void setSelectionStatus(const bool selectionStatus[3]);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implemetation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperClippingPlaneGroup(const ClippingPlaneGroup& obj);

        void setRotationToIdentity();
        
        SceneClassAssistant* m_sceneAssistant;

        float m_translation[3];
        
        float m_rotation[4][4];
        
        float m_thickness[3];
        
        bool m_selectionStatus[3];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLIPPING_PLANE_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLIPPING_PLANE_GROUP_DECLARE__

} // namespace
#endif  //__CLIPPING_PLANE_GROUP_H__
