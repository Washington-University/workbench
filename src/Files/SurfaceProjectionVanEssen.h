#ifndef __SURFACE_PROJECTION_VAN_ESSEN__H_
#define __SURFACE_PROJECTION_VAN_ESSEN__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "SurfaceProjection.h"

namespace caret {

    class SurfaceProjectionVanEssen : public SurfaceProjection {
        
    public:
        SurfaceProjectionVanEssen();
        
        virtual ~SurfaceProjectionVanEssen();
        
        SurfaceProjectionVanEssen(const SurfaceProjectionVanEssen& obj);

        SurfaceProjectionVanEssen& operator=(const SurfaceProjectionVanEssen& obj);
        
        bool unprojectToSurface(const SurfaceFile& surfaceFile,
                                float xyzOut[3],
                                const float offsetFromSurface,
                                const bool unprojectWithOffsetFromSurface) const;
        
        float getDR() const;
        
        void setDR(const float dR);
        
        float getThetaR() const;
        
        void setThetaR(const float thetaR);
        
        float getPhiR() const;
        
        void setPhiR(const float phiR);
        
        float getFracRI() const;
        
        void setFracRI(const float fracRI);
        
        float getFracRJ() const;
        
        void setFracRJ(const float fracRJ);
        
        void setTriVertices(const int32_t triVertices[2][3]);
        
        void setTriVertices(const int32_t indx1,
                            const int32_t vertices[3]);
        
        void getTriVertices(int32_t triVertices[2][3]) const;
        
        void setVertex(const int32_t vertex[2]);
        
        void setVertex(const int32_t indx1,
                       const int32_t vertex);
        
        void getVertex(int32_t vertex[2]) const;
        
        void setTriAnatomical(const float triAnatomical[2][3][3]);
        
        void setTriAnatomical(const int32_t indx1,
                              const int32_t indx2,
                              const float anatomical[3]);
        
        void getTriAnatomical(float triAnatomical[2][3][3]) const;
        
        void setVertexAnatomical(const float vertexAnatomical[2][3]);
        
        void setVertexAnatomical(const int32_t indx1,
                                 const float anatomical[3]);
        
        void getVertexAnatomical(float vertexAnatomical[2][3]) const;
        
        void getPosAnatomical(float posAnatomical[3]) const;
        
        void setPosAnatomical(const float posAnatomical[3]);
        
        void reset();
        
        bool isValid() const;
        
        void setValid(const bool valid);
        
        virtual AString toString() const;
        
        void writeAsXML(XmlWriter& xmlWriter) throw (XmlException);
        
        static  const AString XML_TAG_PROJECTION_VAN_ESSEN;
        
        static  const AString XML_TAG_DR;
        
        static  const AString XML_TAG_TRI_ANATOMICAL;
        
        static  const AString XML_TAG_THETA_R;
        
        static  const AString XML_TAG_PHI_R;
        
        static  const AString XML_TAG_TRI_VERTICES;
        
        static  const AString XML_TAG_VERTEX;
        
        static  const AString XML_TAG_VERTEX_ANATOMICAL;
        
        static  const AString XML_TAG_POS_ANATOMICAL;
        
        static  const AString XML_TAG_FRAC_RI;
        
        static  const AString XML_TAG_FRAC_RJ;
        
    private:
        void copyHelperSurfaceProjectionVanEssen(const SurfaceProjectionVanEssen& obj);
        
        void computeProjectionPoint(float projection[3]) const;
        
        void resetAllValues();
        
        float dR;
        
        float thetaR;
        
        float phiR;
        
        float fracRI;
        
        float fracRJ;
        
        int32_t triVertices[2][3];
        
        int32_t vertex[2];
        
        float triAnatomical[2][3][3];
        
        float vertexAnatomical[2][3];
        
        float posAnatomical[3];
        
        bool projectionValid;
        
    };
    
#ifdef __SURFACE_PROJECTION_VAN_ESSEN_DECLARE__
      const AString SurfaceProjectionVanEssen::XML_TAG_PROJECTION_VAN_ESSEN = "VanEssenProjection";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_DR = "DR";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_TRI_ANATOMICAL = "TriAnatomical";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_THETA_R = "ThetaR";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_PHI_R = "PhiR";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_TRI_VERTICES = "TriVertices";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_VERTEX = "Vertex";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_VERTEX_ANATOMICAL = "VertexAnatomical";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_POS_ANATOMICAL = "PosAnatomical";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_FRAC_RI = "FracRI";
    
      const AString SurfaceProjectionVanEssen::XML_TAG_FRAC_RJ = "FracRJ";
#endif // __SURFACE_PROJECTION_VAN_ESSEN_DECLARE__

} // namespace
#endif  //__SURFACE_PROJECTION_VAN_ESSEN__H_
