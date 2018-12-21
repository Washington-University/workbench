
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

#define __WUQ_MACRO_SHORT_CUT_COMBOBOX_DECLARE__
#include "WuQMacroShortCutKeyComboBox.h"
#undef __WUQ_MACRO_SHORT_CUT_COMBOBOX_DECLARE__

using namespace caret;

/**
 * \class caret::WuQMacroShortCutKeyComboBox
 * \brief Control for selection of a macro short cut key.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *   The parent.
 */
WuQMacroShortCutKeyComboBox::WuQMacroShortCutKeyComboBox(QObject* parent)
: WuQWidget(parent)
{
    std::vector<WuQMacroShortCutKeyEnum::Enum> allShortCutKeys;
    WuQMacroShortCutKeyEnum::getAllEnums(allShortCutKeys);
    const int32_t numShortCutKeys = static_cast<int32_t>(allShortCutKeys.size());
    
    m_shortCutKeyComboBox = new QComboBox();
    for (int32_t i = 0; i < numShortCutKeys; i++) {
        m_shortCutKeyComboBox->addItem(WuQMacroShortCutKeyEnum::toGuiName(allShortCutKeys[i]));
        m_shortCutKeyComboBox->setItemData(i, WuQMacroShortCutKeyEnum::toIntegerCode(allShortCutKeys[i]));
    }
    
    QObject::connect(m_shortCutKeyComboBox, SIGNAL(activated(int)),
                     this, SLOT(shortCutKeyComboBoxSelection(int)));
    
}

/**
 * Destructor.
 */
WuQMacroShortCutKeyComboBox::~WuQMacroShortCutKeyComboBox()
{
    
}

/**
 * Called to set the selected short cut key.
 * @param shortCutKey
 *    New value for short cut key.
 */
void 
WuQMacroShortCutKeyComboBox::setSelectedShortCutKey(const WuQMacroShortCutKeyEnum::Enum shortCutKey)
{
    const int32_t shortCutIntegerCode = WuQMacroShortCutKeyEnum::toIntegerCode(shortCutKey);
    
    const int numShortCutKeys = m_shortCutKeyComboBox->count();
    for (int32_t i = 0; i < numShortCutKeys; i++) {
        if (shortCutIntegerCode == m_shortCutKeyComboBox->itemData(i).toInt()) {
            if (this->signalsBlocked()) {
                m_shortCutKeyComboBox->blockSignals(true);
            }
            
            m_shortCutKeyComboBox->setCurrentIndex(i);
            
            if (this->signalsBlocked()) {
                m_shortCutKeyComboBox->blockSignals(false);
            }
            break;
        }
    }
}

/**
 * @return  The selected short cut key.
 */
WuQMacroShortCutKeyEnum::Enum
WuQMacroShortCutKeyComboBox::getSelectedShortCutKey() const
{
    WuQMacroShortCutKeyEnum::Enum shortCutKey = WuQMacroShortCutKeyEnum::Key_None;
    const int32_t indx = m_shortCutKeyComboBox->currentIndex();
    if (indx >= 0) {
        const int32_t integerCode = m_shortCutKeyComboBox->itemData(indx).toInt();
        shortCutKey = WuQMacroShortCutKeyEnum::fromIntegerCode(integerCode, NULL);
    }
    return shortCutKey;
}

/**
 * @return The widget for this control.
 */
QWidget* 
WuQMacroShortCutKeyComboBox::getWidget()
{
    return m_shortCutKeyComboBox;
}

/**
 * Called when a short cut key is selected 
 * @param indx
 *   Index of selection.
 */
void 
WuQMacroShortCutKeyComboBox::shortCutKeyComboBoxSelection(int indx)
{
    if (this->signalsBlocked() == false) {
        if ((indx >= 0) &&
            (indx < m_shortCutKeyComboBox->count())) {
            const int32_t integerCode = m_shortCutKeyComboBox->itemData(indx).toInt();
            WuQMacroShortCutKeyEnum::Enum shortCutKey = WuQMacroShortCutKeyEnum::fromIntegerCode(integerCode, NULL);
            emit shortCutKeySelected(shortCutKey);
        }
    }
}
