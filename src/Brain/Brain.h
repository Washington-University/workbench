#ifndef __BRAIN_H__
#define __BRAIN_H__

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

#include <vector>
#include <stdint.h>

#include "CaretObject.h"
#include "ChartDataTypeEnum.h"
#include "DataFileTypeEnum.h"
#include "DataFileException.h"
#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"
#include "FiberOrientationSamplesLoader.h"
#include "FiberOrientationSamplesVector.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {
    
    class Border;
    class BorderFile;
    class BorderPointFromSearch;
    class FociFile;
    class BrainStructure;
    class CaretDataFile;
    class CaretMappableDataFile;
    class ChartingDataManager;
    class ChartableBrainordinateInterface;
    class ChartableMatrixInterface;
    class CiftiBrainordinateDataSeriesFile;
    class CiftiBrainordinateLabelFile;
    class CiftiBrainordinateScalarFile;
    class CiftiConnectivityMatrixDenseFile;
    class CiftiConnectivityMatrixDenseParcelFile;
    class CiftiConnectivityMatrixParcelFile;
    class CiftiConnectivityMatrixParcelDenseFile;
    class CiftiFiberOrientationFile;
    class CiftiFiberTrajectoryFile;
    class CiftiMappableDataFile;
    class CiftiMappableConnectivityMatrixDataFile;
    class CiftiParcelSeriesFile;
    class CiftiParcelScalarFile;
    class DisplayProperties;
    class DisplayPropertiesBorders;
    class DisplayPropertiesFiberOrientation;
    class DisplayPropertiesFoci;
    class DisplayPropertiesLabels;
    class DisplayPropertiesSurface;
    class DisplayPropertiesVolume;
    class EventDataFileRead;
    class EventDataFileReload;
    class EventSpecFileReadDataFiles;
    class IdentificationManager;
    class LabelFile;
    class MetricFile;
    class ModelChart;
    class ModelSurfaceMontage;
    class ModelVolume;
    class ModelWholeBrain;
    class PaletteFile;
    class RgbaFile;
    class SceneClassAssistant;
    class SceneFile;
    class SelectionManager;
    class SpecFile;
    class Surface;
    class SurfaceFile;
    class SurfaceProjectedItem;
    class VolumeFile;
    
    class Brain : public CaretObject, public EventListenerInterface, public SceneableInterface {

    public:
        Brain();
        
        ~Brain();
        
    private:
        Brain(const Brain&);
        Brain& operator=(const Brain&);
        
    public:
        int getNumberOfBrainStructures() const;
        
        void addBrainStructure(BrainStructure* brainStructure);
        
        BrainStructure* getBrainStructure(const int32_t indx);

        const BrainStructure* getBrainStructure(const int32_t indx) const;
        
        BrainStructure* getBrainStructure(StructureEnum::Enum structure,
                                          bool createIfNotFound);

        int32_t getNumberOfBorderFiles() const;
        
        BorderFile* getBorderFile(const int32_t indx);
        
        const BorderFile* getBorderFile(const int32_t indx) const;
        
        int32_t getNumberOfFociFiles() const;
        
        FociFile* getFociFile(const int32_t indx);
        
        const FociFile* getFociFile(const int32_t indx) const;
        
        PaletteFile* getPaletteFile();
        
        const PaletteFile* getPaletteFile() const;

        int32_t getNumberOfSceneFiles() const;
        
        SceneFile* getSceneFile(const int32_t indx);
        
        const SceneFile* getSceneFile(const int32_t indx) const;
        
        const SpecFile* getSpecFile() const;
        
        SpecFile* getSpecFile();
        
        Surface* getSurfaceWithName(const AString& surfaceFileName,
                                    const bool useAbsolutePath);
        
        const Surface* getVolumeInteractionSurfaceForStructure(const StructureEnum::Enum structure) const;
        
        Surface* getVolumeInteractionSurfaceNearestCoordinate(const float xyz[3],
                                                              const float tolerance);
        
        std::vector<const Surface*> getVolumeInteractionSurfaces() const;
        
        std::vector<const SurfaceFile*> getVolumeInteractionSurfaceFiles() const;
        
        int32_t getNumberOfVolumeFiles() const;
        
        VolumeFile* getVolumeFile(const int32_t volumeFileIndex);
        
        const VolumeFile* getVolumeFile(const int32_t volumeFileIndex) const;
        
        void resetBrain();
        
        void resetBrainKeepSceneFiles();
        
        void receiveEvent(Event* event);
        
        ModelChart* getChartModel();
        
        const ModelChart* getChartModel() const;
        
        ChartingDataManager* getChartingDataManager();
        
        const ChartingDataManager* getChartingDataManager() const;
        
        void getAllCiftiMappableDataFiles(std::vector<CiftiMappableDataFile*>& allCiftiMappableDataFilesOut) const;
        
        int32_t getNumberOfConnectivityMatrixDenseFiles() const;
        
        CiftiConnectivityMatrixDenseFile* getConnectivityMatrixDenseFile(int32_t indx);
        
        const CiftiConnectivityMatrixDenseFile* getConnectivityMatrixDenseFile(int32_t indx) const;
        
        void getConnectivityMatrixDenseFiles(std::vector<CiftiConnectivityMatrixDenseFile*>& connectivityDenseFilesOut) const;
        
        int32_t getNumberOfConnectivityDenseLabelFiles() const;
        
        CiftiBrainordinateLabelFile* getConnectivityDenseLabelFile(int32_t indx);
        
        const CiftiBrainordinateLabelFile* getConnectivityDenseLabelFile(int32_t indx) const;
        
        void getConnectivityDenseLabelFiles(std::vector<CiftiBrainordinateLabelFile*>& connectivityDenseLabelFilesOut) const;
        
        int32_t getNumberOfConnectivityMatrixDenseParcelFiles() const;
        
        CiftiConnectivityMatrixDenseParcelFile* getConnectivityMatrixDenseParcelFile(int32_t indx);
        
        const CiftiConnectivityMatrixDenseParcelFile* getConnectivityMatrixDenseParcelFile(int32_t indx) const;
        
        void getConnectivityMatrixDenseParcelFiles(std::vector<CiftiConnectivityMatrixDenseParcelFile*>& connectivityDenseParcelFilesOut) const;
        
        int32_t getNumberOfConnectivityDenseScalarFiles() const;
        
        CiftiBrainordinateScalarFile* getConnectivityDenseScalarFile(int32_t indx);
        
        const CiftiBrainordinateScalarFile* getConnectivityDenseScalarFile(int32_t indx) const;
        
        void getConnectivityDenseScalarFiles(std::vector<CiftiBrainordinateScalarFile*>& connectivityDenseScalarFilesOut) const;
        
        int32_t getNumberOfConnectivityParcelScalarFiles() const;
        
        CiftiParcelScalarFile* getConnectivityParcelScalarFile(int32_t indx);
        
        const CiftiParcelScalarFile* getConnectivityParcelScalarFile(int32_t indx) const;
        
        void getConnectivityParcelScalarFiles(std::vector<CiftiParcelScalarFile*>& connectivityParcelScalarFilesOut) const;

        int32_t getNumberOfConnectivityParcelSeriesFiles() const;
        
        CiftiParcelSeriesFile* getConnectivityParcelSeriesFile(int32_t indx);
        
        const CiftiParcelSeriesFile* getConnectivityParcelSeriesFile(int32_t indx) const;
        
        void getConnectivityParcelSeriesFiles(std::vector<CiftiParcelSeriesFile*>& connectivityParcelSeriesFilesOut) const;
        
        int32_t getNumberOfConnectivityFiberOrientationFiles() const;
        
        CiftiFiberOrientationFile* getConnectivityFiberOrientationFile(int32_t indx);
        
        const CiftiFiberOrientationFile* getConnectivityFiberOrientationFile(int32_t indx) const;
        
        void getConnectivityFiberOrientationFiles(std::vector<CiftiFiberOrientationFile*>& connectivityFiberOrientationFilesOut) const;
        
        bool getFiberOrientationSphericalSamplesVectors(std::vector<FiberOrientationSamplesVector>& xVectors,
                                                        std::vector<FiberOrientationSamplesVector>& yVectors,
                                                        std::vector<FiberOrientationSamplesVector>& zVectors,
                                                        FiberOrientation* &fiberOrientationOut,
                                                        AString& errorMessageOut);
        
        int32_t getNumberOfConnectivityFiberTrajectoryFiles() const;
        
        CiftiFiberTrajectoryFile* getConnectivityFiberTrajectoryFile(int32_t indx);
        
        const CiftiFiberTrajectoryFile* getConnectivityFiberTrajectoryFile(int32_t indx) const;
        
        void getConnectivityFiberTrajectoryFiles(std::vector<CiftiFiberTrajectoryFile*>& ciftiFiberTrajectoryFilesOut) const;
        
        int32_t getNumberOfConnectivityMatrixParcelFiles() const;
        
        CiftiConnectivityMatrixParcelFile* getConnectivityMatrixParcelFile(int32_t indx);
        
        const CiftiConnectivityMatrixParcelFile* getConnectivityMatrixParcelFile(int32_t indx) const;
        
        void getConnectivityMatrixParcelFiles(std::vector<CiftiConnectivityMatrixParcelFile*>& connectivityParcelFilesOut) const;

        int32_t getNumberOfConnectivityMatrixParcelDenseFiles() const;
        
        CiftiConnectivityMatrixParcelDenseFile* getConnectivityMatrixParcelDenseFile(int32_t indx);
        
        const CiftiConnectivityMatrixParcelDenseFile* getConnectivityMatrixParcelDenseFile(int32_t indx) const;
        
        void getConnectivityMatrixParcelDenseFiles(std::vector<CiftiConnectivityMatrixParcelDenseFile*>& connectivityParcelDenseFilesOut) const;
        
        int32_t getNumberOfConnectivityTimeSeriesFiles() const;
        
        void getAllCiftiConnectivityMatrixFiles(std::vector<CiftiMappableConnectivityMatrixDataFile*>& allCiftiConnectivityMatrixFiles) const;
        
        int32_t getNumberOfConnectivityDataSeriesFiles() const;
        
        CiftiBrainordinateDataSeriesFile* getConnectivityDataSeriesFile(int32_t indx);
        
        const CiftiBrainordinateDataSeriesFile* getConnectivityDataSeriesFile(int32_t indx) const;
        
        void getConnectivityDataSeriesFiles(std::vector<CiftiBrainordinateDataSeriesFile*>& connectivityDataSeriesFilesOut) const;
        
        void getAllChartableBrainordinateDataFiles(std::vector<ChartableBrainordinateInterface*>& chartableDataFilesOut) const;
        
        void getAllChartableBrainordinateDataFilesForChartDataType(const ChartDataTypeEnum::Enum chartDataType,
                                                      std::vector<ChartableBrainordinateInterface*>& chartableDataFilesOut) const;
        
        void getAllChartableBrainordinateDataFilesWithChartingEnabled(std::vector<ChartableBrainordinateInterface*>& chartableDataFilesOut) const;
        
        void getAllChartableMatrixDataFiles(std::vector<ChartableMatrixInterface*>& chartableDataFilesOut) const;
        
        void getAllChartableMatrixDataFilesForChartDataType(const ChartDataTypeEnum::Enum chartDataType,
                                                                   std::vector<ChartableMatrixInterface*>& chartableDataFilesOut) const;
        
        AString getCurrentDirectory() const;
        
        void setCurrentDirectory(const AString& currentDirectory);
        
        void getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut,
                             const bool includeSpecFile = false) const;
        
        void getAllDataFilesWithDataFileType(const DataFileTypeEnum::Enum dataFileType,
                                             std::vector<CaretDataFile*>& caretDataFilesOut) const;
        
        void getAllMappableDataFiles(std::vector<CaretMappableDataFile*>& allCaretMappableDataFilesOut) const;
        
        void getAllMappableDataFileWithDataFileType(const DataFileTypeEnum::Enum dataFileType,
                                                    std::vector<CaretMappableDataFile*>& caretMappableDataFilesOut) const;
        
        bool isFileValid(const CaretDataFile* caretDataFile) const;

        void getAllModifiedFiles(const std::vector<DataFileTypeEnum::Enum>& excludeTheseDataTypes,
                                 std::vector<CaretDataFile*>& modifiedDataFilesOut) const;
        
        void writeDataFile(CaretDataFile* caretDataFile) throw (DataFileException);
        
        DisplayPropertiesBorders* getDisplayPropertiesBorders();
        
        const DisplayPropertiesBorders* getDisplayPropertiesBorders() const;
        
        DisplayPropertiesFiberOrientation* getDisplayPropertiesFiberOrientation();
        
        const DisplayPropertiesFiberOrientation* getDisplayPropertiesFiberOrientation() const;
        
        DisplayPropertiesFoci* getDisplayPropertiesFoci();
        
        const DisplayPropertiesFoci* getDisplayPropertiesFoci() const;
        
        DisplayPropertiesVolume* getDisplayPropertiesVolume();
        
        const DisplayPropertiesVolume* getDisplayPropertiesVolume() const;
        
        DisplayPropertiesSurface* getDisplayPropertiesSurface();
        
        const DisplayPropertiesSurface* getDisplayPropertiesSurface() const;
        
        
        DisplayPropertiesLabels* getDisplayPropertiesLabels();
        
        const DisplayPropertiesLabels* getDisplayPropertiesLabels() const;
        
        void copyDisplayProperties(const int32_t sourceTabIndex,
                                   const int32_t targetTabIndex);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        IdentificationManager* getIdentificationManager();

        SelectionManager* getSelectionManager();
        
        void getCiftiShapeMap(CiftiBrainordinateScalarFile* &ciftiScalarShapeFileOut,
                              int32_t& ciftiScalarhapeFileMapIndexOut,
                              std::vector<CiftiBrainordinateScalarFile*>& ciftiScalarNotShapeFilesOut) const;
        
    private:
        /**
         * Reset the brain scene file mode
         */
        enum ResetBrainKeepSceneFiles {
            /** Do NOT keep scene files when resetting the brain*/
            RESET_BRAIN_KEEP_SCENE_FILES_NO,
            /** Do keep scene files when resetting the brain*/
            RESET_BRAIN_KEEP_SCENE_FILES_YES
        };
        
        /**
         * Reset the brain spec file mode
         */
        enum ResetBrainKeepSpecFile {
            /** Do NOT keep spec files when resetting the brain*/
            RESET_BRAIN_KEEP_SPEC_FILE_NO,
            /** Do keep spec files when resetting the brain*/
            RESET_BRAIN_KEEP_SPEC_FILE_YES
        };
        
        /**
         * Mode of file reading
         */
        enum FileModeAddReadReload {
            /** Add the file */
            FILE_MODE_ADD,
            /** Read the file */
            FILE_MODE_READ,
            /** Reload the file */
            FILE_MODE_RELOAD
        };
        
        void addDataFile(CaretDataFile* caretDataFile) throw (DataFileException);
        
        bool removeWithoutDeleteDataFile(const CaretDataFile* caretDataFile);
        
        bool removeWithoutDeleteDataFilePrivate(const CaretDataFile* caretDataFile);
        
        bool removeAndDeleteDataFile(CaretDataFile* caretDataFile);
        
        void loadFilesSelectedInSpecFile(EventSpecFileReadDataFiles* readSpecFileDataFilesEvent);
        
        void loadSpecFileFromScene(const SceneAttributes* sceneAttributes,
                                   SpecFile* specFile,
                          const ResetBrainKeepSceneFiles keepSceneFile,
                          const ResetBrainKeepSpecFile keepSpecFile);
        
        void resetBrain(const ResetBrainKeepSceneFiles keepSceneFiles,
                        const ResetBrainKeepSpecFile keepSpecFile);
        
        void processReadDataFileEvent(EventDataFileRead* readDataFileEvent);
        
        void processReloadDataFileEvent(EventDataFileReload* reloadDataFileEvent);
        
        CaretDataFile* readDataFile(const DataFileTypeEnum::Enum dataFileType,
                          const StructureEnum::Enum structure,
                          const AString& dataFileName,
                          const bool markDataFileAsModified) throw (DataFileException);
        
        CaretDataFile* addReadOrReloadDataFile(const FileModeAddReadReload fileMode,
                                            CaretDataFile* caretDataFile,
                                            const DataFileTypeEnum::Enum dataFileType,
                                            const StructureEnum::Enum structure,
                                            const AString& dataFileName,
                                            const bool markDataFileAsModified) throw (DataFileException);
        
        void updateAfterFilesAddedOrRemoved();
        
        LabelFile* addReadOrReloadLabelFile(const FileModeAddReadReload fileMode,
                                 CaretDataFile* caretDataFile,
                                 const AString& filename,
                                 const StructureEnum::Enum structure,
                                 const bool markDataFileAsModified) throw (DataFileException);
        
        MetricFile* addReadOrReloadMetricFile(const FileModeAddReadReload fileMode,
                                   CaretDataFile* caretDataFile,
                                   const AString& filename,
                                   const StructureEnum::Enum structure,
                                   const bool markDataFileAsModified) throw (DataFileException);
        
        RgbaFile* addReadOrReloadRgbaFile(const FileModeAddReadReload fileMode,
                               CaretDataFile* caretDataFile,
                               const AString& filename,
                               const StructureEnum::Enum structure,
                               const bool markDataFileAsModified) throw (DataFileException);
        
        Surface* addReadOrReloadSurfaceFile(const FileModeAddReadReload fileMode,
                                 CaretDataFile* caretDataFile,
                                 const AString& filename,
                                 const StructureEnum::Enum structure,
                                 const bool markDataFileAsModified) throw (DataFileException);
        
        VolumeFile* addReadOrReloadVolumeFile(const FileModeAddReadReload fileMode,
                                   CaretDataFile* caretDataFile,
                                   const AString& filename) throw (DataFileException);
                            
        BorderFile* addReadOrReloadBorderFile(const FileModeAddReadReload fileMode,
                                   CaretDataFile* caretDataFile,
                                   const AString& filename) throw (DataFileException);
        
        CiftiConnectivityMatrixDenseFile* addReadOrReloadConnectivityDenseFile(const FileModeAddReadReload fileMode,
                                                                    CaretDataFile* caretDataFile,
                                                                    const AString& filename) throw (DataFileException);
        
        CiftiBrainordinateLabelFile* addReadOrReloadConnectivityDenseLabelFile(const FileModeAddReadReload fileMode,
                                                                    CaretDataFile* caretDataFile,
                                                                    const AString& filename) throw (DataFileException);
        
        CiftiConnectivityMatrixDenseParcelFile* addReadOrReloadConnectivityMatrixDenseParcelFile(const FileModeAddReadReload fileMode,
                                                                                      CaretDataFile* caretDataFile,
                                                                                      const AString& filename) throw (DataFileException);
        
        CiftiBrainordinateScalarFile* addReadOrReloadConnectivityDenseScalarFile(const FileModeAddReadReload fileMode,
                                                                      CaretDataFile* caretDataFile,
                                                                      const AString& filename) throw (DataFileException);
        
        CiftiParcelScalarFile* addReadOrReloadConnectivityParcelScalarFile(const FileModeAddReadReload fileMode,
                                                                CaretDataFile* caretDataFile,
                                                                const AString& filename) throw (DataFileException);
        
        CiftiParcelSeriesFile* addReadOrReloadConnectivityParcelSeriesFile(const FileModeAddReadReload fileMode,
                                                                CaretDataFile* caretDataFile,
                                                                const AString& filename) throw (DataFileException);
        
        CiftiFiberOrientationFile* addReadOrReloadConnectivityFiberOrientationFile(const FileModeAddReadReload fileMode,
                                                                        CaretDataFile* caretDataFile,
                                                                        const AString& filename) throw (DataFileException);
        
        CiftiFiberTrajectoryFile* addReadOrReloadConnectivityFiberTrajectoryFile(const FileModeAddReadReload fileMode,
                                                                      CaretDataFile* caretDataFile,
                                                                      const AString& filename) throw (DataFileException);
        
        CiftiConnectivityMatrixParcelFile* addReadOrReloadConnectivityMatrixParcelFile(const FileModeAddReadReload fileMode,
                                                                            CaretDataFile* caretDataFile,
                                                                            const AString& filename) throw (DataFileException);
        
        CiftiConnectivityMatrixParcelDenseFile* addReadOrReloadConnectivityMatrixParcelDenseFile(const FileModeAddReadReload fileMode,
                                                                                      CaretDataFile* caretDataFile,
                                                                                      const AString& filename) throw (DataFileException);
        
        CiftiBrainordinateDataSeriesFile* addReadOrReloadConnectivityDataSeriesFile(const FileModeAddReadReload fileMode,
                                                                         CaretDataFile* caretDataFile,
                                                                         const AString& filename) throw (DataFileException);
        
        FociFile* addReadOrReloadFociFile(const FileModeAddReadReload fileMode,
                               CaretDataFile* caretDataFile,
                               const AString& filename) throw (DataFileException);
        
        PaletteFile* addReadOrReloadPaletteFile(const FileModeAddReadReload fileMode,
                                     CaretDataFile* caretDataFile,
                                     const AString& filename) throw (DataFileException);
        
        SceneFile* addReadOrReloadSceneFile(const FileModeAddReadReload fileMode,
                                 CaretDataFile* caretDataFile,
                                 const AString& filename) throw (DataFileException);
        
        AString updateFileNameForReading(const AString& filename);
        
        AString updateFileNameForWriting(const AString& filename) throw (DataFileException);
        
        void updateChartModel();
        
        void updateVolumeSliceModel();
        
        void updateWholeBrainModel();
        
        void updateSurfaceMontageModel();
        
        void updateFiberTrajectoryMatchingFiberOrientationFiles();
        
        void validateCiftiMappableDataFile(const CiftiMappableDataFile* ciftiMapFile) const throw (DataFileException);
        
        std::vector<BrainStructure*> m_brainStructures;
        
        std::vector<BorderFile*> m_borderFiles;
        
        std::vector<FociFile*> m_fociFiles;
        
        std::vector<SceneFile*> m_sceneFiles;
        
        PaletteFile* m_paletteFile;
        
        std::vector<CiftiConnectivityMatrixDenseFile*> m_connectivityMatrixDenseFiles;
        
        std::vector<CiftiBrainordinateLabelFile*> m_connectivityDenseLabelFiles;
        
        std::vector<CiftiConnectivityMatrixDenseParcelFile*> m_connectivityMatrixDenseParcelFiles;
        
        std::vector<CiftiBrainordinateScalarFile*> m_connectivityDenseScalarFiles;
        
        std::vector<CiftiParcelSeriesFile*> m_connectivityParcelSeriesFiles;
        
        std::vector<CiftiParcelScalarFile*> m_connectivityParcelScalarFiles;
        
        std::vector<CiftiFiberOrientationFile*> m_connectivityFiberOrientationFiles;
        
        std::vector<CiftiFiberTrajectoryFile*> m_connectivityFiberTrajectoryFiles;
        
        std::vector<CiftiConnectivityMatrixParcelFile*> m_connectivityMatrixParcelFiles;
        
        std::vector<CiftiConnectivityMatrixParcelDenseFile*> m_connectivityMatrixParcelDenseFiles;
        
        std::vector<CiftiBrainordinateDataSeriesFile*> m_connectivityDataSeriesFiles;
        
        std::vector<CaretDataFile*> m_nonModifiedFilesForRestoringScene;
        
        mutable AString m_currentDirectory;
        
        SpecFile* m_specFile;
        
        std::vector<VolumeFile*> m_volumeFiles;
        
        ModelChart* m_modelChart;
        
        ModelVolume* m_volumeSliceModel;
        
        ModelWholeBrain* m_wholeBrainModel;
        
        ModelSurfaceMontage* m_surfaceMontageModel;
        
        ChartingDataManager* m_chartingDataManager;
        
        /** contains all display properties */
        std::vector<DisplayProperties*> m_displayProperties;
        
        /**
         * Display properties for volume - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesVolume* m_displayPropertiesVolume;
        
        /**
         * Display properties for surface - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesSurface* m_displayPropertiesSurface;
        
        /**
         * Display properties for labels - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesLabels* m_displayPropertiesLabels;
        
        /**
         * Display properties for borders - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesBorders* m_displayPropertiesBorders;
        
        /**
         * Display properties for fiber orientation - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesFiberOrientation* m_displayPropertiesFiberOrientation;

        /**
         * Display properties for foci - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesFoci* m_displayPropertiesFoci;
        
        /** true when a spec file is being read */
        bool m_isSpecFileBeingRead;
        
        SceneClassAssistant* m_sceneAssistant;
        
        /** Selection manager */
        SelectionManager* m_selectionManager;
        
        /** Identification Manager */
        IdentificationManager* m_identificationManager;
        
        /** The loader of fiber orientation samples */
        FiberOrientationSamplesLoader* m_fiberOrientationSamplesLoader;
        
    };

} // namespace

#endif // __BRAIN_H__

