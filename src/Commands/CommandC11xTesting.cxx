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

#include <iostream>
#include <memory>
#include <set>
// Not on Intel #include <tuple>
#include <vector>

#include "CaretAssert.h"
#include "CommandC11xTesting.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandC11xTesting::CommandC11xTesting()
: CommandOperation("-c11x-test",
                   "C11X Compiler Compatibility Testing")
{
    
}

/**
 * Destructor.
 */
CommandC11xTesting::~CommandC11xTesting()
{
    
}

AString 
CommandC11xTesting::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Test for C11x support in compilers.\n"
                        "\n"
                        "Usage:  <anything> \n"
                        "    \n"
                        );
    return helpInfo; 
}

/**
 * Execute the operation.
 * 
 * @param parameters
 *   Parameters for the operation.
 * @throws CommandException
 *   If the command failed.
 * @throws ProgramParametersException
 *   If there is an error in the parameters.
 */
void 
CommandC11xTesting::executeOperation(ProgramParameters& /*parameters*/)
{
#ifdef WORKBENCH_HAVE_C11X
    Cpp11xTesting cppx;
    cppx.test();
#endif // WORKBENCH_HAVE_C11X
}










#ifdef WORKBENCH_HAVE_C11X
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
    std::cout << "Initialized value of X: " << m_x << std::endl;
    std::cout << std::endl;
    
    /*
     * Space no longer needed between consecutive '>'
     */
    std::vector<std::vector<int>> vvi;
    
    /*
     * Initialization list for vector
     * Not supported by Intel Compiler
     */
    std::vector<int> values; // { 3, 8, 5 };
    values.push_back(3);
    values.push_back(8);
    values.push_back(5);
    
    /*
     * Iterator over vector using both 'auto' and the new iterator
     */
    std::cout << "Vector auto and new iterator: ";
    for (auto i : values) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    /*
     * 'nullptr' instead of NULL or zero.
     */
    float* fptr = nullptr;
    if (fptr == nullptr) {
        std::cout << "nullptr works" << std::endl;

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
    std::cout << "Array auto and new iterator: ";
    for (auto i : a) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
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

