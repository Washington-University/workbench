#ifndef __PALETTE_GROUP_STANDARD_PALETTES_H__
#define __PALETTE_GROUP_STANDARD_PALETTES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#include <set>

#include "PaletteGroup.h"



namespace caret {
    class PaletteNew;

    class PaletteGroupStandardPalettes : public PaletteGroup {
        
    public:
        PaletteGroupStandardPalettes();
        
        virtual ~PaletteGroupStandardPalettes();
        
        PaletteGroupStandardPalettes(const PaletteGroupStandardPalettes&) = delete;

        PaletteGroupStandardPalettes& operator=(const PaletteGroupStandardPalettes&) = delete;
        
        void loadPalettes();
        
        virtual void getPalettes(std::vector<PaletteNew>& palettesOut) const override;
        
        virtual std::unique_ptr<PaletteNew> getPaletteWithName(const AString& paletteName) override;

        virtual bool hasPaletteWithName(const AString& paletteName) override;

        // ADD_NEW_METHODS_HERE

    protected:
        virtual bool addPaletteImplementation(const PaletteNew& palette,
                                              AString& errorMessageOut) override;
        
        virtual bool replacePaletteImplementation(const PaletteNew& palette,
                                                  AString& errorMessageOut) override;
        
        virtual bool removePaletteImplementation(const AString& paletteName,
                                                 AString& errorMessageOut) override;
        
        virtual bool renamePaletteImplementation(const AString& paletteName,
                                                 const AString& newPaletteName,
                                                 AString& errorMessageOut) override;

    private:
        void addPaletteHelper(const PaletteNew& palette);

        /* unordered_set may be a better choice as sorting not important */
        typedef std::set<PaletteNew, PaletteNewCompare> ContainerType;
        typedef ContainerType::iterator ContainerIterator;
        ContainerType m_palettes;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_GROUP_STANDARD_PALETTES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_GROUP_STANDARD_PALETTES_DECLARE__

} // namespace
#endif  //__PALETTE_GROUP_STANDARD_PALETTES_H__
