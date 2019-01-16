#ifndef __SCENE_XML_STREAM_BASE_H__
#define __SCENE_XML_STREAM_BASE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"



namespace caret {
    
    class SceneXmlStreamBase : public CaretObject {
        
    public:
        SceneXmlStreamBase();
        
        virtual ~SceneXmlStreamBase();
        
        SceneXmlStreamBase(const SceneXmlStreamBase&) = delete;
        
        SceneXmlStreamBase& operator=(const SceneXmlStreamBase&) = delete;
        
        static const AString ELEMENT_SCENE;
        static const AString ELEMENT_SCENE_DESCRIPTION;
        static const AString ELEMENT_SCENE_NAME;

        static const AString ATTRIBUTE_SCENE_INDEX;
        static const AString ATTRIBUTE_SCENE_TYPE;
        
        static const AString ELEMENT_OBJECT;
        
        static const AString ATTRIBUTE_OBJECT_CLASS;
        static const AString ATTRIBUTE_OBJECT_NAME;
        static const AString ATTRIBUTE_OBJECT_TYPE;
        static const AString ATTRIBUTE_OBJECT_VERSION;
        
        
        static const AString ELEMENT_OBJECT_ARRAY;
        
        static const AString ATTRIBUTE_OBJECT_ARRAY_NAME;
        static const AString ATTRIBUTE_OBJECT_ARRAY_LENGTH;
        static const AString ATTRIBUTE_OBJECT_ARRAY_TYPE;
        
        static const AString ELEMENT_OBJECT_ARRAY_ELEMENT;
        
        static const AString ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX;
        
        static const AString ELEMENT_OBJECT_MAP;
        
        static const AString ATTRIBUTE_OBJECT_MAP_NAME;
        static const AString ATTRIBUTE_OBJECT_MAP_TYPE;
        
        static const AString ELEMENT_OBJECT_MAP_VALUE;
        
        static const AString ATTRIBUTE_OBJECT_MAP_VALUE_KEY;
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;
        
    private:
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __SCENE_XML_STREAM_BASE_DECLARE__
    const AString SceneXmlStreamBase::ELEMENT_SCENE             = "Scene";
    const AString SceneXmlStreamBase::ELEMENT_SCENE_DESCRIPTION = "Description";
    const AString SceneXmlStreamBase::ELEMENT_SCENE_NAME        = "Name";
    
    const AString SceneXmlStreamBase::ATTRIBUTE_SCENE_INDEX = "Index";
    const AString SceneXmlStreamBase::ATTRIBUTE_SCENE_TYPE  = "Type";
    
    const AString SceneXmlStreamBase::ELEMENT_OBJECT = "Object";
    
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_CLASS   = "Class";
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_NAME    = "Name";
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_TYPE    = "Type";
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_VERSION = "Version";
    
    
    const AString SceneXmlStreamBase::ELEMENT_OBJECT_ARRAY = "ObjectArray";
    
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_ARRAY_NAME = "Name";
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_ARRAY_LENGTH = "Length";
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_ARRAY_TYPE = "Type";
    
    const AString SceneXmlStreamBase::ELEMENT_OBJECT_ARRAY_ELEMENT = "Element";
    
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX = "Index";
    
    const AString SceneXmlStreamBase::ELEMENT_OBJECT_MAP = "ObjectMap";
    
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_MAP_NAME = "Name";
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_MAP_TYPE = "Type";
    
    const AString SceneXmlStreamBase::ELEMENT_OBJECT_MAP_VALUE = "Value";
    
    const AString SceneXmlStreamBase::ATTRIBUTE_OBJECT_MAP_VALUE_KEY = "Key";
#endif // __SCENE_XML_STREAM_BASE_DECLARE__
    
} // namespace
#endif  //__SCENE_XML_STREAM_BASE_H__
