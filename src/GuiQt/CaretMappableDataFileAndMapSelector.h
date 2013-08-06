#ifndef __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR__H_
#define __CARET_MAPPABLE_DATA_FILE_AND_MAP_SELECTOR__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
                this->dataFileType = DataFileTypeEnum::UNKNOWN;
                
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
