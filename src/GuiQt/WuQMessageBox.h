
#ifndef __WU_QMESSAGE_BOX_H__
#define __WU_QMESSAGE_BOX_H__

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

#include <QMessageBox>

namespace caret {
    class WuQMessageBox : public QMessageBox {
        
        Q_OBJECT
        
    public:
        enum YesNoCancelResult {
            RESULT_YES,
            RESULT_NO,
            RESULT_CANCEL
        };
        
        static void errorOk(QWidget* parent,
                                  const QString& text);
        
        static void informationOk(QWidget* parent,
                                  const QString& text);
        
        static void warningOk(QWidget* parent,
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
        
        static YesNoCancelResult warningYesNoCancel(QWidget* parent,
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
