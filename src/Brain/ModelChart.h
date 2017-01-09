#ifndef __MODEL_CHART_H__
#define __MODEL_CHART_H__

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

#include <QSharedPointer>
#include <QWeakPointer>

#include <list>
#include <set>
#include <map>

#include "ChartTwoDataTypeEnum.h"
#include "ChartVersionOneDataTypeEnum.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"
#include "Model.h"
#include "StructureEnum.h"

namespace caret {

    class CaretDataFileSelectionModel;
    class ChartData;
    class ChartDataCartesian;
    class ChartDataSource;
    class ChartableLineSeriesBrainordinateInterface;
    class ChartableLineSeriesInterface;
    class ChartableLineSeriesRowColumnInterface;
    class ChartableMatrixInterface;
    class ChartModel;
    class ChartModelDataSeries;
    class ChartModelFrequencySeries;
    class ChartModelTimeSeries;
    class ChartOverlaySetArray;
    class CiftiConnectivityMatrixParcelFile;
    class CiftiMappableDataFile;
    class OverlaySetArray;
    class SurfaceFile;
    
    /// Controls the display of a chart.
    class ModelChart : public Model, public EventListenerInterface  {
        
    public:
        ModelChart(Brain* brain);
        
        virtual ~ModelChart();
        
        void initializeOverlays();
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
        void loadChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                         const int32_t surfaceNumberOfNodes,
                                         const int32_t nodeIndex);
        
