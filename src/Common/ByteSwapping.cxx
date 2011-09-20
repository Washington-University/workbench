
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

