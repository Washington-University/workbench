#ifndef __OPERATION_SCENE_FILE_RESTRUCTURE_H__
#define __OPERATION_SCENE_FILE_RESTRUCTURE_H__

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
    
    class OperationSceneFileRestructure : public AbstractOperation
    {
    public:
        /* For output messages */
        enum class MessageMode {
            /* For use with command line */
            COMMAND_LINE,
            /* For use with GUI */
            GUI
        };
        
        /**
         * Mode for overwrite files
         */
        enum class OverwriteFilesMode {
            /* Yes, overwrite existing files */
            OVERWRITE_YES,
            /* No, do not overwrite existing files and throw error */
            OVERWRITE_ERROR,
            /* No, do not ovewrite existing files and continue processing */
            OVERWRITE_SKIP
        };
        
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
        
        static void copySceneFileAndDataFiles(const AString& inputSceneFileName,
                                              const AString& outputDirectoryNameIn,
                                              const MessageMode messageMode,
                                              const OverwriteFilesMode overwriteFilesMode,
                                              const bool previewModeFlag,
                                              const bool skipMissingFilesFlag,
                                              const bool testScenesFlag,
                                              AString& messagesOut);
        
        static AString getInstructionsInPlainText();
        
        static AString getInstructionsInHtml();
        
    private:
        static std::vector<AString> getChildDataFiles(const AString& dataFileName);
        
        static void copyDataFiles(const std::vector<std::pair<AString, AString>> dataFileFromToFileNames,
                                  const AString& previewText,
                                  const bool executeModeFlag,
                                  const bool allowOverwriteFilesFlag,
                                  const MessageMode messageMode,
                                  AString& messagesOut);
        
        static bool copySpecialFileTypes(const AString& fromFileName,
                                         const AString& toFileName);
        
        static void updateDataFilePaths(SceneFile* sceneFile,
                                        const AString& dataFilesDirectoryName,
                                        const bool executeModeFlag);
        
        static void testScenes(const AString& sceneFileName,
                               const int32_t numberOfScenes,
                               const MessageMode messageMode,
                               AString& messagesOut);
        
        static AString getInstructionsPrivate(const MessageMode messageMode);

    };

    typedef TemplateAutoOperation<OperationSceneFileRestructure> AutoOperationSceneFileRestructure;

}

#endif //__OPERATION_SCENE_FILE_RESTRUCTURE_H__
