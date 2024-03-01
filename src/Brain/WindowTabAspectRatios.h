#ifndef __WINDOW_TAB_ASPECT_RATIOS_H__
#define __WINDOW_TAB_ASPECT_RATIOS_H__

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


#include <map>
#include <memory>
#include <set>

#include "CaretObject.h"

#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class WindowTabAspectRatios : public CaretObject, public SceneableInterface {
        
    public:
        WindowTabAspectRatios();
        
        virtual ~WindowTabAspectRatios();
        
        WindowTabAspectRatios(const WindowTabAspectRatios& obj);

        WindowTabAspectRatios& operator=(const WindowTabAspectRatios& obj);

        bool isValid() const;
        
        int32_t getWindowIndex() const;
        
        float getWindowAspectRatio() const;
        
        void setWindowAspectRatio(const int32_t windowIndex,
                                  const float windowAspectRatio);
        
        void addTabAspectRatio(const int32_t tabIndex,
                               const float tabAspectRatio);

        float getTabAspectRatio(const int32_t tabIndex) const;
        
        std::set<int32_t> getTabIndices() const;
        
        bool testMatchingTabs(const std::set<int32_t>& displayedTabIndices,
                              AString& differentTabsMessageOut) const;
        
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
        void copyHelperWindowTabAspectRatios(const WindowTabAspectRatios& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        int32_t m_windowIndex = -1;
        
        float m_windowAspectRatio = -1.0;
        
        std::map<int32_t, float> m_tabIndexAspectRatios;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WINDOW_TAB_ASPECT_RATIOS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WINDOW_TAB_ASPECT_RATIOS_DECLARE__

} // namespace
#endif  //__WINDOW_TAB_ASPECT_RATIOS_H__
