
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

#include <algorithm>
#include <limits>

#include <QTextStream>

#include "CaretAssert.h"
#include "ChartPoint.h"
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
    m_boundsValid = false;
    m_histogram = NULL;
    
    m_histogramViewingType = ChartTwoHistogramViewingTypeEnum::HISTOGRAM_VIEWING_BARS;
    //m_histogramViewingType = ChartTwoHistogramViewingTypeEnum::HISTOGRAM_VIEWING_ENVELOPE;
    m_color = CaretColorEnum::RED;
    
    std::vector<CaretColorEnum::Enum> colorEnums;
    CaretColorEnum::getColorEnums(colorEnums);
    const int32_t numCaretColors = static_cast<int32_t>(colorEnums.size());
    
    bool colorFound = false;
    while ( ! colorFound) {
        ChartTwoDataHistogram::caretColorIndex++;
        if (ChartTwoDataHistogram::caretColorIndex >= numCaretColors) {
            ChartTwoDataHistogram::caretColorIndex = 0;
        }
        
        if (colorEnums[ChartTwoDataHistogram::caretColorIndex] == CaretColorEnum::BLACK) {
            /* do not use black */
        }
        else if (colorEnums[ChartTwoDataHistogram::caretColorIndex] == CaretColorEnum::WHITE) {
            /* do not use white */
        }
        else {
            m_color = colorEnums[ChartTwoDataHistogram::caretColorIndex];
            colorFound = true;
        }
    }
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_color",
                                                                &m_color);
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
    m_color             = obj.m_color;
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
 * @return Reference to histogram values.  Vector is empty
 *         if histogram is invalid.
 */
const std::vector<float>&
ChartTwoDataHistogram::getHistogramValues() const
{
    if (m_histogram != NULL) {
        return m_histogram->getHistogramDisplay();
    }
    
    return m_emptyData;
}


/**
 * Get the bounds of all of the points.
 *
 * @param boundsOut
 *     On exit contains bounds (xmin, xmax, ymin, ymax).
 * @return
 *     True if bounds valid, else false.
 */
bool
ChartTwoDataHistogram::getBounds(float boundsOut[4]) const
{
    if (! m_boundsValid) {
        m_bounds[0] = 0.0;
        m_bounds[1] = 0.0;
        m_bounds[2] = 0.0;
        m_bounds[3] = 0.0;
        
        if (m_histogram != NULL) {
            const int32_t numBuckets = m_histogram->getNumberOfBuckets();
            if (numBuckets > 0) {
                m_histogram->getRange(m_bounds[0],
                                      m_bounds[1]);
                
                const std::vector<float>& values = getHistogramValues();
                m_bounds[2] = 0.0;
                if (values.empty()) {
                    m_bounds[3] = 0.0;
                }
                else {
                    m_bounds[3] = *std::max_element(values.begin(),
                                                    values.end());
                }
                m_boundsValid = true;
            }
        }
    }
    
    boundsOut[0] = m_bounds[0];
    boundsOut[1] = m_bounds[1];
    boundsOut[2] = m_bounds[2];
    boundsOut[3] = m_bounds[3];
    
    return m_boundsValid;
}

/**
 * @return Color for chart
 */
CaretColorEnum::Enum
ChartTwoDataHistogram::getColor() const
{
    return m_color;
}

/**
 * Set the color for the chart.
 *
 * @param color
 *    New color for chart.
 */
void
ChartTwoDataHistogram::setColor(const CaretColorEnum::Enum color)
{
    m_color = color;
}

ChartTwoHistogramViewingTypeEnum::Enum
ChartTwoDataHistogram::getHistogramViewingType() const
{
    return m_histogramViewingType;
}

void
ChartTwoDataHistogram::setHistogramViewingType(const ChartTwoHistogramViewingTypeEnum::Enum histogramViewingType)
{
    m_histogramViewingType = histogramViewingType;
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

