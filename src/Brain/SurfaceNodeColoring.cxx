
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __SURFACE_NODE_COLORING_DECLARE__
#include "SurfaceNodeColoring.h"
#undef __SURFACE_NODE_COLORING_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventBrainStructureGet.h"
#include "EventManager.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteFile.h"
#include "PaletteScalarAndColor.h"
#include "RgbaFile.h"
#include "Surface.h"

using namespace caret;


/**
 * Constructor.
 */
SurfaceNodeColoring::SurfaceNodeColoring()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
SurfaceNodeColoring::~SurfaceNodeColoring()
{
    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceNodeColoring::toString() const
{
    return "SurfaceNodeColoring";
}

/**
 * Assign color components to surface nodes. 
 *
 * @param browserTabContent
 *    Tab in which this coloring is applied.
 * @param surface
 *    Surface that has its nodes colored.
 * @param surfaceOverlaySet
 *    Surface overlay assignments for surface.
 * @param rgbaNodeColors
 *    RGBA color components that are set by this method.
 */
void 
SurfaceNodeColoring::colorSurfaceNodes(BrowserTabContent* browserTabContent,
                                       const Surface* surface,
                                       OverlaySet* overlaySet,
                                       float* rgbaNodeColors)
{
    const int32_t numNodes = surface->getNumberOfNodes();
    const int32_t numberOfDisplayedOverlays = overlaySet->getNumberOfDisplayedOverlays();
    
    /*
     * Default color.
     */
    for (int32_t i = 0; i < numNodes; i++) {
        const int32_t i4 = i * 4;
        rgbaNodeColors[i4] = 0.70;
        rgbaNodeColors[i4+1] = 0.70;
        rgbaNodeColors[i4+2] = 0.70;
        rgbaNodeColors[i4+3] = 1.0;
    }
    
    
    EventBrainStructureGet brainStructureEvent(surface->getBrainStructureIdentifier());
    EventManager::get()->sendEvent(brainStructureEvent.getPointer());
    BrainStructure* brainStructure = brainStructureEvent.getBrainStructure();
    CaretAssert(brainStructure);
    
    bool firstOverlayFlag = true;
    float* overlayRGBV = new float[numNodes * 4];
    for (int32_t iOver = (numberOfDisplayedOverlays - 1); iOver >= 0; iOver--) {
        Overlay* overlay = overlaySet->getOverlay(iOver);
        if (overlay->isEnabled()) {
            DataFileTypeEnum::Enum mapDataFileType;
            AString selectedMapName;
            overlay->getSelectionData(browserTabContent,
                                      mapDataFileType,
                                      selectedMapName);
            
            bool isColoringValid = false;
            switch (mapDataFileType) {
                case DataFileTypeEnum::LABEL:
                    isColoringValid = this->assignLabelColoring(brainStructure, selectedMapName, numNodes, overlayRGBV);
                    break;
                case DataFileTypeEnum::METRIC:
                    isColoringValid = this->assignMetricColoring(brainStructure, selectedMapName, numNodes, overlayRGBV);
                    break;
                case DataFileTypeEnum::RGBA:
                    isColoringValid = this->assignRgbaColoring(brainStructure, selectedMapName, numNodes, overlayRGBV);
                    break;
                case DataFileTypeEnum::VOLUME:
                    break;
                case DataFileTypeEnum::UNKNOWN:
                    break;
                default:
                    CaretAssertMessage(0, "File type not supported for surface overlay: " 
                                       + DataFileTypeEnum::toName(mapDataFileType));
                    break;
            }
            
            if (isColoringValid) {
                const float opacity = overlay->getOpacity();
                const float oneMinusOpacity = 1.0 - opacity;
                
                for (int32_t i = 0; i < numNodes; i++) {
                    const int32_t i4 = i * 4;
                    const float valid = overlayRGBV[i4 + 3];
                    if (valid > 0.0 ) {
                        if ((opacity < 1.0) && (firstOverlayFlag == false)) {
                            
                            rgbaNodeColors[i4]   = (overlayRGBV[i4]   * opacity) 
                            + (rgbaNodeColors[i4] * oneMinusOpacity);
                            rgbaNodeColors[i4+1] = (overlayRGBV[i4+1] * opacity)
                            + (rgbaNodeColors[i4+1] * oneMinusOpacity);
                            rgbaNodeColors[i4+2] = (overlayRGBV[i4+2] * opacity)
                            + (rgbaNodeColors[i4+2] * oneMinusOpacity);
                        }
                        else {
                            rgbaNodeColors[i4] = overlayRGBV[i4];
                            rgbaNodeColors[i4+1] = overlayRGBV[i4+1];
                            rgbaNodeColors[i4+2] = overlayRGBV[i4+2];
                        }
                    }
                }
                
                firstOverlayFlag = false;
            }
        }
    }
    
    delete[] overlayRGBV;
}

/**
 * Assign label coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param labelColumnName
 *    Name of selected column.
 * @param numberOfNodes
 *    Number of nodes in surface.
 * @param rgbv
 *    Color components set by this method.
 *    Red, green, blue, valid.  If the valid component is
 *    zero, it indicates that the overlay did not assign
 *    any coloring to the node.
 * @return
 *    True if coloring is valid, else false.
 */
bool 
SurfaceNodeColoring::assignLabelColoring(BrainStructure* brainStructure, 
                                         const AString& labelColumnName,
                                         const int32_t numberOfNodes,
                                         float* rgbv)
{
    std::vector<LabelFile*> allLabelFiles;
    brainStructure->getLabelFiles(allLabelFiles);
    
    int32_t displayColumn = -1;
    LabelFile* labelFile = NULL;
    for (std::vector<LabelFile*>::iterator iter = allLabelFiles.begin();
         iter != allLabelFiles.end();
         iter++) {
        LabelFile* lf = *iter;
        displayColumn = lf->getColumnIndexFromColumnName(labelColumnName);
        if (displayColumn >= 0) {
            labelFile = lf;
            break;
        }
    }
    
    if (displayColumn < 0) {
        return false;
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }   
    
    GiftiLabelTable* labelTable = labelFile->getLabelTable();
    
    /*
     * Assign colors from labels to nodes
     */
    float labelRGBA[4];
    for (int i = 0; i < numberOfNodes; i++) {
        int labelKey= labelFile->getLabelKey(i, displayColumn);
        const GiftiLabel* gl = labelTable->getLabel(labelKey);
        if (gl != NULL) {
            gl->getColor(labelRGBA);
            if (labelRGBA[3] > 0.0) {
                const int32_t i4 = i * 4;
                rgbv[i4]   = labelRGBA[0];
                rgbv[i4+1] = labelRGBA[1];
                rgbv[i4+2] = labelRGBA[2];
                rgbv[i4+3] = 1.0;
            }
        }
    }

    return true;
}

bool 
SurfaceNodeColoring::assignMetricColoring(BrainStructure* brainStructure, 
                                          const AString& metricColumnName,
                                          const int32_t numberOfNodes,
                                          float* rgbv)
{
    std::vector<MetricFile*> allMetricFiles;
    brainStructure->getMetricFiles(allMetricFiles);
    
    int32_t displayColumn = -1;
    MetricFile* metricFile = NULL;
    for (std::vector<MetricFile*>::iterator iter = allMetricFiles.begin();
         iter != allMetricFiles.end();
         iter++) {
        MetricFile* mf = *iter;
        displayColumn = mf->getColumnIndexFromColumnName(metricColumnName);
        if (displayColumn >= 0) {
            metricFile = mf;
            break;
        }
    }
    
    if (displayColumn < 0) {
        return false;
    }
    
    PaletteColorMapping* paletteColorMapping = metricFile->getPaletteColorMapping(displayColumn);
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }   
    
    /*
     * Get min/max ranges.
     */
    float minMax[4];
    metricFile->getMinMaxForColorMapping(displayColumn, minMax);
    
    int thresholdColumn = metricFile->getColumnIndexFromColumnName(paletteColorMapping->getThresholdDataName());
    if (thresholdColumn < 0) {
        thresholdColumn = displayColumn;
    }
    const float* metricDisplayData = metricFile->getValuePointerForColumn(displayColumn);
    const float* metricThresholdData = metricFile->getValuePointerForColumn(thresholdColumn);
    
    Brain* brain = brainStructure->getBrain();
    const AString paletteName = paletteColorMapping->getSelectedPaletteName();
    Palette* palette = brain->getPaletteFile()->getPaletteByName(paletteName);
    if (palette != NULL) {
        float rgba[4];
        for (int32_t i = 0; i < numberOfNodes; i++) {
            rgba[0] = -1.0f;
            rgba[1] = -1.0f;
            rgba[2] = -1.0f;
            rgba[3] = -1.0f;
            bool validFlag = applyColorUsingPalette(paletteColorMapping,
                                                    minMax,
                                                    palette,
                                                    metricDisplayData[i],
                                                    metricThresholdData[i],
                                                    rgba);
            
            if (validFlag) {
                const int32_t i4 = i * 4;
                rgbv[i4]   = rgba[0];
                rgbv[i4+1] = rgba[1];
                rgbv[i4+2] = rgba[2];
                rgbv[i4+3] = 1.0;
            }
        }
    }
    else {
        CaretLogSevere("Selected palette for metric is invalid: \"" + paletteName + "\"");
    }
    return true;
}

