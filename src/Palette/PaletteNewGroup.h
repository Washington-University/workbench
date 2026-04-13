#ifndef __PALETTE_NEW_GROUP_H__
#define __PALETTE_NEW_GROUP_H__

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

#include "CaretObject.h"
#include "FunctionResult.h"
#include "PaletteNew.h"

namespace caret {

    class PaletteNewGroup : public CaretObject {
        
    public:
        enum class GroupType {
            STANDARD,
            USER_CUSTOM
        };
        
        PaletteNewGroup(const GroupType groupType);
        
        virtual ~PaletteNewGroup();
        
        PaletteNewGroup(const PaletteNewGroup&) = delete;

        PaletteNewGroup& operator=(const PaletteNewGroup&) = delete;
        
        AString getGroupName() const;
        
        GroupType getGroupType() const;

        bool isEditable() const;
        
        void addExamplePalettes();
        
        FunctionResult addPalette(PaletteNew* palette);
        
        FunctionResultValue<PaletteNew*> addNewPalette(const AString& name,
                                                       const int32_t numberOfPositiveControlPoints,
                                                       const int32_t numberOfNegativeControlPoints);

        FunctionResult removePalette(const PaletteNew* palette);
        
        FunctionResult renamePalette(const PaletteNew* palette,
                                     const AString& newPaletteName);

        void getPalettes(std::vector<PaletteNew*>& palettesOut) const;
        
        void getPalettes(std::vector<const PaletteNew*>& palettesOut) const;
        
        FunctionResultValue<PaletteNew*> getPaletteWithName(const AString& paletteName) const;
                
        FunctionResult updatePalette(const PaletteNew* palette,
                                     const std::vector<PaletteNew::ScalarColor>& positiveMapping,
                                     const std::vector<PaletteNew::ScalarColor>& negativeMapping,
                                     const PaletteNew::ScalarColor& zeroMapping);

        bool hasPaletteWithName(const AString& paletteName);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;

    protected:
        void setEditable(const bool status);

    private:
        const GroupType m_groupType;
        
        AString m_groupName;
        
        bool m_editableFlag = true;
        
        /*
         * Compares palettes by name
         */
        struct PaletteNewCompare {
            bool operator() (const std::unique_ptr<PaletteNew>& lhs, const std::unique_ptr<PaletteNew>& rhs) const {
                return (lhs->getName() < rhs->getName());
            }
        };
        
        /*
         * Compares palettes by name
         */
        struct PaletteNewPointerCompare {
            bool operator() (const PaletteNew* lhs, const PaletteNew* rhs) const {
                const int result(QString::compare(lhs->getName(),
                                                  rhs->getName(),
                                                  Qt::CaseInsensitive));
                return (result < 0);
            }
        } PaletteNewPointerNameSort;
        
        std::set<std::unique_ptr<PaletteNew>, PaletteNewCompare> m_palettes;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_NEW_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_NEW_GROUP_DECLARE__

} // namespace
#endif  //__PALETTE_NEW_GROUP_H__
