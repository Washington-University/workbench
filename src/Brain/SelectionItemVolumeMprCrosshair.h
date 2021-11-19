#ifndef __SELECTION_ITEM_VOLUME_MPR_CROSSHAIR__H_
#define __SELECTION_ITEM_VOLUME_MPR_CROSSHAIR__H_

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

#include <vector>
#include "SelectionItem.h"

namespace caret {

    class SelectionItemVolumeMprCrosshair : public SelectionItem {
        
    public:
        enum class Axis {
            INVALID,
            INFERIOR_ROTATE,
            INFERIOR_SLICE,
            SUPERIOR_ROTATE,
            SUPERIOR_SLICE,
            POSTERIOR_ROTATE,
            POSTERIOR_SLICE,
            ANTERIOR_ROTATE,
            ANTERIOR_SLICE,
            LEFT_ROTATE,
            LEFT_SLICE,
            RIGHT_ROTATE,
            RIGHT_SLICE,
            RING_ROTATE
        };
        
        SelectionItemVolumeMprCrosshair();
        
        virtual ~SelectionItemVolumeMprCrosshair();
        
        SelectionItemVolumeMprCrosshair(const SelectionItemVolumeMprCrosshair&);
        
        SelectionItemVolumeMprCrosshair& operator=(const SelectionItemVolumeMprCrosshair&);
        
        virtual bool isValid() const;
        
        bool isRotateAxisSelected() const;
        
        bool isSliceAxisSelected() const;
        
        Axis getAxis() const;
        
        void setIdentification(Brain* brain,
                               const Axis axis,
                               const double screenDepth);

        virtual void reset();
        
        virtual AString toString() const;

        static AString axisToName(const Axis axis);
        
    protected:
        SelectionItemVolumeMprCrosshair(const SelectionItemDataTypeEnum::Enum itemDataType);
        
    private:
        void copyHelperSelectionItemVolumeMprCrosshair(const SelectionItemVolumeMprCrosshair& idItem);

        void resetPrivate();
        
        Axis m_axis = Axis::INVALID;
    };
    
#ifdef __SELECTION_ITEM_VOLUME_MPR_CROSSHAIR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SELECTION_ITEM_VOLUME_MPR_CROSSHAIR_DECLARE__

} // namespace
#endif  //__SELECTION_ITEM_VOLUME_MPR_CROSSHAIR__H_
