#ifndef __META_VOLUME_FILE_XML_STREAM_BASE_H__
#define __META_VOLUME_FILE_XML_STREAM_BASE_H__

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

    class MetaVolumeFileXmlStreamBase : public CaretObject {
        
    public:
        MetaVolumeFileXmlStreamBase();
        
        virtual ~MetaVolumeFileXmlStreamBase();
        
        MetaVolumeFileXmlStreamBase(const MetaVolumeFileXmlStreamBase&) = delete;

        MetaVolumeFileXmlStreamBase& operator=(const MetaVolumeFileXmlStreamBase&) = delete;

        static const QString ELEMENT_META_VOLUME_FILE;
        
        static const QString ATTRIBUTE_META_VOLUME_FILE_VERSION;

        static const QString ELEMENT_MAP_INFO;
        
        static const QString ELEMENT_FILE;
        
        static const QString ELEMENT_MAP;
        
        static const QString ATTRIBUTE_INDEX;
        
        static const QString METADATA_NAME_PALETTE_COLOR_MAPPING;
        
        static const QString METADATA_NAME_MAP_NAME;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_VOLUME_FILE_XML_STREAM_BASE_DECLARE__
    const QString MetaVolumeFileXmlStreamBase::ELEMENT_META_VOLUME_FILE = "MetaVolume";
    
    const QString MetaVolumeFileXmlStreamBase::ATTRIBUTE_META_VOLUME_FILE_VERSION = "Version";

    const QString MetaVolumeFileXmlStreamBase::ELEMENT_MAP_INFO = "MapInfo";
    
    const QString MetaVolumeFileXmlStreamBase::ELEMENT_FILE = "File";
    
    const QString MetaVolumeFileXmlStreamBase::ELEMENT_MAP = "Map";
    
    const QString MetaVolumeFileXmlStreamBase::ATTRIBUTE_INDEX = "Index";
    
    const QString MetaVolumeFileXmlStreamBase::METADATA_NAME_PALETTE_COLOR_MAPPING = "PaletteColorMapping";

    const QString MetaVolumeFileXmlStreamBase::METADATA_NAME_MAP_NAME = "MapName";
    
#endif // __META_VOLUME_FILE_XML_STREAM_BASE_DECLARE__

} // namespace
#endif  //__META_VOLUME_FILE_XML_STREAM_BASE_H__
