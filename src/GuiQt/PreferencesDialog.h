#ifndef __PREFERENCES_DIALOG__H_
#define __PREFERENCES_DIALOG__H_

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


#include "WuQDialogNonModal.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QSpinBox;

namespace caret {
    
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
        
        void volumeAxesCrosshairsComboBoxToggled(bool value);
        void volumeAxesLabelsComboBoxToggled(bool value);
        
        void dataFileAddToSpecFileComboBoxChanged(bool value);
        void splashScreenShowAtStartupComboBoxChanged(bool value);
        
        void animationStartChanged(double value);
        
    private:
        void addColorItems();
        void addDataFileItems();
        void addLoggingItems();
        void addSplashItems();
        void addVolumeItems();
        void addTimeCourseItems();
        
        void addWidgetToLayout(const QString& labelText,
                               QWidget* widget);
        
        void addWidgetsToLayout(QWidget* leftWidget,
                             QWidget* rightWidget);
        
        PreferencesDialog(const PreferencesDialog&);

        PreferencesDialog& operator=(const PreferencesDialog&);
        
        QWidget* foregroundColorWidget;
        QWidget* backgroundColorWidget;

        QComboBox* loggingLevelComboBox;
        
        WuQTrueFalseComboBox* volumeAxesCrosshairsComboBox;
        WuQTrueFalseComboBox* volumeAxesLabelsComboBox;
        
        WuQTrueFalseComboBox* dataFileAddToSpecFileComboBox;
        
        WuQTrueFalseComboBox* splashScreenShowAtStartupComboBox;
        
        QDoubleSpinBox* animationStartDoubleSpinBox;
        
        QGridLayout* gridLayout;
        
        WuQWidgetObjectGroup* allWidgets;
    };
    
#ifdef __PREFERENCES_DIALOG__H__DECLARE__
#endif // __PREFERENCES_DIALOG__H__DECLARE__

} // namespace
#endif  //__PREFERENCES_DIALOG__H_
