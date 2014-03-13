#ifndef __SCENE_WINDOW_GEOMETRY__H_
#define __SCENE_WINDOW_GEOMETRY__H_

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


#include "CaretObject.h"
#include "SceneableInterface.h"

class QWidget;

namespace caret {

    class BrainBrowserWindow;
    class SceneAttributes;
    class SceneClass;
    
    class SceneWindowGeometry : public CaretObject, public SceneableInterface {
        
    public:
        SceneWindowGeometry(QWidget* window);
        
        SceneWindowGeometry(QWidget* window,
                            QWidget* parentWindow);
        
        virtual ~SceneWindowGeometry();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

        static void setFirstBrowserWindowCoordinatesInvalid();
        
    private:
        SceneWindowGeometry(const SceneWindowGeometry&);

        SceneWindowGeometry& operator=(const SceneWindowGeometry&);
        
        /** The window whose geometry is restored/saved */
        QWidget* m_window;

        /** Optional parent window (may be NULL) */
        QWidget* m_parentWindow;
        
        /** X-coordinate of first browser window from the scene file */
        static int32_t s_firstBrowserWindowSceneX;
        
        /** Y-coordinate of first browser window from the scene file */
        static int32_t s_firstBrowserWindowSceneY;
        
        /** X-coordinate of first browser window after restoration from scene */
        static int32_t s_firstBrowserWindowRestoredX;
        
        /** Y-coordinate of first browser window after restoration from scene */
        static int32_t s_firstBrowserWindowRestoredY;
        
        /** First browser window */
        static BrainBrowserWindow* s_firstBrowserWindow;
        
        /** Are first browser window coordinates valid */
        static bool s_firstBrowserWindowCoordinatesValid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_WINDOW_GEOMETRY_DECLARE__
    BrainBrowserWindow* SceneWindowGeometry::s_firstBrowserWindow = NULL;
    int32_t SceneWindowGeometry::s_firstBrowserWindowSceneX = 0;
    int32_t SceneWindowGeometry::s_firstBrowserWindowSceneY = 0;
    int32_t SceneWindowGeometry::s_firstBrowserWindowRestoredX = 0;
    int32_t SceneWindowGeometry::s_firstBrowserWindowRestoredY = 0;
    bool SceneWindowGeometry::s_firstBrowserWindowCoordinatesValid = false;
#endif // __SCENE_WINDOW_GEOMETRY_DECLARE__

} // namespace
#endif  //__SCENE_WINDOW_GEOMETRY__H_
