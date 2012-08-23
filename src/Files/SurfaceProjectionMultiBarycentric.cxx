
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

#define __SURFACE_PROJECTION_MULTI_BARYCENTRIC_DECLARE__
#include "SurfaceProjectionMultiBarycentric.h"
#undef __SURFACE_PROJECTION_MULTI_BARYCENTRIC_DECLARE__

#include "CaretAssert.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::SurfaceProjectionMultiBarycentric 
 * \brief Barycentric projection to multiple triangles.
 *
 * When projecting points to a surface, some points do not project into
 * a triangle but instead, project to an edge or a node.  In the case of
 * edge, the point is projected to two triangles but outside the triangles
 * which results in a negative weight (degenerate projection).  In the
 * case of a node, the point is projected to all triangles attached to
 * the node and also results in negative weights (degenerate projections).
 */

/**
 * Constructor.
 */
SurfaceProjectionMultiBarycentric::SurfaceProjectionMultiBarycentric()
: SurfaceProjection()
{
    resetAllValues();
}

/**
 * Destructor.
 */
SurfaceProjectionMultiBarycentric::~SurfaceProjectionMultiBarycentric()
{
    resetAllValues();
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SurfaceProjectionMultiBarycentric::SurfaceProjectionMultiBarycentric(const SurfaceProjectionMultiBarycentric& obj)
: SurfaceProjection(obj)
{
    this->copyHelperSurfaceProjectionMultiBarycentric(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SurfaceProjectionMultiBarycentric&
SurfaceProjectionMultiBarycentric::operator=(const SurfaceProjectionMultiBarycentric& obj)
{
    if (this != &obj) {
        SurfaceProjection::operator=(obj);
        this->copyHelperSurfaceProjectionMultiBarycentric(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SurfaceProjectionMultiBarycentric::copyHelperSurfaceProjectionMultiBarycentric(const SurfaceProjectionMultiBarycentric& obj)
{
    resetAllValues();
    
    const int32_t numProj = obj.getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        addProjection(new SurfaceProjectionBarycentric(*obj.m_projections[i]));
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
SurfaceProjectionMultiBarycentric::toString() const
{
    AString txt = SurfaceProjection::toString();
    if (txt.isEmpty() == false) {
        txt += ", ";
    }
    const int32_t numProj = getNumberOfProjections();
    txt += ("numProj="
            + AString::number(numProj)
            + ", ");
    for (int32_t i = 0; i < numProj; i++) {
        txt += (", Index="
                + AString::number(i)
                + "=>"
                + m_projections[i]->toString());
    }
    
    return txt;
}
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
 *    True if unprojection was successful.
 */
bool
SurfaceProjectionMultiBarycentric::unprojectToSurface(const SurfaceFile& surfaceFile,
                                                 float xyzOut[3],
                                                 const float offsetFromSurface,
                                                 const bool unprojectWithOffsetFromSurface) const
{
    /*
     * Make sure projection surface number of nodes matches surface.
     */
    if (this->projectionSurfaceNumberOfNodes > 0) {
        if (surfaceFile.getNumberOfNodes() != this->projectionSurfaceNumberOfNodes) {
            return false;
        }
    }
    
    double sumXYZ[3] = { 0.0, 0.0, 0.0 };
    double numValid = 0.0;
    
    const int32_t numProj = getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        float xyz[3];
        if (m_projections[i]->unprojectToSurface(surfaceFile,
                                                 xyz,
                                                 offsetFromSurface,
                                                 unprojectWithOffsetFromSurface)) {
            sumXYZ[0] += xyz[0];
            sumXYZ[1] += xyz[1];
            sumXYZ[2] += xyz[2];
            numValid += 1.0;
        }
    }
    
    if (numValid > 0.0) {
        xyzOut[0] = sumXYZ[0] / numValid;
        xyzOut[1] = sumXYZ[1] / numValid;
        xyzOut[2] = sumXYZ[2] / numValid;
    }
    
    return false;
}

/**
 * Reset the surface projection to its initial state.
 */
void
SurfaceProjectionMultiBarycentric::reset()
{
    this->resetAllValues();
}

/**
 * Since reset overrides the 'super' class it should
 * never be called from a constructor.  So, this
 * method does the actual reset, and since it does
 * not override a method from the 'super' class, it
 * may be called from this class' constructor.
 */
void
SurfaceProjectionMultiBarycentric::resetAllValues()
{
    const int32_t numProj = getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        delete m_projections[i];
    }
    m_projections.clear();
}

/**
 * @return Is the projection valid?
 */
bool
SurfaceProjectionMultiBarycentric::isValid() const
{
    const int32_t numProj = getNumberOfProjections();
    for (int32_t i = 0; i < numProj; i++) {
        if (m_projections[i]->isValid()) {
            return true;
        }
    }
    return false;
}

/**
 * Set the validity of the projection.
 * @param valid
 *    New validity status.
 */
void
SurfaceProjectionMultiBarycentric::setValid(const bool /*valid*/)
{
    // do nothing
}

/**
 * @return The number of projections.
 */
int32_t
SurfaceProjectionMultiBarycentric::getNumberOfProjections() const
{
    return m_projections.size();
}

/**
 * Add a projection.
 * @param projection
 *    Projection to be added.
 */
void
SurfaceProjectionMultiBarycentric::addProjection(SurfaceProjectionBarycentric* projection)
{
    CaretAssert(projection);
    m_projections.push_back(projection);
}


/**
 * Write the projection to XML.
 * @param xmlWriter
 *   The XML Writer.
 * @throw XmlException
 *   If an error occurs.
 */
void
SurfaceProjectionMultiBarycentric::writeAsXML(XmlWriter& xmlWriter) throw (XmlException)
{
    if (isValid()) {
//        xmlWriter.writeStartElement(XML_TAG_PROJECTION_BARYCENTRIC);
//        xmlWriter.writeElementCharacters(XML_TAG_TRIANGLE_AREAS, this->triangleAreas, 3);
//        xmlWriter.writeElementCharacters(XML_TAG_TRIANGLE_NODES, this->triangleNodes, 3);
//        xmlWriter.writeElementCharacters(XML_TAG_SIGNED_DISTANCE_ABOVE_SURFACE, this->signedDistanceAboveSurface);
//        xmlWriter.writeEndElement();
    }
}

