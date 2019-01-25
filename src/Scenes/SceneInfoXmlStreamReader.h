#ifndef __SCENE_INFO_XML_STREAM_READER_H__
#define __SCENE_INFO_XML_STREAM_READER_H__

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
#include <set>

#include "SceneInfoXmlStreamBase.h"

class QXmlStreamReader;

namespace caret {

    class SceneInfo;
    
    class SceneInfoXmlStreamReader : public SceneInfoXmlStreamBase {
        
    public:
        SceneInfoXmlStreamReader();
        
        virtual ~SceneInfoXmlStreamReader();
        
        SceneInfoXmlStreamReader(const SceneInfoXmlStreamReader&) = delete;

        SceneInfoXmlStreamReader& operator=(const SceneInfoXmlStreamReader&) = delete;
        
        void readSceneInfo(QXmlStreamReader& xmlReader,
                           SceneInfo* sceneInfo);

        std::set<AString> getUnexpectedElements() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        std::set<AString> m_unexpectedXmlElements;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_INFO_XML_STREAM_READER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_INFO_XML_STREAM_READER_DECLARE__

} // namespace
#endif  //__SCENE_INFO_XML_STREAM_READER_H__
