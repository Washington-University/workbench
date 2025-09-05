#ifndef __SYSTEM_BACKTRACE_H__
#define __SYSTEM_BACKTRACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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
#include <vector>

#include "AString.h"

namespace caret {

    class SystemBacktrace  {
        
    public:
        SystemBacktrace();
        
        virtual ~SystemBacktrace();
        
        AString toSymbolString() const;

    private:
#ifdef CARET_OS_WINDOWS
#else // CARET_OS_WINDOWS
        std::vector<void*> m_callstack;
        int m_numFrames;
#endif // CARET_OS_WINDOWS

    friend class SystemUtilities;
    };
    
#ifdef __SYSTEM_BACKTRACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SYSTEM_BACKTRACE_DECLARE__

} // namespace
#endif  //__SYSTEM_BACKTRACE_H__
