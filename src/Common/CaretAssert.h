#ifndef __CARET_ASSERT_H__
#define __CARET_ASSERT_H__

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

#include <stdint.h>

#include "CaretAssertion.h"

#ifdef NDEBUG

    #define CaretAssert(e) ((void)0)
    #define CaretAssertMessage(e, m) ((void)0)
    #define CaretAssertArrayIndex(a, n, i) ((void 0)

#else // NDEBUG

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
             ? caret::CaretAssertion::assertFailed(#e, m, __FILE__, __LINE__) \
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
