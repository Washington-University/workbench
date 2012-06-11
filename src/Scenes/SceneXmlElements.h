#ifndef __SCENE_XML_ELEMENTS__H_
#define __SCENE_XML_ELEMENTS__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


/**
 * \class caret::SceneXmlElements 
 * \brief XML Elements for Scenes written with XML.
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
        static const AString SCENE_TAG = "scene";
        
        /** XML Tag for scene name */
        static const AString SCENE_NAME_TAG = "name";
        
        /** XML Attribute for scene index */
        static const AString SCENE_INDEX_ATTRIBUTE = "index";
        
        /** XML Attribute for scene type */
        static const AString SCENE_TYPE_ATTRIBUTE = "type";
        
        /** XML Tag for scene object */
        static const AString OBJECT_TAG = "object";
        
        /** XML Attributes for scene object type */
        static const AString OBJECT_TYPE_ATTRIBUTE = "type";
                
        /** XML Attributes for scene object name */
        static const AString OBJECT_NAME_ATTRIBUTE = "name";
        
        /** 
         * XML Attributes for scene object class.  Required when
         * 'type' is a class and contains the name of the class.
         */
        static const AString OBJECT_CLASS_ATTRIBUTE = "class";
        
        /** 
         * XML Attributes for scene object length.  Only valid when
         * 'type' is an array.
         */
        //static const AString OBJECT_LENGTH_ATTRIBUTE = "length";
        
        /** 
         * XML Attributes for scene object version.  Only valid when
         * 'type' is a class.  Since a class may change (members added,
         * members deleted, member's type changed, etc), the version
         * can be used to handle previous versions of a class.
         */
        static const AString OBJECT_VERSION_ATTRIBUTE = "version";
        
        
    } // namespace SceneXmlElements
    
} // namespace caret
#endif  //__SCENE_XML_ELEMENTS__H_
