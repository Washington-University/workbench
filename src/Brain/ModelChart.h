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

#include "ChartDataTypeEnum.h"
#include "DataFileException.h"
#include "EventListenerInterface.h"
#include "Model.h"
#include "StructureEnum.h"

namespace caret {

    class CaretDataFileSelectionModel;
    class ChartData;
    class ChartDataCartesian;
    class ChartableMatrixInterface;
    class ChartModel;
    class ChartModelDataSeries;
    class ChartModelTimeSeries;
    class ChartableBrainordinateInterface;
    class CiftiConnectivityMatrixParcelFile;
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
                                         const int32_t nodeIndex) throw (DataFileException);
        
        void loadAverageChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                 const int32_t surfaceNumberOfNodes,
                                                               const std::vector<int32_t>& nodeIndices) throw (DataFileException);
        
        void loadChartDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        virtual void receiveEvent(Event* event);
        
        void getValidChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& validChartDataTypesOut) const;
        
        ChartDataTypeEnum::Enum getSelectedChartDataType(const int32_t tabIndex) const;
        
        void setSelectedChartDataType(const int32_t tabIndex,
                                      const ChartDataTypeEnum::Enum dataType);
        
        ChartModelDataSeries* getSelectedDataSeriesChartModel(const int32_t tabIndex);
        
        const ChartModelDataSeries* getSelectedDataSeriesChartModel(const int32_t tabIndex) const;
        
        ChartModelTimeSeries* getSelectedTimeSeriesChartModel(const int32_t tabIndex);
        
        const ChartModelTimeSeries* getSelectedTimeSeriesChartModel(const int32_t tabIndex) const;
        
        virtual void getDescriptionOfContent(const int32_t tabIndex,
                                             PlainTextStringBuilder& descriptionOut) const;
        
        virtual void copyTabContent(const int32_t sourceTabIndex,
                                    const int32_t destinationTabIndex);
        
        void reset();
        
        CaretDataFileSelectionModel* getChartableMatrixFileSelectionModel(const int32_t tabIndex);
        
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
        
        const ChartModelTimeSeries* getSelectedTimeSeriesChartModelHelper(const int32_t tabIndex) const;
        
        void saveChartModelsToScene(const SceneAttributes* sceneAttributes,
                                    SceneClass* sceneClass,
                                    const std::vector<int32_t>& tabIndices,
                                    std::set<AString>& validChartDataIDsOut);
        
        void restoreChartModelsFromScene(const SceneAttributes* sceneAttributes,
                                         const SceneClass* sceneClass);

        void getTabsAndChartFilesForChartLoading(std::map<ChartableBrainordinateInterface*, std::vector<int32_t> >& chartFileEnabledTabsOut) const;

        /** Overlays sets for this model and for each tab */
        OverlaySetArray* m_overlaySetArray;
        
        mutable ChartDataTypeEnum::Enum m_selectedChartDataType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Chart model for data-series data */
        ChartModelDataSeries* m_chartModelDataSeries[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Chart model for time-series data */
        ChartModelTimeSeries* m_chartModelTimeSeries[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];

        /** Contains data series charts */
        std::list<QWeakPointer<ChartDataCartesian> > m_dataSeriesChartData;
        
        /** Contains time series charts */
        std::list<QWeakPointer<ChartDataCartesian> > m_timeSeriesChartData;
        
        std::vector<ChartableMatrixInterface*> m_previousChartMatrixFiles;

        CaretDataFileSelectionModel* m_chartableMatrixFileSelectionModel[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        SceneClassAssistant* m_sceneAssistant;
    };

} // namespace

#endif // __MODEL_CHART_H__
