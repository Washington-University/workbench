#ifndef __WU_Q_MACRO_COMMAND_PARAMETER_WIDGET_H__
#define __WU_Q_MACRO_COMMAND_PARAMETER_WIDGET_H__

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

#include <QWidget>

class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QSpinBox;
class QStackedWidget;

namespace caret {

    class WuQMacroCommand;
    class WuQMacroCommandParameter;
    
    class WuQMacroCommandParameterWidget : public QObject {
        
        Q_OBJECT

    public:
        WuQMacroCommandParameterWidget(const int32_t index,
                                       QGridLayout* gridLayout,
                                       QWidget* parent);
        
        virtual ~WuQMacroCommandParameterWidget();
        
        WuQMacroCommandParameterWidget(const WuQMacroCommandParameterWidget&) = delete;

        WuQMacroCommandParameterWidget& operator=(const WuQMacroCommandParameterWidget&) = delete;
        
        void updateContent(int32_t windowIndex,
                           WuQMacroCommand* macroCommand,
                           WuQMacroCommandParameter* parameter);

        // ADD_NEW_METHODS_HERE

    signals:
        void dataChanged(const int index);
        
    private slots:
        void booleanOffActionTriggered(bool);
        
        void booleanOnActionTriggered(bool);
        
        void comboBoxActivated(int);
        
        void doubleSpinBoxValueChanged(double);
        
        void lineEditTextEdited(const QString&);
        
        void spinBoxValueChanged(int);
        
    private:
        QLabel* m_nameLabel;

        const int32_t m_index;
        
        int32_t m_windowIndex = -1;
        
        WuQMacroCommand* m_macroCommand = NULL;
        
        WuQMacroCommandParameter* m_parameter = NULL;

        QStackedWidget* m_stackedWidget;
        
        QAction* m_booleanOnAction;
        
        QAction* m_booleanOffAction;
        
        QWidget* m_booleanWidget;
        
        QComboBox* m_comboBox;
        
        QDoubleSpinBox* m_doubleSpinBox;
        
        QLineEdit* m_lineEdit;
        
        QSpinBox* m_spinBox;
        
        QWidget* m_noValueWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_COMMAND_PARAMETER_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_COMMAND_PARAMETER_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_COMMAND_PARAMETER_WIDGET_H__
