#ifndef __SCENE_FILE_XML_STREAM_BASE_H__
#define __SCENE_FILE_XML_STREAM_BASE_H__

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

    class SceneFileXmlStreamBase : public CaretObject {
        
    public:
        SceneFileXmlStreamBase();
        
        virtual ~SceneFileXmlStreamBase();
        
        SceneFileXmlStreamBase(const SceneFileXmlStreamBase&) = delete;

        SceneFileXmlStreamBase& operator=(const SceneFileXmlStreamBase&) = delete;

        static const QString ELEMENT_SCENE_FILE;
        
        static const QString ATTRIBUTE_SCENE_FILE_VERSION;

        static const QString ELEMENT_SCENE_FILE_INFO_DIRECTORY;
        
        static const QString ELEMENT_SCENE_FILE_BALSA_STUDY_ID;

        static const QString ELEMENT_SCENE_FILE_BALSA_STUDY_TITLE;
        
        static const QString ELEMENT_SCENE_FILE_BALSA_BASE_DIRECTORY;
        
        static const QString ELEMENT_SCENE_FILE_BALSA_EXTRACT_TO_DIRECTORY;
        
        static const QString ELEMENT_SCENE_FILE_BALSA_BASE_PATH_TYPE;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_FILE_XML_STREAM_BASE_DECLARE__
     const QString SceneFileXmlStreamBase::ELEMENT_SCENE_FILE = "SceneFile";
    
     const QString SceneFileXmlStreamBase::ATTRIBUTE_SCENE_FILE_VERSION = "Version";
    
     const QString SceneFileXmlStreamBase::ELEMENT_SCENE_FILE_INFO_DIRECTORY = "SceneInfoDirectory";
    
     const QString SceneFileXmlStreamBase::ELEMENT_SCENE_FILE_BALSA_STUDY_ID = "BalsaStudyID";
    
     const QString SceneFileXmlStreamBase::ELEMENT_SCENE_FILE_BALSA_STUDY_TITLE = "BalsaStudyTitle";
    
     const QString SceneFileXmlStreamBase::ELEMENT_SCENE_FILE_BALSA_BASE_DIRECTORY = "BalsaBaseDirectory";
    
     const QString SceneFileXmlStreamBase::ELEMENT_SCENE_FILE_BALSA_EXTRACT_TO_DIRECTORY = "BalsaExtractToDirectory";
    
     const QString SceneFileXmlStreamBase::ELEMENT_SCENE_FILE_BALSA_BASE_PATH_TYPE = "BasePathType";
#endif // __SCENE_FILE_XML_STREAM_BASE_DECLARE__

} // namespace
#endif  //__SCENE_FILE_XML_STREAM_BASE_H__
