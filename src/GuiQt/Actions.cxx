

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

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

#include "Actions.h"
#include "Brain.h"
#include "GuiGlobals.h"
#include "WindowMain.h"


using namespace caret;

/**
 * Constructor.
 *
 * @param mainWindow
 *    Pointer to main window.
 */
Actions::Actions(WindowMain* mainWindow)
{
    this->mainWindow = mainWindow;
    
    this->dataFileOpenAction = 
            this->createAction("Open Data File...", 
                               Qt::CTRL+Qt::Key_O,
                               SLOT(processDataFileOpenAction()));

    this->exitProgramAction =
            this->createAction("Exit", 
                               Qt::CTRL+Qt::Key_Q, 
                               SLOT(processExitProgramAction()));
    
/*
    this->dataFileOpenAction = new QAction(this->mainWindow);
    this->dataFileOpenAction->setText("Open Data File...");
    this->dataFileOpenAction->setShortcut(Qt::CTRL+Qt::Key_O);
    QObject::connect(this->dataFileOpenAction, SIGNAL(triggered(bool)),
                     this, SLOT(processDataFileOpenAction()));

    this->exitProgramAction = new QAction(this->mainWindow);
    this->exitProgramAction->setText("Exit");
    this->exitProgramAction->setShortcut(Qt::CTRL+Qt::Key_M);
    QObject::connect(this->exitProgramAction, SIGNAL(triggered(bool)),
                     this, SLOT(processExitProgramAction()));
*/
}

/**
 * Destructor.
 */
Actions::~Actions()
{
    
}

/**
 * Create an action with the specified text, shortcut, 
 * and calls the specified slot.
 * 
 * @param text
 *    Text for the action.
 * @param method
 *    Slot that is called by the action.
 * @param shortcut
 *    Keyboard shortcut.
 * @return
 *    Action that was created.
 */
QAction* 
Actions::createAction(const QString& text,
                      const QKeySequence& shortcut,
                      const char* method)
{
    QAction* action = this->createAction(text,
                                         method);
    action->setShortcut(shortcut);
    return action;
}

/**
 * Create an action with the specified text and calls 
 * the specified slot.
 * 
 * @param text
 *    Text for the action.
 * @param method
 *    Slot that is called by the action.
 * @return
 *    Action that was created.
 */
QAction* 
Actions::createAction(const QString& text,
                      const char* method)
{
    QAction* action = new QAction(this->mainWindow);
    action->setText(text);
    QObject::connect(action, 
                     SIGNAL(triggered(bool)),
                     this,
                     method);
    return action;
}

/**
 * Exit the program.
 */
void 
Actions::processExitProgramAction()        
{
    if (QMessageBox::warning(this->mainWindow,
                             "Caret 7",
                             "Are you sure you want to quit ?",
                             (QMessageBox::Yes | QMessageBox::No),
                             QMessageBox::Yes)
        == QMessageBox::Yes) {
        qApp->quit();
    }
}

/**
 * Open a data file.
 */
void 
Actions::processDataFileOpenAction()
{
    QString name =
        QFileDialog::getOpenFileName(this->mainWindow,
                                 "Open Surface File",
                                 ".",
                                 "Surfaces (*.surf.gii)");
    if(name.length()==0) return;

    Brain* brain = GuiGlobals::getBrain();
    try {
        brain->readSurfaceFile(name.toStdString());
    }
    catch (DataFileException e) {
        QMessageBox::critical(this->mainWindow, "ERROR", QString::fromStdString(e.whatString()));
    }
    GuiGlobals::redrawAllGraphicsWindows();
}

/**
 * Set the current directory.
 */
void 
Actions::processSetCurrentDirectoryAction()
{
    
}

/**
 * Open a data set.
 */
void 
Actions::processOpenDataSetAction()
{
    
}

/**
 * Close a data set.
 */
void 
Actions::processCloseDataSetAction()
{
    
}

/**
 * manage and save files.
 */
void 
Actions::processManageSaveFilesAction()
{
    
}

/**
 * Capture single images.
 */
void 
Actions::processCaptureSingleImageAction()
{
    
}

/**
 * Capture a sequence of images.
 */
void 
Actions::processCaptureImageSequenceAction()
{
    
}

