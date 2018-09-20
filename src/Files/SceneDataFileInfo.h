#ifndef __SCENE_DATA_FILE_INFO_H__
#define __SCENE_DATA_FILE_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include <algorithm>
#include <memory>
#include <set>

#include "CaretObject.h"



namespace caret {

    class SceneDataFileInfo : public CaretObject {
        
    public:
        /**
         * Mode for sorting
         */
        enum class SortMode {
            /* Sort by file's absolute path and then data file name */
            AbsolutePath,
            /* Sort by path relative to base path and then data file name */
            RelativeToBasePath,
            /* Sort by path relative to scene file path and then data file name */
            RelativeToSceneFilePath
        };
        
        SceneDataFileInfo(const AString& absoluteDataFilePathAndName,
                          const AString& absoluteBasePath,
                          const AString& absoluteSceneFilePathAndName,
                          const std::vector<int32_t>& sceneIndices);
        
        virtual ~SceneDataFileInfo();
        
        SceneDataFileInfo(const SceneDataFileInfo& obj);

        SceneDataFileInfo& operator=(const SceneDataFileInfo& obj);
        
        bool operator<(const SceneDataFileInfo& rhs) const;
        
        void addSceneIndex(const int32_t sceneIndex) const;
        
        AString getSceneIndicesAsString() const;

        AString getAbsolutePath() const;
        
        AString getAbsolutePathAndFileName() const;
        
        AString getDataFileName() const;
        
        AString getRelativePathToBasePath() const;
        
        AString getRelativePathToSceneFile() const;
        
        bool isRemote() const;
        
        bool isMissing() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        static void sort(std::vector<SceneDataFileInfo>& sceneDataFileInfo,
                         const SortMode sortMode);
        
    private:
        void copyHelperSceneDataFileInfo(const SceneDataFileInfo& obj);

        AString m_absolutePath;
        
        AString m_dataFileName;
        
        AString m_relativePathToBasePath;
        
        AString m_relativePathToSceneFile;
        
        bool m_remoteFlag = false;
        
        bool m_missingFlag = false;
        
        mutable std::set<int32_t> m_sceneIndices;
        
        // ADD_NEW_MEMBERS_HERE and DON'T FORGET TO UPDATE COPY COPY METHOD

    };
    
#ifdef __SCENE_DATA_FILE_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_DATA_FILE_INFO_DECLARE__

} // namespace
#endif  //__SCENE_DATA_FILE_INFO_H__
