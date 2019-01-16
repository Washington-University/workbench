#ifndef __SCENE_OBJECT_ARRAY__H_
#define __SCENE_OBJECT_ARRAY__H_

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


#include "SceneObject.h"

namespace caret {
    class SceneClassArray;
    class SceneEnumeratedTypeArray;
    class ScenePathNameArray;
    class ScenePrimitiveArray;

    class SceneObjectArray : public SceneObject {
        
    public:
        SceneObjectArray(const QString& name,
                         const SceneObjectDataTypeEnum::Enum dataType);
        
        virtual ~SceneObjectArray();
        
        virtual const SceneObjectArray* castToSceneObjectArray() const;
        
        virtual const SceneClassArray* castToSceneClassArray() const;
        
        virtual const SceneEnumeratedTypeArray* castToSceneEnumeratedTypeArray() const;
        
        virtual const ScenePathNameArray* castToScenePathNameArray() const;
        
        virtual const ScenePrimitiveArray* castToScenePrimitiveArray() const;
        
    private:
        SceneObjectArray(const SceneObjectArray&);

        SceneObjectArray& operator=(const SceneObjectArray&);
        
    public:

        // ADD_NEW_METHODS_HERE

        virtual SceneObject* clone() const = 0;

        virtual int32_t getNumberOfArrayElements() const = 0;
        
    protected:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_OBJECT_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_OBJECT_ARRAY_DECLARE__

} // namespace
#endif  //__SCENE_OBJECT_ARRAY__H_
