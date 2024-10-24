#ifndef __SURFACE_PROJECTION_BARYCENTRIC__H_
#define __SURFACE_PROJECTION_BARYCENTRIC__H_

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


#include "SurfaceProjection.h"

class QXmlStreamReader;

namespace caret {

    class TopologyHelper;
    class SurfaceProjectionBarycentric : public SurfaceProjection {
        
    public:
        SurfaceProjectionBarycentric();
        
        virtual ~SurfaceProjectionBarycentric();
        
        SurfaceProjectionBarycentric(const SurfaceProjectionBarycentric& obj);

        SurfaceProjectionBarycentric& operator=(const SurfaceProjectionBarycentric& obj);
        
        bool operator==(const SurfaceProjectionBarycentric& rhs);
        
        bool operator!=(const SurfaceProjectionBarycentric& rhs) { return !(*this == rhs); }
        
        virtual AString toString() const;
        
        float getSignedDistanceAboveSurface() const;
        
        void setSignedDistanceAboveSurface(const float signedDistanceAboveSurface);
        
        const int32_t* getTriangleNodes() const;
        
        void setTriangleNodes(const int32_t triangleNodes[3]);
        
        const float* getTriangleAreas() const;
        
        void setTriangleAreas(const float triangleAreas[3]);
        
        int32_t getNodeWithLargestWeight() const;
                
        static bool unprojectToSurface(const SurfaceFile* surfaceFile,
                                       const float barycentricAreas[3],
                                       const int32_t barycentricVertices[3],
                                       float xyzOut[3]);

        bool unprojectToSurface(const SurfaceFile& surfaceFile,
                                float xyzOut[3],
                                const float offsetFromSurface,
                                const bool unprojectWithOffsetFromSurface) const;
        
        bool unprojectToSurface(const SurfaceFile& surfaceFile,
                                const TopologyHelper* topologyHelper,
                                float xyzOut[3],
                                const float offsetFromSurface,
                                const bool unprojectWithOffsetFromSurface) const;
        
        void reset();
        
        bool isValid() const;
        
        void setDegenerate(const bool degenerate);
        
        bool isDegenerate() const;
        
        void setValid(const bool valid);
        
        void writeAsXML(XmlWriter& xmlWriter);
        
        void readBorderFileXML1(QXmlStreamReader& xml);
        
        static  const AString XML_TAG_PROJECTION_BARYCENTRIC;
        
        static  const AString XML_TAG_TRIANGLE_NODES;
        
        static  const AString XML_TAG_TRIANGLE_AREAS;
        
        static  const AString XML_TAG_SIGNED_DISTANCE_ABOVE_SURFACE;
        
    private:
        void copyHelperSurfaceProjectionBarycentric(const SurfaceProjectionBarycentric& obj);
        
        void resetAllValues();
        
        int32_t triangleNodes[3];
        
        float triangleAreas[3];
        
        float signedDistanceAboveSurface;

        bool projectionValid;
        
        bool m_degenerate;
    };
    
#ifdef __SURFACE_PROJECTION_BARYCENTRIC_DECLARE__
      const AString SurfaceProjectionBarycentric::XML_TAG_PROJECTION_BARYCENTRIC = "ProjectionBarycentric";
    
      const AString SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_NODES = "TriangleNodes";
    
      const AString SurfaceProjectionBarycentric::XML_TAG_TRIANGLE_AREAS = "TriangleAreas";
    
      const AString SurfaceProjectionBarycentric::XML_TAG_SIGNED_DISTANCE_ABOVE_SURFACE = "SignedDistanceAboveSurface";
#endif // __SURFACE_PROJECTION_BARYCENTRIC_DECLARE__

} // namespace
#endif  //__SURFACE_PROJECTION_BARYCENTRIC__H_
