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
    return "COPY REGISTRATION DEFORMATIONS TO DIFFERENT SPHERE";
}

OperationParameters* AlgorithmSurfaceSphereProjectUnproject::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "sphere-in", "a sphere with the desired output mesh, to apply the deformations to");
    
    ret->addSurfaceParameter(2, "sphere-project-to", "<sphere-unproject-from>, but 'before' the deformation that is desired, i.e., is aligned with <sphere-in>");
    
    ret->addSurfaceParameter(3, "sphere-unproject-from", "<sphere-project-to>, deformed to the desired output registration");
    
    ret->addSurfaceOutputParameter(4, "sphere-out", "<sphere-in> after the deformations are applied");
    
    ret->setHelpText(
        AString("Background: A surface registration starts with an input sphere and data, and moves the vertices around on the sphere until the new spherical location of the data matches the template data ") +
        "(side note: this does not deform the anatomical position of any data, nor does surface-based resampling, as long as the same pair of spheres is used to resample both the surface and metric files).  " +
        "This means that the spherical deformations produced by the registration are actually represented as the difference between two separate files - the starting sphere, and the registered sphere.  " +
        "Since the starting sphere of the registration may not have vertex correspondence to any other sphere (often, it is a native-mesh sphere), it can be inconvenient to manipulate or compare these spherical deformations across subjects, etc.\n\n" +
        
        "The purpose of this command is to be able to apply these deformations onto a new sphere of the user's choice, to make it easier to compare or manipulate them.  " +
        "Common uses are to concatenate two successive separate registrations (e.g. Human to Chimpanzee, and then Chimpanzee to Macaque) or inversion (for dedrifting or symmetric registration schemes).\n\n" +
        
        "<sphere-in> must already be considered to be in alignment with one of the two ends of the registration (if your registration is Human to Chimpanzee, <sphere-in> must be in register with either Human or Chimpanzee).  " +
        "The 'project-to' sphere must be the side of the registration that is aligned with <sphere-in> (if your registration is Human to Chimpanzee, and <sphere-in> is aligned with Human, then 'project-to' should be the original Human sphere).  " +
        "The 'unproject-from' sphere must be the remaining sphere of the registration pair (original vs deformed/registered).  " +
        "The output is as if you had run the same registration with <sphere-in> as the starting sphere, in the direction of deforming the 'project-to' sphere to create the 'unproject-from' sphere.\n\n" +
        
        "Note that this command cannot check for you what spheres are aligned with other spheres, and using the wrong spheres or in the incorrect order will not necessarily cause an error message.  " +
        "In some cases, it may be useful to use a new, arbitrary sphere as the input, which can be created with the -surface-create-sphere command.\n\n" +
        
        "Example 1: You have a Human to Chimpanzee registration, and a Chimpanzee to Macaque registration, and want to combine them.  " +
        "If you use the Human sphere registered to Chimpanzee as sphere-in, the Chimpanzee standard sphere as project-to, and " +
        "the Chimpanzee sphere registered to Macaque as unproject-from, the output will be the Human sphere in register with the Macaque.\n\n" +
        
        "Example 2: You have a Human to Chimpanzee registration, but what you really want is the inverse, that is, the sphere as if you had run the registration from Chimpanzee to Human.  " +
        "If you use the Chimpanzee standard sphere as sphere-in, the Human sphere registered to Chimpanzee as project-to, and the standard Human sphere as unproject-from, " +
        "the output will be the Chimpanzee sphere in register with the Human.\n\n" +
        
        "Technical details: Each vertex of <sphere-in> is projected to a triangle of <sphere-project-to>, and its new position is determined by the position of the corresponding triangle in <sphere-unproject-from>.  " +
        "The output is a sphere with the topology of <sphere-in>, but coordinates shifted by the deformation from <sphere-project-to> to <sphere-unproject-from>.  " +
        "<sphere-project-to> and <sphere-unproject-from> must have the same topology as each other, but <sphere-in> may have any topology."
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
