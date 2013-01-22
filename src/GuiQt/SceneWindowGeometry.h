#ifndef __SCENE_WINDOW_GEOMETRY__H_
#define __SCENE_WINDOW_GEOMETRY__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
