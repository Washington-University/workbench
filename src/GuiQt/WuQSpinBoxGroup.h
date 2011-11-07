#ifndef __WU_Q_SPIN_BOX_GROUP__H_
#define __WU_Q_SPIN_BOX_GROUP__H_

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
