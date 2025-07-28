#ifndef __META_VOLUME_FILE_XML_STREAM_WRITER_H__
#define __META_VOLUME_FILE_XML_STREAM_WRITER_H__

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

#include "MetaVolumeFileXmlStreamBase.h"

class QXmlStreamWriter;

namespace caret {
    class MetaVolumeFile;
    
    class MetaVolumeFileXmlStreamWriter : public MetaVolumeFileXmlStreamBase {
        
    public:
        MetaVolumeFileXmlStreamWriter();
        
        virtual ~MetaVolumeFileXmlStreamWriter();
        
        MetaVolumeFileXmlStreamWriter(const MetaVolumeFileXmlStreamWriter&) = delete;

        MetaVolumeFileXmlStreamWriter& operator=(const MetaVolumeFileXmlStreamWriter&) = delete;

        void writeFile(const MetaVolumeFile* metaVolumeFile);

        // ADD_NEW_METHODS_HERE

    private:
        void writeFileContentToXmlStreamWriter(QXmlStreamWriter& xmlWriter,
                                               const MetaVolumeFile* metaVolumeFile);
        
        void writeMapInfoDirectory(QXmlStreamWriter& xmlWriter,
                                   const MetaVolumeFile* metaVolumeFile);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_VOLUME_FILE_XML_STREAM_WRITER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __META_VOLUME_FILE_XML_STREAM_WRITER_DECLARE__

} // namespace
#endif  //__META_VOLUME_FILE_XML_STREAM_WRITER_H__
