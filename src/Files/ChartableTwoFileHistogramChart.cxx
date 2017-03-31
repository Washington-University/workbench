
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

#define __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_DECLARE__
#include "ChartableTwoFileHistogramChart.h"
#undef __CHARTABLE_TWO_FILE_HISTOGRAM_CHART_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
//#include "ChartTwoDataHistogram.h"
#include "Histogram.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::ChartableTwoFileHistogramChart 
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
ChartableTwoFileHistogramChart::ChartableTwoFileHistogramChart(const ChartTwoHistogramContentTypeEnum::Enum histogramContentType,
                                                                               CaretMappableDataFile* parentCaretMappableDataFile)
: ChartableTwoFileBaseChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM,
                                    parentCaretMappableDataFile),
m_histogramContentType(histogramContentType)
{
    m_sceneAssistant = new SceneClassAssistant();
    
    int32_t histogramNumberOfBuckets = 0;
    switch (m_histogramContentType) {
        case ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED:
            break;
        case ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_MAP_DATA:
       {
           CaretMappableDataFile* cmdf = getCaretMappableDataFile();
           if (cmdf->getNumberOfMaps() > 0) {
               const Histogram* histogram = cmdf->getMapHistogram(0);
               if (histogram != NULL) {
                   histogramNumberOfBuckets = histogram->getNumberOfBuckets();
               }
           }
       }
            break;
    }

    updateChartTwoCompoundDataTypeAfterFileChanges(ChartTwoCompoundDataType::newInstanceForHistogram(histogramNumberOfBuckets));
}

/**
 * Destructor.
 */
ChartableTwoFileHistogramChart::~ChartableTwoFileHistogramChart()
{
//    for (auto mapHistoPair : m_indexHistogramsMap) {
//        delete mapHistoPair.second;
//    }
//    m_indexHistogramsMap.clear();
    
    delete m_sceneAssistant;
}

/**
 * @return Content type of histogram.
 */
ChartTwoHistogramContentTypeEnum::Enum
ChartableTwoFileHistogramChart::getHistogramContentType() const
{
    return m_histogramContentType;
}

/**
 * @return Is this charting valid ?
 */
bool
ChartableTwoFileHistogramChart::isValid() const
{
    return (m_histogramContentType != ChartTwoHistogramContentTypeEnum::HISTOGRAM_CONTENT_TYPE_UNSUPPORTED);
}

/**
 * @retrurn Is this charting empty (no data at this time)
 */
bool
ChartableTwoFileHistogramChart::isEmpty() const
{
    return false;
}

///**
// * Get the histogram chart for the given map index.
// *
// * @param mapIndex
// *     Index for the map.
// * @return 
// *     Histogram for the map with the given index.  NULL if the file 
// *     does not support a histogram.
// */
//const ChartTwoDataHistogram*
//ChartableTwoFileHistogramChart::getMapHistogramChart(const int32_t mapIndex) const
//{
//    CaretAssertToDoFatal();  // This class may not be needed
//    
//    ChartTwoDataHistogram* histogramChartOut = NULL;
//    
//    std::map<int32_t, ChartTwoDataHistogram*>::iterator iter = m_indexHistogramsMap.find(mapIndex);
//    if (iter != m_indexHistogramsMap.end()) {
//        histogramChartOut = iter->second;
//    }
//    else {
//        histogramChartOut = new ChartTwoDataHistogram();
//        m_indexHistogramsMap.insert(std::make_pair(mapIndex,
//                                                   histogramChartOut));
//    }
//    
//    /*
//     * Update with histogram from the file even if it is NULL.
//     */
//    CaretMappableDataFile* mapFile = const_cast<CaretMappableDataFile*>(getCaretMappableDataFile());
//    CaretAssert(mapFile);
//    histogramChartOut->setHistogram(const_cast<Histogram*>(mapFile->getMapHistogram(mapIndex)));
//    
//    return histogramChartOut;
//}

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
ChartableTwoFileHistogramChart::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
ChartableTwoFileHistogramChart::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

