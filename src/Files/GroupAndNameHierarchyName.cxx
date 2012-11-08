
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
 *
 * Methods that operate on a boolean value are used to query and set
 * the selected status.  A separate method is provided to query
 * the 'check state'.  The 'check state' may be unchecked, checked, or
 * partially checked (some children checked but not all).
 */

/**
 * Constructor.
 * @param name
 *    The name.
 * @param idNumber
 *    ID number assigned to the name.
 */
GroupAndNameHierarchyName::GroupAndNameHierarchyName(const AString& name,
                                                     const int32_t idNumber)
: GroupAndNameAbstractItem(GroupAndNameAbstractItem::ITEM_TYPE_NAME,
                           name,
                           idNumber)
{
}

/**
 * Destructor.
 */
GroupAndNameHierarchyName::~GroupAndNameHierarchyName()
{
}

