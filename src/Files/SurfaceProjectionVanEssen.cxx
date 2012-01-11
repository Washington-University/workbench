
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

#include <algorithm>
#include <cmath>

#define __SURFACE_PROJECTION_VAN_ESSEN_DECLARE__
#include "SurfaceProjectionVanEssen.h"
#undef __SURFACE_PROJECTION_VAN_ESSEN_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"

using namespace caret;


    
/**
 * \class SurfaceProjectionVanEssen 
 * \brief Maintains a VanEssen Projection that projects to an edge with offset.
 *
 */

/**
 * Constructor.
 */
SurfaceProjectionVanEssen::SurfaceProjectionVanEssen()
: SurfaceProjection()
{
    this->resetAllValues();
}

/**
 * Destructor.
 */
SurfaceProjectionVanEssen::~SurfaceProjectionVanEssen()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SurfaceProjectionVanEssen::SurfaceProjectionVanEssen(const SurfaceProjectionVanEssen& obj)
: SurfaceProjection(obj)
{
    this->copyHelperSurfaceProjectionVanEssen(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SurfaceProjectionVanEssen&
SurfaceProjectionVanEssen::operator=(const SurfaceProjectionVanEssen& obj)
{
    if (this != &obj) {
        SurfaceProjection::operator=(obj);
        this->copyHelperSurfaceProjectionVanEssen(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SurfaceProjectionVanEssen::copyHelperSurfaceProjectionVanEssen(const SurfaceProjectionVanEssen& obj)
{
    this->dR = obj.dR;
    
    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < 3; j++) {
            for (int32_t k = 0; k < 3; k++) {
                this->triAnatomical[i][j][k] = obj.triAnatomical[i][j][k];
            }
            this->triVertices[i][j] = obj.triVertices[i][j];
        }
        this->vertex[i] = obj.vertex[i];
    }
    
    this->vertexAnatomical[0][0] = this->vertexAnatomical[0][0];
    this->vertexAnatomical[0][1] = this->vertexAnatomical[0][1];
    this->vertexAnatomical[1][0] = this->vertexAnatomical[1][0];
    this->vertexAnatomical[1][1] = this->vertexAnatomical[1][1];
    
    this->posAnatomical[0] = this->posAnatomical[0];
    this->posAnatomical[1] = this->posAnatomical[1];
    this->posAnatomical[2] = this->posAnatomical[2];
    
    this->thetaR = obj.thetaR;
    
    this->phiR = obj.phiR;
    
    this->fracRI = obj.fracRI;
    
    this->fracRJ = obj.fracRJ;    
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
SurfaceProjectionVanEssen::unprojectToSurface(const SurfaceFile& surfaceFile,
                                                 float xyzOut[3],
                                                 const bool isUnprojectedOntoSurface) const
{
    const int is = 0;
    const int js = 1;
    
    const int32_t n1 = this->vertex[is];
    const int32_t n2 = this->vertex[js];
    
    CaretAssert(n1 < surfaceFile.getNumberOfNodes());
    CaretAssert(n2 < surfaceFile.getNumberOfNodes());
    
    /*
     * All nodes MUST have neighbors (connected)
     */
    const TopologyHelper* topologyHelper = surfaceFile.getTopologyHelper().getPointer();
    if ((topologyHelper->getNodeHasNeighbors(n1) == false) 
        || (topologyHelper->getNodeHasNeighbors(n2) == false)) {
        return false;
    }
    
    float v[3];
    float v_t1[3];
    MathFunctions::subtractVectors(this->vertexAnatomical[js], this->vertexAnatomical[is], v);
    MathFunctions::subtractVectors(this->posAnatomical, this->posAnatomical, v_t1);
    
    float s_t2 = MathFunctions::dotProduct(v, v);
    float s_t3 = MathFunctions::dotProduct(v_t1, v);
    float QR[3];
    for (int j = 0; j < 3; j++) {
        QR[j] = this->vertexAnatomical[is][j] + ((s_t3/s_t2) * v[j]);
    }
    
    const int pis = this->vertex[0];
    const int pjs = this->vertex[1];
    
    const float* posPIS = surfaceFile.getCoordinate(pis);
    const float* posPJS = surfaceFile.getCoordinate(pjs);
    
    if (isUnprojectedOntoSurface) {
        xyzOut[0] = (posPIS[0] + posPJS[0]) / 2.0f;
        xyzOut[1] = (posPIS[1] + posPJS[1]) / 2.0f;
        xyzOut[2] = (posPIS[2] + posPJS[2]) / 2.0f;
        return true;
    }
    
    
    MathFunctions::subtractVectors(posPJS, posPIS, v);
    
    float QS[3];
    
    if ((this->fracRI <= 1.0) && (this->fracRJ <= 1.0)) {
        for (int j = 0; j < 3; j++) {
            QS[j] = posPIS[j] + this->fracRI * v[j];
        }
    }
    else if ((this->fracRI > 1.0) && (this->fracRI > this->fracRJ)) {
        MathFunctions::subtractVectors(QR, this->vertexAnatomical[js], v_t1);
        s_t2 = MathFunctions::vectorLength(v_t1);
        
        MathFunctions::subtractVectors(posPJS, posPIS, v);
        s_t3 = MathFunctions::vectorLength(v);
        for (int j = 0; j < 3; j++) {
            QS[j] = posPJS[j] + s_t2 * (v[j]/s_t3);
        }
    }
    else if ((this->fracRJ > 1.0) && (this->fracRJ > this->fracRI)) {
        MathFunctions::subtractVectors(QR, this->vertexAnatomical[is], v_t1);
        s_t2 = MathFunctions::vectorLength(v_t1);
        
        MathFunctions::subtractVectors(posPIS, posPJS, v);
        s_t3 = MathFunctions::vectorLength(v);
        for (int j = 0; j < 3; j++) {
            QS[j] = posPIS[j] + s_t2 * (v[j]/s_t3);
        }
    }
    else {
        CaretLogWarning(("VanEssen Projection: Unrecognized case for fracRI and fracRJ: "
                         + AString::number(this->fracRI)
                         + ", "
                         + AString::number(this->fracRJ)));
        return false;
    }
    
    if ((this->triVertices[0][0] < 0) || (this->triVertices[1][0] < 0)) {
        return false;
    }
    
        
    float normalB[3];
    MathFunctions::normalVector(
                                surfaceFile.getCoordinate(this->triVertices[1][0]),
                                surfaceFile.getCoordinate(this->triVertices[1][1]),
                                surfaceFile.getCoordinate(this->triVertices[1][2]),
                                normalB);
    
    float normalA[3]; 
    MathFunctions::normalVector(
                                surfaceFile.getCoordinate(this->triVertices[0][0]),
                                surfaceFile.getCoordinate(this->triVertices[0][1]),
                                surfaceFile.getCoordinate(this->triVertices[0][2]),
                                normalA);
    
    s_t2 = MathFunctions::dotProduct(normalA, normalB);
    s_t2 = std::min(s_t2, 1.0f); // limit to <= 1.0
    float phiS = (float)std::acos(s_t2);
    
    float thetaS = 0.0f;
    if (this->phiR > 0.0f) {
        thetaS = (this->thetaR / this->phiR) * phiS;
    }
    else {
        thetaS = 0.5f * phiS;
    }
    
    MathFunctions::subtractVectors(posPJS, posPIS, v);
    MathFunctions::normalizeVector(v);
    
    float projection[3] = { 0.0f, 0.0f, 0.0f };
    this->computeProjectionPoint(projection);
    
    MathFunctions::subtractVectors(projection, QR, v_t1);
    MathFunctions::normalizeVector(v_t1);
    
    MathFunctions::subtractVectors(this->vertexAnatomical[js], this->vertexAnatomical[is], v);
    MathFunctions::normalizeVector(v);
    
    float normalA_3D[3];
    MathFunctions::normalVector(this->triAnatomical[0][0],
                                this->triAnatomical[0][1],
                                this->triAnatomical[0][2],
                                normalA_3D);
    float v_t2[3];
    MathFunctions::crossProduct(normalA_3D, v, v_t2);
    s_t3 = MathFunctions::dotProduct(v_t1, v_t2);
    
    float TS[3];
    for (int k = 0; k < 3; k++) {
        TS[k] = QS[k] + (s_t3 * (this->dR * (float)std::sin(thetaS)) * v_t2[k]);
    }
    
    MathFunctions::subtractVectors(this->posAnatomical, projection, v);
    MathFunctions::normalizeVector(v);
    
    s_t3 = MathFunctions::dotProduct(normalA_3D, v);
    
    for (int i = 0; i < 3; i++) {
        xyzOut[i] = TS[i] + (this->dR * s_t3 * (float)std::cos(thetaS)) * normalA[i];
    }
    
    return true;
}

/**
 * Compute a projection point??
 * @param
 *    Projection that is computed and set by this method.
 */
void 
SurfaceProjectionVanEssen::computeProjectionPoint(float projection[3]) const
{
    float v[3];
    MathFunctions::subtractVectors(this->triAnatomical[0][1],
                                              this->triAnatomical[0][0],
                                   v);
    float w[3];
    MathFunctions::subtractVectors(this->triAnatomical[0][1],
                                   this->triAnatomical[0][2],
                                  w);
    float tnormal[3];
    MathFunctions::crossProduct(w, v, tnormal);
    
    float a[3][3];
    for (int k = 0; k < 3; k++) {
        a[0][k] = v[k];
        a[1][k] = w[k];
        a[2][k] = tnormal[k];
    }
    
    float b[3];
    b[0] = MathFunctions::dotProduct(v, this->posAnatomical);
    b[1] = MathFunctions::dotProduct(w, this->posAnatomical);
    b[2] = MathFunctions::dotProduct(tnormal, this->triAnatomical[0][2]);
    
    
    MathFunctions::vtkLinearSolve3x3(a, b, projection);
}

/**
 * @return dR
 */
float 
SurfaceProjectionVanEssen::getDR() const
{
    return this->dR;
}

/**
 * Set dR
 * @param dR
 *    New value.
 */
void 
SurfaceProjectionVanEssen::setDR(const float dR)
{
    this->dR = dR;
    this->setModified();
}

/**
 * @return thetaR
 */
float 
SurfaceProjectionVanEssen::getThetaR() const
{
    return this->thetaR;
}

/**
 * Set thetaR
 * @param thetaR
 *    New value.
 */
void 
SurfaceProjectionVanEssen::setThetaR(const float thetaR)
{
    this->thetaR = thetaR;
    this->setModified();
}

/**
 * @return phiR
 */
float 
SurfaceProjectionVanEssen::getPhiR() const
{
    return this->phiR;
}

/**
 * Set phiR
 * @param phiR
 *    New value.
 */
void 
SurfaceProjectionVanEssen::setPhiR(const float phiR)
{
    this->phiR = phiR;
    this->setModified();
}

/**
 * @return fracRI
 */
float 
SurfaceProjectionVanEssen::getFracRI() const
{
    return this->fracRI;
}

/**
 * Set fracRI
 * @param fracRI
 *    New value.
 */
void 
SurfaceProjectionVanEssen::setFracRI(const float fracRI)
{
    this->fracRI = fracRI;
    this->setModified();
}

/**
 * @return fracRJ
 */
float 
SurfaceProjectionVanEssen::getFracRJ() const
{
    return this->fracRJ;
}

/**
 * Set fracRJ
 * @param fracRJ
 *    New value.
 */
void 
SurfaceProjectionVanEssen::setFracRJ(const float fracRJ)
{
    this->fracRJ = fracRJ;
    this->setModified();
}

/**
 * Set triVertices
 * @param triVertices
 *    New values.
 */
void 
SurfaceProjectionVanEssen::setTriVertices(const int32_t triVertices[2][3])
{
    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < 3; j++) {
            this->triVertices[i][j] = triVertices[i][j];
        }
    }
    this->setModified();
}

/**
 * Get triVertices
 * @param triVertices
 *    Output values.
 */
void 
SurfaceProjectionVanEssen::getTriVertices(int32_t triVertices[2][3]) const
{
    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < 3; j++) {
            triVertices[i][j] = this->triVertices[i][j];
        }
    }
}

