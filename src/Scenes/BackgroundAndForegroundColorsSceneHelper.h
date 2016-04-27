#ifndef __BACKGROUND_AND_FOREGROUND_COLORS_SCENE_HELPER_H__
#define __BACKGROUND_AND_FOREGROUND_COLORS_SCENE_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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
    class BackgroundAndForegroundColors;
    class SceneClassAssistant;

    class BackgroundAndForegroundColorsSceneHelper : public CaretObject, public SceneableInterface {
        
    public:
        BackgroundAndForegroundColorsSceneHelper(BackgroundAndForegroundColors& colors);
        
        virtual ~BackgroundAndForegroundColorsSceneHelper();
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        bool wasRestoredFromScene() const;
          
          
          
          
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
        BackgroundAndForegroundColorsSceneHelper(const BackgroundAndForegroundColorsSceneHelper&);

        BackgroundAndForegroundColorsSceneHelper& operator=(const BackgroundAndForegroundColorsSceneHelper&);
        
        BackgroundAndForegroundColors& m_colors;
        
        SceneClassAssistant* m_sceneAssistant;

        bool m_wasRestoredFromSceneFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BACKGROUND_AND_FOREGROUND_COLORS_SCENE_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BACKGROUND_AND_FOREGROUND_COLORS_SCENE_HELPER_DECLARE__

} // namespace
#endif  //__BACKGROUND_AND_FOREGROUND_COLORS_SCENE_HELPER_H__
