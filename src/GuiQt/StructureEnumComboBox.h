#ifndef __STRUCTURE_ENUM_COMBOBOX__H_
#define __STRUCTURE_ENUM_COMBOBOX__H_

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

#include "StructureEnum.h"
#include "WuQWidget.h"

namespace caret {

    class Surface;
    
    class StructureEnumComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        StructureEnumComboBox(QObject* parent);
        
        virtual ~StructureEnumComboBox();
        
        StructureEnum::Enum getSelectedStructure() const;
        
        int count() const;
        
        void listOnlyValidStructures();
        
        QWidget* getWidget();

    public slots:
        void setSelectedStructure(const StructureEnum::Enum structure);
        
    signals:
        void structureSelected(const StructureEnum::Enum);
        
    private slots:
        void structureComboBoxSelection(int);
        
    private:
        StructureEnumComboBox(const StructureEnumComboBox&);

        StructureEnumComboBox& operator=(const StructureEnumComboBox&);
        
        QComboBox* structureComboBox;
        
    public:
    private:
    };
    
#ifdef __STRUCTURE_ENUM_COMBOBOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __STRUCTURE_ENUM_COMBOBOX_DECLARE__

} // namespace
#endif  //__STRUCTURE_ENUM_COMBOBOX__H_
