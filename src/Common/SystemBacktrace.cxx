
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

#define __SYSTEM_BACKTRACE_DECLARE__
#include "SystemBacktrace.h"
#undef __SYSTEM_BACKTRACE_DECLARE__

#include <sstream>

#ifndef CARET_OS_WINDOWS
#include <execinfo.h>
#endif

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SystemBacktrace 
 * \brief System backtrace for finding memory leaked classes
 * \ingroup Common
 */

/**
 * Constructor.
 */
SystemBacktrace::SystemBacktrace()
{
#ifndef CARET_OS_WINDOWS
    m_numFrames = 0;
#endif
}

/**
 * Destructor.
 */
SystemBacktrace::~SystemBacktrace()
{
}

/**
 * @return String containing the backtrace symbols.
 */
AString SystemBacktrace::toSymbolString() const
{
    std::stringstream str;
#ifdef CARET_OS_WINDOWS
#else  // CARET_OS_WINDOWS
    if ( ! m_callstack.empty()) {
        char** symbols = backtrace_symbols(&m_callstack[0], m_numFrames);
        for (int i = 0; i < m_numFrames; ++i)
        {
            str << symbols[i] << std::endl;
        }
        free(symbols);
    }
#endif // CARET_OS_WINDOWS
    return AString::fromStdString(str.str());
}

