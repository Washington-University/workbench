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

#include <QApplication>
#include <iostream>

#include "AString.h"
#include "CaretAssert.h"
#include "CaretHttpManager.h"
#include "CaretCommandLine.h"
#include "CaretLogger.h"
#include "CommandOperationManager.h"
#include "ProgramParameters.h"
#include "SessionManager.h"
#include "SystemUtilities.h"
#include "VolumeFile.h"

using namespace caret;


static int runCommand(int argc, char* argv[]) {
    
    ProgramParameters parameters(argc, argv);
    caret_global_commandLine = AString(argv[0]) + " " + parameters.getAllParametersInString();
    /*
     * Log the command parameters.
     */
    CaretLogFine("Running: " + caret_global_commandLine);
    
    CommandOperationManager* commandManager = NULL;
    int ret = 0;
    try {
        commandManager = CommandOperationManager::getCommandOperationManager();
        
        commandManager->runCommand(parameters);
        
    }
    catch (CommandException& e) {
        std::cerr << "While running '" << caret_global_commandLine << "':\nERROR: " << e.whatString().toStdString() << std::endl;
        ret = -1;
    }
    
    if (commandManager != NULL) {
        CommandOperationManager::deleteCommandOperationManager();
    }
    return ret;
}

int main(int argc, char* argv[]) {
    int result = 0;
    {
        /*
         * Handle uncaught exceptions
         */
        SystemUtilities::setHandlersForUnexpected(argc, argv);
        
        /*
         * Create the session manager.
         */
        SessionManager::createSessionManager();
        
        /*
         * Disable volume voxel coloring since it can be a little slow
         * and voxel coloring is not needed for any commands (at this
         * time).
         */
        VolumeFile::setVoxelColoringEnabled(false);
        
        QCoreApplication myApp(argc, argv);//so that it doesn't need to link against gui
        
        result = runCommand(argc, argv);
        
        /*
         * Delete the session manager.
         */
        SessionManager::deleteSessionManager();
        CaretHttpManager::deleteHttpManager();//does this belong in some other singleton manager?
        myApp.processEvents();//since we don't exec(), let it clean up any ->deleteLater()s
    }
    /*
     * See if any objects were not deleted.
     */
    CaretObject::printListOfObjectsNotDeleted(true);
    return result;
}
  
