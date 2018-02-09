#ifndef __LOCK_ASPECT_WARNING_DIALOG_H__
#define __LOCK_ASPECT_WARNING_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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



#include <memory>

#include <QDialog>

class QCheckBox;


namespace caret {

    class LockAspectWarningDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class Result {
            LOCK_ASPECT,
            NO_CHANGES,
            CANCEL
        };
        
        static Result runDialog(const int32_t browserWindowIndex,
                                QWidget* parent);

        LockAspectWarningDialog(QWidget* parent = 0);
        
        virtual ~LockAspectWarningDialog();
        
        Result getResult() const;
        
        bool isDoNotShowAgainChecked() const;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void helpButtonClicked();
        
        void buttonClicked(const Result buttonClicked);
        
    private:
        LockAspectWarningDialog(const LockAspectWarningDialog&);

        LockAspectWarningDialog& operator=(const LockAspectWarningDialog&);
        
        Result m_result = Result::CANCEL;
        
        QCheckBox* m_doNotShowAgainCheckBox;
        
        static bool s_doNotShowAgainStatusFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LOCK_ASPECT_WARNING_DIALOG_DECLARE__
    bool LockAspectWarningDialog::s_doNotShowAgainStatusFlag = false;
#endif // __LOCK_ASPECT_WARNING_DIALOG_DECLARE__

} // namespace
#endif  //__LOCK_ASPECT_WARNING_DIALOG_H__
