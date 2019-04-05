
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

#define __MAC_DUPLICATE_MENU_BAR_DECLARE__
#include "MacDuplicateMenuBar.h"
#undef __MAC_DUPLICATE_MENU_BAR_DECLARE__

#include <iostream>

#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QToolButton>


#include "AString.h"
#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::MacDuplicateMenuBar 
 * \brief A duplicate of the mac menu bar placed in the main window
 * \ingroup GuiQt
 *
 * The menu on macs is separate from the main window and placed at the
 * top of the display.  When creating screen images, such as for tutorials,
 * it makes it difficult to include the menu bar in the image.  This class
 * creates a copy of the menu bar in a widget that can be added to
 * the main window.
 */

/**
 * Constructor.
 *
 * @param mainWindow
 *     Main window whose menu bar is copied.
 * @param parent
 *     Optional parent widget.
 */
MacDuplicateMenuBar::MacDuplicateMenuBar(QMainWindow* mainWindow,
                                         QWidget* parent)
: QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QMargins margins = layout->contentsMargins();
    margins.setTop(0);
    margins.setBottom(0);
    layout->setContentsMargins(margins);
    
    CaretAssert(mainWindow);

    /*
     * Examine contents of menu bar and duplicate all items in it
     */
    QList<QAction*> menuList = mainWindow->menuBar()->actions();
    QListIterator<QAction*> iter(menuList);
    while (iter.hasNext()) {
        QAction* action = iter.next();
        QMenu* menu = action->menu();
        if (menu != NULL) {
            QMenu* dupMenu = duplicateMenu(menu);
            if (dupMenu != NULL) {
                /*
                 * Cannot add a menu bar to the window so each menu
                 * is attached to a new QToolButton
                 */
                QToolButton* tb = new QToolButton();
                tb->setPopupMode(QToolButton::InstantPopup);
                tb->setText(menu->title());
                tb->setMenu(dupMenu);
                layout->addWidget(tb);
            }
        }
    }
    layout->addStretch();
}

/**
 * Destructor.
 */
MacDuplicateMenuBar::~MacDuplicateMenuBar()
{
}

/**
 * Recursively duplicate the given menu.
 *
 * @param copyFromMenu
 *     Menu that is examined and copied
 * @return
 *     Pointer to duplicated menu or NULL if failed to duplicate.
 */
QMenu*
MacDuplicateMenuBar::duplicateMenu(QMenu* copyFromMenu)
{
    const bool printFlag(false);
    
    if (printFlag) {
        std::cout << std::endl;
        std::cout << m_indentText << "Menu: " << copyFromMenu->title() << std::endl;
    }
    m_indentText.append("   ");
    
    QMenu* newMenu = new QMenu(copyFromMenu->title());
    QList<QAction*> actionList = copyFromMenu->actions();
    QListIterator<QAction*> iter(actionList);
    while (iter.hasNext()) {
        QAction* action = iter.next();
        QMenu* subMenu = action->menu();
        if (subMenu != NULL) {
            QMenu* dupMenu = duplicateMenu(subMenu);
            if (dupMenu != NULL) {
                newMenu->addMenu(dupMenu);
            }
        }
        else if (action->isSeparator()) {
            if (printFlag) {
                std::cout << m_indentText << "Separator" << std::endl;
            }
            newMenu->addSeparator();
        }
        else {
            if (printFlag) {
                std::cout << m_indentText << "Item: " << action->text() << std::endl;
            }
            newMenu->addAction(action);
        }
    }
    
    m_indentText.resize(m_indentText.length() - 3);
    
    return newMenu;
}


