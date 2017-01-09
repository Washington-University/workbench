#ifndef __LABEL_DRAWING_PROPERTIES_H__
#define __LABEL_DRAWING_PROPERTIES_H__

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
#include "CaretColorEnum.h"
#include "LabelDrawingTypeEnum.h"

#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class LabelDrawingProperties : public CaretObject, public SceneableInterface {
        
    public:
        LabelDrawingProperties();
        
        virtual ~LabelDrawingProperties();
        
        LabelDrawingProperties(const LabelDrawingProperties&);
        
        LabelDrawingProperties& operator=(const LabelDrawingProperties&);

        LabelDrawingTypeEnum::Enum getDrawingType() const;
        
        void setDrawingType(const LabelDrawingTypeEnum::Enum drawingType);
        
        CaretColorEnum::Enum getOutlineColor() const;
        
        void setOutlineColor(const CaretColorEnum::Enum outlineColor);

        bool isDrawMedialWallFilled() const;
        
        void setDrawMedialWallFilled(const bool drawMedialWallFilled);
        
        // ADD_NEW_METHODS_HERE

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
        
        void initializeInstance();
        
        void copyHelper(const LabelDrawingProperties& obj);
        
        SceneClassAssistant* m_sceneAssistant;

        LabelDrawingTypeEnum::Enum m_drawingType;
        
        CaretColorEnum::Enum m_outlineColor;
        
        /** medial wall is drawn filled*/
        bool m_drawMedialWallFilled;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_DRAWING_PROPERTIES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LABEL_DRAWING_PROPERTIES_DECLARE__

} // namespace
#endif  //__LABEL_DRAWING_PROPERTIES_H__
