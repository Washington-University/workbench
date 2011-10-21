/*LICENSE_START*/
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

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QImage>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QTimer>

#include "WuQDialog.h"

using namespace caret;

/**
 * \class WuQDialog 
 * \brief Base class for dialogs.
 *
 * A base class for dialogs.
 */

/**
 * constructor.
 */
WuQDialog::WuQDialog(const bool isModalDialog,
                     QWidget* parent,
                     Qt::WindowFlags f)
   : QDialog(parent, f)
{
    this->setFocusPolicy(Qt::ClickFocus);
    
    //this->userLayout = new QVBoxLayout();
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    //dialogLayout->addLayout(userLayout);
}
              
/**
 * destructor.
 */
WuQDialog::~WuQDialog()
{
}
      
/**
 * called to capture image after timeout so nothing obscures window.
 */
void 
WuQDialog::slotCaptureImageAfterTimeOut()
{
   QImage image = QPixmap::grabWindow(this->winId()).toImage();
   if (image.isNull() == false) {
      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setImage(image);
      
      QMessageBox::information(this,
          "Information",
          "An image of this dialog has been placed onto the computer's clipboard.");
   }
}

/**
 * called to capture image of window and place it on the clipboard
 */
void 
WuQDialog::slotMenuCaptureImageOfWindowToClipboard()
{ 
   //
   // Need to delay capture so that the context sensistive
   // menu closes or else the menu will be in the captured image.
   //
   QApplication::processEvents();
   QTimer::singleShot(1000, this, SLOT(slotCaptureImageAfterTimeOut()));
}

/**
 * add a capture image of window menu item to the menu.
 */
void 
WuQDialog::addImageCaptureToMenu(QMenu* menu)
{
   menu->addAction("Capture Image to Clipboard",
                   this,
                   SLOT(slotMenuCaptureImageOfWindowToClipboard()));
}

/**
 * called by parent when context menu event occurs.
 */
void 
WuQDialog::contextMenuEvent(QContextMenuEvent* cme)
{
   //
   // Popup menu for selection of pages
   // 
   QMenu menu(this);

   //
   // Add menu item for image capture
   //
   addImageCaptureToMenu(&menu);
   
   //
   // Popup the menu
   //
   menu.exec(cme->globalPos());
}

/**
 * ring the bell.
 */
void 
WuQDialog::beep()
{
   QApplication::beep();
}

/**
 * show the watch cursor.
 */
void 
WuQDialog::showWaitCursor()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

/**
 * normal cursor.
 */
void 
WuQDialog::showNormalCursor()
{
   QApplication::restoreOverrideCursor();
}

/**
 * called to close.
 */
bool 
WuQDialog::close()
{
   return QDialog::close();
}
      

