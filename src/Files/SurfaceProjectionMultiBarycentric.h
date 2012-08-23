#ifndef __SURFACE_PROJECTION_MULTI_BARYCENTRIC__H_
#define __SURFACE_PROJECTION_MULTI_BARYCENTRIC__H_

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

    class SurfaceProjectionBarycentric;
    
    class SurfaceProjectionMultiBarycentric : public SurfaceProjection {
        
    public:
        SurfaceProjectionMultiBarycentric();
        
        virtual ~SurfaceProjectionMultiBarycentric();
        
        SurfaceProjectionMultiBarycentric(const SurfaceProjectionMultiBarycentric& obj);

        SurfaceProjectionMultiBarycentric& operator=(const SurfaceProjectionMultiBarycentric& obj);
        

        // ADD_NEW_METHODS_HERE

        virtual bool isValid() const;
        
        virtual void setValid(const bool valid);
        
        virtual void reset();
        
        void addProjection(SurfaceProjectionBarycentric* projection);
        
        virtual bool unprojectToSurface(const SurfaceFile& surfaceFile,
                                        float xyzOut[3],
                                        const float offsetFromSurface,
                                        const bool unprojectWithOffsetFromSurface) const;
        
        virtual void writeAsXML(XmlWriter& xmlWriter) throw (XmlException);
        
        virtual AString toString() const;
        
    private:
        void copyHelperSurfaceProjectionMultiBarycentric(const SurfaceProjectionMultiBarycentric& obj);

        void resetAllValues();
        
        int32_t getNumberOfProjections() const;
        
        std::vector<SurfaceProjectionBarycentric*> m_projections;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SURFACE_PROJECTION_MULTI_BARYCENTRIC_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_PROJECTION_MULTI_BARYCENTRIC_DECLARE__

} // namespace
#endif  //__SURFACE_PROJECTION_MULTI_BARYCENTRIC__H_
