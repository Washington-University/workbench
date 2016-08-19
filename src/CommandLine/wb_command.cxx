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

#include <cstdlib>
#include <ctime>
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
using namespace std;

static int runCommand(int argc, char* argv[]) {
    
    ProgramParameters parameters(argc, argv);
    caret_global_commandLine_init(parameters);
    /*
     * Log the command parameters.
     */
    CaretLogFine("Running: " + caret_global_commandLine);
    
    CommandOperationManager* commandManager = NULL;
    int ret = 0;
    try {
        commandManager = CommandOperationManager::getCommandOperationManager();
        
        commandManager->runCommand(parameters);
        
    } catch (CaretException& e) {
        cerr << "\nWhile running:\n" << caret_global_commandLine.toLocal8Bit().constData() << "\n\nERROR: " << e.whatString().toLocal8Bit().constData() << endl << endl;
        ret = -1;
    } catch (bad_alloc& e) {//if we stop using a handler for new
        cerr << "\nWhile running:\n" << caret_global_commandLine.toLocal8Bit().constData() << "\n\nERROR: " << e.what() << endl;
        cerr << endl
        << "OUT OF MEMORY" << endl
        << endl
        << "This means that Workbench is unable to get memory that it needs." << endl
        << "Possible causes:" << endl
        << "   (1) Your computer lacks sufficient RAM." << endl
        << "   (2) Swap space is too small (you might increase it)." << endl
        << "   (3) Your computer may be using an non-English character" << endl//is this relevant?
        << "       set.  Try switching to the English character set." << endl
        << endl;
        ret = -1;
    } catch (exception& e) {
        cerr << "\nWhile running:\n" << caret_global_commandLine.toLocal8Bit().constData() << "\n\nERROR: " << e.what() << endl << endl;
        ret = -1;
    } catch (...) {
        cerr << "\nWhile running:\n" << caret_global_commandLine.toLocal8Bit().constData() << "\n\nERROR: caught unknown exception type, rethrowing..." << endl << endl;
        if (commandManager != NULL) {
            CommandOperationManager::deleteCommandOperationManager();
        }
        throw;//rethrow, the runtime might print the type
    }
    
    if (commandManager != NULL) {
        CommandOperationManager::deleteCommandOperationManager();
    }
    return ret;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    int result = 0;
    {
        /*
         * Handle uncaught exceptions
         */
        SystemUtilities::setUnexpectedHandler();
        
        /*
         * Create the session manager.
         */
        SessionManager::createSessionManager(ApplicationTypeEnum::APPLICATION_TYPE_COMMAND_LINE);
        
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
  
