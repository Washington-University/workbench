
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

#define __DATA_FILE_CONTENT_COPY_MOVE_PARAMETERS_DECLARE__
#include "DataFileContentCopyMoveParameters.h"
#undef __DATA_FILE_CONTENT_COPY_MOVE_PARAMETERS_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DataFileContentCopyMoveParameters 
 * \brief Parameters for copying/moving data in data files.
 * \ingroup Common
 */

/**
 * Constructor for data file that is copied/moved with all parameters off.
 *
 * @param dataFileCopyMoveInterface
 *     Interface of file that is copied/moved.
 * @param windowIndex
 *     Index of window (negative if invalid).
 */
DataFileContentCopyMoveParameters::DataFileContentCopyMoveParameters(const DataFileContentCopyMoveInterface* dataFileCopyMoveInterface,
                                                                     const int32_t windowIndex)
: CaretObject(),
m_dataFileCopyMoveInterface(dataFileCopyMoveInterface),
m_windowIndex(windowIndex),
m_optionSelectedItems(false)
{
    
}

/**
 * Destructor.
 */
DataFileContentCopyMoveParameters::~DataFileContentCopyMoveParameters()
{
}

/**
 * @return Copy/Move interface for file whose content is copy/moved.
 */
const DataFileContentCopyMoveInterface*
DataFileContentCopyMoveParameters::getDataFileCopyMoveInterfaceToCopy() const
{
    return m_dataFileCopyMoveInterface;
}

/**
 * @return Index of window (invalid if negative).
 */
int32_t
DataFileContentCopyMoveParameters::getWindowIndex() const
{
    return m_windowIndex;
}


/**
 * @return Is the option for copy/move selected items set?
 */
bool
DataFileContentCopyMoveParameters::isOptionSelectedItems() const
{
    return m_optionSelectedItems;
}

/**
 * Set the option for copy/move selected items.
 *
 * @param optionSelectedItems
 *     New status for copy/move of selected items.
 */
void
DataFileContentCopyMoveParameters::setOptionSelectedItems(const bool optionSelectedItems)
{
    m_optionSelectedItems = optionSelectedItems;
}
