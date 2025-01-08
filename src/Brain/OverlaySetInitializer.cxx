
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __OVERLAY_SET_INITIALIZER_DECLARE__
#include "OverlaySetInitializer.h"
#undef __OVERLAY_SET_INITIALIZER_DECLARE__

#include <set>

#include <QRegularExpression>
#include <QTextStream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "PaletteColorMapping.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class caret::OverlaySetInitializer 
 * \brief Assists with initializing overlays after a spec file is read
 * \ingroup Brain
 */

/**
 * Constructor.
 */
OverlaySetInitializer::OverlaySetInitializer()
: CaretObject()
{
    /*
     * For matching shape data in priority order
     */
    m_matchingNamesShapeData.push_back("sulc");
    m_matchingNamesShapeData.push_back("shape");
    m_matchingNamesShapeData.push_back("curv");
    m_matchingNamesShapeData.push_back("depth");
    m_matchingNamesShapeData.push_back("thick");
    m_matchingNamesShapeData.push_back("gyri");

    /*
     * For matching myelin
     */
    m_matchingNamesMyelinData.push_back("myelin");
    
    /*
     * For matching both shape data and myelin
     */
    m_matchingNamesMyelinAndShapeData.insert(m_matchingNamesMyelinAndShapeData.end(),
                                             m_matchingNamesShapeData.begin(),
                                             m_matchingNamesShapeData.end());
    m_matchingNamesMyelinAndShapeData.insert(m_matchingNamesMyelinAndShapeData.end(),
                                             m_matchingNamesMyelinData.begin(),
                                             m_matchingNamesMyelinData.end());
}

/**
 * Destructor.
 */
OverlaySetInitializer::~OverlaySetInitializer()
{
}

/**
 * Find the files and maps to initialize an overlay set
 * @param matchToStructure
 *    Structures that need data
 * @param matchToVolumeData
 *    Include volume data
 * @param logFlag
 *   If true, log debugging info
 * @return
 *    Data files and maps for initializing the overlay set
 */
std::vector<OverlaySetInitializer::FileAndMapIndex>
OverlaySetInitializer::initializeOverlaySet(const std::vector<StructureEnum::Enum>& matchToStructuresIn,
                                            const bool matchToVolumeData,
                                            const bool logFlag)
{
    std::vector<StructureEnum::Enum> matchToStructures(matchToStructuresIn);

    std::vector<FileAndMapIndex> filesAndMapsOut;

    /*
     * Find underlay files (contain some sort of shape data; possibly a volume file)
     */
    std::vector<FileAndMapIndex> underlayFilesAndMaps(findUnderlayFiles(matchToStructures,
                                                                        matchToVolumeData));
    
    filesAndMapsOut.insert(filesAndMapsOut.end(),
                           underlayFilesAndMaps.begin(), underlayFilesAndMaps.end());
    
    /*
     * Do this AFTER finding underlays so that a file does not end up
     * as both underlay and an middle layer.
     * If match to structures are empty and matching to volume is true,
     * then we are setting overlays for a volume display.
     * Add "INVALID" to structures which is a kludge used to match to volume data.
     */
    if (matchToStructures.empty()) {
        if (matchToVolumeData) {
            matchToStructures.push_back(StructureEnum::INVALID);
        }
    }
    
    /*
     * Find middle layer files (myelin or none of shape/myelin)
     */
    std::vector<FileAndMapIndex> middleFilesAndMaps(findMiddleLayerFiles(matchToStructures,
                                                                         matchToVolumeData));
    
    filesAndMapsOut.insert(filesAndMapsOut.end(),
                           middleFilesAndMaps.begin(), middleFilesAndMaps.end());
    
    /*
     * Find overlay files (label files)
     */
    std::vector<FileAndMapIndex> overlayFilesAndMaps(findOverlayFiles(matchToStructures,
                                                                      matchToVolumeData));
    
    filesAndMapsOut.insert(filesAndMapsOut.end(),
                           overlayFilesAndMaps.begin(), overlayFilesAndMaps.end());
    
    if (logFlag) {
        QString txt;
        QTextStream stream(&txt);
        stream << "Structures:";
        for (const StructureEnum::Enum str : matchToStructures) {
            stream << " " << StructureEnum::toName(str);
        }
        stream << "\n";
        stream << "   INITIALIZED OVERLAYS:\n";
        for (const FileAndMapIndex& fmi : filesAndMapsOut) {
            AString layerText;
            switch (fmi.m_layer) {
                case FileAndMapIndex::Layer::UNKNOWN:
                    layerText = "UNKNOWN";
                    break;
                case FileAndMapIndex::Layer::UNDERLAY:
                    layerText = "UNDERLAY";
                    break;
                case FileAndMapIndex::Layer::MIDDLE:
                    layerText = "MIDDLE";
                    break;
                case FileAndMapIndex::Layer::OVERLAY:
                    layerText = "OVERLAY";
                    break;
            }
            stream << "      " << layerText
            << " " << fmi.m_file->getFileNameNoPath()
            << "   " << fmi.m_file->getMapName(fmi.m_mapIndex) << "\n";
        }
        CaretLogFine(txt.toStdString());
    }
    
    return filesAndMapsOut;
}

