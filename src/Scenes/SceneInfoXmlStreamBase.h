#ifndef __SCENE_INFO_XML_STREAM_BASE_H__
#define __SCENE_INFO_XML_STREAM_BASE_H__

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

    class SceneInfoXmlStreamBase : public CaretObject {
        
    public:
        SceneInfoXmlStreamBase();
        
        virtual ~SceneInfoXmlStreamBase();
        
        SceneInfoXmlStreamBase(const SceneInfoXmlStreamBase&) = delete;

        SceneInfoXmlStreamBase& operator=(const SceneInfoXmlStreamBase&) = delete;
        
        static const AString ELEMENT_BALSA_SCENE_ID;
        static const AString ELEMENT_DESCRIPTION;
        static const AString ELEMENT_IMAGE;
        static const AString ELEMENT_NAME;
        static const AString ELEMENT_SCENE_INFO;
        
        static const AString ATTRIBUTE_ENCODING;
        static const AString ATTRIBUTE_FORMAT;
        static const AString ATTRIBUTE_INDEX;
        
        static const AString VALUE_ENCODING_BASE64;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_INFO_XML_STREAM_BASE_DECLARE__
    const AString SceneInfoXmlStreamBase::ELEMENT_BALSA_SCENE_ID = "BalsaSceneID";
    const AString SceneInfoXmlStreamBase::ELEMENT_DESCRIPTION    = "Description";
    const AString SceneInfoXmlStreamBase::ELEMENT_IMAGE          = "Image";
    const AString SceneInfoXmlStreamBase::ELEMENT_NAME           = "Name";
    const AString SceneInfoXmlStreamBase::ELEMENT_SCENE_INFO     = "SceneInfo";
    
    const AString SceneInfoXmlStreamBase::ATTRIBUTE_ENCODING = "Encoding";
    const AString SceneInfoXmlStreamBase::ATTRIBUTE_FORMAT   = "Format";
    const AString SceneInfoXmlStreamBase::ATTRIBUTE_INDEX    = "Index";
    
    const AString SceneInfoXmlStreamBase::VALUE_ENCODING_BASE64 = "Base64";
#endif // __SCENE_INFO_XML_STREAM_BASE_DECLARE__

} // namespace
#endif  //__SCENE_INFO_XML_STREAM_BASE_H__
