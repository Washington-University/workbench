#ifndef __SESSION_MANAGER__H_
#define __SESSION_MANAGER__H_

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

#include <array>
#include <memory>

#include "ApplicationTypeEnum.h"
#include "BrainConstants.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"

namespace caret {
    
    class Brain;
    class BrowserTabContent;
    class BrowserWindowContent;
    class CaretPreferences;
    class CiftiConnectivityMatrixDataFileManager;
    class CiftiFiberTrajectoryManager;
    class DataToolTipsManager;
    class ImageCaptureSettings;
    class Model;
    
    /// Manages a Caret session which contains 'global' brain data.
    class SessionManager : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        static void createSessionManager(const ApplicationTypeEnum::Enum applicationType);
        
        static void deleteSessionManager();
        
        static SessionManager* get();
        
        void receiveEvent(Event* event);
        
        Brain* addBrain(const bool shareDisplayPropertiesFlag);
        
        int32_t getNumberOfBrains() const;
        
        Brain* getBrain(const int32_t brainIndex) const;
        
        CaretPreferences* getCaretPreferences();
        
        CiftiConnectivityMatrixDataFileManager* getCiftiConnectivityMatrixDataFileManager();
        
        const CiftiConnectivityMatrixDataFileManager* getCiftiConnectivityMatrixDataFileManager() const;
        
        CiftiFiberTrajectoryManager* getCiftiFiberTrajectoryManager();
        
        const CiftiFiberTrajectoryManager* getCiftiFiberTrajectoryManager() const;
        
        DataToolTipsManager* getDataToolTipsManager();
        
        const DataToolTipsManager* getDataToolTipsManager() const;
        
        ImageCaptureSettings* getImageCaptureDialogSettings();
        
        const ImageCaptureSettings* getImageCaptureDialogSettings() const;
        
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
        
        /** The browser window content */
        std::array<BrowserWindowContent*, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS> m_browserWindowContent;
        
        /** Holds valid models */
        std::vector<Model*> m_models;
        
        /** Holds all loaded brains */
        std::vector<Brain*> m_brains;
        
        /** Caret's preferences */
        CaretPreferences* m_caretPreferences;
        
        /** Loads connectivity matrix data */
        CiftiConnectivityMatrixDataFileManager* m_ciftiConnectivityMatrixDataFileManager;
        
        /** Loads fiber trajectory data */
        CiftiFiberTrajectoryManager* m_ciftiFiberTrajectoryManager;
        
        /** Data Tool Tips Manager */
        std::unique_ptr<DataToolTipsManager> m_dataToolTipsManager;
        
        /** Settings for image capture dialog */
        ImageCaptureSettings* m_imageCaptureDialogSettings;
        
    };
    
#ifdef __SESSION_MANAGER_DECLARE__
    SessionManager* SessionManager::s_singletonSessionManager = NULL;
    
#endif // __SESSION_MANAGER_DECLARE__

} // namespace
#endif  //__SESSION_MANAGER__H_
