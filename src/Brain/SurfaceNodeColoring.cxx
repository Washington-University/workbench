
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

#define __SURFACE_NODE_COLORING_DECLARE__
#include "SurfaceNodeColoring.h"
#undef __SURFACE_NODE_COLORING_DECLARE__

#include "Brain.h"
#include "BrainordinateRegionOfInterest.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "EventBrowserTabGet.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelScalarFile.h"
#include "CiftiParcelSeriesFile.h"
#include "DisplayPropertiesSurface.h"
#include "GroupAndNameHierarchyModel.h"
#include "DisplayPropertiesLabels.h"
#include "DisplayPropertiesSurface.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyGroup.h"
#include "LabelFile.h"
#include "LabelDrawingProperties.h"
#include "MetricFile.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "NodeAndVoxelColoring.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Palette.h"
#include "PaletteColorMapping.h"
#include "PaletteScalarAndColor.h"
#include "RgbaFile.h"
#include "SessionManager.h"
#include "Surface.h"
#include "TopologyHelper.h"

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
 * If colors are currently valid, no changes are made to the surface coloring.
 * @param model
 *     Model that is displayed.  If NULL use find ModelSurface
 *     for the surface.  This case occurs when needing surface coloring 
 *     when surface outline is drawn on volume slices.
 * @param surface
 *     Surface that is displayed.
 * @param browserTabIndex
 *     Index of tab in which model is displayed.
 */
float* 
SurfaceNodeColoring::colorSurfaceNodes(Model* model,
                                       Surface* surface,
                                       const int32_t browserTabIndex)
{
    CaretAssert(surface);

    ModelSurface* surfaceModel = dynamic_cast<ModelSurface*>(model);
    ModelSurfaceMontage* surfaceMontageModel = dynamic_cast<ModelSurfaceMontage*>(model);
    ModelWholeBrain* wholeBrainModel = dynamic_cast<ModelWholeBrain*>(model);
    
    OverlaySet* overlaySet = NULL;
    float* rgba = NULL;

    EventBrowserTabGet getBrowserTab(browserTabIndex);
    EventManager::get()->sendEvent(getBrowserTab.getPointer());
    BrowserTabContent* browserTabContent = getBrowserTab.getBrowserTab();
    
    Brain* brain = NULL;
    if (model != NULL) {
        brain = model->getBrain();
    }
    
    /*
     * For a NULL model, find and use the surface model for the
     * surface and in the same tab as the volume model.  This typically
     * occurs when the volume surface outline is drawn over a volume slice.
     */
    if (model == NULL) {
        EventModelSurfaceGet surfaceGet(surface);
        EventManager::get()->sendEvent(surfaceGet.getPointer());
        surfaceModel = surfaceGet.getModelSurface();
        CaretAssert(surfaceModel);
        if (surfaceModel != NULL) {
            brain = surfaceModel->getBrain();
        }
        
        /*
         * If whole brain is displayed in the tab, use coloring
         * from whole brain instead of surface.
         */
        if (browserTabContent != NULL) {
            ModelWholeBrain* wholeBrain = browserTabContent->getDisplayedWholeBrainModel();
            if (wholeBrain != NULL) {
                wholeBrainModel = wholeBrain;
                brain = wholeBrainModel->getBrain();
                surfaceModel = NULL;
            }
            ModelSurfaceMontage* surfMont = browserTabContent->getDisplayedSurfaceMontageModel();
            if (surfMont != NULL) {
                surfaceMontageModel = surfMont;
                brain = surfaceMontageModel->getBrain();
                surfaceModel = NULL;
            }
        }
    }
    
    /*
     * Get coloring and overlays for the valid model.
     */
    if (surfaceModel != NULL) {
        rgba = surface->getSurfaceNodeColoringRgbaForBrowserTab(browserTabIndex);
        overlaySet = surfaceModel->getOverlaySet(browserTabIndex);
    }
    else if (surfaceMontageModel != NULL) {
        rgba = surface->getSurfaceMontageNodeColoringRgbaForBrowserTab(browserTabIndex);
        overlaySet = surfaceMontageModel->getOverlaySet(browserTabIndex);
    }
    else if (wholeBrainModel != NULL) {
        rgba = surface->getWholeBrainNodeColoringRgbaForBrowserTab(browserTabIndex);
        overlaySet = wholeBrainModel->getOverlaySet(browserTabIndex);
    }
    
    CaretAssert(overlaySet);
    
    /*
     * RGBA will be Non-NULL if the surface HAS valid coloring
     */
    if (rgba != NULL) {
        return rgba;
    }
    
    /*
     * Drawing type for labels
     */
    DisplayPropertiesLabels* displayPropertiesLabels = NULL;
    if (brain != NULL) {
        displayPropertiesLabels = brain->getDisplayPropertiesLabels();
    }
    
    std::array<uint8_t, 3> defaultColor { 178, 178, 178 };
    if (brain != NULL) {
        const DisplayPropertiesSurface* dsp = brain->getDisplayPropertiesSurface();
        defaultColor = dsp->getDefaultColorRGB();
    }
    
    const int numNodes = surface->getNumberOfNodes();
    const int numColorComponents = numNodes * 4;
    float *rgbaColor = new float[numColorComponents];
    
    /*
     * Color the surface nodes
     */
    this->colorSurfaceNodes(displayPropertiesLabels,
                            browserTabIndex,
                            surface,
                            defaultColor,
                            overlaySet, 
                            rgbaColor);
    
    if (surfaceModel != NULL) {
        surface->setSurfaceNodeColoringRgbaForBrowserTab(browserTabIndex,
                                                         rgbaColor);
        rgba = surface->getSurfaceNodeColoringRgbaForBrowserTab(browserTabIndex);
    }
    else if (surfaceMontageModel != NULL) {
        surface->setSurfaceMontageNodeColoringRgbaForBrowserTab(browserTabIndex,
                                                                rgbaColor);
        rgba = surface->getSurfaceMontageNodeColoringRgbaForBrowserTab(browserTabIndex);
    }
    else if (wholeBrainModel != NULL) {
        surface->setWholeBrainNodeColoringRgbaForBrowserTab(browserTabIndex, 
                                                            rgbaColor);
        rgba = surface->getWholeBrainNodeColoringRgbaForBrowserTab(browserTabIndex);
    }

    if(rgbaColor) delete [] rgbaColor;
    
    return rgba;
}

