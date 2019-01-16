#ifndef __SCENE_PRIMITIVE__H_
#define __SCENE_PRIMITIVE__H_

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

    class ScenePrimitive : public SceneObject {
        
    public:
        virtual ~ScenePrimitive();

        virtual const ScenePrimitive* castToScenePrimitive() const;
        
    protected:
        ScenePrimitive(const QString& name,
                       const SceneObjectDataTypeEnum::Enum dataType);
        
    private:
        ScenePrimitive(const ScenePrimitive&);

        ScenePrimitive& operator=(const ScenePrimitive&);
        
        virtual AString toString() const;
        
    public:

        // ADD_NEW_METHODS_HERE
        
        /** @return Value as a boolean data type */
        virtual bool booleanValue() const = 0;

        /** @return Value as a float data type */
        virtual float floatValue() const = 0;
        
        /** @return Value as a integer data type */
        virtual int32_t integerValue() const = 0;
        
        /** @return Value as a string data type */
        virtual AString stringValue() const = 0;
                
        /** @return Value as a unsigned byte data type */
        virtual uint8_t unsignedByteValue() const = 0;
        
        virtual SceneObject* clone() const = 0;
        
    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_PRIMITIVE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_PRIMITIVE_DECLARE__

} // namespace
#endif  //__SCENE_PRIMITIVE__H_
