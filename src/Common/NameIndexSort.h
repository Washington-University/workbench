#ifndef __NAME_INDEX_SORT__H_
#define __NAME_INDEX_SORT__H_

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

    class NameIndexSort : public CaretObject {
        
    public:
        NameIndexSort();
        
        virtual ~NameIndexSort();
        
        void add(const int64_t dataIndex,
                 const AString& dataName);
        
        int64_t getNumberOfItems() const;
        
        void getSortedNameAndIndex(const int64_t sortedPositionIndex,
                                   int64_t& dataIndexOut,
                                   AString& dataNameOut) const;
        
        void getSortedNameAndIndex(const int64_t sortedPositionIndex,
                                   int32_t& dataIndexOut,
                                   AString& dataNameOut) const;
        
        int64_t getSortedIndex(const int64_t sortedPositionIndex) const;
        
        void sortByNameCaseSensitive();
        
        void sortByNameCaseInsensitive();
        
    private:
        class NameIndexPair {
        public:
            int64_t indx;
            AString name;
            
            NameIndexPair(const int64_t indx,
                          const AString& name);
            
            ~NameIndexPair();
            
            bool operator<(const NameIndexPair& nip) const;
            
            static bool lessThanCaseInsensitive(const NameIndexPair& nip1,
                                                const NameIndexPair& nip2);        
        };
        
        NameIndexSort(const NameIndexSort&);

        NameIndexSort& operator=(const NameIndexSort&);
        
        std::vector<NameIndexPair> items;
        
    };
    
#ifdef __NAME_INDEX_SORT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NAME_INDEX_SORT_DECLARE__

} // namespace
#endif  //__NAME_INDEX_SORT__H_
