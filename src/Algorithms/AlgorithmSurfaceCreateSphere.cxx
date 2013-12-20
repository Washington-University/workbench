/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AlgorithmSurfaceCreateSphere.h"
#include "AlgorithmException.h"

#include "CaretLogger.h"
#include "SurfaceFile.h"
#include "Vector3D.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmSurfaceCreateSphere::getCommandSwitch()
{
    return "-surface-create-sphere";
}

AString AlgorithmSurfaceCreateSphere::getShortDescription()
{
    return "GENERATE A SPHERE WITH CONSISTENT VERTEX AREAS";
}

OperationParameters* AlgorithmSurfaceCreateSphere::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addIntegerParameter(1, "num-vertices", "desired number of vertices");
    
    ret->addSurfaceOutputParameter(2, "sphere-out", "the output sphere");
    
    ret->setHelpText(
        AString("Generates a sphere by regularly dividing the triangles of an icosahedron, to come as close to the desired number of vertices as possible, ") +
        "and modifying it to have very similar vertex areas for all vertices.  " +
        "To generate a pair of vertex-matched left and right spheres, use this command, then -surface-flip-lr to generate the other sphere, then -set-structure on each.  " +
        "For example:\n\n" +
        "$ wb_command -surface-create-sphere 6000 Sphere.6k.R.surf.gii\n" + 
        "$ wb_command -surface-flip-lr Sphere.6k.R.surf.gii Sphere.6k.L.surf.gii\n" +
        "$ wb_command -set-structure Sphere.6k.R.surf.gii CORTEX_RIGHT\n" +
        "$ wb_command -set-structure Sphere.6k.L.surf.gii CORTEX_LEFT"
    );
    return ret;
}

void AlgorithmSurfaceCreateSphere::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    int numVertices = (int)myParams->getInteger(1);
    SurfaceFile* mySurfOut = myParams->getOutputSurface(2);
    AlgorithmSurfaceCreateSphere(myProgObj, numVertices, mySurfOut);
}

