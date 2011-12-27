#ifndef __EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION_H__
#define __EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION_H__

/*LICENSE_START*/ 
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

#include "Event.h"
#include "StructureEnum.h"

namespace caret {

    class BrainStructure;
    class VolumeFile;
    
    /// Highlight location of an indentification
    class EventIdentificationHighlightLocation : public Event {
        
    public:
        enum IdentificationType {
            IDENTIFICATION_SURFACE,
            IDENTIFICATION_VOLUME
        };
        
        EventIdentificationHighlightLocation(BrainStructure* brainStructure,
                                             const StructureEnum::Enum surfaceStructure,
                                             const int32_t surfaceNodeNumber,
                                             const int32_t surfaceNumberOfNodes,
                                             const float xyz[3]);
        
        EventIdentificationHighlightLocation(const VolumeFile* volumeFile,
                                             const int64_t volumeSliceIndices[3],
                                             const float xyz[3]);
                
        virtual ~EventIdentificationHighlightLocation();
        
        const VolumeFile* getVolumeFile() const;
        
        BrainStructure* getSurfaceBrainStructure();
        
        StructureEnum::Enum getSurfaceStructure() const;
        
        int32_t getSurfaceNodeNumber() const;
        
        int32_t getSurfaceNumberOfNodes() const;
        
        const float* getXYZ() const;
        
        const int64_t* getVolumeSliceIndices() const;
        
        IdentificationType getIdentificationType() const;
        
    private:
        EventIdentificationHighlightLocation(const EventIdentificationHighlightLocation&);
        
        EventIdentificationHighlightLocation& operator=(const EventIdentificationHighlightLocation&);
        
        void initializeMembers();
        
        const VolumeFile* volumeFile;
        
        BrainStructure* brainStructure;
        
        StructureEnum::Enum surfaceStructure;
        
        int32_t surfaceNodeNumber;
        
        int32_t surfaceNumberOfNodes;
        
        int64_t volumeSliceIndices[3];
        
        float xyz[3];
        
        IdentificationType identificationType;
    };

} // namespace

#endif // __EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION_H__