/**
 * Show brainordinate region of interest highlighting on the surface.
 *
 * @param brain
 *     The brain.
 * @param surface
 *     Surface on which highlighting is displayed.
 * @param rgbaNodeColors
 *     Node coloring that is updated with highlighting.
 */
void
SurfaceNodeColoring::showBrainordinateHighlightRegionOfInterest(const Brain* brain,
                                                                const Surface* surface,
                                                                float* rgbaNodeColors)
{
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    uint8_t foregroundColorByte[4];
    prefs->getBackgroundAndForegroundColors()->getColorForegroundSurfaceView(foregroundColorByte);
    const float foregroundColor[4] = {
        static_cast<float>(foregroundColorByte[0]) / 255.0f,
        static_cast<float>(foregroundColorByte[1]) / 255.0f,
        static_cast<float>(foregroundColorByte[2]) / 255.0f,
        1.0f
    };
    
    const BrainordinateRegionOfInterest* roi = brain->getBrainordinateHighlightRegionOfInterest();
    CaretAssert(roi);
    
    if (roi->isBrainordinateHighlightingEnabled()) {
        const StructureEnum::Enum structure = surface->getStructure();
        const int64_t surfaceNumberOfNodes = surface->getNumberOfNodes();
        
        if (roi->hasNodesForSurfaceStructure(structure, surfaceNumberOfNodes)) {
            const std::vector<int64_t>& nodeIndices = roi->getNodesForSurfaceStructure(structure,
                                                                                       surfaceNumberOfNodes);
            for (std::vector<int64_t>::const_iterator nodeIter = nodeIndices.begin();
                 nodeIter != nodeIndices.end();
                 nodeIter++) {
                const int64_t nodeIndex = *nodeIter;
                const int64_t rgbaIndex = nodeIndex * 4;
                CaretAssertArrayIndex(rgbaNodeColors, surfaceNumberOfNodes*4 , rgbaIndex + 3);
                
                rgbaNodeColors[rgbaIndex]   = foregroundColor[0];
                rgbaNodeColors[rgbaIndex+1] = foregroundColor[1];
                rgbaNodeColors[rgbaIndex+2] = foregroundColor[2];
                rgbaNodeColors[rgbaIndex+3] = foregroundColor[3];
            }
        }
    }
}

/**
 * Assign color components to surface nodes. 
 *
 * @param displayPropertiesLabels
 *    Label display properties
 * @param browserTabIndex
 *    Index of browser tab
 * @param surface
 *    Surface that has its nodes colored.
 * @param defaultSurfaceColor
 *    Default coloring for surface
 * @param overlaySet
 *    Surface overlay assignments for surface.
 * @param rgbaNodeColors
 *    RGBA color components that are set by this method.
 */
