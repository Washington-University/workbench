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
class QSpinBox;

namespace caret {
    
    class EnumComboBoxTemplate;
    class WuQTrueFalseComboBox;
    class WuQWidgetObjectGroup;
    
    class PreferencesDialog : public WuQDialogNonModal {
        Q_OBJECT
        
    public:
        PreferencesDialog(QWidget* parent);
        
        virtual ~PreferencesDialog();

        void updateDialog();
        
    protected:
        virtual void applyButtonPressed();

    private slots:
        void backgroundColorPushButtonPressed();
        void foregroundColorPushButtonPressed();
        void loggingLevelComboBoxChanged(int);
        void openGLDrawingMethodEnumComboBoxItemActivated();
        void imageCaptureMethodEnumComboBoxItemActivated();
        
        void volumeAxesCrosshairsComboBoxToggled(bool value);
        void volumeAxesLabelsComboBoxToggled(bool value);
        void volumeAxesMontageCoordinatesComboBoxToggled(bool value);
        void volumeMontageGapValueChanged(int value);
        void volumeMontageCoordinatePrecisionChanged(int value);
        
        void splashScreenShowAtStartupComboBoxChanged(bool value);
        
        void developMenuEnabledComboBoxChanged(bool value);
        
    private:
        void addColorItems();
        void addDevelopItems();
        void addLoggingItems();
        void addOpenGLItems();
        void addSplashItems();
        void addVolumeItems();
        
        QLabel* addWidgetToLayout(const QString& labelText,
                                  QWidget* widget);
        
        void addWidgetsToLayout(QWidget* leftWidget,
                             QWidget* rightWidget);
        
        PreferencesDialog(const PreferencesDialog&);

        PreferencesDialog& operator=(const PreferencesDialog&);
        
        QWidget* foregroundColorWidget;
        QWidget* backgroundColorWidget;

        QComboBox* loggingLevelComboBox;
        
        EnumComboBoxTemplate* m_openGLDrawingMethodEnumComboBox;
        
        EnumComboBoxTemplate* m_imageCaptureMethodEnumComboBox;
        
        WuQTrueFalseComboBox* volumeAxesCrosshairsComboBox;
        WuQTrueFalseComboBox* volumeAxesLabelsComboBox;
        WuQTrueFalseComboBox* volumeAxesMontageCoordinatesComboBox;
        QSpinBox* volumeMontageGapSpinBox;
        QSpinBox* volumeMontageCoordinatePrecisionSpinBox;
        
        WuQTrueFalseComboBox* splashScreenShowAtStartupComboBox;
        
        WuQTrueFalseComboBox* developMenuEnabledComboBox;
        
        QGridLayout* gridLayout;
        
        WuQWidgetObjectGroup* allWidgets;
    };
    
#ifdef __PREFERENCES_DIALOG__H__DECLARE__
#endif // __PREFERENCES_DIALOG__H__DECLARE__

} // namespace
#endif  //__PREFERENCES_DIALOG__H_
