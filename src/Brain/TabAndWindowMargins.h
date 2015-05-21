#ifndef __TAB_AND_WINDOW_MARGINS_H__
#define __TAB_AND_WINDOW_MARGINS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

    class TabAndWindowMargins : public CaretObject, public SceneableInterface {
        
    public:
        TabAndWindowMargins();
        
        virtual ~TabAndWindowMargins();
        
        TabAndWindowMargins(const TabAndWindowMargins& obj);

        TabAndWindowMargins& operator=(const TabAndWindowMargins& obj);
        
        void getTabMargins(int32_t& leftOut,
                           int32_t& rightOut,
                           int32_t& bottomOut,
                           int32_t& topOut) const;

        void setTabMargins(const int32_t left,
                           const int32_t right,
                           const int32_t bottom,
                           const int32_t top);
        
        void getWindowMargins(int32_t& leftOut,
                              int32_t& rightOut,
                              int32_t& bottomOut,
                              int32_t& topOut) const;
        
        void setWindowMargins(const int32_t left,
                              const int32_t right,
                              const int32_t bottom,
                              const int32_t top);
        
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
        void copyHelperTabAndWindowMargins(const TabAndWindowMargins& obj);

        SceneClassAssistant* m_sceneAssistant;

        int32_t m_tabMargins[4];
        
        int32_t m_windowMargins[4];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TAB_AND_WINDOW_MARGINS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TAB_AND_WINDOW_MARGINS_DECLARE__

} // namespace
#endif  //__TAB_AND_WINDOW_MARGINS_H__
