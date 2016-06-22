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
#include "SceneObjectDataTypeEnum.h"

namespace caret {

#ifdef CARET_SCENE_DEBUG
    class SceneObject : public CaretObject {
#else   // CARET_SCENE_DEBUG
    class SceneObject {
#endif  // CARET_SCENE_DEBUG
    
    public:
        virtual ~SceneObject();
        
        QString getName() const;
        
        SceneObjectDataTypeEnum::Enum getDataType() const;
        
    protected:
        SceneObject(const QString& name,
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
        
        /** Type of object */
        const SceneObjectDataTypeEnum::Enum m_dataType;
        
    };
    
#ifdef __SCENE_OBJECT_DECLARE__
#endif // __SCENE_OBJECT_DECLARE__

} // namespace
#endif  //__SCENE_OBJECT__H_
