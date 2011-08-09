#ifndef __CARET_ASSERTION_H__
#define __CARET_ASSERTION_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <iosfwd>
#include <stdint.h>

namespace caret {
    
/**
 * Contains static methods for assertion processing.
 * 
 * DO NOT USE THIS CLASS!!!!
 
 * USE THE MACROS DEFINED IN CaretAssert.h
 */
class CaretAssertion {
private:
    CaretAssertion();

    CaretAssertion(const CaretAssertion& o);
    
    CaretAssertion& operator=(const CaretAssertion&);
    
    ~CaretAssertion();

public:
    static void assertFailed(const char* expression,
                             const char* filename,
                             const int64_t lineNumber);
    
    static void assertFailed(const char* expression,
                             const char* message,
                             const char* filename,
                             const int64_t lineNumber);
    
    static void assertArrayIndexFailed(const char* arrayName,
                                       const int64_t arrayNumberOfElements,
                                       const int64_t arrayIndex,
                                       const char* filename,
                                       const int64_t lineNumber);
    
    static void assertVectorIndexFailed(const char* vectorName,
                                        const int64_t vectorNumberOfElements,
                                        const int64_t vectorIndex,
                                        const char* filename,
                                        const int64_t lineNumber);
    
    static void unitTest(std::ostream& stream,
                         const bool isVerbose);
    
private:
    static void unitTestHelper(std::ostream& stream,
                               const std::string& testOutput,
                               const std::string& testName,
                               const bool correctTestStatus,
                               const bool isVerbose);
    
    static bool unitTestFlag;
};

#ifdef __CARET_ASSERTION_DEFINE__
    bool CaretAssertion::unitTestFlag = false;
#endif // __CARET_ASSERTION_DEFINE__
    
} // namespace

#endif // __CARET_ASSERTION_H__