AlgorithmSurfaceCreateSphere::AlgorithmSurfaceCreateSphere(ProgressObject* myProgObj, const int& numVertices, SurfaceFile* mySurfOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (numVertices < 1) throw AlgorithmException("desired number of vertices must be positive");
    int lastnodes, mynodes, mytris;
    m_numDivisions = -1;
    getNumberOfNodesAndTrianglesFromDivisions(0, lastnodes, mytris);
    for (int i = 1; i < 10000; ++i)//10363 divisions overflows an int32 in number of triangles, tighten this sanity check?
    {
        getNumberOfNodesAndTrianglesFromDivisions(i, mynodes, mytris);
        if (abs(numVertices - lastnodes) < abs(numVertices - mynodes))
        {
            m_numDivisions = i - 1;
            CaretLogFine("Closest divided icosahedron has " + AString::number(lastnodes) + " nodes.");
            break;
        }
        lastnodes = mynodes;
    }
    if (m_numDivisions == -1) throw AlgorithmException("too many vertices specified");
    m_surface = mySurfOut;
    getNumberOfNodesAndTrianglesFromDivisions(m_numDivisions, mynodes, mytris);
    m_surface->setNumberOfNodesAndTriangles(mynodes, mytris);
    m_surface->setSurfaceType(SurfaceTypeEnum::SPHERICAL);
    m_surface->setSecondaryType(SecondarySurfaceTypeEnum::INVALID);
    SurfaceFile initSurf;
    initSurf.setNumberOfNodesAndTriangles(12, 20);
    const double phi = (1.0 + sqrt(5.0)) / 2.0;
    initSurf.setCoordinate(0, -phi,  0.0,  1.0);//initial icosahedron
    initSurf.setCoordinate(1,    0, -1.0,  phi);
    initSurf.setCoordinate(2,  phi,  0.0,  1.0);
    initSurf.setCoordinate(3,    0,  1.0,  phi);

    initSurf.setCoordinate(4, -1.0, -phi, 0.0);
    initSurf.setCoordinate(5,  1.0, -phi, 0.0);
    initSurf.setCoordinate(6,  1.0,  phi, 0.0);
    initSurf.setCoordinate(7, -1.0,  phi, 0.0);

    initSurf.setCoordinate(8, -phi,  0.0, -1.0);
    initSurf.setCoordinate(9,    0, -1.0, -phi);
    initSurf.setCoordinate(10, phi,  0.0, -1.0);
    initSurf.setCoordinate(11,   0,  1.0, -phi);

    initSurf.setTriangle(0, 0, 7, 8);
    initSurf.setTriangle(1, 0, 3, 7);
    initSurf.setTriangle(2, 0, 1, 3);
    initSurf.setTriangle(3, 1, 2, 3);
    initSurf.setTriangle(4, 1, 5, 2);
    initSurf.setTriangle(5, 5, 9, 10);
    initSurf.setTriangle(6, 10, 11, 6);
    initSurf.setTriangle(7, 4, 1, 0);
    initSurf.setTriangle(8, 4, 5, 1);
    initSurf.setTriangle(9, 4, 9, 5);
    initSurf.setTriangle(10, 5, 10, 2);
    initSurf.setTriangle(11, 10, 6, 2);
    initSurf.setTriangle(12, 6, 11, 7);
    initSurf.setTriangle(13, 11, 9, 8);
    initSurf.setTriangle(14, 9, 4, 8);
    initSurf.setTriangle(15, 8, 4, 0);
    initSurf.setTriangle(16, 11, 8, 7);
    initSurf.setTriangle(17, 6, 7, 3);
    initSurf.setTriangle(18, 6, 3, 2);
    initSurf.setTriangle(19, 11, 10, 9);
    
    Vector3D tempcoord;
    for (int i = 0; i < 12; ++i)
    {
        tempcoord = initSurf.getCoordinate(i);
        tempcoord *= 100.0f / tempcoord.length();//make it directly into a radius 100 sphere rather than taking a second pass
        m_surface->setCoordinate(i, tempcoord);
    }
    
    m_curTiles = 0;
    m_curNodes = 12;
    m_edgenodes.resize(11);//stores by low node #
    vector<int> edge1(m_numDivisions + 1), edge2(m_numDivisions + 1), edge3(m_numDivisions + 1);
    vector<vector<int> > facenodes(m_numDivisions + 1);
    for (int i = 0; i <= m_numDivisions; ++i)
    {// first index is row from bottom, second index is column from left
        facenodes[i].resize(m_numDivisions - i + 1);
    }
    for (int i = 0; i < 20; ++i)
    {
        const int32_t* nodes = initSurf.getTriangle(i);
        //convention for visualization: nodes[0] is bottom left, nodes[1] is bottom right, nodes[2] is top
        //tile generation also follows this convention, with the consequence that tile orientation is preserved
        getEdge(nodes[0], nodes[1], edge1.data());//bottom edge, left to right
        getEdge(nodes[0], nodes[2], edge2.data());//left edge, bottom to top
        getEdge(nodes[1], nodes[2], edge3.data());//right edge, bottom to top
        const float* coord1 = initSurf.getCoordinate(nodes[0]);
        const float* coord2 = initSurf.getCoordinate(nodes[1]);
        const float* coord3 = initSurf.getCoordinate(nodes[2]);
        for (int j = 0; j <= m_numDivisions; ++j)
        {//copy edge nodes into the face array
            facenodes[0][j] = edge1[j];
            facenodes[j][0] = edge2[j];
            facenodes[j][m_numDivisions - j] = edge3[j];
        }
        for (int j = 1; j < m_numDivisions - 1; ++j)
        {//generate interior coordinates
            int intCols = m_numDivisions - j;
            for (int k = 1; k < intCols; ++k)
            {
                interp3(coord1, coord2, coord3, j, k, tempcoord);
                tempcoord *= 100.0f / tempcoord.length();//make it directly into a radius 100 sphere rather than taking a second pass
                m_surface->setCoordinate(m_curNodes, tempcoord);
                facenodes[j][k] = m_curNodes;
                ++m_curNodes;
            }
        }
        for (int j = 0; j < m_numDivisions; ++j)
        {//generate tiles
            for (int k = 0; k < m_numDivisions - j - 1; ++k)
            {//pairs for trapezoidal pieces
                m_surface->setTriangle(m_curTiles, facenodes[j][k], facenodes[j][k + 1], facenodes[j + 1][k]);
                m_surface->setTriangle(m_curTiles + 1, facenodes[j + 1][k], facenodes[j][k + 1], facenodes[j + 1][k + 1]);
                m_curTiles += 2;
            }//and one more
            m_surface->setTriangle(m_curTiles, facenodes[j][m_numDivisions - j - 1], facenodes[j][m_numDivisions - j], facenodes[j + 1][m_numDivisions - j - 1]);
            ++m_curTiles;
        }
    }
}

