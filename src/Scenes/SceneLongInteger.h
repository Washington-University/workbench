#ifndef __SCENE_LONG_INTEGER__H_
#define __SCENE_LONG_INTEGER__H_

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

    class SceneLongInteger : public ScenePrimitive {
        
    public:
        SceneLongInteger(const AString& name,
                         const int64_t value);
        
        SceneLongInteger(const SceneLongInteger& rhs);

        virtual ~SceneLongInteger();
        
        void setValue(const int64_t value);
        
        virtual bool booleanValue() const;
        
        virtual float floatValue() const;
        
        virtual int32_t integerValue() const;
        
        virtual int64_t longIntegerValue() const override;

        virtual AString stringValue() const;
        
        virtual uint8_t unsignedByteValue() const;
        
    private:
        SceneLongInteger& operator=(const SceneLongInteger&);
        
    public:
        
        virtual SceneObject* clone() const;

        // ADD_NEW_METHODS_HERE

    private:

        int64_t m_value;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_LONG_INTEGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_LONG_INTEGER_DECLARE__

} // namespace
#endif  //__SCENE_LONG_INTEGER__H_
