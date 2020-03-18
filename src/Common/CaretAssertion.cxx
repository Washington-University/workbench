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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define __CARET_ASSERTION_DEFINE__
#include "CaretAssertion.h"
#undef __CARET_ASSERTION_DEFINE__

#include "CaretAssert.h"

#include "SystemUtilities.h"

using namespace caret;

/**
 * Called when an assertion has failed.
 * The following events will occur:
 * Prints the expression that failed, the name of
 * file, and the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  Lastly, abort() is called.
 * 
 *
 * @param expression
 *    Expression that failed assertion testing.
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void 
CaretAssertion::assertFailed(const char* expression,
                             const char* filename,
                             const int64_t lineNumber)
{
    CaretAssertion::assertFailed(expression,
                                 NULL,
                                 filename,
                                 lineNumber);
}

/**
 * Called for a CaretAssertToDoWarning()
 *
 * The following events will occur:
 * Prints the name of
 * file, and the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  DOES NOT call abort.
 *
 *
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void
CaretAssertion::assertToDoWarning(const char* filename,
                                  const int64_t lineNumber)
{
    std::cerr \
    << "CaretAssertToDo WARNING"
    << std::endl
    << "File: "
    << filename
    << std::endl
    << "Line number: "
    << lineNumber
    << std::endl
    << std::endl;

    const AString s = SystemUtilities::getBackTrace();
    if (s.isEmpty() == false) {
        std::cerr
        << s
        << std::endl
        << std::endl;
    }
    
}

/**
 * Called for a CaretAssertToDoFatal()
 *
 * The following events will occur:
 * Prints the name of
 * file, and the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  Does call abort.
 *
 *
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void
CaretAssertion::assertToDoFatal(const char* filename,
                                  const int64_t lineNumber)
{
    std::cerr \
    << "CaretAssertToDo FATAL"
    << std::endl
    << "File: "
    << filename
    << std::endl
    << "Line number: "
    << lineNumber
    << std::endl
    << std::endl;

    const AString s = SystemUtilities::getBackTrace();
    if (s.isEmpty() == false) {
        std::cerr
        << s
        << std::endl
        << std::endl;
    }
    
    if (CaretAssertion::unitTestFlag == false) {
        std::abort();
    }
}

/**
 * Called when an assertion has failed.
 * The following events will occur:
 * Prints the expression that failed, the name of
 * file, and the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  Lastly, abort() is called.
 *
 *
 * @param expression
 *    Expression that failed assertion testing.
 * @param message
 *    Message that is printed.
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void
CaretAssertion::assertFailed(const char* expression,
                             const AString& message,
                             const char* filename,
                             const int64_t lineNumber)
{
    assertFailed(expression,
                 message.toLatin1().constData(),
                 filename,
                 lineNumber);
}

/**
 * Called when an assertion has failed.
 * The following events will occur:
 * Prints the expression that failed, the name of
 * file, and the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  Lastly, abort() is called.
 * 
 *
 * @param expression
 *    Expression that failed assertion testing.
 * @param message
 *    Message that is printed.
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void 
CaretAssertion::assertFailed(const char* expression,
                             const char* message,
                             const char* filename,
                             const int64_t lineNumber)
{
    std::cerr \
    << "Assertion Failure of expression \""
    << expression
    << "\""
    << std::endl
    << "File: "
    << filename
    << std::endl
    << "Line number: "
    << lineNumber
    << std::endl
    << std::endl;
    if (message != NULL) {
        std::cerr
        << "Message: "
        << message 
        << std::endl
        << std::endl;
    }
    
    const AString s = SystemUtilities::getBackTrace();
    if (s.isEmpty() == false) {
        std::cerr 
        << s
        << std::endl
        << std::endl;
    }
    
    if (CaretAssertion::unitTestFlag == false) {
        std::abort();
    }
}

/**
 * Called when an array index assertion has failed.
 * The following events will occur:
 * Prints the name of the array, the number of
 * elements in the array, the invalid array index,
 * the name of the file where the assertion failed,
 * the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  Lastly, abort() is called.
 * 
 *
 * @param arrayName
 *    Name of the array.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param arrayIndex
 *    Invalid array index.
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void 
CaretAssertion::assertArrayIndexFailed(const char* arrayName,
                                     const int64_t arrayNumberOfElements,
                                     const int64_t arrayIndex,
                                     const char* filename,
                                     const int64_t lineNumber)
{
    std::cerr \
    << "Assertion of Array Bounds failed for array: "
    << arrayName
    << std::endl
    << "File: "
    << filename
    << std::endl
    << "Line number: "
    << lineNumber
    << std::endl;
    
    std::cerr 
    << "Index: "
    << arrayIndex
    << std::endl
    << "Number of elements in array: "
    << arrayNumberOfElements
    << std::endl
    << std::endl;
    
    const AString s = SystemUtilities::getBackTrace();
    if (s.isEmpty() == false) {
        std::cerr << s << std::endl << std::endl;
    }
    
    if (CaretAssertion::unitTestFlag == false) {
        std::abort();
    }
    
}

/**
 * Called when an vector index assertion has failed.
 * The following events will occur:
 * Prints the name of the vector, the number of
 * elements in the vector, the invalid vector index,
 * the name of the file where the assertion failed,
 * the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  Lastly, abort() is called.
 * 
 *
 * @param vectorName
 *    Name of the vector.
 * @param vectorNumberOfElements
 *    Number of elements in the vector.
 * @param vectorIndex
 *    Invalid vector index.
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void 
CaretAssertion::assertVectorIndexFailed(const char* vectorName,
                                        const int64_t vectorNumberOfElements,
                                        const int64_t vectorIndex,
                                        const char* filename,
                                        const int64_t lineNumber)
{
    std::cerr \
    << "Assertion of Vector Bounds failed for vector: "
    << vectorName
    << std::endl
    << "File: "
    << filename
    << std::endl
    << "Line number: "
    << lineNumber
    << std::endl;
    
    std::cerr 
    << "Index: "
    << vectorIndex
    << std::endl
    << "Number of elements in vector: "
    << vectorNumberOfElements
    << std::endl
    << std::endl;
    
    const AString s = SystemUtilities::getBackTrace();
    if (s.isEmpty() == false) {
        std::cerr << s << std::endl << std::endl;
    }
    
    if (CaretAssertion::unitTestFlag == false) {
        std::abort();
    }
}

/**
 * Called when a std array index assertion has failed.
 * The following events will occur:
 * Prints the name of the array, the number of
 * elements in the array, the invalid array index,
 * the name of the file where the assertion failed,
 * the line number in the file.  If
 * a callstack (backtrace) is available, it will
 * also be printed.  Lastly, abort() is called.
 *
 *
 * @param arrayName
 *    Name of the array.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param arrayIndex
 *    Invalid array index.
 * @param filename
 *    Name of file in which assertion failed.
 * @param lineNumber
 *    Line number where assertion failed.
 */
