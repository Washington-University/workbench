#ifndef __WU_Q_MACRO_SIGNAL_EMITTER_H__
#define __WU_Q_MACRO_SIGNAL_EMITTER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include <QObject>

class QAction;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QMenu;
class QPushButton;
class QRadioButton;
class QSlider;
class QSpinBox;
class QTabBar;
class QTabWidget;
class QToolButton;

namespace caret {

    class WuQMacroSignalEmitter : public QObject {
        
        Q_OBJECT

    public:
        WuQMacroSignalEmitter();
        
        virtual ~WuQMacroSignalEmitter();
        
        WuQMacroSignalEmitter(const WuQMacroSignalEmitter&) = delete;

        WuQMacroSignalEmitter& operator=(const WuQMacroSignalEmitter&) = delete;
        
        void emitQActionSignal(QAction* action,
                              const bool checked);

        void emitQCheckBoxSignal(QCheckBox* checkBox,
                                 const bool checked);
        
        void emitQComboBoxSignal(QComboBox* comboBox,
                                 const int32_t index);
        
        void emitQDoubleSpinBoxSignal(QDoubleSpinBox* doubleSpinBox,
                                      const double value);
        
        void emitQLineEditSignal(QLineEdit* lineEdit,
                                 const QString& text);

        void emitQListWidgetSignal(QListWidget* listWidget,
                                   const QString& text);
        
        void emitQMenuTriggered(QMenu* menu,
                                const QString& text);
        
        void emitQPushButtonSignal(QPushButton* pushButton,
                                   const bool checked);
        
        void emitQRadioButtonSignal(QRadioButton* radioButton,
                                    const bool checked);
        
        void emitQSliderSignal(QSlider* slider,
                               const int32_t value);
        
        void emitQSpinBoxSignal(QSpinBox* spinBox,
                                const int32_t value);
        
        void emitQTabBarSignal(QTabBar* tabBar,
                               const int32_t value);
        
        void emitQTabWidgetSignal(QTabWidget* tabWidget,
                                  const int32_t value);
        
        void emitQToolButtonSignal(QToolButton* toolButton,
                                   const bool checked);

        // ADD_NEW_METHODS_HERE

    signals:
        void valueChangeSignalMenuAction(QAction*);
        
        void valueChangedSignalBool(bool);
        
        void valueChangedSignalInt(int);
        
        void valueChangedSignalDouble(double);
        
        void valueChangedSignalString(QString);
        
        void valueChangeSignalListWidgetItem(QListWidgetItem*);
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_SIGNAL_EMITTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_SIGNAL_EMITTER_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_SIGNAL_EMITTER_H__
