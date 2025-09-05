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

#include <map>
#include <memory>
#include <set>

#include "CaretDataFile.h"
#include "FunctionResult.h"
#include "SceneDataFileInfo.h"
#include "SceneFileBasePathTypeEnum.h"

namespace caret {

    class Scene;
    class SceneInfo;
    
    class SceneFile : public CaretDataFile {
        
    public:
        static FunctionResultValue<std::map<int32_t, SceneInfo*>> readSceneInfoOnly(const AString& filename);
        
        SceneFile();
        
        virtual ~SceneFile();
        
    private:
        SceneFile(const SceneFile&);

        SceneFile& operator=(const SceneFile&);
        
    public:
        class FileAndSceneIndicesInfo {
        public:
            FileAndSceneIndicesInfo(const AString& dataFileName,
                                    const int32_t sceneIndex)
            : m_dataFileName(dataFileName) {
                m_sceneIndices.push_back(sceneIndex + 1);
            }
            
            bool operator<(const FileAndSceneIndicesInfo& rhs) const {
                return m_dataFileName < rhs.m_dataFileName;
            }
            
            void addSceneIndex(const int32_t sceneIndex) const {
                m_sceneIndices.push_back(sceneIndex + 1);
            }
            
            AString getSceneIndices() const {
                return AString::fromNumbers(m_sceneIndices, ",");
            }
            
            const AString m_dataFileName;
            
            mutable std::vector<int32_t> m_sceneIndices;
        };
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        void clear();
        
        virtual void setFileName(const AString& filename) override;

        void readFile(const AString& filename);
        
        void readFileStreamReader(const AString& filename);
        
        void writeFile(const AString& filename);
        
        void writeFileStreamWriter(const AString& filename);
        
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
        
        Scene* getSceneWithNameOrNumber(const AString& sceneNameOrNumber);
        
        Scene* getSceneWithSceneID(const AString& sceneID);
        
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
        
        bool findBaseDirectoryForDataFiles(AString& baseDirectoryOut,
                                           std::vector<AString>& missingFileNamesOut,
                                           AString& errorMessageOut) const;
        
        static bool findBaseDirectoryForDataFiles(const AString& sceneFileName,
                                                  const std::set<FileAndSceneIndicesInfo>& filesFromScenes,
                                                  AString& baseDirectoryOut,
                                                  std::vector<AString>& missingFileNamesOut,
                                                  AString& errorMessageOut);

        std::vector<AString> getBaseDirectoryHierarchyForDataFiles(const int32_t maximumAncestorCount = 25);
        
        bool getSelectedBasePathTypeAndName(SceneFileBasePathTypeEnum::Enum& basePathTypeOut,
                                                 AString& basePathNameOut,
                                                 AString& errorMessageOut) const;
        
        std::set<FileAndSceneIndicesInfo> getAllDataFileNamesFromAllScenes() const;
        
        std::vector<SceneDataFileInfo> getAllDataFileInfoFromAllScenes() const;
        
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

        int32_t getSceneFileVersionForWriting() const;
        
        static int32_t getSceneFileVersionBeforeMacros();
        
        static int32_t getMaxiumSupportedSceneFileVersion();
        
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

        /** Version of this SceneFile before addition of macros */
        static const int32_t s_sceneFileVersionBeforeMacros;
        
        /** Version of this SceneFile containing macros */
        static const int32_t s_sceneFileVersionContainingMacros;
    };
    
#ifdef __SCENE_FILE_DECLARE__
    const AString SceneFile::XML_TAG_SCENE_FILE = "SceneFile";
    const AString SceneFile::XML_ATTRIBUTE_VERSION = "Version";
    const AString SceneFile::XML_TAG_SCENE_INFO_DIRECTORY_TAG = "SceneInfoDirectory";
    
    /*
     * NOTE: If these scene file versions change, getSupportedSceneFileVersion()
     * will need to be updated with the maximum scene file version that
     * can be read
     */
    const int32_t SceneFile::s_sceneFileVersionBeforeMacros     = 3;
    const int32_t SceneFile::s_sceneFileVersionContainingMacros = 4;
    
#endif // __SCENE_FILE_DECLARE__

} // namespace
#endif  //__SCENE_FILE__H_
