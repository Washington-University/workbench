#ifndef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_OBJECT_H__
#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_OBJECT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include <QObject>

#include "CaretVolumeExtension.h"
#include "DataFileTypeEnum.h"

class QComboBox;
class QSpinBox;

namespace caret {
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class CaretMappableDataFileAndMapSelectionModel;
    
    class CaretMappableDataFileAndMapSelectorObject : public QObject {
        
        Q_OBJECT

    public:
        /** Options for selector */
        enum Options {
            /** Create the map index spin box */
            OPTION_SHOW_MAP_INDEX_SPIN_BOX = 1
        };
        
        CaretMappableDataFileAndMapSelectorObject(const Options options,
                                                  QObject* parent);
        
        CaretMappableDataFileAndMapSelectorObject(const DataFileTypeEnum::Enum dataFileType,
                                                  const Options options,
                                                  QObject* parent);
        
        CaretMappableDataFileAndMapSelectorObject(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                                  const Options options,
                                                  QObject* parent);
        
        CaretMappableDataFileAndMapSelectorObject(const std::vector<DataFileTypeEnum::Enum>& dataFileTypes,
                                                  const std::vector<SubvolumeAttributes::VolumeType>& volumeTypes,
                                                  const Options options,
                                                  QObject* parent);
        
        void updateFileAndMapSelector(CaretMappableDataFileAndMapSelectionModel* model);
        
        CaretMappableDataFileAndMapSelectionModel* getModel();
        
        virtual ~CaretMappableDataFileAndMapSelectorObject();
        
        void getWidgetsForAddingToLayout(QWidget* &mapFileComboBox,
                                         QWidget* &mapIndexSpinBox,
                                         QWidget* &mapNameComboBox);
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        // ADD_NEW_METHODS_HERE
    signals:
        void selectionWasPerformed();
        
    private slots:
        void mapFileComboBoxFileSelected(CaretDataFile* caretDataFile);
        
        void mapIndexSpinBoxValuesChanged(int);
        
        void mapNameComboBoxActivated(int);
        
    private:
        CaretMappableDataFileAndMapSelectorObject(const CaretMappableDataFileAndMapSelectorObject&);

        CaretMappableDataFileAndMapSelectorObject& operator=(const CaretMappableDataFileAndMapSelectorObject&);
        
        void initializeConstruction(const Options options);
        
        void updateContent();
        
        CaretMappableDataFileAndMapSelectionModel* m_model;
        
        bool m_needToDestroyModelFlag;
        
        CaretDataFileSelectionComboBox* m_mapFileComboBox;
        
        QSpinBox* m_mapIndexSpinBox;
        
        QComboBox* m_mapNameComboBox;
        
        bool m_enabled;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_OBJECT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_OBJECT_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_OBJECT_H__
