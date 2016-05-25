#ifndef __PROGRESS_REPORTING_BAR_H__
#define __PROGRESS_REPORTING_BAR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include <QWidget>

class QLabel;
class QProgressBar;

namespace caret {

    class ProgressReportingFromEvent;
    
    class ProgressReportingBar : public QWidget {
        
        Q_OBJECT

    public:
        ProgressReportingBar(QWidget* parent = 0);
        
        virtual ~ProgressReportingBar();
        
        void setEnabledForUpdates(const bool enabledForUpdates);
        
        void reset();

        void setMessage(const QString& text);
        
        // ADD_NEW_METHODS_HERE

    private:
        ProgressReportingBar(const ProgressReportingBar&);

        ProgressReportingBar& operator=(const ProgressReportingBar&);
        
        QProgressBar* m_progressBar;
        
        QLabel* m_messageLabel;
        
        ProgressReportingFromEvent* m_progressFromEvent;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PROGRESS_REPORTING_BAR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PROGRESS_REPORTING_BAR_DECLARE__

} // namespace
#endif  //__PROGRESS_REPORTING_BAR_H__
