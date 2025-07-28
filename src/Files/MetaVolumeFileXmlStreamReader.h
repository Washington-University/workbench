#ifndef __META_VOLUME_FILE_XML_STREAM_READER_H__
#define __META_VOLUME_FILE_XML_STREAM_READER_H__

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


#include <map>
#include <memory>
#include <set>

#include "MetaVolumeFileXmlStreamBase.h"

class QXmlStreamReader;

namespace caret {

    class GiftiMetaData;
    class MetaVolumeFile;
    
    class MetaVolumeFileXmlStreamReader : public MetaVolumeFileXmlStreamBase {
        
    public:
        MetaVolumeFileXmlStreamReader();
        
        virtual ~MetaVolumeFileXmlStreamReader();
        
        MetaVolumeFileXmlStreamReader(const MetaVolumeFileXmlStreamReader&) = delete;

        MetaVolumeFileXmlStreamReader& operator=(const MetaVolumeFileXmlStreamReader&) = delete;

        void readFile(const AString& filename,
                      MetaVolumeFile* metaVolumeFile);

        // ADD_NEW_METHODS_HERE

    private:
        void readFileContent(const AString& metaVolumeFilePath,
                             QXmlStreamReader& xmlReader,
                             MetaVolumeFile* metaVolumeFile);
        
//        void readMapInfo(QXmlStreamReader& xmlReader,
//                         std::map<int32_t, std::unique_ptr<GiftiMetaData>>& metadataOut);
        void readMapInfo(QXmlStreamReader& xmlReader,
                         std::map<int32_t, GiftiMetaData*>& metadataOut);

        AString m_filename;
        
        int32_t m_fileVersion = -1;
        
        std::set<AString> m_unexpectedXmlElements;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_VOLUME_FILE_XML_STREAM_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __META_VOLUME_FILE_XML_STREAM_READER_DECLARE__

} // namespace
#endif  //__META_VOLUME_FILE_XML_STREAM_READER_H__
