
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __CARET_RESULT_DECLARE__
#include "CaretResult.h"
#undef __CARET_RESULT_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::CaretResult 
 * \brief Contains an error status and an error description that are returned from functions/methods.
 * \ingroup Common
 *
 * This class helps with returning an error status and description from functions/methods.
 * For new instances, it is best to use the static method to create the instance.  These static
 * methods create a unique pointer for the instance that can prevent copy operations and
 * will destroy the instance.
 *
 * The design is loosely based upon Qt's Q*Error classes.
 */

/**
 * @return A unique pointer that points to a new instance of CaretResult
 * that indicates success (NO_ERROR)
 */
std::unique_ptr<CaretResult>
CaretResult::newInstanceSuccess()
{
    std::unique_ptr<CaretResult> ptr(new CaretResult());
    return ptr;
}

/**
 * @return New instance containing  error status code GENERIC_ERROR and description
 * @param errorDescription
 *    The description of the error
 */

std::unique_ptr<CaretResult>
CaretResult::newInstanceError(const AString& errorDescription)
{
    std::unique_ptr<CaretResult> ptr(new CaretResult(ErrorStatusCode::GENERIC_ERROR,
                                                     errorDescription));
    return ptr;
}

/**
 * @return New instance containing the given error status code and description
 * @param errorStatusCode
 *    The error status
 * @param errorDescription
 *    The description of the error
 */

std::unique_ptr<CaretResult>
CaretResult::newInstanceError(const ErrorStatusCode errorStatusCode,
                              const AString& errorDescription)
{
    std::unique_ptr<CaretResult> ptr(new CaretResult(errorStatusCode,
                                                     errorDescription));
    return ptr;
}

/**
 * Constructor that creates instance with a NO_ERROR status code (indicates success).
 */
CaretResult::CaretResult()
: CaretObject(),
m_errorStatusCode(NO_ERROR)
{
}

/**
 * Constructor that creates instance with the given error status code and description
 * @param errorStatusCode
 *    The error status
 * @param errorDescription
 *    The description of the error
 */
CaretResult::CaretResult(const ErrorStatusCode errorStatusCode,
                         const AString& errorDescription)
: CaretObject(),
m_errorStatusCode(errorStatusCode),
m_errorDescription(errorDescription)
{
}

/**
 * Destructor.
 */
CaretResult::~CaretResult()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CaretResult::CaretResult(const CaretResult& obj)
: CaretObject(obj)
{
    this->copyHelperCaretResult(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CaretResult&
CaretResult::operator=(const CaretResult& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCaretResult(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CaretResult::copyHelperCaretResult(const CaretResult& obj)
{
    m_errorStatusCode  = obj.m_errorStatusCode;
    m_errorDescription = obj.m_errorDescription;
}

/**
 * Convert an error status code to its name
 * @param errorStatusCode
 *    The error status code
 * @return Name of an error status code
 */
AString
CaretResult::errorStatusCodeToName(const ErrorStatusCode errorStatusCode)
{
    AString name;
    switch (errorStatusCode) {
        case GENERIC_ERROR:
            name = "Generic_Error";
            break;
        case NO_ERROR:
            name = "No_Error";
            break;
    }
    return name;
}

/**
 * @param The name of the error status code
 */
AString
CaretResult::getErrorStatusCodeName() const
{
    return errorStatusCodeToName(m_errorStatusCode);
}

/**
 * @return The error status code
 */
CaretResult::ErrorStatusCode
CaretResult::getErrorStatusCode() const
{
    return m_errorStatusCode;
}

/**
 * @return The error description
 */
AString
CaretResult::getErrorDescription() const
{
    return m_errorDescription;
}

/**
 * @return True if there is an error (the error status code is anything but NO_ERROR).
 */
bool
CaretResult::isError() const{
    return (m_errorStatusCode != ErrorStatusCode::NO_ERROR);
}

/**
 * @return True if successful (error status code is NO_ERROR)
 */
bool
CaretResult::isSuccess() const{
    return (m_errorStatusCode == ErrorStatusCode::NO_ERROR);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CaretResult::toString() const
{
    return ("Error status code: "
            + getErrorStatusCodeName()
            + " Description: "
            + getErrorDescription());
}

