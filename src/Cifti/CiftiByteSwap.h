/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
/*LICENSE_END*/
#ifndef __CIFTI_BYTE_SWAP
#define __CIFTI_BYTE_SWAP

#include <algorithm>

namespace caret {

class CiftiByteSwap {
   public:
      /// byte swap the data
      static inline void swapBytes(char* n, int typeSize, unsigned long long numToSwap)
      {
            for(unsigned long long i = 0;i<numToSwap;i++)
            {
               std::reverse(n,n+typeSize);n+=typeSize;
            }
      }
      static void swapBytes(short* n, unsigned long long numToSwap)
      {
         swapBytes((char *)n,2,numToSwap);
      }
      
      /// byte swap the data
      static void swapBytes(unsigned short* n, unsigned long long numToSwap)
      {
         swapBytes((char *)n,2,numToSwap);
      }
      
      /// byte swap the data
      static void swapBytes(int* n, unsigned long long numToSwap){
         swapBytes((char *)n,4,numToSwap);
      }
      
      /// byte swap the data
      static void swapBytes(unsigned int* n, unsigned long long numToSwap)
      {
         swapBytes((char *)n,4,numToSwap);
      }
      
      /// byte swap the data
      static void swapBytes(long long* n, unsigned long long numToSwap)
      {
         swapBytes((char *)n,8,numToSwap);
      }
      
      /// byte swap the data
      static void swapBytes(unsigned long long* n, unsigned long long numToSwap)
      {
         swapBytes((char *)n,8,numToSwap);
      }
      
      /// byte swap the data
      static void swapBytes(float* n, unsigned long long numToSwap)
      {
         swapBytes((char *)n,4,numToSwap);
      }
      
      /// byte swap the data
      static void swapBytes(double* n, unsigned long long numToSwap)
      {
         swapBytes((char *)n,8,numToSwap);
      }  
};

}
#endif //__CIFTI_BYTE_SWAP
