#ifndef __WU_Q_FACTORY_H__
#define __WU_Q_FACTORY_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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
