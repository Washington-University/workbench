
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
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventCaretPreferencesGet.h"
#include "EventManager.h"
#include "FastStatistics.h"
#include "Histogram.h"
#include "NodeAndVoxelColoring.h"
#include "PaletteColorMapping.h"
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
    if ( ! isValid()) {
        return true;
    }
    
    return m_mapHistogramPrimitives.empty();
}

/**
 * Invalidate coloring for all histograms.
 */
void
ChartableTwoFileHistogramChart::invalidateAllColoring()
{
    m_mapHistogramPrimitives.clear();
}

/**
 * Get the histogram for drawing histogram as a chart
 *
 * @param mapIndex
 *     Index of the map.
 * @param useDataFromAllMapsFlag
 *     If true, include data from all maps for histogram chart.
 * @return
 *     Histogram for histogram chart drawing.  May be NULL if map is not
 *     mapped with a color palette or the map index is invalid.
 */
const Histogram*
ChartableTwoFileHistogramChart::getHistogramForChartDrawing(const int32_t mapIndex,
                                                            const bool useDataFromAllMapsFlag)
{
    CaretMappableDataFile* myMapFile = getCaretMappableDataFile();
    CaretAssert(myMapFile);
    
    const Histogram* histogramOut= NULL;
    
    if ( ! myMapFile->isMappedWithPalette()) {
        return histogramOut;
    }
    if ((mapIndex < 0)
        || (mapIndex >= myMapFile->getNumberOfMaps())) {
        return histogramOut;
    }
    
    PaletteColorMapping* paletteColorMapping = myMapFile->getMapPaletteColorMapping(mapIndex);
    CaretAssert(paletteColorMapping);
    
    FastStatistics* statistics = NULL;
    if (useDataFromAllMapsFlag) {
        statistics = const_cast<FastStatistics*>(myMapFile->getFileFastStatistics());
    }
    else {
        switch (myMapFile->getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                statistics = const_cast<FastStatistics*>(myMapFile->getFileFastStatistics());
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                statistics = const_cast<FastStatistics*>(myMapFile->getMapFastStatistics(mapIndex));
                break;
        }
    }
    
    /*
     * Statistics may be NULL for connectivity files (dense, dense dynamic)
     * that have not yet loaded any data caused by the user clicking
     * brainordinates.
     */
    if (statistics == NULL) {
        return histogramOut;
    }
    
    CaretAssert(statistics);
    float mostPos  = 0.0;
    float leastPos = 0.0;
    float leastNeg = 0.0;
    float mostNeg  = 0.0;
    bool matchFlag = false;
    
    switch (paletteColorMapping->getHistogramRangeMode()) {
        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_ALL:
        {
            float dummy;
            statistics->getNonzeroRanges(mostNeg, dummy, dummy, mostPos);
        }
            break;
        case PaletteHistogramRangeModeEnum::PALETTE_HISTOGRAM_RANGE_MATCH_PALETTE:
        {
            matchFlag = true;
            switch (paletteColorMapping->getScaleMode()) {
                case PaletteScaleModeEnum::MODE_AUTO_SCALE:
                    mostPos  = statistics->getMax();
                    leastPos = 0.0;
                    leastNeg = 0.0;
                    mostNeg  = statistics->getMin();
                    break;
                case PaletteScaleModeEnum::MODE_AUTO_SCALE_ABSOLUTE_PERCENTAGE:
                    mostPos  =  statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
                    leastPos =  statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
                    leastNeg = -statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMinimum());
                    mostNeg  = -statistics->getApproxAbsolutePercentile(paletteColorMapping->getAutoScaleAbsolutePercentageMaximum());
                    break;
                case PaletteScaleModeEnum::MODE_AUTO_SCALE_PERCENTAGE:
                    mostPos  = statistics->getApproxPositivePercentile(paletteColorMapping->getAutoScalePercentagePositiveMaximum());
                    leastPos = statistics->getApproxPositivePercentile(paletteColorMapping->getAutoScalePercentagePositiveMinimum());
                    leastNeg = statistics->getApproxNegativePercentile(paletteColorMapping->getAutoScalePercentageNegativeMinimum());
                    mostNeg  = statistics->getApproxNegativePercentile(paletteColorMapping->getAutoScalePercentageNegativeMaximum());
                    break;
                case PaletteScaleModeEnum::MODE_USER_SCALE:
                    mostPos  = paletteColorMapping->getUserScalePositiveMaximum();
                    leastPos = paletteColorMapping->getUserScalePositiveMinimum();
                    leastNeg = paletteColorMapping->getUserScaleNegativeMinimum();
                    mostNeg  = paletteColorMapping->getUserScaleNegativeMaximum();
                    break;
            }
        }
            break;
    }
    
    /*
     * Remove data that is not displayed
     */
    bool isZeroIncluded = true;
    if (matchFlag) {
        isZeroIncluded = paletteColorMapping->isDisplayZeroDataFlag();
        
        if ( ! paletteColorMapping->isDisplayNegativeDataFlag()) {
            mostNeg  = 0.0;
            leastNeg = 0.0;
        }
        if ( ! paletteColorMapping->isDisplayPositiveDataFlag()) {
            mostPos  = 0.0;
            leastPos = 0.0;
        }
        
        if (useDataFromAllMapsFlag) {
            histogramOut = myMapFile->getFileHistogram(mostPos,
                                                       leastPos,
                                                       leastNeg,
                                                       mostNeg,
                                                       isZeroIncluded);
        }
        else {
            histogramOut = myMapFile->getMapHistogram(mapIndex,
                                                      mostPos,
                                                      leastPos,
                                                      leastNeg,
                                                      mostNeg,
                                                      isZeroIncluded);
        }
    }
    else {
        if (useDataFromAllMapsFlag) {
            histogramOut = myMapFile->getFileHistogram();
        }
        else {
            histogramOut = myMapFile->getMapHistogram(mapIndex);
        }
    }
    
    return histogramOut;
}

