
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __WU_Q_MACRO_STANDARD_ITEM_TYPES_DECLARE__
#include "WuQMacroStandardItemTypes.h"
#undef __WU_Q_MACRO_STANDARD_ITEM_TYPES_DECLARE__

#include <QStandardItem>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQMacroStandardItemTypes 
 * \brief Standard Items Types for Macro Classes that subclass QStandardItem
 * \ingroup Common
 */

/**
 * Constructor.
 */
WuQMacroStandardItemTypes::WuQMacroStandardItemTypes()
{
}

/**
 * Destructor.
 */
WuQMacroStandardItemTypes::~WuQMacroStandardItemTypes()
{
}

/**
 * @return Standard item type for WuQMacro
 */
int
WuQMacroStandardItemTypes::typeWuQMacro()
{
    return (QStandardItem::UserType + 1);
}

/**
 * @return Standard item type for WuQMacroCommand
 */
int
WuQMacroStandardItemTypes::typeWuQMacroCommand()
{
    return (QStandardItem::UserType + 2);
}


