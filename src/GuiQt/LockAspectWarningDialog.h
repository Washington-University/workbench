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



namespace caret {

    class LockAspectWarningDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class Result {
            LOCK_WINDOW_ASPECT_AND_ALL_TAB_ASPECTS,
            LOCK_WINDOW_ASPECT,
            NO_CHANGES,
            CANCEL
        };
        
        enum class TabMode {
            ALL_TABS,
            SELECTED_TAB
        };
        
        static Result runDialog(const int32_t browserWindowIndex,
                                QWidget* parent);

        LockAspectWarningDialog(const TabMode tabMode,
                                const bool browserWindowAspectLocked,
                                const int32_t tabAspectLockedCount,
                                const int32_t tabCount,
                                QWidget* parent = 0);
        
        virtual ~LockAspectWarningDialog();
        
        Result getResult() const;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void lockWindowAndTabsClicked();
        
        void lockWindowClicked();
        
        void noChangesClicked();
        
        void cancelClicked();
        
    private:
        LockAspectWarningDialog(const LockAspectWarningDialog&);

        LockAspectWarningDialog& operator=(const LockAspectWarningDialog&);
        
        QString getLockedStatusText() const;
        
        const TabMode m_tabMode;
        
        const bool m_browserWindowAspectLocked;
        
        const int32_t m_tabAspectLockedCount;
        
        const int32_t m_tabCount;
        
        Result m_result = Result::CANCEL;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LOCK_ASPECT_WARNING_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LOCK_ASPECT_WARNING_DIALOG_DECLARE__

} // namespace
#endif  //__LOCK_ASPECT_WARNING_DIALOG_H__
