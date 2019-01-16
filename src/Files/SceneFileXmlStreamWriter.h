#ifndef __SCENE_FILE_XML_STREAM_WRITER_H__
#define __SCENE_FILE_XML_STREAM_WRITER_H__

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

#include "SceneFileXmlStreamBase.h"

class QXmlStreamWriter;

namespace caret {
    class SceneFile;
    
    class SceneFileXmlStreamWriter : public SceneFileXmlStreamBase {
        
    public:
        SceneFileXmlStreamWriter();
        
        virtual ~SceneFileXmlStreamWriter();
        
        SceneFileXmlStreamWriter(const SceneFileXmlStreamWriter&) = delete;

        SceneFileXmlStreamWriter& operator=(const SceneFileXmlStreamWriter&) = delete;

        void writeFile(const SceneFile* sceneFile);

        // ADD_NEW_METHODS_HERE

    private:
        void writeFileContentToXmlStreamWriter(QXmlStreamWriter& xmlWriter,
                                               const SceneFile* sceneFile,
                                               const AString& sceneFileName);
        
        void writeSceneInfoDirectory(QXmlStreamWriter& xmlWriter,
                                     const SceneFile* sceneFile,
                                     const AString& sceneFileName);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_FILE_XML_STREAM_WRITER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_FILE_XML_STREAM_WRITER_DECLARE__

} // namespace
#endif  //__SCENE_FILE_XML_STREAM_WRITER_H__
