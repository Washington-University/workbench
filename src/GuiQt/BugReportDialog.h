#ifndef __BUG_REPORT_DIALOG_H__
#define __BUG_REPORT_DIALOG_H__

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


#include "WuQDialogNonModal.h"

class QTextEdit;

namespace caret {

    class BugReportDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        BugReportDialog(QWidget* parent,
                        const AString& openGLInformation);
        
        virtual ~BugReportDialog();
        
        virtual void updateDialog();
        
    private:
        BugReportDialog(const BugReportDialog&);

        BugReportDialog& operator=(const BugReportDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private slots:
        void copyToClipboard();
        
        void copyToEmail();
        
        void openUploadWebsite();
        
    private:        
        QTextEdit* m_textEdit;
        
        QString m_emailAddressURL;
        
        QString m_uploadWebSite;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BUG_REPORT_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BUG_REPORT_DIALOG_DECLARE__

} // namespace
#endif  //__BUG_REPORT_DIALOG_H__
