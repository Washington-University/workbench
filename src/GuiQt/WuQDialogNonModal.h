#ifndef __WU_Q_DIALOG_NON_MODAL__H_
#define __WU_Q_DIALOG_NON_MODAL__H_

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

#include "WuQDialog.h"

namespace caret {

    class WuQDialogNonModal : public WuQDialog {
        Q_OBJECT
        
    public:
        WuQDialogNonModal(const AString& dialogTitle,
                          QWidget* parent = 0,
                          Qt::WindowFlags f = Qt::WindowFlags());
        
        virtual ~WuQDialogNonModal();
        
        void setApplyButtonText(const AString& text);
        
        void setCloseButtonText(const AString& text);
        
        void showDialog();
        
        virtual void updateDialog();
        //virtual void updateDialog() = 0;
        
        void setSaveWindowPositionForNextTime(const bool saveIt);
        
    signals:
        /**
         * This signal is emitted when the dialog is closed (hidden).
         */
        void dialogWasClosed();
        
    protected slots:
        void apply();
        
    protected:
        virtual void closeEvent(QCloseEvent* event);
        
        virtual void showEvent(QShowEvent* event);
        
    private:
        WuQDialogNonModal(const WuQDialogNonModal&);

        WuQDialogNonModal& operator=(const WuQDialogNonModal&);
        
        QPoint m_positionWhenClosed;
        QSize  m_sizeWhenClosed;
        bool m_positionWhenClosedValid;
        bool m_isPositionRestoredWhenReopened;
    };
    
#ifdef __WU_Q_DIALOG_NON_MODAL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_DIALOG_NON_MODAL_DECLARE__

} // namespace

#endif  //__WU_Q_DIALOG_NON_MODAL__H_
