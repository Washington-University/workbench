#ifndef __SURFACE_PROJECTION__H_
#define __SURFACE_PROJECTION__H_

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


#include "CaretObjectTracksModification.h"
#include "XmlException.h"

namespace caret {

    class SurfaceFile;
    class XmlWriter;
    
    class SurfaceProjection : public CaretObjectTracksModification {
        
    public:
        SurfaceProjection();
        
        virtual ~SurfaceProjection();
        
        SurfaceProjection(const SurfaceProjection& obj);

        SurfaceProjection& operator=(const SurfaceProjection& obj);
        
        /**
         * @return Is the projection valid?
         */
        virtual bool isValid() = 0;
        
        /**
         * Set the validity of the projection.
         * @param valid
         *    New validity status.
         */
        virtual void setValid(const bool valid) = 0;
        
        /**
         * Reset the surface projection to its initial state.
         */
        virtual void reset() = 0;
        
        /**
         * Unproject to the surface using 'this' projection.
         * 
         * @param surfaceFile
         *    Surface file used for unprojecting.
         * @param xyzOut
         *    Output containing coordinate created by unprojecting.
         * @param isUnprojectedOntoSurface
         *    If true, ouput coordinate will be directly on the surface.
         * @return
         *    True if unprojection is successful, else false.
         */
        virtual bool unprojectToSurface(const SurfaceFile& surfaceFile,
                                        float xyzOut[3],
                                        const bool isUnprojectedOntoSurface = false) const = 0;
        
        int32_t getProjectionSurfaceNumberOfNodes() const;
        
        void setProjectionSurfaceNumberOfNodes(const int surfaceNumberOfNodes);

        /**
         * Write the projection to XML.
         * @param xmlWriter
         *   The XML Writer.
         * @throw XmlException
         *   If an error occurs.
         */
        virtual void writeAsXML(XmlWriter& xmlWriter) throw (XmlException) = 0;
        
    protected:
        /** Number of nodes in surface to which item is projected. */
        int32_t projectionSurfaceNumberOfNodes;
        
    private:
        void copyHelperSurfaceProjection(const SurfaceProjection& obj);
        
    };
    
#ifdef __SURFACE_PROJECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_PROJECTION_DECLARE__

} // namespace
#endif  //__SURFACE_PROJECTION__H_
