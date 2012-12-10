#ifndef __PROGRESS_REPORTING_DIALOG_H__
#define __PROGRESS_REPORTING_DIALOG_H__

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
#include <QProgressDialog>

#include "EventListenerInterface.h"

namespace caret {

    class ProgressReportingWithSlots;
    
    class ProgressReportingDialog : public QProgressDialog {
        
        Q_OBJECT

    public:
        static void runEvent(Event* event,
                             QWidget* parent,
                             const QString& title);
        
    private:
        ProgressReportingDialog(const QString& initialMessage,
                                QWidget* parent);

    public:
        virtual ~ProgressReportingDialog();

    private:
        ProgressReportingDialog(const ProgressReportingDialog&);

        ProgressReportingDialog& operator=(const ProgressReportingDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE

        ProgressReportingWithSlots* m_progressReporter;
    };
    
    class ProgressReportingWithSlots : public QObject, public EventListenerInterface {
        Q_OBJECT
        
    public:
        ProgressReportingWithSlots(QObject* parent);
        
        virtual ~ProgressReportingWithSlots();
        
        void receiveEvent(Event* event);
        
        void setProgressRange(const int minimumProgress,
                                      const int maximumProgress);
        
        void setProgressValue(const int progressValue);
        
        void setProgressMessage(const QString& progressMessage);
        
        //bool isCancelled() const;
        
    public slots:
        void requestCancel();
        
    signals:
        void reportProgressRange(const int minimumProgress,
                                 const int maximumProgress);
        
        void reportProgressValue(const int progressValue);
        
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
    
#ifdef __PROGRESS_REPORTING_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PROGRESS_REPORTING_DIALOG_DECLARE__

} // namespace
#endif  //__PROGRESS_REPORTING_DIALOG_H__
