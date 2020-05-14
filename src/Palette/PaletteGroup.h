#ifndef __PALETTE_GROUP_H__
#define __PALETTE_GROUP_H__

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

#include "CaretObject.h"
#include "PaletteNew.h"

namespace caret {

    class PaletteGroup : public CaretObject {
        
    public:
        enum class GroupType {
            DATA_FILE,
            STANDARD,
            USER_CUSTOM
        };
        
        PaletteGroup(const GroupType groupType);
        
        virtual ~PaletteGroup();
        
        PaletteGroup(const PaletteGroup&) = delete;

        PaletteGroup& operator=(const PaletteGroup&) = delete;
        
        AString getGroupName() const;
        
        GroupType getGroupType() const;

        bool isEditable() const;
        
        bool addPalette(const PaletteNew& palette,
                        AString& errorMessageOut);
        
        bool replacePalette(const PaletteNew& palette,
                            AString& errorMessageOut);
        
        bool removePalette(const AString& paletteName,
                           AString& errorMessageOut);
        
        bool renamePalette(const AString& paletteName,
                           const AString& newPaletteName,
                           AString& errorMessageOut);

        virtual void getPalettes(std::vector<PaletteNew>& palettesOut) const = 0;
        
        virtual std::unique_ptr<PaletteNew> getPaletteWithName(const AString& paletteName) = 0;
                
        virtual bool hasPaletteWithName(const AString& paletteName) = 0;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;

    protected:
        void setEditable(const bool status);

        virtual bool addPaletteImplementation(const PaletteNew& palette,
                                              AString& errorMessageOut) = 0;
        
        virtual bool replacePaletteImplementation(const PaletteNew& palette,
                                                  AString& errorMessageOut) = 0;
        
        virtual bool removePaletteImplementation(const AString& paletteName,
                                                 AString& errorMessageOut) = 0;
        
        virtual bool renamePaletteImplementation(const AString& paletteName,
                                                 const AString& newPaletteName,
                                                 AString& errorMessageOut) = 0;
        
        /*
         * Compares palettes by name
         */
        struct PaletteNewCompare {
            bool operator() (const PaletteNew& lhs, const PaletteNew& rhs) const {
                return (lhs.getName() < rhs.getName());
            }
        };
        
    private:
        const GroupType m_groupType;
        
        AString m_groupName;
        
        bool m_editableFlag = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_GROUP_DECLARE__

} // namespace
#endif  //__PALETTE_GROUP_H__
