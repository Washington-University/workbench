#ifndef __SCENE_INFO_H__
#define __SCENE_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
#include <stdint.h>
#include "CaretObjectTracksModification.h"



namespace caret {
    class GiftiMetaData;
    class XmlWriter;
    
    class SceneInfo : public CaretObjectTracksModification {
        
    public:
        SceneInfo();
        
        SceneInfo(const SceneInfo& rhs);

        virtual ~SceneInfo();
        
        bool isModified() const override;
        
        void clearModified() override;
        
        AString getName() const;
        
        AString getDescription() const;
        
        void setDescription(const AString& description);
        
        AString getBalsaSceneID() const;
        
        void setBalsaSceneID(const AString& balsaSceneID);
        
        void getImageBytes(QByteArray& imageBytesOut,
                                    AString& imageFormatOut) const;

        void setImageBytes(const QByteArray& imageBytes,
                                    const AString& imageFormat);
        
        bool hasImage() const;
        
        GiftiMetaData* getMetaData();
        
        const GiftiMetaData* getMetaData() const;
        
        void setImageFromText(const AString& text,
                                       const AString& encoding,
                                       const AString& imageFormat);
        
        void addWorkbenchVersionInfoToSceneMetaData();
        
        static void addWorkbenchVersionInfoToMetaData(GiftiMetaData* metaData);
        
        static const AString METADATA_WORKBENCH_COMMIT_DATE_NAME;
        static const AString METADATA_WORKBENCH_COMMIT_NAME;
        static const AString METADATA_WORKBENCH_CURRENT_TIME_NAME;
        static const AString METADATA_WORKBENCH_SYSTEM_INFO;
        static const AString METADATA_WORKBENCH_WORKBENCH_VERSION_NAME;

    private:
        /*
         * setName() is private as users  should call Scene::setName()
         * but the XML readers are allowed to call setName()
         */
        void setName(const AString& sceneName);
        
        SceneInfo& operator=(const SceneInfo&);
        
        static AString removeNamePrefix(const AString& text);
        
        /** name of scene*/
        AString m_sceneName;
        
        /** description of scene */
        AString m_sceneDescription;
        
        /** balsa scene ID */
        AString m_balsaSceneID;
        
        /** thumbnail image bytes */
        QByteArray m_imageBytes;
        
        /** format of thumbnail image (eg: jpg, ppm, etc.) */
        AString m_imageFormat;
        
        /** Metadata for scene */
        std::unique_ptr<GiftiMetaData> m_metaData;
        
        // ADD_NEW_MEMBERS_HERE

        friend class Scene;
        friend class SceneInfoXmlStreamReader;
    };
    
#ifdef __SCENE_INFO_DECLARE__
    const AString SceneInfo::METADATA_WORKBENCH_COMMIT_DATE_NAME       = "WorkbenchCommitDate";
    const AString SceneInfo::METADATA_WORKBENCH_COMMIT_NAME            = "WorkbenchCommit";
    const AString SceneInfo::METADATA_WORKBENCH_CURRENT_TIME_NAME      = "WorkbenchCurrentTime";
    const AString SceneInfo::METADATA_WORKBENCH_SYSTEM_INFO            = "WorkbenchSystem";
    const AString SceneInfo::METADATA_WORKBENCH_WORKBENCH_VERSION_NAME = "WorkbenchVersion";
#endif // __SCENE_INFO_DECLARE__

} // namespace
#endif  //__SCENE_INFO_H__