/**
 * Find underlay files.
 *
 * @param matchToStructures
 *    Structures to include.
 * @param includeVolumeFiles
 *    Include volume files.
 * @return
 *     Files and maps for the underlay
 */
std::vector<OverlaySetInitializer::FileAndMapIndex>
OverlaySetInitializer::findUnderlayFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                         const bool includeVolumeFiles) const
{
    std::vector<FileAndMapIndex> filesAndMapsOut;
    
    /*
     * Include a volume as underlay
     */
    if (includeVolumeFiles) {
        std::vector<VolumeFile*> volumeFiles = getVolumeFilesOfType(SubvolumeAttributes::ANATOMY);
        if ( ! volumeFiles.empty()) {
            CaretAssertVectorIndex(volumeFiles, 0);
            filesAndMapsOut.emplace_back(volumeFiles[0], 0, FileAndMapIndex::Layer::UNDERLAY);
        }
    }
    
    std::set<FileAndMapIndex> matchingFilesAndMaps;
    
    /*
     * Find dense scalar and metric files containing shape data
     */
    for (const StructureEnum::Enum structure : matchToStructures) {
        FileAndMapIndex fileAndMap(getMatchingFilesAndMapsForStructureOrVolume(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                                                                               DataFileTypeEnum::METRIC,
                                                                               structure,
                                                                               m_matchingNamesShapeData,
                                                                               MatchToNameMode::MATCH,
                                                                               FileAndMapIndex::Layer::UNDERLAY));
        if (fileAndMap.m_file != NULL) {
            matchingFilesAndMaps.insert(fileAndMap);
        }
    }
    
    filesAndMapsOut.insert(filesAndMapsOut.end(),
                           matchingFilesAndMaps.begin(),
                           matchingFilesAndMaps.end());
    
    return filesAndMapsOut;
}

/**
 * Find middle layer files.
 *
 * @param matchToStructures
 *    Structures to include.
 * @param includeVolumeFiles
 *    Include volume files.
 * @return
 *     Files and maps for the middle layer
 */
std::vector<OverlaySetInitializer::FileAndMapIndex>
OverlaySetInitializer::findMiddleLayerFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                            const bool includeVolumeFiles) const
{
    std::set<FileAndMapIndex> matchingFilesAndMaps;
    
    /*
     * Look for 'myelin' files
     */
    for (const StructureEnum::Enum structure : matchToStructures) {
        FileAndMapIndex fileAndMap(getMatchingFilesAndMapsForStructureOrVolume(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                                                                               DataFileTypeEnum::METRIC,
                                                                               structure,
                                                                               m_matchingNamesMyelinData,
                                                                               MatchToNameMode::MATCH,
                                                                               FileAndMapIndex::Layer::MIDDLE));
        if (fileAndMap.m_file != NULL) {
            matchingFilesAndMaps.insert(fileAndMap);
        }
    }
    
    if (matchingFilesAndMaps.empty()) {
        /*
         * look for files that are neither myelin nor shape
         */
        for (const StructureEnum::Enum structure : matchToStructures) {
            FileAndMapIndex fileAndMap(getMatchingFilesAndMapsForStructureOrVolume(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                                                                                   DataFileTypeEnum::METRIC,
                                                                                   structure,
                                                                                   m_matchingNamesMyelinAndShapeData,
                                                                                   MatchToNameMode::INVERSE_MATCH,
                                                                                   FileAndMapIndex::Layer::MIDDLE));
            if (fileAndMap.m_file != NULL) {
                matchingFilesAndMaps.insert(fileAndMap);
            }
        }
    }
    
    std::vector<FileAndMapIndex> filesAndMapsOut;
    filesAndMapsOut.insert(filesAndMapsOut.end(),
                           matchingFilesAndMaps.begin(),
                           matchingFilesAndMaps.end());
    
    /*
     * Find functional files
     */
    if (includeVolumeFiles) {
        std::vector<VolumeFile*> volumeFiles = getVolumeFilesOfType(SubvolumeAttributes::FUNCTIONAL);
        if ( ! volumeFiles.empty()) {
            CaretAssertVectorIndex(volumeFiles, 0);
            filesAndMapsOut.emplace_back(volumeFiles[0], 0, FileAndMapIndex::Layer::MIDDLE);
        }
    }
    
    return filesAndMapsOut;
}

