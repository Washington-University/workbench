
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
#include <QCloseEvent>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QMessageBox>

#include "Actions.h"
#include "BrainOpenGLWidget.h"
#include "GuiGlobals.h"
#include "WindowMain.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param openGLSizeX
 * @param openGLSizeY
 */
WindowMain::WindowMain(const int openGLSizeX,
                       const int openGLSizeY)
: QMainWindow()
{
    /*
     * This must be done first since other 
     * code may try to get main window from
     * the globals.
     */ 
    GuiGlobals::setMainWindow(this);
    
    this->setAttribute(Qt::WA_DeleteOnClose);

    this->openGLWidget =
        new BrainOpenGLWidget(this,
                              CaretWindowEnum::WINDOW_MAIN);
    
    this->openGLWidget->setMinimumSize(openGLSizeX, 
                                       openGLSizeY);
    
    this->setCentralWidget(this->openGLWidget);
    
    this->actions = GuiGlobals::getActions();
    
    this->createMenus();
    
    this->setWindowTitle("Caret 7");
}

/**
 * Destructor.
 */
WindowMain::~WindowMain()
{
    
}

/**
 * Process a close event.
 *
 * @param event
 *    The close event.
 */
void 
WindowMain::closeEvent(QCloseEvent* event)
{
    this->actions->getExitProgram()->trigger();
    event->ignore();
}

/**
 * Create the menus. 
 */
void 
WindowMain::createMenus()
{
    /*
     * Create the menu bar and add menus to it.
     */
    QMenuBar* menuBar = this->menuBar();
    menuBar->addMenu(this->createMenuFile());
    menuBar->addMenu(this->createMenuData());
    menuBar->addMenu(this->createMenuSurface());
    menuBar->addMenu(this->createMenuVolume());
    menuBar->addMenu(this->createMenuWindow());
    menuBar->addMenu(this->createMenuWindow());
}

QMenu* 
WindowMain::createMenuFile()
{
    /*
     * Create the menu.
     */
    QMenu* menu = new QMenu("File", this->openGLWidget);
    
    /*
     * Add items to the menu.
     */
    menu->addAction(this->actions->getDataFileOpen());
    menu->addAction(this->actions->getExitProgram());
    
    return menu;
}

QMenu* 
WindowMain::createMenuData()
{
    /*
     * Create the menu.
     */
    QMenu* menu = new QMenu("Data", this->openGLWidget);
    
    return menu;
}

QMenu*
WindowMain::createMenuSurface()
{
    /*
     * Create the menu.
     */
    QMenu* menu = new QMenu("Surface", this->openGLWidget);
    
    return menu;
}

QMenu* 
WindowMain::createMenuVolume()
{
    /*
     * Create the menu.
     */
    QMenu* menu = new QMenu("Volume", this->openGLWidget);
    
    return menu;
}

QMenu* 
WindowMain::createMenuWindow()
{
    /*
     * Create the menu.
     */
    QMenu* menu = new QMenu("Window", this->openGLWidget);
    
    return menu;    
}

QMenu* 
WindowMain::createMenuHelp()
{
    /*
     * Create the menu.
     */
    QMenu* menu = new QMenu("Help", this->openGLWidget);
    
    return menu;
}


