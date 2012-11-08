#ifndef __GROUP_AND_NAME_ITEM_INTERFACE_H__
#define __GROUP_AND_NAME_ITEM_INTERFACE_H__

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "DisplayGroupEnum.h"
#include "GroupAndNameCheckStateEnum.h"

namespace caret {

    /**
     * \class caret::GroupAndNameItemInterface
     * \brief Interface for items in a group and name hierarchy.
     */
    class GroupAndNameItemInterface {
        
    public:
        /**
         * Type of item.
         */
        enum ItemType {
            /** Model (eg: file) */
            ITEM_TYPE_MODEL,
            /** Group (eg: class or map) */
            ITEM_TYPE_GROUP,
            /** Name (eg: border, focus, or label) */
            ITEM_TYPE_NAME
        };
        
        /**
         * Constructor.
         */
        GroupAndNameItemInterface(GroupAndNameItemInterface* parent = 0) { }
        
        /**
         * Destructor.
         */
        virtual ~GroupAndNameItemInterface() { }
        
        /**
         * @return The type of the item.
         */
        virtual ItemType getItemType() const = 0;
        
        /**
         * @return The name of the item.
         */
        virtual AString getName() const = 0;
        
        /**
         * Is this item selected?
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @return 
         *    True if item is selected, else false.
         */
        virtual bool isSelected(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex) const = 0;
        
        /**
         * Get the "check state" of an item.
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @return
         *    CHECKED if this item and ALL of its children are selected.
         *    PARTIALLY_CHECKED if this item is selected and any of its
         *       children, but not all of its children, are selected.
         *    UNCHECKED if this item is not selected.
         */
        virtual GroupAndNameCheckStateEnum::Enum getCheckState(const DisplayGroupEnum::Enum displayGroup,
                                                               const int32_t tabIndex) const = 0;
        
        /**
         * Set the selected status of this item only.  It does not alter
         * the status of ancestors and children.
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @param status
         *    True if item is selected, else false.
         */
        virtual void setSelected(const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const bool status) = 0;
        
        /**
         * Set the selected status for all of this item's children.
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @param status
         *    True if item is selected, else false.
         */
        virtual void setChildrenSelected(const DisplayGroupEnum::Enum displayGroup,
                                         const int32_t tabIndex,
                                         const bool status) = 0;
        
        /**
         * Set the selected status of this item's ancestor's (parent,
         * its parent, etc).
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @param status
         *    True if item is selected, else false.
         */
        virtual void setAncestorsSelected(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const bool status) = 0;
        
        /**
         * Set the selected status of this item, its ancestors, and all
         * of its children.
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @param status
         *    True if item is selected, else false.
         */
        virtual void setSelfAncestorsAndChildrenSelected(const DisplayGroupEnum::Enum displayGroup,
                                                      const int32_t tabIndex,
                                                      const bool status) = 0;
        
        /**
         * Get the RGBA color for an Icon that is displayed
         * in the selection control for this item.
         *
         * @return
         *     Pointer to the Red, Green, Blue, and Alpha
         *         color components for this item's icon.
         *     If no icon is to be displayed, NULL is returned.
         */
        virtual const float* getIconColorRGBA() const = 0;
        
        /**
         * Set the RGBA color components for an icon displayed in the
         * selection control for this item.
         * 
         * @param rgba
         *     The Red, Green, Blue, and Alpha color components.
         */
        virtual void setIconColorRGBA(const float rgba[4]) = 0;
        
        /**
         * Is this item expanded to display its children in the 
         * selection controls?
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @return
         *    True if children should be visible, else false.
         */
        virtual bool isExpandedToDisplayChildren(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex) const = 0;
        
        /**
         * Set this item expanded to display its children in the
         * selection controls.
         *
         * @param displayGroup
         *    The display group in which the item is controlled/viewed.
         * @param tabIndex
         *    Index of browser tab in which item is controlled/viewed.
         * @param expanded
         *    True if children should be visible, else false.
         */
        virtual void setExpandedToDisplayChildren(const DisplayGroupEnum::Enum displayGroup,
                                                  const int32_t tabIndex,
                                                  const bool expanded) = 0;
        
        /**
         * Copy the selections from one tab to another tab.
         * @param sourceTabIndex
         *     Index of source tab (copy "from")
         * @param targetTabIndex
         *     Index of target tab (copy "to")
         */
        virtual void copySelections(const int32_t sourceTabIndex,
                                    const int32_t targetTabIndex) = 0;
        
    private:
        GroupAndNameItemInterface(const GroupAndNameItemInterface&);

        GroupAndNameItemInterface& operator=(const GroupAndNameItemInterface&);
        
    };    
} // namespace

#endif  //__GROUP_AND_NAME_ITEM_INTERFACE_H__