/**
 * Find overlay files.
 *
 * @param matchToStructures
 *    Structures to include.
 * @param includeVolumeFiles
 *    Include volume files.
 * @return
 *     Files and maps for the overlay
 */
std::vector<OverlaySetInitializer::FileAndMapIndex>
OverlaySetInitializer::findOverlayFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                        const bool includeVolumeFiles) const
{
    std::set<FileAndMapIndex> matchingFilesAndMaps;
    
    /*
     * Both GIFT label and CIFTI label files have "label" in the file extension
     * use that for matching the file names
     */
    std::vector<AString> nameMatchingLabelData { "label" };
    
    /*
     * Find dense label files
     */
    for (const StructureEnum::Enum structure : matchToStructures) {
        FileAndMapIndex fileAndMap(getMatchingFilesAndMapsForStructureOrVolume(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL,
                                                                               DataFileTypeEnum::LABEL,
                                                                               structure,
                                                                               nameMatchingLabelData,
                                                                               MatchToNameMode::MATCH,
                                                                               FileAndMapIndex::Layer::OVERLAY));
        if (fileAndMap.m_file != NULL) {
            matchingFilesAndMaps.insert(fileAndMap);
        }
    }
    
    std::vector<FileAndMapIndex> filesAndMapsOut;
    
    /*
     * Find label volume files
     */
    if (includeVolumeFiles) {
        std::vector<VolumeFile*> volumeFiles = getVolumeFilesOfType(SubvolumeAttributes::LABEL);
        if ( ! volumeFiles.empty()) {
            CaretAssertVectorIndex(volumeFiles, 0);
            filesAndMapsOut.emplace_back(volumeFiles[0], 0, FileAndMapIndex::Layer::OVERLAY);
        }
    }
    
    filesAndMapsOut.insert(filesAndMapsOut.end(),
                           matchingFilesAndMaps.begin(),
                           matchingFilesAndMaps.end());
    
    return filesAndMapsOut;
}

/**
 * Find first Dense Scalar or Metric files that map to the given structure (or map to a volume file)
 * and that either match to a name or do not match to any name
 * @param primaryDataFileType
 *    The primary data file type to match
 * @param secondaryDataFileType
 *    The secondary data file type to match
 * @param structure
 *    Structure searching for, use OTHER when searching for files that map to volume
 * @param matchToNames
 *    Vector of names for matching
 * @param matchToNameMode
 *    Match or do not match
 * @param layer
 *    The underlay, middle, or overlay
 */
