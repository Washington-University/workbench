#ifndef __WU_Q_TRUE_FALSE_COMBO_BOX__H_
#define __WU_Q_TRUE_FALSE_COMBO_BOX__H_

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


#include <QComboBox>

#include "WuQWidget.h"

namespace caret {

    class WuQTrueFalseComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        WuQTrueFalseComboBox(const QString& trueText,
                             const QString& falseText,
                             QObject* parent);
        
        WuQTrueFalseComboBox(QObject* parent);
        
        virtual ~WuQTrueFalseComboBox();
        
        QWidget* getWidget();
        
        bool isTrue();
        
        bool isFalse();
        
        void setStatus(const bool status);
    
    signals:
        /** Emitted when user makes a selection */
        void statusChanged(bool);
        
    private slots:
        void comboBoxValueChanged(int indx);
        
    private:
        WuQTrueFalseComboBox(const WuQTrueFalseComboBox&);

        WuQTrueFalseComboBox& operator=(const WuQTrueFalseComboBox&);

        void createComboBox(const QString& trueText,
                            const QString& falseText);
        QComboBox* comboBox;
    };
    
#ifdef __WU_Q_TRUE_FALSE_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TRUE_FALSE_COMBO_BOX_DECLARE__

} // namespace
#endif  //__WU_Q_TRUE_FALSE_COMBO_BOX__H_