/**
 * Set vertex
 * @param vertex
 *    New values.
 */
void 
SurfaceProjectionVanEssen::setVertex(const int32_t vertex[2])
{
    this->vertex[0] = vertex[0];
    this->vertex[1] = vertex[1];
    this->setModified();
}

/**
 * Get vertex
 * @param vertex
 *    Output values.
 */
void 
SurfaceProjectionVanEssen::getVertex(int32_t vertex[2]) const
{
    vertex[0] = this->vertex[0];
    vertex[1] = this->vertex[1];
}

/**
 * Set triAnatomical
 * @param triAnatomical
 *    New values.
 */
void 
SurfaceProjectionVanEssen::setTriAnatomical(const float triAnatomical[2][3][3])
{
    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < 3; j++) {
            for (int32_t k = 0; k < 3; k++) {
                this->triAnatomical[i][j][k] = triAnatomical[i][j][k];
            }
        }
    }
    this->setModified();
}

/**
 * Get triAnatomical
 * @param triAnatomical
 *    Output values.
 */
void 
SurfaceProjectionVanEssen::getTriAnatomical(float triAnatomical[2][3][3]) const
{
    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < 3; j++) {
            for (int32_t k = 0; k < 3; k++) {
                triAnatomical[i][j][k] = this->triAnatomical[i][j][k];
            }
        }
    }
}

