#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_H__

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



#include <memory>

#include "BrainBrowserWindowToolBarComponent.h"
#include "ModelSurface.h"
#include "StructureEnum.h"


namespace caret {

    class StructureSurfaceSelectionControl;

    class BrainBrowserWindowToolBarSurface : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
    public:
        BrainBrowserWindowToolBarSurface(const QString& parentObjectName,
                                         BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarSurface();
        
        BrainBrowserWindowToolBarSurface(const BrainBrowserWindowToolBarSurface&) = delete;

        BrainBrowserWindowToolBarSurface& operator=(const BrainBrowserWindowToolBarSurface&) = delete;
        
        virtual void updateContent(BrowserTabContent* browserTabContent) override;
        
    private slots:
        void surfaceSelectionControlChanged(const StructureEnum::Enum,
                                            ModelSurface*);

        // ADD_NEW_METHODS_HERE

    private:
        StructureSurfaceSelectionControl* surfaceSurfaceSelectionControl;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_SURFACE_H__
