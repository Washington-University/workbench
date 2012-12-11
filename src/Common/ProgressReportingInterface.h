#ifndef __PROGRESS_REPORTING_INTERFACE_H__
#define __PROGRESS_REPORTING_INTERFACE_H__

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
