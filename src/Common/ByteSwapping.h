
#ifndef __BYTE_SWAPPING_H__
#define __BYTE_SWAPPING_H__

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

#include <stdint.h>

/**
 * This class contains static methods for byte swapping data, typically used
 * when reading binary data files.
 */

namespace caret {

class ByteSwapping {
private:
    ByteSwapping() { }
    
    ~ByteSwapping() { }

public:
      static void swapBytes(uint16_t* n, const uint64_t numToSwap);
      
      static void swapBytes(int16_t* n, const uint64_t numToSwap);
      
      static void swapBytes(int32_t* n, const uint64_t numToSwap);
      
      static void swapBytes(uint32_t* n, const uint64_t numToSwap);
      
      static void swapBytes(int64_t* n, const uint64_t numToSwap);
      
      static void swapBytes(uint64_t* n, const uint64_t numToSwap);
      
      static void swapBytes(float* n, const uint64_t numToSwap);
      
      static void swapBytes(double* n, const uint64_t numToSwap);
  
};

}

#endif  // __BYTE_SWAPPING_H__
