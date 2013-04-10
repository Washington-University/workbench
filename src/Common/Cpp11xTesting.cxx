
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __CPP11X_TESTING_DECLARE__
#include "Cpp11xTesting.h"
#undef __CPP11X_TESTING_DECLARE__

#ifdef WORKBENCH_HAVE_C11X

using namespace caret;

#include <iostream>
#include <memory>
#include <set>
// Not on Intel #include <tuple>
#include <vector>

/**
 * \class caret::Cpp11xTesting 
 * \brief The purpose of this class is to test for c11x compiler support.
 * \ingroup Common
 */

/**
 * Constructor.
 *
 * Tests delegating constructor (calls another constructor within same class)
 * Not supported by Intel compiler
 */
Cpp11xTesting::Cpp11xTesting()
/*: Cp11xTesting(5)*/
{
    
}

/**
 * Constructor.
 */
Cpp11xTesting::Cpp11xTesting(const int /*value*/)
{
    
}

/**
 * Destructor.
 */
Cpp11xTesting::~Cpp11xTesting()
{
    
}

/**
 * Test 'noexcept' keyword indicating no exception is thrown
 * Not supported by Intel Compiler
 */
//void
//Cpp11xTesting::methodName() noexcept()
//{
//    
//}

void
Cpp11xTesting::test()
{
    /*
     * Space no longer needed between consecutive '>'
     */
    std::vector<std::vector<int>> vvi;
    
    /*
     * Initialization list for vector
     * Not supported by Intel Compiler
     */
    std::vector<int> values; // { 3, 8, 5 };
    
    /*
     * Iterator over vector using both 'auto' and the new iterator
     */
    for (auto i : values) {
        std::cout << i << std::endl;
    }
    
    /*
     * 'nullptr' instead of NULL or zero.
     */
    float* fptr = nullptr;
    if (fptr != nullptr) {
        
    }
    
    /*
     * Array initialization, new style
     * Not supported by Intel compiler
     */
    //int a[] { 1, 3, 5 };
    int a[] = { 1, 3, 5 };
    
    /*
     * Iteration over array using both 'auto' and the new iterator
     */
    for (auto i : a) {
        std::cout << i << std::endl;
    }

    /*
     * Tuple
     * Not supported by Intel compiler
     */
    //std::tuple<int, double, double> tp(5, 9.1, 12.5);
    //std::cout << "Tuple 2nd value: " << std::get<1>(tp) << std::endl;
    
    /*
     * Test Lambda
     */
    auto l = [] (const char* s) { std::cout << "Lambda: " << s << std::endl; };
    l("lambda test");

}

/**
 * 'override' keyword would cause an error if superclass did not have methodName()
 * Not supported by Intel compiler
 */
//void
//SubClass:methodName() override noexcept
//{
//    
//}

#endif // WORKBENCH_HAVE_C11X
