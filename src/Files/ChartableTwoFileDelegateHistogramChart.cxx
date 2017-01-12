
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

#define __CHARTABLE_TWO_FILE_DELEGATE_HISTOGRAM_CHART_DECLARE__
#include "ChartableTwoFileDelegateHistogramChart.h"
#undef __CHARTABLE_TWO_FILE_DELEGATE_HISTOGRAM_CHART_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartableTwoFileDelegateHistogramChart 
 * \brief Implementation of base chart delegate for histogram charts.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param histogramContentType
 *     Content of histogram.
 * @param parentCaretMappableDataFile
 *     Parent caret mappable data file that this delegate supports.
 */
ChartableTwoFileDelegateHistogramChart::ChartableTwoFileDelegateHistogramChart(const ChartTwoHistogramContentTypeEnum::Enum histogramContentType,
                                                                               CaretMappableDataFile* parentCaretMappableDataFile)
: ChartableTwoFileDelegateBaseChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM,
                                    parentCaretMappableDataFile),
m_histogramContentType(histogramContentType)
{
    CaretAssert(m_histogramContentType != ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED);
    m_sceneAssistant = new SceneClassAssistant();
    
}

/**
 * Destructor.
 */
ChartableTwoFileDelegateHistogramChart::~ChartableTwoFileDelegateHistogramChart()
{
    delete m_sceneAssistant;
}

/**
 * @return Content type of histogram.
 */
ChartTwoHistogramContentTypeEnum::Enum
ChartableTwoFileDelegateHistogramChart::getHistogramContentType() const
{
    return m_histogramContentType;
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
ChartableTwoFileDelegateHistogramChart::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
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
ChartableTwoFileDelegateHistogramChart::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

