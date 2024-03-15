#ifndef __HISTOLOGY_SLICES_FILE_XML_STREAM_BASE_H__
#define __HISTOLOGY_SLICES_FILE_XML_STREAM_BASE_H__

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

    class HistologySlicesFileXmlStreamBase : public CaretObject {
        
    public:
        HistologySlicesFileXmlStreamBase();
        
        virtual ~HistologySlicesFileXmlStreamBase();
        
        HistologySlicesFileXmlStreamBase(const HistologySlicesFileXmlStreamBase&) = delete;

        HistologySlicesFileXmlStreamBase& operator=(const HistologySlicesFileXmlStreamBase&) = delete;

        static const QString ATTRIBUTE_FILE;
        
        static const QString ATTRIBUTE_SCENE_NAME;
        
        static const QString ATTRIBUTE_SLICE_NAME;
        
        static const QString ATTRIBUTE_SLICE_NUMBER;
        
        static const QString ATTRIBUTE_VERSION;
        
        static const QString ELEMENT_CZI;
        
        static const QString ELEMENT_DISTANCE;
        
        static const QString ELEMENT_HIST_TO_MRI_WARP;
        
        static const QString ELEMENT_IMAGE;
        
        static const QString ELEMENT_META_CZI;
        
        static const QString ELEMENT_META_IMAGE;
        
        static const QString ELEMENT_MRI_TO_HIST_WARP;
        
        static const QString ELEMENT_PLANE_TO_MM;
        
        static const QString ELEMENT_SCALED_TO_PLANE;
        
        static const QString ELEMENT_SCENE;
        
        static const QString ELEMENT_SLICE;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HISTOLOGY_SLICES_FILE_XML_STREAM_BASE_DECLARE__
    const QString HistologySlicesFileXmlStreamBase::ATTRIBUTE_FILE = "File";
    
    const QString HistologySlicesFileXmlStreamBase::ATTRIBUTE_SCENE_NAME = "Name";
    
    const QString HistologySlicesFileXmlStreamBase::ATTRIBUTE_SLICE_NAME = "Name";
    
    const QString HistologySlicesFileXmlStreamBase::ATTRIBUTE_SLICE_NUMBER = "Number";
    
    const QString HistologySlicesFileXmlStreamBase::ATTRIBUTE_VERSION = "Version";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_CZI = "CZI";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_DISTANCE = "Distance";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_HIST_TO_MRI_WARP = "HistToMRIWarp";

    const QString HistologySlicesFileXmlStreamBase::ELEMENT_IMAGE = "Image";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_META_CZI = "MetaCZI";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_META_IMAGE = "MetaImage";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_MRI_TO_HIST_WARP = "MRItoHistWarp";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_PLANE_TO_MM = "PlaneToMM";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_SCALED_TO_PLANE = "ScaledToPlane";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_SCENE = "Scene";
    
    const QString HistologySlicesFileXmlStreamBase::ELEMENT_SLICE = "Slice";
#endif // __HISTOLOGY_SLICES_FILE_XML_STREAM_BASE_DECLARE__

} // namespace
#endif  //__HISTOLOGY_SLICES_FILE_XML_STREAM_BASE_H__
