#ifndef __WU_Q_SPIN_BOX_GROUP__H_
#define __WU_Q_SPIN_BOX_GROUP__H_

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

#include <vector>
#include <QObject>

class QAbstractSpinBox;
class QDoubleSpinBox;
class QSpinBox;


namespace caret {
    class AString;
    class SpinBoxReceiver;
    
    class WuQSpinBoxGroup : public QObject {
        
        Q_OBJECT

    public:
        WuQSpinBoxGroup(QObject* parent);
        
        virtual ~WuQSpinBoxGroup();
        
        void addSpinBox(QAbstractSpinBox* abstractSpinBox);
        
    signals:
        /**
         * Emitted when a QSpinBox has its value changed.
         * @param spinBox
         *    Spin box that had its value changed.
         * @param i
         *    New value from spin box.
         */
        void spinBoxValueChanged(QSpinBox* spinBox,
                                 const int i);
        
        /**
         * Emitted when a QDoubleSpinBox has its value changed.
         * @param doubleSpinBox
         *    Double spin box that had its value changed.
         * @param d
         *    New value from spin box.
         */
        void doubleSpinBoxValueChanged(QDoubleSpinBox* doubleSpinBox,
                                       const double d);
        
        //void valueChanged(const AString& text);
        
    private:
        WuQSpinBoxGroup(const WuQSpinBoxGroup&);

        WuQSpinBoxGroup& operator=(const WuQSpinBoxGroup&);
        
        
        std::vector<SpinBoxReceiver*> spinBoxReceivers;
        
        void doubleSpinBoxChangeReceiver(QDoubleSpinBox* doubleSpinBox,
                                         const double d);
        
        void spinBoxChangeReceiver(QSpinBox* spinBox,
                                         const int i);
        
        friend class SpinBoxReceiver;
    };
    

    class SpinBoxReceiver : public QObject {
        Q_OBJECT
        
    public:
        SpinBoxReceiver(WuQSpinBoxGroup* spinBoxGroup,
                        QAbstractSpinBox* abstractSpinBox,
                        const int indx);
        
        ~SpinBoxReceiver();
        
    private slots:
        void valueChangedSlot(int i);
        
        void valueChangedSlot(double d);
        
    private:
        SpinBoxReceiver(const SpinBoxReceiver&);
        
        SpinBoxReceiver& operator=(const SpinBoxReceiver&);
        
        WuQSpinBoxGroup* spinBoxGroup;
        
        QSpinBox* spinBox;
        
        QDoubleSpinBox* doubleSpinBox;
        
        
        int spinBoxIndex;
        
    };
    
#ifdef __WU_Q_SPIN_BOX_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_SPIN_BOX_GROUP_DECLARE__

} // namespace
#endif  //__WU_Q_SPIN_BOX_GROUP__H_
