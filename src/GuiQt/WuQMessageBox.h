
#ifndef __WU_QMESSAGE_BOX_H__
#define __WU_QMESSAGE_BOX_H__

/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QMessageBox>

namespace caret {
    class WuQMessageBox : public QMessageBox {
        
        Q_OBJECT
        
    public:
        
        static void errorOk(QWidget* parent,
                                  const QString& text);
        
        static void informationOk(QWidget* parent,
                                  const QString& text);
        
        static bool warningOkCancel(QWidget* parent,
                                    const QString& text);
        
        static bool warningYesNo(QWidget* parent,
                                    const QString& text);
        
        static bool warningOkCancel(QWidget* parent,
                                    const QString& text,
                                    const QString& informativeText);
        
        static bool warningYesNo(QWidget* parent,
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
