
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

#define __IDENTIFICATION_FORMATTED_TEXT_GENERATOR_DECLARE__
#include "IdentificationFormattedTextGenerator.h"
#undef __IDENTIFICATION_FORMATTED_TEXT_GENERATOR_DECLARE__

#include "Border.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "ChartDataCartesian.h"
#include "ChartDataSource.h"
#include "ChartModelDataSeries.h"
#include "ChartTwoDataCartesian.h"
#include "ChartableMatrixInterface.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "ChartableTwoFileLineLayerChart.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "ChartableTwoFileMatrixChart.h"
#include "CiftiDenseSparseFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiMappableDataFile.h"
#include "CaretVolumeExtension.h"
#include "DataToolTipsManager.h"
#include "EventBrowserTabGetAll.h"
#include "EventBrowserTabIndicesGetAllViewed.h"
#include "EventCaretDataFilesGet.h"
#include "EventCaretMappableDataFilesAndMapsInDisplayedOverlays.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "FileIdentificationAttributes.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GraphicsPrimitive.h"
#include "GraphicsPrimitiveV3f.h"
#include "Histogram.h"
#include "HistologySlicesFile.h"
#include "HtmlTableBuilder.h"
#include "IdentificationFilter.h"
#include "IdentificationManager.h"
#include "IdentifiedItemUniversal.h"
#include "MapFileDataSelector.h"
#include "MediaFile.h"
#include "MetaVolumeFile.h"
#include "MetricDynamicConnectivityFile.h"
#include "OverlaySet.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemChartDataSeries.h"
#include "SelectionItemChartFrequencySeries.h"
#include "SelectionItemChartMatrix.h"
#include "SelectionItemCiftiConnectivityMatrixRowColumn.h"
#include "SelectionItemChartTimeSeries.h"
#include "SelectionItemChartTwoHistogram.h"
#include "SelectionItemChartTwoLineLayer.h"
#include "SelectionItemChartTwoLineLayerVerticalNearest.h"
#include "SelectionItemChartTwoLineSeries.h"
#include "SelectionItemChartTwoMatrix.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocus.h"
#include "SelectionItemHistologyCoordinate.h"
#include "SelectionItemMediaLogicalCoordinate.h"
#include "SelectionItemMediaPlaneCoordinate.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemUniversalIdentificationSymbol.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "IdentificationStringBuilder.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Surface.h"
#include "VolumeDynamicConnectivityFile.h"
#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class IdentificationFormattedTextGenerator
 * \brief Creates text describing selected data.
 *
 * Examine the selected data and generate descriptive text.
 */

/**
 * Constructor.
 */
IdentificationFormattedTextGenerator::IdentificationFormattedTextGenerator()
: CaretObject(),
m_noDataText("no-data")
{
    
}

/**
 * Destructor.
 */
IdentificationFormattedTextGenerator::~IdentificationFormattedTextGenerator()
{
    
}

/**
 * Create identification text from selection in the identification manager.
 * @param idManselectionManagerager
 *    Selection manager containing selection.
 * @param brain
 *    The brain.
 * @param tabIndex
 *    Index of tab where identification took place
 */
AString 
IdentificationFormattedTextGenerator::createIdentificationText(const SelectionManager* selectionManager,
                                                               const Brain* brain,
                                                               const int32_t tabIndex) const
{
    CaretAssert(selectionManager);
    CaretAssert(brain);
    
    const IdentificationManager* idManager = brain->getIdentificationManager();
    const IdentificationFilter* filter = idManager->getIdentificationFilter();
    
    IdentificationStringBuilder idText;
    std::unique_ptr<HtmlTableBuilder> chartHtmlTableBuilder = createHtmlTableBuilder(3);
    chartHtmlTableBuilder->setTitleBold("Charts");
    std::unique_ptr<HtmlTableBuilder> geometryHtmlTableBuilder = createHtmlTableBuilder(3);
    geometryHtmlTableBuilder->setTitleBold("Geometry");
    std::unique_ptr<HtmlTableBuilder> histologyHtmlTableBuilder = createHtmlTableBuilder(2);
    histologyHtmlTableBuilder->setTitlePlain("Histology");
    std::unique_ptr<HtmlTableBuilder> mediaHtmlTableBuilder = createHtmlTableBuilder(2);
    mediaHtmlTableBuilder->setTitlePlain("Media Image");
    std::unique_ptr<HtmlTableBuilder> labelHtmlTableBuilder = createHtmlTableBuilder(2);
    labelHtmlTableBuilder->setTitlePlain("Labels");
    std::unique_ptr<HtmlTableBuilder> layersHtmlTableBuilder = createHtmlTableBuilder(3);
    layersHtmlTableBuilder->setTitlePlain("Layers");
    std::unique_ptr<HtmlTableBuilder> rgbaHtmlTableBuilder = createHtmlTableBuilder(3);
    rgbaHtmlTableBuilder->setTitlePlain("RGBA");
    std::unique_ptr<HtmlTableBuilder> scalarHtmlTableBuilder = createHtmlTableBuilder(3);
    scalarHtmlTableBuilder->setTitlePlain("Scalars");

    const SelectionItemSurfaceNode* surfaceID = selectionManager->getSurfaceNodeIdentification();
    
    if (filter->isShowVertexVoxelEnabled()) {
        this->generateSurfaceVertexIdentificationText(*geometryHtmlTableBuilder,
                                                      brain,
                                                      surfaceID);
        this->generateVolumeVoxelIdentificationText(*geometryHtmlTableBuilder,
                                                    brain,
                                                    selectionManager->getVoxelIdentification());
    }
    
    std::vector<MapFileAndMapIndices> mapFilesAndIndices;
    std::vector<MapFileAndMapIndices> chartFilesAndIndices;
    std::vector<MapFileAndMapIndices> histologyFilesAndIndices;
    std::vector<MapFileAndMapIndices> mediaFilesAndIndices;
    getFilesForIdentification(filter,
                              tabIndex,
                              mapFilesAndIndices,
                              chartFilesAndIndices,
                              histologyFilesAndIndices,
                              mediaFilesAndIndices);
    
    for (auto& mfi : mapFilesAndIndices) {
        CaretMappableDataFile* cmdf(mfi.m_mapFile->castToCaretMappableDataFile());
        CaretAssert(cmdf);
        
        if (cmdf->isSurfaceMappable()) {
            this->generateSurfaceDataIdentificationText(*labelHtmlTableBuilder,
                                                        *scalarHtmlTableBuilder,
                                                        cmdf,
                                                        mfi.m_mapIndices,
                                                        brain,
                                                        surfaceID);
        }
        if (selectionManager->getVoxelIdentification()->isValid()) {
            if (cmdf->isVolumeMappable()) {
                this->generateVolumeDataIdentificationText(*labelHtmlTableBuilder,
                                                           *rgbaHtmlTableBuilder,
                                                           *scalarHtmlTableBuilder,
                                                           cmdf,
                                                           mfi.m_mapIndices,
                                                           brain,
                                                           selectionManager->getVoxelIdentification());
            }
        }
    }
    
    for (auto& cfi : chartFilesAndIndices) {
        CaretMappableDataFile* mapFile(cfi.m_mapFile->castToCaretMappableDataFile());
        CaretAssert(mapFile);
        this->generateChartTwoHistogramIdentificationText(*chartHtmlTableBuilder,
                                                          idText,
                                                          selectionManager->getChartTwoHistogramIdentification(),
                                                          mapFile,
                                                          cfi.m_mapIndices,
                                                          false);
        
        this->generateChartTwoLineLayerNearestIdentificationText(*chartHtmlTableBuilder,
                                                                 idText,
                                                                 selectionManager->getChartTwoLineLayerVerticalNearestIdentification(),
                                                                 mapFile,
                                                                 cfi.m_mapIndices,
                                                                 false);
        
        this->generateChartTwoLineLayerIdentificationText(*chartHtmlTableBuilder,
                                                          idText,
                                                          selectionManager->getChartTwoLineLayerIdentification(),
                                                          mapFile,
                                                          cfi.m_mapIndices,
                                                          false);
        
        this->generateChartTwoLineSeriesIdentificationText(*chartHtmlTableBuilder,
                                                           idText,
                                                           selectionManager->getChartTwoLineSeriesIdentification(),
                                                           mapFile,
                                                           cfi.m_mapIndices,
                                                           false);
        
        this->generateChartTwoMatrixIdentificationText(*chartHtmlTableBuilder,
                                                       idText,
                                                       selectionManager->getChartTwoMatrixIdentification(),
                                                       mapFile,
                                                       cfi.m_mapIndices,
                                                       false);
    }
    
    generateHistologyPlaneCoordinateIdentificationText(idManager,
                                                       *histologyHtmlTableBuilder,
                                                       idText,
                                                       selectionManager->getHistologyPlaneCoordinateIdentification());

    for (auto& mfi : mediaFilesAndIndices) {
        MediaFile* mediaFile(mfi.m_mapFile->castToMediaFile());
        CaretAssert(mediaFile);
        this->generateMediaLogicalCoordinateIdentificationText(*mediaHtmlTableBuilder,
                                              idText,
                                              mediaFile,
                                              mfi.m_mapIndices,
                                              selectionManager->getMediaLogicalCoordinateIdentification());
        this->generateMediaPlaneCoordinateIdentificationText(*mediaHtmlTableBuilder,
                                                               idText,
                                                               mediaFile,
                                                               mfi.m_mapIndices,
                                                               selectionManager->getMediaPlaneCoordinateIdentification());
    }

    if (filter->isShowFociEnabled()) {
        this->generateSurfaceFocusIdentifcationText(*layersHtmlTableBuilder,
                                                    selectionManager->getSurfaceFocusIdentification(),
                                                    false);
        this->generateFocusIdentifcationText(*layersHtmlTableBuilder,
                                             idText,
                                             selectionManager->getFocusIdentification(),
                                             false);
    }

    if (filter->isShowBorderEnabled()) {
        this->generateSurfaceBorderIdentifcationText(*layersHtmlTableBuilder,
                                                     idText,
                                                     selectionManager->getSurfaceBorderIdentification(),
                                                     false);
    }
    
    
    this->generateChartDataSeriesIdentificationText(*chartHtmlTableBuilder,
                                                    selectionManager->getChartDataSeriesIdentification());
    
    this->generateChartFrequencySeriesIdentificationText(*chartHtmlTableBuilder,
                                                         selectionManager->getChartFrequencySeriesIdentification());
    
    this->generateChartTimeSeriesIdentificationText(*chartHtmlTableBuilder,
                                                    selectionManager->getChartTimeSeriesIdentification());
    
    this->generateChartMatrixIdentificationText(*chartHtmlTableBuilder,
                                                selectionManager->getChartMatrixIdentification());
    
    this->generateCiftiConnectivityMatrixIdentificationText(*chartHtmlTableBuilder,
                                                            selectionManager->getCiftiConnectivityMatrixRowColumnIdentification());
    
    AString textOut;
    textOut.append(geometryHtmlTableBuilder->getAsHtmlTable());
    textOut.append(labelHtmlTableBuilder->getAsHtmlTable());
    textOut.append(rgbaHtmlTableBuilder->getAsHtmlTable());
    textOut.append(scalarHtmlTableBuilder->getAsHtmlTable());
    textOut.append(layersHtmlTableBuilder->getAsHtmlTable());
    textOut.append(chartHtmlTableBuilder->getAsHtmlTable());
    textOut.append(histologyHtmlTableBuilder->getAsHtmlTable());
    textOut.append(mediaHtmlTableBuilder->getAsHtmlTable());
    return textOut;
}

/**
 * Get files for information
 * @param filter
 * Identification filter
 * @param tabIndex
 * Index of tab where ID took place
 * @param mapFileInfoOut
 * Map files for ID
 * @param histologyFilesAndIndicesOut,
 *  Histology files for ID
 * @param mediaFileInfoOut
 * Media files for ID
 */
