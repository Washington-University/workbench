
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

#define __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__
#include "GroupAndNameHierarchyName.h"
#undef __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__

#include "CaretAssert.h"

using namespace caret;

/**
 * \class caret::GroupAndNameHierarchyName
 * \brief Maintains selection of a name in each 'DisplayGroupEnum'.
 */

/**
 * Constructor.
 * @param name
 *    The name.
 * @param key
 *    Key assigned to the name.
 */
GroupAndNameHierarchyName::GroupAndNameHierarchyName(const AString& name,
                                                     const int32_t key)
{
    this->name = name;
    this->key  = key;
    this->iconRGBA[0] = 0.0;
    this->iconRGBA[1] = 0.0;
    this->iconRGBA[2] = 0.0;
    this->iconRGBA[3] = 0.0;
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        this->selectedInDisplayGroup[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->selectedInTab[i] = true;
    }
    this->counter = 0;
}

/**
 * Destructor.
 */
GroupAndNameHierarchyName::~GroupAndNameHierarchyName()
{
}

/**
 * Copy the selections from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which selections are copied.
 * @param targetTabIndex
 *    Index of tab to which selections are copied.
 */
void
GroupAndNameHierarchyName::copySelections(const int32_t sourceTabIndex,
                                          const int32_t targetTabIndex)
{
    this->selectedInTab[targetTabIndex] = this->selectedInTab[sourceTabIndex];
}

/**
 * @return The RGBA color components for the icon.
 * Valid when alpha is greater than zero.
 * The color components range [0.0, 1.0]
 */
const float*
GroupAndNameHierarchyName::getIconColorRGBA() const
{
    return this->iconRGBA;
}

/**
 * Set the RGBA color components for the icon.
 * Valid when alpha is greater than zero.
 * @param rgba
 *     The color components ranging [0.0, 1.0]
 */
void
GroupAndNameHierarchyName::setIconColorRGBA(const float rgba[4])
{
    this->iconRGBA[0] = rgba[0];
    this->iconRGBA[1] = rgba[1];
    this->iconRGBA[2] = rgba[2];
    this->iconRGBA[3] = rgba[3];
}

/**
 * @return The name.
 */
AString
GroupAndNameHierarchyName::getName() const
{
    return this->name;
}

/**
 * @return The key.
 */
int32_t
GroupAndNameHierarchyName::getKey() const
{
    return this->key;
}

/**
 * Is this name selected for the given display group.
 * @param displayGroup
 *    Display Group for which selection status is requested.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return
 *    True if selected, else false.
 */
bool
GroupAndNameHierarchyName::isSelected(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectedInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectedInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return this->selectedInTab[tabIndex];
    }
    
    return this->selectedInDisplayGroup[displayIndex];
}

/**
 * Set name seletion status for the given display group.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param status
 *    New selection status.
 */
void
GroupAndNameHierarchyName::setSelected(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const bool status)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectedselectedInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectedInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        this->selectedInTab[tabIndex] = status;
    }
    else {
        this->selectedInDisplayGroup[displayIndex] = status;
    }
}

/**
 * Clear the counter.
 */
void
GroupAndNameHierarchyName::clearCounter()
{
    this->counter = 0;
}

/**
 * Increment the counter.
 */
void
GroupAndNameHierarchyName::incrementCounter()
{
    this->counter++;
}

/**
 * @return The value of the counter.
 */
int32_t
GroupAndNameHierarchyName::getCounter() const
{
    return this->counter;
}

