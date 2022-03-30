#ifndef __EXIT_PROGRAM_MODIFIED_FILES_DIALOG_H__
#define __EXIT_PROGRAM_MODIFIED_FILES_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

class QAbstractButton;
class QDialogButtonBox;

namespace caret {

    class ExitProgramModifiedFilesDialog : public QDialog {
        
        Q_OBJECT

    public:
        /**
         * Result from dialog
         */
        enum class Result {
            /* Invalid result */
            INVALID,
            /* Cancel exit of program */
            CANCEL_EXIT,
            /* discard modified files and exit program */
            DISCARD_AND_EXIT,
            /* save modified files and exit program */
            SAVE_AND_EXIT
        };
        
        ExitProgramModifiedFilesDialog(const QString& modifiedFilesText,
                                       QWidget* parent = 0);
        
        virtual ~ExitProgramModifiedFilesDialog();
        
        ExitProgramModifiedFilesDialog(const ExitProgramModifiedFilesDialog&) = delete;

        ExitProgramModifiedFilesDialog& operator=(const ExitProgramModifiedFilesDialog&) = delete;
        
        Result getResult() const;

        // ADD_NEW_METHODS_HERE

        QSize sizeHint() const override;
        
    protected:

    private slots:
        void buttonClicked(QAbstractButton *button);
        
    private:
        QDialogButtonBox* m_buttonBox;
        
        QAbstractButton* m_cancelButton;
        
        QAbstractButton* m_discardButton;
        
        QAbstractButton* m_saveButton;

        Result m_result = Result::INVALID;
        
        bool m_haveScrollAreaFlag = false;
        
        static constexpr int32_t s_scrollWidth  = 700;
        static constexpr int32_t s_scrollHeight = 400;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EXIT_PROGRAM_MODIFIED_FILES_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EXIT_PROGRAM_MODIFIED_FILES_DIALOG_DECLARE__

} // namespace
#endif  //__EXIT_PROGRAM_MODIFIED_FILES_DIALOG_H__
