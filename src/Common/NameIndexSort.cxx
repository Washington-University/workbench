
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

#include <algorithm>

#define __NAME_INDEX_SORT_DECLARE__
#include "NameIndexSort.h"
#undef __NAME_INDEX_SORT_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::NameIndexSort 
 * \brief A utility class that sorts pairs of indices and names
 */

/**
 * Constructor.
 */
NameIndexSort::NameIndexSort()
: CaretObject()
{
}

/**
 * Destructor.
 */
NameIndexSort::~NameIndexSort()
{
}

/**
 * @param dataIndex
 *    
 * @param dataName
 *    
 */
void 
NameIndexSort::add(const int64_t dataIndex,
                   const AString& dataName)
{
    this->items.push_back(NameIndexPair(dataIndex, dataName));
}

/**
 * @return The number of items that have
 * been added by the user.
 */
int64_t 
NameIndexSort::getNumberOfItems() const
{
    return this->items.size();
}

/**
 * AFTER sorting, this method will return the index and data
 * for the item a the given sorted position.
 *
 * @param sortedPositionIndex
 *    Index at the after sorted position.
 * @param dataIndexOut
 *    Index of data the the sorted position.
 * @param dataNameOut
 *    Name of data at the sorted position.
 */
void 
NameIndexSort::getSortedNameAndIndex(const int64_t sortedPositionIndex,
                                     int64_t& dataIndexOut,
                                     AString& dataNameOut) const
{
    CaretAssertVectorIndex(this->items, sortedPositionIndex);
    
    dataIndexOut = this->items[sortedPositionIndex].indx;
    dataNameOut  = this->items[sortedPositionIndex].name;
}

/**
 * AFTER sorting, this method will return the index and data
 * for the item a the given sorted position.
 *
 * @param sortedPositionIndex
 *    Index at the after sorted position.
 * @param dataIndexOut
 *    Index of data the the sorted position.
 * @param dataNameOut
 *    Name of data at the sorted position.
 */
void 
NameIndexSort::getSortedNameAndIndex(const int64_t sortedPositionIndex,
                                     int32_t& dataIndexOut,
                                     AString& dataNameOut) const
{
    CaretAssertVectorIndex(this->items, sortedPositionIndex);
    
    dataIndexOut = this->items[sortedPositionIndex].indx;
    dataNameOut  = this->items[sortedPositionIndex].name;
}

/**
 * AFTER sorting, this method will return the index
 * for the item a the given sorted position.
 *
 * @param sortedPositionIndex
 *    Index at the after sorted position.
 * @return 
 *    Index of data the the sorted position.
 */
int64_t 
NameIndexSort::getSortedIndex(const int64_t sortedPositionIndex) const
{
    CaretAssertVectorIndex(this->items, sortedPositionIndex);
    
    return this->items[sortedPositionIndex].indx;
}

/**
 * Sort the items by name with case SENSITIVE comparisons.  
 */
void 
NameIndexSort::sortByNameCaseSensitive()
{
    std::sort(this->items.begin(), 
              this->items.end());
}

/**
 * Sort the items by name with case INSENSITIVE comparisons.  
 */
void 
NameIndexSort::sortByNameCaseInsensitive()
{
    std::sort(this->items.begin(), 
              this->items.end(), 
              NameIndexPair::lessThanCaseInsensitive);
}


//=================================================================

/**
 * Constructor.
 *
 * @param indx
 *    Index of item.
 * @param name
 *    Name of item.
 */
NameIndexSort::NameIndexPair::NameIndexPair(const int64_t indx,
              const AString& name)
{
    this->indx = indx;
    this->name = name;
}

/**
 * Destructor.
 */
NameIndexSort::NameIndexPair::~NameIndexPair()
{
    
}

/**
 * Comparison operator.
 *
 * @param nip
 *     Other instance for comparison.
 * @return
 *     true if this instance is 'less than' the given instance,
 *     else false.
 */
bool 
NameIndexSort::NameIndexPair::operator<(const NameIndexPair& nip) const
{
    if (this->name == nip.name) {
        return (this->indx < nip.indx);
    }
    return (this->name < nip.name);    
}

/**
 * Case insenstive comparison.
 * @param nip1
 *     First instance for comparison.
 * @param nip2
 *     Second instance for comparison.
 * @return
 *     true if first instance is less than second instance.
 */
bool 
NameIndexSort::NameIndexPair::lessThanCaseInsensitive(const NameIndexPair& nip1,
                                    const NameIndexPair& nip2)
{
    const int result = QString::compare(nip1.name, nip2.name, Qt::CaseInsensitive);  
    if (result < 0) {
        return true;
    }
    else if (result > 0) {
        return false;
    }
    
    return (nip1.indx < nip2.indx);
}

