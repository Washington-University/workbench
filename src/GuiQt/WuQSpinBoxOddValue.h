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

/* Qt License is included since the API is copied from QSpinBox */

/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtGui module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/


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
