#ifndef __DATA_FILE_COLOR_MODULATE_SELECTOR_H__
#define __DATA_FILE_COLOR_MODULATE_SELECTOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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
#include "SceneableInterface.h"

namespace caret {

    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectionModel;
    class SceneClassAssistant;
    class VolumeFile;
    
    class DataFileColorModulateSelector : public CaretObject, SceneableInterface {
        
    public:
        DataFileColorModulateSelector(CaretMappableDataFile* mapFile);
        
        virtual ~DataFileColorModulateSelector();
        
        DataFileColorModulateSelector(const DataFileColorModulateSelector&) = delete;

        DataFileColorModulateSelector& operator=(const DataFileColorModulateSelector&) = delete;

        const CaretMappableDataFile* getSelectedMapFile() const;
        
        const VolumeFile* getSelectedVolumeFile() const;
        
        int32_t getSelectedMapIndex() const;
        
        CaretMappableDataFileAndMapSelectionModel* getSelectionModel();

        const CaretMappableDataFileAndMapSelectionModel* getSelectionModel() const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        std::unique_ptr<CaretMappableDataFileAndMapSelectionModel> m_selectionModel = NULL;
        
        bool m_enabledFlag = false;
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DATA_FILE_COLOR_MODULATE_SELECTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DATA_FILE_COLOR_MODULATE_SELECTOR_DECLARE__

} // namespace
#endif  //__DATA_FILE_COLOR_MODULATE_SELECTOR_H__