/**
 * Set vertexAnatomical
 * @param vertexAnatomical
 *    New values.
 */
void 
SurfaceProjectionVanEssen::setVertexAnatomical(const float vertexAnatomical[2][2])
{
    this->vertexAnatomical[0][0] = vertexAnatomical[0][0];
    this->vertexAnatomical[0][1] = vertexAnatomical[0][1];
    this->vertexAnatomical[1][0] = vertexAnatomical[1][0];
    this->vertexAnatomical[1][1] = vertexAnatomical[1][1];
    this->setModified();
}

/**
 * Get vertexAnatomical
 * @param vertexAnatomical
 *    Output values.
 */
void 
SurfaceProjectionVanEssen::getVertexAnatomical(float vertexAnatomical[2][2]) const
{
    vertexAnatomical[0][0] = this->vertexAnatomical[0][0];
    vertexAnatomical[0][1] = this->vertexAnatomical[0][1];
    vertexAnatomical[1][0] = this->vertexAnatomical[1][0];
    vertexAnatomical[1][1] = this->vertexAnatomical[1][1];
}

/**
 * Get posAnatomical
 * @param posAnatomical
 *    Output values.
 */
void 
SurfaceProjectionVanEssen::getPosAnatomical(float posAnatomical[3]) const
{
    posAnatomical[0] = this->posAnatomical[0];
    posAnatomical[1] = this->posAnatomical[1];
    posAnatomical[2] = this->posAnatomical[2];
}

