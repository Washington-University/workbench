
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

#define __SELECTION_ITEM_CHART_TWO_LABEL_DECLARE__
#include "SelectionItemChartTwoLabel.h"
#undef __SELECTION_ITEM_CHART_TWO_LABEL_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::SelectionItemChartTwoLabel 
 * \brief Selection of chart version two label
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SelectionItemChartTwoLabel::SelectionItemChartTwoLabel()
: SelectionItem(SelectionItemDataTypeEnum::CHART_TWO_LABEL)
{
    m_chartTwoCartesianAxis = NULL;
    m_chartTwoOverlaySet    = NULL;
}

/**
 * Destructor.
 */
SelectionItemChartTwoLabel::~SelectionItemChartTwoLabel()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SelectionItemChartTwoLabel::SelectionItemChartTwoLabel(const SelectionItemChartTwoLabel& obj)
: SelectionItem(obj)
{
    this->copyHelperSelectionItemChartTwoLabel(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SelectionItemChartTwoLabel&
SelectionItemChartTwoLabel::operator=(const SelectionItemChartTwoLabel& obj)
{
    if (this != &obj) {
        SelectionItem::operator=(obj);
        this->copyHelperSelectionItemChartTwoLabel(obj);
    }
    return *this;    
}

/**
 * @return The cartesian axis.
 */
const ChartTwoCartesianAxis*
SelectionItemChartTwoLabel::getChartTwoCartesianAxis() const
{
    return m_chartTwoCartesianAxis;
}

/**
 * @return The cartesian axis.
 */
ChartTwoCartesianAxis*
SelectionItemChartTwoLabel::getChartTwoCartesianAxis()
{
    return m_chartTwoCartesianAxis;
}

/**
 * @return The chart overlay set.
 */
ChartTwoOverlaySet*
SelectionItemChartTwoLabel::getChartOverlaySet()
{
    return m_chartTwoOverlaySet;
}

/**
 * @return The chart overlay set.
 */
const ChartTwoOverlaySet*
SelectionItemChartTwoLabel::getChartOverlaySet() const
{
    return m_chartTwoOverlaySet;
}

/**
 * Set selection.
 *
 * @param chartTwoCartesianAxis
 *     The cartesian axis.
 * @param chartOverlaySet
 *     The chart overlay set
 */
void
SelectionItemChartTwoLabel::setChartTwoCartesianAxis(ChartTwoCartesianAxis* chartTwoCartesianAxis,
                                                     ChartTwoOverlaySet* chartOverlaySet)
{
    m_chartTwoCartesianAxis = chartTwoCartesianAxis;
    m_chartTwoOverlaySet    = chartOverlaySet;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SelectionItemChartTwoLabel::copyHelperSelectionItemChartTwoLabel(const SelectionItemChartTwoLabel& obj)
{
    m_chartTwoCartesianAxis = obj.m_chartTwoCartesianAxis;
    m_chartTwoOverlaySet    = obj.m_chartTwoOverlaySet;
}

/**
 * @return True if the selected chart label is valid, else false.
 */
bool
SelectionItemChartTwoLabel::isValid() const
{
    return ((m_chartTwoCartesianAxis != NULL)
            && (m_chartTwoOverlaySet != NULL));
}

/**
 * Reset the selections.
 */
void
SelectionItemChartTwoLabel::reset()
{
    m_chartTwoCartesianAxis = NULL;
    m_chartTwoOverlaySet    = NULL;
}

