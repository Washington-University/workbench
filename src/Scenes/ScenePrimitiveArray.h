#ifndef __SCENE_PRIMITIVE_ARRAY__H_
#define __SCENE_PRIMITIVE_ARRAY__H_

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

    class ScenePrimitiveArray : public SceneObjectArray {
        
    public:
        virtual ~ScenePrimitiveArray();

        virtual const ScenePrimitiveArray* castToScenePrimitiveArray() const;
        
    protected:
        ScenePrimitiveArray(const QString& name,
                            const SceneObjectDataTypeEnum::Enum dataType);
        
    private:
        ScenePrimitiveArray(const ScenePrimitiveArray&);

        ScenePrimitiveArray& operator=(const ScenePrimitiveArray&);
        
    public:

        // ADD_NEW_METHODS_HERE
        
        /** 
         * Get the values as a boolean. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual bool booleanValue(const int32_t arrayIndex) const = 0;

        /** 
         * Get the values as a float. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual float floatValue(const int32_t arrayIndex) const = 0;
        
        /** 
         * Get the values as a integer. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual int32_t integerValue(const int32_t arrayIndex) const = 0;
        
        /** 
         * Get the values as a string. 
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual AString stringValue(const int32_t arrayIndex) const = 0;
        
        /**
         * Get the values as an unsigned byte.
         * @param arrayIndex
         *    Index of element.
         * @return The value.
         */
        virtual uint8_t unsignedByteValue(const int32_t arrayIndex) const = 0;
        
        virtual void booleanValues(bool valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const bool defaultValue) const;
        
        virtual void booleanValues(std::vector<bool>& valuesOut,
                                   const bool defaultValue) const;
        
        virtual void booleanVectorValues(std::vector<bool>& valuesOut) const;
        
        virtual void floatValues(float valuesOut[],
                           const int32_t arrayNumberOfElements,
                           const float defaultValue) const;
        
        virtual void floatValues(std::vector<float>& valuesOut,
                                   const float defaultValue) const;
        
        virtual void floatVectorValues(std::vector<float>& valuesOut) const;
        
        virtual void integerValues(int32_t valuesOut[],
                                 const int32_t arrayNumberOfElements,
                                 const int32_t defaultValue) const;
        
        virtual void integerValues(std::vector<int32_t>& valuesOut,
                                 const int32_t defaultValue) const;
        
        virtual void integerVectorValues(std::vector<int32_t>& valuesOut) const;
        
        virtual void stringValues(AString valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const AString& defaultValue) const;
        
        virtual void stringValues(std::vector<AString>& valuesOut,
                                   const AString& defaultValue) const;
        
        virtual void stringVectorValues(std::vector<AString>& valuesOut) const;
        
        virtual void unsignedByteValues(uint8_t valuesOut[],
                                   const int32_t arrayNumberOfElements,
                                   const uint8_t defaultValue) const;
        
        virtual void unsignedByteValues(std::vector<uint8_t>& valuesOut,
                                   const uint8_t defaultValue) const;
        
        virtual void unsignedByteVectorValues(std::vector<uint8_t>& valuesOut) const;
        
        
    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_PRIMITIVE_ARRAY_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_PRIMITIVE_ARRAY_DECLARE__

} // namespace
#endif  //__SCENE_PRIMITIVE_ARRAY__H_
