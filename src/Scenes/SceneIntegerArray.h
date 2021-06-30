#ifndef __SCENE_INTEGER_ARRAY_H__
#define __SCENE_INTEGER_ARRAY_H__

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


#include "ScenePrimitiveArray.h"

namespace caret {

    class SceneIntegerArray : public ScenePrimitiveArray {
        
    public:
        SceneIntegerArray(const AString& name,
                               const int32_t values[],
                               const int32_t numberOfArrayElements);
        
        SceneIntegerArray(const AString& name,
                          const std::vector<int32_t>& values);
        
        SceneIntegerArray(const AString& name,
                          const int numberOfArrayElements);
        
        SceneIntegerArray(const SceneIntegerArray& rhs);

        virtual ~SceneIntegerArray();
        
        void setValue(const int32_t arrayIndex,
                      const int32_t value);
        
        virtual bool booleanValue(const int32_t arrayIndex) const;
        
        virtual float floatValue(const int32_t arrayIndex) const;
        
        virtual int32_t integerValue(const int32_t arrayIndex) const;
        
        virtual int64_t longIntegerValue(const int32_t arrayIndex) const override;
        
        virtual AString stringValue(const int32_t arrayIndex) const;
        
        virtual uint8_t unsignedByteValue(const int32_t arrayIndex) const;
        
    private:
        SceneIntegerArray& operator=(const SceneIntegerArray&);
        
    public:
        
        virtual int32_t getNumberOfArrayElements() const { return m_values.size(); }
        
        virtual SceneObject* clone() const { return new SceneIntegerArray(*this); }

        // ADD_NEW_METHODS_HERE

    private:

        std::vector<int32_t> m_values;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_INTEGER_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_INTEGER_ARRAY_DECLARE__

} // namespace
#endif  //__SCENE_INTEGER_ARRAY_H__