void
CaretAssertion::assertStdArrayIndexFailed(const char* arrayName,
                                          const int64_t arrayNumberOfElements,
                                          const int64_t arrayIndex,
                                          const char* filename,
                                          const int64_t lineNumber)
{
    std::cerr \
    << "Assertion of Std Array Bounds failed for array: "
    << arrayName
    << std::endl
    << "File: "
    << filename
    << std::endl
    << "Line number: "
    << lineNumber
    << std::endl;
    
    std::cerr
    << "Index: "
    << arrayIndex
    << std::endl
    << "Number of elements in array: "
    << arrayNumberOfElements
    << std::endl
    << std::endl;
    
    const AString s = SystemUtilities::getBackTrace();
    if (s.isEmpty() == false) {
        std::cerr << s << std::endl << std::endl;
    }
    
    if (CaretAssertion::unitTestFlag == false) {
        std::abort();
    }
}

/**
 * Unit testing of assertions.
 * 
 * @param stream
 *    Stream to which messages are written.
 * @param isVerbose
 *    Print detailed messages.
 */
void 
CaretAssertion::unitTest(std::ostream& stream,
                         const bool isVerbose)
{
#ifdef NDEBUG
    if (isVerbose)
    {
        stream << "Unit testing of CaretAssertion will not take place since software is not compiled with debug on." << std::endl;
    }
    return;
#else
    
    CaretAssertion::unitTestFlag = true;
    stream << "CaretAssertion::unitTest is starting" << std::endl;
    
    /*
     * Redirect std::err to the string stream.
     */
    std::ostringstream str;
    std::streambuf* cerrSave = std::cerr.rdbuf();
    std::cerr.rdbuf(str.rdbuf());
    
    int32_t zero = 0;
    int32_t one  = 1;
    
    
    CaretAssert(zero);
    CaretAssertion::unitTestHelper(stream, "Assert Zero", str.str(), false, isVerbose);
    str.str("");
    
    CaretAssert(one);
    CaretAssertion::unitTestHelper(stream, "Assert One", str.str(), true, isVerbose);
    str.str("");
    
    CaretAssertMessage(zero, "This test should fail along with this message being printed.");
    CaretAssertion::unitTestHelper(stream, "Assert Message Zero", str.str(), false, isVerbose);
    str.str("");

    CaretAssertMessage(one, "This test SHOULD NOT fail.");
    CaretAssertion::unitTestHelper(stream, "Assert Message One", str.str(), true, isVerbose);
    str.str("");
    
    /*int32_t someArray[] = { 1, 2, 3 };
    someArray[1] = 2;//*/
    
    CaretAssertArrayIndex(someArray, 3, -1);
    CaretAssertion::unitTestHelper(stream, "Assert Array Index -1", str.str(), false, isVerbose);
    str.str("");
    
    CaretAssertArrayIndex(someArray, 3, 3);
    CaretAssertion::unitTestHelper(stream, "Assert Array Index 3", str.str(), false, isVerbose);
    str.str("");
    
    CaretAssertArrayIndex(someArray, 3, 5);
    CaretAssertion::unitTestHelper(stream, "Assert Array Index 5", str.str(), false, isVerbose);
    str.str("");
    
    CaretAssertArrayIndex(someArray, 3, 2);
    CaretAssertion::unitTestHelper(stream, "Assert Aray Index 2", str.str(), true, isVerbose);
    str.str("");
    
    std::vector<int32_t> someVector;
    someVector.push_back(1);
    someVector.push_back(2);

    CaretAssertVectorIndex(someVector, 1);
    CaretAssertion::unitTestHelper(stream, "Assert Vector Index 1", str.str(), true, isVerbose);
    str.str("");

    CaretAssertVectorIndex(someVector, -1);  // Yes, do get signed/unsigned warning
    CaretAssertion::unitTestHelper(stream, "Assert Vector Index -1", str.str(), false, isVerbose);
    str.str("");

    CaretAssertVectorIndex(someVector, 2);
    CaretAssertion::unitTestHelper(stream, "Assert Vector Index 2", str.str(), false, isVerbose);
    str.str("");
    
    /*
     * Restore std::err
     */
    std::cerr.rdbuf(cerrSave);
    
    stream << "CaretAssertion::unitTest has ended" << std::endl << std::endl;;
    CaretAssertion::unitTestFlag = false;
#endif
}

void 
CaretAssertion::unitTestHelper(std::ostream& stream,
                               const std::string& testName,                               
                               const std::string& testOutput,
                               const bool correctTestStatus,
                               const bool isVerbose)
{
    /*
     * Should test pass?
     */
    if (correctTestStatus) {
        if (testOutput.empty() == false) {
            stream 
            << "ERROR: CaretAssertion unit test failed but should have passed. "
            << std::endl
            << "Test Name: "
            << testName 
            << std::endl
            << "Test Output: "
            << testOutput
            << std::endl 
            << std::endl;
        }
        else if (isVerbose) {
            stream 
            << "Test "
            << testName
            << " functioned correclty (passed)."
            << std::endl;
        }
    }
    else {
        if (testOutput.empty()) {
            stream 
            << "ERROR: CaretAssertion unit test passed but should have failed." 
            << std::endl
            << "Test Name: "
            << testName 
            << std::endl 
            << std::endl;   
        }
        else if (isVerbose) {
            stream 
            << "Test "
            << testName
            << " functioned correctly (failed)."
            << std::endl;
        }
    }
}