/**
 * Set posAnatomical
 * @param posAnatomical
 *    New values.
 */
void 
SurfaceProjectionVanEssen::setPosAnatomical(const float posAnatomical[3])
{
    this->posAnatomical[0] = posAnatomical[0];
    this->posAnatomical[1] = posAnatomical[1];
    this->posAnatomical[2] = posAnatomical[2];
    this->setModified();
}

/**
 * Reset the surface projection to its initial state.
 */
void 
SurfaceProjectionVanEssen::reset()
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
SurfaceProjectionVanEssen::resetAllValues()
{
    this->dR = 0.0;
    
    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < 3; j++) {
            for (int32_t k = 0; k < 3; k++) {
                this->triAnatomical[i][j][k] = 0;
            }
            this->triVertices[i][j] = 0.0;
        }
        this->vertex[i] = 0.0;
    }
    
    this->vertexAnatomical[0][0] = 0.0;
    this->vertexAnatomical[0][1] = 0.0;
    this->vertexAnatomical[1][0] = 0.0;
    this->vertexAnatomical[1][1] = 0.0;
    
    this->posAnatomical[0] = 0.0;
    this->posAnatomical[1] = 0.0;
    this->posAnatomical[2] = 0.0;
    
    this->thetaR = 0.0;
    
    this->phiR = 0.0;
    
    this->fracRI = 0.0;
    
    this->fracRJ = 0.0;
}



