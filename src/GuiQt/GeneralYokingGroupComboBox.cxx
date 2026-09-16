
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __GENERAL_YOKING_GROUP_COMBO_BOX_DECLARE__
#include "GeneralYokingGroupComboBox.h"
#undef __GENERAL_YOKING_GROUP_COMBO_BOX_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GeneralYokingGroupComboBox 
 * \brief Combo box for selecting GeneralYokingGroupEnum
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param textMode
 *    Type of text to show in combo box
 */
GeneralYokingGroupComboBox::GeneralYokingGroupComboBox(const TextMode textMode,
                                                       QWidget* parent)
: QComboBox(parent)
{
    std::vector<GeneralYokingGroupEnum::Enum> allEnums;
    GeneralYokingGroupEnum::getAllEnums(allEnums);
    
    for (GeneralYokingGroupEnum::Enum e : allEnums) {
        AString text;
        switch (textMode) {
            case TextMode::ARABIC_NUMERAL:
                text = GeneralYokingGroupEnum::toArabicNumeralText(e);
                break;
            case TextMode::LOWER_CASE_ROMAN_NUMERAL:
                text = GeneralYokingGroupEnum::toLowerCaseRomanNumeralText(e);
                break;
            case TextMode::UPPER_CASE_ROMAN_NUMERAL:
                text = GeneralYokingGroupEnum::toUpperCaseRomanNumeralText(e);
                break;
            case TextMode::LOWER_CASE_ALPHABETICAL:
                text = GeneralYokingGroupEnum::tolowerCaseAlphabeticalText(e);
                break;
            case TextMode::UPPER_CASE_ALPHABETICAL:
                text = GeneralYokingGroupEnum::toUpperCaseAlphabeticalText(e);
                break;
        }
        addItem(text,
                GeneralYokingGroupEnum::toIntegerCode(e));
    }
    setCurrentIndex(0);
    
    QObject::connect(this, &QComboBox::activated,
                     this, &GeneralYokingGroupComboBox::itemActivated);
}

/**
 * Destructor.
 */
GeneralYokingGroupComboBox::~GeneralYokingGroupComboBox()
{
}

/**
 * Called when item is selected by user
 */
void
GeneralYokingGroupComboBox::itemActivated(int index)
{
    bool validFlag(false);
    GeneralYokingGroupEnum::Enum yg(GeneralYokingGroupEnum::fromIntegerCode(index,
                                                                                                        &validFlag));
    CaretAssert(validFlag);

    emit yokingGroupSelected(yg);
}

/**
 * @return The selected yoking group
 */
GeneralYokingGroupEnum::Enum
GeneralYokingGroupComboBox::getYokingGroup() const
{
    const int32_t intValue(currentIndex());
    bool validFlag(false);
    GeneralYokingGroupEnum::Enum yg(GeneralYokingGroupEnum::fromIntegerCode(intValue,
                                                                                                        &validFlag));
    CaretAssert(validFlag);
    return yg;
}

/**
 * Set the selected yoking group
 * @param yokingGroup
 *    New yokintg group
 */
void
GeneralYokingGroupComboBox::setYokingGroup(const GeneralYokingGroupEnum::Enum yokingGroup)
{
    const int32_t intValue(GeneralYokingGroupEnum::toIntegerCode(yokingGroup));
    QSignalBlocker blocker(this);
    setCurrentIndex(intValue);
}

