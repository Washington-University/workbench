
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
#include "BrowserTabContent.h"
#include "EventBrowserTabGet.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "GroupAndNameHierarchyModel.h"
#include "DisplayPropertiesLabels.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GroupAndNameHierarchyGroup.h"
#include "LabelFile.h"
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
#include "PaletteFile.h"
#include "PaletteScalarAndColor.h"
#include "RgbaFile.h"
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
 * @param modelDisplayController
 *     Model controller that is displayed.  If NULL use find ModelSurface
 *     for the surface.  This case occurs when needing surface coloring 
 *     when surface outline is drawn on volume slices.
 * @param surface
 *     Surface that is displayed.
 * @param browserTabIndex
 *     Index of tab in which model is displayed.
 */
float* 
SurfaceNodeColoring::colorSurfaceNodes(Model* modelDisplayController,
                                       Surface* surface,
                                       const int32_t browserTabIndex)
{
//    CaretAssert(modelDisplayController);
    CaretAssert(surface);

    ModelSurface* surfaceController = dynamic_cast<ModelSurface*>(modelDisplayController);
    ModelSurfaceMontage* surfaceMontageController = dynamic_cast<ModelSurfaceMontage*>(modelDisplayController);
//    ModelVolume* volumeController = dynamic_cast<ModelVolume*>(modelDisplayController);
    ModelWholeBrain* wholeBrainController = dynamic_cast<ModelWholeBrain*>(modelDisplayController);
    
    OverlaySet* overlaySet = NULL;
    float* rgba = NULL;

    EventBrowserTabGet getBrowserTab(browserTabIndex);
    EventManager::get()->sendEvent(getBrowserTab.getPointer());
    BrowserTabContent* browserTabContent = getBrowserTab.getBrowserTab();
    
    Brain* brain = NULL;
    if (modelDisplayController != NULL) {
        brain = modelDisplayController->getBrain();
    }
    
    /*
     * For a NULL controller, find and use the surface controller for the
     * surface and in the same tab as the volume controller.  This typically 
     * occurs when the volume surface outline is drawn over a volume slice.
     */
    if (modelDisplayController == NULL) {
        EventModelSurfaceGet surfaceGet(surface);
        EventManager::get()->sendEvent(surfaceGet.getPointer());
        surfaceController = surfaceGet.getModelSurface();
        CaretAssert(surfaceController);
        if (surfaceController != NULL) {
            brain = surfaceController->getBrain();
        }
        
        /*
         * If whole brain is displayed in the tab, use coloring
         * from whole brain instead of surface.
         */
        if (browserTabContent != NULL) {
            ModelWholeBrain* wholeBrain = browserTabContent->getDisplayedWholeBrainModel();
            if (wholeBrain != NULL) {
                wholeBrainController = wholeBrain;
                brain = wholeBrainController->getBrain();
                surfaceController = NULL;
            }
            ModelSurfaceMontage* surfMont = browserTabContent->getDisplayedSurfaceMontageModel();
            if (surfMont != NULL) {
                surfaceMontageController = surfMont;
                brain = surfaceMontageController->getBrain();
                surfaceController = NULL;
            }
        }
    }
    
    /*
     * Get coloring and overlays for the valid controller.
     */
    if (surfaceController != NULL) {
        rgba = surface->getSurfaceNodeColoringRgbaForBrowserTab(browserTabIndex);
        overlaySet = surfaceController->getOverlaySet(browserTabIndex);
    }
    else if (surfaceMontageController != NULL) {
        rgba = surface->getSurfaceMontageNodeColoringRgbaForBrowserTab(browserTabIndex);
        overlaySet = surfaceMontageController->getOverlaySet(browserTabIndex);
    }
    else if (wholeBrainController != NULL) {
        rgba = surface->getWholeBrainNodeColoringRgbaForBrowserTab(browserTabIndex);
        overlaySet = wholeBrainController->getOverlaySet(browserTabIndex);
    }
//    else if (volumeController != NULL) {
//        // nothing since surfaceController enabled above
//    }
//    else {
//        
//        //CaretAssertMessage(0, "Unknown controller type: " + modelDisplayController->getNameForGUI(false));
//    }
    
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
//    if (browserTabContent != NULL) {
//        if (modelDisplayController != NULL) {
//            Brain* brain = modelDisplayController->getBrain();
//            if (brain != NULL) {
//                displayPropertiesLabels = brain->getDisplayPropertiesLabels();
//            }
//        }
//    }
    
    const int numNodes = surface->getNumberOfNodes();
    const int numColorComponents = numNodes * 4;
    float *rgbaColor = new float[numColorComponents];
    
    /*
     * Color the surface nodes
     */
    this->colorSurfaceNodes(displayPropertiesLabels,
                            browserTabIndex,
                            surface,
                            overlaySet, 
                            rgbaColor);
    
    if (surfaceController != NULL) {
        surface->setSurfaceNodeColoringRgbaForBrowserTab(browserTabIndex,
                                                         rgbaColor);
        rgba = surface->getSurfaceNodeColoringRgbaForBrowserTab(browserTabIndex);
    }
    else if (surfaceMontageController != NULL) {
        surface->setSurfaceMontageNodeColoringRgbaForBrowserTab(browserTabIndex,
                                                                rgbaColor);
        rgba = surface->getSurfaceMontageNodeColoringRgbaForBrowserTab(browserTabIndex);
    }
    else if (wholeBrainController != NULL) {
        surface->setWholeBrainNodeColoringRgbaForBrowserTab(browserTabIndex, 
                                                            rgbaColor);
        rgba = surface->getWholeBrainNodeColoringRgbaForBrowserTab(browserTabIndex);
    }

    if(rgbaColor) delete [] rgbaColor;
    
    return rgba;
}

