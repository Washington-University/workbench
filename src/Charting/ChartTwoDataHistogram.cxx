
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

#define __CHART_TWO_DATA_HISTOGRAM_DECLARE__
#include "ChartTwoDataHistogram.h"
#undef __CHART_TWO_DATA_HISTOGRAM_DECLARE__

#include "CaretAssert.h"
#include "Histogram.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoDataHistogram 
 * \brief Chart histogram chart data.
 * \ingroup Charting
 */

/**
 * Constructor.
 */
ChartTwoDataHistogram::ChartTwoDataHistogram()
: ChartTwoData(ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM)
{
    initializeMembersChartTwoDataHistogram();
}

/**
 * Destructor.
 */
ChartTwoDataHistogram::~ChartTwoDataHistogram()
{
    delete m_sceneAssistant;
}

/**
 * Initialize members of a new instance.
 */
void
ChartTwoDataHistogram::initializeMembersChartTwoDataHistogram()
{
    m_histogram = NULL;
    
    m_sceneAssistant = new SceneClassAssistant();
}

/**
 * At times a copy of chart data will be needed BUT it must be
 * the proper subclass so copy constructor and assignment operator
 * will no function when this abstract, base class is used.  Each
 * subclass will override this method so that the returned class
 * is of the proper type.
 *
 * @return Copy of this instance that is the actual subclass.
 */
ChartTwoData*
ChartTwoDataHistogram::clone() const
{
    ChartTwoDataHistogram* cloneCopy = new ChartTwoDataHistogram(*this);
    return cloneCopy;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
ChartTwoDataHistogram::ChartTwoDataHistogram(const ChartTwoDataHistogram& obj)
: ChartTwoData(obj)
{
    initializeMembersChartTwoDataHistogram();
    this->copyHelperChartTwoDataHistogram(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
ChartTwoDataHistogram&
ChartTwoDataHistogram::operator=(const ChartTwoDataHistogram& obj)
{
    if (this != &obj) {
        ChartTwoData::operator=(obj);
        this->copyHelperChartTwoDataHistogram(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
ChartTwoDataHistogram::copyHelperChartTwoDataHistogram(const ChartTwoDataHistogram& obj)
{
    CaretAssert(0);
    //m_histogram = obj.m_histogram;
}

/**
 * @return The actual histogram (may be NULL).
 */
const Histogram*
ChartTwoDataHistogram::getHistogram() const
{
    return m_histogram;
}

/**
 * Set the histogram for charting.
 *
 * @param histogram
 *     Updated histogram (may be NULL).
 */
void
ChartTwoDataHistogram::setHistogram(Histogram* histogram)
{
    m_histogram = histogram;
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
ChartTwoDataHistogram::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                     SceneClass* sceneClass)
{
    SceneClass* chartDataHistogram = new SceneClass("chartDataHistogram",
                                               "ChartTwoDataHistogram",
                                               1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  chartDataHistogram);
    
    
    sceneClass->addClass(chartDataHistogram);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ChartTwoDataHistogram::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass)
{
    const SceneClass* chartDataHistogram = sceneClass->getClass("chartDataCartesian");
    if (chartDataHistogram == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, chartDataHistogram);
}

