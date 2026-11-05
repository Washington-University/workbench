#ifndef __COMBO_BOX_ITEM_INTERFACE_COMBO_BOX_H__
#define __COMBO_BOX_ITEM_INTERFACE_COMBO_BOX_H__

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



#include <memory>

#include "WuQWidget.h"

class QComboBox;

namespace caret {

    class ComboBoxItemInterface;
    
    class ComboBoxItemInterfaceComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        ComboBoxItemInterfaceComboBox(QWidget* parent = 0);
        
        virtual ~ComboBoxItemInterfaceComboBox();
        
        ComboBoxItemInterfaceComboBox(const ComboBoxItemInterfaceComboBox&) = delete;

        ComboBoxItemInterfaceComboBox& operator=(const ComboBoxItemInterfaceComboBox&) = delete;
        
        void addItem(ComboBoxItemInterface* item);
        
        void addItems(std::vector<ComboBoxItemInterface*>& items);
        
        void updateComboBox(std::vector<ComboBoxItemInterface*>& items);
        
        int32_t count() const;
        
        ComboBoxItemInterface* currentItem() const;
        
        int32_t currentIndex() const;
        
        ComboBoxItemInterface* itemData(const int32_t index) const;
        
        virtual QWidget* getWidget() override;

        // ADD_NEW_METHODS_HERE

    public slots:
        void clear();
        
        void setCurrentItem(ComboBoxItemInterface* item);
        
        void setCurrentIndex(int index);

    signals:
        void activated(int index);
        
        void itemActivated(ComboBoxItemInterface* item);
        
    private:
        QComboBox* m_comboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __COMBO_BOX_ITEM_INTERFACE_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __COMBO_BOX_ITEM_INTERFACE_COMBO_BOX_DECLARE__

} // namespace
#endif  //__COMBO_BOX_ITEM_INTERFACE_COMBO_BOX_H__