void 
SurfaceNodeColoring::colorSurfaceNodes(const DisplayPropertiesLabels* displayPropertiesLabels,
                                       const int32_t browserTabIndex,
                                       const Surface* surface,
                                       const std::array<uint8_t, 3>& defaultSurfaceColor,
                                       OverlaySet* overlaySet,
                                       float* rgbaNodeColors)
{
    const int32_t numNodes = surface->getNumberOfNodes();
    const int32_t numberOfDisplayedOverlays = overlaySet->getNumberOfDisplayedOverlays();
    
    const float defaultColorFloat[3] = {
        defaultSurfaceColor[0] / 255.0f,
        defaultSurfaceColor[1] / 255.0f,
        defaultSurfaceColor[2] / 255.0f
    };
    
    /*
     * Default color.
     */
    for (int32_t i = 0; i < numNodes; i++) {
        const int32_t i4 = i * 4;
        rgbaNodeColors[i4]   = defaultColorFloat[0];
        rgbaNodeColors[i4+1] = defaultColorFloat[1];
        rgbaNodeColors[i4+2] = defaultColorFloat[2];
        rgbaNodeColors[i4+3] = 1.0;
    }
    
    const BrainStructure* brainStructure = surface->getBrainStructure();
    CaretAssert(brainStructure);
    const Brain* brain = brainStructure->getBrain();
    CaretAssert(brain);
    
    bool firstOverlayFlag = true;
    float* overlayRGBV = new float[numNodes * 4];
    
    for (int32_t iOver = (numberOfDisplayedOverlays - 1); iOver >= 0; iOver--) {
        Overlay* overlay = overlaySet->getOverlay(iOver);
        if (overlay->isEnabled()) {            
            std::vector<CaretMappableDataFile*> mapFiles;
            CaretMappableDataFile* selectedMapFile;
            int32_t selectedMapIndex;
            
            overlay->getSelectionData(mapFiles,
                                      selectedMapFile,
                                      selectedMapIndex);
            
            DataFileTypeEnum::Enum mapDataFileType = DataFileTypeEnum::UNKNOWN;
            if (selectedMapFile != NULL) {
                mapDataFileType = selectedMapFile->getDataFileType();
            }
            
            bool isColoringValid = false;
            switch (mapDataFileType) {
                case DataFileTypeEnum::ANNOTATION:
                    break;
                case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
                    break;
                case DataFileTypeEnum::BORDER:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                                    cmf,
                                                                                    selectedMapIndex,
                                                                                    numNodes,
                                                                                    overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                                    cmf,
                                                                                    selectedMapIndex,
                                                                                    numNodes,
                                                                                    overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                    isColoringValid = this->assignCiftiDenseLabelColoring(displayPropertiesLabels,
                                                                     browserTabIndex,
                                                                     brainStructure,
                                                                          surface,
                                                                      dynamic_cast<CiftiBrainordinateLabelFile*>(selectedMapFile),
                                                                     selectedMapIndex,
                                                                      numNodes,
                                                                      overlayRGBV);
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                            cmf,
                                                                                    selectedMapIndex,
                                                                            numNodes,
                                                                            overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                    isColoringValid = this->assignCiftiScalarColoring(brainStructure,
                                                                 dynamic_cast<CiftiBrainordinateScalarFile*>(selectedMapFile),
                                                                      selectedMapIndex,
                                                                 numNodes,
                                                                 overlayRGBV);
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                    isColoringValid = this->assignCiftiDataSeriesColoring(brainStructure,
                                                                      dynamic_cast<CiftiBrainordinateDataSeriesFile*>(selectedMapFile),
                                                                          selectedMapIndex,
                                                                      numNodes,
                                                                      overlayRGBV);
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                            cmf,
                                                                                    selectedMapIndex,
                                                                            numNodes,
                                                                            overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                            cmf,
                                                                                    selectedMapIndex,
                                                                            numNodes,
                                                                            overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                {
                    CiftiParcelLabelFile* cplf = dynamic_cast<CiftiParcelLabelFile*>(selectedMapFile);
                    isColoringValid = assignCiftiParcelLabelColoring(displayPropertiesLabels,
                                                   browserTabIndex,
                                                   brainStructure,
                                                                     surface,
                                                   cplf,
                                                   selectedMapIndex,
                                                   numNodes,
                                                   overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                    isColoringValid = this->assignCiftiParcelScalarColoring(brainStructure,
                                                                            dynamic_cast<CiftiParcelScalarFile*>(selectedMapFile),
                                                                            selectedMapIndex,
                                                                            numNodes,
                                                                            overlayRGBV);
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                    isColoringValid = this->assignCiftiParcelSeriesColoring(brainStructure,
                                                                            dynamic_cast<CiftiParcelSeriesFile*>(selectedMapFile),
                                                                            selectedMapIndex,
                                                                            numNodes,
                                                                            overlayRGBV);
                    break;
                case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                    break;
                case DataFileTypeEnum::CZI_IMAGE_FILE:
                    break;
                case DataFileTypeEnum::FOCI:
                    break;
                case DataFileTypeEnum::IMAGE:
                    break;
                case DataFileTypeEnum::LABEL:
                    isColoringValid = this->assignLabelColoring(displayPropertiesLabels,
                                                                browserTabIndex,
                                                                brainStructure,
                                                                surface,
                                                                dynamic_cast<LabelFile*>(selectedMapFile),
                                                                selectedMapIndex,
                                                                numNodes, 
                                                                overlayRGBV);
                    break;
                case DataFileTypeEnum::METRIC:
                case DataFileTypeEnum::METRIC_DYNAMIC: // same as metric
                    isColoringValid = this->assignMetricColoring(brainStructure,
                                                                 dynamic_cast<MetricFile*>(selectedMapFile),
                                                                 selectedMapIndex,
                                                                 numNodes, 
                                                                 overlayRGBV);
                    break;
                case DataFileTypeEnum::PALETTE:
                    break;
                case DataFileTypeEnum::RGBA:
                    isColoringValid = this->assignRgbaColoring(brainStructure, 
                                                               dynamic_cast<RgbaFile*>(selectedMapFile),
                                                               selectedMapIndex,
                                                               numNodes, 
                                                               overlayRGBV);
                    break;
                case DataFileTypeEnum::SCENE:
                    break;
                case DataFileTypeEnum::SPECIFICATION:
                    break;
                case DataFileTypeEnum::SURFACE:
                    break;
                case DataFileTypeEnum::VOLUME:
                    break;
                case DataFileTypeEnum::VOLUME_DYNAMIC:
                    break;
                case DataFileTypeEnum::UNKNOWN:
                    break;
            }
            
            if (isColoringValid) {
                if (selectedMapFile->isMappedWithPalette()) {
                    const PaletteColorMapping* pcm = selectedMapFile->getMapPaletteColorMapping(selectedMapIndex);
                    CaretAssert(pcm);
                    bool hideDataFlag    = false;
                    bool showOutlineFlag = false;
                    const PaletteThresholdOutlineDrawingModeEnum::Enum outlineMode = pcm->getThresholdOutlineDrawingMode();
                    switch (outlineMode) {
                        case PaletteThresholdOutlineDrawingModeEnum::OFF:
                            break;
                        case PaletteThresholdOutlineDrawingModeEnum::OUTLINE:
                            hideDataFlag    = true;
                            showOutlineFlag = true;
                            break;
                        case PaletteThresholdOutlineDrawingModeEnum::OUTLINE_AND_DATA:
                            showOutlineFlag = true;
                            break;
                    }
                    
                    if (showOutlineFlag) {
                        const CaretColorEnum::Enum outlineColor = pcm->getThresholdOutlineDrawingColor();
                        float outlineRGBA[4];
                        CaretColorEnum::toRGBAFloat(outlineColor, outlineRGBA);
                        
                        CaretPointer<TopologyHelper> topologyHelper = surface->getTopologyHelper();
                        std::vector<float> rgbaCopy(numNodes * 4);
                        for (int32_t i = 0; i < (numNodes*4); i++) {
                            rgbaCopy[i] = overlayRGBV[i];
                        }
                        for (int32_t i = 0; i < numNodes; i++) {
                            const int32_t i4 = i * 4;
                            CaretAssertVectorIndex(rgbaCopy, i4 + 3);
                            const float alpha = rgbaCopy[i4 + 3];
                            if (alpha > 0.0 ) {
                                /*
                                 * If a node is the same color as all of its neighbors,
                                 * use the fill color.  Otherwise, use the outline color.
                                 */
                                bool isLabelBoundaryNode = false;
                                int32_t numNeighbors = 0;
                                const int32_t* allNeighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
                                for (int32_t n = 0; n < numNeighbors; n++) {
                                    const int32_t neighborNodeIndex = allNeighbors[n];
                                    const int32_t n4 = neighborNodeIndex * 4;
                                    CaretAssertVectorIndex(rgbaCopy, n4 + 3);
                                    const float neighborAlpha = rgbaCopy[n4 + 3];
                                    if (neighborAlpha <= 0.0) {
                                        isLabelBoundaryNode = true;
                                        break;
                                    }
                                }
                                CaretAssertArrayIndex(overlayRGBV, numNodes * 4, i4 + 3);
                                if (isLabelBoundaryNode) {
                                    overlayRGBV[i4]   = outlineRGBA[0];
                                    overlayRGBV[i4+1] = outlineRGBA[1];
                                    overlayRGBV[i4+2] = outlineRGBA[2];
                                    overlayRGBV[i4+3] = 1.0;
                                }
                                else if (hideDataFlag) {
                                    overlayRGBV[i4+3] = 0.0;
                                }
                            }
                        }
                    }
                }
            }
            
            if (isColoringValid) {
                const float opacity = overlay->getOpacity();
                const float oneMinusOpacity = 1.0 - opacity;
                
                for (int32_t i = 0; i < numNodes; i++) {
                    const int32_t i4 = i * 4;
                    const float valid = overlayRGBV[i4 + 3];
                    if (valid > 0.0 ) {
                        if (opacity < 1.0) {
                            if (firstOverlayFlag) {
                                /*
                                 * When first overlay, there is nothing to 
                                 * blend with
                                 */
                                rgbaNodeColors[i4]   = (overlayRGBV[i4]   * opacity);
                                rgbaNodeColors[i4+1] = (overlayRGBV[i4+1] * opacity);
                                rgbaNodeColors[i4+2] = (overlayRGBV[i4+2] * opacity);
                            }
                            else {
                                /*
                                 * Blend with underlaying colors
                                 */
                                rgbaNodeColors[i4]   = (overlayRGBV[i4]   * opacity)
                                + (rgbaNodeColors[i4] * oneMinusOpacity);
                                rgbaNodeColors[i4+1] = (overlayRGBV[i4+1] * opacity)
                                + (rgbaNodeColors[i4+1] * oneMinusOpacity);
                                rgbaNodeColors[i4+2] = (overlayRGBV[i4+2] * opacity)
                                + (rgbaNodeColors[i4+2] * oneMinusOpacity);
                            }
                        }
                        else {
                            /*
                             * No opacity so simple replace coloring
                             */
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
    
    /*
     * Opacity from first overlay is used as overall surface opacity
     * so replace alpha with opacity
     */
    const float opacity = brain->getDisplayPropertiesSurface()->getOpacity();
    if (opacity < 1.0) {
        for (int32_t i = 0; i < numNodes; i++) {
            const int32_t i4 = i * 4;
            rgbaNodeColors[i4+3] = opacity;
        }
    }
    
    showBrainordinateHighlightRegionOfInterest(brain,
                                               surface,
                                               rgbaNodeColors);
    
    delete[] overlayRGBV;
}

/**
 * Assign label coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param labelFile
 *    Label file that is selected.
 * @param labelMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignLabelColoring(const DisplayPropertiesLabels* displayPropertiesLabels,
                                         const int32_t browserTabIndex,
                                         const BrainStructure* /*brainStructure*/,
                                         const Surface* surface,
                                         const LabelFile* labelFile,
                                         const int32_t displayColumn,
                                         const int32_t numberOfNodes,
                                         float* rgbv)
{
    if (labelFile == NULL) {
        return false;
    }
    
    
    if ( ! labelFile->isMappableToSurfaceStructure(surface->getStructure())) {
        return false;
    }
    
    const LabelDrawingProperties* props = labelFile->getLabelDrawingProperties();
    LabelDrawingTypeEnum::Enum labelDrawingType = props->getDrawingType();
    CaretColorEnum::Enum outlineColor = props->getOutlineColor();
    
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    
    const GroupAndNameHierarchyModel* classNameModel = labelFile->getGroupAndNameHierarchyModel();
    if (classNameModel->isSelected(displayGroup, browserTabIndex) == false) {
        return false;
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
    
    const GiftiLabelTable* labelTable = labelFile->getLabelTable();
    
    CaretAssert(surface);
    CaretPointer<TopologyHelper> topologyHelper = surface->getTopologyHelper();
    
    /*
     * Assign colors from labels to nodes
     */
    std::vector<float> labelKeys;
    for (int32_t i = 0; i < numberOfNodes; i++) {
        labelKeys.push_back(labelFile->getLabelKey(i, displayColumn));
    }
    
    const bool drawMedialWallFilledFlag = props->isDrawMedialWallFilled();
    assignLabelTableColors(labelTable,
                           labelDrawingType,
                           outlineColor,
                           topologyHelper,
                           displayGroup,
                           browserTabIndex,
                           labelKeys,
                           drawMedialWallFilledFlag,
                           rgbv);

    return true;
}

/**
 * Assign label coloring to surface nodes.
 *
 * @param labelTable
 *    The label table.
 * @param labelDrawingType
 *    Label drawing type.
 * @param outlineColor
 *    Outline color.
 * @param topologyHelper
 *    The topology helper for node neighbors.
 * @param displayGroup
 *    Selected Display Group
 * @param browserTabIndex
 *    Index of browser tab.
 * @param labelIndices
 *    Indices of labels for each node.
 * @param medialWallLabelKey
 *    Index of the medial wall label key
 * @param drawMedialWallFilledFlag
 *    True if medial wall is always filled
 * @param rgbv
 *    RGB coloring. (4 per node).
 *
 */
void
SurfaceNodeColoring::assignLabelTableColors(const GiftiLabelTable* labelTable,
                                            const LabelDrawingTypeEnum::Enum labelDrawingType,
                                            const CaretColorEnum::Enum outlineColor,
                                            const CaretPointer<TopologyHelper> topologyHelper,
                                            const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t browserTabIndex,
                                            const std::vector<float>& labelIndices,
                                            const bool drawMedialWallFilledFlag,
                                            float* rgbv)
{
    const int32_t numberOfIndices = static_cast<int32_t>(labelIndices.size());
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfIndices; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    
    float outlineRGBA[4];
    CaretColorEnum::toRGBAFloat(outlineColor, outlineRGBA);
    outlineRGBA[3] = 1.0;
    
    /*
     * Assign colors from labels to nodes
     */
    float nodeRGBA[4];
    for (int32_t i = 0; i < numberOfIndices; i++) {
        CaretAssertVectorIndex(labelIndices, i);
        const int32_t labelKey= static_cast<int32_t>(labelIndices[i]);
        const GiftiLabel* label = labelTable->getLabel(labelKey);
        if (label == NULL) {
            continue;
        }
        
        const GroupAndNameHierarchyItem* nameItem = label->getGroupNameSelectionItem();
        if (nameItem != NULL) {
            if (nameItem->isSelected(displayGroup,
                                     browserTabIndex) == false) {
                continue;
            }
        }
        
        /*
         * Initialize node color to its label's color
         */
        label->getColor(nodeRGBA);
        if (nodeRGBA[3] <= 0.0) {
            continue;
        }
        
        /*
         * If a node is the same color as all of its neighbors,
         * use the fill color.  Otherwise, use the outline color.
         */
        bool isLabelBoundaryNode = false;
        int32_t numNeighbors = 0;
        const int32_t* allNeighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
        for (int32_t n = 0; n < numNeighbors; n++) {
            const int32_t neighborNodeIndex = allNeighbors[n];
            CaretAssertVectorIndex(labelIndices, neighborNodeIndex);
            const int32_t neighborLabelKey = static_cast<int32_t>(labelIndices[neighborNodeIndex]);
            if (labelKey != neighborLabelKey) {
                isLabelBoundaryNode = true;
                break;
            }
        }
        
        /*
         * User may request that medial wall is always filled
         * and never receives outlining
         */
        bool doOutlineFlag = true;
        if (drawMedialWallFilledFlag) {
            if (label->isMedialWallName()) {
                doOutlineFlag = false;
            }
        }
        
        if (doOutlineFlag) {
            switch (labelDrawingType) {
                case LabelDrawingTypeEnum::DRAW_FILLED:
                    break;
                case LabelDrawingTypeEnum::DRAW_FILLED_WITH_OUTLINE_COLOR:
                    if (isLabelBoundaryNode) {
                        nodeRGBA[0] = outlineRGBA[0];
                        nodeRGBA[1] = outlineRGBA[1];
                        nodeRGBA[2] = outlineRGBA[2];
                        nodeRGBA[3] = outlineRGBA[3];
                    }
                    break;
                case LabelDrawingTypeEnum::DRAW_OUTLINE_COLOR:
                    if (isLabelBoundaryNode) {
                        nodeRGBA[0] = outlineRGBA[0];
                        nodeRGBA[1] = outlineRGBA[1];
                        nodeRGBA[2] = outlineRGBA[2];
                        nodeRGBA[3] = outlineRGBA[3];
                    }
                    else {
                        nodeRGBA[3] = 0.0;
                    }
                    break;
                case LabelDrawingTypeEnum::DRAW_OUTLINE_LABEL_COLOR:
                    if ( ! isLabelBoundaryNode) {
                        nodeRGBA[3] = 0.0;
                    }
                    break;
            }
        }
        
        const int32_t i4 = i * 4;
        rgbv[i4]   = nodeRGBA[0];
        rgbv[i4+1] = nodeRGBA[1];
        rgbv[i4+2] = nodeRGBA[2];
        rgbv[i4+3] = nodeRGBA[3];
    }
}

/**
 * Assign metric coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param metricFile
 *    Metric file that is selected.
 * @param metricMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignMetricColoring(const BrainStructure* brainStructure,
                                          MetricFile* metricFile,
                                          const int32_t displayColumn,
                                          const int32_t numberOfNodes,
                                          float* rgbv)
{
    if (displayColumn < 0) {
        return false;
    }
    if ( ! metricFile->isMappableToSurfaceStructure(brainStructure->getStructure())) {
        return false;
    }
    
    PaletteColorMapping* paletteColorMapping = metricFile->getPaletteColorMapping(displayColumn);
    
    bool useThreshMapFileFlag = false;
    switch (paletteColorMapping->getThresholdType()) {
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_FILE:
            useThreshMapFileFlag = true;
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_MAPPED_AVERAGE_AREA:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_NORMAL:
            break;
        case PaletteThresholdTypeEnum::THRESHOLD_TYPE_OFF:
            break;
    }
    
    const float* metricDisplayData = metricFile->getValuePointerForColumn(displayColumn);
    float* metricThresholdData = const_cast<float*>(metricDisplayData);
    PaletteColorMapping* thresholdPaletteColorMapping = paletteColorMapping;
    
    if (useThreshMapFileFlag) {
        const CaretMappableDataFileAndMapSelectionModel* threshFileModel = metricFile->getMapThresholdFileSelectionModel(displayColumn);
        CaretAssert(threshFileModel);
        const CaretMappableDataFile* threshMapFile = threshFileModel->getSelectedFile();
        if (threshMapFile != NULL) {
            const MetricFile* threshMetricFile = dynamic_cast<const MetricFile*>(threshMapFile);
            if (threshMetricFile != NULL) {
                const int32_t threshMapIndex = threshFileModel->getSelectedMapIndex();
                if ((threshMapIndex >= 0)
                    && (threshMapIndex < threshMapFile->getNumberOfMaps())) {
                    metricThresholdData = const_cast<float*>(threshMetricFile->getValuePointerForColumn(threshMapIndex));
                    thresholdPaletteColorMapping = const_cast<PaletteColorMapping*>(threshMapFile->getMapPaletteColorMapping(threshMapIndex));
                    CaretAssert(thresholdPaletteColorMapping);
                }
            }
        }
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    /*
     * Get min/max ranges.
     */
    int thresholdColumn = metricFile->getColumnIndexFromColumnName(paletteColorMapping->getThresholdDataName());
    if (thresholdColumn < 0) {
        thresholdColumn = displayColumn;
    }
    
    
    FastStatistics* statistics = NULL;
    switch (metricFile->getPaletteNormalizationMode()) {
        case PaletteNormalizationModeEnum::NORMALIZATION_ALL_MAP_DATA:
            statistics = const_cast<FastStatistics*>(metricFile->getFileFastStatistics());
            break;
        case PaletteNormalizationModeEnum::NORMALIZATION_SELECTED_MAP_DATA:
            statistics = const_cast<FastStatistics*>(metricFile->getMapFastStatistics(displayColumn));
            break;
    }
    CaretAssert(statistics);
    
    if (statistics != NULL) {
        NodeAndVoxelColoring::colorScalarsWithPalette(statistics, 
                                                      paletteColorMapping, 
                                                      metricDisplayData,
                                                      thresholdPaletteColorMapping,
                                                      metricThresholdData, 
                                                      numberOfNodes, 
                                                      rgbv);
    }
    
    return true;
}

/**
 * Assign cifti scalar coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param ciftiScalarFile
 *    Cifti Scalar file that is selected.
 * @param ciftiMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignCiftiMappableConnectivityMatrixColoring(const BrainStructure* brainStructure,
                                                           CiftiMappableConnectivityMatrixDataFile* ciftiConnectivityMatrixFile,
                                                                   const int32_t mapIndex,
                                                           const int32_t numberOfNodes,
                                                           float* rgbv)
{
    CaretAssert(ciftiConnectivityMatrixFile);
    
    if (mapIndex < 0) {
        return false;
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    const StructureEnum::Enum structure = brainStructure->getStructure();
    std::vector<float> dataValues(numberOfNodes);
    
    ciftiConnectivityMatrixFile->getMapSurfaceNodeColoring(mapIndex,
                                                           structure,
                                                           rgbv,
                                                           &dataValues[0],
                                                           numberOfNodes);


	
	CiftiConnectivityMatrixParcelFile* parcelFile = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(ciftiConnectivityMatrixFile);
    if (parcelFile != NULL) {
        const Surface* surface = brainStructure->getPrimaryAnatomicalSurface();
        CaretPointer<TopologyHelper> topologyHelper = surface->getTopologyHelper();
        
        std::set<int64_t> nodeSet;
        bool selectedParcelValid = false;
        selectedParcelValid = ciftiConnectivityMatrixFile->getParcelNodesElementForSelectedParcel(nodeSet,structure);
        
        std::vector<int64_t> selectedParcelNodes = std::vector<int64_t>(nodeSet.begin(), nodeSet.end());
        
        if(selectedParcelValid)	{
            const CaretColorEnum::Enum parcelColor = parcelFile->getSelectedParcelColor();
            const float* rgb = CaretColorEnum::toRGB(parcelColor);
            CaretAssert(rgb);
            
            switch (parcelFile->getSelectedParcelColoringMode()) {
                case CiftiParcelColoringModeEnum::CIFTI_PARCEL_COLORING_OFF:
                    break;
                case CiftiParcelColoringModeEnum::CIFTI_PARCEL_COLORING_FILL:
                {
                    for(uint64_t pNode = 0; pNode < selectedParcelNodes.size(); pNode++) {
                        int64_t nodeIndex = selectedParcelNodes[pNode];
                        if(nodeIndex >= 0 && nodeIndex < numberOfNodes) {
                            uint64_t node4 = nodeIndex*4;
                            rgbv[node4]   =  rgb[0];
                            rgbv[node4+1] =  rgb[1];
                            rgbv[node4+2] =  rgb[2];
                            rgbv[node4+3] =  1.0;
                            continue;
                        }
                    }
                }
                    break;
                case CiftiParcelColoringModeEnum::CIFTI_PARCEL_COLORING_OUTLINE:
                {
                    /*
                     * Check for any neighbors with different label key.
                     */
                    
                    //make a quick lookup table
                    std::vector<int64_t> selectedNodesLookup(numberOfNodes,0);
                    
                    for(uint64_t pNode = 0;pNode < selectedParcelNodes.size();pNode++)
                    {
                        int64_t nodeIndex = selectedParcelNodes[pNode];
                        if(nodeIndex >= 0 && nodeIndex < (int64_t)selectedNodesLookup.size()) {
                            selectedNodesLookup[nodeIndex] = 1;
                        }
                    }
                    
                    for(uint64_t pNode = 0;pNode < selectedParcelNodes.size();pNode++)
                    {
                        int32_t numNeighbors = 0;
                        
                        const int64_t nodeIndex = selectedParcelNodes[pNode];
                        const int32_t* allNeighbors = topologyHelper->getNodeNeighbors(nodeIndex, numNeighbors);
                        for (int32_t n = 0; n < numNeighbors; n++) {
                            const int32_t neighbor = allNeighbors[n];
                            if (!selectedNodesLookup[neighbor]) {
                                int64_t node4 = nodeIndex*4;
                                rgbv[node4]   = rgb[0];
                                rgbv[node4+1] = rgb[1];
                                rgbv[node4+2] = rgb[2];
                                rgbv[node4+3] = 1.0;
                                continue;
                            }
                        }
                    }
                }
                    break;
            }
        }
	}
	   
    return true;
    
}

/**
 * Assign cifti dense label coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param ciftiLabelFile
 *    Cifti Label file that is selected.
 * @param ciftiMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignCiftiDenseLabelColoring(const DisplayPropertiesLabels* displayPropertiesLabels,
                                              const int32_t browserTabIndex,
                                              const BrainStructure* brainStructure,
                                                   const Surface* surface,
                                          CiftiBrainordinateLabelFile* ciftiLabelFile,
                                              const int32_t mapIndex,
                                          const int32_t numberOfNodes,
                                          float* rgbv)
{
    CaretAssert(displayPropertiesLabels);
    CaretAssert(brainStructure);
    CaretAssert(ciftiLabelFile);
    CaretAssert(rgbv);
    
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiBrainordinateLabelFile*> allCiftiBrainordinateLabelFiles;
    brain->getConnectivityDenseLabelFiles(allCiftiBrainordinateLabelFiles);
    
    if (mapIndex < 0) {
        return false;
    }
    
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    const LabelDrawingProperties* props = ciftiLabelFile->getLabelDrawingProperties();
    LabelDrawingTypeEnum::Enum labelDrawingType = props->getDrawingType();
    CaretColorEnum::Enum outlineColor = props->getOutlineColor();
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    /*
     * Update coloring
     */
    if (ciftiLabelFile->isMapColoringValid(mapIndex) == false) {
        ciftiLabelFile->updateScalarColoringForMap(mapIndex);
    }
    
    std::vector<float> dataValues(numberOfNodes);
    
    /*
     * Assigns colors for all nodes.
     */
    const StructureEnum::Enum structure = brainStructure->getStructure();
    ciftiLabelFile->getMapSurfaceNodeColoring(mapIndex,
                                              structure,
                                              rgbv,
                                              &dataValues[0],
                                              numberOfNodes);
    
    CaretAssert(surface);
    CaretPointer<TopologyHelper> topologyHelper = surface->getTopologyHelper();

    /*
     * All nodes are colored.  Remove coloring for nodes whose
     * label is not selected.
     */
    GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
    CaretAssert(labelTable);
    
    const bool drawMedialWallFilledFlag = props->isDrawMedialWallFilled();
    assignLabelTableColors(labelTable,
                           labelDrawingType,
                           outlineColor,
                           topologyHelper,
                           displayGroup,
                           browserTabIndex,
                           dataValues,
                           drawMedialWallFilledFlag,
                           rgbv);
    
    return true;
}

/**
 * Assign cifti parcel label coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param ciftiParcelLabelFile
 *    Cifti Parcel Label file that is selected.
 * @param mapIndex
 *    Index of selected map.
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
SurfaceNodeColoring::assignCiftiParcelLabelColoring(const DisplayPropertiesLabels* displayPropertiesLabels,
                                                   const int32_t browserTabIndex,
                                                   const BrainStructure* brainStructure,
                                                    const Surface* surface,
                                                   CiftiParcelLabelFile* ciftiParcelLabelFile,
                                                   const int32_t mapIndex,
                                                   const int32_t numberOfNodes,
                                                   float* rgbv)
{
    CaretAssert(displayPropertiesLabels);
    CaretAssert(brainStructure);
    CaretAssert(ciftiParcelLabelFile);
    CaretAssert(rgbv);
    
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiBrainordinateLabelFile*> allCiftiBrainordinateLabelFiles;
    brain->getConnectivityDenseLabelFiles(allCiftiBrainordinateLabelFiles);
    
    if (mapIndex < 0) {
        return false;
    }
    
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    const LabelDrawingProperties* props = ciftiParcelLabelFile->getLabelDrawingProperties();
    LabelDrawingTypeEnum::Enum labelDrawingType = props->getDrawingType();
    CaretColorEnum::Enum outlineColor = props->getOutlineColor();
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    /*
     * Update coloring
     */
    if (ciftiParcelLabelFile->isMapColoringValid(mapIndex) == false) {
        ciftiParcelLabelFile->updateScalarColoringForMap(mapIndex);
    }
    
    std::vector<float> dataValues(numberOfNodes);
    
    /*
     * Assigns colors for all nodes.
     */
    const StructureEnum::Enum structure = brainStructure->getStructure();
    ciftiParcelLabelFile->getMapSurfaceNodeColoring(mapIndex,
                                              structure,
                                              rgbv,
                                              &dataValues[0],
                                              numberOfNodes);
    
    CaretAssert(surface);
    CaretPointer<TopologyHelper> topologyHelper = surface->getTopologyHelper();
    
    /*
     * All nodes are colored.  Remove coloring for nodes whose
     * label is not selected.
     */
    GiftiLabelTable* labelTable = ciftiParcelLabelFile->getMapLabelTable(mapIndex);
    CaretAssert(labelTable);
    
    const bool drawMedialWallFilledFlag = props->isDrawMedialWallFilled();
    assignLabelTableColors(labelTable,
                           labelDrawingType,
                           outlineColor,
                           topologyHelper,
                           displayGroup,
                           browserTabIndex,
                           dataValues,
                           drawMedialWallFilledFlag,
                           rgbv);
    return true;
}