void
IdentificationFormattedTextGenerator::getFilesForIdentification(const IdentificationFilter* filter,
                                                                const int32_t tabIndex,
                                                                std::vector<MapFileAndMapIndices>& mapFilesAndIndicesOut,
                                                                std::vector<MapFileAndMapIndices>& chartFilesAndIndicesOut,
                                                                std::vector<MapFileAndMapIndices>& histologyFilesAndIndicesOut,
                                                                std::vector<MapFileAndMapIndices>& mediaFilesAndIndicesOut) const
{
    mapFilesAndIndicesOut.clear();
    chartFilesAndIndicesOut.clear();
    histologyFilesAndIndicesOut.clear();
    mediaFilesAndIndicesOut.clear();
    
    /**
     * Event gets files from enabled overlays in the viewed tab(s)
     */
    EventCaretMappableDataFilesAndMapsInDisplayedOverlays overlayFilesEvent;
    switch (filter->getTabFiltering()) {
        case IdentificationFilterTabSelectionEnum::ALL_DISPLAYED_TABS:
        {
            EventBrowserTabIndicesGetAllViewed viewedTabsEvent;
            EventManager::get()->sendEvent(viewedTabsEvent.getPointer());
            const std::vector<int32_t> tabs = viewedTabsEvent.getAllBrowserTabIndices();
            std::set<int32_t> tabsSet(tabs.begin(),
                                      tabs.end());
            overlayFilesEvent.setTabIndicesConstraint(tabsSet);
        }
            break;
        case IdentificationFilterTabSelectionEnum::MOUSE_CLICKED_TAB:
            if (tabIndex >= 0) {
                std::set<int32_t> tabIndices { tabIndex };
                overlayFilesEvent.setTabIndicesConstraint(tabIndices);
            }
            break;
    }
    EventManager::get()->sendEvent(overlayFilesEvent.getPointer());

    /**
     * Get chart files in overlays.  Chart data is NOT filtered at this time
     */
    std::vector<EventCaretMappableDataFilesAndMapsInDisplayedOverlays::MapFileInfo> chartTwoFilesInOverlays(overlayFilesEvent.getChartTwoFilesAndMaps());
    for (EventCaretMappableDataFilesAndMapsInDisplayedOverlays::MapFileInfo chartInfo : chartTwoFilesInOverlays) {
        MapFileAndMapIndices fileInfo(chartInfo.m_mapFile);
        fileInfo.addMapIndices(chartInfo.m_mapIndices);
        chartFilesAndIndicesOut.push_back(fileInfo);
    }
    
    /*
     * Get files in brainordinate overlays
     */
    std::vector<EventCaretMappableDataFilesAndMapsInDisplayedOverlays::MapFileInfo> brainordinateFilesInOverlays(overlayFilesEvent.getBrainordinateFilesAndMaps());
    const int32_t numBrainordinateFilesInOverlays(brainordinateFilesInOverlays.size());

    /*
     * Get files in media overlays
     */
    std::vector<EventCaretMappableDataFilesAndMapsInDisplayedOverlays::MediaFileInfo> mediaFilesInOverlays(overlayFilesEvent.getMediaFilesAndMaps());
    const int32_t numMediaFilesInOverlays(static_cast<int32_t>(mediaFilesInOverlays.size()));
    
    /*
     * Get files in histology overlays
     */
    std::vector<EventCaretMappableDataFilesAndMapsInDisplayedOverlays::HistologySlicesFileInfo> histologyFilesInOverlays(overlayFilesEvent.getHistologySlicesFilesAndMaps());
    const int32_t numHistologyFilesInOverlays(static_cast<int32_t>(histologyFilesInOverlays.size()));

    
    /*
     * Get all identifiable data files
     */
    std::vector<CaretDataFile*> idCaretDataFiles(EventCaretDataFilesGet::getIdentifiableFilesSortedByName());
    
    /*
     * Get all files that should have identification info displayed
     */
    for (CaretDataFile* caretDataFile : idCaretDataFiles) {
        CaretAssert(caretDataFile);
        CaretMappableDataFile* mapFile(caretDataFile->castToCaretMappableDataFile());
        HistologySlicesFile* histologyFile(caretDataFile->castToHistologySlicesFile());
        MediaFile* mediaFile(caretDataFile->castToMediaFile());
        const CiftiFiberOrientationFile* fibersFile(dynamic_cast<CiftiFiberOrientationFile*>(caretDataFile));
        if (fibersFile != NULL) {
            /* Ignore fiber orientations file, no maps */
            continue;
        }
        if ((mapFile == NULL)
            && (histologyFile == NULL)
            && (mediaFile == NULL)) {
            const AString msg(caretDataFile->getFileName()
                              + " is neither brainordinate mappable nor a media file.");
            CaretAssertMessage(0, msg);
            CaretLogSevere(msg);
            continue;
        }
        
        bool allMapsFlag(false);
        switch (caretDataFile->getFileIdentificationAttributes()->getMapSelectionMode()) {
            case FileIdentificationMapSelectionEnum::ALL:
                allMapsFlag = true;
                break;
            case FileIdentificationMapSelectionEnum::SELECTED:
                allMapsFlag = false;
                break;
        }

        switch (caretDataFile->getFileIdentificationAttributes()->getDisplayMode()) {
            case FileIdentificationDisplayModeEnum::ALWAYS:
            {
                if (mapFile != NULL) {
                    MapFileAndMapIndices mapFileAndIndices(mapFile);
                    if (allMapsFlag) {
                        for (int32_t i = 0; i < mapFile->getNumberOfMaps(); i++) {
                            mapFileAndIndices.addMapIndex(i);
                        }
                    }
                    else {
                        mapFileAndIndices.addMapIndex(mapFile->getFileIdentificationAttributes()->getMapIndex());
                    }
                    mapFilesAndIndicesOut.push_back(mapFileAndIndices);
                }
                else if (histologyFile != NULL) {
                    MapFileAndMapIndices mapFileAndIndices(histologyFile);
                    if (allMapsFlag) {
                        for (int32_t i = 0; i < histologyFile->getNumberOfHistologySlices(); i++) {
                            mapFileAndIndices.addMapIndex(i);
                        }
                    }
                    else {
                        mapFileAndIndices.addMapIndex(histologyFile->getFileIdentificationAttributes()->getMapIndex());
                    }
                    histologyFilesAndIndicesOut.push_back(mapFileAndIndices);
                }
                else if (mediaFile != NULL) {
                    MapFileAndMapIndices mapFileAndIndices(mediaFile);
                    if (allMapsFlag) {
                        for (int32_t i = 0; i < mediaFile->getNumberOfFrames(); i++) {
                            mapFileAndIndices.addMapIndex(i);
                        }
                    }
                    else {
                        mapFileAndIndices.addMapIndex(mediaFile->getFileIdentificationAttributes()->getMapIndex());
                    }
                    mediaFilesAndIndicesOut.push_back(mapFileAndIndices);
                }
                else {
                    CaretAssert(0);
                }
            }
                break;
            case FileIdentificationDisplayModeEnum::NEVER:
                break;
            case FileIdentificationDisplayModeEnum::OVERLAY:
            {
                if (mapFile != NULL) {
                    for (int32_t i = 0; i < numBrainordinateFilesInOverlays; i++) {
                        CaretAssertVectorIndex(brainordinateFilesInOverlays, i);
                        if (brainordinateFilesInOverlays[i].m_mapFile == mapFile) {
                            MapFileAndMapIndices mapFileAndIndices(mapFile);
                            if (allMapsFlag) {
                                for (int32_t i = 0; i < mapFile->getNumberOfMaps(); i++) {
                                    mapFileAndIndices.addMapIndex(i);
                                }
                            }
                            else {
                                mapFileAndIndices.addMapIndices(brainordinateFilesInOverlays[i].m_mapIndices);
                            }
                            mapFilesAndIndicesOut.push_back(mapFileAndIndices);
                            break;
                        }
                    }
                }
                else if (mediaFile != NULL) {
                    for (int32_t i = 0; i < numMediaFilesInOverlays; i++) {
                        CaretAssertVectorIndex(mediaFilesInOverlays, i);
                        if (mediaFilesInOverlays[i].m_mediaFile == mediaFile) {
                            MapFileAndMapIndices mapFileAndIndices(mediaFile);
                            if (allMapsFlag) {
                                for (int32_t i = 0; i < mediaFile->getNumberOfFrames(); i++) {
                                    mapFileAndIndices.addMapIndex(i);
                                }
                            }
                            else {
                                mapFileAndIndices.addMapIndices(mediaFilesInOverlays[i].m_frameIndices);
                            }
                            mediaFilesAndIndicesOut.push_back(mapFileAndIndices);
                            break;
                        }
                    }
                }
                else if (histologyFile != NULL) {
                    for (int32_t i = 0; i < numHistologyFilesInOverlays; i++) {
                        CaretAssertVectorIndex(histologyFilesInOverlays, i);
                        if (histologyFilesInOverlays[i].m_histologySlicesFile == histologyFile) {
                            MapFileAndMapIndices mapFileAndIndices(histologyFile);
                            if (allMapsFlag) {
                                for (int32_t i = 0; i < histologyFile->getNumberOfHistologySlices(); i++) {
                                    mapFileAndIndices.addMapIndex(i);
                                }
                            }
                            else {
                                mapFileAndIndices.addMapIndices(histologyFilesInOverlays[i].m_sliceIndices);
                            }
                            histologyFilesAndIndicesOut.push_back(mapFileAndIndices);
                        }
                    }
                }
                else {
                    const AString msg(caretDataFile->getFileName()
                                      + " is neither brainordinate mappable nor a media file.");
                    CaretAssertMessage(0, msg);
                    CaretLogSevere(msg);
                }
                break;
            }
        }
        
    }
}

/**
 * Get text for the tooltip for a selected node.
 *
 * @param brain
 *     The Brain.
 * @param browserTab
 *     Browser tab in which tooltip is displayed
 * @param selectionManager
 *     The selection manager.
 * @param dataToolTipsManager
 *     The data tooltips manager
 * @param idText
 *     String builder for identification text.
 */
AString
IdentificationFormattedTextGenerator::createToolTipText(const Brain* brain,
                                               const BrowserTabContent* browserTab,
                                               const SelectionManager* selectionManager,
                                               const DataToolTipsManager* dataToolTipsManager) const
{
    CaretAssert(brain);
    CaretAssert(browserTab);
    CaretAssert(selectionManager);
    CaretAssert(dataToolTipsManager);
 
    const IdentificationManager* idManager = brain->getIdentificationManager();

    const SelectionItemSurfaceNode* selectedNode = selectionManager->getSurfaceNodeIdentification();
    const SelectionItemVoxel* selectedVoxel = selectionManager->getVoxelIdentification();
    const SelectionItemHistologyCoordinate* selectionHistologyCoordinate(selectionManager->getHistologyPlaneCoordinateIdentification());
    const SelectionItemMediaLogicalCoordinate* selectionMediaLogicalCoordinate = selectionManager->getMediaLogicalCoordinateIdentification();
    const SelectionItemMediaPlaneCoordinate* selectionMediaPlaneCoordinate(selectionManager->getMediaPlaneCoordinateIdentification());
    const SelectionItemUniversalIdentificationSymbol* selectionSymbol = selectionManager->getUniversalIdentificationSymbol();
    AString selectionToolTip;
    if (selectionSymbol->isValid()) {
        const IdentifiedItemUniversal* idItem(idManager->getIdentifiedItemWithIdentifier(selectionSymbol->getIdentifiedItemUniqueIdentifier()));
        if (idItem != NULL) {
            selectionToolTip = idItem->getToolTip();
        }
    }
    
    IdentificationStringBuilder idText;
    
    if ( ! selectionToolTip.isEmpty()) {
        idText.append(selectionToolTip);
    }
    else if (selectedNode->isValid()) {
        generateSurfaceToolTip(brain,
                               idManager,
                               browserTab,
                               selectionManager,
                               dataToolTipsManager,
                               idText);
    }
    else if (selectedVoxel->isValid()) {
        generateVolumeToolTip(idManager,
                              browserTab,
                              selectionManager,
                              dataToolTipsManager,
                              idText);
    }
    else if (selectionHistologyCoordinate->isValid()) {
        generateHistologyPlaneCoordinateToolTip(idManager,
                                                selectionManager,
                                                dataToolTipsManager,
                                                idText);
    }
    else if (selectionMediaLogicalCoordinate->isValid()) {
        generateMediaLogicalCoordinateToolTip(selectionManager,
                             dataToolTipsManager,
                             idText);
    }
    else if (selectionMediaPlaneCoordinate->isValid()) {
        generateMediaPlaneCoordinateToolTip(selectionManager,
                                              dataToolTipsManager,
                                              idText);
    }
    else if (selectionMediaPlaneCoordinate->isValid()) {
        generateMediaPlaneCoordinateToolTip(selectionManager,
                                            dataToolTipsManager,
                                            idText);
    }
    else {
        generateChartToolTip(selectionManager,
                             dataToolTipsManager,
                             idText);
    }

    AString text;
    if (idText.length() > 0) {
       text = idText.toStringWithHtmlBodyForToolTip();
    }

    return text;
}


/**
 * Generate identification text for volume voxel identification.
 *
 * @param htmlTableBuilder
 *     Html table builder for identification text.
 * @param brain
 *     The brain.
 * @param idVolumeVoxel
 *     Information for volume voxel ID.
 */
void
IdentificationFormattedTextGenerator::generateVolumeVoxelIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                              const Brain* /*brain*/,
                                                              const SelectionItemVoxel* idVolumeVoxel) const
{
    if (idVolumeVoxel->isValid() == false) {
        return;
    }
    
    const VolumeMappableInterface* idVolumeFile = idVolumeVoxel->getVolumeFile();
    const VoxelIJK ijk(idVolumeVoxel->getVoxelIJK());
    const Vector3D xyz(idVolumeVoxel->getVoxelXYZ());

    const QString xyzText(xyzToText(xyz));
    
    const QString ijkText("Voxel IJK ("
                          + AString::fromNumbers(ijk.m_ijk, 3, ", ")
                          + ")");

    QString filename;
    const CaretDataFile* caretDataFile = dynamic_cast<const CaretDataFile*>(idVolumeFile);
    if (caretDataFile != NULL) {
        filename = caretDataFile->getFileNameNoPath();
    }
    
    htmlTableBuilder.addHeaderRow(ijkText,
                                  xyzText,
                                  filename);
}

/**
 * Generate identification text for volume data identification.
 *
 * @param labelHtmlTableBuilder
 *     HTML table builder for label identification text.
 * @param rgbaHtmlTableBuilder
 *     HTML table builder for RGBA identification text.
 * @param scalarHtmlTableBuilder
 *     HTML table builder for scalar identification text.
 * @param mapFile
 *     FIle for generating identification
 * @param mapIndices
 *     Indices of map
 * @param brain
 *     The brain.
 * @param idVolumeVoxel
 *     Information for volume voxel ID.
 */
