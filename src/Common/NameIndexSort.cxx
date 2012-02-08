
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

