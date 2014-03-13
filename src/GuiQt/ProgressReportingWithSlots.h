#ifndef __PROGRESS_REPORTING_WITH_SLOTS_H__
#define __PROGRESS_REPORTING_WITH_SLOTS_H__

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
