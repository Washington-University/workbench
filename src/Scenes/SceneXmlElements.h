#ifndef __SCENE_XML_ELEMENTS__H_
#define __SCENE_XML_ELEMENTS__H_

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


/**
 * \class caret::SceneXmlElements 
 * \brief XML Elements for Scenes written with XML.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 *
 * Format of tags is:
 *    XML_<class-name>_<name>_<TAG | ATTRIBUTE>
 *
 * Since the user names scenes, CDATA is used for scene
 * name.
 */


namespace caret {

    namespace SceneXmlElements {
        
        /** XML Tag for scene */
        static const AString SCENE_TAG = "Scene";
        
        /** XML Tag for scene name */
        static const AString SCENE_NAME_TAG = "Name";
        
        /** XML Tag for scene description */
        static const AString SCENE_DESCRIPTION_TAG = "Description";
        
        /** XML Attribute for scene index */
        static const AString SCENE_INDEX_ATTRIBUTE = "Index";
        
        /** XML Attribute for scene type */
        static const AString SCENE_TYPE_ATTRIBUTE = "Type";
        
        /** XML Tag for scene object */
        static const AString OBJECT_TAG = "Object";
        
        /** XML Tag for scene object arrays */
        static const AString OBJECT_ARRAY_TAG = "ObjectArray";
        
        /** XML Tag for scene object map */
        static const AString OBJECT_MAP_TAG = "ObjectMap";
        
        /** XML Attributes for scene object type */
        static const AString OBJECT_TYPE_ATTRIBUTE = "Type";
                
        /** XML Attributes for scene object name */
        static const AString OBJECT_NAME_ATTRIBUTE = "Name";
        
        /** 
         * XML Attributes for scene object class.  Required when
         * 'type' is a class and contains the name of the class.
         */
        static const AString OBJECT_CLASS_ATTRIBUTE = "Class";
        
        /** 
         * XML Attributes for scene object array length.
         */
        static const AString OBJECT_ARRAY_LENGTH_ATTRIBUTE = "Length";
        
        /**
         * XML Tag Element for array element
         */
        static const AString OBJECT_ARRAY_ELEMENT_TAG = "Element";
        
        /**
         * XML Attribute for element array index
         */
        static const AString OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE = "Index";
        
        /** XML Tag for ObjectMap Value */
        static const AString OBJECT_MAP_VALUE_TAG = "Value";
        
        /** XML Tag for ObjectMap Key */
        static const AString OBJECT_MAP_VALUE_KEY_ATTRIBUTE = "Key";
        
        /** 
         * XML Attributes for scene object version.  Only valid when
         * 'type' is a class.  Since a class may change (members added,
         * members deleted, member's type changed, etc), the version
         * can be used to handle previous versions of a class.
         */
        static const AString OBJECT_VERSION_ATTRIBUTE = "Version";
    
        /**
         * XML Tag for Scene Info element.
         */
        static const AString SCENE_INFO_TAG = "SceneInfo";
    
        /**
         * XML Tag for Scene Info index attribute.
         */
        static const AString SCENE_INFO_INDEX_ATTRIBUTE = "Index";
    
        /**
         * XML Tag for Scene Info name element.
         */
        static const AString SCENE_INFO_NAME_TAG = "Name";

        /**
         * XML Tag for Scene Info description element.
         */
        static const AString SCENE_INFO_DESCRIPTION_TAG = "Description";
    
        /**
         * XML Tag for Scene Info Image element.
         */
        static const AString SCENE_INFO_IMAGE_TAG = "Image";
    
        /**
         * XML Tag for Scene Info Balsa Scene ID element.
         */
        static const AString SCENE_INFO_BALSA_SCENE_ID_TAG = "BalsaSceneID";
        
        /**
         * XML Tag for Scene Info image format attribute.
         */
        static const AString SCENE_INFO_IMAGE_FORMAT_ATTRIBUTE = "Format";
        
        /**
         * XML Tag for Scene Info image encoding attribute.
         */
        static const AString SCENE_INFO_IMAGE_ENCODING_ATTRIBUTE = "Encoding";
    
        /**
         * Name for Base64 encoding used in Scene Info image encoding attribute.
         */
        static const AString SCENE_INFO_ENCODING_BASE64_NAME = "Base64";
        
        /**
         * XML Tag for BALSA Study ID element.
         */
        static const AString SCENE_INFO_BALSA_STUDY_ID_TAG = "BalsaStudyID";
    
        
        /**
         * XML Tag for Base Directory ID element.
         */
        static const AString SCENE_INFO_BASE_DIRECTORY_TAG = "BaseDirectory";
        
    } // namespace SceneXmlElements
    
} // namespace caret
#endif  //__SCENE_XML_ELEMENTS__H_
