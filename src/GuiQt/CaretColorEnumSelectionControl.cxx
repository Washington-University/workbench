
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

#define __CARET_COLOR_ENUM_SELECTION_CONTROL_DECLARE__
#include "CaretColorEnumSelectionControl.h"
#undef __CARET_COLOR_ENUM_SELECTION_CONTROL_DECLARE__

#include <QComboBox>

using namespace caret;


    
/**
 * \class CaretColorEnumSelectionControl 
 * \brief Control for selection of Caret Color enumerated types.
 *
 * Control for selection of Caret Color enumerated types.
 */
/**
 * Constructor.
 * @param includeSurfaceColor
 *   If true, the special color that indicates surface coloring is to
 *   be used is included in the color selections.
 */
CaretColorEnumSelectionControl::CaretColorEnumSelectionControl(const bool includeSurfaceColor)
: QObject()
{
    this->colorComboBox = new QComboBox();
    
    std::vector<CaretColorEnum::Enum> colors;
    CaretColorEnum::getAllEnums(colors,
                                includeSurfaceColor);
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    for (int32_t i = 0; i < numColors; i++) {
        const CaretColorEnum::Enum colorEnum = colors[i];
        const int32_t indx = this->colorComboBox->count();
        const AString name = CaretColorEnum::toGuiName(colorEnum);
        this->colorComboBox->addItem(name);
        this->colorComboBox->setItemData(indx, 
                                         CaretColorEnum::toIntegerCode(colorEnum));

    }
    
    QObject::connect(this->colorComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(colorComboBoxIndexChanged(int)));
}

/**
 * Destructor.
 */
CaretColorEnumSelectionControl::~CaretColorEnumSelectionControl()
{
    
}

/**
 * @return The actual widget.
 */
QWidget* 
CaretColorEnumSelectionControl::getWidget()
{
    return this->colorComboBox;
}

/**
 * @return The selected color.
 */
CaretColorEnum::Enum 
CaretColorEnumSelectionControl::getSelectedColor()
{
    const int32_t indx = this->colorComboBox->currentIndex();
    const int32_t integerCode = this->colorComboBox->itemData(indx).toInt();
    CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(integerCode, NULL);
    return color;
}

/**
 * Set the selected color.
 * @param color
 *   New color for selection.
 */
void 
CaretColorEnumSelectionControl::setSelectedColor(const CaretColorEnum::Enum color)
{
    const int32_t numColors = static_cast<int32_t>(this->colorComboBox->count());
    for (int32_t i = 0; i < numColors; i++) {
        const int32_t integerCode = this->colorComboBox->itemData(i).toInt();
        CaretColorEnum::Enum c = CaretColorEnum::fromIntegerCode(integerCode, NULL);
        if (c == color) {
            this->colorComboBox->setCurrentIndex(i);
            break;
        }
    }
}

/**
 * Called when a color is selected.
 * @param indx
 *   Index of item selected.
 */
void 
CaretColorEnumSelectionControl::colorComboBoxIndexChanged(int indx)
{
    const int32_t integerCode = this->colorComboBox->itemData(indx).toInt();
    CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(integerCode, NULL);
    emit colorSelected(color);
}

