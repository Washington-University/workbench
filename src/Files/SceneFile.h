#ifndef __SCENE_FILE__H_
#define __SCENE_FILE__H_

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


#include "CaretDataFile.h"
#include "SceneFileBasePathTypeEnum.h"

namespace caret {

    class Scene;
    
    class SceneFile : public CaretDataFile {
        
    public:
        SceneFile();
        
        virtual ~SceneFile();
        
    private:
        SceneFile(const SceneFile&);

        SceneFile& operator=(const SceneFile&);
        
    public:

        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        void clear();
        
        void readFile(const AString& filename);
        
        void writeFile(const AString& filename);
        
        bool isEmpty() const;

        void addScene(Scene* scene);
        
        int32_t getIndexOfScene(const Scene* scene) const;
        
        void insertScene(Scene* newScene,
                         const Scene* insertAboveThisScene);
        
        void replaceScene(Scene* newScene,
                          Scene* sceneThatIsReplacedAndDeleted);
        
        int32_t getNumberOfScenes() const;
        
        Scene* getSceneAtIndex(const int32_t indx) const;
        
        Scene* getSceneWithName(const AString& sceneName);
        
        void moveScene(Scene* scene,
                       const int32_t indexDelta);
        
        void removeScene(Scene* scene);
        
        void removeSceneAtIndex(const int32_t indx);
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        GiftiMetaData* getFileMetaData();
        
        const GiftiMetaData* getFileMetaData() const;
        
        AString getBalsaStudyID() const;
        
        void setBalsaStudyID(const AString& balsaStudyID);
        
        AString getBalsaStudyTitle() const;
        
        void setBalsaStudyTitle(const AString& balsaStudyTitle);
        
        SceneFileBasePathTypeEnum::Enum getBasePathType() const;
        
        void setBasePathType(const SceneFileBasePathTypeEnum::Enum basePathType);
        
        AString getBalsaCustomBaseDirectory() const;
        
        void setBalsaCustomBaseDirectory(const AString& balsaBaseDirectory);
        
        AString findBaseDirectoryForDataFiles(std::vector<AString>& missingFileNamesOut) const;
        
        std::vector<AString> getBaseDirectoryHierarchyForDataFiles(const int32_t maximumAncestorCount = 25);
        
        std::vector<AString> getAllDataFileNamesFromAllScenes() const;
        
        void reorderScenes(std::vector<Scene*>& orderedScenes);
        
        int32_t getSceneIndexFromNumberOrName(const AString& numberOrName);
        
        Scene* releaseScene(const int32_t& index);

        AString getDefaultZipFileName() const;
        
        AString getBalsaExtractToDirectoryName() const;
        
        void setBalsaExtractToDirectoryName(const AString& balsaExtractToDirectoryName);

        AString getDefaultExtractToDirectoryName() const;
        
        virtual bool isModified() const override;
        
        virtual void clearModified() override;
        
        // ADD_NEW_METHODS_HERE

        /** Version of file */
        static float getFileVersion() { return s_sceneFileVersion; }
        
        /** XML Tag for scene file */
        static const AString XML_TAG_SCENE_FILE;
        
        /**
         * XML Tag for Scene Info Directory element.
         */
        static const AString XML_TAG_SCENE_INFO_DIRECTORY_TAG;
        
        /** XML Tag for Version attribute */
        static const AString XML_ATTRIBUTE_VERSION;
        
    private:

        /** the scenes*/
        std::vector<Scene*> m_scenes;

        /** the metadata */
        GiftiMetaData* m_metadata;

        /** the BALSA Study ID */
        AString m_balsaStudyID;
        
        /** the BALSA Study Title */
        AString m_balsaStudyTitle;
        
        /** the Base Path Type */
        SceneFileBasePathTypeEnum::Enum m_basePathType;
        
        /** the Custom Base Directory */
        AString m_balsaCustomBaseDirectory;
        
        /** The "extract to" directory name */
        AString m_balsaExtractToDirectoryName;
        
        // ADD_NEW_MEMBERS_HERE

        /** Version of this SceneFile */
        static const float s_sceneFileVersion;
    };
    
#ifdef __SCENE_FILE_DECLARE__
    const AString SceneFile::XML_TAG_SCENE_FILE = "SceneFile";
    const AString SceneFile::XML_ATTRIBUTE_VERSION = "Version";
    const AString SceneFile::XML_TAG_SCENE_INFO_DIRECTORY_TAG = "SceneInfoDirectory";
    const float SceneFile::s_sceneFileVersion = 3.0;
#endif // __SCENE_FILE_DECLARE__

} // namespace
#endif  //__SCENE_FILE__H_
