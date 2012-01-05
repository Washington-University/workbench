#ifndef __WU_Q_SPIN_BOX_GROUP__H_
#define __WU_Q_SPIN_BOX_GROUP__H_

/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

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
