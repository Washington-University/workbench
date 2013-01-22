#ifndef __STRUCTURE_ENUM_COMBOBOX__H_
#define __STRUCTURE_ENUM_COMBOBOX__H_

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
