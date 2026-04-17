#ifndef __PALETTE_BASE_H__
#define __PALETTE_BASE_H__

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



#include <memory>

#include "CaretObject.h"
#include "PaletteDesignTypeEnum.h"


namespace caret {
    class Palette;
    class PaletteNew;

    class PaletteBase : public CaretObject {
        
    public:
        static void sortByName(std::vector<const PaletteBase*>& palettes);
        
        PaletteBase(const PaletteDesignTypeEnum::Enum paletteDesignType);
        
        virtual ~PaletteBase();
        
        PaletteBase(const PaletteBase& obj);

        PaletteBase& operator=(const PaletteBase& obj);
        
        PaletteDesignTypeEnum::Enum getPaletteDesignType() const;

        virtual void getPaletteColor(const float scalar,
                                     const bool interpolateColorFlag,
                                     float rgbaOut[4]) const = 0;

        virtual AString getName() const = 0;
        
        virtual const PaletteBase* getInvertedPalette() const = 0;
        
        virtual const PaletteBase* getSignSeparateInvertedPalette() const = 0;
        
        virtual const PaletteBase* getNoneSeparateInvertedPalette() const = 0;
        
        virtual Palette* castToPalette() { return NULL; }
        
        virtual const Palette* castToPalette() const { return NULL; }
        
        virtual PaletteNew* castToPaletteNew() { return NULL; }
        
        virtual const PaletteNew* castToPaletteNew() const { return NULL; }
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperPaletteBase(const PaletteBase& obj);

        const PaletteDesignTypeEnum::Enum m_paletteDesignType;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_BASE_DECLARE__

} // namespace
#endif  //__PALETTE_BASE_H__
