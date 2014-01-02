#ifndef __WU_Q_DIALOG_MODAL__H_
#define __WU_Q_DIALOG_MODAL__H_

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

    class WuQDialogModal : public WuQDialog {
        Q_OBJECT
        
    public:
        WuQDialogModal(const AString& dialogTitle,
                       QWidget* parent,
                       Qt::WindowFlags f = 0);
        
        WuQDialogModal(const AString& dialogTitle,
                       QWidget* centralWidget,
                       QWidget* parent,
                       Qt::WindowFlags f = 0);
        
        virtual ~WuQDialogModal();
        
    
        void setOkButtonText(const AString& text);
        
        void setCancelButtonText(const AString& text);

        QPushButton* addUserPushButton(const AString& text,
                                       const QDialogButtonBox::ButtonRole buttonRole);
        
        void setSaveWindowPositionForNextTime(const AString& savePositionName = "");
    
    protected:
        virtual void okButtonClicked();
        
        virtual void cancelButtonClicked();
        
        /**
         * Result of user button pressed.
         */
        enum ModalDialogUserButtonResult {
            /** accept which means OK pressed and dialog closes */
            RESULT_ACCEPT,
            /** reject which means Cancel pressed and dialog closes */
            RESULT_REJECT,
            /** none which means no action is taken and dialog remains open */
            RESULT_NONE
        };
        
        virtual ModalDialogUserButtonResult userButtonPressed(QPushButton* userPushButton);        
   
    public slots:
        virtual void setVisible(bool);

    private slots:
        void clicked(QAbstractButton* button);
        
    private:
        WuQDialogModal(const WuQDialogModal&);

        WuQDialogModal& operator=(const WuQDialogModal&);
    private:
        
        bool m_isSaveDialogPosition;
        AString m_saveDialogPositionName;
        
        struct SavedPosition {
            SavedPosition() {
                x = -1;
                y = -1;
                w = -1;
                h = -1;
            }
            int x;
            int y;
            int w;
            int h;
        };
        
        static std::map<QString, SavedPosition> s_savedDialogPositions;
    };
    
#ifdef __WU_Q_DIALOG_MODAL_DECLARE__
    std::map<QString, WuQDialogModal::SavedPosition> WuQDialogModal::s_savedDialogPositions;
#endif // __WU_Q_DIALOG_MODAL_DECLARE__

} // namespace
#endif  //__WU_Q_DIALOG_MODAL__H_
