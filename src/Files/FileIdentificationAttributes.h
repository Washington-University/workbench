#ifndef __FILE_IDENTIFICATION_ATTRIBUTES_H__
#define __FILE_IDENTIFICATION_ATTRIBUTES_H__

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

#include "CaretObject.h"
#include "FileIdentificationDisplayModeEnum.h"
#include "FileIdentificationMapSelectionEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class FileIdentificationAttributes : public CaretObject, public SceneableInterface {
        
    public:
        FileIdentificationAttributes();
        
        virtual ~FileIdentificationAttributes();
        
        FileIdentificationAttributes(const FileIdentificationAttributes& obj);

        FileIdentificationAttributes& operator=(const FileIdentificationAttributes& obj);
        
        FileIdentificationDisplayModeEnum::Enum getDisplayMode() const;
        
        void setDisplayMode(const FileIdentificationDisplayModeEnum::Enum displayMode);
        
        FileIdentificationMapSelectionEnum::Enum getMapSelectionMode() const;
        
        void setMapSelectionMode(const FileIdentificationMapSelectionEnum::Enum mapSelectionMode);
        
        int32_t getMapIndex() const;
        
        void setMapIndex(const int32_t mapIndex);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperFileIdentificationAttributes(const FileIdentificationAttributes& obj);

        void reset();
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        /* display mode */
        FileIdentificationDisplayModeEnum::Enum m_displayMode = FileIdentificationDisplayModeEnum::OVERLAY;
       
        /** identifiy all maps in file*/
        FileIdentificationMapSelectionEnum::Enum m_mapSelectionMode = FileIdentificationMapSelectionEnum::SELECTED;
        
        /** map selected for identification*/
        int32_t m_mapIndex = 0;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FILE_IDENTIFICATION_ATTRIBUTES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FILE_IDENTIFICATION_ATTRIBUTES_DECLARE__

} // namespace
#endif  //__FILE_IDENTIFICATION_ATTRIBUTES_H__
