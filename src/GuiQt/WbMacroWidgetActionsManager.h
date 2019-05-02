#ifndef __WB_MACRO_WIDGET_ACTIONS_MANAGER_H__
#define __WB_MACRO_WIDGET_ACTIONS_MANAGER_H__

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

#include <QObject>



namespace caret {

    class WuQMacroWidgetAction;
    
    class WbMacroWidgetActionsManager : public QObject {
        
        Q_OBJECT

    public:
        WbMacroWidgetActionsManager(QObject* parent);
        
        virtual ~WbMacroWidgetActionsManager();
        
        
        WbMacroWidgetActionsManager(const WbMacroWidgetActionsManager&) = delete;

        WbMacroWidgetActionsManager& operator=(const WbMacroWidgetActionsManager&) = delete;
        
        std::vector<WuQMacroWidgetAction*> getMacroWidgetActions();
        
        WuQMacroWidgetAction* getSurfacePropertiesLinkDiameterWidgetAction();

        WuQMacroWidgetAction* getSurfacePropertiesOpacityWidgetAction();
        
        WuQMacroWidgetAction* getSurfacePropertiesVertexDiameterWidgetAction();
        
        // ADD_NEW_METHODS_HERE

    private:
        std::vector<WuQMacroWidgetAction*> m_macroWidgetActions;
        
        WuQMacroWidgetAction* createSurfacePropertiesOpacityWidgetAction();
        
        WuQMacroWidgetAction* m_surfacePropertiesOpacityWidgetAction;
        
        WuQMacroWidgetAction* createSurfacePropertiesLinkDiameterWidgetAction();
        
        WuQMacroWidgetAction* m_surfacePropertiesLinkDiameterWidgetAction;
        
        WuQMacroWidgetAction* createSurfacePropertiesVertexDiameterWidgetAction();
        
        WuQMacroWidgetAction* m_surfacePropertiesVertexDiameterWidgetAction;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WB_MACRO_WIDGET_ACTIONS_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WB_MACRO_WIDGET_ACTIONS_MANAGER_DECLARE__

} // namespace
#endif  //__WB_MACRO_WIDGET_ACTIONS_MANAGER_H__
