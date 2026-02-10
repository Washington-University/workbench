#ifndef __WU_Q_TOOL_BUTTON_H__
#define __WU_Q_TOOL_BUTTON_H__

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


#include <utility>

#include <QMenu> /* without this get a compilation error in QToolButton */
#include <QToolButton>

#include "EventListenerInterface.h"
#include "GuiDarkLightThemeModeEnum.h"
#include "WorkbenchIconTypeEnum.h"

namespace caret {

    class WuQToolButton : public QToolButton, public EventListenerInterface {
        
        Q_OBJECT

    public:
        WuQToolButton(const WorkbenchIconTypeEnum::Enum iconType,
                      QWidget* parent = 0);
        
        virtual ~WuQToolButton();
        
        WuQToolButton(const WuQToolButton&) = delete;

        WuQToolButton& operator=(const WuQToolButton&) = delete;

        virtual void receiveEvent(Event* event) override;

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void showEvent(QShowEvent *event) override;
        
    private:
        void updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        void updateButtonForMacOS(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        QPixmap createPixmap(const WorkbenchIconTypeEnum::Enum iconType,
                             const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        void setFontHeight(QSharedPointer<QPainter>& painter,
                           const int32_t fontHeight);

        GuiDarkLightThemeModeEnum::Enum getCurrentDarkLightThemeMode() const;
        
        QPixmap m_lightPixmap;
        
        QPixmap m_darkPixmap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_TOOL_BUTTON_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TOOL_BUTTON_DECLARE__

} // namespace
#endif  //__WU_Q_TOOL_BUTTON_H__
