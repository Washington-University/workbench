#ifndef __OPERATION_SCENE_FILE_UPDATE_H__
#define __OPERATION_SCENE_FILE_UPDATE_H__

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


#include "AbstractOperation.h"

namespace caret {

    class Brain;
    class CaretMappableDataFile;
    class Scene;
    class SceneAttributes;
    
    class OperationSceneFileUpdate : public AbstractOperation {

    public:
        static OperationParameters* getParameters();

        static void useParameters(OperationParameters* myParams, 
                                  ProgressObject* myProgObj);

        static AString getCommandSwitch();

        static AString getShortDescription();

    private:
        enum ParamKeys : int32_t {
            PARAM_KEY_INPUT_SCENE_FILE = 1,
            PARAM_KEY_OUTPUT_SCENE_FILE = 2,
            PARAM_KEY_SCENE_NAME_NUMBER = 3,
            PARAM_KEY_OPTION_COPY_MAP_ONE_PALETTE = 4,
            PARAM_KEY_OPTION_DATA_FILE_ADD = 5,
            PARAM_KEY_OPTION_DATA_FILE_REMOVE = 6,
            PARAM_KEY_OPTION_FIX_MAP_PALETTE_SETTINGS= 7,
            PARAM_KEY_OPTION_ERROR_AS_WARNING = 8,
            PARAM_KEY_OPTION_VERBOSE = 9
        };
        
        enum class MatchNameMode {
            MATCH_EXACT,
            MATCH_END_OF_NAME
        };
        
        enum class SceneOperationType {
            COPY_MAP_ONE_PALETTE,
            DATA_FILE_ADD,
            DATA_FILE_REMOVE,
            FIX_MAP_PALETTE_SETTINGS
        };
        
        class SceneOperation {
        public:
            SceneOperation(const SceneOperationType sceneOperationType,
                           const AString& parameter)
            : m_sceneOperationType(sceneOperationType),
            m_parameter(parameter) { }
            
            std::vector<AString> getFileNames() const;
            
            SceneOperationType m_sceneOperationType;
            AString m_parameter;
        };
        
        static int32_t fixPalettesInFilesWithMapCountChanged(Scene* scene,
                                                             SceneAttributes& sceneAttributes);
        
        static int32_t copyMapOnePalettes(Scene* scene,
                                          SceneAttributes& sceneAttributes,
                                          const AString& copyMapOneDataFileName);
        
        static int32_t dataFileAddRemove(Scene* scene,
                                         SceneAttributes& sceneAttributes,
                                         const std::vector<AString>& filenames,
                                         Brain* brain,
                                         const SceneOperationType addRemoveOperation);
        
        static int32_t updateScenePaletteXML(Scene* scene,
                                             SceneAttributes* sceneAttributes,
                                             CaretMappableDataFile* mapFile,
                                             const AString& dataFileName,
                                             const MatchNameMode matchMode);
        
        static void processError(const AString& message);
        
        static bool s_fatalErrorFlag;
        
        static bool s_enableCopyMapsOptionFlag;
        
        static bool s_verboseFlag;
    };

    typedef TemplateAutoOperation<OperationSceneFileUpdate> AutoOperationSceneFileUpdate;

} // namespace

#endif  //__OPERATION_SCENE_FILE_UPDATE_H__