/**
 * Get the graphics primitive for drawing bars for the given map index.
 *
 * @param mapIndex
 *     Index of the map.
 * @return
 *     Primitives for drawing the histogram, bars, and envelope.  May be NULL if map is not
 *     mapped with a color palette or the map index is invalid.
 */
ChartableTwoFileHistogramChart::HistogramPrimitives*
ChartableTwoFileHistogramChart::getMapHistogramDrawingPrimitives(const int32_t mapIndex,
                                                             const bool useDataFromAllMapsFlag)
{
    MapIndexPrimitiveContainer::iterator iter = m_mapHistogramPrimitives.find(mapIndex);
    if (iter != m_mapHistogramPrimitives.end()) {
        HistogramPrimitives* histogramPrimitives = iter->second.get();
        if (histogramPrimitives != NULL) {
            return histogramPrimitives;
        }
    }
    
    CaretMappableDataFile* myMapFile = getCaretMappableDataFile();
    CaretAssert(myMapFile);
    
    const Histogram* histogram = getHistogramForChartDrawing(mapIndex,
                                                       useDataFromAllMapsFlag);
    if (histogram == NULL) {
        return NULL;
    }
    
    CaretAssert(histogram);
    
    FastStatistics* statistics = NULL;
    if (useDataFromAllMapsFlag) {
        statistics = const_cast<FastStatistics*>(myMapFile->getFileFastStatistics());
    }
    else {
        switch (myMapFile->getPaletteNormalizationMode()) {
            case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
                statistics = const_cast<FastStatistics*>(myMapFile->getFileFastStatistics());
                break;
            case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
                statistics = const_cast<FastStatistics*>(myMapFile->getMapFastStatistics(mapIndex));
                break;
        }
    }
    
    PaletteColorMapping* paletteColorMapping = myMapFile->getMapPaletteColorMapping(mapIndex);
    CaretAssert(paletteColorMapping);
    
    float minValueX = 0.0;
    float maxValueX = 0.0;
    histogram->getRange(minValueX,
                        maxValueX);
    const float valueRangeX = maxValueX - minValueX;
    if (valueRangeX <= 0.0) {
        CaretLogSevere("Range of histogram X-values is zero");
        return NULL;
    }
    
    if ((paletteColorMapping != NULL)
        && (statistics != NULL)
        && (histogram != NULL)) {
        std::vector<float> histogramBuckets = histogram->getHistogramDisplay();
        const int32_t numBucketValues = static_cast<int32_t>(histogramBuckets.size());
        if (numBucketValues < 2) {
            CaretLogSevere("Histogram must contain two or more values");
            return NULL;
        }
        
        const float bucketWidth = valueRangeX / (numBucketValues - 1);
        std::vector<float> xValuesAtBuckets;
        xValuesAtBuckets.reserve(numBucketValues);
        for (int32_t i = 0; i < numBucketValues; i++) {
            xValuesAtBuckets.push_back(minValueX + (bucketWidth * i));
        }
        CaretAssert(xValuesAtBuckets.size() == histogramBuckets.size());
        
        std::vector<float> histogramRGBA;
        histogramRGBA.resize(numBucketValues * 4,
                             0.0f);
        NodeAndVoxelColoring::colorScalarsWithPalette(statistics,
                                                      paletteColorMapping,
                                                      &xValuesAtBuckets[0],
                                                      paletteColorMapping,
                                                      &xValuesAtBuckets[0],
                                                      numBucketValues,
                                                      &histogramRGBA[0],
                                                      true); // ignore thresholding
        
        
        const int32_t estimatedNumberOfVerticesForQuads = (numBucketValues * 4); // four vertices per quad
        const int32_t estimatedNumberOfVerticesForEnvelope = (numBucketValues * 2) + 4; // 2 vertices per line segment plus start/end
        
        /*
         * Use Quads when drawing bars (four vertices quad).
         * Use Lines when drawing envelope (two vertices per line)
         * Using quads and lines simplifies identification of individual bars in the histogram
         */
        GraphicsPrimitiveV3fC4f* barsPrimitive = new GraphicsPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES);
        barsPrimitive->reserveForNumberOfVertices(estimatedNumberOfVerticesForQuads);
        GraphicsPrimitiveV3fC4f* envelopePrimitive  = new GraphicsPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN);
        envelopePrimitive->reserveForNumberOfVertices(estimatedNumberOfVerticesForEnvelope);
        GraphicsPrimitiveV3fC4f* thresholdPrimitive = NULL;
        
        bool setThresholdingFlag = false;
        switch (paletteColorMapping->getThresholdType()) {
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
                setThresholdingFlag = true;
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_FILE:
                break;
            case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
                break;
        }
        
        if (setThresholdingFlag) {
            /*
             * RGBA for thresholding
             */
            EventCaretPreferencesGet preferencesEvent;
            EventManager::get()->sendEvent(preferencesEvent.getPointer());
            CaretPreferences* caretPreferences = preferencesEvent.getCaretPreferences();
            float threshRGBA[4] = { 1.0, 0.0, 0.0, 1.0 };
            if (caretPreferences != NULL) {
                uint8_t threshByteRGBA[4];
                caretPreferences->getBackgroundAndForegroundColors()->getColorChartHistogramThreshold(threshByteRGBA);
                threshRGBA[0] = static_cast<float>(threshByteRGBA[0]) / 255.0f;
                threshRGBA[1] = static_cast<float>(threshByteRGBA[1]) / 255.0f;
                threshRGBA[2] = static_cast<float>(threshByteRGBA[2]) / 255.0f;
                threshRGBA[3] = 1.0;
            }
            const float minX = -1000000.0f;
            const float maxX = 1000000.0f;
            const float minY = 0.0;
            const float maxY = *std::max_element(histogramBuckets.begin(),
                                                 histogramBuckets.end());
            
            float threshMinValue = paletteColorMapping->getThresholdNormalMinimum();
            float threshMaxValue = paletteColorMapping->getThresholdNormalMaximum();
            
            thresholdPrimitive = new GraphicsPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES);
            
            switch (paletteColorMapping->getThresholdTest()) {
                case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_INSIDE:
                {
                    thresholdPrimitive->reserveForNumberOfVertices(12);

                    thresholdPrimitive->addVertex(minX, maxY, threshRGBA);
                    thresholdPrimitive->addVertex(minX, minY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMinValue, minY, threshRGBA);
                    
                    thresholdPrimitive->addVertex(minX, maxY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMinValue, minY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMinValue, maxY, threshRGBA);
                    
                    
                    thresholdPrimitive->addVertex(threshMaxValue, maxY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMaxValue, minY, threshRGBA);
                    thresholdPrimitive->addVertex(maxX, minY, threshRGBA);
                    
                    thresholdPrimitive->addVertex(threshMaxValue, maxY, threshRGBA);
                    thresholdPrimitive->addVertex(maxX, minY, threshRGBA);
                    thresholdPrimitive->addVertex(maxX, maxY, threshRGBA);
                }
                    break;
                case PaletteThresholdTestEnum::THRESHOLD_TEST_SHOW_OUTSIDE:
                {
                    thresholdPrimitive->reserveForNumberOfVertices(6);
                    
                    thresholdPrimitive->addVertex(threshMinValue, maxY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMinValue, minY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMaxValue, minY, threshRGBA);
                    
                    thresholdPrimitive->addVertex(threshMinValue, maxY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMaxValue, minY, threshRGBA);
                    thresholdPrimitive->addVertex(threshMaxValue, maxY, threshRGBA);
                }
                    break;
            }
        }
        
        /*
         * If selected, apply solid color to envelope histogram
         */
        const CaretColorEnum::Enum histogramEnvelopeColor = paletteColorMapping->getHistogramEnvelopeColor();
        float envelopeSolidColorRGBA[4] = { 0.0, 0.0, 0.0, 0.0 };
        bool envelopeSolidColorFlag = false;
        if (histogramEnvelopeColor != CaretColorEnum::CUSTOM) {
            CaretColorEnum::toRGBAFloat(histogramEnvelopeColor,
                                        envelopeSolidColorRGBA);
            envelopeSolidColorFlag = true;
        }
        
        /*
         * If selected, apply solid color to bars histogram
         */
        const CaretColorEnum::Enum histogramBarsColor = paletteColorMapping->getHistogramBarsColor();
        float barsSolidColorRGBA[4] = { 0.0, 0.0, 0.0, 0.0 };
        bool barsSolidColorFlag = false;
        if (histogramBarsColor != CaretColorEnum::CUSTOM) {
            CaretColorEnum::toRGBAFloat(histogramBarsColor,
                                        barsSolidColorRGBA);
            barsSolidColorFlag = true;
        }
        
        /*
         * Move start one-half of bucket width so that bar's
         * X-value is centered within the bar
         */
        const float halfBucketWidth = bucketWidth / 2.0f;
        const float startX = minValueX - halfBucketWidth;
        
        
        float x = startX;
        for (int32_t i = 0; i < numBucketValues; i++) {
            CaretAssertVectorIndex(histogramBuckets, i);
            const float xMin = x;
            const float xMax = x + bucketWidth;
            const float yMin = 0.0f;
            float yMax = histogramBuckets[i];
            
            CaretAssertVectorIndex(histogramRGBA, i*4+3);
            const float* rgba = &histogramRGBA[i * 4];
            const float alpha = rgba[3];
            
            /*
             * Use a height of zero when there is no coloring
             * (apha == 0) for a bar since the bars will be
             * smoothed to create the envelope.
             */
            if (alpha <= 0.0) {
                yMax = 0.0;
            }
            
            if (barsSolidColorFlag) {
                barsSolidColorRGBA[3] = rgba[3];
            }
            const float* cellRGBA = (barsSolidColorFlag ? barsSolidColorRGBA : rgba);

            /* Triangle One for Cell*/
            barsPrimitive->addVertex(xMin, yMax, cellRGBA);
            barsPrimitive->addVertex(xMin, yMin, cellRGBA);
            barsPrimitive->addVertex(xMax, yMin, cellRGBA);
            
            /* Triangle Two for Cell */
            barsPrimitive->addVertex(xMin, yMax, cellRGBA);
            barsPrimitive->addVertex(xMax, yMin, cellRGBA);
            barsPrimitive->addVertex(xMax, yMax, cellRGBA);

            if (envelopeSolidColorFlag) {
                const float envelopeX = x + halfBucketWidth;
                
                if (i == 0) {
                    envelopePrimitive->addVertex(envelopeX, yMin, envelopeSolidColorRGBA);
                }
                envelopePrimitive->addVertex(envelopeX, yMax, envelopeSolidColorRGBA);
                if (i == (numBucketValues - 1)) {
                    envelopePrimitive->addVertex(envelopeX, yMin, envelopeSolidColorRGBA);
                }
            }
            else {
                const float envelopeX = x + halfBucketWidth;
                
                if (i == 0) {
                    envelopePrimitive->addVertex(envelopeX, yMin, rgba);
                }
                envelopePrimitive->addVertex(envelopeX, yMax, rgba);
                if (i == (numBucketValues - 1)) {
                    envelopePrimitive->addVertex(envelopeX, yMin, rgba);
                }
            }
            
            x += bucketWidth;
        }
        
        HistogramPrimitives* histogramPrimitives =  new HistogramPrimitives(thresholdPrimitive,
                                                                            barsPrimitive,
                                                                            envelopePrimitive,
                                                                            paletteColorMapping->getHistogramEnvelopeLineWidthPercentage());
        
        m_mapHistogramPrimitives.insert(std::make_pair(mapIndex,
                                                          std::unique_ptr<HistogramPrimitives>(histogramPrimitives)));
        
        return histogramPrimitives;
        
    }
    
    return NULL;
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

