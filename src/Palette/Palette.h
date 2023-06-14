#ifndef __PALETTE_H__
#define __PALETTE_H__

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

#include <memory>
#include <stdint.h>
#include <vector>

#include "CaretAssert.h"
#include "CaretObject.h"
#include "TracksModificationInterface.h"


namespace caret {

    class PaletteScalarAndColor;

    /**
     * A color palette.
     */
    class Palette : public CaretObject, TracksModificationInterface {
        
    public:
        Palette();
        
        Palette(const Palette& p);
        
        Palette& operator=(const Palette& o);
        
        virtual ~Palette();
        
    private:
        void copyHelper(const Palette& o);
        
        void initializeMembersPalette();
        
    public:
        AString toString() const;
        
        AString getName() const;
        
        void setName(const AString& name);
        
        /**
         * Get the number of scalars and colors.
         *
         * @return - number of scalars and colors.
         *
         */
        inline int32_t getNumberOfScalarsAndColors() const {
            return this->paletteScalars.size();
        }
        
        /**
         * Get a scalar and color for the specified index.
         *
         * @param index - index of scalar and color.
         * @return  Reference to item at index or null if invalid index.
         *
         */
        inline PaletteScalarAndColor* getScalarAndColor(const int32_t indx) const {
            CaretAssertVectorIndex(this->paletteScalars, indx);
            return this->paletteScalars[indx];
        }
        
        void addScalarAndColor(const float scalar,
                               const AString& colorName);
        
        void insertScalarAndColor(
                                  const PaletteScalarAndColor& psac,
                                  const int32_t insertAfterIndex);
        
        void removeScalarAndColor(const int32_t index);
        
        void getMinMax(float& minOut, float& maxOut) const;
        
        void getPaletteColor(const float scalar,
                             const bool interpolateColorFlag,
                             float rgbaOut[4]) const;
        
        void setModified();
        
        void clearModified();
        
        bool isModified() const;
        
        const Palette* getInvertedPalette() const;
        
        const Palette* getSignSeparateInvertedPalette() const;
        
        const Palette* getNoneSeparateInvertedPalette() const;
        
        static AString getDefaultPaletteName();
        
    public:
        /**Name of gray interpolate palette */
        static  const AString GRAY_INTERP_PALETTE_NAME;
        
        /**Name of gray interpolate palette for positive data */
        static  const AString GRAY_INTERP_POSITIVE_PALETTE_NAME;
        
        /**"none" color name. */
        static  const AString NONE_COLOR_NAME;
        
        /** "ROY-BIG-BL" palette */
        static const AString ROY_BIG_BL_PALETTE_NAME;
        
        /** Special  palette name that is used but the file's three maps are mapped to red, green, blue*/
        static const AString SPECIAL_RGB_VOLUME_PALETTE_NAME;
        
    private:
        Palette* createSignSeparateInvertedPalette() const;
        
        /**has this object been modified. (DO NOT CLONE) */
        bool modifiedFlag;
        
        /**Name of the palette. */
        AString name;
        
        /**The scalars in the palette. */
        std::vector<PaletteScalarAndColor*> paletteScalars;
        
        /** The inverted palette is lazily initialized */
        mutable std::unique_ptr<Palette> m_invertedPalette;
        
        /** The inverted palette with negative inverted separate from positive */
        mutable std::unique_ptr<Palette> m_signSeparateInvertedPalette;
        
        /** The inverted palette with negative inverted separate from positive */
        mutable std::unique_ptr<Palette> m_noneSeparateInvertedPalette;
    };

    
#ifdef __PALETTE_DEFINE__
    const AString Palette::GRAY_INTERP_PALETTE_NAME = "Gray_Interp";
    const AString Palette::GRAY_INTERP_POSITIVE_PALETTE_NAME = "Gray_Interp_Positive";
    //const AString Palette::NONE_COLOR_NAME = "none";
    const AString Palette::ROY_BIG_BL_PALETTE_NAME = "ROY-BIG-BL";
    const AString Palette::SPECIAL_RGB_VOLUME_PALETTE_NAME = "Special-RGB-Volume";
#endif // __PALETTE_DEFINE__
} // namespace

#endif // __PALETTE_H__