/**
 * Assign cifti scalar coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param ciftiScalarFile
 *    Cifti Scalar file that is selected.
 * @param ciftiMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignCiftiScalarColoring(const BrainStructure* brainStructure,
                                               CiftiBrainordinateScalarFile* ciftiScalarFile,
                                               const int32_t mapIndex,
                                               const int32_t numberOfNodes,
                                               float* rgbv)
{
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiBrainordinateScalarFile*> allCiftiBrainordinateScalarFiles;
    brain->getConnectivityDenseScalarFiles(allCiftiBrainordinateScalarFiles);
    
    if (mapIndex < 0) {
        return false;
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    /*
     * Update coloring
     */
    if (ciftiScalarFile->isMapColoringValid(mapIndex) == false) {
        ciftiScalarFile->updateScalarColoringForMap(mapIndex);
    }
    
    std::vector<float> dataValues(numberOfNodes);
    const StructureEnum::Enum structure = brainStructure->getStructure();
    ciftiScalarFile->getMapSurfaceNodeColoring(mapIndex,
                                               structure,
                                               rgbv,
                                               &dataValues[0],
                                               numberOfNodes);
    return true;
}

/**
 * Assign cifti parcel scalar coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param ciftiScalarFile
 *    Cifti Scalar file that is selected.
 * @param ciftiMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignCiftiParcelScalarColoring(const BrainStructure* brainStructure,
                                               CiftiParcelScalarFile* ciftiParcelScalarFile,
                                                     const int32_t mapIndex,
                                               const int32_t numberOfNodes,
                                               float* rgbv)
{
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiParcelScalarFile*> allCiftiParcelScalarFiles;
    brain->getConnectivityParcelScalarFiles(allCiftiParcelScalarFiles);
    
    if (mapIndex < 0) {
        return false;
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    /*
     * Update coloring
     */
    if (ciftiParcelScalarFile->isMapColoringValid(mapIndex) == false) {
        ciftiParcelScalarFile->updateScalarColoringForMap(mapIndex);
    }
    
    std::vector<float> dataValues(numberOfNodes);
    const StructureEnum::Enum structure = brainStructure->getStructure();
    ciftiParcelScalarFile->getMapSurfaceNodeColoring(mapIndex,
                                               structure,
                                               rgbv,
                                               &dataValues[0],
                                               numberOfNodes);
    return true;
}