/**
 * Assign color components to surface nodes. 
 *
 * @param surface
 *    Surface that has its nodes colored.
 * @param overlaySet
 *    Surface overlay assignments for surface.
 * @param rgbaNodeColors
 *    RGBA color components that are set by this method.
 */
void 
SurfaceNodeColoring::colorSurfaceNodes(const DisplayPropertiesLabels* displayPropertiesLabels,
                                       const int32_t browserTabIndex,
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
    
    const BrainStructure* brainStructure = surface->getBrainStructure();
    CaretAssert(brainStructure);
    
    bool firstOverlayFlag = true;
    float* overlayRGBV = new float[numNodes * 4];
    for (int32_t iOver = (numberOfDisplayedOverlays - 1); iOver >= 0; iOver--) {
        Overlay* overlay = overlaySet->getOverlay(iOver);
        if (overlay->isEnabled()) {
            std::vector<CaretMappableDataFile*> mapFiles;
            CaretMappableDataFile* selectedMapFile;
            AString selectedMapUniqueID;
            int32_t selectedMapIndex;
            
            overlay->getSelectionData(mapFiles,
                                      selectedMapFile,
                                      selectedMapUniqueID,
                                      selectedMapIndex);
            
            DataFileTypeEnum::Enum mapDataFileType = DataFileTypeEnum::UNKNOWN;
            if (selectedMapFile != NULL) {
                mapDataFileType = selectedMapFile->getDataFileType();
            }
            
            bool isColoringValid = false;
            switch (mapDataFileType) {
                case DataFileTypeEnum::BORDER:
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                                    cmf,
                                                                                    selectedMapUniqueID,
                                                                                    numNodes,
                                                                                    overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                    isColoringValid = this->assignCiftiLabelColoring(displayPropertiesLabels,
                                                                     browserTabIndex,
                                                                     brainStructure,
                                                                      dynamic_cast<CiftiBrainordinateLabelFile*>(selectedMapFile),
                                                                      selectedMapUniqueID,
                                                                      numNodes,
                                                                      overlayRGBV);
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                            cmf,
                                                                            selectedMapUniqueID,
                                                                            numNodes,
                                                                            overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                    isColoringValid = this->assignCiftiScalarColoring(brainStructure,
                                                                 dynamic_cast<CiftiBrainordinateScalarFile*>(selectedMapFile),
                                                                 selectedMapUniqueID,
                                                                 numNodes,
                                                                 overlayRGBV);
                    break;
                case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                    isColoringValid = this->assignCiftiDataSeriesColoring(brainStructure,
                                                                      dynamic_cast<CiftiBrainordinateDataSeriesFile*>(selectedMapFile),
                                                                      selectedMapUniqueID,
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
                                                                            selectedMapUniqueID,
                                                                            numNodes,
                                                                            overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                {
                    CiftiMappableConnectivityMatrixDataFile* cmf = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(selectedMapFile);
                    isColoringValid = assignCiftiMappableConnectivityMatrixColoring(brainStructure,
                                                                            cmf,
                                                                            selectedMapUniqueID,
                                                                            numNodes,
                                                                            overlayRGBV);
                }
                    break;
                case DataFileTypeEnum::FOCI:
                    break;
                case DataFileTypeEnum::LABEL:
                    isColoringValid = this->assignLabelColoring(displayPropertiesLabels,
                                                                browserTabIndex,
                                                                brainStructure,
                                                                surface,
                                                                dynamic_cast<LabelFile*>(selectedMapFile),
                                                                selectedMapUniqueID, 
                                                                numNodes, 
                                                                overlayRGBV);
                    break;
                case DataFileTypeEnum::METRIC:
                    isColoringValid = this->assignMetricColoring(brainStructure, 
                                                                 dynamic_cast<MetricFile*>(selectedMapFile),
                                                                 selectedMapUniqueID, 
                                                                 numNodes, 
                                                                 overlayRGBV);
                    break;
                case DataFileTypeEnum::PALETTE:
                    break;
                case DataFileTypeEnum::RGBA:
                    isColoringValid = this->assignRgbaColoring(brainStructure, 
                                                               dynamic_cast<RgbaFile*>(selectedMapFile),
                                                               selectedMapUniqueID, 
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
                case DataFileTypeEnum::UNKNOWN:
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
                                         const BrainStructure* brainStructure,
                                         const Surface* surface,
                                         const LabelFile* labelFile,
                                         const AString& labelMapUniqueID,
                                         const int32_t numberOfNodes,
                                         float* rgbv)
{
    if (labelFile == NULL) {
        return false;
    }
    
    std::vector<LabelFile*> allLabelFiles;
    brainStructure->getLabelFiles(allLabelFiles);
    
    int32_t displayColumn = -1;
    for (std::vector<LabelFile*>::iterator iter = allLabelFiles.begin();
         iter != allLabelFiles.end();
         iter++) {
        LabelFile* lf = *iter;
        if (lf == labelFile) {
            displayColumn = lf->getMapIndexFromUniqueID(labelMapUniqueID);
            if (displayColumn >= 0) {
                break;
            }
        }
    }
    
    const DisplayPropertiesLabels* dpl = brainStructure->getBrain()->getDisplayPropertiesLabels();
    
    DisplayGroupEnum::Enum displayGroup = dpl->getDisplayGroupForTab(browserTabIndex);
    LabelDrawingTypeEnum::Enum labelDrawingType = LabelDrawingTypeEnum::DRAW_FILLED;
    if (displayPropertiesLabels != NULL) {
        displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
        labelDrawingType = displayPropertiesLabels->getDrawingType(displayGroup,
                                           browserTabIndex);
    }
    
    
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
    
    CaretPointer<TopologyHelper> topologyHelper = surface->getTopologyHelper();
    
    /*
     * Assign colors from labels to nodes
     */
    float labelRGBA[4];
    for (int32_t i = 0; i < numberOfNodes; i++) {
        const int32_t labelKey= labelFile->getLabelKey(i, displayColumn);
        const GiftiLabel* label = labelTable->getLabel(labelKey);
        if (label == NULL) {
            continue;
        }
        
        const GroupAndNameHierarchyItem* nameItem = label->getGroupNameSelectionItem();
        if (nameItem != NULL) {
            if (nameItem->isSelected(displayGroup, browserTabIndex) == false) {
                continue;
            }
        }
        
        bool colorIt = false;
        switch (labelDrawingType) {
            case LabelDrawingTypeEnum::DRAW_FILLED:
                colorIt = true;
                break;
            case LabelDrawingTypeEnum::DRAW_OUTLINE:
            {
                /*
                 * Check for any neighbors with different label key.
                 */
                int32_t numNeighbors = 0;
                const int32_t* allNeighbors = topologyHelper->getNodeNeighbors(i, numNeighbors);
                for (int32_t n = 0; n < numNeighbors; n++) {
                    const int32_t neighbor = allNeighbors[n];
                    if (labelKey != labelFile->getLabelKey(neighbor, displayColumn)) {
                        colorIt = true;
                        break;
                    }
                }
            }
        }
        
        if (colorIt) {
            label->getColor(labelRGBA);
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
                                          const AString& metricMapUniqueID,
                                          const int32_t numberOfNodes,
                                          float* rgbv)
{
    std::vector<MetricFile*> allMetricFiles;
    brainStructure->getMetricFiles(allMetricFiles);
    
    int32_t displayColumn = -1;
    for (std::vector<MetricFile*>::iterator iter = allMetricFiles.begin();
         iter != allMetricFiles.end();
         iter++) {
        MetricFile* mf = *iter;
        if (mf == metricFile) {
            displayColumn = mf->getMapIndexFromUniqueID(metricMapUniqueID);
            if (displayColumn >= 0) {
                break;
            }
        }
    }
    
    if (displayColumn < 0) {
        return false;
    }
    
    const PaletteColorMapping* paletteColorMapping = metricFile->getPaletteColorMapping(displayColumn);
    
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
    const float* metricDisplayData = metricFile->getValuePointerForColumn(displayColumn);
    const float* metricThresholdData = metricFile->getValuePointerForColumn(thresholdColumn);
    
    const DescriptiveStatistics* statistics = metricFile->getMapStatistics(displayColumn);
    
    const Brain* brain = brainStructure->getBrain();
    const AString paletteName = paletteColorMapping->getSelectedPaletteName();
    const Palette* palette = brain->getPaletteFile()->getPaletteByName(paletteName);
    if (palette != NULL) {
        
        //std::vector<float> rgbaColorsVector(numberOfNodes * 4);
        //float* rgbaColors = &rgbaColorsVector[0];
        
        NodeAndVoxelColoring::colorScalarsWithPalette(statistics, 
                                                      paletteColorMapping, 
                                                      palette, 
                                                      metricDisplayData, 
                                                      metricThresholdData, 
                                                      numberOfNodes, 
                                                      rgbv);
    }
    else {
        CaretLogSevere("Selected palette for metric is invalid: \"" + paletteName + "\"");
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
                                                           const AString& selectedMapUniqueID,
                                                           const int32_t numberOfNodes,
                                                           float* rgbv)
{
    CaretAssert(ciftiConnectivityMatrixFile);
    
    const int32_t mapIndex = ciftiConnectivityMatrixFile->getMapIndexFromUniqueID(selectedMapUniqueID);
    if (mapIndex < 0) {
        return false;
    }
    
    /*
     * Invalidate all coloring.
     */
    for (int32_t i = 0; i < numberOfNodes; i++) {
        rgbv[i*4+3] = 0.0;
    }
    
    //    const PaletteColorMapping* paletteColorMapping = ciftiConnectivityMatrixFile->getMapPaletteColorMapping(mapIndex);
    //    const AString paletteName = paletteColorMapping->getSelectedPaletteName();
    //    const Palette* palette = brain->getPaletteFile()->getPaletteByName(paletteName);
    
    const StructureEnum::Enum structure = brainStructure->getStructure();
    std::vector<float> dataValues(numberOfNodes);
    ciftiConnectivityMatrixFile->getMapSurfaceNodeColoring(mapIndex,
                                                           structure,
                                                           rgbv,
                                                           &dataValues[0],
                                                           numberOfNodes);
    return true;
    
}

/**
 * Assign cifti label coloring to nodes
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
SurfaceNodeColoring::assignCiftiLabelColoring(const DisplayPropertiesLabels* displayPropertiesLabels,
                                              const int32_t browserTabIndex,
                                              const BrainStructure* brainStructure,
                                          CiftiBrainordinateLabelFile* ciftiLabelFile,
                                          const AString& ciftiMapUniqueID,
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
    
    int32_t mapIndex = -1;
    for (std::vector<CiftiBrainordinateLabelFile*>::iterator iter = allCiftiBrainordinateLabelFiles.begin();
         iter != allCiftiBrainordinateLabelFiles.end();
         iter++) {
        CiftiBrainordinateLabelFile* csf = *iter;
        if (csf == ciftiLabelFile) {
            mapIndex = csf->getMapIndexFromUniqueID(ciftiMapUniqueID);
            if (mapIndex >= 0) {
                break;
            }
        }
    }
    
    if (mapIndex < 0) {
        return false;
    }
    
    const DisplayGroupEnum::Enum displayGroup = displayPropertiesLabels->getDisplayGroupForTab(browserTabIndex);
    const LabelDrawingTypeEnum::Enum labelDrawingType = displayPropertiesLabels->getDrawingType(displayGroup,
                                                               browserTabIndex);
    bool isOutlineMode = false;
    switch (labelDrawingType) {
        case LabelDrawingTypeEnum::DRAW_FILLED:
            break;
        case LabelDrawingTypeEnum::DRAW_OUTLINE:
            isOutlineMode = true;
            break;
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
    ciftiLabelFile->updateScalarColoringForMap(mapIndex,
                                                    brain->getPaletteFile());
    
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
    
    const Surface* surface = ((brainStructure->getNumberOfSurfaces() > 0)
                              ? brainStructure->getSurface(0)
                              : NULL);
    CaretPointer<TopologyHelper> topologyHelper;
    if (surface != NULL) {
        topologyHelper = surface->getTopologyHelper();
    }

    /*
     * All nodes are colored.  Remove coloring for nodes whose
     * label is not selected.
     */
    GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
    CaretAssert(labelTable);
    for (int32_t iNode = 0; iNode < numberOfNodes; iNode++) {
        const int32_t indexAlpha = (iNode * 4) + 3;
        if (rgbv[indexAlpha] > 0.0) {
            bool disableIt = false;
            
            const int32_t labelKey = static_cast<int32_t>(dataValues[iNode]);
            GiftiLabel* label = labelTable->getLabel(labelKey);
            if (label != NULL) {
                const GroupAndNameHierarchyItem* item = label->getGroupNameSelectionItem();
                if (item->isSelected(displayGroup,
                                     browserTabIndex) == false) {
                    disableIt = true;
                }
            }
            
            if (disableIt) {
                rgbv[indexAlpha] = 0.0;
            }
            else if (isOutlineMode) {
                /*
                 * In outline mode, only those nodes that are on the 'outside'
                 * are drawn so that the labels appear as outlines.
                 */
                if (topologyHelper != NULL) {
                    int32_t numberOfNeighbors = 0;
                    const int32_t* neighbors = topologyHelper->getNodeNeighbors(iNode,
                                                                                numberOfNeighbors);
                    bool insideFlag = true;
                    for (int32_t iNeigh = 0; iNeigh < numberOfNeighbors; iNeigh++) {
                        const int32_t ni = neighbors[iNeigh];
                        const int32_t neighborLabelKey = static_cast<int32_t>(dataValues[ni]);
                        if (labelKey != neighborLabelKey) {
                            insideFlag = false;
                            break;
                        }
                    }
                    
                    if (insideFlag) {
                        rgbv[indexAlpha] = 0.0;
                    }
                }
            }
        }
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
SurfaceNodeColoring::assignCiftiScalarColoring(const BrainStructure* brainStructure,
                                               CiftiBrainordinateScalarFile* ciftiScalarFile,
                                               const AString& ciftiMapUniqueID,
                                               const int32_t numberOfNodes,
                                               float* rgbv)
{
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiBrainordinateScalarFile*> allCiftiBrainordinateScalarFiles;
    brain->getConnectivityDenseScalarFiles(allCiftiBrainordinateScalarFiles);
    
    int32_t mapIndex = -1;
    for (std::vector<CiftiBrainordinateScalarFile*>::iterator iter = allCiftiBrainordinateScalarFiles.begin();
         iter != allCiftiBrainordinateScalarFiles.end();
         iter++) {
        CiftiBrainordinateScalarFile* csf = *iter;
        if (csf == ciftiScalarFile) {
            mapIndex = csf->getMapIndexFromUniqueID(ciftiMapUniqueID);
            if (mapIndex >= 0) {
                break;
            }
        }
    }
    
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
    ciftiScalarFile->updateScalarColoringForMap(mapIndex,
                                                    brain->getPaletteFile());

    
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
                                               const AString& ciftiMapUniqueID,
                                               const int32_t numberOfNodes,
                                               float* rgbv)
{
    Brain* brain = (Brain*)(brainStructure->getBrain());
    std::vector<CiftiBrainordinateDataSeriesFile*> allCiftiDataSeriesFiles;
    brain->getConnectivityDataSeriesFiles(allCiftiDataSeriesFiles);
    
    int32_t mapIndex = -1;
    for (std::vector<CiftiBrainordinateDataSeriesFile*>::iterator iter = allCiftiDataSeriesFiles.begin();
         iter != allCiftiDataSeriesFiles.end();
         iter++) {
        CiftiBrainordinateDataSeriesFile* cdsf = *iter;
        if (cdsf == ciftiDataSeriesFile) {
            mapIndex = cdsf->getMapIndexFromUniqueID(ciftiMapUniqueID);
            if (mapIndex >= 0) {
                break;
            }
        }
    }
    
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
    ciftiDataSeriesFile->updateScalarColoringForMap(mapIndex,
                                                    brain->getPaletteFile());

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
SurfaceNodeColoring::assignRgbaColoring(const BrainStructure* /*brainStructure*/, 
                                        const RgbaFile* /*rgbaFile*/,
                                        const AString& /*rgbaMapUniqueID*/,
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

