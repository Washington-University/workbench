#ifndef __WU_Q_DIALOG_MODAL__H_
#define __WU_Q_DIALOG_MODAL__H_

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
