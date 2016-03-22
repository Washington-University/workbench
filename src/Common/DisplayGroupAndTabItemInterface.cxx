
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __DISPLAY_GROUP_AND_TAB_ITEM_INTERFACE_DECLARE__
#include "DisplayGroupAndTabItemInterface.h"
#undef __DISPLAY_GROUP_AND_TAB_ITEM_INTERFACE_DECLARE__

#include "CaretAssert.h"
using namespace caret;

/**
 * \class caret::DisplayGroupAndTabItemInterface
 * \brief Interface for items in a display group tab selection hierarchy
 * \ingroup Common
 */

/**
 * Constructor.
 */
DisplayGroupAndTabItemInterface::DisplayGroupAndTabItemInterface()
{
    
}

/**
 * Destructor.
 */
DisplayGroupAndTabItemInterface::~DisplayGroupAndTabItemInterface()
{
}

///**
// * Copy constructor.
// * @param obj
// *    Object that is copied.
// */
//DisplayGroupAndTabItemInterface::DisplayGroupAndTabItemInterface(const DisplayGroupAndTabItemInterface& obj)
//: CaretObject(obj)
//{
//    this->copyHelperDisplayGroupAndTabItemInterface(obj);
//}
//
///**
// * Assignment operator.
// * @param obj
// *    Data copied from obj to this.
// * @return 
// *    Reference to this object.
// */
//DisplayGroupAndTabItemInterface&
//DisplayGroupAndTabItemInterface::operator=(const DisplayGroupAndTabItemInterface& obj)
//{
//    if (this != &obj) {
//        CaretObject::operator=(obj);
//        this->copyHelperDisplayGroupAndTabItemInterface(obj);
//    }
//    return *this;    
//}
//
///**
// * Helps with copying an object of this type.
// * @param obj
// *    Object that is copied.
// */
//void 
//DisplayGroupAndTabItemInterface::copyHelperDisplayGroupAndTabItemInterface(const DisplayGroupAndTabItemInterface& obj)
//{
//    
//}

/**
 * Helps with display selection status of all children.
 *
 * @param displayGroupTabInterface
 *     Item that has all of its children selected for display.
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 * @return
 *     Selection status for children.  If there are no children,
 *     'UNSELECTED' is returned.
 */
TriStateSelectionStatusEnum::Enum
DisplayGroupAndTabItemInterface::getChildrenDisplaySelectedHelper(DisplayGroupAndTabItemInterface* displayGroupTabInterface,
                                                                   const DisplayGroupEnum::Enum displayGroup,
                                                                   const int32_t tabIndex)
{
    TriStateSelectionStatusEnum::Enum status = TriStateSelectionStatusEnum::UNSELECTED;
    
    std::vector<DisplayGroupAndTabItemInterface*> children = displayGroupTabInterface->getItemChildren();
    const int numChildren = static_cast<int32_t>(children.size());
    if (numChildren > 0) {
        int32_t selectedCount = 0;
        int32_t partialSelectedCount = 0;
        for (int32_t i = 0; i < numChildren; i++) {
            CaretAssertVectorIndex(children, i);
            switch (children[i]->getItemDisplaySelected(displayGroup,
                                                                  tabIndex)) {
                case TriStateSelectionStatusEnum::PARTIALLY_SELECTED:
                    partialSelectedCount++;
                    break;
                case TriStateSelectionStatusEnum::SELECTED:
                    selectedCount++;
                    break;
                case TriStateSelectionStatusEnum::UNSELECTED:
                    break;
            }
        }
        
        if (selectedCount == numChildren) {
            status = TriStateSelectionStatusEnum::SELECTED;
        }
        else if ((selectedCount > 0)
                 || (partialSelectedCount > 0)) {
            status = TriStateSelectionStatusEnum::PARTIALLY_SELECTED;
        }
    }
    
    return status;
}


/**
 * Helps with display selection of all children.
 *
 * @param displayGroupTabInterface
 *     Item that has all of its children selected for display.
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 * @param status
 *     New selection status.
 * @return
 *     True if there are children and their status was set, else false.
 */
bool
DisplayGroupAndTabItemInterface::setChildrenDisplaySelectedHelper(DisplayGroupAndTabItemInterface* displayGroupTabInterface,
                                                            const DisplayGroupEnum::Enum displayGroup,
                                                            const int32_t tabIndex,
                                                            const TriStateSelectionStatusEnum::Enum status)
{
    CaretAssert(displayGroupTabInterface);
    
    std::vector<DisplayGroupAndTabItemInterface*> children = displayGroupTabInterface->getItemChildren();
    for (std::vector<DisplayGroupAndTabItemInterface*>::iterator childIter = children.begin();
         childIter != children.end();
         childIter++) {
        DisplayGroupAndTabItemInterface* child = *childIter;
        child->setItemDisplaySelected(displayGroup,
                                      tabIndex,
                                      status);
    }
    
    return ( ! children.empty());
}
