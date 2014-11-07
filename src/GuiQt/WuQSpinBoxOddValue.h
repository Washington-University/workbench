#ifndef __WU_Q_SPIN_BOX_ODD_VALUE_H__
#define __WU_Q_SPIN_BOX_ODD_VALUE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "WuQWidget.h"

#include <QSpinBox>

namespace caret {

    class WuQSpinBoxOddValueSpinBox : public QSpinBox {
    public:
        WuQSpinBoxOddValueSpinBox(QWidget* parent = 0);
        
        virtual ~WuQSpinBoxOddValueSpinBox();
        
    protected:
        virtual int valueFromText(const QString& text) const;
    };
    
    class WuQSpinBoxOddValue : public WuQWidget {
        
        Q_OBJECT

    public:
        WuQSpinBoxOddValue(QObject* parent);
        
        virtual ~WuQSpinBoxOddValue();
        
        virtual QWidget* getWidget();
        
        int minimum() const;
        
        int maximum() const;
        
        void setMinimum(int min);
        
        void setMaximum(int max);
        
        void setRange(int minimum, int maximum);
        
        int singleStep() const;
        
        void setSingleStep(int val);
        
        int value() const;
        
        // ADD_NEW_METHODS_HERE

    signals:
        void valueChanged(int i);
        
    public slots:
        void setValue(int val);
        
    private:
        WuQSpinBoxOddValue(const WuQSpinBoxOddValue&);

        WuQSpinBoxOddValue& operator=(const WuQSpinBoxOddValue&);
        
        QSpinBox* m_spinBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_SPIN_BOX_ODD_VALUE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_SPIN_BOX_ODD_VALUE_DECLARE__

} // namespace
#endif  //__WU_Q_SPIN_BOX_ODD_VALUE_H__
