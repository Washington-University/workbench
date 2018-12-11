#ifndef __DISPLAY_GROUP_ENUM_COMBO_BOX__H_
#define __DISPLAY_GROUP_ENUM_COMBO_BOX__H_

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


#include "DisplayGroupEnum.h"
#include "WuQWidget.h"

class QComboBox;

namespace caret {

    class DisplayGroupEnumComboBox : public WuQWidget {
        Q_OBJECT
    public:
        DisplayGroupEnumComboBox(QObject* parent);
        
        DisplayGroupEnumComboBox(QObject* parent,
                                 const QString& objectNameForMacros);
        
        virtual ~DisplayGroupEnumComboBox();
        
        DisplayGroupEnum::Enum getSelectedDisplayGroup() const;
        
        QWidget* getWidget();
        
    public slots:
        void setSelectedDisplayGroup(const DisplayGroupEnum::Enum displayGroup);
        
    signals:
        void displayGroupSelected(const DisplayGroupEnum::Enum);
        
    private slots:
        void displayGroupComboBoxSelection(int);
        
    private:
        DisplayGroupEnumComboBox(const DisplayGroupEnumComboBox&);

        DisplayGroupEnumComboBox& operator=(const DisplayGroupEnumComboBox&);
        
        QComboBox* displayGroupComboBox;
    };
    
#ifdef __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__

} // namespace
#endif  //__DISPLAY_GROUP_ENUM_COMBO_BOX__H_
