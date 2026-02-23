#ifndef __WORKBENCH_TOOL_BUTTON_H__
#define __WORKBENCH_TOOL_BUTTON_H__

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

#include <map>

#include <QMenu> /* without this get a compilation error in QToolButton */
#include <QToolButton>

#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "GuiDarkLightColorSchemeModeEnum.h"
#include "WorkbenchIconTypeEnum.h"

namespace caret {
    class CaretObject;

    class WorkbenchToolButton :
       public QToolButton,
       public CaretObject,  // Can be used to track allocation/deallocation of instances to ensure no memory leaks
       public EventListenerInterface {
        
        Q_OBJECT

    public:
        enum class MenuStatus {
            MENU_NO,
            MENU_YES
        };
        
        WorkbenchToolButton(const WorkbenchIconTypeEnum::Enum iconType,
                            const MenuStatus menuStatus = MenuStatus::MENU_NO,
                            QWidget* parent = 0);
        
        WorkbenchToolButton(const MenuStatus menuStatus = MenuStatus::MENU_NO,
                            QWidget* parent = 0);
        
        virtual ~WorkbenchToolButton();
        
        WorkbenchToolButton(const WorkbenchToolButton&) = delete;

        WorkbenchToolButton& operator=(const WorkbenchToolButton&) = delete;

        virtual void receiveEvent(Event* event) override;

        void updateStyleSheet();
        
        void setSpecialBackgroundColor(const QColor& specialBackgroundColor);
           
        void setSpecialBackgroundColorEnabled(const bool enabled);
           
        static void printLeftoverWorkbenchToolButtons();
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void showEvent(QShowEvent *event) override;
        
    private:
        void initialize();
        
        void updateForDarkLightColorScheme(const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode);
        
        void updateButtonForMacOS(const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode);
        
        GuiDarkLightColorSchemeModeEnum::Enum getCurrentDarkLightColorSchemeMode() const;
        
        const MenuStatus m_menuStatus;
        
        QColor m_specialBackgroundColor;
           
        bool m_specialBackgroundColorEnabled = false;
           
        static std::map<WorkbenchToolButton*, int> s_allWorkbenchToolButtons;
        static int s_allWorkbenchButtonsCounter;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_TOOL_BUTTON_DECLARE__
    std::map<WorkbenchToolButton*, int> WorkbenchToolButton::s_allWorkbenchToolButtons;
    int WorkbenchToolButton::s_allWorkbenchButtonsCounter = 0;
#endif // __WORKBENCH_TOOL_BUTTON_DECLARE__

} // namespace
#endif  //__WORKBENCH_TOOL_BUTTON_H__