/**
 * Default constructor.
 */
ChartableTwoFileHistogramChart::HistogramPrimitives::HistogramPrimitives()
{
}

/**
 * Constructor for primitives
 *
 * @param thresholdPrimitive
 *     Graphics Primitive for drawing threshold ranges.
 * @param barsPrimitive
 *     Graphics Primitive for drawing histogram bars.
 * @param envelopePrimitive
 *     Graphics Primitive for drawing histogram envelope.
 * @param envelopeLineWidthPercentage
 *     Line width percentage for drawing envelope.
 */
ChartableTwoFileHistogramChart::HistogramPrimitives::HistogramPrimitives(GraphicsPrimitiveV3fC4f* thresholdPrimitive,
                                                                         GraphicsPrimitiveV3fC4f* barsPrimitive,
                                                                         GraphicsPrimitiveV3fC4f* envelopePrimitive,
                                                                         const float envelopeLineWidthPercentage)
{
    m_thresholdPrimitive.reset(thresholdPrimitive);
    m_barsPrimitive.reset(barsPrimitive);
    m_envelopePrimitive.reset(envelopePrimitive);
    m_envelopeLineWidthPercentage = envelopeLineWidthPercentage;
}

/*
 * Destructor.
 */
ChartableTwoFileHistogramChart::HistogramPrimitives::~HistogramPrimitives()
{
    
}
