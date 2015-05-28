#ifndef __MARGIN_H__
#define __MARGIN_H__

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

    class Margin : public CaretObject, public SceneableInterface {
        
    public:
        Margin();
        
        virtual ~Margin();
        
        Margin(const Margin& obj);

        Margin& operator=(const Margin& obj);

        void getMargins(int32_t& leftOut,
                           int32_t& rightOut,
                           int32_t& bottomOut,
                           int32_t& topOut) const;
        
        void setMargins(const int32_t left,
                           const int32_t right,
                           const int32_t bottom,
                           const int32_t top);
        
        void reset();
        
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
        void copyHelperMargin(const Margin& obj);

        SceneClassAssistant* m_sceneAssistant;

        int32_t m_margin[4];
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MARGIN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MARGIN_DECLARE__

} // namespace
#endif  //__MARGIN_H__
