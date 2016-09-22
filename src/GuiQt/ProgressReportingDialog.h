#ifndef __PROGRESS_REPORTING_DIALOG_H__
#define __PROGRESS_REPORTING_DIALOG_H__

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

#include <QProgressDialog>

#include "AString.h"
#include "EventListenerInterface.h"

namespace caret {

    class ProgressReportingInterface;
    
    class ProgressReportingDialog : public QProgressDialog {
        
        Q_OBJECT

    public:
        ProgressReportingDialog(const AString& title,
                                const AString& initialMessage,
                                QWidget* parent,
                                Qt::WindowFlags f = 0);

        static void runEvent(Event* event,
                             QWidget* parent,
                             const AString& title);
        
    public:
        virtual ~ProgressReportingDialog();

    private:
        ProgressReportingDialog(const ProgressReportingDialog&);

        ProgressReportingDialog& operator=(const ProgressReportingDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

        ProgressReportingInterface* m_progressReporter;
    };

#ifdef __PROGRESS_REPORTING_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PROGRESS_REPORTING_DIALOG_DECLARE__

} // namespace
#endif  //__PROGRESS_REPORTING_DIALOG_H__
