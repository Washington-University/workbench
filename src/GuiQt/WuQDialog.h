#ifndef __WU_QDIALOG_H__
#define __WU_QDIALOG_H__

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

#include <QDialog>
#include <QDialogButtonBox>

#include "AString.h"

class QKeyEvent;
class QMenu;
class QVBoxLayout;

namespace caret  {

    class WuQDialog : public QDialog {
        Q_OBJECT
        
    protected:
        WuQDialog(const AString& dialogTitle,
                  QWidget* parent,
                  Qt::WindowFlags f = 0);
        
    public:
        virtual ~WuQDialog();
        
        QDialogButtonBox* getDialogButtonBox();
        
        void setCentralWidget(QWidget* w);
        
        void setStandardButtonText(QDialogButtonBox::StandardButton button,
                                   const AString& text);
        
        void setDeleteWhenClosed(bool deleteFlag);
        
        static void beep();
        
        static void showWaitCursor();
        
        static void showNormalCursor();
        
    public slots:

        virtual bool close();
        
    protected slots:

        void slotMenuCaptureImageOfWindowToClipboard();
        
        void slotCaptureImageAfterTimeOut();
        
    protected:
        void addImageCaptureToMenu(QMenu* menu);
        
        virtual void contextMenuEvent(QContextMenuEvent*);
        
    private:
        QVBoxLayout* userWidgetLayout;
        
        QDialogButtonBox* buttonBox;
    };

}

#endif // __WU_QDIALOG_H__

