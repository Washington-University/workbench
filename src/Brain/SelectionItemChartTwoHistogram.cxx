
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __SELECTION_ITEM_CHART_TWO_HISTOGRAM_DECLARE__
#include "SelectionItemChartTwoHistogram.h"
#undef __SELECTION_ITEM_CHART_TWO_HISTOGRAM_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTwoHistogram 
 * \brief Selection of charting two histogram
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTwoHistogram::SelectionItemChartTwoHistogram()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TWO_HISTOGRAM)
{
    
}

/**
 * Destructor.
 */
SelectionItemChartTwoHistogram::~SelectionItemChartTwoHistogram()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTwoHistogram::SelectionItemChartTwoHistogram(const SelectionItemChartTwoHistogram& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTwoHistogram(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTwoHistogram&
SelectionItemChartTwoHistogram::operator=(const SelectionItemChartTwoHistogram& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTwoHistogram(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTwoHistogram::copyHelperSelectionItemChartTwoHistogram(const SelectionItemChartTwoHistogram& obj)
{
    
}

/**
 * @return True if the selected chart is valid, else false.
 */
bool
SelectionItemChartTwoHistogram::isValid() const
{
    return false;
}

/**
 * Reset the selections.
 */
void
SelectionItemChartTwoHistogram::reset()
{
}


