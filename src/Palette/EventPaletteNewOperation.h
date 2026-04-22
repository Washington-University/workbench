#ifndef __EVENT_PALETTE_NEW_OPERATION_H__
#define __EVENT_PALETTE_NEW_OPERATION_H__

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
#include <vector>

#include "Event.h"
#include "FunctionResult.h"
#include "PaletteNew.h"

namespace caret {
    
    class EventPaletteNewOperation : public Event {
        
    public:
        enum class Operation {
            ADD_PALETTE,
            DELETE_PALETTE,
            GET_PALETTE_WITH_NAME,
            GET_USER_PALETTES,
            NEW_PALETTE,
            PALETTES_CHANGED_NOTIFICATION,
            READ_PALETTE,
            RENAME_PALETTE,
            UPDATE_PALETTE
        };
        
        Operation getOperation() const;
        
        static FunctionResult addPalette(PaletteNew* palette);
        
        static const PaletteNew* getPaletteWithName(const AString& name);
        
        static FunctionResultValue<std::vector<const PaletteNew*>> getUserPalettes();
        
        static FunctionResultValue<const PaletteNew*> createNewPalette(const AString& name,
                                                                       const int32_t numberOfPositiveControlPoints,
                                                                       const int32_t numberOfNegativeControlPoints);
        
        static FunctionResultValue<const PaletteNew*> copyPalette(const PaletteBase* paletteBase,
                                                                  const AString& newPaletteName);
        
        static FunctionResult deletePalette(const PaletteNew* palette);
        
        static void sendPalettesChangedNotification();
        
        static FunctionResultValue<const PaletteNew*> readPalette(const AString& filename);
        
        static FunctionResult renamePalette(const PaletteNew* palette,
                                            const AString& newName);
        
        static FunctionResult updatePalette(const PaletteNew* palette,
                                            const std::vector<PaletteNew::ScalarColor>& positiveMapping,
                                            const std::vector<PaletteNew::ScalarColor>& negativeMapping,
                                            const PaletteNew::ScalarColor& zeroMapping);
        
        virtual ~EventPaletteNewOperation();
        
        EventPaletteNewOperation(const EventPaletteNewOperation&) = delete;

        EventPaletteNewOperation& operator=(const EventPaletteNewOperation&) = delete;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventPaletteNewOperation(const Operation operation);
        
        void setPalettes(std::vector<const PaletteNew*>& palettePointers);
        
        void getNewPaletteInfo(AString& nameOut,
                               int32_t& numberOfPositiveControlPointsOut,
                               int32_t& numberOfNegativeControlPointsOut);
        
        void setNewPaletteInfo(const AString& name,
                               const int32_t numberOfPositiveControlPoints,
                               const int32_t numberOfNegativeControlPoints);

        void getUpdateMappings(std::vector<PaletteNew::ScalarColor>& positiveMappingOut,
                               std::vector<PaletteNew::ScalarColor>& negativeMappingOut,
                               PaletteNew::ScalarColor& zeroMappingOut);

        const Operation m_operation;
        
        std::vector<const PaletteNew*> m_palettes;

        AString m_paletteName;
        
        int32_t m_numberOfPositiveControlPoints = 0;
        
        int32_t m_numberOfNegativeControlPoints = 0;

        std::vector<PaletteNew::ScalarColor> m_positiveMapping;
        
        std::vector<PaletteNew::ScalarColor> m_negativeMapping;
        
        PaletteNew::ScalarColor m_zeroMapping;

        AString m_filename;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class Brain;

    };
    
#ifdef __EVENT_PALETTE_NEW_OPERATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_PALETTE_NEW_OPERATION_DECLARE__

} // namespace
#endif  //__EVENT_PALETTE_NEW_OPERATION_H__
