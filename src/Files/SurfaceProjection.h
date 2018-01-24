#ifndef __SURFACE_PROJECTION__H_
#define __SURFACE_PROJECTION__H_

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
#include "XmlException.h"

namespace caret {

    class SurfaceFile;
    class TopologyHelper;
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
        virtual bool isValid() const = 0;
        
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
         * @param offsetFromSurface
         *    If 'unprojectWithOffsetFromSurface' is true, unprojected
         *    position will be this distance above (negative=below)
         *    the surface.
         * @param unprojectWithOffsetFromSurface
         *    If true, ouput coordinate will be offset 'offsetFromSurface' 
         *    distance from the surface.
         * @return
         *    True if unprojection is successful, else false.
         */
        virtual bool unprojectToSurface(const SurfaceFile& surfaceFile,
                                        float xyzOut[3],
                                        const float offsetFromSurface,
                                        const bool unprojectWithOffsetFromSurface) const = 0;
        
        /**
         * Unproject to the surface using 'this' projection.
         *
         * @param surfaceFile
         *    Surface file used for unprojecting.
         * @param topologyHelperIn
         *    Topology helper.  If NULL, topology helper from surfaceFile
         *    will be used but frequent calls to get the topology helper
         *    may be slow.
         * @param xyzOut
         *    Output containing coordinate created by unprojecting.
         * @param offsetFromSurface
         *    If 'unprojectWithOffsetFromSurface' is true, unprojected
         *    position will be this distance above (negative=below)
         *    the surface.
         * @param unprojectWithOffsetFromSurface
         *    If true, ouput coordinate will be offset 'offsetFromSurface'
         *    distance from the surface.
         * @return
         *    True if unprojection is successful, else false.
         */
        virtual bool unprojectToSurface(const SurfaceFile& surfaceFile,
                                        const TopologyHelper* topologyHelperIn,
                                        float xyzOut[3],
                                        const float offsetFromSurface,
                                        const bool unprojectWithOffsetFromSurface) const = 0;
        
        int32_t getProjectionSurfaceNumberOfNodes() const;
        
        void setProjectionSurfaceNumberOfNodes(const int surfaceNumberOfNodes);

        /**
         * Write the projection to XML.
         * @param xmlWriter
         *   The XML Writer.
         * @throw XmlException
         *   If an error occurs.
         */
        virtual void writeAsXML(XmlWriter& xmlWriter) = 0;
        
        /* @return a string describing the projection */
        virtual AString toString() const;
        
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
