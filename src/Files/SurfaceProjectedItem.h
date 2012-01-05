#ifndef __SURFACEPROJECTEDITEM_H__
#define __SURFACEPROJECTEDITEM_H__

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


#include "CaretObjectTracksModification.h"
#include "StructureEnum.h"
#include "SurfaceProjectionTypeEnum.h"

namespace caret {
    
    class SurfaceFile;
    class SurfaceProjectionBarycentric;
    class SurfaceProjectionVanEssen;
    
    /**
     * Information about something (focus, border point, etc)
     * projected to a surface.
     */
    class SurfaceProjectedItem : public CaretObjectTracksModification {
        
    public:
        SurfaceProjectedItem();
        
        SurfaceProjectedItem(const float xyz[3]);
        
        SurfaceProjectedItem(const SurfaceProjectedItem& o);
        
        SurfaceProjectedItem& operator=(const SurfaceProjectedItem& o);
        
        virtual ~SurfaceProjectedItem();
        
    private:
        void copyHelper(const SurfaceProjectedItem& o);
        
        void initializeMembersSurfaceProjectedItem();
        
    public:
        void unprojectToOriginalXYZ(
                                    const SurfaceFile& sf,
                                    const bool isUnprojectedOntoSurface);
        
        void unprojectToVolumeXYZ(
                                  const SurfaceFile& sf,
                                  const bool isUnprojectedOntoSurface);
        
        bool getProjectedPosition(
                                  const SurfaceFile& sf,
                                  float xyzOut[3],
                                  const bool isUnprojectedOntoSurface) const;
        
        const float* getOriginalXYZ() const;
        
        void getOriginalXYZ(float xyzOut[3]) const;
        
        bool isOriginalXYZValid() const;
        
        void setOriginalXYZ(const float xyz[3]);
        
        const float* getVolumeXYZ() const;
        
        void getVolumeXYZ(float xyzOut[3]) const;
        
        bool isVolumeXYZValid() const;
        
        void setVolumeXYZ(const float volumeXYZ[3]);
        
        SurfaceProjectionTypeEnum::Enum getProjectionType() const;
        
        void setProjectionType(const SurfaceProjectionTypeEnum::Enum projectionType);
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
    public:
        /**Tag for root element of a SurfaceProjectedItem */
        static  const std::string XML_TAG_SURFACE_PROJECTED_ITEM;
        
    protected:
        /**projection type. */
        SurfaceProjectionTypeEnum::Enum projectionType;
        
        /**original position of projected item. */
        float originalXYZ[3];
        
        /** Position in volume */
        float volumeXYZ[3];
        
        /** Structure to which projected. */
        StructureEnum::Enum structure;
        
        /** The barycentric projection */
        SurfaceProjectionBarycentric* barycentricProjection;
        
        /** The Van Essen projection */
        SurfaceProjectionVanEssen* vanEssenProjection;
        
    private:
        static  const std::string XML_TAG_STRUCTURE;
        
        static  const std::string XML_TAG_ORIGINAL_XYZ;
        
        static  const std::string XML_TAG_VOLUME_XYZ;
        
        static  const std::string XML_TAG_PROJECTION_TYPE;
    };
    
} // namespace

#endif // __SURFACEPROJECTEDITEM_H__
