
#ifndef __WU_QMESSAGE_BOX_H__
#define __WU_QMESSAGE_BOX_H__

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

#include <QMessageBox>

namespace caret {
    class WuQMessageBox : public QMessageBox {
        
        Q_OBJECT
        
    public:
        
        static void informationOk(QWidget* parent,
                                  const QString& text);
        
        static bool warningOkCancel(QWidget* parent,
                                    const QString& text);
        
        static bool warningOkCancel(QWidget* parent,
                                    const QString& text,
                                    const QString& informativeText);
        
        static bool warningCloseCancel(QWidget* parent,
                                       const QString& text,
                                       const QString& informativeText);
        
        static QMessageBox::StandardButton saveDiscardCancel(QWidget* parent,
                                                             const QString& text,
                                                             const QString& informativeText);
        
    private:
        WuQMessageBox(QWidget* parent = 0);
        ~WuQMessageBox();
        
        WuQMessageBox(const WuQMessageBox&);
        WuQMessageBox& operator=(const WuQMessageBox&);
        
    };
}

#endif // __WU_QMESSAGE_DIALOG_H__
