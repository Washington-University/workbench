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

#include "AlgorithmSurfaceSphereProjectUnproject.h"
#include "AlgorithmException.h"

#include "CaretOMP.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceSphereProjectUnproject::getCommandSwitch()
{
    return "-surface-sphere-project-unproject";
}

AString AlgorithmSurfaceSphereProjectUnproject::getShortDescription()
{
    return "DEFORM A SPHERE ACCORDING TO A REGISTRATION";
}

OperationParameters* AlgorithmSurfaceSphereProjectUnproject::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "sphere-in", "the sphere with the desired output mesh");
    
    ret->addSurfaceParameter(2, "sphere-project-to", "a sphere that aligns with sphere-in");
    
    ret->addSurfaceParameter(3, "sphere-unproject-from", "sphere-project-to deformed to the output space");
    
    ret->addSurfaceOutputParameter(4, "sphere-out", "the output sphere");
    
    ret->setHelpText(
        AString("Each vertex of <sphere-in> is projected to <sphere-project-to> to obtain barycentric weights, which are then used to unproject ") +
        "from <sphere-unproject-from>.  This results in a sphere with the topology of <sphere-in>, but coordinates shifted by the deformation between " +
        "<sphere-project-to> and <sphere-unproject-from>.  <sphere-project-to> and <sphere-unproject-from> must have the same topology as each other, " +
        "but <sphere-in> may have different topology."
    );
    return ret;
}

void AlgorithmSurfaceSphereProjectUnproject::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* sphereIn = myParams->getSurface(1);
    SurfaceFile* projectSphere = myParams->getSurface(2);
    SurfaceFile* unprojectSphere = myParams->getSurface(3);
    SurfaceFile* sphereOut = myParams->getOutputSurface(4);
    AlgorithmSurfaceSphereProjectUnproject(myProgObj, sphereIn, projectSphere, unprojectSphere, sphereOut);//executes the algorithm
}

AlgorithmSurfaceSphereProjectUnproject::AlgorithmSurfaceSphereProjectUnproject(ProgressObject* myProgObj, const SurfaceFile* sphereIn, const SurfaceFile* projectSphere,
                                                                               const SurfaceFile* unprojectSphere, SurfaceFile* sphereOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (!projectSphere->hasNodeCorrespondence(*unprojectSphere)) throw AlgorithmException("projection sphere and unprojection sphere do not have vertex correspondence");
    if (!checkSphere(sphereIn) || !checkSphere(projectSphere) || !checkSphere(unprojectSphere)) throw AlgorithmException("all inputs must be spheres centered around the origin");
    SurfaceFile inMod = *sphereIn, projectMod = *projectSphere, unprojectMod = *unprojectSphere;
    changeRadius(100.0f, &inMod);
    changeRadius(100.0f, &projectMod);
    changeRadius(100.0f, &unprojectMod);
    const float* unprojectCoords = unprojectMod.getCoordinateData();
    const float* inCoords = inMod.getCoordinateData();
    int numNodes = sphereIn->getNumberOfNodes();
    vector<float> outCoords(numNodes * 3);
    *sphereOut = *sphereIn;
    sphereOut->setStructure(unprojectSphere->getStructure());
    CaretPointer<SignedDistanceHelper> myHelper = projectMod.getSignedDistanceHelper();
    for (int i = 0; i < numNodes; ++i)
    {
        int i3 = i * 3;
        BarycentricInfo myInfo;
        myHelper->barycentricWeights(inCoords + i3, myInfo);
        Vector3D outCoord = myInfo.baryWeights[0] * Vector3D(unprojectCoords + myInfo.nodes[0] * 3) +
                            myInfo.baryWeights[1] * Vector3D(unprojectCoords + myInfo.nodes[1] * 3) +
                            myInfo.baryWeights[2] * Vector3D(unprojectCoords + myInfo.nodes[2] * 3);
        outCoords[i3] = outCoord[0];
        outCoords[i3 + 1] = outCoord[1];
        outCoords[i3 + 2] = outCoord[2];
    }
    sphereOut->setCoordinates(outCoords.data());
    changeRadius(100.0f, sphereOut);
}

bool AlgorithmSurfaceSphereProjectUnproject::checkSphere(const SurfaceFile* surface)
{
    int numNodes = surface->getNumberOfNodes();
    CaretAssert(numNodes > 1);
    int numNodes3 = numNodes * 3;
    const float* coordData = surface->getCoordinateData();
    float mindist = Vector3D(coordData).length();
    if (mindist != mindist) throw CaretException("found NaN coordinate in an input sphere");
    float maxdist = mindist;
    const float TOLERANCE = 1.001f;
    for (int i = 3; i < numNodes3; i += 3)
    {
        float tempf = Vector3D(coordData + i).length();
        if (tempf != tempf) throw CaretException("found NaN coordinate in an input sphere");
        if (tempf < mindist)
        {
            mindist = tempf;
        }
        if (tempf > maxdist)
        {
            maxdist = tempf;
        }
    }
    return (mindist * TOLERANCE > maxdist);
}

void AlgorithmSurfaceSphereProjectUnproject::changeRadius(const float& radius, SurfaceFile* inout)
{
    int numNodes = inout->getNumberOfNodes();
    int numNodes3 = numNodes * 3;
    vector<float> newCoordData(numNodes3);
    const float* oldCoordData = inout->getCoordinateData();
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int i = 0; i < numNodes3; i += 3)
    {
        Vector3D tempvec = Vector3D(oldCoordData + i).normal() * radius;
        newCoordData[i] = tempvec[0];
        newCoordData[i + 1] = tempvec[1];
        newCoordData[i + 2] = tempvec[2];
    }
    inout->setCoordinates(newCoordData.data());
}

float AlgorithmSurfaceSphereProjectUnproject::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceSphereProjectUnproject::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
