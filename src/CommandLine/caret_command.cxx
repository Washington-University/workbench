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

#include <iostream>

#include "CaretAssert.h"
#include "SurfaceFile.h"
#include "StringUtilities.h"

using namespace caret;


int main(int argc, char* argv[]) {

    SurfaceFile sf;
    try {
        sf.readFile("/Users/john/caret_data/caret7_test_files/gifti/Human.PALS_B12.LEFT_AVG_B1-12.FIDUCIAL_FLIRT.clean.73730.surf.gii");
        std::cout << sf.toString() << std::endl;
        
        const float* xyz = sf.getCoordinate(1000);
        std::cout << "Coordinate 1000: "
        << xyz[0] << " "
        << xyz[1] << " "
        << xyz[2] << std::endl;
        
        const int32_t* triangle = sf.getTriangle(1000);
        std::cout << "Triangle 1000: "
        << triangle[0] << " "
        << triangle[1] << " "
        << triangle[2] << std::endl;
        
        const float* normal = sf.getNormalVector(1000);
        std::cout << "Normal 1000: "
        << normal[0] << " "
        << normal[1] << " "
        << normal[2] << std::endl;
        
    }
    catch (DataFileException e) {
        std::cout << "ERROR: " << e.whatString() << std::endl;
    }

    /*
     * See if any objects were not deleted.
     */
    CaretObject::printListOfObjectsNotDeleted(true);
}
  
