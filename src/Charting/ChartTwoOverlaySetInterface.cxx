
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

#define __CHART_TWO_OVERLAY_SET_INTERFACE_DECLARE__
#include "ChartTwoOverlaySetInterface.h"
#undef __CHART_TWO_OVERLAY_SET_INTERFACE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::ChartTwoOverlaySetInterface 
 * \brief Interface for some functions that are needed in Chart module
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoOverlaySetInterface::ChartTwoOverlaySetInterface()
{
    
}

/**
 * Destructor.
 */
ChartTwoOverlaySetInterface::~ChartTwoOverlaySetInterface()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoOverlaySetInterface::ChartTwoOverlaySetInterface(const ChartTwoOverlaySetInterface& obj)
{
    this->copyHelperChartTwoOverlaySetInterface(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoOverlaySetInterface&
ChartTwoOverlaySetInterface::operator=(const ChartTwoOverlaySetInterface& obj)
{
    if (this != &obj) {
        this->copyHelperChartTwoOverlaySetInterface(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoOverlaySetInterface::copyHelperChartTwoOverlaySetInterface(const ChartTwoOverlaySetInterface& /* obj */)
{
    
}