/**
 * Assign cifti scalar coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param ciftiDataSeriesFile
 *    Cifti Data Series file that is selected.
 * @param ciftiMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignCiftiDataSeriesColoring(const BrainStructure* brainStructure,
                                               CiftiBrainordinateDataSeriesFile* ciftiDataSeriesFile,
                                                   const int32_t mapIndex,
                                               const int32_t numberOfNodes,
                                               float* rgbv)
{
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiBrainordinateDataSeriesFile*> allCiftiDataSeriesFiles;
    brain->getConnectivityDataSeriesFiles(allCiftiDataSeriesFiles);
    
    if (mapIndex < 0) {
        return false;
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    /*
     * Update coloring
     */
    if (ciftiDataSeriesFile->isMapColoringValid(mapIndex) == false) {
        ciftiDataSeriesFile->updateScalarColoringForMap(mapIndex);
    }

    /*
     * Get Coloring
     */
    std::vector<float> dataValues(numberOfNodes);
    const StructureEnum::Enum structure = brainStructure->getStructure();
    ciftiDataSeriesFile->getMapSurfaceNodeColoring(mapIndex,
                                               structure,
                                               rgbv,
                                               &dataValues[0],
                                               numberOfNodes);
    return true;
}

/**
 * Assign cifti parcel series coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param ciftiParcelSeriesFile
 *    Cifti Parcel Series file that is selected.
 * @param ciftiMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignCiftiParcelSeriesColoring(const BrainStructure* brainStructure,
                                                   CiftiParcelSeriesFile* ciftiParcelSeriesFile,
                                                     const int32_t mapIndex,
                                                   const int32_t numberOfNodes,
                                                   float* rgbv)
{
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiParcelSeriesFile*> allCiftiParcelSeriesFiles;
    brain->getConnectivityParcelSeriesFiles(allCiftiParcelSeriesFiles);
    
    if (mapIndex < 0) {
        return false;
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    /*
     * Update coloring
     */
    if (ciftiParcelSeriesFile->isMapColoringValid(mapIndex) == false) {
        ciftiParcelSeriesFile->updateScalarColoringForMap(mapIndex);
    }
    
    /*
     * Get Coloring
     */
    std::vector<float> dataValues(numberOfNodes);
    const StructureEnum::Enum structure = brainStructure->getStructure();
    ciftiParcelSeriesFile->getMapSurfaceNodeColoring(mapIndex,
                                                   structure,
                                                   rgbv,
                                                   &dataValues[0],
                                                   numberOfNodes);
    return true;
}

/**
 * Assign RGBA coloring to nodes
 * @param brainStructure
 *    The brain structure that contains the data files.
 * @param rgbaFile
 *    RGBA file that is selected.
 * @param metricMapUniqueID
 *    UniqueID of selected map.
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
SurfaceNodeColoring::assignRgbaColoring(const BrainStructure* brainStructure,
                                        const RgbaFile* rgbaFile,
                                        const int32_t /*mapIndex*/,
                                        const int32_t numberOfNodes,
                                        float* rgbv)
{
    if ( ! rgbaFile->isMappableToSurfaceStructure(brainStructure->getStructure())) {
        return false;
    }
    
    if (rgbaFile->isEmpty()) {
        return false;
    }
    
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t i4 = i * 4;
        
        rgbaFile->getVertexRGBA(i, &rgbv[i4]);
    }
    
    return true;
}

