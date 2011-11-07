
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __MANAGE_LOADED_FILES_DIALOG_DECLARE__
#include "ManageLoadedFilesDialog.h"
#undef __MANAGE_LOADED_FILES_DIALOG_DECLARE__

#include "Brain.h"

using namespace caret;


    
/**
 * \class ManageLoadedFilesDialog 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */
/**
 * Constructor.
 */
ManageLoadedFilesDialog::ManageLoadedFilesDialog(QWidget* parent,
                                                 Brain* brain)
: WuQDialogModal("Manager Loaded Files",
                 parent)
{
    this->setOkButtonText("");
    this->saveCheckedFilesPushButton = this->addUserPushButton("Save Checked Files");
}

/**
 * Destructor.
 */
ManageLoadedFilesDialog::~ManageLoadedFilesDialog()
{
    
}

/**
 * Called when a push button was added using addUserPushButton().
 * Subclasses MUST override this if user push buttons were 
 * added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 */
void 
ManageLoadedFilesDialog::userButtonPressed(QPushButton* userPushButton)
{
    std::cout << "Save checked files pushbutton was pressed." << std::endl;
}

