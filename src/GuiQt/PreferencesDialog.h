#ifndef __PREFERENCES_DIALOG__H_
#define __PREFERENCES_DIALOG__H_

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


#include "WuQDialogNonModal.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QSignalMapper;
class QSpinBox;

namespace caret {
    
    class CaretPreferences;
    class EnumComboBoxTemplate;
    class PreferencesRecentFilesWidget;
    class WuQTrueFalseComboBox;
    class WuQWidgetObjectGroup;
    
    class PreferencesDialog : public WuQDialogNonModal {
        Q_OBJECT
        
    public:
        PreferencesDialog(QWidget* parent);
        
        virtual ~PreferencesDialog();

        void updateDialog();
        
    protected:
        virtual void applyButtonClicked();

    private slots:
        void colorPushButtonClicked(int);
        
        void miscDevelopMenuEnabledComboBoxChanged(bool value);
        void miscLoggingLevelComboBoxChanged(int);
        void miscSpecFileDialogViewFilesTypeEnumComboBoxItemActivated();
        void miscGuiGesturesEnabledComboBoxChanged(bool value);
        void miscDynamicConnectivityComboBoxChanged(bool value);
        void miscWindowToolBarWidthModeComboBoxItemActivated();
        void miscFileOpenFromOpSysTypeComboBoxItemActivated();
        void openGLDrawingMethodEnumComboBoxItemActivated();
        void openGLImageCaptureMethodEnumComboBoxItemActivated();
        
        void volumeAxesCrosshairsComboBoxToggled(bool value);
        void volumeAxesLabelsComboBoxToggled(bool value);
        void volumeAxesMontageCoordinatesComboBoxToggled(bool value);
        void volumeMontageCoordinatePrecisionChanged(int value);
        void volumeIdentificationComboBoxToggled(bool value);
        void volumeAllSlicePlanesLayoutItemActivated();
        
        void yokingComboBoxToggled(bool value);
        
        void identificationSymbolToggled();
        void identificationModeEnumComboBoxItemActivated();
        
        void recentFilesChanged();
    private:
        enum PREF_COLOR {
            PREF_COLOR_BACKGROUND_ALL          = 0,
            PREF_COLOR_BACKGROUND_CHART        = 1,
            PREF_COLOR_BACKGROUND_SURFACE      = 2,
            PREF_COLOR_BACKGROUND_VOLUME       = 3,
            PREF_COLOR_FOREGROUND_ALL          = 4,
            PREF_COLOR_FOREGROUND_CHART        = 5,
            PREF_COLOR_FOREGROUND_SURFACE      = 6,
            PREF_COLOR_FOREGROUND_VOLUME       = 7,
            PREF_COLOR_CHART_MATRIX_GRID_LINES = 8,
            PREF_COLOR_CHART_THRESHOLD         = 9,
            PREF_COLOR_BACKGROUND_WINDOW       = 10,
            PREF_COLOR_FOREGROUND_WINDOW       = 11,
            NUMBER_OF_PREF_COLORS              = 12
        };
        
        QWidget* createColorsWidget();
        QWidget* createIdentificationSymbolWidget();
        QWidget* createMiscellaneousWidget();
        QWidget* createOpenGLWidget();
        QWidget* createTabDefaltsWidget();
        
        void updateColorWidget(CaretPreferences* prefs);
        void updateIdentificationWidget(CaretPreferences* prefs);
        void updateMiscellaneousWidget(CaretPreferences* prefs);
        void updateOpenGLWidget(CaretPreferences* prefs);
        void updateVolumeWidget(CaretPreferences* prefs);
        
        void updateColorWithDialog(const PREF_COLOR prefColor);

        QLabel* addWidgetToLayout(QGridLayout* gridLayout,
                                  const QString& labelText,
                                  QWidget* widget);
        
        void addWidgetsToLayout(QGridLayout* gridLayout,
                                QWidget* leftWidget,
                                QWidget* rightWidget);
        
        void addColorButtonAndSwatch(QGridLayout* gridLayout,
                                     const PREF_COLOR prefColor,
                                     QSignalMapper* colorSignalMapper);
        
        PreferencesDialog(const PreferencesDialog&);

        PreferencesDialog& operator=(const PreferencesDialog&);
        
        QWidget* m_foregroundColorWindowWidget;
        QWidget* m_foregroundColorAllWidget;
        QWidget* m_foregroundColorChartWidget;
        QWidget* m_foregroundColorSurfaceWidget;
        QWidget* m_foregroundColorVolumeWidget;
        QWidget* m_backgroundColorWindowWidget;
        QWidget* m_backgroundColorAllWidget;
        QWidget* m_backgroundColorChartWidget;
        QWidget* m_backgroundColorSurfaceWidget;
        QWidget* m_backgroundColorVolumeWidget;
        QWidget* m_chartMatrixGridLinesColorWidget;
        QWidget* m_chartHistogramThresholdColorWidget;

        WuQTrueFalseComboBox* m_miscDevelopMenuEnabledComboBox;
        QComboBox* m_miscLoggingLevelComboBox;
        EnumComboBoxTemplate* m_miscSpecFileDialogViewFilesTypeEnumComboBox;
        WuQTrueFalseComboBox* m_guiGesturesEnabledComboBox;
        EnumComboBoxTemplate* m_windowToolBarWidthModeComboBox;
        EnumComboBoxTemplate* m_fileOpenFromOpSysTypeComboBox;
        
        EnumComboBoxTemplate* m_openGLDrawingMethodEnumComboBox;
        EnumComboBoxTemplate* m_openGLImageCaptureMethodEnumComboBox;

        WuQTrueFalseComboBox* m_dynamicConnectivityComboBox;
        
        EnumComboBoxTemplate* m_volumeAllSlicePlanesLayoutComboBox;
        WuQTrueFalseComboBox* m_volumeAxesCrosshairsComboBox;
        WuQTrueFalseComboBox* m_volumeAxesLabelsComboBox;
        WuQTrueFalseComboBox* m_volumeAxesMontageCoordinatesComboBox;
        QSpinBox* m_volumeMontageCoordinatePrecisionSpinBox;
        WuQTrueFalseComboBox* m_volumeIdentificationComboBox;
        
        WuQTrueFalseComboBox* m_yokingDefaultComboBox;
        
        WuQTrueFalseComboBox* m_surfaceIdentificationSymbolComboBox;
        WuQTrueFalseComboBox* m_volumeIdentificationSymbolComboBox;
        WuQTrueFalseComboBox* m_dataToolTipsComboBox;
        EnumComboBoxTemplate* m_identificationModeComboBox;
        
        WuQWidgetObjectGroup* m_allWidgets;
        
        PreferencesRecentFilesWidget* m_recentFilesWidget;
    };
    
#ifdef __PREFERENCES_DIALOG__H__DECLARE__
#endif // __PREFERENCES_DIALOG__H__DECLARE__

} // namespace
#endif  //__PREFERENCES_DIALOG__H_
