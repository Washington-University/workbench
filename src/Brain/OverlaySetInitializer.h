#ifndef __OVERLAY_SET_INITIALIZER_H__
#define __OVERLAY_SET_INITIALIZER_H__

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


#include <cstdint>
#include <memory>
#include <vector>

#include "CaretObject.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"
#include "VolumeFile.h"

namespace caret {

    class CaretMappableDataFile;
    class CiftiBrainordinateLabelFile;
    class CiftiBrainordinateScalarFile;
    class LabelFile;
    class MetricFile;
    
    class OverlaySetInitializer : public CaretObject {
        
    public:
        class FileAndMapIndex {
        public:
            enum class Layer {
                UNKNOWN,
                UNDERLAY,
                MIDDLE,
                OVERLAY
            };
            
            FileAndMapIndex()
            : m_file(NULL),
            m_mapIndex(-1),
            m_layer(Layer::UNKNOWN)
            { }
            
            FileAndMapIndex(CaretMappableDataFile* file,
                            const int32_t mapIndex,
                            const Layer layer)
            : m_file(file),
            m_mapIndex(mapIndex),
            m_layer(layer)
            { }
            
            CaretMappableDataFile* m_file = NULL;
            int32_t m_mapIndex = -1;
            Layer m_layer = Layer::UNKNOWN;
            
            bool operator<(const FileAndMapIndex& fmi) const {
                if (m_file < fmi.m_file) {
                    return true;
                }
                if (m_file > fmi.m_file) {
                    return false;
                }
                return (m_mapIndex < fmi.m_mapIndex);
            }
        };
        
        OverlaySetInitializer();
        
        virtual ~OverlaySetInitializer();
        
        OverlaySetInitializer(const OverlaySetInitializer&) = delete;

        OverlaySetInitializer& operator=(const OverlaySetInitializer&) = delete;

        std::vector<FileAndMapIndex> initializeOverlaySet(const std::vector<StructureEnum::Enum>& matchToStructuresIn,
                                                          const bool matchToVolumeData,
                                                          const bool logFlag);

        // ADD_NEW_METHODS_HERE

    private:
        /** Filename and map matching mode */
        enum class MatchToNameMode {
            /** Match to names */
            MATCH,
            /** Do NOT match to ANY names */
            INVERSE_MATCH
        };

        std::vector<FileAndMapIndex> findUnderlayFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                                       const bool includeVolumeFiles) const;
        
        std::vector<FileAndMapIndex> findMiddleLayerFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                                          const bool includeVolumeFiles) const;
        
        std::vector<FileAndMapIndex> findOverlayFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                                      const bool includeVolumeFiles) const;
        
        FileAndMapIndex getMatchingFilesAndMapsForStructureOrVolume(const DataFileTypeEnum::Enum primaryDataFileType,
                                                                    const DataFileTypeEnum::Enum secondaryDataFileType,
                                                                    const StructureEnum::Enum structureIn,
                                                                    const std::vector<AString>& matchToNames,
                                                                    const MatchToNameMode matchToNameMode,
                                                                    const FileAndMapIndex::Layer layer) const;

        std::vector<VolumeFile*> getVolumeFilesOfType(const SubvolumeAttributes::VolumeType volumeType) const;
        
        std::vector<CaretMappableDataFile*> getDataFilesOfType(const DataFileTypeEnum::Enum dataFileType) const;

        std::vector<AString> m_matchingNamesShapeData;
        
        std::vector<AString> m_matchingNamesMyelinData;
        
        std::vector<AString> m_matchingNamesMyelinAndShapeData;
        
        // ADD_NEW_MEMBERS_HERE

        /** regular expression for matching myeline names - NOT saved to scenes */
        static const AString s_myelinMatchRegularExpressionText;
        
        /** regular expression for matching shape names - NOT saved to scenes */
        static const AString s_shapeMatchRegularExpressionText;
        
        /** regular expression for matching shape and myelin names - NOT saved to scenes */
        static const AString s_shapeMyelinMatchRegularExpressionText;

    };
    
#ifdef __OVERLAY_SET_INITIALIZER_DECLARE__
    AString const OverlaySetInitializer::s_myelinMatchRegularExpressionText = "(myelin)";
    AString const OverlaySetInitializer::s_shapeMatchRegularExpressionText = "(sulc|shape|curv|depth|thick)";
    AString const OverlaySetInitializer::s_shapeMyelinMatchRegularExpressionText = "(myelin|sulc|shape|curv|depth|thick)";
#endif // __OVERLAY_SET_INITIALIZER_DECLARE__

} // namespace
#endif  //__OVERLAY_SET_INITIALIZER_H__
