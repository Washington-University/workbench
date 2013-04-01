#ifndef __SESSION_MANAGER__H_
#define __SESSION_MANAGER__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include "BrainConstants.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

namespace caret {
    
    class Brain;
    class BrowserTabContent;
    class CaretPreferences;
    class Model;
    
    /// Manages a Caret session which contains 'global' brain data.
    class SessionManager : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        static void createSessionManager();
        
        static void deleteSessionManager();
        
        static SessionManager* get();
        
        void receiveEvent(Event* event);
        
        Brain* addBrain(const bool shareDisplayPropertiesFlag);
        
        int32_t getNumberOfBrains() const;
        
        Brain* getBrain(const int32_t brainIndex);
        
        CaretPreferences* getCaretPreferences();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        SessionManager();
        
        virtual ~SessionManager();
        
        SessionManager(const SessionManager&);

        SessionManager& operator=(const SessionManager&);
        
    public:
        virtual AString toString() const;
        
    private:
        void updateBrowserTabContents();
        
        void resetBrains(const bool keepSceneFiles);
        
        /** The session manager */
        static SessionManager* s_singletonSessionManager;
        
        /** The browser tabs */
        BrowserTabContent* m_browserTabs[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];  
        
        /** Holds valid model display controllers */
        std::vector<Model*> m_modelDisplayControllers;
        
        /** Holds all loaded brains */
        std::vector<Brain*> m_brains;
        
        /** Caret's preferences */
        CaretPreferences* m_caretPreferences;
    };
    
#ifdef __SESSION_MANAGER_DECLARE__
    SessionManager* SessionManager::s_singletonSessionManager = NULL;
    
#endif // __SESSION_MANAGER_DECLARE__

} // namespace
#endif  //__SESSION_MANAGER__H_
