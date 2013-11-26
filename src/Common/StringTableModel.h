#ifndef __STRING_TABLE_MODEL_H__
#define __STRING_TABLE_MODEL_H__

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


#include "CaretObject.h"

namespace caret {

    class StringTableModel : public CaretObject {
        
    public:
        enum Alignment {
            ALIGN_LEFT,
            ALIGN_RIGHT
        };
        
        StringTableModel(const int32_t numberOfRows,
                         const int32_t numberOfColumns,
                         const int32_t floatingPointPrecision = 3);
        
        virtual ~StringTableModel();
        
        void setElement(const int32_t row,
                        const int32_t column,
                        const AString& value);
        
        void setElement(const int32_t row,
                        const int32_t column,
                        const char* value);
        
        void setElement(const int32_t row,
                        const int32_t column,
                        const int32_t value);
        
        void setElement(const int32_t row,
                        const int32_t column,
                        const int64_t value);
        
        void setElement(const int32_t row,
                        const int32_t column,
                        const double value);
        
        void setElement(const int32_t row,
                        const int32_t column,
                        const bool value);
        
        void setColumnAlignment(const int32_t column,
                                const Alignment alignment);
        
        AString getInString() const;
        
    private:
        StringTableModel(const StringTableModel&);

        StringTableModel& operator=(const StringTableModel&);

        int32_t getOffset(const int32_t row,
                          const int32_t column) const;
        
        const int32_t m_numberOfRows;
        
        const int32_t m_numberOfColumns;
        
        const int32_t m_floatingPointPrecsion;
        
        std::vector<AString> m_stringTable;
        
        std::vector<Alignment> m_columnAlignment;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __STRING_TABLE_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __STRING_TABLE_MODEL_DECLARE__

} // namespace
#endif  //__STRING_TABLE_MODEL_H__
