#ifndef __SCENE_ENUMERATED_TYPE__H_
#define __SCENE_ENUMERATED_TYPE__H_

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

    class SceneEnumeratedType : public SceneObject {
        
    public:
        SceneEnumeratedType(const AString& name,
                            const AString& enumeratedValueAsString);
        
        virtual ~SceneEnumeratedType();
        
        void setValue(const AString& enumeratedValueAsString);
        
        AString stringValue() const;
        
    private:
        SceneEnumeratedType(const SceneEnumeratedType&);

        SceneEnumeratedType& operator=(const SceneEnumeratedType&);
        
    public:

        // ADD_NEW_METHODS_HERE
        
        virtual SceneObject* clone() const;

    private:

        // ADD_NEW_MEMBERS_HERE

        AString m_enumeratedValueAsString;
    };
    
#ifdef __SCENE_ENUMERATED_TYPE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_ENUMERATED_TYPE_DECLARE__

} // namespace
#endif  //__SCENE_ENUMERATED_TYPE__H_
