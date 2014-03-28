#ifndef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR__H_
#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR__H_

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

#include <set>
#include <vector>

#include "AString.h"
#include "CaretMappableDataFile.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"
#include "WuQWidget.h"

class QAction;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QStackedWidget;

namespace caret {
    
    class Brain;
    class BrainStructure;
    class CiftiBrainordinateScalarFile;
    class GiftiTypeFile;
    class LabelFile;
    class MetricFile;
    class StructureEnumComboBox;
    class WuQWidgetObjectGroup;
    
    class CaretMappableDataFileAndMapSelector : public WuQWidget {
        
        Q_OBJECT

    public:
        CaretMappableDataFileAndMapSelector(const AString defaultName,
                                            Brain* brain,
                                            const std::vector<DataFileTypeEnum::Enum>& supportedMapFileTypes,
                                            const std::vector<StructureEnum::Enum>& supportedStructures,
                                            QObject* parent);
        
        virtual ~CaretMappableDataFileAndMapSelector();
        
        QWidget* getWidget();
        
        CaretMappableDataFile* getSelectedMapFile();
        
        DataFileTypeEnum::Enum getSelectedMapFileType() const;
        
        StructureEnum::Enum getSelectedMapFileStructure() const;
        
        int32_t getSelectedMapIndex();
        
        AString getNameOfSelectedMapFileType();
        
        bool isValidSelections(AString& errorMessageOut);
        
        float getSelectedMetricValue() const;
        
        int32_t getSelectedLabelKey() const;
        
        AString getSelectedLabelName() const;
        
        void saveCurrentSelections();
        
    signals:
        void selectionChanged(CaretMappableDataFileAndMapSelector*);
        
    private:
        CaretMappableDataFileAndMapSelector(const CaretMappableDataFileAndMapSelector&);

        CaretMappableDataFileAndMapSelector& operator=(const CaretMappableDataFileAndMapSelector&);
        
    private slots:
        void mapFileTypeComboBoxSelected(int);
        
        void mapFileStructureComboBoxSelected(const StructureEnum::Enum);
        
        void mapFileComboBoxSelected(int);
        
        void mapNameComboBoxSelected(int);
        
        void labelNameComboBoxSelected(int);
        
        void floatValueChanged(double);
        
        void newMapFileToolButtonSelected();
        
        void newMapToolButtonSelected();
        
        void showLabelsEditor();
        
    private:
        void setMapFileTypeComboBoxCurrentIndex(int indx);
        
        void setMapFileComboBoxCurrentIndex(int indx);

        void setMapNameComboBoxCurrentIndex(int indx);
        
        void loadMapFileComboBox(const int32_t selectedFileIndex);
        
        void loadMapNameComboBox(const int32_t selectedMapIndex);
        
        void loadLabelNameComboBox();
        
        void updateFileTypeSelections(const DataFileTypeEnum::Enum dataFileType);
        
        void enableDisableNewMapAction();
        
        QWidget* widget;
        
        QComboBox* mapFileTypeComboBox;
        
        StructureEnumComboBox* m_mapFileStructureComboBox;
        
        QComboBox* mapFileComboBox;
        
        QComboBox* mapNameComboBox;
        
        QAction* m_newMapAction;
        
        Brain* m_brain;
        
        
        QWidget* m_valueWidgetFloat;
        
        WuQWidgetObjectGroup* m_floatValueControlsGroup;
        
        QDoubleSpinBox* m_floatValueSpinBox;
        
        
        QWidget* valueWidgetLabel;
        
        WuQWidgetObjectGroup* labelValueControlsGroup;
        
        QComboBox* labelSelectionComboBox;
        

        QStackedWidget* valueEntryStackedWidget;
        
        std::vector<DataFileTypeEnum::Enum> m_supportedMapFileTypes;
        
        std::vector<StructureEnum::Enum> m_supportedStructures;
        
        std::vector<DataFileTypeEnum::Enum> m_mapFileTypesThatAllowAddingMaps;
        
        AString m_defaultName;
        
        class PreviousSelection {
        public:
            StructureEnum::Enum m_structure;
            DataFileTypeEnum::Enum m_dataFileType;

            CaretMappableDataFile* m_mapFile;
            AString m_mapName;
            
            AString m_labelName;
            float m_scalarValue;
            
            PreviousSelection(const StructureEnum::Enum structure,
                              const DataFileTypeEnum::Enum dataFileType,
                              CaretMappableDataFile* mapFile,
                              const AString& mapName,
                              const AString& labelName,
                              const float scalarValue) {
                m_structure    = structure;
                m_dataFileType = dataFileType;
                m_mapFile = mapFile;
                m_mapName = mapName;
                m_labelName = labelName;
                m_scalarValue = scalarValue;
            }
            
            bool operator=(const PreviousSelection& ps) const {
                if ((m_structure == ps.m_structure)
                    && (m_dataFileType == ps.m_dataFileType)) {
                    return true;
                }
                
                return false;
            }
        };
        
        PreviousSelection* getPreviousSelection(const DataFileTypeEnum::Enum dataFileType,
                                                const StructureEnum::Enum structure);
        
        void loadLastSelectionsForFileType(const DataFileTypeEnum::Enum dataFileType);
        
        static std::vector<PreviousSelection*> previousSelections;
    };
    
    
#ifdef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__
    std::vector<CaretMappableDataFileAndMapSelector::PreviousSelection*> CaretMappableDataFileAndMapSelector::previousSelections;
#endif // __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR__H_
