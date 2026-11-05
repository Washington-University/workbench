
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

#define __COMBO_BOX_ITEM_INTERFACE_COMBO_BOX_DECLARE__
#include "ComboBoxItemInterfaceComboBox.h"
#undef __COMBO_BOX_ITEM_INTERFACE_COMBO_BOX_DECLARE__

#include <cstdint>
#include <vector>

#include <QComboBox>
#include <QVariant>

#include "CaretAssert.h"
#include "ComboBoxItemInterface.h"

using namespace caret;

/**
 * \class caret::ComboBoxItemInterfaceComboBox
 * \brief A combo box whose entries are ComboBoxItemInterface items
 *
 * The combo box does NOT take ownership of the items.  The caller must keep
 * each item alive for as long as it is in this combo box (or call clear()
 * before the item is destroyed).
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent of this object and of the combo box widget.
 */
ComboBoxItemInterfaceComboBox::ComboBoxItemInterfaceComboBox(QWidget* parent)
: WuQWidget(parent)
{
    m_comboBox = new QComboBox(parent);
    
    QObject::connect(m_comboBox, QOverload<int>::of(&QComboBox::activated),
                     this, [=](int index) {
        if ((index >= 0)
            && (index < m_comboBox->count())) {
            emit itemActivated(itemData(index));
            emit activated(index);
        }
    });
}

/**
 * Destructor.
 *
 * The combo box is destroyed by Qt when its parent is destroyed.  If this
 * object was created with no parent, the widget is owned by whatever layout or
 * widget it was added to via getWidget().
 */
ComboBoxItemInterfaceComboBox::~ComboBoxItemInterfaceComboBox()
{
}

/**
 * Add an item to the end of the combo box.
 *
 * @param item
 *    Item to add.  Not owned by this combo box.  Null items are ignored.
 */
void
ComboBoxItemInterfaceComboBox::addItem(ComboBoxItemInterface* item)
{
    if (item == nullptr) {
        return;
    }

    QVariant variant(QVariant::fromValue((void*)item));
    m_comboBox->addItem(item->comboBoxText(),
                        variant);
}

/**
 * Add items to the end of the combo box.
 *
 * @param items
 *    Items to add, in order.  Not owned by this combo box.  Null items are ignored.
 */
void
ComboBoxItemInterfaceComboBox::addItems(std::vector<ComboBoxItemInterface*>& items)
{
    for (ComboBoxItemInterface* item : items) {
        addItem(item);
    }
}

/**
 * Update items in the combo box while maintaining current selection
 *
 * @param items
 *    Items to add, in order.  Not owned by this combo box
 */
void
ComboBoxItemInterfaceComboBox::updateComboBox(std::vector<ComboBoxItemInterface*>& items)
{
    ComboBoxItemInterface* selectedItem(currentItem());
    
    clear();
    addItems(items);
    
    setCurrentItem(selectedItem);
}

/**
 * @return Number of items in the combo box.
 */
int32_t
ComboBoxItemInterfaceComboBox::count() const
{
    return m_comboBox->count();
}

/**
 * @return The selected item, or nullptr if there is no selection.
 */
ComboBoxItemInterface*
ComboBoxItemInterfaceComboBox::currentItem() const
{
    const int32_t index = currentIndex();
    return itemData(index);
}

/**
 * @return Item at given index or null if invalid index
 * @param index
 *    Index of item
 */
ComboBoxItemInterface*
ComboBoxItemInterfaceComboBox::itemData(const int32_t index) const
{
    ComboBoxItemInterface* itemOut(NULL);
    if ((index >= 0)
        && (index < m_comboBox->count())) {
        void* pointer(m_comboBox->itemData(index).value<void*>());
        itemOut = (ComboBoxItemInterface*)pointer;
    }
    return itemOut;
}

/**
 * @return Index of the selected item, or -1 if there is no selection.
 */
int32_t
ComboBoxItemInterfaceComboBox::currentIndex() const
{
    return m_comboBox->currentIndex();
}

/**
 * @return The combo box as a QWidget, for adding to layouts.
 */
QWidget*
ComboBoxItemInterfaceComboBox::getWidget()
{
    return m_comboBox;
}

/**
 * Remove all items from the combo box.  The items are not deleted.
 */
void
ComboBoxItemInterfaceComboBox::clear()
{
    m_comboBox->clear();
}

/**
 * Select the given item.  If the item is not in the combo box, the
 * selection is unchanged.  Does not emit activated() or itemActivated()
 * (same as QComboBox::setCurrentIndex()).
 *
 * @param item
 *    Item to select.
 */
void
ComboBoxItemInterfaceComboBox::setCurrentItem(ComboBoxItemInterface* item)
{
    if (item == nullptr) {
        return;
    }
    const int32_t num = m_comboBox->count();
    for (int32_t i = 0; i < num; i++) {
        if (itemData(i) == item) {
            m_comboBox->setCurrentIndex(i);
            return;
        }
    }
}

/**
 * Select the item at the given index.  Does not emit activated() or
 * itemActivated().
 *
 * @param index
 *    Index of item to select, or -1 for no selection.
 */
void
ComboBoxItemInterfaceComboBox::setCurrentIndex(int index)
{
    m_comboBox->setCurrentIndex(index);
}
