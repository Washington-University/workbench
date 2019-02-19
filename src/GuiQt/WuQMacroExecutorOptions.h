#ifndef __WU_Q_MACRO_EXECUTOR_OPTIONS_H__
#define __WU_Q_MACRO_EXECUTOR_OPTIONS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include "CaretObject.h"



namespace caret {

    class WuQMacroExecutorOptions : public CaretObject {
        
    public:
        WuQMacroExecutorOptions();
        
        virtual ~WuQMacroExecutorOptions();
        
        WuQMacroExecutorOptions(const WuQMacroExecutorOptions& obj);

        WuQMacroExecutorOptions& operator=(const WuQMacroExecutorOptions& obj);

        bool isShowMouseMovement() const;
        
        void setShowMouseMovement(const bool status);
        
        bool isStopOnError() const;
        
        void setStopOnError(const bool status);
        
        bool isLooping() const;

        void setLooping(const bool status);

        bool isRecordMovieDuringExecution() const;
        
        void setRecordMovieDuringExecution(const bool status);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperWuQMacroExecutorOptions(const WuQMacroExecutorOptions& obj);

        bool m_showMouseMovementFlag = false;
        
        bool m_stopOnErrorFlag = true;
        
        bool m_loopingFlag = false;

        bool m_recordMovieDuringExecutionFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_EXECUTOR_OPTIONS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_EXECUTOR_OPTIONS_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_EXECUTOR_OPTIONS_H__