bool 
SurfaceNodeColoring::assignRgbaColoring(BrainStructure* brainStructure, 
                                        const AString& rgbaColumnName,
                                        const int32_t numberOfNodes,
                                        float* rgbv)
{
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t i4 = i * 4;
        rgbv[i4]   = 0.0;
        rgbv[i4+1] = 0.0;
        rgbv[i4+2] = 1.0;
        rgbv[i4+3] = 1.0;
    }
    
    return true;
}

/**
 * Color using a palette.
 *
 * @param paletteColorMapping  Settings for color mapping with a palette.
 * @param negPosMinMax  array of four containing, in order, neg-max,
 *                       neg-min, pos-min, pos-max.
 * @param palette  the color palette.
 * @param value  the value whose color is to be determined.
 * @param thresholdValue  the threshold value
 * @param rgbaOut  the RGBA values out (4-dim array values ranging 0 to 255)
 * @return true if the output color is valid.
 */
bool 
SurfaceNodeColoring::applyColorUsingPalette(PaletteColorMapping* paletteColorMapping,
                                             float negPosMinMax[4],
                                             Palette* palette,
                                             float value,
                                             float thresholdValue,
                                             float rgbaOut[4]) {
    
    MetricColorType colorType = SurfaceNodeColoring::METRIC_COLOR_TYPE_NORMAL;
    
    bool thresholdTestPassedFlag =
    paletteColorMapping->performThresholdTest(thresholdValue);
    if (thresholdTestPassedFlag == false) {
        colorType = SurfaceNodeColoring::METRIC_COLOR_TYPE_DO_NOT_COLOR;
        if (paletteColorMapping->isShowThresholdFailureInGreen()) {
            if (paletteColorMapping->getThresholdType() ==
                PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED) {
                if (thresholdValue > 0.0f) {
                    if ((thresholdValue < paletteColorMapping->getThresholdMappedPositive()) &&
                        (thresholdValue > paletteColorMapping->getThresholdMappedAverageAreaPositive())) {
                        colorType = SurfaceNodeColoring::METRIC_COLOR_TYPE_POS_THRESH_COLOR;
                    }
                }
                else if (thresholdValue < 0.0f) {
                    if ((thresholdValue > paletteColorMapping->getThresholdMappedNegative()) &&
                        (thresholdValue < paletteColorMapping->getThresholdMappedAverageAreaNegative())) {
                        colorType = SurfaceNodeColoring::METRIC_COLOR_TYPE_NEG_THRESH_COLOR;
                    }
                }
            }
        }
    }
    
    if (value > 0.0f) {
        if (paletteColorMapping->isDisplayPositiveDataFlag() == false) {
            colorType = SurfaceNodeColoring::METRIC_COLOR_TYPE_DO_NOT_COLOR;
        }
    }
    else if (value < 0.0f) {
        if (paletteColorMapping->isDisplayNegativeDataFlag() == false) {
            colorType = SurfaceNodeColoring::METRIC_COLOR_TYPE_DO_NOT_COLOR;
        }
    }
    else {
        if (paletteColorMapping->isDisplayZeroDataFlag() == false) {
            colorType = SurfaceNodeColoring::METRIC_COLOR_TYPE_DO_NOT_COLOR;
        }
    }
    
    float negMax = negPosMinMax[0];
    float negMin = negPosMinMax[1];
    float posMin = negPosMinMax[2];
    float posMax = negPosMinMax[3];
    
    float posThreshColor[4] = {
        115.0f / 255.0f,
        255.0f / 255.0f,
        180.0f / 255.0f,
        255.0f / 255.0f
    };
    float negThreshColor[] = {
        180.0f / 255.0f,
        255.0f / 255.0f,
        115.0f / 255.0f,
        255.0f / 255.0f
    };
    
    bool colorValidFlag = false;
    switch (colorType) {
        case SurfaceNodeColoring::METRIC_COLOR_TYPE_NORMAL:
        {
            float normalized = 0.0f;
            if (value >= posMin) {
                float numerator = value - posMin;
                float denominator = posMax - posMin;
                if (denominator == 0.0f) {
                    denominator = 1.0f;
                }
                normalized = numerator / denominator;
            }
            else if (value <= negMin) {
                float numerator = value - negMin;
                float denominator = negMax - negMin;
                if (denominator == 0.0f) {
                    denominator = 1.0f;
                }
                else if (denominator < 0.0f) {
                    denominator = -denominator;
                }
                normalized = numerator / denominator;
            }
            
            float rgba[4];
            palette->getPaletteColor(normalized,
                                     paletteColorMapping->isInterpolatePaletteFlag(),
                                     rgba);
            if (rgba[3] > 0.0f) {
                rgbaOut[0]     = rgba[0];
                rgbaOut[1] = rgba[1];
                rgbaOut[2] = rgba[2];
                rgbaOut[3] = rgba[3];
                colorValidFlag = true;
            }
        }
            break;
        case SurfaceNodeColoring::METRIC_COLOR_TYPE_POS_THRESH_COLOR:
            rgbaOut[0]     = posThreshColor[0];
            rgbaOut[1] = posThreshColor[1];
            rgbaOut[2] = posThreshColor[2];
            rgbaOut[3] = posThreshColor[3];
            colorValidFlag = true;
            break;
        case SurfaceNodeColoring::METRIC_COLOR_TYPE_NEG_THRESH_COLOR:
            rgbaOut[0]     = negThreshColor[0];
            rgbaOut[1] = negThreshColor[1];
            rgbaOut[2] = negThreshColor[2];
            rgbaOut[3] = negThreshColor[3];
            colorValidFlag = true;
            break;
        case SurfaceNodeColoring::METRIC_COLOR_TYPE_DO_NOT_COLOR:
            break;
    }
    
    return colorValidFlag;
}
