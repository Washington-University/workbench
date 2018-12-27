#ifndef __GUI_MACRO_HELPER_H__
#define __GUI_MACRO_HELPER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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
#include "WuQMacroHelperInterface.h"


namespace caret {

    class GuiMacroHelper : public CaretObject, public WuQMacroHelperInterface {
        
    public:
        GuiMacroHelper();
        
        virtual ~GuiMacroHelper();
        
        GuiMacroHelper(const GuiMacroHelper&) = delete;

        GuiMacroHelper& operator=(const GuiMacroHelper&) = delete;
        
        virtual std::vector<WuQMacroGroup*> getMacroGroups();
        
        virtual void macroWasModified(WuQMacro* macro);
        
        virtual void macroGroupWasModified(WuQMacroGroup* macroGroup);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GUI_MACRO_HELPER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GUI_MACRO_HELPER_DECLARE__

} // namespace
#endif  //__GUI_MACRO_HELPER_H__
