#ifndef __CARET_COLOR_ENUM_COMBOBOX__H_
#define __CARET_COLOR_ENUM_COMBOBOX__H_

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


#include "CaretColorEnum.h"
#include "WuQWidget.h"

class QComboBox;

namespace caret {

    class CaretColorEnumComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        CaretColorEnumComboBox(QObject* parent);
        
        CaretColorEnumComboBox(const AString& customColorSelectionName,
                               QObject* parent);
        
        CaretColorEnumComboBox(const AString& customColorSelectionName,
                               const QIcon& customColorSelectionIcon,
                               QObject* parent);
        
        virtual ~CaretColorEnumComboBox();
        
        CaretColorEnum::Enum getSelectedColor();
        
        void setSelectedColor(const CaretColorEnum::Enum color);
        
        QWidget* getWidget();
        
        QComboBox* getComboBox();
        
    signals:
        void colorSelected(const CaretColorEnum::Enum);
        
    private slots:
        void colorComboBoxIndexChanged(int);
        
    private:
        CaretColorEnumComboBox(const CaretColorEnumComboBox&);

        CaretColorEnumComboBox& operator=(const CaretColorEnumComboBox&);
        
    private:
        QComboBox* colorComboBox;

        void initializeCaretColorComboBox(const AString& customColorSelectionName,
                                          const QIcon* customColorSelectionIcon);
        
    };
    
#ifdef __CARET_COLOR_ENUM_COMBOBOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_COLOR_ENUM_COMBOBOX_DECLARE__

} // namespace
#endif  //__CARET_COLOR_ENUM_COMBOBOX__H_
