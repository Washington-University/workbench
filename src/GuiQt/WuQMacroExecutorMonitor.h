#ifndef __WU_Q_MACRO_EXECUTOR_MONITOR_H__
#define __WU_Q_MACRO_EXECUTOR_MONITOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include <QMutex>
#include <QObject>

namespace caret {

    class WuQMacroExecutorMonitor : public QObject {
        Q_OBJECT

    public:
        enum class Mode {
            RUN,
            PAUSE,
            STOP
        };
        
        WuQMacroExecutorMonitor(QObject* parent);
        
        virtual ~WuQMacroExecutorMonitor();
        
        WuQMacroExecutorMonitor(const WuQMacroExecutorMonitor&) = delete;

        WuQMacroExecutorMonitor& operator=(const WuQMacroExecutorMonitor&) = delete;

        QString getStoppedByUserMessage() const;
        
        Mode getMode() const;
        
        bool testForStop() const;
        
        void setMode(const Mode mode);
        
        bool doPause() const;
        
        mutable Mode m_mode = Mode::STOP;
        
        mutable QMutex m_modeMutex;

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

        friend class WuQMacroExecutor;
        friend class WuQMacroManager;
    };
    
#ifdef __WU_Q_MACRO_EXECUTOR_MONITOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_EXECUTOR_MONITOR_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_EXECUTOR_MONITOR_H__
