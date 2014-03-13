#ifndef __WU_Q_FACTORY_H__
#define __WU_Q_FACTORY_H__

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

class QComboBox;
class QDoubleSpinBox;
class QObject;
class QSpinBox;
class QStyle;
class QWidget;

namespace caret {

    class WuQFactory {
        
    public:
        static QComboBox* newComboBox();
        
        static QComboBox* newComboBoxSignalInt(QObject* receiver,
                                               const char* method);
        
        static QSpinBox* newSpinBox();
        
        static QSpinBox* newSpinBoxWithSignalInt(QObject* receiver,
                                                 const char* method);
        
        static QSpinBox* newSpinBoxWithMinMaxStep(const int minimumValue,
                                                  const int maximumValue,
                                                  const int stepSize);
        
        static QSpinBox* newSpinBoxWithMinMaxStepSignalInt(const int minimumValue,
                                                           const int maximumValue,
                                                           const int stepSize,
                                                           QObject* receiver,
                                                           const char* method);
        
        static QDoubleSpinBox* newDoubleSpinBox();
        
        static QDoubleSpinBox* newDoubleSpinBoxWithSignalDouble(QObject* receiver,
                                                                const char* method);
        
        static QDoubleSpinBox* newDoubleSpinBoxWithMinMaxStepDecimals(const double minimumValue,
                                                                      const double maximumValue,
                                                                      const double stepSize,
                                                                      const int digitsRightOfDecimal);
        
        static QDoubleSpinBox* newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(const double minimumValue,
                                                                               const double maximumValue,
                                                                               const double stepSize,
                                                                               const int digitsRightOfDecimal,
                                                                               QObject* receiver,
                                                                               const char* method);
        
    private:
        WuQFactory();
        
        virtual ~WuQFactory();
        
        WuQFactory(const WuQFactory&);

        WuQFactory& operator=(const WuQFactory&);
        
        static void setWindowsStyleForApple(QWidget* w);
        
        static QStyle* s_windowsStyleForApple;
        
        static bool s_windowsStyleForAppleWasCreated;
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    

//    class WorkbenchMacStyle : public QMacStyle {
//    public:
//        WorkbenchMacStyle() {}
//        
//        virtual ~WorkbenchMacStyle() {}
//        virtual int	styleHint ( StyleHint sh, const QStyleOption * opt = 0, const QWidget * w = 0, QStyleHintReturn * hret = 0 ) const;
//    };
    
#ifdef __WU_Q_FACTORY_DECLARE__
    QStyle* WuQFactory::s_windowsStyleForApple = NULL;
    bool WuQFactory::s_windowsStyleForAppleWasCreated;
#endif // __WU_Q_FACTORY_DECLARE__

} // namespace
#endif  //__WU_Q_FACTORY_H__
