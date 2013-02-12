#ifndef __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_H_
#define __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <set>
#include <stdint.h> // needed by windows
#include <QWidget>

class QAbstractButton;
class QCheckBox;
class QDoubleSpinBox;
class QComboBox;
class QLabel;
class QLayout;
class QPushButton;
class QRadioButton;
class QwtPlot;


namespace caret {
    
    class CaretMappableDataFile;
    class DescriptiveStatistics;
    class FastStatistics;
    class Histogram;
    class PaletteColorMapping;
    class WuQDoubleSlider;
    class WuQWidgetObjectGroup;
    
    class MapSettingsPaletteColorMappingWidget : public QWidget {
        Q_OBJECT
        
    public:
        MapSettingsPaletteColorMappingWidget(QWidget* parent = 0);
        
        void updateEditor(CaretMappableDataFile* caretMappableDataFile,
                                 const int32_t mapIndex);
                    
        virtual ~MapSettingsPaletteColorMappingWidget();

        void updateWidget();
        
    private:
        MapSettingsPaletteColorMappingWidget(const MapSettingsPaletteColorMappingWidget&);

        MapSettingsPaletteColorMappingWidget& operator=(const MapSettingsPaletteColorMappingWidget&);
        
    private slots:
        void thresholdLowSpinBoxValueChanged(double);
        void thresholdHighSpinBoxValueChanged(double);
        void thresholdLowSliderValueChanged(double);
        void thresholdHighSliderValueChanged(double);
        void thresholdTypeChanged(int);
        
        void histogramControlChanged();
        void histogramResetViewButtonClicked();
        void applyAndUpdate();
        void applySelections();
        
        void applyAllMapsCheckBoxStateChanged(int state);
        
        void applyToMultipleFilesPushbuttonClicked();
        
    private:
        QWidget* createPaletteSection();
        QWidget* createThresholdSection();
        QWidget* createHistogramSection();
        QWidget* createHistogramControlSection();
        QWidget* createDataOptionsSection();
        
        void updateHistogramPlot();
        
        void setLayoutMargins(QLayout* layout);
        
        const DescriptiveStatistics* getDescriptiveStatisticsForHistogram(const DescriptiveStatistics* statisticsForAll) const;
        
        const Histogram* getHistogram(const FastStatistics* statisticsForAll) const;
        
        PaletteColorMapping* paletteColorMapping;
        
        QComboBox* paletteNameComboBox;
        
        QCheckBox* applyAllMapsCheckBox;
        QPushButton* applyToMultipleFilesPushButton;
        
        QRadioButton* scaleAutoRadioButton;
        QRadioButton* scaleAutoPercentageRadioButton;
        QRadioButton* scaleFixedRadioButton;
        
        QDoubleSpinBox* scaleAutoPercentageNegativeMaximumSpinBox;
        QDoubleSpinBox* scaleAutoPercentageNegativeMinimumSpinBox;
        QDoubleSpinBox* scaleAutoPercentagePositiveMinimumSpinBox;
        QDoubleSpinBox* scaleAutoPercentagePositiveMaximumSpinBox;
        
        QDoubleSpinBox* scaleFixedNegativeMaximumSpinBox;
        QDoubleSpinBox* scaleFixedNegativeMinimumSpinBox;
        QDoubleSpinBox* scaleFixedPositiveMinimumSpinBox;
        QDoubleSpinBox* scaleFixedPositiveMaximumSpinBox;
        
        QCheckBox* displayModePositiveCheckBox;
        QCheckBox* displayModeZeroCheckBox;
        QCheckBox* displayModeNegativeCheckBox;
        
        QCheckBox* interpolateColorsCheckBox;
        
        QComboBox* thresholdTypeComboBox;
        
        WuQDoubleSlider* thresholdLowSlider;
        WuQDoubleSlider* thresholdHighSlider;

        WuQWidgetObjectGroup* thresholdAdjustmentWidgetGroup;
        
        QDoubleSpinBox* thresholdLowSpinBox;
        QDoubleSpinBox* thresholdHighSpinBox;
        
        QRadioButton* thresholdShowInsideRadioButton;
        QRadioButton* thresholdShowOutsideRadioButton;
        
        QwtPlot* thresholdPlot;
        
        QLabel* statisticsMinimumValueLabel;
        QLabel* statisticsMaximumValueLabel;
        QLabel* statisticsMeanValueLabel;
        QLabel* statisticsStandardDeviationLabel;
        
        QRadioButton* histogramAllRadioButton;
        QRadioButton* histogramMatchPaletteRadioButton;
        
        QCheckBox* histogramUsePaletteColors;
        
        bool isHistogramColored;
        
        CaretMappableDataFile* caretMappableDataFile;
        
        int32_t mapFileIndex;
        
        std::set<CaretMappableDataFile*> previousApplyPaletteToMapFilesSelected;
        
        WuQWidgetObjectGroup* paletteWidgetGroup;
        WuQWidgetObjectGroup* thresholdWidgetGroup;
    };
    
#ifdef __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_PALETTE_COLOR_MAPPING_WIDGET_H_
