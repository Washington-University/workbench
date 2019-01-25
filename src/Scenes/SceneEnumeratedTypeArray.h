#ifndef __SCENE_ENUMERATED_TYPE_ARRAY_H__
#define __SCENE_ENUMERATED_TYPE_ARRAY_H__

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

    class SceneEnumeratedTypeArray : public SceneObjectArray {
        
    public:
        SceneEnumeratedTypeArray(const AString& name,
                               const AString enumeratedValuesAsStrings[],
                               const int32_t numberOfArrayElements);
        
        SceneEnumeratedTypeArray(const AString& name,
                          const std::vector<AString>& enumeratedValuesAsStrings);
        
        SceneEnumeratedTypeArray(const AString& name,
                          const int numberOfArrayElements);
        
        SceneEnumeratedTypeArray(const SceneEnumeratedTypeArray& rhs);

        virtual ~SceneEnumeratedTypeArray();
        
        virtual SceneEnumeratedTypeArray* castToSceneEnumeratedTypeArray();
        
        virtual const SceneEnumeratedTypeArray* castToSceneEnumeratedTypeArray() const;
        
        void setValue(const int32_t arrayIndex,
                      const AString enumeratedValueAsString);
        
        virtual AString stringValue(const int32_t arrayIndex) const;
        
        void stringValues(AString enumeratedValuesAsString[],
                          const int32_t numberOfArrayElements,
                          const AString defaultValue) const;
        
    private:
        SceneEnumeratedTypeArray& operator=(const SceneEnumeratedTypeArray&);
        
    public:
        
        virtual int32_t getNumberOfArrayElements() const { return m_values.size(); }
        
        virtual SceneObject* clone() const { return new SceneEnumeratedTypeArray(*this); }

        // ADD_NEW_METHODS_HERE

    private:

        std::vector<AString> m_values;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_ENUMERATED_TYPE_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_ENUMERATED_TYPE_ARRAY_DECLARE__

} // namespace
#endif  //__SCENE_ENUMERATED_TYPE_ARRAY_H__
