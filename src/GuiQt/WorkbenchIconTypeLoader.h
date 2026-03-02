#ifndef __WORKBENCH_ICON_TYPE_LOADER_H__
#define __WORKBENCH_ICON_TYPE_LOADER_H__

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
#include <set>
#include <utility>

#include <QPixmap>

#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTextAlignVerticalEnum.h"
#include "AnnotationTextOrientationEnum.h"
#include "CaretObject.h"
#include "FunctionResult.h"
#include "GuiDarkLightColorSchemeModeEnum.h"
#include "WorkbenchIconTypeEnum.h"

namespace caret {

    class WorkbenchIconTypeLoader : public CaretObject {

    public:
        virtual ~WorkbenchIconTypeLoader();
        
        WorkbenchIconTypeLoader(const WorkbenchIconTypeLoader&) = delete;

        WorkbenchIconTypeLoader& operator=(const WorkbenchIconTypeLoader&) = delete;

        static QPixmap loadPixmapForIconType(const WorkbenchIconTypeEnum::Enum iconType,
                                             const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode);
        
        static QPixmap loadPixmapForIconTypeForCurrrentDarkLightColorScheme(const WorkbenchIconTypeEnum::Enum iconType);

        // ADD_NEW_METHODS_HERE

    private:
        WorkbenchIconTypeLoader();
        
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
        
        static void createPixmapPainter(const int32_t width,
                                 const int32_t height,
                                 const Origin origin,
                                 const int32_t fontHeight,
                                 const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode,
                                 QPixmap& pixmapOut,
                                 std::shared_ptr<QPainter>& painterOut);
        
        static QPixmap createPixmapForIconType(const WorkbenchIconTypeEnum::Enum iconType,
                                               const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode);
        
        static void setFontHeight(std::shared_ptr<QPainter>& painter,
                           const int32_t fontHeight);

        static void setPixmapIcon(QPixmap& pixmap,
                                  QPainter* painter,
                                  const AString& imageFileName,
                                  const AString& alternativeTextForPixmap,
                                  const bool darkColorSchemeFlag,
                                  const PixelModification pixelModification);

        static void replacePixmapPixelColor(const AString& imageFileName,
                                     QPixmap& pixmapInOut,
                                     const QColor& findColor,
                                     const QColor& replaceColor);

        static void createHorizontalAlignmentPixmap(QPixmap& pixmap,
                                             QPainter* painter,
                                             const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment);
        
        static void createVerticalAlignmentPixmap(QPixmap& pixmap,
                                           QPainter* painter,
                                           const AnnotationTextAlignVerticalEnum::Enum verticalAlignment);
        
        static void createTextOrientationPixmap(QPixmap& pixmap,
                                         QPainter* painter,
                                         const AnnotationTextOrientationEnum::Enum orientation);

        static void createArrowPixmap(QPixmap& pixmap,
                               QPainter* painter,
                               const ArrowType arrowType);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_ICON_TYPE_LOADER_DECLARE__
#endif // __WORKBENCH_ICON_TYPE_LOADER_DECLARE__

} // namespace
#endif  //__WORKBENCH_ICON_TYPE_LOADER_H__
