#ifndef __PLANE_H__
#define __PLANE_H__

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


#include "CaretObject.h"


namespace caret {

///Operations on a plane.
    class Plane : public CaretObject {
        
    public:
        Plane(const float p1[3],
              const float p2[3],
              const float p3[3]);
        
        virtual ~Plane();
        
    private:
        Plane(const Plane& o);
        
        Plane& operator=(const Plane& o);
        
    public:
        bool isValidPlane() const;
        
        bool triangleIntersectPlane(
                                    const float t1[3],
                                    const float t2[3],
                                    const float t3[3],
                                    float intersectionPointOut1[3],
                                    float intersectionPointOut2[3]) const;
        
        double absoluteDistanceToPlane(const float p[3]) const;
        
        double signedDistanceToPlane(const float p[3]) const;
        
        bool lineSegmentIntersectPlane(
                                       const float lp1[3],
                                       const float lp2[3],
                                       float intersectionOut[3]) const;
        
        static void unitTest(std::ostream& stream,
                      const bool isVerbose);
        
    private:
        static void unitTestLineIntersectPlane(std::ostream& stream,
                                        const AString& testName,
                                        const float p1[3],
                                        const float p2[3],
                                        const float p3[3],
                                        const float l1[3],
                                        const float l2[3],
                                        const float correctIntersectionPoint[3],
                                        const bool intersectionValid);
        
        static void unitTest1(std::ostream& stream);
        
        static void unitTest2(std::ostream& stream);
        
        static void unitTest3(std::ostream& stream);
        
        double p1[3];
        
        double p2[3];
        
        double p3[3];
        
        double normalVector[3];
        
        bool validPlaneFlag;
        
        double A;
        
        double B;
        
        double C;
        
        double D;
        
    };
    
} // namespace

#endif // __PLANE_H__
