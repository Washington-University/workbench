#ifndef __SCENE_BOOLEAN__H_
#define __SCENE_BOOLEAN__H_

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

    class SceneBoolean : public ScenePrimitive {
        
    public:
        SceneBoolean(const AString& name,
                     const bool value);
        
        void setValue(const bool value);
        
        virtual ~SceneBoolean();
        
        virtual bool booleanValue() const;
        
        virtual float floatValue() const;
        
        virtual int32_t integerValue() const;
        
        virtual AString stringValue() const;
        
        virtual uint8_t unsignedByteValue() const;
        
    private:
        SceneBoolean(const SceneBoolean&);

        SceneBoolean& operator=(const SceneBoolean&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        bool m_value;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_BOOLEAN_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_BOOLEAN_DECLARE__

} // namespace
#endif  //__SCENE_BOOLEAN__H_
