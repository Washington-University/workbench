#ifndef __CARET_COLOR_ENUM_SELECTION_CONTROL__H_
#define __CARET_COLOR_ENUM_SELECTION_CONTROL__H_

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


#include <QObject>

#include "CaretColorEnum.h"

class QComboBox;

namespace caret {

    class CaretColorEnumSelectionControl : public QObject {
        
        Q_OBJECT

    public:
        CaretColorEnumSelectionControl(QObject* parent,
                                       const uint64_t options = 0);
        
        virtual ~CaretColorEnumSelectionControl();
        
        CaretColorEnum::Enum getSelectedColor();
        
        void setSelectedColor(const CaretColorEnum::Enum color);
        
        QWidget* getWidget();
        
    signals:
        void colorSelected(const CaretColorEnum::Enum);
        
    private slots:
        void colorComboBoxIndexChanged(int);
        
    private:
        CaretColorEnumSelectionControl(const CaretColorEnumSelectionControl&);

        CaretColorEnumSelectionControl& operator=(const CaretColorEnumSelectionControl&);
        
    private:
        QComboBox* colorComboBox;
    };
    
#ifdef __CARET_COLOR_ENUM_SELECTION_CONTROL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_COLOR_ENUM_SELECTION_CONTROL_DECLARE__

} // namespace
#endif  //__CARET_COLOR_ENUM_SELECTION_CONTROL__H_
