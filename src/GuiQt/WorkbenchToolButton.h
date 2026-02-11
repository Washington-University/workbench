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


#include <QMenu> /* without this get a compilation error in QToolButton */
#include <QToolButton>

#include "EventListenerInterface.h"
#include "GuiDarkLightThemeModeEnum.h"
#include "WorkbenchIconTypeEnum.h"

namespace caret {

    class WorkbenchToolButton : public QToolButton, public EventListenerInterface {
        
        Q_OBJECT

    public:
        WorkbenchToolButton(const WorkbenchIconTypeEnum::Enum iconType,
                            QWidget* parent = 0);
        
        WorkbenchToolButton(QWidget* parent = 0);
        
        virtual ~WorkbenchToolButton();
        
        WorkbenchToolButton(const WorkbenchToolButton&) = delete;

        WorkbenchToolButton& operator=(const WorkbenchToolButton&) = delete;

        virtual void receiveEvent(Event* event) override;

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void showEvent(QShowEvent *event) override;
        
    private:
        void updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        void updateButtonForMacOS(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        GuiDarkLightThemeModeEnum::Enum getCurrentDarkLightThemeMode() const;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_TOOL_BUTTON_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WORKBENCH_TOOL_BUTTON_DECLARE__

} // namespace
#endif  //__WORKBENCH_TOOL_BUTTON_H__
