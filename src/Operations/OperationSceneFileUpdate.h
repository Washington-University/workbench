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
            PARAM_KEY_INPUT_SCENE_FILE,
            PARAM_KEY_OUTPUT_SCENE_FILE,
            PARAM_KEY_SCENE_NAME_NUMBER,
            PARAM_KEY_OPTION_COPY_MAP_ONE_PALETTE,
            PARAM_KEY_OPTION_DATA_FILE_ADD,
            PARAM_KEY_OPTION_DATA_FILE_REMOVE,
            PARAM_KEY_OPTION_ERROR,
            PARAM_KEY_OPTION_FIX_MAP_PALETTE_SETTINGS,
            PARAM_KEY_OPTION_REMOVE_MISSING_FILES,
            PARAM_KEY_OPTION_VERBOSE
        };
        
        enum class MatchNameMode {
            MATCH_EXACT,
            MATCH_END_OF_NAME
        };
        
        /**
         * Type of operations on scene
         *
         * The order of these enums (not alphabetical) is the order that
         * the operations must be performed.
         */
        enum class SceneOperationType : int32_t {
            DATA_FILE_REMOVE,
            REMOVE_MISSING_FILES, /* Run after DATA_FILE_REMOVE as file removed may not exist */
            DATA_FILE_ADD,
            COPY_MAP_ONE_PALETTE, /* Run after DATA_FILE_ADD since this operation may apply to added files */
            FIX_MAP_PALETTE_SETTINGS
        };
        
        class SceneOperation {
        public:
            static AString typeToName(const SceneOperationType sceneOperationType);
            
            SceneOperation(const SceneOperationType sceneOperationType,
                           const AString& parameter)
            : m_sceneOperationType(sceneOperationType),
            m_parameter(parameter) { }
            
            std::vector<AString> getFileNames() const;
            
            bool operator<(const SceneOperation& rhs) const {
                return (m_sceneOperationType < rhs.m_sceneOperationType);
            }
            
            SceneOperationType m_sceneOperationType;
            AString m_parameter;
        };
        
        static int32_t fixPalettesInFilesWithMapCountChanged(Scene* scene,
                                                             SceneAttributes& sceneAttributes);
        
        static int32_t copyMapOnePalettes(Scene* scene,
                                          SceneAttributes& sceneAttributes,
                                          const AString& copyMapOneDataFileName);
        
        static int32_t dataFileAddRemove(Scene* scene,
                                         const std::vector<AString>& filenames,
                                         Brain* brain,
                                         const SceneOperationType addRemoveOperation);
        
        static int32_t updateScenePaletteXML(Scene* scene,
                                             SceneAttributes* sceneAttributes,
                                             CaretMappableDataFile* mapFile,
                                             const AString& dataFileName,
                                             const MatchNameMode matchMode);
        
        static void addToVerboseMessages(const AString& message);
        
        static void addToErrorMessages(const AString& message);
        
        static AString s_errorMessages;
        
        static bool s_fatalErrorFlag;
        
        static bool s_enableCopyMapsOptionFlag;
        
        static bool s_verboseFlag;
    };

    typedef TemplateAutoOperation<OperationSceneFileUpdate> AutoOperationSceneFileUpdate;

} // namespace

#endif  //__OPERATION_SCENE_FILE_UPDATE_H__

