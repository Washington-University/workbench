
#include <iostream>

#include "SurfaceFile.h"
#include "StringUtilities.h"

using namespace caret;


int main(int argc, char* argv[]) {
   std::cout << "In Main" << std::endl;

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
}
  
