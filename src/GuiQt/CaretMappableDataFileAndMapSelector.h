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

#include <map>
#include <vector>

#include "AString.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"
#include "WuQWidget.h"

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QStackedWidget;

namespace caret {
    
    class BrainStructure;
    class GiftiTypeFile;
    class LabelFile;
    class MetricFile;
    class WuQWidgetObjectGroup;
    
    class CaretMappableDataFileAndMapSelector : public WuQWidget {
        
        Q_OBJECT

    public:
        CaretMappableDataFileAndMapSelector(const AString defaultName,
                                            BrainStructure* brainStructure,
                                            QObject* parent);
        
        virtual ~CaretMappableDataFileAndMapSelector();
        
        QWidget* getWidget();
        
        GiftiTypeFile* getSelectedMapFile();
        
        DataFileTypeEnum::Enum getSelectedMapFileType() const;
        
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
        
        void mapFileComboBoxSelected(int);
        
        void mapNameComboBoxSelected(int);
        
        void labelNameComboBoxSelected(int);
        
        void metricValueChanged(double);
        
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
        
        QWidget* widget;
        
        QComboBox* mapFileTypeComboBox;
        
        QComboBox* mapFileComboBox;
        
        QComboBox* mapNameComboBox;
        
        BrainStructure* brainStructure;
        
        WuQWidgetObjectGroup* metricValueControlsGroup;
        
        QDoubleSpinBox* metricValueSpinBox;
        
        WuQWidgetObjectGroup* labelValueControlsGroup;
        
        QComboBox* labelSelectionComboBox;
        
        QStackedWidget* valueEntryStackedWidget;
        
        QWidget* valueWidgetMetric;
        
        QWidget* valueWidgetLabel;
        
        std::vector<DataFileTypeEnum::Enum> supportedMapFileTypes;
        
        AString m_defaultName;
        
        class Selections {
        public:
            DataFileTypeEnum::Enum dataFileType;

            LabelFile* labelFile;
            AString labelMapName;
            AString labelName;
            
            MetricFile* metricFile;
            AString metricMapName;
            float metricValue;
            
            Selections() {
                this->dataFileType = DataFileTypeEnum::METRIC;
                
                this->labelFile = NULL;
                this->labelMapName = "";
                this->labelName = "";
                
                this->metricFile = NULL;
                this->metricMapName = "";
                this->metricValue = 1.0;
            }
        };
        
        Selections* getPreviousSelection(const StructureEnum::Enum structure);
        
        void applyPreviousSelection(const bool useFileTypeFromPreviousSelection);
        
        static std::map<StructureEnum::Enum, Selections*> previousSelections;
    };
    
#ifdef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__
    std::map<StructureEnum::Enum, CaretMappableDataFileAndMapSelector::Selections*> CaretMappableDataFileAndMapSelector::previousSelections;
#endif // __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR_DECLARE__

} // namespace
#endif  //__CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR__H_
