
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
#include <QColorDialog>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQMacroManager.h"
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
: CaretColorEnumComboBox(CustomColorModeEnum::DISABLED,
                         NoneColorModeEnum::DISABLED,
                         parent)
{
    /* delegating constructor above */
}

/**
 * Constructor.
 *
 * @param customColorMode
 *     Mode for custom color
 * @param noneColorMode
 *     Mode for none color
 * @param parent
 *     Parent object.
 */
CaretColorEnumComboBox::CaretColorEnumComboBox(const CustomColorModeEnum customColorMode,
                                               const NoneColorModeEnum noneColorMode,
                                               QObject* parent)
: WuQWidget(parent),
m_customColorMode(customColorMode),
m_noneColorMode(noneColorMode)
{
    this->colorComboBox = new QComboBox();
    
    std::vector<CaretColorEnum::Enum> colors;
    
    int64_t caretColorOptions = 0;
    switch (m_customColorMode) {
        case CustomColorModeEnum::DISABLED:
            break;
        case CustomColorModeEnum::EDITABLE:
            caretColorOptions |= CaretColorEnum::OPTION_INCLUDE_CUSTOM_COLOR;
            break;
        case CustomColorModeEnum::FIXED:
            caretColorOptions |= CaretColorEnum::OPTION_INCLUDE_CUSTOM_COLOR;
            break;
    }
    
    switch (m_noneColorMode) {
        case NoneColorModeEnum::DISABLED:
            break;
        case NoneColorModeEnum::ENABLED:
            caretColorOptions |= CaretColorEnum::OPTION_INCLUDE_NONE_COLOR;
            break;
    }
    
    CaretColorEnum::getColorAndOptionalEnums(colors,
                                             caretColorOptions);
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    for (int32_t i = 0; i < numColors; i++) {
        const CaretColorEnum::Enum colorEnum = colors[i];
        const int32_t indx = this->colorComboBox->count();
        AString name = CaretColorEnum::toGuiName(colorEnum);
        if (colorEnum == CaretColorEnum::CUSTOM) {
            m_customColorIndex = this->colorComboBox->count();
        }
        else if (colorEnum == CaretColorEnum::NONE) {
            m_noneColorIndex = this->colorComboBox->count();
        }
        
        this->colorComboBox->addItem(name);
        this->colorComboBox->setItemData(indx,
                                         CaretColorEnum::toIntegerCode(colorEnum));
        
        /*
         * Create an icon with the color.
         */
        uint8_t rgba[4];
        CaretColorEnum::toRGBAByte(colorEnum, rgba);
        if (colorEnum == CaretColorEnum::NONE) {
            rgba[3] = 0;
        }
        else if (colorEnum == CaretColorEnum::CUSTOM) {
            rgba[3] = 0;
        }
        else {
            rgba[3] = 255;
        }
        
        if (rgba[3] > 0.0) {
            setIconColor(indx, rgba);
//            QPixmap pm(WuQtUtilities::createCaretColorEnumPixmap(getWidget(), 10, 10, colorEnum, rgba, false));
//            QIcon icon(pm);
//            this->colorComboBox->setItemIcon(indx,
//                                             icon);
        }
    }
    
    setSelectedColor(CaretColorEnum::BLACK);
    QObject::connect(this->colorComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(colorComboBoxIndexChanged(int)));
}

/**
 * Set the icon color
 * @param index
 * Index in the combo box
 * @param rgba
 * RGBA components
 */
void
CaretColorEnumComboBox::setIconColor(const int32_t index,
                                     const uint8_t rgba[4])
{
    if (index >= 0) {
        const float floatRGBA[4] = {
            rgba[0] / 255.0f,
            rgba[1] / 255.0f,
            rgba[2] / 255.0f,
            rgba[3] / 255.0f
        };
        QPixmap pm(WuQtUtilities::createCaretColorEnumPixmap(getWidget(), 10, 10, CaretColorEnum::CUSTOM, floatRGBA, false));
        QIcon icon(pm);
        this->colorComboBox->setItemIcon(index,
                                         icon);
    }
}

/**
 * Destructor.
 */
CaretColorEnumComboBox::~CaretColorEnumComboBox()
{
    
}


/**
 * Override the custom color name
 * @param customColorName
 *  Name for use with custom color
 */
void
CaretColorEnumComboBox::setCustomColorName(const AString& customColorName)
{
    if (m_customColorIndex < 0) {
        CaretLogWarning("Attempt to set custom color name but custom color was not enabled");
        return;
    }
    
    this->colorComboBox->setItemText(m_customColorIndex,
                                     customColorName);
}

/**
 * Override the custom color icon
 * @param customColorIcon
 *  Icon for use with custom color
 */
void
CaretColorEnumComboBox::setCustomColorIcon(const QIcon& customColorIcon)
{
    if (m_customColorIndex < 0) {
        CaretLogWarning("Attempt to set custom color name but custom color was not enabled");
        return;
    }
    
    this->colorComboBox->setItemIcon(m_customColorIndex,
                                     customColorIcon);
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
 * @return The actual combo box encapsulated in this instance.
 */
QComboBox*
CaretColorEnumComboBox::getComboBox()
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
 * Get the custom color.
 * @param rgbOut
 *   Output with current custom color.
 */
void
CaretColorEnumComboBox::getCustomColor(std::array<uint8_t, 3>& rgbOut) const
{
    rgbOut = m_customColorRGB;
}

/**
 * Get the custom color.
 * @param rgbaOut
 *   Output with current custom color.
 */
void
CaretColorEnumComboBox::getCustomColor(std::array<uint8_t, 4>& rgbaOut) const
{
    rgbaOut[0] = m_customColorRGB[0];
    rgbaOut[1] = m_customColorRGB[1];
    rgbaOut[2] = m_customColorRGB[2];
    rgbaOut[3] = 255;
}


/**
 * Set the custom color.
 * @param rgb
 *   New custom color.
 */
void
CaretColorEnumComboBox::setCustomColor(const std::array<uint8_t, 3>& rgb)
{
    m_customColorRGB = rgb;
    uint8_t rgba[4] = {
        m_customColorRGB[0], m_customColorRGB[1], m_customColorRGB[2], 255
    };
    setIconColor(m_customColorIndex, rgba);
}

/**
 * Set the custom color.
 * @param rgba
 *   New custom color.
 */
void
CaretColorEnumComboBox::setCustomColor(const std::array<uint8_t, 4>& rgba)
{
    m_customColorRGB[0] = rgba[0];
    m_customColorRGB[1] = rgba[1];
    m_customColorRGB[2] = rgba[2];
    setCustomColor(m_customColorRGB);
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
    
    if (color == CaretColorEnum::CUSTOM) {
        switch (m_customColorMode) {
            case CustomColorModeEnum::DISABLED:
                break;
            case CustomColorModeEnum::EDITABLE:
            {
                const QColor initialColor(m_customColorRGB[0],
                                          m_customColorRGB[1],
                                          m_customColorRGB[2]);
                
                QColorDialog colorDialog(getWidget());
                colorDialog.setOption(QColorDialog::DontUseNativeDialog);
                colorDialog.setWindowTitle("Choose Color");
                colorDialog.setCurrentColor(initialColor);
                
                if (colorDialog.exec() == QColorDialog::Accepted) {
                    const QColor newColor = colorDialog.currentColor();
                    m_customColorRGB[0] = newColor.red();
                    m_customColorRGB[1] = newColor.green();
                    m_customColorRGB[2] = newColor.blue();
                }
            }
                break;
            case CustomColorModeEnum::FIXED:
                break;
        }
    }
    
    emit colorSelected(color);
}

