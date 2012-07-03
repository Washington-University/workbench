#ifndef __BRAIN_H__
#define __BRAIN_H__

/*
 *  Copyright 1995-2002 Washington University School of Medicine
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
/*LICENSE_END*/

#include <vector>
#include <stdint.h>

#include "CaretObject.h"
#include "DataFileTypeEnum.h"
#include "DataFileException.h"
#include "EventListenerInterface.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {
    
    class Border;
    class BorderFile;
    class FociFile;
    class BrainStructure;
    class CaretDataFile;
    class ConnectivityLoaderFile;
    class ConnectivityLoaderManager;
    class DisplayProperties;
    class DisplayPropertiesBorders;
    class DisplayPropertiesFoci;
    class DisplayPropertiesInformation;
    class DisplayPropertiesVolume;
    class EventDataFileRead;
    class EventSpecFileReadDataFiles;
    class LabelFile;
    class MetricFile;
    class ModelSurfaceMontage;
    class ModelVolume;
    class ModelWholeBrain;
    class PaletteFile;
    class RgbaFile;
    class SceneClassAssistant;
    class SceneFile;
    class Surface;
    class SurfaceFile;
    class SurfaceProjectedItem;
    class SpecFile;
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

        BrainStructure* getBrainStructure(StructureEnum::Enum structure,
                                          bool createIfNotFound);

        int32_t getNumberOfBorderFiles() const;
        
        BorderFile* getBorderFile(const int32_t indx);
        
        const BorderFile* getBorderFile(const int32_t indx) const;
        
        BorderFile* addBorderFile();
        
        enum NearestBorderTestMode {
            NEAREST_BORDER_TEST_MODE_ALL_POINTS,
            NEAREST_BORDER_TEST_MODE_ENDPOINTS
        };
        bool findBorderNearestBorder(const SurfaceFile* surfaceFile,
                                    const Border* border,
                                    const NearestBorderTestMode borderTestMode,
                                    const float maximumDistance,
                                    BorderFile*& borderFileOut,
                                    int32_t& borderFileIndexOut,
                                    Border*& borderOut,
                                    int32_t& borderIndexOut,
                                    SurfaceProjectedItem*& borderPointOut,
                                    int32_t& borderPointIndexOut,
                                    float& distanceToBorderPointOut) const;
                                    
        bool findBorderNearestXYZ(const SurfaceFile* surfaceFile,
                                 const float xyz[3],
                                 const float maximumDistance,
                                 BorderFile*& borderFileOut,
                                 int32_t& borderFileIndexOut,
                                 Border*& borderOut,
                                 int32_t& borderIndexOut,
                                 SurfaceProjectedItem*& borderPointOut,
                                 int32_t& borderPointIndexOut,
                                 float& distanceToBorderPointOut) const;
        
        int32_t getNumberOfFociFiles() const;
        
        FociFile* getFociFile(const int32_t indx);
        
        const FociFile* getFociFile(const int32_t indx) const;
        
        FociFile* addFociFile();
        
        PaletteFile* getPaletteFile();
        
        const PaletteFile* getPaletteFile() const;

        SceneFile* addSceneFile();
        
        int32_t getNumberOfSceneFiles() const;
        
        SceneFile* getSceneFile(const int32_t indx);
        
        const SceneFile* getSceneFile(const int32_t indx) const;
        
        SpecFile* getSpecFile();
        
        int32_t getNumberOfVolumeFiles() const;
        
        VolumeFile* getVolumeFile(const int32_t volumeFileIndex);
        
        const VolumeFile* getVolumeFile(const int32_t volumeFileIndex) const;
        
        void resetBrain();
        
        void resetBrainKeepSceneFiles();
        
        void receiveEvent(Event* event);
        
        ConnectivityLoaderManager* getConnectivityLoaderManager();
        
        const ConnectivityLoaderManager* getConnectivityLoaderManager() const;
        
        void getConnectivityFilesOfAllTypes(std::vector<ConnectivityLoaderFile*>& connectivityFilesOfAllTypes) const;
        
        int32_t getNumberOfConnectivityDenseFiles() const;
        
        ConnectivityLoaderFile* getConnectivityDenseFile(int32_t indx);
        
        const ConnectivityLoaderFile* getConnectivityDenseFile(int32_t indx) const;
        
        void getConnectivityDenseFiles(std::vector<ConnectivityLoaderFile*>& connectivityDenseFilesOut) const;
        
        int32_t getNumberOfConnectivityTimeSeriesFiles() const;
        
        ConnectivityLoaderFile* getConnectivityTimeSeriesFile(int32_t indx);
        
        const ConnectivityLoaderFile* getConnectivityTimeSeriesFile(int32_t indx) const;
        
        void getConnectivityTimeSeriesFiles(std::vector<ConnectivityLoaderFile*>& connectivityTimeSeriesFilesOut) const;
        
        AString getCurrentDirectory() const;
        
        void setCurrentDirectory(const AString& currentDirectory);
        
        void getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut);
        
        void determineDisplayedDataFiles();
        
        void writeDataFile(CaretDataFile* caretDataFile,
                           const bool isAddToSpecFile) throw (DataFileException);
        
        bool removeDataFile(CaretDataFile* caretDataFile);
        
        DisplayPropertiesBorders* getDisplayPropertiesBorders();
        
        const DisplayPropertiesBorders* getDisplayPropertiesBorders() const;
        
        DisplayPropertiesFoci* getDisplayPropertiesFoci();
        
        const DisplayPropertiesFoci* getDisplayPropertiesFoci() const;
        
        DisplayPropertiesVolume* getDisplayPropertiesVolume();
        
        const DisplayPropertiesVolume* getDisplayPropertiesVolume() const;
        
        DisplayPropertiesInformation* getDisplayPropertiesInformation();
        
        const DisplayPropertiesInformation* getDisplayPropertiesInformation() const;
        
        void copyDisplayProperties(const int32_t sourceTabIndex,
                                   const int32_t targetTabIndex);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        enum ResetBrainKeepSceneFiles {
            RESET_BRAIN_KEEP_SCENE_FILES_NO,
            RESET_BRAIN_KEEP_SCENE_FILES_YES
        };
        
        void loadFilesSelectedInSpecFile(EventSpecFileReadDataFiles* readSpecFileDataFilesEvent);
        
        bool loadSpecFile(SpecFile* specFile,
                          const ResetBrainKeepSceneFiles keepSceneFiles,
                          AString& errorMessageOut);
        
        void resetBrain(const ResetBrainKeepSceneFiles keepSceneFiles);
        
        void processReadDataFileEvent(EventDataFileRead* readDataFileEvent);
        
        void readDataFile(const DataFileTypeEnum::Enum dataFileType,
                          const StructureEnum::Enum structure,
                          const AString& dataFileName,
                          const bool addDataFileToSpecFile) throw (DataFileException);
        
        LabelFile* readLabelFile(const AString& filename,
                           const StructureEnum::Enum structure) throw (DataFileException);
        
        MetricFile* readMetricFile(const AString& filename,
                            const StructureEnum::Enum structure) throw (DataFileException);
        
        RgbaFile* readRgbaFile(const AString& filename,
                          const StructureEnum::Enum structure) throw (DataFileException);
        
        Surface* readSurfaceFile(const AString& filename,
                             const StructureEnum::Enum structure) throw (DataFileException);
        
        void readVolumeFile(const AString& filename) throw (DataFileException);
                            
        void readBorderFile(const AString& filename) throw (DataFileException);
        
        void readConnectivityDenseFile(const AString& filename) throw (DataFileException);
        
        void readConnectivityTimeSeriesFile(const AString& filename) throw (DataFileException);
        
        void validateConnectivityFile(const ConnectivityLoaderFile* clf) throw (DataFileException);
        
        void readFociFile(const AString& filename) throw (DataFileException);
        
        void readPaletteFile(const AString& filename) throw (DataFileException);
        
        void readSceneFile(const AString& filename) throw (DataFileException);
        
        AString updateFileNameForReadingAndWriting(const AString& filename);
        
        void setFileSelectedStatusInSpecFile(CaretDataFile* dataFile,
                                             const bool selectedStatus);
        
        void updateVolumeSliceController();
        
        void updateWholeBrainController();
        
        void updateSurfaceMontageController();
        
        std::vector<BrainStructure*> brainStructures;
        
        std::vector<BorderFile*> borderFiles;
        
        std::vector<FociFile*> fociFiles;
        
        std::vector<SceneFile*> sceneFiles;
        
        PaletteFile* paletteFile;
        
        std::vector<ConnectivityLoaderFile*> connectivityDenseFiles;
        
        std::vector<ConnectivityLoaderFile*> connectivityTimeSeriesFiles;
        
        mutable AString currentDirectory;
        
        SpecFile* specFile;
        
        std::vector<VolumeFile*> volumeFiles;
        
        ModelVolume* volumeSliceController;
        
        ModelWholeBrain* wholeBrainController;
        
        ModelSurfaceMontage* surfaceMontageController;
        
        ConnectivityLoaderManager* connectivityLoaderManager;
        
        /** contains all display properties */
        std::vector<DisplayProperties*> displayProperties;
        
        /**
         * Display properties for volume - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesVolume* displayPropertiesVolume;
        
        /**
         * Display properties for borders - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesBorders* displayPropertiesBorders;
        
        /**
         * Display properties for foci - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesFoci* displayPropertiesFoci;
        
        /**
         * Display properties for information - DO NOT delete since this
         * is also in the displayProperties std::vector.
         */
        DisplayPropertiesInformation* displayPropertiesInformation;
        
        /** true when a spec file is being read */
        bool isSpecFileBeingRead;
        
        AString fileReadingUsername;
        
        AString fileReadingPassword;
        
        SceneClassAssistant* m_sceneAssistant;
    };

} // namespace

#endif // __BRAIN_H__