        void loadAverageChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                 const int32_t surfaceNumberOfNodes,
                                                               const std::vector<int32_t>& nodeIndices);
        
        void loadChartDataForVoxelAtCoordinate(const float xyz[3]);
        
        void loadChartDataForCiftiMappableFileRow(CiftiMappableDataFile* ciftiMapFile,
                                                  const int32_t rowIndex);
        
        void loadChartDataForYokedCiftiMappableFiles(const MapYokingGroupEnum::Enum mapYokingGroup,
                                                     const int32_t mapIndex);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        virtual ChartOverlaySet* getChartOverlaySet(const int tabIndex);
        
        virtual const ChartOverlaySet* getChartOverlaySet(const int tabIndex) const;
        
        virtual void receiveEvent(Event* event);
        
        void getValidChartOneDataTypes(std::vector<ChartVersionOneDataTypeEnum::Enum>& validChartDataTypesOut) const;
        
        ChartVersionOneDataTypeEnum::Enum getSelectedChartOneDataType(const int32_t tabIndex) const;
        
        void setSelectedChartOneDataType(const int32_t tabIndex,
                                      const ChartVersionOneDataTypeEnum::Enum dataType);
        
        void getValidChartDataTypes(std::vector<ChartTwoDataTypeEnum::Enum>& validChartDataTypesOut) const;
        
        ChartTwoDataTypeEnum::Enum getSelectedChartDataType(const int32_t tabIndex) const;
        
        void setSelectedChartDataType(const int32_t tabIndex,
                                         const ChartTwoDataTypeEnum::Enum dataType);
        
        ChartModelDataSeries* getSelectedDataSeriesChartModel(const int32_t tabIndex);
        
        const ChartModelDataSeries* getSelectedDataSeriesChartModel(const int32_t tabIndex) const;
        
        ChartModelFrequencySeries* getSelectedFrequencySeriesChartModel(const int32_t tabIndex);
        
        const ChartModelFrequencySeries* getSelectedFrequencySeriesChartModel(const int32_t tabIndex) const;
        
        ChartModelTimeSeries* getSelectedTimeSeriesChartModel(const int32_t tabIndex);
        
        const ChartModelTimeSeries* getSelectedTimeSeriesChartModel(const int32_t tabIndex) const;
        
        virtual void getDescriptionOfContent(const int32_t tabIndex,
                                             PlainTextStringBuilder& descriptionOut) const;
        
        virtual void copyTabContent(const int32_t sourceTabIndex,
                                    const int32_t destinationTabIndex);
        
        void reset();
        
        CaretDataFileSelectionModel* getChartableMatrixParcelFileSelectionModel(const int32_t tabIndex);
        
        CaretDataFileSelectionModel* getChartableMatrixSeriesFileSelectionModel(const int32_t tabIndex);
        
    protected:
        virtual void saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass);
        
        virtual void restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
        
    private:
        ModelChart(const ModelChart&);
        
        ModelChart& operator=(const ModelChart&);
        
        void addChartToChartModels(const std::vector<int32_t>& tabIndices,
                                   ChartData* chartData);

        void initializeCharts();
        
        void removeAllCharts();
        
        const ChartModelDataSeries* getSelectedDataSeriesChartModelHelper(const int32_t tabIndex) const;
        
        const ChartModelFrequencySeries* getSelectedFrequencySeriesChartModelHelper(const int32_t tabIndex) const;
        
        const ChartModelTimeSeries* getSelectedTimeSeriesChartModelHelper(const int32_t tabIndex) const;
        
        void saveChartModelsToScene(const SceneAttributes* sceneAttributes,
                                    SceneClass* sceneClass,
                                    const std::vector<int32_t>& tabIndices,
                                    std::set<AString>& validChartDataIDsOut);
        
        void restoreVersionOneChartModelsFromScene(const SceneAttributes* sceneAttributes,
                                                   const SceneClass* sceneClass);

        void restoreVersionTwoChartModelsFromScene(const SceneAttributes* sceneAttributes,
                                                   const SceneClass* sceneClass);
        
        void getTabsAndBrainordinateChartFilesForLineChartLoading(std::map<ChartableLineSeriesBrainordinateInterface*, std::vector<int32_t> >& chartBrainordinateFileEnabledTabsOut) const;

        void getTabsAndRowColumnChartFilesForLineChartLoading(std::map<ChartableLineSeriesRowColumnInterface*, std::vector<int32_t> >& chartRowColumnFilesEnabledTabsOut) const;
        
        void getTabsAndLineSeriesChartFilesForLineChartLoading(std::map<ChartableLineSeriesInterface*, std::vector<int32_t> >& chartFileEnabledTabsOut) const;
        
        ChartData* loadCartesianChartWhenRestoringScene(const ChartData* chartData);

        void restoreVersionOneModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass);
        
        void restoreVersionTwoModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                                        const SceneClass* sceneClass);
        
        /** Overlays sets for this model and for each tab */
        OverlaySetArray* m_overlaySetArray;
        
        /** Chart Overlay sets for XX data type */
        std::unique_ptr<ChartOverlaySetArray> m_histogramChartOverlaySetArray;
        
        /** Chart Overlay sets for XX data type */
        std::unique_ptr<ChartOverlaySetArray> m_lineSeriesChartOverlaySetArray;
        
        /** Chart Overlay sets for XX data type */
        std::unique_ptr<ChartOverlaySetArray> m_matrixChartOverlaySetArray;
        
        mutable ChartTwoDataTypeEnum::Enum m_selectedChartTwoDataType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        mutable ChartVersionOneDataTypeEnum::Enum m_selectedChartOneDataType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Chart model for data-series data */
        ChartModelDataSeries* m_chartModelDataSeries[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Chart model for frequency-series data */
        ChartModelFrequencySeries* m_chartModelFrequencySeries[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Chart model for time-series data */
        ChartModelTimeSeries* m_chartModelTimeSeries[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

        /** Contains data series charts */
        std::list<QWeakPointer<ChartDataCartesian> > m_dataSeriesChartData;
        
        /** Contains time series charts */
        std::list<QWeakPointer<ChartDataCartesian> > m_frequencySeriesChartData;
        
        /** Contains time series charts */
        std::list<QWeakPointer<ChartDataCartesian> > m_timeSeriesChartData;
        
        std::vector<ChartableMatrixInterface*> m_previousChartMatrixFiles;

        CaretDataFileSelectionModel* m_chartableMatrixFileSelectionModel[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        CaretDataFileSelectionModel* m_chartableMatrixSeriesFileSelectionModel[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
    };

} // namespace

#endif // __MODEL_CHART_H__
