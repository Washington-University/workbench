#ifndef __PROGRESS_REPORTING_WITH_SLOTS_H__
#define __PROGRESS_REPORTING_WITH_SLOTS_H__

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

#include <QMutex>
#include <QObject>

#include "ProgressReportingInterface.h"

namespace caret {

    class ProgressReportingWithSlots : public QObject, public ProgressReportingInterface {
        Q_OBJECT
        
    public:
        ProgressReportingWithSlots(QObject* parent);
        
        virtual ~ProgressReportingWithSlots();
        
        virtual void setProgressRange(const int minimumProgress,
                              const int maximumProgress);
        
        virtual void setProgressValue(const int progressValue);
        
        virtual void setProgressMessage(const AString& message);
        
        virtual bool isCancelRequested() const;
        
        virtual void setCancelRequested();
        
    public slots:
        void requestCancel();
        
    signals:
        /*
         * Emitted when the range of progress is updated.
         * @param minimumProgress
         *    New value for minimum.
         * @param maximumProgress
         *    New value for maximum.
         */
        void reportProgressRange(const int minimumProgress,
                                 const int maximumProgress);
        
        /**
         * Emitted when the progress value is updated.
         * @param progressValue
         *    New value for progress.
         */
        void reportProgressValue(const int progressValue);
        
        /**
         * Emitted when the progress message is updated.
         * @param progressMessage
         *    New value for progress message.
         *
         * NOTE: This must use a QString (not AString) since
         * it connects to a Qt slot expecting a QString
         */
        void reportProgressMessage(const QString& progressMessage);
        
    private:
        /**
         * Ensures each method is synchronized meaning that each of
         * the methods for updating complete blocks if any other
         * methods are in progress.
         */
        mutable QMutex m_synchronizeMutex;
        
        bool m_cancelled;
    };
#ifdef __PROGRESS_REPORTING_WITH_SLOTS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PROGRESS_REPORTING_WITH_SLOTS_DECLARE__

} // namespace
#endif  //__PROGRESS_REPORTING_WITH_SLOTS_H__
