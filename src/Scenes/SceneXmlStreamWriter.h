#ifndef __SCENE_XML_STREAM_WRITER_H__
#define __SCENE_XML_STREAM_WRITER_H__

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

#include "SceneXmlStreamBase.h"

class QXmlStreamWriter;

namespace caret {

    class Scene;
    class SceneClass;
    class SceneClassArray;
    class SceneEnumeratedType;
    class SceneEnumeratedTypeArray;
    class SceneObject;
    class SceneObjectArray;
    class SceneObjectMapIntegerKey;
    class ScenePathName;
    class ScenePathNameArray;
    class ScenePrimitive;
    class ScenePrimitiveArray;
    
    class SceneXmlStreamWriter : public SceneXmlStreamBase {
        
    public:
        SceneXmlStreamWriter();
        
        virtual ~SceneXmlStreamWriter();
        
        SceneXmlStreamWriter(const SceneXmlStreamWriter&) = delete;

        SceneXmlStreamWriter& operator=(const SceneXmlStreamWriter&) = delete;
        
        void writeXML(QXmlStreamWriter* xmlWriter,
                      const Scene* scene,
                      const int32_t sceneIndex,
                      const AString& sceneFileName);

        // ADD_NEW_METHODS_HERE

    private:
        void writeSceneClass(const SceneClass* sceneClass);
        
        void writeSceneObject(const SceneObject* sceneObject);
        
        void writeArrayObject(const SceneObjectArray* objectArray);
        
        void writeMapObject(const SceneObjectMapIntegerKey* objectMap);
        
        void writeSingleObject(const SceneObject* sceneObject);
        
        void writeEnumeratedType(const SceneEnumeratedType* sceneEnumeratedType);
        
        void writePathName(const ScenePathName* scenePathName);
        
        void writePrimitive(const ScenePrimitive* scenePrimitive);
        
        void writeArrayEnumeratedType(const SceneEnumeratedTypeArray* enumeratedArray);
        
        void writeArrayPrimitiveType(const ScenePrimitiveArray* primitiveArray);
        
        void writeArrayClass(const SceneClassArray* classArray);
        
        void writeArrayPathName(const ScenePathNameArray* pathNameArray);
        
        QXmlStreamWriter* m_xmlWriter = NULL;
        
        AString m_sceneFileName;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_XML_STREAM_WRITER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_XML_STREAM_WRITER_DECLARE__

} // namespace
#endif  //__SCENE_XML_STREAM_WRITER_H__
