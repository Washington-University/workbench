
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __EVENT_CHART_MATRIX_PARCEL_LOADING_YOKING_DECLARE__
#include "EventChartMatrixParcelYokingValidation.h"
#undef __EVENT_CHART_MATRIX_PARCEL_LOADING_YOKING_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "ChartableMatrixParcelInterface.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventChartMatrixParcelYokingValidation 
 * \brief Event for coordination of Matrix Chart yoking.
 * \ingroup Files
 */

/**
 * Constructor for validating that the given file is compatible with any
 * current yoking for the given yoking group.  After sending this event,
 * call isYokingCompatible() to validate the yoking selection.
 *
 * @param chartableInterface
 *   The file for which yoking compatibility is verified.
 * @param yokingGroup
 *   The selected yoking group.
 */
EventChartMatrixParcelYokingValidation::EventChartMatrixParcelYokingValidation(const ChartableMatrixParcelInterface* chartableInterface,
                                                             const YokingGroupEnum::Enum yokingGroup)
: Event(EventTypeEnum::EVENT_CHART_MATRIX_YOKING_VALIDATION),
m_mode(MODE_VALIDATE_YOKING),
m_chartableInterface(chartableInterface),
m_yokingGroup(yokingGroup)
{
    CaretAssert(chartableInterface);
}

/**
 * Destructor.
 */
EventChartMatrixParcelYokingValidation::~EventChartMatrixParcelYokingValidation()
{
}

/**
 * @return The mode of this event (apply or validate).
 */
EventChartMatrixParcelYokingValidation::Mode
EventChartMatrixParcelYokingValidation::getMode() const
{
    return m_mode;
}

/**
 * Add chartable interface for validating yoking compatibility.
 *
 * @param chartableInterface
 *   A file for which yoking compatibility is verified with the file passed
 *   to the constructor.
 * @param selectedRowOrColumnIndex
 *   Selected row or column index in the file.
 */
void
EventChartMatrixParcelYokingValidation::addValidateYokingChartableInterface(const ChartableMatrixParcelInterface* chartableInterface,
                                                                      const int32_t selectedRowOrColumnIndex)
{
    CaretAssert(chartableInterface);
    
    int32_t numberOfRowsColumns = -1;
    AString loadingDimensionName;
    
    int32_t numRows = -1;
    int32_t numCols = -1;
    m_chartableInterface->getMatrixDimensions(numRows,
                                              numCols);
    switch (m_chartableInterface->getMatrixLoadingDimension()) {
        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
            numberOfRowsColumns = numCols;
            loadingDimensionName = "columns";
            break;
        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
            numberOfRowsColumns = numRows;
            loadingDimensionName = "rows";
            break;
    }
    
    if (chartableInterface != m_chartableInterface) {
        if ((chartableInterface->getYokingGroup() == m_yokingGroup)
            && (chartableInterface->getYokingGroup() != YokingGroupEnum::YOKING_GROUP_OFF)) {
            int32_t chartInterNumRowsColumns = -1;
            AString chartDimensionsName;
            int32_t chartRows = -1;
            int32_t chartCols = -1;
            chartableInterface->getMatrixDimensions(chartRows,
                                                    chartCols);
            switch (chartableInterface->getMatrixLoadingDimension()) {
                case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                    chartInterNumRowsColumns = chartCols;
                    chartDimensionsName = "columns";
                    break;
                case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                    chartInterNumRowsColumns = chartRows;
                    chartDimensionsName = "rows";
                    break;
            }
            
            if (numberOfRowsColumns != chartInterNumRowsColumns) {
                if (m_incompatibilityMessage.isEmpty()) {
                     m_incompatibilityMessage.appendWithNewLine(m_chartableInterface->getMatrixChartCaretMappableDataFile()->getFileNameNoPath()
                                                                + " is loading from "
                                                                + AString::number(numberOfRowsColumns)
                                                                + " "
                                                                + loadingDimensionName);
                    m_incompatibilityMessage.appendWithNewLine("and is incompatible with: ");
                }
                m_incompatibilityMessage.appendWithNewLine("   "
                                                           + chartableInterface->getMatrixChartCaretMappableDataFile()->getFileNameNoPath()
                                                           + " is loading from "
                                                           + AString::number(chartInterNumRowsColumns)
                                                           + " "
                                                           + chartDimensionsName);
            }
            
            /*
             * For the map key is row/column index and value is number of times that
             * row column index is used.
             */
            std::map<int32_t, int32_t>::iterator iter = m_compatibleRowColumnIndicesCount.find(selectedRowOrColumnIndex);
            if (iter != m_compatibleRowColumnIndicesCount.end()) {
                iter->second++;
            }
            else {
                m_compatibleRowColumnIndicesCount.insert(std::make_pair(selectedRowOrColumnIndex, 1));
            }
        }
    }
}

/**
 * Is the yoking for the file passed to the constructor validated with
 * other files yoked to the yoking group?
 *
 * @param messageOut
 *    Contains information on compatibility.
 * @param selectedRowOrColumnIndexOut
 *    Row or column index that should be selected if yoking is compatible.
 * @return
 *    True if compatible, else false.
 */
bool
EventChartMatrixParcelYokingValidation::isValidateYokingCompatible(AString& messageOut,
                                                             int32_t& selectedRowOrColumnIndexOut) const
{
    selectedRowOrColumnIndexOut = -1;
    messageOut = m_incompatibilityMessage;
    
    /*
     * For the map key is row/column index and value is number of times that
     * row column index is used.
     */
    int32_t maxCount = 0;
    for (std::map<int32_t, int32_t>::const_iterator iter = m_compatibleRowColumnIndicesCount.begin();
         iter != m_compatibleRowColumnIndicesCount.end();
         iter++) {
        if (iter->second > maxCount) {
            maxCount = iter->second;
            selectedRowOrColumnIndexOut = iter->first;
        }
    }
    
    if (messageOut.isEmpty()) {
        return true;
    }
    
    return false;
}

/**
 * @return Chartable interface for which this event was issued.
 */
const ChartableMatrixParcelInterface*
EventChartMatrixParcelYokingValidation::getChartableMatrixParcelInterface() const
{
    return m_chartableInterface;
}

