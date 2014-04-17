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

#include "ByteSwapping.h"

using namespace caret;
/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(int16_t* n, const uint64_t numToSwap)
{
   for (uint64_t i = 0; i < numToSwap; i++) {
      char* bytes = (char*)&n[i];
      char  temp = bytes[0];
      bytes[0] = bytes[1];
      bytes[1] = temp;
   }
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(uint16_t* n, const uint64_t numToSwap)
{
   swapBytes((int16_t*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(int32_t* n, const uint64_t numToSwap)
{
   for (uint64_t i = 0; i < numToSwap; i++) {
      char* bytes = (char*)&n[i];
      char  temp = bytes[0];
      bytes[0] = bytes[3];
      bytes[3] = temp;

      temp = bytes[1];
      bytes[1] = bytes[2];
      bytes[2] = temp;
   }
}

/**
 *
 */
void 
ByteSwapping::swapBytes(uint32_t* n, const uint64_t numToSwap)
{
   swapBytes((int32_t*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(int64_t* n, const uint64_t numToSwap)
{
   for (uint64_t i = 0; i < numToSwap; i++) {
      char* bytes = (char*)&n[i];
      char  temp = bytes[0];
      bytes[0] = bytes[7];
      bytes[7] = temp;

      temp = bytes[1];
      bytes[1] = bytes[6];
      bytes[6] = temp;

      temp = bytes[2];
      bytes[2] = bytes[5];
      bytes[5] = temp;

      temp = bytes[3];
      bytes[3] = bytes[4];
      bytes[4] = temp;
   }
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(uint64_t* n, const uint64_t numToSwap)
{
   swapBytes((int64_t*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(float* n, const uint64_t numToSwap)
{
   swapBytes((int32_t*)n, numToSwap);
}

/**
 * Swap bytes for the specified type.
 */
void 
ByteSwapping::swapBytes(double* n, const uint64_t numToSwap)
{
   swapBytes((int64_t*)n, numToSwap);
}

void 
ByteSwapping::swapBytes(long double* n, const uint64_t numToSwap)
{
    swapArray(n, numToSwap);
}
