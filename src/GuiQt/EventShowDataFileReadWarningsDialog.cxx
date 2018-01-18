
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __EVENT_DISPLAY_DATA_FILE_READ_WARNINGS_DIALOG_DECLARE__
#include "EventShowDataFileReadWarningsDialog.h"
#undef __EVENT_DISPLAY_DATA_FILE_READ_WARNINGS_DIALOG_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventShowDataFileReadWarningsDialog 
 * \brief This event is sent after files are read to show any warnings during file reading
 * \ingroup GuiQt
 *
 * All data files are tested for the prescence of warnings from reading of the files.
 * If there are warnings, a dialog is displayed listing the file read warnings.  
 * If there are no file read warnings, no dialog is displayed.
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window on which dialog is displayed.  
 *    If the window index is invalid, the warning
 *    will be displayed on an open browser window.
 */
EventShowDataFileReadWarningsDialog::EventShowDataFileReadWarningsDialog(const int32_t browserWindowIndex)
: Event(EventTypeEnum::EVENT_SHOW_FILE_DATA_READ_WARNING_DIALOG),
m_browserWindowIndex(browserWindowIndex)
{
    
}

/**
 * Destructor.
 */
EventShowDataFileReadWarningsDialog::~EventShowDataFileReadWarningsDialog()
{
}

/**
 * @return Index of window on which the dialog is displayed.  If
 * the index is not a valid window, the dialog is dispalayed over
 * any available browser window.
 */
int32_t
EventShowDataFileReadWarningsDialog::getBrowserWindowIndex() const
{
    return m_browserWindowIndex;
}

