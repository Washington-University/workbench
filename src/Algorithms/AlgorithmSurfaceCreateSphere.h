#ifndef __ALGORITHM_SURFACE_CREATE_SPHERE_H__
#define __ALGORITHM_SURFACE_CREATE_SPHERE_H__

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

#include "AbstractAlgorithm.h"

#include <vector>

namespace caret {
    
    class AlgorithmSurfaceCreateSphere : public AbstractAlgorithm
    {
        AlgorithmSurfaceCreateSphere();
        void interp3(const float coord1[3], const float coord2[3], const float coord3[3], const int& row, const int& col, float out[3]);
        void getEdge(int node1, int node2, int* out);
        int m_numDivisions, m_curNodes, m_curTiles;
        SurfaceFile* m_surface;
        std::vector<std::vector<std::vector<int> > > m_edgenodes;
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        void getNumberOfNodesAndTrianglesFromDivisions(const int& divisions, int& numNodesOut, int& numTrianglesOut);
        AlgorithmSurfaceCreateSphere(ProgressObject* myProgObj, const int& numVertices, SurfaceFile* mySurfOut);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmSurfaceCreateSphere> AutoAlgorithmSurfaceCreateSphere;

}

#endif //__ALGORITHM_SURFACE_CREATE_SPHERE_H__
