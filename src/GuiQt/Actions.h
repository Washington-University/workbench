
#ifndef __ACTIONS_H__
#define __ACTIONS_H__

/*
 *  Copyright 1995-2002 Washington University School of Medicine
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
/*LICENSE_END*/

#include <QObject>

class QAction;


namespace caret  {

    class WindowMain;

    
    /**
     * Contains actions that are attached to and executed
     * by GUI components.  Placing all of the actions 
     * allows a single action to be used by multiple
     * components.
     */
    class Actions : public QObject {
        Q_OBJECT
        
    public:
        
        Actions(WindowMain* mainWindow);
        
        ~Actions();
        
        /**
         * Get the exit program action.
         *
         * @return
         *    Action for exiting the program.
         */
        QAction* getExitProgram() { return this->exitProgramAction; }
        
        /**
         * Get the data file open action.
         *
         * @return
         *    Action for opening a data file.
         */
        QAction* getDataFileOpen() { return this->dataFileOpenAction; }
        
        /**
         * Get the set current directory action.
         *
         * @return
         *    Action for setting current directory.
         */
        QAction* getSetCurrentDirectory() { return this->setCurrentDirectoryAction; }
        
        /**
         * Get the open data set action.
         *
         * @return
         *    Action for opening a data set.
         */
        QAction* getOpenDataSet() { return this->openDataSetAction; }
        
        /**
         * Get the closing data set action.
         *
         * @return
         *    Action for closing data set.
         */
        QAction* getCloseDataSet() { return this->closeDataSetAction; }
        
        /**
         * Get the manage/save files action.
         *
         * @return
         *    Action for managing and saving files.
         */
        QAction* getManageSaveFiles() { return this->manageSaveFilesAction; }
        
        /**
         * Get the capture single image action.
         *
         * @return
         *    Action for capturing a single image.
         */
        QAction* getSaptureSingleImage() { return this->captureSingleImageAction; }
        
        /**
         * Get the capture sequence of images action.
         *
         * @return
         *    Action for capturing a sequence of images.
         */
        QAction* getCaptureImageSequence() { return this->captureImageSequenceAction; }
        
    private slots:
        
        void processExitProgramAction();        
        
        void processDataFileOpenAction();
        
        void processSetCurrentDirectoryAction();
        
        void processOpenDataSetAction();
        
        void processCloseDataSetAction();
        
        void processManageSaveFilesAction();
        
        void processCaptureSingleImageAction();
        
        void processCaptureImageSequenceAction();
        
    private:
        Actions(const Actions&);
        Actions& operator=(const Actions&);
        
        WindowMain* mainWindow;
        
        QAction* exitProgramAction;        
        
        QAction* dataFileOpenAction;
        
        QAction* setCurrentDirectoryAction;
        
        QAction* openDataSetAction;
        
        QAction* closeDataSetAction;
        
        QAction* manageSaveFilesAction;
        
        QAction* captureSingleImageAction;
        
        QAction* captureImageSequenceAction;
        
    };

}  // namespace

#endif // __ACTIONS_H__
