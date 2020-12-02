#ifndef __EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS_H__
#define __EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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


#include <map>
#include <memory>
#include <set>

#include "Event.h"



namespace caret {

    class CaretMappableDataFile;
    
    class EventCaretMappableDataFilesAndMapsInDisplayedOverlays : public Event {
        
    public:
        enum class OverlayType {
            BRAINORDINATE,
            CHART_ONE,
            CHART_TWO
        };

        class FileInfo {
        public:
            FileInfo(OverlayType overlayType,
                     CaretMappableDataFile* mapFile,
                     const std::set<int32_t>& mapIndices);
            
            const OverlayType m_overlayType;
            
            CaretMappableDataFile* m_mapFile;
            
            const std::set<int32_t> m_mapIndices;
        };
        
        EventCaretMappableDataFilesAndMapsInDisplayedOverlays();
        
        virtual ~EventCaretMappableDataFilesAndMapsInDisplayedOverlays();
        
        EventCaretMappableDataFilesAndMapsInDisplayedOverlays(const EventCaretMappableDataFilesAndMapsInDisplayedOverlays&) = delete;

        EventCaretMappableDataFilesAndMapsInDisplayedOverlays& operator=(const EventCaretMappableDataFilesAndMapsInDisplayedOverlays&) = delete;
        
        void setWindowIndexConstraint(const int32_t windowIndex);
        
        void setTabIndicesConstraint(const std::set<int32_t>& tabIndices);
        
        void addBrainordinateFileAndMap(CaretMappableDataFile* mapFile,
                                        const int32_t mapIndex,
                                        const int32_t tabIndex);

        void addChartOneFileAndMap(CaretMappableDataFile* mapFile,
                                   const int32_t mapIndex,
                                   const int32_t tabIndex);
        
        void addChartTwoFileAndMap(CaretMappableDataFile* mapFile,
                                   const int32_t mapIndex,
                                   const int32_t tabIndex);
        
//        void addMediaFileAndFrame(MediaFile* mediaFile,
//                                  const int32_t frameIndex,
//                                  const int32_ tabIndex);
                
        std::map<CaretMappableDataFile*, std::set<int32_t>> getMapFilesAndIndices() const;
        
        std::vector<FileInfo> getFilesAndMaps() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        bool satisfiesConstraints(const int32_t tabIndex);
        
        std::map<CaretMappableDataFile*, std::set<int32_t>> m_mapFilesAndIndices;
        
        std::map<CaretMappableDataFile*, std::set<int32_t>> m_surfaceVolumeMapFilesAndIndices;
        
        std::map<CaretMappableDataFile*, std::set<int32_t>> m_chartOneMapFilesAndIndices;
        
        std::map<CaretMappableDataFile*, std::set<int32_t>> m_chartTwoMapFilesAndIndices;
        
        int32_t m_windowIndex = -1;
        
        std::set<int32_t> m_tabIndices;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS_DECLARE__

} // namespace
#endif  //__EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS_H__
