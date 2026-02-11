#ifndef __WORKBENCH_ACTION_H__
#define __WORKBENCH_ACTION_H__

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

#include <QAction>

#include "EventListenerInterface.h"
#include "GuiDarkLightThemeModeEnum.h"
#include "WorkbenchIconTypeEnum.h"

namespace caret {

    class WorkbenchAction : public QAction, public EventListenerInterface {
        
        Q_OBJECT

    public:
        WorkbenchAction(const WorkbenchIconTypeEnum::Enum iconType,
                        QObject* parent = nullptr);
        
        virtual ~WorkbenchAction();
        
        WorkbenchAction(const WorkbenchAction&) = delete;

        WorkbenchAction& operator=(const WorkbenchAction&) = delete;

        virtual void receiveEvent(Event* event) override;

        // ADD_NEW_METHODS_HERE

    private:
        void updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        QPixmap createPixmap(const WorkbenchIconTypeEnum::Enum iconType,
                             const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        void setFontHeight(QSharedPointer<QPainter>& painter,
                           const int32_t fontHeight);

        void setPixmapIcon(QPixmap& pixmap,
                           QPainter* painter,
                           const AString& imageFileName,
                           const AString& alternativeTextForPixmap,
                           const bool darkThemeFlag,
                           const bool replaceWhiteWithTransparentFlag);
        
        GuiDarkLightThemeModeEnum::Enum getCurrentDarkLightThemeMode() const;
        
        void replaceWhiteWithTransparent(const AString& imageFileName,
                                         QPixmap& pixmapInOut);
        
        QPixmap m_lightPixmap;
        
        QPixmap m_darkPixmap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_ACTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WORKBENCH_ACTION_DECLARE__

} // namespace
#endif  //__WORKBENCH_ACTION_H__
