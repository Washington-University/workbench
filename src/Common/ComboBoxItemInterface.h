#ifndef __COMBO_BOX_ITEM_INTERFACE_H__
#define __COMBO_BOX_ITEM_INTERFACE_H__

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

#include <QVariant>

#include "AString.h"

namespace caret {

    class ComboBoxItemInterface {
        
    public:
        /**
         * @return Text for this item displayed in combo box
         * MUST OVERRIDE THIS METHOD
         */
        virtual AString comboBoxText() const = 0;
        
//        /**
//         * @return The variant converted to a ComboBoxItemInterface
//         * @param variant
//         *    Variant that is converted
//         */
//        static ComboBoxItemInterface* convertFromVariant(const QVariant& variant)
//        {
//            return static_cast<ComboBoxItemInterface*>(variant.value<void*>());
//        }

        /**
         * Constructor
         */
        ComboBoxItemInterface() { }
        
        /**
         * Destructor
         */
        virtual ~ComboBoxItemInterface() { };
        
        ComboBoxItemInterface(const ComboBoxItemInterface&) = delete;

        ComboBoxItemInterface& operator=(const ComboBoxItemInterface&) = delete;
        
//        /**
//         * @return "This" in a QVariant
//         */
//        QVariant toVariant() const {
//            return QVariant::fromValue(reinterpret_cast<const void*>(this));
//        }

//        /**
//         * @return Void pointer for this item (const)
//         */
//        const void* toVoidPointer() const { return this; }

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __COMBO_BOX_ITEM_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __COMBO_BOX_ITEM_INTERFACE_DECLARE__

} // namespace
#endif  //__COMBO_BOX_ITEM_INTERFACE_H__
