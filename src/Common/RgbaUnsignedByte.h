#ifndef __RGBA_UNSIGNED_BYTE_H__
#define __RGBA_UNSIGNED_BYTE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include <cstdint>

#include "AString.h"

namespace caret {

    class RgbaUnsignedByte {
        
    public:
        RgbaUnsignedByte();
        
        RgbaUnsignedByte(const uint8_t red,
                         const uint8_t green,
                         const uint8_t blue,
                         const uint8_t alpha);
        
        RgbaUnsignedByte(const uint8_t rgba[4]);

        virtual ~RgbaUnsignedByte();
        
        RgbaUnsignedByte(const RgbaUnsignedByte& obj);

        RgbaUnsignedByte& operator=(const RgbaUnsignedByte& obj);
        
        bool operator==(const RgbaUnsignedByte& obj) const;
        
        const uint8_t* getRgbaPointer() const;
        
        void getRgba(uint8_t rgba[4]) const;
        
        void getRgba(uint8_t& red,
                     uint8_t& green,
                     uint8_t& blue,
                     uint8_t& alpha) const;
        
        void setRgba(const uint8_t rgba[4]);
        
        void setRgba(const uint8_t red,
                     const uint8_t green,
                     const uint8_t blue,
                     const uint8_t alpha);

        uint32_t getUnsignedIntRGBA() const;
        
        void setUnsignedIntRGBA(const uint32_t rgbaUnsignedInt);
        
        AString toString() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperRgbaUnsignedByte(const RgbaUnsignedByte& obj);

        void setUnsignedIntValue();
        
        uint32_t m_unsignedIntRGBA = 0;
        
        uint8_t* m_rgba = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RGBA_UNSIGNED_BYTE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RGBA_UNSIGNED_BYTE_DECLARE__

} // namespace
#endif  //__RGBA_UNSIGNED_BYTE_H__