OverlaySetInitializer::FileAndMapIndex
OverlaySetInitializer::getMatchingFilesAndMapsForStructureOrVolume(const DataFileTypeEnum::Enum primaryDataFileType,
                                                                   const DataFileTypeEnum::Enum secondaryDataFileType,
                                                                   const StructureEnum::Enum structure,
                                                                   const std::vector<AString>& matchToNames,
                                                                   const MatchToNameMode matchToNameMode,
                                                                   const FileAndMapIndex::Layer layer) const
{
    FileAndMapIndex fileAndMapIndexOut;

    /* Contains files that map to the structure */
    std::set<CaretMappableDataFile*> mappingToStructureFiles;
    
    /* Contains files that map to the structure AND match to a name */
    std::set<CaretMappableDataFile*> matchingToNameFiles;
    
    for (const AString& matchName : matchToNames) {
        for (int32_t iFileType = 0; iFileType < 2; iFileType++) {
            std::vector<CaretMappableDataFile*> mapFiles;
            if (iFileType == 0) {
                mapFiles = getDataFilesOfType(primaryDataFileType);
            }
            else if (secondaryDataFileType != DataFileTypeEnum::UNKNOWN) {
                mapFiles = getDataFilesOfType(secondaryDataFileType);
            }
            
            for (CaretMappableDataFile* mf : mapFiles) {
                /*
                 * Does file map to volume or structure
                 */
                bool fileMapsToStructureFlag(false);
                if (structure == StructureEnum::INVALID) { /* INVALID is 'volume' */
                    if (mf->isVolumeMappable()) {
                        fileMapsToStructureFlag = true;
                    }
                }
                else {
                    if (mf->isMappableToSurfaceStructure(structure)) {
                        fileMapsToStructureFlag = true;
                    }
                }
                
                if (fileMapsToStructureFlag) {
                    /*
                     * Will be used if NOT matching to name but still
                     * needed to map to structure or volume
                     */
                    mappingToStructureFiles.insert(mf);
                    
                    /*
                     * Match to name of file
                     */
                    const bool nameMatchFlag(mf->getFileNameNoPath().contains(matchName,
                                                                              Qt::CaseInsensitive));
                    switch (matchToNameMode) {
                        case MatchToNameMode::MATCH:
                            if (nameMatchFlag) {
                                /*
                                 * Found file that matches by name so done
                                 */
                                return FileAndMapIndex(mf, 0, layer);
                            }
                            break;
                        case MatchToNameMode::INVERSE_MATCH:
                            if (nameMatchFlag) {
                                matchingToNameFiles.insert(mf);
                            }
                            break;
                    }
                    
                    /*
                     * Match to names of maps in file
                     */
                    const int32_t numMaps(mf->getNumberOfMaps());
                    for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                        const bool mapNameMatchFlag(mf->getMapName(iMap).contains(matchName,
                                                                                  Qt::CaseInsensitive));
                        switch (matchToNameMode) {
                            case MatchToNameMode::MATCH:
                                if (mapNameMatchFlag) {
                                    /*
                                     * Found map name that matches so done
                                     */
                                    return FileAndMapIndex(mf, 0, layer);
                                }
                                break;
                            case MatchToNameMode::INVERSE_MATCH:
                                if (nameMatchFlag) {
                                    matchingToNameFiles.insert(mf);
                                }
                                break;
                        }
                    }
                }
            }
        }
    }
    
    /*
     * If we got here, no files matched
     */
    switch (matchToNameMode) {
        case MatchToNameMode::MATCH:
            break;
        case MatchToNameMode::INVERSE_MATCH:
            /*
             * Find a file that maps to structure but
             * DOES NOT match to a name
             */
            for (CaretMappableDataFile* cmdf : mappingToStructureFiles) {
                if (matchingToNameFiles.find(cmdf) == matchingToNameFiles.end()) {
                    return FileAndMapIndex(cmdf, 0, layer);
                }
            }
            break;
    }
    
    return fileAndMapIndexOut;
}

/**
 * @return Mappable data files of the given type
 * @param dataFileType
 *   Type of the files
 */
std::vector<CaretMappableDataFile*>
OverlaySetInitializer::getDataFilesOfType(const DataFileTypeEnum::Enum dataFileType) const
{
    EventCaretMappableDataFilesGet mapFileGetEvent(dataFileType);
    EventManager::get()->sendEvent(mapFileGetEvent.getPointer());
    std::vector<CaretMappableDataFile*> filesOut;
    mapFileGetEvent.getAllFiles(filesOut);
    return filesOut;
}

/**
 * @return All volume files of the given type
 * @param volumeType
 *    Type of the volume file
 */
std::vector<VolumeFile*>
OverlaySetInitializer::getVolumeFilesOfType(const SubvolumeAttributes::VolumeType volumeType) const
{
    std::vector<VolumeFile*> volumeFilesOut;
    
    std::vector<CaretMappableDataFile*> dataFiles(getDataFilesOfType(DataFileTypeEnum::VOLUME));
    
    for (CaretMappableDataFile* cmdf : dataFiles) {
        VolumeFile* vf = dynamic_cast<VolumeFile*>(cmdf);
        CaretAssert(vf);
        if (vf->getNumberOfMaps() > 0) {
            if (vf->getType() == volumeType) {
                volumeFilesOut.push_back(vf);
            }
        }
    }
    
    /*
     * If searching for anatomy volume files and none found,
     * look for palette mapped volume file with a gray/grey
     * palette that is typically used with anatomy volumes
     */
    if (volumeFilesOut.empty()) {
        if (volumeType == SubvolumeAttributes::ANATOMY) {
            for (CaretMappableDataFile* cmdf : dataFiles) {
                VolumeFile* vf = dynamic_cast<VolumeFile*>(cmdf);
                CaretAssert(vf);
                if (vf->isMappedWithPalette()) {
                    if (vf->getNumberOfMaps() > 0) {
                        PaletteColorMapping* pcm = vf->getMapPaletteColorMapping(0);
                        if (pcm != NULL) {
                            const AString paletteName = pcm->getSelectedPaletteName();
                            if (paletteName.contains("gray")
                                || paletteName.contains("grey")) {
                                volumeFilesOut.push_back(vf);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return volumeFilesOut;
}
