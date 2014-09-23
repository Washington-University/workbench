#ifndef __WU_Q_EVENT_BLOCKING_FILTER__H_
#define __WU_Q_EVENT_BLOCKING_FILTER__H_

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

#include <QEvent>
#include <QHash>
#include <QObject>

class QComboBox;

namespace caret {

    class WuQEventBlockingFilter : public QObject {
        
        Q_OBJECT

    public:
        static void blockMouseWheelEventInMacComboBox(QComboBox* comboBox);
        
        WuQEventBlockingFilter(QObject* parent);
        
        virtual ~WuQEventBlockingFilter();
        
        void setEventBlocked(const QEvent::Type eventType,
                             const bool blockedStatus);
        
        bool isEventBlocked(const QEvent::Type eventType) const;
        
    protected:
        bool eventFilter(QObject* object,
                         QEvent* event);
        
    private:
        WuQEventBlockingFilter(const WuQEventBlockingFilter&);

        WuQEventBlockingFilter& operator=(const WuQEventBlockingFilter&);

        /** 
         * Hash Table is probably fastest way to track multiple events
         * that can be blocked.  Key is int (QEvent::Type) and value
         * is blocked status.
         */
        QHash<int, bool> blockedEventsHashTable;
    };
    
#ifdef __WU_Q_EVENT_BLOCKING_FILTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_EVENT_BLOCKING_FILTER_DECLARE__

} // namespace
#endif  //__WU_Q_EVENT_BLOCKING_FILTER__H_
