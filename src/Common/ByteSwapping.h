#ifndef __BYTE_SWAPPING_H__
#define __BYTE_SWAPPING_H__

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

#include <stdint.h>


namespace caret {

/**
 * This class contains static methods for byte swapping data, typically used
 * when reading binary data files.
 */
    class ByteSwapping {
    private:
        ByteSwapping() { }

        ~ByteSwapping() { }

    public:
        static void swapBytes(uint8_t*, const uint64_t) { }//define them for completeness, so templated stuff can just call it without specializing

        static void swapBytes(int8_t*, const uint64_t) { }

        static void swapBytes(uint16_t* n, const uint64_t numToSwap);

        static void swapBytes(int16_t* n, const uint64_t numToSwap);

        static void swapBytes(int32_t* n, const uint64_t numToSwap);

        static void swapBytes(uint32_t* n, const uint64_t numToSwap);

        static void swapBytes(int64_t* n, const uint64_t numToSwap);

        static void swapBytes(uint64_t* n, const uint64_t numToSwap);

        static void swapBytes(float* n, const uint64_t numToSwap);

        static void swapBytes(double* n, const uint64_t numToSwap);

        static void swapBytes(long double* n, const uint64_t numToSwap);

        template<typename T>
        static void swap(T& toSwap);//templated versions, to replace hand-coding variants

        template<typename T>
        static void swapArray(T* toSwap, const uint64_t& count);

        inline static bool isBigEndian()
        {
            uint16_t test = 1;
            return (((char*)&test)[0] == 0);
        }

    };

    template<typename T>
    void ByteSwapping::swap(T& toSwap)
    {
        if (sizeof(T) == 1) return;//we could specialize 1-byte types, but this should optimize out
        T temp = toSwap;
        char* from = (char*)&temp;
        char* to = (char*)&toSwap;
        for (int i = 0; i < (int)sizeof(T); ++i)
        {
            to[i] = from[sizeof(T) - i - 1];
        }
    }

    template<typename T>
    void ByteSwapping::swapArray(T* toSwap, const uint64_t& count)
    {
        if (sizeof(T) == 1) return;//ditto
        for (uint64_t i = 0; i < count; ++i)
        {
            swap(toSwap[i]);
        }
    }

}

#endif  // __BYTE_SWAPPING_H__
