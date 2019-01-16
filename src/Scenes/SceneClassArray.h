#ifndef __SCENE_CLASS_ARRAY__H_
#define __SCENE_CLASS_ARRAY__H_

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


#include "SceneObjectArray.h"

namespace caret {

    class SceneClass;
    
    class SceneClassArray : public SceneObjectArray {
        
    public:
        SceneClassArray(const AString& name,
                        SceneClass* values[],
                        const int32_t numberOfArrayElements);
        
        SceneClassArray(const AString& name,
                        const std::vector<SceneClass*>& values);
        
        SceneClassArray(const AString& name,
                          const int numberOfArrayElements);
        
        SceneClassArray(const SceneClassArray& rhs);

        virtual ~SceneClassArray();
        
        virtual const SceneClassArray* castToSceneClassArray() const;

        void setClassAtIndex(const int32_t arrayIndex,
                      SceneClass* sceneClass);
        
        const SceneClass* getClassAtIndex(const int32_t arrayIndex) const;
        
        virtual std::vector<SceneObject*> getDescendants() const;
        
    private:
        SceneClassArray& operator=(const SceneClassArray&);
        
    public:
        
        virtual SceneObject* clone() const { return new SceneClassArray(*this); }
        
        virtual int32_t getNumberOfArrayElements() const { return m_values.size(); }

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE
        
        std::vector<SceneClass*> m_values;

    };
    
#ifdef __SCENE_CLASS_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_CLASS_ARRAY_DECLARE__

} // namespace
#endif  //__SCENE_CLASS_ARRAY__H_
