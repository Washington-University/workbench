#ifndef __OPERATION_SCENE_FILE_UTILITY_H__
#define __OPERATION_SCENE_FILE_UTILITY_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "AbstractOperation.h"

namespace caret {
    class SceneFile;
    
    class OperationSceneFileUtility : public AbstractOperation
    {
    public:
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
        
        static void copySceneFileAndDataFiles(const AString& inputSceneFileName,
                                              const AString& outputDirectoryName,
                                              const bool previewModeFlag);
        
    private:
        static std::vector<AString> getChildDataFiles(const AString& dataFileName);
        
        static void updateDataFilePaths(SceneFile* sceneFile,
                                        const AString& dataFilesDirectoryName,
                                        const AString& previewText,
                                        const bool executeModeFlag);
    };

    typedef TemplateAutoOperation<OperationSceneFileUtility> AutoOperationSceneFileUtility;

}

#endif //__OPERATION_SCENE_FILE_UTILITY_H__
