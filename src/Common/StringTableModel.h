#ifndef __STRING_TABLE_MODEL_H__
#define __STRING_TABLE_MODEL_H__

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
        
        int32_t getNumberOfRows() const;
        
        int32_t getNumberOfColumns() const;
        
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