void
IdentificationFormattedTextGenerator::generateVolumeDataIdentificationText(HtmlTableBuilder& labelHtmlTableBuilder,
                                                                           HtmlTableBuilder& rgbaHtmlTableBuilder,
                                                                           HtmlTableBuilder& scalarHtmlTableBuilder,
                                                                           CaretMappableDataFile* mapFile,
                                                                           const std::set<int32_t>& mapIndicesSet,
                                                                           const Brain* brain,
                                                                           const SelectionItemVoxel* idVolumeVoxel) const
{
    if (idVolumeVoxel->isValid() == false) {
        return;
    }
    
    const Vector3D xyz(idVolumeVoxel->getVoxelXYZ());
    const float x(xyz[0]);
    const float y(xyz[1]);
    const float z(xyz[2]);

    /*
     * Get all volume files
     */
    std::vector<const VolumeMappableInterface*> volumeInterfaces;
    const int32_t numVolumeFiles = brain->getNumberOfVolumeFiles();
    for (int32_t i = 0; i < numVolumeFiles; i++) {
        const VolumeFile* vf = brain->getVolumeFile(i);
        volumeInterfaces.push_back(vf);

        const VolumeDynamicConnectivityFile* volDynConnFile = vf->getVolumeDynamicConnectivityFile();
        if (volDynConnFile != NULL) {
            if (volDynConnFile->isDataValid()) {
                volumeInterfaces.push_back(volDynConnFile);
            }
        }
    }

    /*
     * Get the CIFTI files that are volume mappable
     */
    std::vector<CiftiMappableDataFile*> allCiftiMappableDataFiles;
    brain->getAllCiftiMappableDataFiles(allCiftiMappableDataFiles);
    for (std::vector<CiftiMappableDataFile*>::iterator ciftiMapIter = allCiftiMappableDataFiles.begin();
         ciftiMapIter != allCiftiMappableDataFiles.end();
         ciftiMapIter++) {
        const CiftiMappableDataFile* cmdf = *ciftiMapIter;
        if (cmdf->isEmpty() == false) {
            if (cmdf->isVolumeMappable()) {
                volumeInterfaces.push_back(cmdf);
            }
        }
    }

    const std::vector<int32_t> mapIndices(mapIndicesSet.begin(),
                                          mapIndicesSet.end());
    const int32_t numMapIndices = static_cast<int32_t>(mapIndices.size());
    
    const VolumeMappableInterface* volumeInterfaceFile = dynamic_cast<VolumeMappableInterface*>(mapFile);
    
    /*
     * For meta-volume file, need to find child volume at ID location
     */
    AString metaVolumeFileName;
    const MetaVolumeFile* mvf(dynamic_cast<MetaVolumeFile*>(mapFile));
    if (mvf != NULL) {
        if (numMapIndices > 0) {
            const int32_t mapIndex(mapIndices[0]);
            float voxelValue(0.0);
            const VolumeFile* vf(mvf->getVolumeFileContainingXYZ(mapIndex,
                                                                 xyz,
                                                                 voxelValue));
            if (vf != NULL) {
                metaVolumeFileName = mvf->getFileNameNoPath();
                volumeInterfaceFile = vf;
            }
        }
    }
    
    if (volumeInterfaceFile != NULL) {
        const VolumeFile* volumeFile = dynamic_cast<const VolumeFile*>(volumeInterfaceFile);
        const CiftiMappableDataFile* ciftiFile = dynamic_cast<const CiftiMappableDataFile*>(volumeInterfaceFile);
        const CiftiDenseSparseFile* denseSparseFile(dynamic_cast<const CiftiDenseSparseFile*>(volumeInterfaceFile));
        CaretAssert((volumeFile != NULL)
                    || (ciftiFile != NULL)
                    || (denseSparseFile != NULL));

        int64_t vfI, vfJ, vfK;
        volumeInterfaceFile->enclosingVoxel(x, y, z,
                                            vfI, vfJ, vfK);

        if (volumeInterfaceFile->indexValid(vfI, vfJ, vfK)) {
            if (volumeFile != NULL) {
                AString ijkText("IJK ("
                                 + AString::number(vfI)
                                 + ", "
                                 + AString::number(vfJ)
                                 + ", "
                                 + AString::number(vfK)
                                 + ")  ");

                AString text;
                
                for (int32_t k = 0; k < numMapIndices; k++) {
                    CaretAssertVectorIndex(mapIndices, k);
                    const int32_t mapIndex = mapIndices[k];
                    if (k > 0) {
                        text += " ";
                    }
                    if (volumeFile != NULL) {
                        if (volumeFile->getType() == SubvolumeAttributes::LABEL) {
                            const int32_t labelIndex = static_cast<int32_t>(volumeFile->getValue(vfI, vfJ, vfK, mapIndex));
                            const GiftiLabelTable* glt = volumeFile->getMapLabelTable(mapIndex);
                            const GiftiLabel* gl = glt->getLabel(labelIndex);
                            if (gl != NULL) {
                                text += gl->getName();
                                text += (" ("
                                         + volumeFile->getMapName(mapIndex)
                                         + ")");
                            }
                            else {
                                text += ("LABLE_MISSING_FOR_INDEX="
                                         + AString::number(labelIndex));
                            }
                        }
                        else if (volumeFile->getType() == SubvolumeAttributes::RGB) {
                            if (volumeFile->getNumberOfComponents() == 4) {
                                text += ("RGBA("
                                         + AString::number(volumeFile->getValue(vfI, vfJ, vfK, mapIndex, 0) * 255.0, 'f', 0)
                                         + ","
                                         + AString::number(volumeFile->getValue(vfI, vfJ, vfK, mapIndex, 1) * 255.0, 'f', 0)
                                         + ","
                                         + AString::number(volumeFile->getValue(vfI, vfJ, vfK, mapIndex, 2) * 255.0, 'f', 0)
                                         + ","
                                         + AString::number(volumeFile->getValue(vfI, vfJ, vfK, mapIndex, 3) * 255.0, 'f', 0)
                                         + ")");
                            }
                            else if (volumeFile->getNumberOfComponents() == 3) {
                                text += ("RGB("
                                         + AString::number(volumeFile->getValue(vfI, vfJ, vfK, mapIndex, 0) * 255.0, 'f', 0)
                                         + ","
                                         + AString::number(volumeFile->getValue(vfI, vfJ, vfK, mapIndex, 1) * 255.0, 'f', 0)
                                         + ","
                                         + AString::number(volumeFile->getValue(vfI, vfJ, vfK, mapIndex, 2) * 255.0, 'f', 0)
                                         + ")");
                            }
                        }
                        else {
                            text += dataValueToText(volumeFile->getValue(vfI, vfJ, vfK, mapIndex));
                        }
                    }
                    else if (ciftiFile != NULL) {
                    }
                }

                AString filename;
                if (dynamic_cast<const VolumeDynamicConnectivityFile*>(volumeFile) != NULL) {
                    filename.append(DataFileTypeEnum::toOverlayTypeName(DataFileTypeEnum::VOLUME_DYNAMIC) + " ");
                }
                filename.append(volumeFile->getFileNameNoPath());
                if ( ! metaVolumeFileName.isEmpty()) {
                    filename.append("<br> ("
                                    + metaVolumeFileName
                                    + ")");
                }
                if (volumeFile->isMappedWithLabelTable()) {
                    labelHtmlTableBuilder.addRow(text,
                                                 filename);
                }
                else if (volumeFile->isMappedWithPalette()) {
                    scalarHtmlTableBuilder.addRow(text,
                                                  filename,
                                                  "");
                }
                else if (volumeFile->isMappedWithRGBA()) {
                    rgbaHtmlTableBuilder.addRow(text,
                                                filename,
                                                "");
                }
            }
            else if (ciftiFile != NULL) {
                if (ciftiFile->isEmpty() == false) {
                    /*
                     * Does file have both label and scalar data
                     */
                    const bool parcelDataFlag(isParcelAndScalarTypeFile(ciftiFile->getDataFileType()));

                    AString textValue;
                    int64_t voxelIJK[3];
                    const QString separator("<br>");
                    if (ciftiFile->getVolumeVoxelIdentificationForMaps(mapIndices,
                                                                       xyz,
                                                                       separator,
                                                                       s_dataValueDigitsRightOfDecimal,
                                                                       voxelIJK,
                                                                       textValue)) {
                        AString typeIJKText = (DataFileTypeEnum::toOverlayTypeName(ciftiFile->getDataFileType())
                                            + " "
                                            + "IJK ("
                                            + AString::number(voxelIJK[0])
                                            + ", "
                                            + AString::number(voxelIJK[1])
                                            + ", "
                                            + AString::number(voxelIJK[2])
                                            + ")  ");
                        
                        
                        AString labelText;
                        if (ciftiFile->isMappedWithLabelTable()) {
                            labelText = textValue;
                        }
                        AString scalarText;
                        if (ciftiFile->isMappedWithPalette()) {
                            scalarText = textValue;
                        }
                        if (parcelDataFlag) {
                            /*
                             * Parcel data has parcel name, separator, scalar value 1, separator, scalar value 2, etc.
                             * Display parcel name in label table, and scalar
                             * value in the scalar table
                             */
                            QStringList parcelAndValue = textValue.split(separator);
                            if (parcelAndValue.size() == static_cast<int>(mapIndices.size() + 1)) {
                                labelText  = parcelAndValue.at(0);
                                parcelAndValue.removeAt(0);
                                scalarText = parcelAndValue.join(separator);
                            }
                        }
                        
                        if ( ! labelText.isEmpty()) {
                            labelHtmlTableBuilder.addRow(labelText,
                                                         ciftiFile->getFileNameNoPath());
                        }
                        if ( ! scalarText.isEmpty()) {
                            AString rowColumnIndex;
                            int64_t rowIndex(-1), columnIndex(-1);
                            if (ciftiFile->getRowColumnIndexFromVolumeXYZ(xyz,
                                                                          rowIndex,
                                                                          columnIndex)) {
                                if (rowIndex >= 0) {
                                    rowColumnIndex = ("Row "
                                                      + AString::number(rowIndex
                                                                        + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                                }
                                if (columnIndex >= 0) {
                                    if ( ! rowColumnIndex.isEmpty()) {
                                        rowColumnIndex.append("<br>");
                                    }
                                    rowColumnIndex.append("Column "
                                                          + AString::number(columnIndex
                                                                            + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                                }
                            }
                            scalarHtmlTableBuilder.addRow(scalarText,
                                                          ciftiFile->getFileNameNoPath(),
                                                          rowColumnIndex);
                        }
                    }
                    else {
                        /* no data */
                        AString labelText;
                        if (ciftiFile->isMappedWithLabelTable()) {
                            labelHtmlTableBuilder.addRow(m_noDataText,
                                                         ciftiFile->getFileNameNoPath());
                        }
                        if (ciftiFile->isMappedWithPalette()) {
                            scalarHtmlTableBuilder.addRow(m_noDataText,
                                                          ciftiFile->getFileNameNoPath());
                        }
                    }
                }
            }
            else if (denseSparseFile != NULL) {
                AString textValue;
                int64_t voxelIJK[3];
                const QString separator("<br>");
                if (denseSparseFile->getVolumeVoxelIdentificationForMaps(mapIndices,
                                                                    xyz,
                                                                    separator,
                                                                    s_dataValueDigitsRightOfDecimal,
                                                                    voxelIJK,
                                                                    textValue)) {
                    AString typeIJKText = (DataFileTypeEnum::toOverlayTypeName(denseSparseFile->getDataFileType())
                                           + " "
                                           + "IJK ("
                                           + AString::number(voxelIJK[0])
                                           + ", "
                                           + AString::number(voxelIJK[1])
                                           + ", "
                                           + AString::number(voxelIJK[2])
                                           + ")  ");
                    AString scalarText;
                    if (denseSparseFile->isMappedWithPalette()) {
                        scalarText = textValue;
                    }
                    if ( ! scalarText.isEmpty()) {
                        AString rowColumnIndex;
                        const int64_t rowIndex(denseSparseFile->getRowIndexFromVolumeXYZ(xyz));
                        if (rowIndex >= 0) {
                            rowColumnIndex = ("Row "
                                              + AString::number(rowIndex
                                                                + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                        }
                        scalarHtmlTableBuilder.addRow(scalarText,
                                                      denseSparseFile->getFileNameNoPath(),
                                                      rowColumnIndex);
                    }
                }
                else {
                    /* no data */
                    AString labelText;
                    if (denseSparseFile->isMappedWithPalette()) {
                        scalarHtmlTableBuilder.addRow(m_noDataText,
                                                      denseSparseFile->getFileNameNoPath());
                    }
                }
            }
        }
        else {
            if (mapFile->isMappedWithLabelTable()) {
                labelHtmlTableBuilder.addRow(m_noDataText,
                                             mapFile->getFileNameNoPath());
            }
            if (mapFile->isMappedWithRGBA()) {
                rgbaHtmlTableBuilder.addRow(m_noDataText,
                                            mapFile->getFileNameNoPath());
            }
            if (mapFile->isMappedWithPalette()) {
                scalarHtmlTableBuilder.addRow(m_noDataText,
                                              mapFile->getFileNameNoPath());
            }
        }
    }
}

/**
 * Generate identification text for a surface vertex.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param brain
 *     The brain.
 * @param browserTabContent
 *     Content of the browser tab.
 * @param idSurfaceNode
 *     Information for surface node ID.
 */
void 
IdentificationFormattedTextGenerator::generateSurfaceVertexIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                              const Brain* /*brain*/,
                                                                              const SelectionItemSurfaceNode* idSurfaceNode) const
{
    const Surface* surface = idSurfaceNode->getSurface();
    const int32_t nodeNumber = idSurfaceNode->getNodeNumber();
    
    if ((surface != NULL) 
        && (nodeNumber >= 0)) {
        const float* xyz = surface->getCoordinate(nodeNumber);
        const QString xyzText(xyzToText(xyz));
        htmlTableBuilder.addHeaderRow(("VERTEX " + QString::number(nodeNumber)),
                                      xyzText,
                                      StructureEnum::toGuiName(surface->getStructure()));
    }
}

/**
 * @return Is the given data file type a parcel and scalar type file?
 * @param dataFileType
 * Type of data file
 */
bool
IdentificationFormattedTextGenerator::isParcelAndScalarTypeFile(const DataFileTypeEnum::Enum dataFileType) const
{
    bool parcelDataFlag      = false;
    
    switch (dataFileType) {
        case DataFileTypeEnum::ANNOTATION:
            break;
        case DataFileTypeEnum::ANNOTATION_TEXT_SUBSTITUTION:
            break;
        case DataFileTypeEnum::BORDER:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
            parcelDataFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_SPARSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
            break;
        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_MAPS:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
            parcelDataFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC:
            parcelDataFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
            parcelDataFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
            parcelDataFlag = true;
            break;
        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
            break;
        case DataFileTypeEnum::CZI_IMAGE_FILE:
            break;
        case DataFileTypeEnum::FOCI:
            break;
        case DataFileTypeEnum::HISTOLOGY_SLICES:
            break;
        case DataFileTypeEnum::IMAGE:
            break;
        case DataFileTypeEnum::LABEL:
            break;
        case DataFileTypeEnum::META_VOLUME:
            break;
        case DataFileTypeEnum::METRIC:
            break;
        case DataFileTypeEnum::METRIC_DYNAMIC:
            break;
        case DataFileTypeEnum::OME_ZARR_IMAGE_FILE:
            break;
        case DataFileTypeEnum::PALETTE:
            break;
        case DataFileTypeEnum::RGBA:
            break;
        case DataFileTypeEnum::SAMPLES:
            break;
        case DataFileTypeEnum::SCENE:
            break;
        case DataFileTypeEnum::SPECIFICATION:
            break;
        case DataFileTypeEnum::SURFACE:
            break;
        case DataFileTypeEnum::UNKNOWN:
            CaretAssert(0);
            break;
        case DataFileTypeEnum::VOLUME:
            break;
        case DataFileTypeEnum::VOLUME_DYNAMIC:
            break;
    }

    return parcelDataFlag;
}

/**
 * Generate identification text for a surface vertex.
 * @param labelHtmlTableBuilder
 *     HTML table builder for label identification text.
 * @param scalarHtmlTableBuilder
 *     HTML table builder for scalar identification text.
 * @param mapFile
 *     FIle for generating identification
 * @param mapIndices
 *     Indices of map
 * @param brain
 *     The brain.
 * @param browserTabContent
 *     Content of the browser tab.
 * @param idSurfaceNode
 *     Information for surface node ID.
 */
void
IdentificationFormattedTextGenerator::generateSurfaceDataIdentificationText(HtmlTableBuilder& labelHtmlTableBuilder,
                                                                            HtmlTableBuilder& scalarHtmlTableBuilder,
                                                                            CaretMappableDataFile* mapFile,
                                                                            const std::set<int32_t>& mapIndicesSet,
                                                                            const Brain* /*brain*/,
                                                                            const SelectionItemSurfaceNode* idSurfaceNode) const
{
    const Surface* surface = idSurfaceNode->getSurface();
    const int32_t nodeNumber = idSurfaceNode->getNodeNumber();
    
    if ((surface != NULL)
        && (nodeNumber >= 0)) {
        LabelFile* labelFile(NULL);
        MetricFile* metricFile(NULL);
        MetricDynamicConnectivityFile* metricDynConnFile(NULL);
        CiftiMappableDataFile* cmdf = dynamic_cast<CiftiMappableDataFile*>(mapFile);
        CiftiDenseSparseFile* cdsf(dynamic_cast<CiftiDenseSparseFile*>(mapFile));
        if (cmdf == NULL) {
            if (cdsf == NULL) {
                labelFile = dynamic_cast<LabelFile*>(mapFile);
                if (labelFile == NULL) {
                    metricDynConnFile = dynamic_cast<MetricDynamicConnectivityFile*>(mapFile);
                    if (metricDynConnFile == NULL) {
                        metricFile = dynamic_cast<MetricFile*>(mapFile);
                    }
                }
            }
        }

        std::vector<int32_t> mapIndices(mapIndicesSet.begin(),
                                        mapIndicesSet.end());
        const int numMapIndices = static_cast<int32_t>(mapIndices.size());

        if (cmdf != NULL) {
            AString boldText = (DataFileTypeEnum::toOverlayTypeName(cmdf->getDataFileType())
                                + " "
                                + cmdf->getFileNameNoPath());
            
            /*
             * Does file have both label and scalar data
             */
            const bool parcelDataFlag(isParcelAndScalarTypeFile(cmdf->getDataFileType()));
            AString textValue;
            
            const AString separator("<br>");
            const bool valid = cmdf->getSurfaceNodeIdentificationForMaps(mapIndices,
                                                                         surface->getStructure(),
                                                                         nodeNumber,
                                                                         surface->getNumberOfNodes(),
                                                                         separator,
                                                                         s_dataValueDigitsRightOfDecimal,
                                                                         textValue);
            if (valid) {
                AString labelText;
                if (cmdf->isMappedWithLabelTable()) {
                    labelText = textValue;
                }
                AString scalarText;
                if (cmdf->isMappedWithPalette()) {
                    scalarText = textValue;
                }
                if (parcelDataFlag) {
                    /*
                     * Parcel data has parcel name, separator, scalar value 1, separator, scalar value 2, etc.
                     * Display parcel name in label table, and scalar
                     * value in the scalar table
                     */
                    QStringList parcelAndValue = textValue.split(separator);
                    if (parcelAndValue.size() == static_cast<int>(mapIndices.size() + 1)) {
                        labelText  = parcelAndValue.at(0);
                        parcelAndValue.removeAt(0);
                        scalarText = parcelAndValue.join(separator);
                    }
                }
                
                AString rowColumnIndex;
                int64_t rowIndex(-1), columnIndex(-1);
                if (cmdf->getRowColumnIndexFromSurfaceVertex(surface->getStructure(),
                                                             surface->getNumberOfNodes(),
                                                             nodeNumber,
                                                             rowIndex,
                                                             columnIndex)) {
                    if (rowIndex >= 0) {
                        rowColumnIndex = ("Row "
                                          + AString::number(rowIndex
                                                            + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                    }
                    if (columnIndex >= 0) {
                        if ( ! rowColumnIndex.isEmpty()) {
                            rowColumnIndex.append("<br>");
                        }
                        rowColumnIndex.append("Column "
                                              + AString::number(columnIndex
                                                                + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                    }
                }
                if ( ! labelText.isEmpty()) {
                    labelHtmlTableBuilder.addRow(labelText,
                                                 cmdf->getFileNameNoPath());
                }
                if ( ! scalarText.isEmpty()) {
                    scalarHtmlTableBuilder.addRow(scalarText,
                                                  cmdf->getFileNameNoPath(),
                                                  rowColumnIndex);
                }
            }
            else {
                if (cmdf->isMappedWithLabelTable()) {
                    labelHtmlTableBuilder.addRow(m_noDataText,
                                                 cmdf->getFileNameNoPath());
                }
                if (cmdf->isMappedWithPalette()) {
                    scalarHtmlTableBuilder.addRow(m_noDataText,
                                                  cmdf->getFileNameNoPath());
                }
            }
        }
        
        if (cdsf != NULL) {
            CaretAssert(cmdf == NULL);
            AString boldText = (DataFileTypeEnum::toOverlayTypeName(cdsf->getDataFileType())
                                + " "
                                + cdsf->getFileNameNoPath());
            
            /*
             * Does file have both label and scalar data
             */
            const bool parcelDataFlag(isParcelAndScalarTypeFile(cdsf->getDataFileType()));
            AString textValue;
            
            const AString separator("<br>");
            const bool valid = cdsf->getSurfaceNodeIdentificationForMaps(mapIndices,
                                                                         surface->getStructure(),
                                                                         nodeNumber,
                                                                         surface->getNumberOfNodes(),
                                                                         separator,
                                                                         s_dataValueDigitsRightOfDecimal,
                                                                         textValue);
            if (valid) {
                AString labelText;
                if (cdsf->isMappedWithLabelTable()) {
                    labelText = textValue;
                }
                AString scalarText;
                if (cdsf->isMappedWithPalette()) {
                    scalarText = textValue;
                }
                if (parcelDataFlag) {
                    /*
                     * Parcel data has parcel name, separator, scalar value 1, separator, scalar value 2, etc.
                     * Display parcel name in label table, and scalar
                     * value in the scalar table
                     */
                    QStringList parcelAndValue = textValue.split(separator);
                    if (parcelAndValue.size() == static_cast<int>(mapIndices.size() + 1)) {
                        labelText  = parcelAndValue.at(0);
                        parcelAndValue.removeAt(0);
                        scalarText = parcelAndValue.join(separator);
                    }
                }
                
                AString rowColumnIndex;
                const int64_t rowIndex(cdsf->getRowIndexFromSurfaceVertex(surface->getStructure(),
                                                                          surface->getNumberOfNodes(),
                                                                          nodeNumber));
                const int32_t columnIndex(-1);
                if (rowIndex >= 0) {
                    if (rowIndex >= 0) {
                        rowColumnIndex = ("Row "
                                          + AString::number(rowIndex
                                                            + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                    }
                    if (columnIndex >= 0) {
                        if ( ! rowColumnIndex.isEmpty()) {
                            rowColumnIndex.append("<br>");
                        }
                        rowColumnIndex.append("Column "
                                              + AString::number(columnIndex
                                                                + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                    }
                }
                if ( ! labelText.isEmpty()) {
                    labelHtmlTableBuilder.addRow(labelText,
                                                 cdsf->getFileNameNoPath());
                }
                if ( ! scalarText.isEmpty()) {
                    scalarHtmlTableBuilder.addRow(scalarText,
                                                  cdsf->getFileNameNoPath(),
                                                  rowColumnIndex);
                }
            }
            else {
                if (cdsf->isMappedWithLabelTable()) {
                    labelHtmlTableBuilder.addRow(m_noDataText,
                                                 cdsf->getFileNameNoPath());
                }
                if (cdsf->isMappedWithPalette()) {
                    scalarHtmlTableBuilder.addRow(m_noDataText,
                                                  cdsf->getFileNameNoPath());
                }
            }
        }
        
        if (labelFile != NULL) {
            AString text;
            for (int32_t k = 0; k < numMapIndices; k++) {
                CaretAssertVectorIndex(mapIndices, k);
                const int32_t mapIndex = mapIndices[k];
                if (k >= 1) {
                    text.append("<br>");
                }
                AString labelName = labelFile->getLabelName(nodeNumber, mapIndex);
                if (labelName.isEmpty()) {
                    labelName = ("Map-" + AString::number(mapIndex + 1));
                }
                text.append(labelName);
                text.append(" ("
                            + labelFile->getMapName(mapIndex)
                            + ")");
            }
            labelHtmlTableBuilder.addRow(text,
                                         labelFile->getFileNameNoPath());
        }
        
        if (metricFile != NULL) {
            AString text;
            for (int32_t k = 0; k < numMapIndices; k++) {
                CaretAssertVectorIndex(mapIndices, k);
                const int32_t mapIndex = mapIndices[k];
                if (k >= 1) {
                    text.append("<br>");
                }
                text.append(dataValueToText(metricFile->getValue(nodeNumber, mapIndex)));
            }
            scalarHtmlTableBuilder.addRow(text,
                                          metricFile->getFileNameNoPath(),
                                          "");
        }
        if (metricDynConnFile != NULL) {
            if (metricDynConnFile->isDataValid()) {
                if (metricDynConnFile->isEnabledAsLayer()) {
                    AString boldText = "METRIC DYNAMIC " + metricDynConnFile->getFileNameNoPath();
                    AString text;
                    for (int32_t k = 0; k < numMapIndices; k++) {
                        CaretAssertVectorIndex(mapIndices, k);
                        const int32_t mapIndex = mapIndices[k];
                        if (k >= 1) {
                            text.append("<br>");
                        }
                        text += (" " + dataValueToText(metricDynConnFile->getValue(nodeNumber, mapIndex)));
                    }
                    scalarHtmlTableBuilder.addRow(text,
                                                  metricDynConnFile->getFileNameNoPath(),
                                                  "");
                }
            }
        }
    }
}

/**
 * Generate identification text for a data series chart.
 * @param htmlTableBuilder
 *     String builder for identification text.
 * @param idChartDataSeries
 *     Information for chart id.
 */
void
IdentificationFormattedTextGenerator::generateChartDataSeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                       const SelectionItemChartDataSeries* idChartDataSeries) const
{
    if (idChartDataSeries->isValid()) {
        const ChartDataCartesian* chartDataCartesian = idChartDataSeries->getChartDataCartesian();
        
        const ChartDataSource* chartDataSource = chartDataCartesian->getChartDataSource();
        generateChartDataSourceText(htmlTableBuilder,
                                    "DATA SERIES CHART",
                                    chartDataSource);
    }
}

/**
 * Generate identification text for a data series chart.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idChartDataSeries
 *     Information for chart id.
 */
void
IdentificationFormattedTextGenerator::generateChartFrequencySeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                       const SelectionItemChartFrequencySeries* idChartFrequencySeries) const
{
    if (idChartFrequencySeries->isValid()) {
        const ChartDataCartesian* chartDataCartesian = idChartFrequencySeries->getChartDataCartesian();
        
        const ChartDataSource* chartDataSource = chartDataCartesian->getChartDataSource();
        generateChartDataSourceText(htmlTableBuilder,
                                    "FREQUENCY SERIES CHART",
                                    chartDataSource);
    }
}

/**
 * Generate identification text for a matrix chart.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idChartMatrix
 *     Information for matrix chart id.
 */
void
IdentificationFormattedTextGenerator::generateChartMatrixIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                   const SelectionItemChartMatrix* idChartMatrix) const
{
    if (idChartMatrix->isValid()) {
        const ChartableMatrixInterface* chartMatrixInterface = idChartMatrix->getChartableMatrixInterface();
        const CaretMappableDataFile* caretMappableDataFile = chartMatrixInterface->getMatrixChartCaretMappableDataFile();
        
        const int32_t rowIndex = idChartMatrix->getMatrixRowIndex();
        const int32_t columnIndex = idChartMatrix->getMatrixColumnIndex();
        AString rowName;
        AString columnName;
        AString cellValue;
        const bool validData = chartMatrixInterface->getMatrixCellAttributes(rowIndex,
                                                                             columnIndex,
                                                                             cellValue,
                                                                             rowName,
                                                                             columnName);
        
        AString boldText("MATRIX CHART");
        
        if (validData) {
            htmlTableBuilder.addRow(cellValue,
                                    (("Row: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()))
                                     + "<br>"
                                     + ("Column: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()))),
                                    caretMappableDataFile->getFileNameNoPath());
        }
        else {
            htmlTableBuilder.addRow(boldText,
                                    "",
                                    caretMappableDataFile->getFileNameNoPath());
        }
    }
}

/**
 * Generate identification text for a chart two histogram.
 *
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     Identification string builder
 * @param idChartTwoHistogram
 *     Information for selected chart two histogram.
 * @param mapFile
 *     FIle for generating identification
 * @param mapIndices
 *     Indices of map
 * @param toolTipFlag
 *     If true, create tooltip text
 */
void
IdentificationFormattedTextGenerator::generateChartTwoHistogramIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                                  IdentificationStringBuilder& idText,
                                                                                  const SelectionItemChartTwoHistogram* idChartTwoHistogram,
                                                                                  CaretMappableDataFile* mapFile,
                                                                                  const std::set<int32_t>& mapIndices,
                                                                                  const bool toolTipFlag) const
{
    const int32_t mapIndex    = idChartTwoHistogram->getMapIndex();
    const int32_t bucketIndex = idChartTwoHistogram->getBucketIndex();
    const bool    allMapsFlag = idChartTwoHistogram->isAllMapsSelected();
    
    if (idChartTwoHistogram->isValid()) {
        ChartableTwoFileHistogramChart* fileHistogramChart = idChartTwoHistogram->getFileHistogramChart();
        CaretAssert(fileHistogramChart);
        CaretMappableDataFile* chartMapFile = fileHistogramChart->getCaretMappableDataFile();
        CaretAssert(chartMapFile);
        
        {
            ChartableTwoFileHistogramChart* chartingDelegate = chartMapFile->getChartingDelegate()->getHistogramCharting();
            CaretAssert(chartingDelegate);
            const Histogram* histogram = chartingDelegate->getHistogramForChartDrawing(mapIndex,
                                                                                       allMapsFlag);
            CaretAssert(histogram);
            
            float bucketValue = 0.0;
            float bucketHeight = 0.0;
            QString columnOne;
            if (histogram->getHistogramDisplayBucketDataValueAndHeight(bucketIndex, bucketValue, bucketHeight)) {
                if (toolTipFlag) {
                    AString boldText("Histogram");
                    idText.addLine(false,
                                   boldText,
                                   chartMapFile->getFileNameNoPath());
                    
                    idText.addLine(true,
                                   "Bucket Index",
                                   (AString::number(bucketIndex)));
                    
                    idText.addLine(true,
                                   "Data Value at Bucket",
                                   (AString::number(bucketValue)));
                    
                    const int64_t bucketHeightInteger = static_cast<int64_t>(bucketHeight);
                    idText.addLine(true,
                                   "Bucket Count",
                                   (AString::number(bucketHeightInteger)));
                }
                else {
                    if (mapFile == chartMapFile) {
                        if ((mapIndices.find(mapIndex) != mapIndices.end())
                            || allMapsFlag) {
                            columnOne.append("Bucket: " + (AString::number(bucketIndex)));
                            columnOne.append("<br>Data Value: " +  (AString::number(bucketValue)));
                            
                            const int64_t bucketHeightInteger = static_cast<int64_t>(bucketHeight);
                            columnOne.append("<br>Bucket Count: " + (AString::number(bucketHeightInteger)));
                            
                            htmlTableBuilder.addRow(columnOne,
                                                    "Histogram",
                                                    chartMapFile->getFileNameNoPath());
                        }
                    }
                }
            }
        }
    }
}

/**
 * Generate identification text for a chart two line-layer.
 *
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     Identification string builder
 * @param idChartTwoLineLayer
 *     Information for selected chart two line-layer.
 * @param mapFile
 *     FIle for generating identification
 * @param mapIndices
 *     Indices of map
 * @param toolTipFlag
 *     If true, generate tooltip
 */
void
IdentificationFormattedTextGenerator::generateChartTwoLineLayerNearestIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                                   IdentificationStringBuilder& idText,
                                                                                   const SelectionItemChartTwoLineLayerVerticalNearest* idChartTwoLineLayer,
                                                                                   CaretMappableDataFile* mapFile,
                                                                                   const std::set<int32_t>& /*mapIndices*/,
                                                                                   const bool toolTipFlag) const
{
    if (idChartTwoLineLayer->isValid()) {
        const ChartableTwoFileLineLayerChart* fileLineSeriesChart = idChartTwoLineLayer->getFileLineLayerChart();
        CaretAssert(fileLineSeriesChart);
        const CaretMappableDataFile* chartMapFile = fileLineSeriesChart->getCaretMappableDataFile();
        CaretAssert(chartMapFile);
        const ChartTwoDataCartesian* cartesianData = idChartTwoLineLayer->getChartTwoCartesianData();
        CaretAssert(cartesianData);
        const MapFileDataSelector* mapFileDataSelector = cartesianData->getMapFileDataSelector();
        CaretAssert(mapFileDataSelector);
        
        if ( ! toolTipFlag) {
            if (chartMapFile != mapFile) {
                return;
            }
        }
        int32_t primitiveIndex = idChartTwoLineLayer->getLineSegmentIndex();
        
        AString boldText("Line Layer Chart");
        
        cartesianData->getGraphicsPrimitive();
        const GraphicsPrimitive* primitive = cartesianData->getGraphicsPrimitive();
        CaretAssert(primitive);
        
        if (primitiveIndex >= 0) {
            float xyz1[3];
            primitive->getVertexFloatXYZ(primitiveIndex,
                                         xyz1);
            
            const int32_t nextIndex(((primitiveIndex + 1) < primitive->getNumberOfVertices())
                                    ? (primitiveIndex + 1)
                                    : -1);
            float xyz2[3];
            if (nextIndex >= 0) {
                primitive->getVertexFloatXYZ(nextIndex,
                                             xyz2);
            }
            
            if (toolTipFlag) {
                idText.addLine(true,
                               "XY Start",
                               xyToText(xyz1));
                if (nextIndex >= 0) {
                    idText.addLine(true,
                                   "XY End ",
                                   xyToText(xyz2));
                }
            }
            else {
                AString text("XY Start:" +
                             xyToText(xyz1));
                if (nextIndex >= 0) {
                    text.append(" XY End:"
                                + xyToText(xyz2));
                }
                htmlTableBuilder.addRow(text,
                                        boldText,
                                        chartMapFile->getFileNameNoPath());
            }
        }
        
        generateMapFileSelectorText(htmlTableBuilder,
                                    mapFileDataSelector);
    }
}

/**
 * Generate identification text for a chart two line-layer.
 *
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     Identification string builder
 * @param idChartTwoLineLayer
 *     Information for selected chart two line-layer.
 * @param mapFile
 *     FIle for generating identification
 * @param mapIndices
 *     Indices of map
 * @param toolTipFlag
 *     If true, generate tooltip
 */
void
IdentificationFormattedTextGenerator::generateChartTwoLineLayerIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                                  IdentificationStringBuilder& idText,
                                                                                  const SelectionItemChartTwoLineLayer* idChartTwoLineLayer,
                                                                                  CaretMappableDataFile* mapFile,
                                                                                  const std::set<int32_t>& /*mapIndices*/,
                                                                                  const bool toolTipFlag) const
{
    if (idChartTwoLineLayer->isValid()) {
        const ChartableTwoFileLineLayerChart* fileLineSeriesChart = idChartTwoLineLayer->getFileLineLayerChart();
        CaretAssert(fileLineSeriesChart);
        const CaretMappableDataFile* chartMapFile = fileLineSeriesChart->getCaretMappableDataFile();
        CaretAssert(chartMapFile);
        const ChartTwoDataCartesian* cartesianData = idChartTwoLineLayer->getChartTwoCartesianData();
        CaretAssert(cartesianData);
        const MapFileDataSelector* mapFileDataSelector = cartesianData->getMapFileDataSelector();
        CaretAssert(mapFileDataSelector);
        
        if ( ! toolTipFlag) {
            if (chartMapFile != mapFile) {
                return;
            }
        }
        int32_t primitiveIndex = idChartTwoLineLayer->getLineSegmentIndex();
        
        AString boldText("Line Layer Chart");
        
        cartesianData->getGraphicsPrimitive();
        const GraphicsPrimitive* primitive = cartesianData->getGraphicsPrimitive();
        CaretAssert(primitive);
        
        if (primitiveIndex >= 0) {
            float xyz1[3];
            primitive->getVertexFloatXYZ(primitiveIndex,
                                         xyz1);
            
            const int32_t nextIndex(((primitiveIndex + 1) < primitive->getNumberOfVertices())
                                    ? (primitiveIndex + 1)
                                    : -1);
            float xyz2[3];
            if (nextIndex >= 0) {
                primitive->getVertexFloatXYZ(nextIndex,
                                             xyz2);
            }
            
            if (toolTipFlag) {
                idText.addLine(true,
                               "XY Start",
                               xyToText(xyz1));
                if (nextIndex >= 0) {
                    idText.addLine(true,
                                   "XY End ",
                                   xyToText(xyz2));
                }
            }
            else {
                AString text("XY Start:"
                             + xyToText(xyz1));
                if (nextIndex >= 0) {
                    text.append(" XY End:"
                                + xyToText(xyz2));
                }
                htmlTableBuilder.addRow(text,
                                        boldText,
                                        chartMapFile->getFileNameNoPath());
            }
        }
        
        generateMapFileSelectorText(htmlTableBuilder,
                                    mapFileDataSelector);
    }
}

/**
 * Generate identification text for a chart two line-series.
 *
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     Identification string builder
 * @param idChartTwoLineSeries
 *     Information for selected chart two line-series.
 * @param mapFile
 *     FIle for generating identification
 * @param mapIndices
 *     Indices of map
 * @param toolTipFlag
 *     If true, generate tooltip
 */
void
IdentificationFormattedTextGenerator::generateChartTwoLineSeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                                   IdentificationStringBuilder& idText,
                                                                                   const SelectionItemChartTwoLineSeries* idChartTwoLineSeries,
                                                                                   CaretMappableDataFile* mapFile,
                                                                                   const std::set<int32_t>& /*mapIndices*/,
                                                                                   const bool toolTipFlag) const
{
    if (idChartTwoLineSeries->isValid()) {
        const ChartableTwoFileLineSeriesChart* fileLineSeriesChart = idChartTwoLineSeries->getFileLineSeriesChart();
        CaretAssert(fileLineSeriesChart);
        const CaretMappableDataFile* chartMapFile = fileLineSeriesChart->getCaretMappableDataFile();
        CaretAssert(chartMapFile);
        const ChartTwoDataCartesian* cartesianData = idChartTwoLineSeries->getChartTwoCartesianData();
        CaretAssert(cartesianData);
        const MapFileDataSelector* mapFileDataSelector = cartesianData->getMapFileDataSelector();
        CaretAssert(mapFileDataSelector);
        
        if ( ! toolTipFlag) {
            if (chartMapFile != mapFile) {
                return;
            }
        }
        int32_t primitiveIndex = idChartTwoLineSeries->getLineSegmentIndex();
        
        AString boldText("Line Series Chart");
        
        cartesianData->getGraphicsPrimitive();
        const GraphicsPrimitive* primitive = cartesianData->getGraphicsPrimitive();
        CaretAssert(primitive);
        
        if (primitiveIndex >= 1) {
            float xyz1[3];
            primitive->getVertexFloatXYZ(primitiveIndex - 1,
                                         xyz1);
            float xyz2[3];
            primitive->getVertexFloatXYZ(primitiveIndex,
                                         xyz2);
            if (toolTipFlag) {
                idText.addLine(true,
                               "XY Start",
                               xyToText(xyz1));
                idText.addLine(true,
                               "XY End ",
                               xyToText(xyz2));
            }
            else {
                htmlTableBuilder.addRow(("XY Start:"
                                         + xyToText(xyz1))
                                        + ("XY End:"
                                           + xyToText(xyz2)),
                                        boldText,
                                        chartMapFile->getFileNameNoPath());
            }
        }
        else {
            float xyz[3];
            primitive->getVertexFloatXYZ(primitiveIndex,
                                         xyz);
            if (toolTipFlag) {
                idText.addLine(true,
                               "XY",
                               xyToText(xyz));
            }
            else {
                htmlTableBuilder.addRow(("XY:"
                                         + xyToText(xyz)),
                                        boldText,
                                        chartMapFile->getFileNameNoPath());
            }
        }
        
        generateMapFileSelectorText(htmlTableBuilder,
                                    mapFileDataSelector);
    }
}

/**
 * Generate identification text for a chart two matrix.
 *
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     Identification string builder
 * @param idChartTwoMatrix
 *     Information for selected chart two matrix.
 * @param mapFile
 *     FIle for generating identification
 * @param mapIndices
 *     Indices of map
 * @param toolTipFlag
 *     If true, generate tool tip
 */
void
IdentificationFormattedTextGenerator::generateChartTwoMatrixIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                               IdentificationStringBuilder& idText,
                                                                               const SelectionItemChartTwoMatrix* idChartTwoMatrix,
                                                                               CaretMappableDataFile* mapFile,
                                                                               const std::set<int32_t>& /*mapIndices*/,
                                                                               const bool toolTipFlag) const
{
    if (idChartTwoMatrix->isValid()) {
        const ChartableTwoFileMatrixChart* matrixChart = idChartTwoMatrix->getFileMatrixChart();
        CaretAssert(matrixChart);
        
        const int32_t rowIndex = idChartTwoMatrix->getRowIndex();
        const int32_t colIndex = idChartTwoMatrix->getColumnIndex();
        
        const CaretMappableDataFile* chartMapFile = matrixChart->getCaretMappableDataFile();
        CaretAssert(chartMapFile);
       
        if ( ! toolTipFlag) {
            if (chartMapFile != mapFile) {
                return;
            }
        }
        
        AString boldText("MATRIX ");
        QString rowText;
        QString colText;
        if (rowIndex >= 0) {
            if (matrixChart->hasRowSelection()) {
                rowText = matrixChart->getRowName(rowIndex);
            }
            if (rowText.isEmpty()) {
                rowText = ("Row: "
                           + AString::number(rowIndex
                                             + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
            }
        }
        if (colIndex >= 0) {
            if (matrixChart->hasColumnSelection()) {
                colText = matrixChart->getColumnName(colIndex);
            }
            if (colText.isEmpty()) {
                colText = ("Col: "
                           + AString::number(colIndex
                                             + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
            }
        }
        
        AString dataValueText;
        if ((rowIndex >= 0)
            && (colIndex >= 0)) {
            const CiftiMappableDataFile* ciftiFile = matrixChart->getCiftiMappableDataFile();
            if (ciftiFile != NULL) {
                MapFileDataSelector mapSelector;
                mapSelector.setRowIndex(const_cast<CaretMappableDataFile*>(chartMapFile),
                                        "",
                                        rowIndex);
                std::vector<float> rowData;
                ciftiFile->getDataForSelector(mapSelector, rowData);
                if ( ! rowData.empty()) {
                    if (colIndex < static_cast<int32_t>(rowData.size())) {
                        CaretAssertVectorIndex(rowData, colIndex);
                        dataValueText = dataValueToText(rowData[colIndex]);
                    }
                }
            }
        }
        
        if (( ! colText.isEmpty())
            || ( ! rowText.isEmpty())) {
            if (toolTipFlag) {
                if ( ! colText.isEmpty()) {
                    colText.append(" ");
                }
                if ( ! dataValueText.isEmpty()) {
                    dataValueText.append(" ");
                }
                idText.addLine(true,
                               (dataValueText + rowText + "; " + colText),
                               chartMapFile->getFileNameNoPath());
            }
            else {
                htmlTableBuilder.addRow(dataValueText,
                                        (rowText + "; " + colText),
                                        chartMapFile->getFileNameNoPath());
            }
        }
    }
}

/**
 * Generate identification text for a CIFTI Connectivity Matrix Row/Column
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idCiftiConnMatrix
 *     Information for CIFTI Connectivity Matrix Row/Column.
 */
void
IdentificationFormattedTextGenerator::generateCiftiConnectivityMatrixIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                               const SelectionItemCiftiConnectivityMatrixRowColumn* idCiftiConnMatrix) const
{
    if (idCiftiConnMatrix->isValid()) {
        const CiftiMappableConnectivityMatrixDataFile* connMatrixFile = idCiftiConnMatrix->getCiftiConnectivityMatrixFile();
        const int32_t rowIndex = idCiftiConnMatrix->getMatrixRowIndex();
        const int32_t colIndex = idCiftiConnMatrix->getMatrixColumnIndex();
        
        AString colTwoText("MATRIX ROW/COLUMN");
        AString colThreeText(connMatrixFile->getFileNameNoPath());
        
        AString colOneText;
        AString rowName = " ";
        AString colName = " ";
        bool validData = true;
        if (validData) {
            if (rowIndex >= 0) {
                colOneText = ("Row " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())
                              + "<br>"
                              + rowName);
            }
            if (colIndex >= 0) {
                colOneText = ("Column " + AString::number(colIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())
                              + "<br>"
                              + colName);
            }
        }
        
        htmlTableBuilder.addRow(colOneText,
                                colTwoText,
                                colThreeText);
    }
}

/**
 * Generate identification text for chart data source.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param typeOfChartText
 *     Text describing the type of chart.
 * @param chartDataSource
 *     Source of chart data.
 */
void
IdentificationFormattedTextGenerator::generateChartDataSourceText(HtmlTableBuilder& htmlTableBuilder,
                                                                  const AString& typeOfChartText,
                                                                  const ChartDataSource* chartDataSource) const
{
    AString chartFileName = chartDataSource->getChartableFileName();
    if (! chartFileName.isEmpty()) {
        chartFileName = FileInformation(chartFileName).getFileName();
    }
    
    AString columnOne;
    switch (chartDataSource->getDataSourceMode()) {
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_INVALID:
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_FILE_ROW:
        {
            AString fileName;
            int32_t rowIndex;
            chartDataSource->getFileRow(fileName,
                                        rowIndex);
            chartFileName = fileName;
            columnOne = ("Row " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
        }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDEX:
        {
            AString structureName;
            int32_t numberOfNodes;
            int32_t nodeIndex;
            chartDataSource->getSurfaceNode(structureName,
                                            numberOfNodes,
                                            nodeIndex);
            columnOne = ("Vertex Index " + AString::number(nodeIndex));
            columnOne.append("<br>Structure " + structureName);
        }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_SURFACE_NODE_INDICES_AVERAGE:
        {
            AString structureName;
            int32_t numberOfNodes;
            std::vector<int32_t> nodeIndices;
            chartDataSource->getSurfaceNodeAverage(structureName, numberOfNodes, nodeIndices);
            columnOne = ("Vertex Avg Count " + AString::number(nodeIndices.size()));
            columnOne.append("<br>Structure " + structureName);
        }
            break;
        case ChartDataSourceModeEnum::CHART_DATA_SOURCE_MODE_VOXEL_IJK:
        {
            float voxelXYZ[3];
            chartDataSource->getVolumeVoxel(voxelXYZ);
            columnOne = ("Voxel XYZ " +
                         xyzToText(voxelXYZ));
        }
            break;
    }
    
    if (! columnOne.isEmpty()) {
        htmlTableBuilder.addRow(columnOne,
                                typeOfChartText,
                                chartFileName);
    }
}

/**
 * Generate text for a map file data selector.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param mapFileDataSelector
 *     The map file data selector.
 */
void
IdentificationFormattedTextGenerator::generateMapFileSelectorText(HtmlTableBuilder& htmlTableBuilder,
                                                                  const MapFileDataSelector* mapFileDataSelector) const
{
    
    switch (mapFileDataSelector->getDataSelectionType()) {
        case MapFileDataSelector::DataSelectionType::INVALID:
            break;
        case MapFileDataSelector::DataSelectionType::COLUMN_DATA:
        {
            CaretMappableDataFile* mapFile = NULL;
            AString mapFileName;
            int32_t columnIndex = -1;
            mapFileDataSelector->getColumnIndex(mapFile,
                                                mapFileName,
                                                columnIndex);
            htmlTableBuilder.addRow("Column: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()),
                                    "",
                                    mapFileName);
        }
            break;
        case MapFileDataSelector::DataSelectionType::ROW_DATA:
        {
            CaretMappableDataFile* mapFile = NULL;
            AString mapFileName;
            int32_t rowIndex = -1;
            mapFileDataSelector->getRowIndex(mapFile,
                                             mapFileName,
                                             rowIndex);
            htmlTableBuilder.addRow("Row: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()),
                                    "",
                                    mapFileName);
        }
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTEX:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t numberOfVertices = 0;
            int32_t vertexIndex = -1;
            mapFileDataSelector->getSurfaceVertex(structure,
                                                  numberOfVertices,
                                                  vertexIndex);
            
            if ((structure != StructureEnum::INVALID)
                && (vertexIndex >= 0)) {
                htmlTableBuilder.addRow("Vertex: " + AString::number(vertexIndex),
                                        "Structure: " + StructureEnum::toGuiName(structure),
                                        "");
            }
        }
            break;
        case MapFileDataSelector::DataSelectionType::SURFACE_VERTICES_AVERAGE:
        {
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t numberOfVertices = 0;
            std::vector<int32_t> vertexIndices;
            mapFileDataSelector->getSurfaceVertexAverage(structure,
                                                         numberOfVertices,
                                                         vertexIndices);
            
            const int32_t averageCount = static_cast<int32_t>(vertexIndices.size());
            if ((structure != StructureEnum::INVALID)
                && (averageCount > 0)) {
                htmlTableBuilder.addRow("Vertex Avg Count: " + AString::number(averageCount),
                                        "Structure: " + StructureEnum::toGuiName(structure),
                                        "");
            }
        }
            break;
        case MapFileDataSelector::DataSelectionType::VOLUME_XYZ:
        {
            float voxelXYZ[3];
            mapFileDataSelector->getVolumeVoxelXYZ(voxelXYZ);
            htmlTableBuilder.addRow("Voxel XYZ: " +
                                    xyzToText(voxelXYZ));
        }
            break;
    }
}


/**
 * Generate identification text for a time series chart.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idChartTimeSeries
 *     Information for chart id.
 */
void
IdentificationFormattedTextGenerator::generateChartTimeSeriesIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                       const SelectionItemChartTimeSeries* idChartTimeSeries) const
{
    if (idChartTimeSeries->isValid()) {
        const ChartDataCartesian* chartDataCartesian = idChartTimeSeries->getChartDataCartesian();
        
        const ChartDataSource* chartDataSource = chartDataCartesian->getChartDataSource();
        generateChartDataSourceText(htmlTableBuilder,
                                    "TIME SERIES CHART",
                                    chartDataSource);
    }
}

/**
 * Generate identification text for a surface border identification.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     Identification string builder
 * @param idSurfaceBorder
 *     Information for surface border ID.
 * @param toolTipFlag
 *     True if this is for tooltip.
 */
void
IdentificationFormattedTextGenerator::generateSurfaceBorderIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                                                             IdentificationStringBuilder& idText,
                                                                    const SelectionItemBorderSurface* idSurfaceBorder,
                                                                    const bool toolTipFlag) const
{
    if (idSurfaceBorder->isValid()) {
        const Border* border = idSurfaceBorder->getBorder();
        const SurfaceProjectedItem* spi = border->getPoint(idSurfaceBorder->getBorderPointIndex());
        float xyz[3];
        spi->getProjectedPosition(*idSurfaceBorder->getSurface(), xyz, false);

        if (toolTipFlag) {
            bool indentFlag = false;
            idText.addLine(indentFlag,
                           "Border",
                           border->getName());
            indentFlag = true;
            idText.addLine(indentFlag,
                           "XYZ",
                           xyzToText(xyz));
        }
        else {
            const AString numberIndexText = ("("
                                             + AString::number(idSurfaceBorder->getBorderIndex())
                                             + ","
                                             + AString::number(idSurfaceBorder->getBorderPointIndex()));

            htmlTableBuilder.addRow(xyzToText(xyz),
                                          ("BORDER " + numberIndexText),
                                          ("Name: " + border->getName()
                                           + "<br>Class: " + border->getClassName()));
            
        }
    }
}

/**
 * Generate identification text for a surface focus identification.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idSurfaceFocus
 *     Information for surface focus ID.
 * @param toolTipFlag
 *     True if this is for tooltip.
 */
void
IdentificationFormattedTextGenerator::generateSurfaceFocusIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                                                  const SelectionItemFocusSurface* idSurfaceFocus,
                                                                  const bool toolTipFlag) const
{
    if (idSurfaceFocus->isValid()) {
        const Focus* focus = idSurfaceFocus->getFocus();
        const int32_t projectionIndex = idSurfaceFocus->getFocusProjectionIndex();
        
        IdentificationStringBuilder idText;
        generateFocusIdentifcationText(htmlTableBuilder,
                                       idText,
                                       focus,
                                       idSurfaceFocus->getFocusIndex(),
                                       projectionIndex,
                                       toolTipFlag);
    }
}

/**
 * Generate identification text for a surface focus identification.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     String builder for identification text.
 * @param focus
 *     The focus.
 * @param focusIndex
 *     Index of focus
 * @param projectionIndex
 *     Index of projection
 * @param toolTipFlag
 *     True if this is for tooltip.
 */
void
IdentificationFormattedTextGenerator::generateFocusIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                                                     IdentificationStringBuilder& idText,
                                                                     const Focus* focus,
                                                                     const int32_t focusIndex,
                                                                     const int32_t projectionIndex,
                                                                     const bool toolTipFlag) const
{
    const SurfaceProjectedItem* spi = focus->getProjection(projectionIndex);
    float xyzStereo[3];
    spi->getStereotaxicXYZ(xyzStereo);
    const AString stereoXYZText((spi->isStereotaxicXYZValid()
                                 ? xyzToText(xyzStereo)
                                 : "Invalid"));
    if (toolTipFlag) {
        bool indentFlag = false;
        idText.addLine(indentFlag,
                       "Focus",
                       focus->getName());
        indentFlag = true;
        idText.addLine(indentFlag,
                       "XYZ",
                       stereoXYZText);
    }
    else {
        AString focusIdText;
        if ( ! focus->getFocusID().isEmpty()) {
            focusIdText = ("<br>ID: " + focus->getFocusID());
        }
        htmlTableBuilder.addRow(stereoXYZText,
                                      ("FOCUS " + AString::number(focusIndex)),
                                      ("Name: " + focus->getName()
                                       + "<br>Class: " + focus->getClassName()
                                       + focusIdText));
    }
}

/**
 * Generate identification text for a focus identification.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     Text for tooltip
 * @param idFocus
 *     Information for surface focus ID.
 * @param toolTipFlag
 *     True when generating text for tooltip
 */
void
IdentificationFormattedTextGenerator::generateFocusIdentifcationText(HtmlTableBuilder& htmlTableBuilder,
                                                                     IdentificationStringBuilder& idText,
                                                                     const SelectionItemFocus* idFocus,
                                                                     const bool toolTipFlag) const
{
    if ( ! idFocus->isValid()) {
        return;
    }
    const Focus* focus = idFocus->getFocus();
    CaretAssert(focus);
    const SurfaceProjectedItem* spi = focus->getProjection(idFocus->getFocusProjectionIndex());
    CaretAssert(spi);
    float xyzVolume[3];
    spi->getVolumeXYZ(xyzVolume);
    float xyzStereo[3];
    spi->getStereotaxicXYZ(xyzStereo);
    
    const AString stereoXYZText((spi->isStereotaxicXYZValid()
                                 ? xyzToText(xyzStereo)
                                 : "Invalid"));
    
    if (toolTipFlag) {
        bool indentFlag = false;
        idText.addLine(indentFlag,
                       "Focus",
                       focus->getName());
        indentFlag = true;
        idText.addLine(indentFlag,
                       "XYZ",
                       stereoXYZText);
        return;
    }
    
    AString projectedXYZText;
    switch (idFocus->getIdType()) {
        case SelectionItemFocus::IdType::INVALID:
            break;
        case SelectionItemFocus::IdType::HISTOLOGY:
            break;
        case SelectionItemFocus::IdType::SURFACE:
        {
            const Surface* surface(idFocus->getSurface());
            if (surface != NULL) {
                float xyzProj[3];
                if (spi->getProjectedPosition(*surface, xyzProj, false)) {
                    AString xyzProjName;
                    if (spi->getBarycentricProjection()->isValid()) {
                        xyzProjName = "XYZ (Projected to Triangle): ";
                    }
                    else if (spi->getVanEssenProjection()->isValid()) {
                        xyzProjName = "XYZ (Projected to Edge): ";
                    }
                    if ( ! xyzProjName.isEmpty()) {
                        projectedXYZText = (xyzProjName +
                                            xyzToText(xyzProj));
                    }
                    
                }
            }
        }
            break;
        case SelectionItemFocus::IdType::VOLUME:
            break;
    }
    
    htmlTableBuilder.addRow(("Focus Name: " + focus->getName()),
                            ("Class: " + focus->getClassName()));
    htmlTableBuilder.addRow(("Stereotaxic XYZ: " + stereoXYZText),
                            projectedXYZText);
    addIfColumnTwoNotEmpty(htmlTableBuilder,
                           "Area: ",
                           focus->getArea());
    if (focus->getExtent() != 0.0) {
        addIfColumnTwoNotEmpty(htmlTableBuilder,
                               "Extent: ",
                               AString::number(focus->getExtent(), 'f', 3));
    }
    addIfColumnTwoNotEmpty(htmlTableBuilder,
                           "Geography: ",
                           focus->getGeography());
    addIfColumnTwoNotEmpty(htmlTableBuilder,
                           "Region of Interest: ",
                           focus->getRegionOfInterest());
    addIfColumnTwoNotEmpty(htmlTableBuilder,
                           "Statistic: ",
                           focus->getStatistic());
    addIfColumnTwoNotEmpty(htmlTableBuilder,
                           "Comment : ",
                           focus->getComment());
}

/**
 * Add if second column is not empty
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param columnOne
 *    Text for column one
 * @param columnTwo
 *    Text for column two
 */
void
IdentificationFormattedTextGenerator::addIfColumnTwoNotEmpty(HtmlTableBuilder& htmlTableBuilder,
                                                             const AString& columnOne,
                                                             const AString& columnTwo) const
{
    if ( ! columnTwo.trimmed().isEmpty()) {
        htmlTableBuilder.addRow(columnOne,
                                columnTwo);
    }
}

/**
 * Generate identification text for histology identification.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     string builder for id text
 * @param idHistology
 *    Histology identification
 */
void
IdentificationFormattedTextGenerator::generateHistologyPlaneCoordinateIdentificationText(const IdentificationManager* idManager,
                                                                                         HtmlTableBuilder& htmlTableBuilder,
                                                                                         IdentificationStringBuilder& idText,
                                                                                         const SelectionItemHistologyCoordinate* idHistology) const
{
    if (idHistology->isValid()) {
        std::vector<AString> columnOneText, columnTwoText, toolTipText;
        
        const HistologyCoordinate histologyCoordinate(idHistology->getCoordinate());
        const HistologySlicesFile* histologySlicesFile(idHistology->getHistologySlicesFile());
        CaretAssert(histologySlicesFile);
        histologySlicesFile->getIdentificationText(idHistology->getTabIndex(),
                                                   histologyCoordinate,
                                                   columnOneText,
                                                   columnTwoText,
                                                   toolTipText);
        
        const int32_t numColOne(columnOneText.size());
        const int32_t numColTwo(columnTwoText.size());
        const int32_t maxNum(std::max(numColOne, numColTwo));
        for (int32_t i = 0; i < maxNum; i++) {
            AString colOne;
            AString colTwo;
            if (i < numColOne) {
                CaretAssertVectorIndex(columnOneText, i);
                colOne = columnOneText[i];
            }
            if (i < numColTwo) {
                CaretAssertVectorIndex(columnTwoText, i);
                colTwo = columnTwoText[i];
            }
            htmlTableBuilder.addRow(colOne, colTwo);
        }
        
        /*
         * For tooltip
         */
        const bool indentFlag(false);
        for (const auto& text : toolTipText) {
            idText.addLine(indentFlag,
                           text);
        }
        if (histologyCoordinate.isStereotaxicXYZValid()) {
            const AString distanceText = getTextDistanceToMostRecentIdentificationSymbol(idManager,
                                                                                         histologyCoordinate.getStereotaxicXYZ());
            if ( ! distanceText.isEmpty()) {
                idText.addLine(indentFlag,
                               distanceText);
            }
        }
    }
}

/**
 * Generate identification text for media identification.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     string builder for id text
 * @param mediaFile
 *    The media file
 * @param frameIndices
 *    The frame indices
 * @param idMedia
 *     Information for media ID.
 */
void
IdentificationFormattedTextGenerator::generateMediaLogicalCoordinateIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                      IdentificationStringBuilder& idText,
                                                                      const MediaFile* mediaFile,
                                                                      const std::set<int32_t>& frameIndices,
                                                                      const SelectionItemMediaLogicalCoordinate* idMedia) const
{
    if (idMedia->isValid()) {
        std::array<float, 3> modelXYZ;
        idMedia->getModelXYZ(modelXYZ.data());
        std::vector<AString> columnOneText, columnTwoText, toolTipText;
        std::vector<int32_t> frameIndicesVector(frameIndices.begin(),
                                                frameIndices.end());
        mediaFile->getPixelLogicalIdentificationTextForFrames(idMedia->getTabIndex(),
                                                       frameIndicesVector,
                                                       idMedia->getPixelLogicalIndex(),
                                                       columnOneText,
                                                       columnTwoText,
                                                       toolTipText);
        const int32_t numColOne(columnOneText.size());
        const int32_t numColTwo(columnTwoText.size());
        const int32_t maxNum(std::max(numColOne, numColTwo));
        for (int32_t i = 0; i < maxNum; i++) {
            AString colOne;
            AString colTwo;
            if (i < numColOne) {
                CaretAssertVectorIndex(columnOneText, i);
                colOne = columnOneText[i];
            }
            if (i < numColTwo) {
                CaretAssertVectorIndex(columnTwoText, i);
                colTwo = columnTwoText[i];
            }
            htmlTableBuilder.addRow(colOne, colTwo);
        }
        
        /*
         * For tooltip
         */
        for (const auto& text : toolTipText) {
            bool indentFlag(false);
            idText.addLine(indentFlag,
                           text);
        }
    }

}

/**
 * Generate identification text for media identification.
 * @param htmlTableBuilder
 *     HTML table builder for identification text.
 * @param idText
 *     string builder for id text
 * @param mediaFile
 *    The media file
 * @param frameIndices
 *    The frame indices
 * @param idMedia
 *     Information for media ID.
 */
void
IdentificationFormattedTextGenerator::generateMediaPlaneCoordinateIdentificationText(HtmlTableBuilder& htmlTableBuilder,
                                                                                       IdentificationStringBuilder& idText,
                                                                                       const MediaFile* mediaFile,
                                                                                       const std::set<int32_t>& frameIndices,
                                                                                       const SelectionItemMediaPlaneCoordinate* idMedia) const
{
    if (idMedia->isValid()) {
        std::array<float, 3> modelXYZ;
        idMedia->getModelXYZ(modelXYZ.data());
        std::vector<AString> columnOneText, columnTwoText, toolTipText;
        std::vector<int32_t> frameIndicesVector(frameIndices.begin(),
                                                frameIndices.end());
        const bool histologyIdFlag(false);
        mediaFile->getPixelPlaneIdentificationTextForFrames(idMedia->getTabIndex(),
                                                            frameIndicesVector,
                                                            idMedia->getPlaneCoordinate(),
                                                            histologyIdFlag,
                                                            columnOneText,
                                                            columnTwoText,
                                                            toolTipText);
        const int32_t numColOne(columnOneText.size());
        const int32_t numColTwo(columnTwoText.size());
        const int32_t maxNum(std::max(numColOne, numColTwo));
        for (int32_t i = 0; i < maxNum; i++) {
            AString colOne;
            AString colTwo;
            if (i < numColOne) {
                CaretAssertVectorIndex(columnOneText, i);
                colOne = columnOneText[i];
            }
            if (i < numColTwo) {
                CaretAssertVectorIndex(columnTwoText, i);
                colTwo = columnTwoText[i];
            }
            htmlTableBuilder.addRow(colOne, colTwo);
        }
        
        /*
         * For tooltip
         */
        for (const auto& text : toolTipText) {
            bool indentFlag(false);
            idText.addLine(indentFlag,
                           text);
        }
    }
    
}

/**
 * Get text for the tooltip for a selected node.
 *
 * @param idManager
 *    The identification manager
 * @param brain
 *     The Brain.
 * @param browserTab
 *     Browser tab in which tooltip is displayed
 * @param selectionManager
 *     The selection manager.
 * @param dataToolTipsManager
 *     The data tooltips manager
 * @param idText
 *     String builder for identification text.
 */
void
IdentificationFormattedTextGenerator::generateSurfaceToolTip(const Brain* brain,
                                                             const IdentificationManager* idManager,
                                                          const BrowserTabContent* browserTab,
                                                          const SelectionManager* selectionManager,
                                                          const DataToolTipsManager* dataToolTipsManager,
                                                          IdentificationStringBuilder& idText) const
{
    std::unique_ptr<HtmlTableBuilder> htmlTableBuilder = createHtmlTableBuilder(3);
    
    const SelectionItemSurfaceNode* nodeSelection = selectionManager->getSurfaceNodeIdentification();
    CaretAssert(nodeSelection);
    Vector3D vertexXYZ;
    bool vertexXYZValidFlag(false);
    if (nodeSelection->isValid()) {
        const Surface* surface = nodeSelection->getSurface();
        CaretAssert(surface);
        int32_t surfaceNumberOfNodes = surface->getNumberOfNodes();
        int32_t surfaceNodeIndex = nodeSelection->getNodeNumber();
        StructureEnum::Enum surfaceStructure = surface->getStructure();
        
        bool indentFlag = false;
        if ((surfaceStructure != StructureEnum::INVALID)
            && (surfaceNumberOfNodes > 0)
            && (surfaceNodeIndex >= 0)) {
            
            bool addVertexFlag(false);
            bool showSurfaceFlag = dataToolTipsManager->isShowSurfaceViewed();
            if (dataToolTipsManager->isShowSurfacePrimaryAnatomical()) {
                const Surface* anatSurface = brain->getPrimaryAnatomicalSurfaceForStructure(surfaceStructure);
                if (anatSurface != NULL) {
                    if (anatSurface->getNumberOfNodes() == surfaceNumberOfNodes) {
                        anatSurface->getCoordinate(surfaceNodeIndex,
                                                   vertexXYZ);
                        vertexXYZValidFlag = true;
                        idText.addLine(indentFlag,
                                       "Vertex",
                                       AString::number(surfaceNodeIndex));
                        indentFlag = true;
                        addVertexFlag = false;
                        
                        idText.addLine(indentFlag,
                                       "Anatomy Surface",
                                       xyzToText(vertexXYZ));
                        if (surface == anatSurface) {
                            showSurfaceFlag = false;
                        }
                    }
                }
            }
            
            if (showSurfaceFlag) {
                float xyz[3];
                surface->getCoordinate(surfaceNodeIndex,
                                       xyz);
                if (addVertexFlag) {
                    idText.addLine(indentFlag,
                                   "Vertex",
                                   AString::number(surfaceNodeIndex));
                    indentFlag = true;
                }
                
                idText.addLine(indentFlag,
                               (SurfaceTypeEnum::toGuiName(surface->getSurfaceType())
                                + " Surface"),
                               xyzToText(xyz));
            }
            
            if (dataToolTipsManager->isShowTopEnabledLayer()) {
                OverlaySet* overlaySet = const_cast<OverlaySet*>(browserTab->getOverlaySet());
                CaretAssert(overlaySet);
                Overlay* overlay = getTopEnabledOverlay(overlaySet);
                if (overlay != NULL) {
                    CaretMappableDataFile* mapFile(NULL);
                    int32_t mapIndex(-1);
                    overlay->getSelectionData(mapFile,
                                              mapIndex);
                    if ((mapFile != NULL)
                        && (mapIndex >= 0)) {
                        std::vector<int32_t> mapIndices { mapIndex };
                        AString textValue;
                        mapFile->getSurfaceNodeIdentificationForMaps(mapIndices,
                                                                     surfaceStructure,
                                                                     surfaceNodeIndex,
                                                                     surfaceNumberOfNodes,
                                                                     " ",
                                                                     s_dataValueDigitsRightOfDecimal,
                                                                     textValue);
                        if ( ! textValue.isEmpty()) {
                            idText.addLine(indentFlag,
                                           "Top Enabled Layer",
                                           textValue);
                        }
                    }
                }
            }
        }
    }
    
    if (dataToolTipsManager->isShowBorder()) {
        const SelectionItemBorderSurface* borderSelection = selectionManager->getSurfaceBorderIdentification();
        CaretAssert(borderSelection);
        if (borderSelection->isValid()) {
            generateSurfaceBorderIdentifcationText(*htmlTableBuilder,
                                                   idText,
                                                   borderSelection,
                                                   true);
        }
    }
    
    if (dataToolTipsManager->isShowFocus()) {
        const SelectionItemFocusSurface* focusSelection = selectionManager->getSurfaceFocusIdentification();
        CaretAssert(focusSelection);
        if (focusSelection->isValid()) {
            const FociFile* fociFile = focusSelection->getFociFile();
            const int32_t focusIndex = focusSelection->getFocusIndex();
            if ((fociFile != NULL)
                && (focusIndex >= 0)) {
                const Focus* focus = fociFile->getFocus(focusIndex);
                if (focus != NULL) {
                    generateFocusIdentifcationText(*htmlTableBuilder,
                                                   idText,
                                                   focus,
                                                   focusIndex,
                                                   focusSelection->getFocusProjectionIndex(),
                                                   true);
                }
            }
        }
        
        generateFocusIdentifcationText(*htmlTableBuilder,
                                       idText,
                                       selectionManager->getFocusIdentification(),
                                       true);
    }
    
    if (vertexXYZValidFlag) {
        const AString distText = getTextDistanceToMostRecentIdentificationSymbol(idManager,
                                                                                 vertexXYZ);
        if ( ! distText.isEmpty()) {
            idText.append(distText);
        }
    }
}

/**
 * Get text for the tooltip for a selected node.
 *
 * @param idManager
 *    The identification manager
 * @param browserTab
 *     Browser tab in which tooltip is displayed
 * @param selectionManager
 *     The selection manager.
 * @param dataToolTipsManager
 *     The data tooltips manager
 * @param idText
 *     String builder for identification text.
 */
void
IdentificationFormattedTextGenerator::generateVolumeToolTip(const IdentificationManager* idManager,
                                                            const BrowserTabContent* browserTab,
                                                   const SelectionManager* selectionManager,
                                                   const DataToolTipsManager* dataToolTipsManager,
                                                   IdentificationStringBuilder& idText) const
{
    const SelectionItemVoxel* voxelSelection = selectionManager->getVoxelIdentification();
    
    OverlaySet* overlaySet = const_cast<OverlaySet*>(browserTab->getOverlaySet());
    CaretAssert(overlaySet);
    
    double selectionXYZ[3];
    voxelSelection->getModelXYZ(selectionXYZ);
    float xyz[3] {
        static_cast<float>(selectionXYZ[0]),
        static_cast<float>(selectionXYZ[1]),
        static_cast<float>(selectionXYZ[2])
    };
    
    Vector3D voxelXYZ;
    bool voxelXYZValidFlag(false);
    
    bool indentFlag = false;
    if (dataToolTipsManager->isShowVolumeUnderlay()) {
        Overlay* volumeUnderlay = overlaySet->getUnderlayContainingVolume();
        if (volumeUnderlay != NULL) {
            CaretMappableDataFile* mapFile = NULL;
            int32_t mapIndex(-1);
            volumeUnderlay->getSelectionData(mapFile,
                                             mapIndex);
            
            VolumeMappableInterface* underlayVolumeInterface = NULL;
            if (mapFile != NULL) {
                underlayVolumeInterface = dynamic_cast<VolumeMappableInterface*>(mapFile);
                CaretAssert(underlayVolumeInterface == overlaySet->getUnderlayVolume());
            }
            
            if (underlayVolumeInterface != NULL) {
                /*
                 * Update IJK and XYZ since selection XYZ may be
                 * a different volume file.
                 */
                const VoxelIJK ijk(voxelSelection->getVoxelIJK());
                
                bool validFlag(false);
                const float value = underlayVolumeInterface->getVoxelValue(xyz[0], xyz[1], xyz[2],
                                                                           &validFlag,
                                                                           mapIndex);
                if (validFlag) {
                    const Vector3D xyz(voxelSelection->getVoxelXYZ());
                    idText.addLine(indentFlag,
                                   "Underlay Value",
                                   dataValueToText(value));
                    indentFlag = true;
                    idText.addLine(indentFlag,
                                   "IJK: ",
                                   AString::fromNumbers(ijk.m_ijk, 3, ", "));
                    idText.addLine(indentFlag,
                                   "XYZ",
                                   xyzToText(xyz));
                    voxelXYZ.set(xyz[0], xyz[1], xyz[2]);
                    voxelXYZValidFlag = true;
                }
            }
        }
    }
    
    if (dataToolTipsManager->isShowTopEnabledLayer()) {
        Overlay* overlay = getTopEnabledOverlay(overlaySet);
        if (overlay != NULL) {
            CaretMappableDataFile* mapFile(NULL);
            int32_t mapIndex(-1);
            overlay->getSelectionData(mapFile,
                                      mapIndex);
            if ((mapFile != NULL)
                && (mapIndex >= 0)) {
                std::vector<int32_t> mapIndices { mapIndex };
                AString textValue;
                int64_t ijk[3];
                mapFile->getVolumeVoxelIdentificationForMaps(mapIndices,
                                                             xyz,
                                                             " ",
                                                             s_dataValueDigitsRightOfDecimal,
                                                             ijk,
                                                             textValue);
                if ( ! textValue.isEmpty()) {
                    idText.addLine(indentFlag,
                                   ("Top Enabled Layer: "
                                    + textValue));
                }
            }
        }
    }
    
    if (voxelXYZValidFlag) {
        const AString distText = getTextDistanceToMostRecentIdentificationSymbol(idManager,
                                                                                 voxelXYZ);
        if ( ! distText.isEmpty()) {
            idText.append(distText);
        }
    }
}

/**
 * @return Get the top-most enabled overlay.  NULL if no overlays enabled
 *
 * @param overlaySet
 *     Overlay set for overlay.
 */
Overlay*
IdentificationFormattedTextGenerator::getTopEnabledOverlay(OverlaySet* overlaySet) const
{
    CaretAssert(overlaySet);
    const int32_t numberOfOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        Overlay* overlay = overlaySet->getOverlay(i);
        CaretAssert(overlay);
        if (overlay->isEnabled()) {
            return overlay;
        }
    }
    return NULL;
}

/**
 * Get text for the tooltip for a selected node.
 *
 * @param selectionManager
 *     The selection manager.
 * @param dataToolTipsManager
 *     The data tooltips manager
 * @param idText
 *     String builder for identification text.
 */
void
IdentificationFormattedTextGenerator::generateChartToolTip(const SelectionManager* selectionManager,
                                                  const DataToolTipsManager* dataToolTipsManager,
                                                  IdentificationStringBuilder& idText) const
{
    if (dataToolTipsManager->isShowChart()) {
        auto htmlTableBuilder = createHtmlTableBuilder(3);
        
        CaretMappableDataFile* dummyMapFile(NULL);
        std::set<int32_t> dummyMapIndices;
        this->generateChartTwoHistogramIdentificationText(*htmlTableBuilder,
                                                          idText,
                                                          selectionManager->getChartTwoHistogramIdentification(),
                                                          dummyMapFile,
                                                          dummyMapIndices,
                                                          true);

        this->generateChartTwoLineSeriesIdentificationText(*htmlTableBuilder,
                                                           idText,
                                                           selectionManager->getChartTwoLineSeriesIdentification(),
                                                           dummyMapFile,
                                                           dummyMapIndices,
                                                           true);

        this->generateChartTwoMatrixIdentificationText(*htmlTableBuilder,
                                                       idText,
                                                       selectionManager->getChartTwoMatrixIdentification(),
                                                       dummyMapFile,
                                                       dummyMapIndices,
                                                       true);
    }
}

/**
 * Get text for the tooltip for media
 *
 * @param idManager
 *    The identification manager
 * @param selectionManager
 *     The selection manager.
 * @param dataToolTipsManager
 *     The data tooltips manager
 * @param idText
 *     String builder for identification text.
 */
void
IdentificationFormattedTextGenerator::generateHistologyPlaneCoordinateToolTip(const IdentificationManager* idManager,
                                                                              const SelectionManager* selectionManager,
                                                                              const DataToolTipsManager* dataToolTipsManager,
                                                                              IdentificationStringBuilder& idText) const
{
    if (dataToolTipsManager->isShowHistology()) {
        std::unique_ptr<HtmlTableBuilder> htmlTableBuilder = createHtmlTableBuilder(3);
        
        const SelectionItemHistologyCoordinate* histologySelection(selectionManager->getHistologyPlaneCoordinateIdentification());
        if (histologySelection->isValid()) {
            generateHistologyPlaneCoordinateIdentificationText(idManager,
                                                               *htmlTableBuilder,
                                                               idText,
                                                               histologySelection);
        }
    }
}

/**
 * Append distance from item to most recent identification symbol
 * @param idManager
 *    The identification manager
 * @param selectionXYZ
 *    Stereotaxic coordinate of selection
 */
AString
IdentificationFormattedTextGenerator::getTextDistanceToMostRecentIdentificationSymbol(const IdentificationManager* idManager,
                                                                                      const float selectionXYZ[3]) const
{
    CaretAssert(idManager);
    
    const IdentifiedItemUniversal* lastIdItem(idManager->getMostRecentIdentifiedItem());
    if (lastIdItem != NULL) {
        if (lastIdItem->isStereotaxicXYZValid()) {
            const Vector3D xyz(lastIdItem->getStereotaxicXYZ());
            const float distance((xyz - Vector3D(selectionXYZ)).length());
            const AString text("Distance to Most Recent ID Symbol: "
                               + dataValueToText(distance));
            return text;
        }
    }
    
    return "";
}

/**
 * Get text for the tooltip for media
 *
 * @param selectionManager
 *     The selection manager.
 * @param dataToolTipsManager
 *     The data tooltips manager
 * @param idText
 *     String builder for identification text.
 */
void
IdentificationFormattedTextGenerator::generateMediaLogicalCoordinateToolTip(const SelectionManager* selectionManager,
                                                           const DataToolTipsManager* dataToolTipsManager,
                                                           IdentificationStringBuilder& idText) const
{
    if (dataToolTipsManager->isShowMedia()) {
        std::unique_ptr<HtmlTableBuilder> htmlTableBuilder = createHtmlTableBuilder(3);
        
        const SelectionItemMediaLogicalCoordinate* mediaSelection = selectionManager->getMediaLogicalCoordinateIdentification();
        if (mediaSelection->isValid()) {
            const MediaFile* mediaFile(mediaSelection->getMediaFile());
            if (mediaFile != NULL) {
                std::set<int32_t> frameIndices;
                for (int32_t i = 0; i < mediaFile->getNumberOfFrames(); i++) {
                    frameIndices.insert(i);
                }
                generateMediaLogicalCoordinateIdentificationText(*htmlTableBuilder,
                                                idText,
                                                mediaFile,
                                                frameIndices,
                                                mediaSelection);
            }
        }
    }
}

/**
 * Get text for the tooltip for media
 *
 * @param selectionManager
 *     The selection manager.
 * @param dataToolTipsManager
 *     The data tooltips manager
 * @param idText
 *     String builder for identification text.
 */
void
IdentificationFormattedTextGenerator::generateMediaPlaneCoordinateToolTip(const SelectionManager* selectionManager,
                                                                            const DataToolTipsManager* dataToolTipsManager,
                                                                            IdentificationStringBuilder& idText) const
{
    if (dataToolTipsManager->isShowMedia()) {
        std::unique_ptr<HtmlTableBuilder> htmlTableBuilder = createHtmlTableBuilder(3);
        
        const SelectionItemMediaPlaneCoordinate* mediaSelection = selectionManager->getMediaPlaneCoordinateIdentification();
        if (mediaSelection->isValid()) {
            const MediaFile* mediaFile(mediaSelection->getMediaFile());
            if (mediaFile != NULL) {
                std::set<int32_t> frameIndices;
                for (int32_t i = 0; i < mediaFile->getNumberOfFrames(); i++) {
                    frameIndices.insert(i);
                }
                generateMediaPlaneCoordinateIdentificationText(*htmlTableBuilder,
                                                                 idText,
                                                                 mediaFile,
                                                                 frameIndices,
                                                                 mediaSelection);
            }
        }
    }
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationFormattedTextGenerator::toString() const
{
    return "IdentificationFormattedTextGenerator";
}
        
/**
 * @return New instance of an HTML Table builder
 * @param numberOfColumns
 *  Number of columns in the table
 */
std::unique_ptr<HtmlTableBuilder>
IdentificationFormattedTextGenerator::createHtmlTableBuilder(const int32_t numberOfColumns) const
{
    std::unique_ptr<HtmlTableBuilder> htb(new HtmlTableBuilder(HtmlTableBuilder::HtmlVersion::V4_01,
                                                               numberOfColumns));
    return htb;
}

/**
 * Convert XY coordinates to text
 * @param xy
 *    The XY coordinates
 * @param precisionDigits
 *    Optional number of digits right of decimal.  When zero or greater, it specifies the number of digits
 *    right of the decimal.  If negative, the default number of digits right of decimal are used.
 * @return
 *    Text representation of the coordinates
 */
AString
IdentificationFormattedTextGenerator::xyToText(const float xy[2],
                                               const int32_t precisionDigits) const
{
    const int32_t defaultDigitsRightOfDecimal(2);
    const int32_t digits((precisionDigits >= 0)
                         ? precisionDigits
                         : defaultDigitsRightOfDecimal);
    
    return AString::fromNumbers(xy,
                                2,       /* number of elements */
                                ", ",    /* separator between elements */
                                'f',     /* floating point format */
                                digits); /* digits right of decimal */
}

/**
 * Convert XYZ coordinates to text
 * @param xyx
 *    The XYZ coordinates
 * @param precisionDigits
 *    Optional number of digits right of decimal.  When zero or greater, it specifies the number of digits
 *    right of the decimal.  If negative, the default number of digits right of decimal are used.
 * @return
 *    Text representation of the coordinates
 */
AString
IdentificationFormattedTextGenerator::xyzToText(const float xyz[3],
                                                const int32_t precisionDigits) const
{
    const int32_t defaultDigitsRightOfDecimal(2);
    const int32_t digits((precisionDigits >= 0)
                         ? precisionDigits
                         : defaultDigitsRightOfDecimal);
    
    return AString::fromNumbers(xyz,
                                3,       /* number of elements */
                                ", ",    /* separator between elements */
                                'f',     /* floating point format */
                                digits); /* digits right of decimal */
}

AString
IdentificationFormattedTextGenerator::dataValueToText(const float value,
                                                      const int32_t precisionDigits) const
{
    const int32_t digits((precisionDigits >= 0)
                         ? precisionDigits
                         : s_dataValueDigitsRightOfDecimal);
    
    return AString::number(value,
                           'f',     /* floating point format */
                           digits); /* digits right of decimal */
}



/* =============================================================================================*/

IdentificationFormattedTextGenerator::MapFileAndMapIndices::MapFileAndMapIndices(CaretDataFile* mapFile)
: m_mapFile(mapFile) {
    CaretAssert(mapFile);
}

void
IdentificationFormattedTextGenerator::MapFileAndMapIndices::addMapIndex(const int32_t mapIndex) {
    m_mapIndices.insert(mapIndex);
}

void
IdentificationFormattedTextGenerator::MapFileAndMapIndices::addMapIndices(const std::vector<int32_t> mapIndices) {
    m_mapIndices.insert(mapIndices.begin(),
                        mapIndices.end());
}

void
IdentificationFormattedTextGenerator::MapFileAndMapIndices::addMapIndices(const std::set<int32_t> mapIndices) {
    m_mapIndices.insert(mapIndices.begin(),
                        mapIndices.end());
}
