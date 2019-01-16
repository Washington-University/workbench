#ifndef __SCENE_PATH_NAME_ARRAY__H_
#define __SCENE_PATH_NAME_ARRAY__H_

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

    class ScenePathName;
    
    class ScenePathNameArray : public SceneObjectArray {
        
    public:
        ScenePathNameArray(const AString& name,
                        const AString pathNameValues[],
                        const int32_t numberOfArrayElements);
        
        ScenePathNameArray(const AString& name,
                        const std::vector<AString>& pathNameValues);
        
        ScenePathNameArray(const AString& name,
                          const int numberOfArrayElements);
        
        ScenePathNameArray(const ScenePathNameArray& rhs);

        virtual ~ScenePathNameArray();
        
        virtual const ScenePathNameArray* castToScenePathNameArray() const;
        
        void setScenePathNameAtIndex(const int32_t arrayIndex,
                                     const AString& sceneFileName,
                                     const AString& pathNameValue);
        
        ScenePathName* getScenePathNameAtIndex(const int32_t arrayIndex) const;
        
        virtual std::vector<SceneObject*> getDescendants() const;
        
    private:
        ScenePathNameArray& operator=(const ScenePathNameArray&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual int32_t getNumberOfArrayElements() const { return m_values.size(); }
        
        virtual SceneObject* clone() const { return new ScenePathNameArray(*this); }

    private:

        // ADD_NEW_MEMBERS_HERE
        
        std::vector<ScenePathName*> m_values;

    };
    
#ifdef __SCENE_PATH_NAME_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_PATH_NAME_ARRAY_DECLARE__

} // namespace
#endif  //__SCENE_PATH_NAME_ARRAY__H_
