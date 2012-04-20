#ifndef __WU_Q_DIALOG_NON_MODAL__H_
#define __WU_Q_DIALOG_NON_MODAL__H_

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

#include "WuQDialog.h"

namespace caret {

    class WuQDialogNonModal : public WuQDialog {
        Q_OBJECT
        
    public:
        WuQDialogNonModal(const AString& dialogTitle,
                          QWidget* parent = 0,
                          Qt::WindowFlags f = 0);
        
        virtual ~WuQDialogNonModal();
        
        void setApplyButtonText(const AString& text);
        
        void setCloseButtonText(const AString& text);
        
        /** May be called requesting the dialog to update its content */
        virtual void updateDialog() = 0;
        
        void setSavePositionForNextTime(const bool saveIt);
        
    protected slots:
        void apply();
        
    protected:
        virtual void applyButtonPressed();
        
        virtual void closeButtonPressed();

        virtual void closeEvent(QCloseEvent* event);
        
        virtual void showEvent(QShowEvent* event);
        
    private slots:
        void clicked(QAbstractButton* button);
        
    private:
        WuQDialogNonModal(const WuQDialogNonModal&);

        WuQDialogNonModal& operator=(const WuQDialogNonModal&);
        
        QPoint positionWhenClosed;
        bool positionWhenClosedValid;
        bool isPositionRestoredWhenReopened;
    };
    
#ifdef __WU_Q_DIALOG_NON_MODAL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_DIALOG_NON_MODAL_DECLARE__

} // namespace

#endif  //__WU_Q_DIALOG_NON_MODAL__H_
