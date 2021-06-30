#ifndef __SCENE_UNSIGNED_BYTE__H_
#define __SCENE_UNSIGNED_BYTE__H_

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


#include "ScenePrimitive.h"

namespace caret {

    class SceneUnsignedByte : public ScenePrimitive {
        
    public:
        SceneUnsignedByte(const AString& name,
                     const uint8_t value);
        
        SceneUnsignedByte(const SceneUnsignedByte& rhs);

        virtual ~SceneUnsignedByte();
        
        void setValue(const uint8_t value);
        
        virtual bool booleanValue() const;
        
        virtual float floatValue() const;
        
        virtual int32_t integerValue() const;
        
        virtual int64_t longIntegerValue() const override;

        virtual AString stringValue() const;
        
        virtual uint8_t unsignedByteValue() const;
        
    private:
        SceneUnsignedByte& operator=(const SceneUnsignedByte&);
        
    public:
        
        virtual SceneObject* clone() const;

        // ADD_NEW_METHODS_HERE

    private:

        uint8_t m_value;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_UNSIGNED_BYTE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_UNSIGNED_BYTE_DECLARE__

} // namespace
#endif  //__SCENE_UNSIGNED_BYTE__H_
