#ifndef __CARET_RESULT_H__
#define __CARET_RESULT_H__

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



#include <memory>

#include "CaretObject.h"



namespace caret {

    class CaretResult : public CaretObject {
        
    public:
        /**
         * Enumerated type for error status codes
         * Additional error codes can be added but they must designate an error.
         */
        enum ErrorStatusCode {
            /* No Error, the only error code that indicates no error */
            NO_ERROR,
            /* Generic error */
            GENERIC_ERROR
        };
        
        static std::unique_ptr<CaretResult> newInstanceSuccess();
        
        static std::unique_ptr<CaretResult> newInstanceError(const AString& errorDescription);
        
        static std::unique_ptr<CaretResult> newInstanceError(const ErrorStatusCode errorStatusCode,
                                                             const AString& errorDescription);
        
        CaretResult();
        
        CaretResult(const ErrorStatusCode errorStatusCode,
                    const AString& errorDescription);
        
        virtual ~CaretResult();
        
        CaretResult(const CaretResult& obj);

        CaretResult& operator=(const CaretResult& obj);

        static AString errorStatusCodeToName(const ErrorStatusCode errorStatusCode);
        
        AString getErrorStatusCodeName() const;
        
        ErrorStatusCode getErrorStatusCode() const;
        
        AString getErrorDescription() const;

        bool isError() const;
        
        bool isSuccess() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperCaretResult(const CaretResult& obj);

        ErrorStatusCode m_errorStatusCode;
        
        AString m_errorDescription;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_RESULT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_RESULT_DECLARE__

} // namespace
#endif  //__CARET_RESULT_H__
