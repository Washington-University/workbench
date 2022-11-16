#ifndef __OCT_TREE_H__
#define __OCT_TREE_H__

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

/*This code can be used with the Oxford FMRIB Software Library and/or
 * Multimodal Surface Matching under the MIT license instead of the
 * above GPL license.  The MIT license follows:*/
/*
 *  Copyright (C) Washington University School of Medicine
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "MathFunctions.h"
#include <vector>

namespace caret
{
    ///low level Oct structure with a bunch of helper members, use it to build your own tree of Octs, possibly by extension
    template<typename T>
    struct Oct
    {
        //data
        T m_data;
        //children
        Oct<T>* m_children[2][2][2];
        Oct<T>* m_parent;
        bool m_leaf;
        float m_bounds[3][3];
        
        Oct();
        Oct(const float minCoords[3], const float maxCoords[3]);
        ~Oct();
        void makeChildren();
        void makeChildrenExcept(const int octant[3]);
        void deleteChildren();
        ///makes an Oct with this node as the child specified by octant
        Oct* makeParent(const int octant[3]);
        Oct* makeContains(const float pointToContain[3]);
        float distToPoint(const float point[3]);
        float distSquaredToPoint(const float point[3]);
        bool lineIntersects(const float p1[3], const float p2[3]);
        bool rayIntersects(const float start[3], const float p2[3]);
        bool lineSegmentIntersects(const float start[3], const float end[3]);
        bool pointInside(const float point[3]);
        bool boundsOverlaps(const float minCoords[3], const float maxCoords[3]);
        ///returns which child Oct the point would be contained in if the point were inside this Oct
        Oct* containingChild(const float point[3], int* whichOct = NULL);
    };
    
    ///simple templated vector pointer that can be deleted, since you shouldn't rely on any method for actually deleting a vector's memory, for convenience
    template <typename T>
    struct LeafVector
    {
        std::vector<T>* m_vector;
        LeafVector() { m_vector = new std::vector<T>(); }
        ~LeafVector() { freeData(); }
        void freeData()
        {
            if (m_vector != NULL)
            {
                delete m_vector;
                m_vector = NULL;
            }
        }
        T& operator[](const int64_t index)
        {
            return (*m_vector)[index];
        }
        const T& operator[](const int64_t index) const
        {
            return (*m_vector)[index];
        }
    };
    
    template<typename T>
    Oct<T>::Oct()
    {
        for (int i = 0; i < 2; ++i)
        {
            m_children[i][0][0] = NULL; m_children[i][0][1] = NULL;
            m_children[i][1][0] = NULL; m_children[i][1][1] = NULL;
        }
        m_parent = NULL;
        m_leaf = true;
    }

    template<typename T>
    Oct<T>::Oct(const float minCoords[3], const float maxCoords[3])
    {
        for (int i = 0; i < 2; ++i)
        {
            m_children[i][0][0] = NULL; m_children[i][0][1] = NULL;
            m_children[i][1][0] = NULL; m_children[i][1][1] = NULL;
        }
        m_parent = NULL;
        m_leaf = true;
        for (int i = 0; i < 3; ++i)
        {
            m_bounds[i][0] = minCoords[i];
            m_bounds[i][2] = maxCoords[i];
            m_bounds[i][1] = (m_bounds[i][0] + m_bounds[i][2]) * 0.5f;
        }
    }

    template<typename T>
    Oct<T>::~Oct()
    {
        deleteChildren();
    }
    
    template<typename T>
    void Oct<T>::makeChildren()
    {
        m_leaf = false;
        int ijk[3];
        for (ijk[0] = 0; ijk[0] < 2; ++ijk[0])
        {
            for (ijk[1] = 0; ijk[1] < 2; ++ijk[1])
            {
                for (ijk[2] = 0; ijk[2] < 2; ++ijk[2])
                {
                    Oct<T>* temp = new Oct<T>();
                    m_children[ijk[0]][ijk[1]][ijk[2]] = temp;
                    temp->m_parent = this;
                    for (int m = 0; m < 3; ++m)
                    {
                        temp->m_bounds[m][0] = m_bounds[m][ijk[m]];
                        temp->m_bounds[m][2] = m_bounds[m][ijk[m] + 1];
                        temp->m_bounds[m][1] = (temp->m_bounds[m][0] + temp->m_bounds[m][2]) * 0.5f;
                    }
                }
            }
        }
    }
    
    template<typename T>
    void Oct<T>::makeChildrenExcept(const int octant[3])
    {
        m_leaf = false;
        int ijk[3];
        for (ijk[0] = 0; ijk[0] < 2; ++ijk[0])
        {
            for (ijk[1] = 0; ijk[1] < 2; ++ijk[1])
            {
                for (ijk[2] = 0; ijk[2] < 2; ++ijk[2])
                {
                    if (ijk[0] != octant[0] && ijk[1] != octant[1] && ijk[2] != octant[2])
                    {//avoiding one new/delete pair should be worth 8 times this conditional
                        Oct<T>* temp = new Oct<T>();
                        m_children[ijk[0]][ijk[1]][ijk[2]] = temp;
                        temp->m_parent = this;
                        for (int m = 0; m < 3; ++m)
                        {
                            temp->m_bounds[m][0] = m_bounds[m][ijk[m]];
                            temp->m_bounds[m][2] = m_bounds[m][ijk[m] + 1];
                            temp->m_bounds[m][1] = (temp->m_bounds[m][0] + temp->m_bounds[m][2]) * 0.5f;
                        }
                    }
                }
            }
        }
    }
    
    template<typename T>
    void Oct<T>::deleteChildren()
    {
        m_leaf = true;
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    if (m_children[i][j][k] != NULL)
                    {
                        delete m_children[i][j][k];
                        m_children[i][j][k] = NULL;
                    }
                }
            }
        }
    }
    
    template<typename T>
    Oct<T>* Oct<T>::makeParent(const int octant[3])
    {
        Oct<T>* ret = new Oct<T>();
        for (int i = 0; i < 3; ++i)
        {
            ret->m_bounds[i][octant[i]] = m_bounds[i][0];
            ret->m_bounds[i][octant[i] + 1] = m_bounds[i][2];
            ret->m_bounds[i][(octant[i] + 2) % 3] = (octant[i] ? (2.0f * m_bounds[i][0] - m_bounds[i][2]) : (2.0f * m_bounds[i][2] - m_bounds[i][0]));
        }
        ret->makeChildrenExcept(octant);
        ret->m_children[octant[0]][octant[1]][octant[2]] = this;
        m_parent = ret;
        return ret;
    }
    
    template<typename T>
    Oct<T>* Oct<T>::makeContains(const float pointToContain[3])
    {
        Oct<T>* ret = this;
        while (!ret->pointInside(pointToContain))
        {
            int octant[3];
            octant[0] = (pointToContain[0] < m_bounds[0][1] ? 1 : 0);//use midpoint to intelligently pick best division when more than one division would contain it
            octant[1] = (pointToContain[1] < m_bounds[1][1] ? 1 : 0);
            octant[2] = (pointToContain[2] < m_bounds[2][1] ? 1 : 0);
            ret = ret->makeParent(octant);
        }
        return ret;
    }
    
    template<typename T>
    float Oct<T>::distToPoint(const float point[3])
    {
        float temp[3];
        for (int i = 0; i < 3; ++i)
        {
            if (point[i] < m_bounds[i][0])
            {
                temp[i] = m_bounds[i][0] - point[i];
            } else {
                if (point[i] > m_bounds[i][2])
                {
                    temp[i] = m_bounds[i][2] - point[i];
                } else {
                    temp[i] = 0.0f;
                }
            }
        }
        return MathFunctions::vectorLength(temp);
    }
    
    template<typename T>
    float Oct<T>::distSquaredToPoint(const float point[3])
    {
        float temp[3];
        for (int i = 0; i < 3; ++i)
        {
            if (point[i] < m_bounds[i][0])
            {
                temp[i] = m_bounds[i][0] - point[i];
            } else {
                if (point[i] > m_bounds[i][2])
                {
                    temp[i] = m_bounds[i][2] - point[i];
                } else {
                    temp[i] = 0.0f;
                }
            }
        }
        return temp[0] * temp[0] + temp[1] * temp[1] + temp[2] * temp[2];
    }
    
    template<typename T>
    bool Oct<T>::lineIntersects(const float p1[3], const float p2[3])
    {
        float direction[3];
        float curlow = 1.0f, curhigh = -1.0f;//quiet compiler, make default say "false", but we use pointInside logic on zero length queries
        MathFunctions::subtractVectors(p2, p1, direction);
        bool first = true;
        for (int i = 0; i < 3; ++i)
        {
            if (direction[i] != 0.0f)
            {
                float templow;
                float temphigh;
                if (direction[i] > 0.0f)
                {
                    templow = (m_bounds[i][0] - p1[i]) / direction[i];//compute the range of t over which this line lies between the planes for this axis
                    temphigh = (m_bounds[i][2] - p1[i]) / direction[i];
                } else {
                    templow = (m_bounds[i][2] - p1[i]) / direction[i];//compute the range of t over which this line lies between the planes for this axis
                    temphigh = (m_bounds[i][0] - p1[i]) / direction[i];
                }
                if (first)
                {
                    first = false;
                    curlow = templow;
                    curhigh = temphigh;
                } else {
                    if (templow > curlow) curlow = templow;//intersect the ranges
                    if (temphigh < curhigh) curhigh = temphigh;
                }
                if (curhigh < curlow) return false;//if intersection is null, false
            } else {
                if (p1[i] < m_bounds[i][0] || p1[i] > m_bounds[i][2]) return false;
            }
        }
        return true;
    }
    
    template<typename T>
    bool Oct<T>::rayIntersects(const float start[3], const float p2[3])
    {
        float direction[3];
        float curlow = 1.0f, curhigh = -1.0f;//quiet compiler, make default say "false", but we use pointInside logic on zero length queries
        MathFunctions::subtractVectors(p2, start, direction);
        bool first = true;
        for (int i = 0; i < 3; ++i)
        {
            if (direction[i] != 0.0f)
            {
                float templow;
                float temphigh;
                if (direction[i] > 0.0f)
                {
                    templow = (m_bounds[i][0] - start[i]) / direction[i];//compute the range of t over which this line lies between the planes for this axis
                    temphigh = (m_bounds[i][2] - start[i]) / direction[i];
                } else {
                    templow = (m_bounds[i][2] - start[i]) / direction[i];//compute the range of t over which this line lies between the planes for this axis
                    temphigh = (m_bounds[i][0] - start[i]) / direction[i];
                }
                if (first)
                {
                    first = false;
                    curlow = templow;
                    curhigh = temphigh;
                } else {
                    if (templow > curlow) curlow = templow;//intersect the ranges
                    if (temphigh < curhigh) curhigh = temphigh;
                }
                if (curhigh < curlow || curhigh < 0.0f) return false;//if intersection is null or has no positive range, false
            } else {
                if (start[i] < m_bounds[i][0] || start[i] > m_bounds[i][2]) return false;
            }
        }
        return true;
    }
    
    template<typename T>
    bool Oct<T>::lineSegmentIntersects(const float start[3], const float end[3])
    {
        float direction[3];
        float curlow = 1.0f, curhigh = -1.0f;//quiet compiler, make default say "false", but we use pointInside logic on zero length queries
        MathFunctions::subtractVectors(end, start, direction);//parameterize the line segment to the range [0, 1] of t
        bool first = true;
        for (int i = 0; i < 3; ++i)
        {
            if (direction[i] != 0.0f)
            {
                float templow;
                float temphigh;
                if (direction[i] > 0.0f)
                {
                    templow = (m_bounds[i][0] - start[i]) / direction[i];//compute the range of t over which this line lies between the planes for this axis
                    temphigh = (m_bounds[i][2] - start[i]) / direction[i];
                } else {
                    templow = (m_bounds[i][2] - start[i]) / direction[i];//compute the range of t over which this line lies between the planes for this axis
                    temphigh = (m_bounds[i][0] - start[i]) / direction[i];
                }
                if (first)
                {
                    first = false;
                    curlow = templow;
                    curhigh = temphigh;
                } else {
                    if (templow > curlow) curlow = templow;//intersect the ranges
                    if (temphigh < curhigh) curhigh = temphigh;
                }
                if (curhigh < curlow || curhigh < 0.0f || curlow > 1.0f) return false;//if intersection is null or has no positive range, or has no range less than 1, false
            } else {
                if (start[i] < m_bounds[i][0] || start[i] > m_bounds[i][2]) return false;
            }
        }
        return true;
    }
    
    template<typename T>
    bool Oct<T>::pointInside(const float point[3])
    {
        for (int i = 0; i < 3; ++i)
        {
            if (point[i] < m_bounds[i][0] || point[i] > m_bounds[i][2]) return false;//be permissive, equal to boundary falls into both, though for traversal, strictly less than the boundary is the test condition
        }
        return true;
    }
    
    template<typename T>
    bool Oct<T>::boundsOverlaps(const float minCoords[3], const float maxCoords[3])
    {
        for (int i = 0; i < 3; ++i)
        {
            if (maxCoords[i] < m_bounds[i][0] || minCoords[i] > m_bounds[i][2]) return false;//be permissive, equal to boundary falls into both
        }
        return true;
    }
    
    template<typename T>
    Oct<T>* Oct<T>::containingChild(const float point[3], int* whichOct)
    {
        int myOct[3];
        for (int i = 0; i < 3; ++i)
        {
            myOct[i] = (point[i] < m_bounds[i][1] ? 0 : 1);//strictly less than, using only the midpoint is how traversal works, even if the point isn't inside the Oct
            if (whichOct != NULL) whichOct[i] = myOct[i];
        }
        return m_children[myOct[0]][myOct[1]][myOct[2]];
    }
}

#endif //__OCT_TREE_H__
