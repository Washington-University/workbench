#ifndef __CZI_META_FILE_XML_STREAM_BASE_H__
#define __CZI_META_FILE_XML_STREAM_BASE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

    class CziMetaFileXmlStreamBase : public CaretObject {
        
    public:
        CziMetaFileXmlStreamBase();
        
        virtual ~CziMetaFileXmlStreamBase();
        
        CziMetaFileXmlStreamBase(const CziMetaFileXmlStreamBase&) = delete;

        CziMetaFileXmlStreamBase& operator=(const CziMetaFileXmlStreamBase&) = delete;

        static const QString ATTRIBUTE_FILE;
        
        static const QString ATTRIBUTE_NAME;
        
        static const QString ATTRIBUTE_NUMBER;
        
        static const QString ATTRIBUTE_VERSION;
        
        static const QString ELEMENT_CZI;
        
        static const QString ELEMENT_DISTANCE;
        
        static const QString ELEMENT_HIST_TO_MRI_WARP;
        
        static const QString ELEMENT_META_CZI;
        
        static const QString ELEMENT_MRI_TO_HIST_WARP;
        
        static const QString ELEMENT_PLANE_TO_MM;
        
        static const QString ELEMENT_SCALED_TO_PLANE;
        
        static const QString ELEMENT_SCENE;
        
        static const QString ELEMENT_SLICE;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_META_FILE_XML_STREAM_BASE_DECLARE__
    const QString CziMetaFileXmlStreamBase::ATTRIBUTE_FILE = "File";
    
    const QString CziMetaFileXmlStreamBase::ATTRIBUTE_NAME = "Name";
    
    const QString CziMetaFileXmlStreamBase::ATTRIBUTE_NUMBER = "Number";
    
    const QString CziMetaFileXmlStreamBase::ATTRIBUTE_VERSION = "Version";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_CZI = "CZI";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_DISTANCE = "Distance";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_HIST_TO_MRI_WARP = "HistToMRIWarp";

    const QString CziMetaFileXmlStreamBase::ELEMENT_META_CZI = "MetaCZI";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_MRI_TO_HIST_WARP = "MRItoHistWarp";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_PLANE_TO_MM = "PlaneToMM";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_SCALED_TO_PLANE = "ScaledToPlane";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_SCENE = "Scene";
    
    const QString CziMetaFileXmlStreamBase::ELEMENT_SLICE = "Slice";
#endif // __CZI_META_FILE_XML_STREAM_BASE_DECLARE__

} // namespace
#endif  //__CZI_META_FILE_XML_STREAM_BASE_H__
