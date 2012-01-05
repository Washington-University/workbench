
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

#define __SURFACE_PROJECTION_BARYCENTRIC_DECLARE__
#include "SurfaceProjectionBarycentric.h"
#undef __SURFACE_PROJECTION_BARYCENTRIC_DECLARE__

#include "CaretAssert.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"

using namespace caret;


    
/**
 * \class SurfaceProjectionBarycentric 
 * \brief Maintains a barycentric projection.
 *
 */

/**
 * Constructor.
 */
SurfaceProjectionBarycentric::SurfaceProjectionBarycentric()
: SurfaceProjection()
{
    this->triangleAreas[0] = 0.0;
    this->triangleAreas[1] = 0.0;
    this->triangleAreas[2] = 0.0;
    
    this->triangleVertices[0] = 0;
    this->triangleVertices[1] = 0;
    this->triangleVertices[2] = 0;
    
    this->signedDistanceAboveSurface = 0.0;
}

/**
 * Destructor.
 */
SurfaceProjectionBarycentric::~SurfaceProjectionBarycentric()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SurfaceProjectionBarycentric::SurfaceProjectionBarycentric(const SurfaceProjectionBarycentric& obj)
: SurfaceProjection(obj)
{
    this->copyHelperSurfaceProjectionBarycentric(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SurfaceProjectionBarycentric&
SurfaceProjectionBarycentric::operator=(const SurfaceProjectionBarycentric& obj)
{
    if (this != &obj) {
        SurfaceProjection::operator=(obj);
        this->copyHelperSurfaceProjectionBarycentric(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SurfaceProjectionBarycentric::copyHelperSurfaceProjectionBarycentric(const SurfaceProjectionBarycentric& obj)
{
    this->setTriangleAreas(obj.getTriangleAreas());
    this->setTriangleVertices(obj.getTriangleVertices());
    this->signedDistanceAboveSurface = obj.signedDistanceAboveSurface;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceProjectionBarycentric::toString() const
{
    const AString txt = (SurfaceProjection::toString()
                         + "\nSurfaceProjectionBarycentric::triangleAreas=" + AString::fromNumbers(this->triangleAreas, 3, ",")
                         + "triangleVertices=" + AString::fromNumbers(this->triangleVertices, 3, ",")
                         + "signedDistanceAboveSurface=" + AString::number(this->signedDistanceAboveSurface));
    return txt;
}

/**
 * @return The signed distance above the surface.
 */
float 
SurfaceProjectionBarycentric::getSignedDistanceAboveSurface() const
{
    return this->signedDistanceAboveSurface;
}

/**
 * Set the signed distance above the surface.
 * 
 * @param signedDistanceAboveSurface
 *    New value.
 */
void 
SurfaceProjectionBarycentric::setSignedDistanceAboveSurface(const float signedDistanceAboveSurface)
{
    this->signedDistanceAboveSurface = signedDistanceAboveSurface;
    this->setModified();
}

/**
 * @return The triangle vertices (3 elements).
 */
const int32_t* 
SurfaceProjectionBarycentric::getTriangleVertices() const
{
    return this->triangleVertices;
}

/**
 * Set the triangle vertices.
 * 
 * @param triangleVertices
 *    New values for vertices.
 */
void 
SurfaceProjectionBarycentric::setTriangleVertices(const int32_t triangleVertices[3])
{
    this->triangleVertices[0] = triangleVertices[0];
    this->triangleVertices[1] = triangleVertices[1];
    this->triangleVertices[2] = triangleVertices[2];
    this->setModified();
}

/**
 * @return The triangle areas (3 elements).
 */
const float* 
SurfaceProjectionBarycentric::getTriangleAreas() const
{
    return this->triangleAreas;
}

/**
 * Set the triangle areas.
 * 
 * @param triangleAreas
 *    New values for triangle areas.
 */
void 
SurfaceProjectionBarycentric::setTriangleAreas(const float triangleAreas[3])
{
    this->triangleAreas[0] = triangleAreas[0];
    this->triangleAreas[1] = triangleAreas[1];
    this->triangleAreas[2] = triangleAreas[2];
    this->setModified();
}

/**
 * Unproject to the surface using 'this' projection.
 * 
 * @param surfaceFile
 *    Surface file used for unprojecting.
 * @param xyzOut
 *    Output containing coordinate created by unprojecting.
 * @param isUnprojectedOntoSurface
 *    If true, ouput coordinate will be directly on the surface.
 */
bool 
SurfaceProjectionBarycentric::unprojectToSurface(const SurfaceFile& surfaceFile,
                                                 float xyzOut[3],
                                                 const bool isUnprojectedOntoSurface) const
{
    const int32_t n1 = this->triangleVertices[0];
    const int32_t n2 = this->triangleVertices[1];
    const int32_t n3 = this->triangleVertices[2];
    
    CaretAssert(n1 < surfaceFile.getNumberOfNodes());
    CaretAssert(n2 < surfaceFile.getNumberOfNodes());
    CaretAssert(n3 < surfaceFile.getNumberOfNodes());
    
    /*
     * All nodes MUST have neighbors (connected)
     */
    const TopologyHelper* topologyHelper = surfaceFile.getTopologyHelper().getPointer();
    if ((topologyHelper->getNodeHasNeighbors(n1) == false) 
        || (topologyHelper->getNodeHasNeighbors(n2) == false) 
        || (topologyHelper->getNodeHasNeighbors(n3) == false)) {
        return false;
    }
    
    const float* c1 = surfaceFile.getCoordinate(n1);
    const float* c2 = surfaceFile.getCoordinate(n2);
    const float* c3 = surfaceFile.getCoordinate(n3);
    
    float barycentricXYZ[3];
    float barycentricNormal[3];
    
    /*
     * If all the nodes are the same (object projects to a single node, not triangle)
     */
    if ((n1 == n2) &&
        (n2 == n3)) {
        /*
         * Use node's normal vector and position
         */
        barycentricXYZ[0] = c1[0];
        barycentricXYZ[1] = c1[1];
        barycentricXYZ[2] = c1[2];
        const float* nodeNormal = surfaceFile.getNormalVector(n1);
        barycentricNormal[0] = nodeNormal[0];
        barycentricNormal[1] = nodeNormal[1];
        barycentricNormal[2] = nodeNormal[2];
    }
    else {
        /*
         * Compute position using barycentric coordinates
         */
        float t1[3];
        float t2[3];
        float t3[3];
        for (int i = 0; i < 3; i++) {
            t1[i] = triangleAreas[0] * c1[i];
            t2[i] = triangleAreas[1] * c2[i];
            t3[i] = triangleAreas[2] * c3[i];
        }
        float area = (triangleAreas[0] 
                      + triangleAreas[1]
                      + triangleAreas[2]);
        if (area != 0) {
            for (int i = 0; i < 3; i++) {
                barycentricXYZ[i] = (t1[i] + t2[i] + t3[i]) / area;
            }
        }
        else {
            return false;
        }
        
        if (MathFunctions::normalVector(c1, c2, c3, barycentricNormal) == false) {
            return false;
        }
    }
    
    /*
     * Set output coordinate, possibly offsetting from surface.
     */
    for (int j = 0; j < 3; j++) {
        if (isUnprojectedOntoSurface) {
            xyzOut[j] = barycentricXYZ[j];
        }
        else {
            xyzOut[j] = barycentricXYZ[j] 
            + (barycentricNormal[j] * signedDistanceAboveSurface);
        }
    }
    
    return true;
}
