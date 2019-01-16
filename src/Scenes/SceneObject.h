#ifndef __SCENE_OBJECT__H_
#define __SCENE_OBJECT__H_

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
#include "SceneObjectContainerTypeEnum.h"
#include "SceneObjectDataTypeEnum.h"

namespace caret {

    class SceneClass;
    class SceneEnumeratedType;
    class SceneObjectArray;
    class SceneObjectMapIntegerKey;
    class ScenePathName;
    class ScenePrimitive;
    
#ifdef CARET_SCENE_DEBUG
    class SceneObject : public CaretObject {
#else   // CARET_SCENE_DEBUG
    class SceneObject {
#endif  // CARET_SCENE_DEBUG
    
    public:
        virtual ~SceneObject();
        
        QString getName() const;
        
        SceneObjectContainerTypeEnum::Enum getContainerType() const;
        
        SceneObjectDataTypeEnum::Enum getDataType() const;
        
        bool isRestored() const;
        
        void setRestored(const bool restoredFlag) const;
        
        void setDescendantsRestored(const bool restoredFlag) const;
        
        static void setRestoredStatus(std::vector<SceneObject*> sceneObjects,
                                      const bool restoredStatus);
        
        static void logObjectsFailedRestore(const AString& sceneName,
                                            const std::vector<SceneObject*> sceneObjects);
        
        /// Should be overridden by any sub-classes that have children
        virtual std::vector<SceneObject*> getDescendants() const;
        
        virtual const SceneClass* castToSceneClass() const;
        
        virtual const SceneEnumeratedType* castToSceneEnumeratedType() const;
        
        virtual const SceneObjectArray* castToSceneObjectArray() const;
        
        virtual const SceneObjectMapIntegerKey* castToSceneObjectMapIntegerKey() const;
        
        virtual const ScenePathName* castToScenePathName() const;
        
        virtual const ScenePrimitive* castToScenePrimitive() const;
        
    protected:
        SceneObject(const QString& name,
                    const SceneObjectContainerTypeEnum::Enum containerType,
                    const SceneObjectDataTypeEnum::Enum dataType);
        
    private:
        SceneObject(const SceneObject&);

        SceneObject& operator=(const SceneObject&);
        
    public:
        virtual AString toString() const;
        
        virtual SceneObject* clone() const = 0;

        // ADD_NEW_METHODS_HERE
        
    private:
        
        // ADD_NEW_MEMBERS_HERE

        /** Name of the item*/
        const QString m_name;
        
        /** Container type of object */
        const SceneObjectContainerTypeEnum::Enum m_containerType;
        
        /** Type of object */
        const SceneObjectDataTypeEnum::Enum m_dataType;
        
    protected:
        mutable bool m_restoredFlag;
        
    private:
        friend class SceneClass;
        friend class SceneClassArray;
        friend class SceneObjectMapIntegerKey;
        friend class ScenePathName;
        friend class ScenePathNameArray;
        
    };
    
#ifdef __SCENE_OBJECT_DECLARE__
#endif // __SCENE_OBJECT_DECLARE__

} // namespace
#endif  //__SCENE_OBJECT__H_
