#ifndef __CARET_ASSERT_H__
#define __CARET_ASSERT_H__

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

#include <stdint.h>

#include "CaretAssertion.h"

#ifdef NDEBUG

    #define CaretAssert(e) ((void)0)
    #define CaretAssertToDoWarning() ((void)0)
    #define CaretAssertToDoFatal() ((void)0)
    #define CaretAssertMessage(e, m) ((void)0)
    #define CaretAssertArrayIndex(a, n, i) ((void) 0)
    #define CaretAssertVectorIndex(v, i) ((void) 0)

    #define CaretUsedInDebugCompileOnly(e) ((void) 0)
    #define CaretParameterUsedInDebugCompileOnly(e)
#else // NDEBUG


    /**
     * \def CaretUsedInDebugCompileOnly
     *
     * The code within the expression is compiled ONLY
     * if the compiler's "debug" option is enabled.  It
     * can be used to eliminate "unused" compilation warnings
     * when debug is off for variables that are only
     * used in one of the CaretAssert() macros.
     *
     * Example:
     *    std::vector<int> v;
     *    CaretUsedInDebugCompileOnly(const int index = 5 * x + y);
     *    CaretAssertVectorIndex(v, index);
     *
     * @param e
     *    Expression that is only present if compiled with debug on.
     */
    #define CaretUsedInDebugCompileOnly(e) e

    /**
     * \def CaretParameterUsedInDebugCompileOnly
     *
     * The code within the expression is compiled ONLY
     * if the compiler's "debug" option is enabled. It
     * is used when a function parameter is only used 
     * within a CaretAssert() macro.
     *
     * Example:
     *    void function(const float a,
     *                  const int CaretParameterUsedInDebugCompileOnly(index)) {
     *       CaretAssertVectorIndex(v, index);
     *    }
     *
     * @param e
     *    Expression that is only present if compiled with debug on.
     */
    #define CaretParameterUsedInDebugCompileOnly(e) e

    /**
     * \def CaretAssert
     *
     * If the expression evaluates to zero, a message is printed
     * showing the file its line number.  A call stack may also
     * be printed.   Lastly, abort() is called.
     * @param e
     *    Expression that is tested.
     */
    #define CaretAssert(e) \
         (((e) == 0)  \
             ? caret::CaretAssertion::assertFailed(#e, __FILE__, __LINE__) \
             : (void)0)

    /**
     * \def CaretAssertToDoWarning
     *
     * Its purpose is to add "easy to find" reminders in the code
     * for items that will function without crashing but still
     * need some additional work.  It is expected
     * CaretAssertToDoWarning will rarely, if ever, be in code that
     * is pushed to the main repository.
     *
     * A message is printed showing the file and its line number.  
     * A call stack may also be printed.   Does not call abort().
     */
    #define CaretAssertToDoWarning() \
        caret::CaretAssertion::assertToDoWarning(__FILE__, __LINE__)
                                                
    /**
     * \def CaretAssertToDoFatal
     *
     * Its purpose is to add "easy to find" reminders in the code
     * for items that are unlikely to be encountered and that
     * will likely crash or cause significant errors
     * in the functionality.  It is expected
     * CaretAssertToDoFatal will rarely, if ever, be in code that
     * is pushed to the main repository.
     *
     * A message is printed showing the file and its line number.
     * A call stack may also be printed.   WILL CALL abort()
     * and terminate execution.
     */
    #define CaretAssertToDoFatal() \
       caret::CaretAssertion::assertToDoFatal(__FILE__, __LINE__)

    /**
     * \def CaretAssertMessage
     *
     * If the expression evaluates to zero, a message is printed
     * showing the file its line number.  The users message is
     * then printed.  A call stack may also
     * be printed.   Lastly, abort() is called.
     * @param e
     *    Expression that is tested.
     * @param m 
     *    Message that is printed.
     */
        #define CaretAssertMessage(e, m) \
             (((e) == 0)  \
                 ? caret::CaretAssertion::assertFailed(#e, AString(m), __FILE__, __LINE__) \
                 : (void)0)

/**
 * \def CaretAssertArrayIndex
 *
 * If the array index is out of bounds, a message is printed
 * listing the array name, the arrays number of elements, the
 * invalid array index, the name of the file, and the line
 * number in the file.  A call stack may also
 * be printed.   Lastly, abort() is called.
 * @param a
 *    The array variable.
 * @param n
 *    Number of elements in the array.
 * @param i
 *    The index into the array.
 */
#define CaretAssertArrayIndex(a, n, i) \
    ((((i) < 0) || ((i) >= (n)))   \
        ? caret::CaretAssertion::assertArrayIndexFailed(#a, n, i, __FILE__, __LINE__) \
        : (void)0)

/**
 * \def CaretAssertVectorIndex
 *
 * If the vector index is out of bounds, a message is printed
 * listing the vector name, the vector's number of elements, the
 * invalid vector index, the name of the file, and the line
 * number in the file.  A call stack may also
 * be printed.   Lastly, abort() is called.
 * @param v
 *    The vector.
 * @param i
 *    The index into the vector.
 */
#define CaretAssertVectorIndex(v, i) \
    ((((i) < 0) || ((i) >= (static_cast<int64_t>(v.size())))) \
        ? caret::CaretAssertion::assertVectorIndexFailed(#v, (static_cast<int64_t>(v.size())), i, __FILE__, __LINE__) \
        : (void)0)

#endif // NDEBUG



#endif // __CARET_ASSERT_H__
