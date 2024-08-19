#ifndef __FUNCTION_RESULT_VALUE_H__
#define __FUNCTION_RESULT_VALUE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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



#include <cinttypes>
#include <memory>

#include "CaretObject.h"


namespace caret {

    /**
     * \class caret::FunctionResult
     * \brief Result from function based upon tensorstore's Result template
     * \ingroup Common
     * Returns an object containing an error message, and error status
     */
    class FunctionResult : public CaretObject {
        
    public:
        /**
         * @return An instance with OK result
         */
        static FunctionResult ok() {
            return FunctionResult("",
                                  true);
        }
        
        /**
         * @return An instance with an error result
         * @param errorMessage
         *    The error message
         */
        static FunctionResult error(const AString& errorMessage) {
            return FunctionResult(errorMessage,
                                  false);
        }
        
        /**
         * Constructor
         * @param errorMessage
         *   The error message (should be empty if okFlag == true)
         * @param okFlag
         *   True if no error, else false.
         */
        FunctionResult(const AString& errorMessage,
                       const bool okFlag)
        : CaretObject(),
        m_errorMessage(errorMessage),
        m_okFlag(okFlag)
        { }
        
        /**
         * Destructor
         */
        virtual ~FunctionResult() { }
        
        /**
         * Copy constructor
         * @param obj
         *    Object that is copied
         */
        FunctionResult(const FunctionResult& obj)
        : CaretObject(obj)
        {
            this->copyHelperFunctionResult(obj);
        }
        
        /**
         * Assignment operator
         * @param obj
         *    Object that is assigned (copied) to this
         */
        FunctionResult& operator=(const FunctionResult& obj)
        {
            if (this != &obj) {
                CaretObject::operator=(obj);
                this->copyHelperFunctionResult(obj);
            }
            return *this;
        }
        
        /**
         * @return The error message.
         */
        AString getErrorMessage() const { return m_errorMessage; }
        
        /**
         * @return True if result is OK (success)
         */
        bool isOk() const { return m_okFlag; }
        
        /**
         * @return True if there is an error (see error message)
         */
        bool isError() const { return ( ! m_okFlag); }
        
        // ADD_NEW_METHODS_HERE
        
    private:
        /**
         * Helps with copying instance
         * @param obj
         *    Instance that is copied to 'this'
         */
        void copyHelperFunctionResult(const FunctionResult& obj)
        {
            m_errorMessage  = obj.m_errorMessage;
            m_okFlag        = obj.m_okFlag;
        }
        
        AString m_errorMessage;
        
        bool m_okFlag;
        
        // ADD_NEW_MEMBERS_HERE
        
    };

    /**
     * \class caret::FunctionResultValue
     * \brief Result from function based upon tensorstore's Result template
     * \ingroup Common
     * Returns an object containing a return value, error message, and error status
     */
    template <class T> class FunctionResultValue : public FunctionResult {
        
    public:
        /**
         * Constructor
         * @param value
         *    Value that is being returned
         * @param errorMessage
         *   The error message (should be empty if okFlag == true)
         * @param okFlag
         *   True if no error, else false.
         */
        FunctionResultValue(const T& value,
                            const AString& errorMessage,
                            const bool okFlag)
        :  FunctionResult(errorMessage,
                          okFlag),
        m_value(value)
        { }
        
        virtual ~FunctionResultValue() { }
        
        /**
         * Copy constructor
         * @param obj
         *    Object that is copied
         */
        FunctionResultValue(const FunctionResultValue& obj)
        : FunctionResult(obj)
        {
            this->copyHelperFunctionResultValue(obj);
        }

        /**
         * Assignment operator
         * @param obj
         *    Object that is assigned (copied) to this
         */
        FunctionResultValue& operator=(const FunctionResultValue& obj)
        {
            if (this != &obj) {
                FunctionResult::operator=(obj);
                this->copyHelperFunctionResultValue(obj);
            }
            return *this;
        }

        /**
         * @return The value
         */
        const T& getValue() const { return m_value; }
        
        // ADD_NEW_METHODS_HERE

    private:
        /**
         * Helps with copying instance
         * @param obj
         *    Instance that is copied to 'this'
         */
        void copyHelperFunctionResultValue(const FunctionResultValue& obj)
        {
            m_value         = obj.m_value;
        }

        T m_value;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
    typedef FunctionResultValue<int32_t> FunctionResultInt32;
    
    typedef FunctionResultValue<AString> FunctionResultString;
    
    typedef FunctionResultValue<float> FunctionResultFloat;
    
#ifdef __FUNCTION_RESULT_VALUE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FUNCTION_RESULT_VALUE_DECLARE__

} // namespace
#endif  //__FUNCTION_RESULT_VALUE_H__
