#ifndef __SURFACE_PROJECTED_ITEM_H__
#define __SURFACE_PROJECTED_ITEM_H__

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


#include "CaretObjectTracksModification.h"
#include "StructureEnum.h"
#include "XmlException.h"

class QXmlStreamReader;

namespace caret {
    
    class SurfaceFile;
    class SurfaceProjectionBarycentric;
    class SurfaceProjectionVanEssen;
    class TopologyHelper;
    class XmlWriter;
    
    class SurfaceProjectedItem : public CaretObjectTracksModification {
        
    public:
        SurfaceProjectedItem();
        
        SurfaceProjectedItem(const SurfaceProjectedItem& o);
        
        SurfaceProjectedItem& operator=(const SurfaceProjectedItem& o);
        
        bool operator==(const SurfaceProjectedItem& rhs) const;
        
        bool operator!=(const SurfaceProjectedItem& rhs) const { return !(*this == rhs); }
        
        virtual ~SurfaceProjectedItem();
        
    private:
        void copyHelper(const SurfaceProjectedItem& o);
        
        void initializeMembersSurfaceProjectedItem();
        
    public:
        void unprojectToStereotaxicXYZ(const SurfaceFile& sf,
                                       const bool isUnprojectedOntoSurface);
        
        void unprojectToVolumeXYZ(const SurfaceFile& sf,
                                  const bool isUnprojectedOntoSurface);
        
        bool getProjectedPosition(const SurfaceFile* sf,
                                  float xyzOut[3],
                                  const bool isUnprojectedOntoSurface) const;
        
        bool getProjectedPosition(const SurfaceFile& sf,
                                  float xyzOut[3],
                                  const bool isUnprojectedOntoSurface) const;
        
        bool getProjectedPositionAboveSurface(const SurfaceFile& sf,
                                              const TopologyHelper* th,
                                              float xyzOut[3],
                                              const float distanceAboveSurface) const;
        
        const float* getStereotaxicXYZ() const;
        
        void getStereotaxicXYZ(float stereotaxicXYZOut[3]) const;
        
        bool isStereotaxicXYZValid() const;
        
        void setStereotaxicXYZ(const float stereotaxicXYZ[3]);
        
        const float* getVolumeXYZ() const;
        
        void getVolumeXYZ(float xyzOut[3]) const;
        
        bool isVolumeXYZValid() const;
        
        void setVolumeXYZ(const float volumeXYZ[3]);
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        const SurfaceProjectionBarycentric* getBarycentricProjection() const;
        
        SurfaceProjectionBarycentric* getBarycentricProjection();
        
        const SurfaceProjectionVanEssen* getVanEssenProjection() const;
        
        SurfaceProjectionVanEssen* getVanEssenProjection();
        
        bool hasValidProjection() const;
        
        void reset();
        
        void writeAsXML(XmlWriter& xmlWriter);
        
        void readBorderFileXML1(QXmlStreamReader& xml);
        
        virtual void clearModified();
        
        virtual bool isModified() const;
        
        static AString XML_TAG_SURFACE_PROJECTED_ITEM;
        static AString XML_TAG_STEREOTAXIC_XYZ;
        static AString XML_TAG_VOLUME_XYZ;
        static AString XML_TAG_STRUCTURE;
        
    protected:
        /** stereotaxic position of projected item. */
        float stereotaxicXYZ[3];
        
        /** stereotaxic position of projected item valid */
        bool stereotaxicXYZValid;
        
        /** position in volume */
        float volumeXYZ[3];
        
        /** position in volume valid */
        bool volumeXYZValid;
        
        /** Structure to which projected. */
        StructureEnum::Enum structure;
        
        /** The barycentric projection */
        SurfaceProjectionBarycentric* barycentricProjection;
        
        /** The Van Essen projection */
        SurfaceProjectionVanEssen* vanEssenProjection;
    };
    
#ifdef __SURFACE_PROJECTED_ITEM_DEFINE__
    AString SurfaceProjectedItem::XML_TAG_SURFACE_PROJECTED_ITEM = "SurfaceProjectedItem";
    AString SurfaceProjectedItem::XML_TAG_STEREOTAXIC_XYZ        = "StereotaxicXYZ";
    AString SurfaceProjectedItem::XML_TAG_VOLUME_XYZ             = "VolumeXYZ";
    AString SurfaceProjectedItem::XML_TAG_STRUCTURE              = "Structure";
#endif // __SURFACE_PROJECTED_ITEM_DEFINE__
    
} // namespace

#endif // __SURFACE_PROJECTED_ITEM_H__
