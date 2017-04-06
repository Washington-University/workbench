
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

#define __CARET_COLOR_ENUM_COMBOBOX_DECLARE__
#include "CaretColorEnumComboBox.h"
#undef __CARET_COLOR_ENUM_COMBOBOX_DECLARE__

#include <QComboBox>

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::CaretColorEnumComboBox 
 * \brief Control for selection of Caret Color enumerated types.
 *
 * Control for selection of Caret Color enumerated types.
 */

/**
 * Constructor.
 *
 * @param parent
 *     Parent object. 
 */
CaretColorEnumComboBox::CaretColorEnumComboBox(QObject* parent)
: WuQWidget(parent)
{
    initializeCaretColorComboBox("",
                                 NULL);
}

/**
 * Constructor.
 *
 * @param customColorSelectionName
 *     CaretColorEnum::CUSTOM is added to the combo with this name as the text.
 *     Text must NOT be empty.
 * @param parent
 *     Parent object.
 */
CaretColorEnumComboBox::CaretColorEnumComboBox(const AString& customColorSelectionName,
                                               QObject* parent)
: WuQWidget(parent)
{
    CaretAssert( ! customColorSelectionName.isEmpty());
    initializeCaretColorComboBox(customColorSelectionName,
                                 NULL);
}

/**
 * Constructor.
 *
 * @param customColorSelectionName
 *     CaretColorEnum::CUSTOM is added to the combo with this name as the text.
 *     Text must NOT be empty.
 * @param customColorSelectionIcon
 *     ICon for custom color.
 * @param parent
 *     Parent object.
 */
CaretColorEnumComboBox::CaretColorEnumComboBox(const AString& customColorSelectionName,
                                               const QIcon& customColorSelectionIcon,
                                               QObject* parent)
: WuQWidget(parent)
{
    CaretAssert( ! customColorSelectionName.isEmpty());
    initializeCaretColorComboBox(customColorSelectionName,
                                 &customColorSelectionIcon);
}

/**
 * Destructor.
 */
CaretColorEnumComboBox::~CaretColorEnumComboBox()
{
    
}

/**
 * Initialize instance that may have optional caret color enums.
 *
 * @param customColorSelectionName
 *     If NOT empty, CaretColorEnum::CUSTOM is added with text from this name.
 * @param customColorSelectionIcon
 *     ICON for custom color (ignored if NULL)
 */
void
CaretColorEnumComboBox::initializeCaretColorComboBox(const AString& customColorSelectionName,
                                                     const QIcon* customColorSelectionIcon)
{
    this->colorComboBox = new QComboBox();
    
    std::vector<CaretColorEnum::Enum> colors;
    
    int64_t caretColorOptions = 0;
    if ( ! customColorSelectionName.isEmpty()) {
        caretColorOptions |= CaretColorEnum::OPTION_INCLUDE_CUSTOM_COLOR;
    }
    CaretColorEnum::getColorAndOptionalEnums(colors,
                                             caretColorOptions);
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    for (int32_t i = 0; i < numColors; i++) {
        const CaretColorEnum::Enum colorEnum = colors[i];
        const int32_t indx = this->colorComboBox->count();
        AString name = CaretColorEnum::toGuiName(colorEnum);
        if (colorEnum == CaretColorEnum::CUSTOM) {
            if ( ! customColorSelectionName.isEmpty()) {
                name = customColorSelectionName;
            }
        }
        this->colorComboBox->addItem(name);
        this->colorComboBox->setItemData(indx,
                                         CaretColorEnum::toIntegerCode(colorEnum));
        
        /*
         * Create an icon with the color.
         */
        float rgba[4];
        CaretColorEnum::toRGBAFloat(colorEnum, rgba);
        if (colorEnum == CaretColorEnum::NONE) {
            rgba[3] = 0.0;
        }
        else if (colorEnum == CaretColorEnum::CUSTOM) {
            /*
             * If NO icon for CUSTOM
             */
            if (customColorSelectionIcon == NULL) {
                rgba[3] = 0.0;
            }
        }
        else {
            rgba[3] = 1.0;
        }
        
        if (rgba[3] > 0.0) {
            QPixmap pm(WuQtUtilities::createCaretColorEnumPixmap(getWidget(), 10, 10, colorEnum, rgba, false));
            QIcon icon(pm);
            if (colorEnum == CaretColorEnum::CUSTOM) {
                if (customColorSelectionIcon != NULL) {
                    icon = *customColorSelectionIcon;
                }
            }
            this->colorComboBox->setItemIcon(indx,
                                             icon);
        }
    }
    
    setSelectedColor(CaretColorEnum::BLACK);
    QObject::connect(this->colorComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(colorComboBoxIndexChanged(int)));
}

/**
 * @return The actual widget.
 */
QWidget* 
CaretColorEnumComboBox::getWidget()
{
    return this->colorComboBox;
}

/**
 * @return The selected color.
 */
CaretColorEnum::Enum 
CaretColorEnumComboBox::getSelectedColor()
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
CaretColorEnumComboBox::setSelectedColor(const CaretColorEnum::Enum color)
{
    const int32_t numColors = static_cast<int32_t>(this->colorComboBox->count());
    for (int32_t i = 0; i < numColors; i++) {
        const int32_t integerCode = this->colorComboBox->itemData(i).toInt();
        CaretColorEnum::Enum c = CaretColorEnum::fromIntegerCode(integerCode, NULL);
        if (c == color) {
            this->colorComboBox->blockSignals(true);
            this->colorComboBox->setCurrentIndex(i);
            this->colorComboBox->blockSignals(false);
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
CaretColorEnumComboBox::colorComboBoxIndexChanged(int indx)
{
    const int32_t integerCode = this->colorComboBox->itemData(indx).toInt();
    CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(integerCode, NULL);
    emit colorSelected(color);
}