void AlgorithmSurfaceCreateSphere::getNumberOfNodesAndTrianglesFromDivisions(const int& divisions, int& numNodesOut, int& numTrianglesOut)
{
    int div2 = divisions * divisions;
    numNodesOut = 2 + 10 * div2;
    numTrianglesOut = 20 * div2;//yes, its really that simple, the sum of the two triangulars is square
}

void AlgorithmSurfaceCreateSphere::interp3(const float coord1[3], const float coord2[3], const float coord3[3], const int& row, const int& col, float out[3])
{//this is the function to change if you want different spacing
    float weight2 = ((float)col) / m_numDivisions;//start with flat interpolation weights
    float weight3 = ((float)row) / m_numDivisions;
    float weight1 = 1.0f - weight2 - weight3;
    //polynomial for each weight - should map 0 to 0 and 1 to 1
    const float quintweight = 0.0537206f;//WEIGHTS TUNED FOR ICOSAHEDRON VIA GENETIC ALGORITHM, ADJUST FOR OTHER INITIAL POLYGONS
    const float quartweight = -0.174466f;//this polynomial should be highly dependent on size of the triangle being interpolated
    const float cubeweight = 0.292547f;
    const float quadweight = -0.456351f;
    const float linweight = 1.0f - quintweight - quartweight - cubeweight - quadweight;//make sure it maps 0 to 0 and 1 to 1
    weight1 = ((((quintweight * weight1 + quartweight) * weight1 + cubeweight) * weight1 + quadweight) * weight1 + linweight) * weight1;//quintic approximation of great arc equal area weight transformation function
    weight2 = ((((quintweight * weight2 + quartweight) * weight2 + cubeweight) * weight2 + quadweight) * weight2 + linweight) * weight2;
    weight3 = ((((quintweight * weight3 + quartweight) * weight3 + cubeweight) * weight3 + quadweight) * weight3 + linweight) * weight3;//three weights no longer sum to 1, but thats ok
    out[0] = coord1[0] * weight1 + coord2[0] * weight2 + coord3[0] * weight3;
    out[1] = coord1[1] * weight1 + coord2[1] * weight2 + coord3[1] * weight3;
    out[2] = coord1[2] * weight1 + coord2[2] * weight2 + coord3[2] * weight3;
}

void AlgorithmSurfaceCreateSphere::getEdge(int node1, int node2, int* out)
{
    bool reverse = false;
    int i, index, edgesize = m_numDivisions + 1;
    if (node1 > node2)
    {
        reverse = true;
        i = node1;
        node1 = node2;
        node2 = i;
    }
    bool found = false;
    for (i = 0; i < (int)m_edgenodes[node1].size(); ++i)
    {
        if (m_edgenodes[node1][i][m_numDivisions] == node2)
        {
            found = true;
            index = i;
            break;
        }
    }
    if (!found)
    {
        float coord3[3] = {0.0f, 0.0f, 0.0f};
        Vector3D tempcoord;
        const float* coord1, *coord2;
        coord1 = m_surface->getCoordinate(node1);
        coord2 = m_surface->getCoordinate(node2);
        std::vector<int> tempvec;
        tempvec.resize(edgesize);
        tempvec[0] = node1;
        tempvec[m_numDivisions] = node2;
        for (i = 1; i < m_numDivisions; ++i)
        {
            interp3(coord1, coord2, coord3, 0, i, tempcoord);//use 0 as dummy node, with row 0 it is unused
            tempvec[i] = m_curNodes;
            tempcoord *= 100.0f / tempcoord.length();
            m_surface->setCoordinate(m_curNodes, tempcoord);
            ++m_curNodes;
        }
        index = m_edgenodes[node1].size();
        m_edgenodes[node1].push_back(tempvec);
    }
    if (reverse)
    {
        for (i = 0; i < edgesize; ++i)
        {
            out[i] = m_edgenodes[node1][index][edgesize - i - 1];
        }
    } else {
        for (i = 0; i < edgesize; ++i)
        {
            out[i] = m_edgenodes[node1][index][i];
        }
    }
}

float AlgorithmSurfaceCreateSphere::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceCreateSphere::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
