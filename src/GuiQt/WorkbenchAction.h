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

#include <set>
#include <utility>

#include <QAction>

#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTextAlignVerticalEnum.h"
#include "AnnotationTextOrientationEnum.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "GuiDarkLightThemeModeEnum.h"
#include "WorkbenchIconTypeEnum.h"

namespace caret {

    class WorkbenchAction :
    public QAction,
    public CaretObject,
    public EventListenerInterface {
        
        Q_OBJECT

    public:
        WorkbenchAction(const WorkbenchIconTypeEnum::Enum iconType,
                        QObject* parent);
        
        virtual ~WorkbenchAction();
        
        WorkbenchAction(const WorkbenchAction&) = delete;

        WorkbenchAction& operator=(const WorkbenchAction&) = delete;

        virtual void receiveEvent(Event* event) override;

        static void printLeftoverWorkbenchActions();
        
        // ADD_NEW_METHODS_HERE

    private:
        enum class ArrowType {
            DOWN,
            UP
        };
        
        enum class Origin {
            CENTER,
            TOP_LEFT
        };
        
        enum class PixelModification {
            NONE,
            REPLACE_BLACK_WITH_DARK_GRAY
        };
        
        void createPixmapPainter(const int32_t width,
                                 const int32_t height,
                                 const Origin origin,
                                 const int32_t fontHeight,
                                 const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode,
                                 QPixmap& pixmapOut,
                                 QSharedPointer<QPainter>& painterOut);
        
        void updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        QPixmap createPixmapForIconType(const WorkbenchIconTypeEnum::Enum iconType,
                                        const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        void setFontHeight(QSharedPointer<QPainter>& painter,
                           const int32_t fontHeight);

        void setPixmapIcon(QPixmap& pixmap,
                           QPainter* painter,
                           const AString& imageFileName,
                           const AString& alternativeTextForPixmap,
                           const bool darkThemeFlag,
                           const PixelModification pixelModification);
        
        GuiDarkLightThemeModeEnum::Enum getCurrentDarkLightThemeMode() const;
        
        void replacePixmapPixelColor(const AString& imageFileName,
                                     QPixmap& pixmapInOut,
                                     const QColor& findColor,
                                     const QColor& replaceColor);

        void createHorizontalAlignmentPixmap(QPixmap& pixmap,
                                             QPainter* painter,
                                             const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment);
        
        void createVerticalAlignmentPixmap(QPixmap& pixmap,
                                           QPainter* painter,
                                           const AnnotationTextAlignVerticalEnum::Enum verticalAlignment);
        
        void createTextOrientationPixmap(QPixmap& pixmap,
                                         QPainter* painter,
                                         const AnnotationTextOrientationEnum::Enum orientation);

        void createArrowPixmap(QPixmap& pixmap,
                               QPainter* painter,
                               const ArrowType arrowType);

        const WorkbenchIconTypeEnum::Enum m_iconType;
        
        QPixmap m_lightPixmap;
        
        QPixmap m_darkPixmap;
        
        static std::set<WorkbenchAction*> s_allWorkbenchActions;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_ACTION_DECLARE__
    std::set<WorkbenchAction*> WorkbenchAction::s_allWorkbenchActions;
#endif // __WORKBENCH_ACTION_DECLARE__

} // namespace
#endif  //__WORKBENCH_ACTION_H__
