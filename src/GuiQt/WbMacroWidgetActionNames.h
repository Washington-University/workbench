#ifndef __WB_MACRO_WIDGET_ACTION_NAMES_H__
#define __WB_MACRO_WIDGET_ACTION_NAMES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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


#include <QString>

namespace caret {

    class WbMacroWidgetActionNames {
        
    public:
        /*
         * NOTE: These names are saved into scene and changing the names will break scenes
         */

        static QString getSurfacePropertiesOpacityName() { return "SurfaceProperties:surfaceOpacity"; }
        
        static QString getSurfacePropertiesLinkDiameterName() { return "SurfaceProperties:linkDiameter"; }
        
        static QString getSurfacePropertiesVertexDiameterName() { return "SurfaceProperties:vertexDiameter"; }
        
        static QString getSurfacePropertiesDisplayNormalVectorsName() { return "SurfaceProperties:displayNormalVectors"; }
        
        static QString getSurfacePropertiesDrawingTypeName() { return "SurfaceProperties:drawingType"; }
        
        WbMacroWidgetActionNames() = delete;
        
        virtual ~WbMacroWidgetActionNames() = delete;
        
        WbMacroWidgetActionNames(const WbMacroWidgetActionNames&) = delete;

        WbMacroWidgetActionNames& operator=(const WbMacroWidgetActionNames&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WB_MACRO_WIDGET_ACTION_NAMES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WB_MACRO_WIDGET_ACTION_NAMES_DECLARE__

} // namespace
#endif  //__WB_MACRO_WIDGET_ACTION_NAMES_H__
