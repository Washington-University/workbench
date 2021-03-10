#ifndef __WU_Q_DIALOG_MODAL__H_
#define __WU_Q_DIALOG_MODAL__H_

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

    class WuQDialogModal : public WuQDialog {
        Q_OBJECT
        
    public:
        WuQDialogModal(const AString& dialogTitle,
                       QWidget* parent,
                       Qt::WindowFlags f = Qt::WindowFlags());
        
        WuQDialogModal(const AString& dialogTitle,
                       QWidget* centralWidget,
                       QWidget* parent,
                       Qt::WindowFlags f = Qt::WindowFlags());
        
        virtual ~WuQDialogModal();
        
    
        void setOkButtonText(const AString& text);
        
        void setOkButtonEnabled(const bool enabled);
        
        void setCancelButtonText(const AString& text);

        void setSaveWindowPositionForNextTime(const AString& savePositionName = "");
    
    public slots:
        virtual void setVisible(bool);

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
