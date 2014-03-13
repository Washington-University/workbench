#ifndef __PROGRESS_REPORTING_INTERFACE_H__
#define __PROGRESS_REPORTING_INTERFACE_H__

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



/**
 * \class caret::ProgressReportingInterface
 * \brief Interface for any process that may display progress in the GUI
 *
 * This interface is intended for any task that takes a non-trivial 
 * amount of time to run.  It allows display of the task's progress, 
 * usually in the user-interface's progress dialog and provides feedback 
 * to the user about how much work has been and still needs to be completed.
 * The interface also provides a mechanism for the user to cancel the
 * task and the task to detect that the user has requested the task
 * be cancelled.
 */

#include "AString.h"

namespace caret {

    class ProgressReportingInterface {
        
    public:
        ProgressReportingInterface() { /* nothing */ }
        
        virtual ~ProgressReportingInterface() { /* nothing */ }
        
        /**
         * Used by task to set the range of progress reporting.
         *
         * @param minimumProgress
         *    The minimum amount reported (typically zero).
         * @param maximumProgress
         *    The maximum amount of progress.  
         */
        virtual void setProgressRange(const int minimumProgress,
                                             const int maximumProgress) = 0;
        
        /**
         * Used by task to set the current progress.
         * 
         * @param progress
         *    The current progress within range of the minimum and maximum.
         */
        virtual void setProgressValue(const int progress) = 0;
        
        /**
         * Used by task to set the message describing the task's activity.
         * 
         * @param message
         *    Message that is displayed.
         */
        virtual void setProgressMessage(const AString& message) = 0;
        
        /**
         * Used by the user-interface to request that the task end as
         * soon as possible.
         */
        virtual void setCancelRequested() = 0;
        
        /**
         * @return "true" if a request been made to cancel the task, else false.
         *
         * Used by the task to see if the task should end as soon as 
         * possible.  If so, the task should clean up after itself 
         * (release resources);
         */
        virtual bool isCancelRequested() const = 0;
        
    private:
        ProgressReportingInterface(const ProgressReportingInterface&);

        ProgressReportingInterface& operator=(const ProgressReportingInterface&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PROGRESS_REPORTING_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PROGRESS_REPORTING_INTERFACE_DECLARE__

} // namespace
#endif  //__PROGRESS_REPORTING_INTERFACE_H__
