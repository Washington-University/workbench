
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

#define __WORKBENCH_ACTION_DECLARE__
#include "WorkbenchAction.h"
#undef __WORKBENCH_ACTION_DECLARE__

#include <QEvent>
#include <QImage>
#include <QPainter>

#include "AnnotationCoordinateSpaceEnum.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventDarkLightThemeModeChanged.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WorkbenchAction 
 * \brief Extends QAction to work with light/dark mode for the icon
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param iconType
 *    Type of icon for tool button
 * @param parent
 *    Parent widget
 */
WorkbenchAction::WorkbenchAction(const WorkbenchIconTypeEnum::Enum iconType,
                                 QObject* parent)
: QAction(parent)
{
    static bool firstFlag(true);
    if (firstFlag) {
        firstFlag = false;
    }

    m_darkPixmap = createPixmap(iconType,
                                 GuiDarkLightThemeModeEnum::DARK);
    m_lightPixmap = createPixmap(iconType,
                                 GuiDarkLightThemeModeEnum::LIGHT);
    
    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
}

/**
 * Destructor.
 */
WorkbenchAction::~WorkbenchAction()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
WorkbenchAction::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED) {
        EventDarkLightThemeModeChanged* themeChangedEvent(dynamic_cast<EventDarkLightThemeModeChanged*>(event));
        CaretAssert(themeChangedEvent);
        
        const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode(themeChangedEvent->getDarkLightThemeMode());
        updateForDarkLightTheme(darkLightThemeMode);
    }
}

/**
 * Update the button for the given dark / light theme
 *
 * @param darkLightThemeMode
 *    The dark / light theme for the button
 */
void
WorkbenchAction::updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            setIcon(m_lightPixmap);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            setIcon(m_darkPixmap);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            setIcon(m_lightPixmap);
            break;
    }
}

/**
 * @return Icon for the given icon type and dark/light theme mode
 * @param iconType
 *    Type of icon
 * @param darkLightThemeMode
 *    The dark light theme
 * @return
 *    The pixmap that was created
 */
QPixmap
WorkbenchAction::createPixmap(const WorkbenchIconTypeEnum::Enum iconType,
                              const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    /*
     * Create the pixmap the painter for drawing in the pixmap
     */
    const int32_t width(24);
    const int32_t height(24);
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    pixmap.fill(QColor(0, 0, 0, 0));
    
    /*
     * Create a painter and fill the pixmap with
     * the background color
     */
    QSharedPointer<QPainter> painter(new QPainter(&pixmap));
    painter->setRenderHint(QPainter::Antialiasing,
                           true);
    painter->setBackgroundMode(Qt::TransparentMode);
    
    bool lightThemeFlag(false);
    QColor foregroundColor(255, 255, 255, 255);
    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            foregroundColor.setRgb(0, 0, 0);
            lightThemeFlag = true;
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            lightThemeFlag = false;
            foregroundColor.setRgb(255, 255, 255);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            foregroundColor.setRgb(0, 0, 0);
            lightThemeFlag = true;
            break;
    }
    const bool darkThemeFlag( ! lightThemeFlag);
    
    painter->setPen(foregroundColor);

    /*
     * Default height of text
     */
    const int32_t fontHeight(20);
    setFontHeight(painter,
                  fontHeight);
    
    const bool REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO  = false;
//    const bool REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_YES = true;

    switch (iconType) {
        case WorkbenchIconTypeEnum::NO_ICON:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_COLOR_BACKGROUND:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_COLOR_FOREGROUND:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_DELETE:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_DOWN:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_UP:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_ARROW:
            painter->drawLine(2, height - 2, width - 2, 2);
            painter->drawLine(width / 2, 2, width - 2, 2);
            painter->drawLine(width - 2, 2, width - 2, height / 2);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_BOX:
            painter->drawRect(1, 1, width - 2, height - 2);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_IMAGE:
        {
            /*
             * Background (sky)
             */
            int blueAsGray = qGray(25,25,255);
            if (darkThemeFlag) {
                /*
                 * Dark sky
                 */
                blueAsGray = qGray(25, 25, 128);
            }
            QColor skyColor(blueAsGray, blueAsGray, blueAsGray);
            painter->fillRect(pixmap.rect(), skyColor);
            
            
            /*
             * Terrain
             */
            int greenAsGray = qGray(0, 255, 0);
            if (darkThemeFlag) {
                greenAsGray = qGray(0, 180, 0);
            }
            QColor terrainColor(greenAsGray, greenAsGray, greenAsGray);
            painter->setBrush(terrainColor);
            painter->setPen(terrainColor);
            const int w14 = width * 0.25;
            const int h23 = height * 0.667;
            const int h34 = height * 0.75;
            QPolygon terrain;
            terrain.push_back(QPoint(1, height - 1));
            terrain.push_back(QPoint(width - 1, height - 1));
            terrain.push_back(QPoint(width - 1, h23));
            terrain.push_back(QPoint(w14 * 3, h34));
            terrain.push_back(QPoint(w14 * 2, h23));
            terrain.push_back(QPoint(w14, h34));
            terrain.push_back(QPoint(1, h23));
            terrain.push_back(QPoint(1, height - 1));
            painter->drawPolygon(terrain);
            
            /*
             * Sun / Moon
             */
            const int yellowAsGray = qGray(255, 255, 0);
            QColor sunColor(yellowAsGray, yellowAsGray, yellowAsGray);
            if (darkThemeFlag) {
                /*
                 * Replace sun with moon
                 */
                sunColor.setRgb(128, 128, 128);
            }
            painter->setBrush(sunColor);
            painter->setPen(sunColor);
            const int radius = width * 0.25;
            painter->drawEllipse(width * 0.33, height * 0.33, radius, radius);
        }
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_LINE:
            painter->drawLine(2, height - 2, width - 2, 2);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_MARKER:
            painter->drawLine(2, height - 2, width - 2, 2);
            painter->drawLine(2, 2, width - 2, height - 2);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_OVAL:
            painter->drawEllipse(1, 1, width - 1, height - 1);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYHEDRON:
        {
            CaretAssert(width  == 24);
            CaretAssert(height == 24);
            {
                QPolygon polygon;
                polygon.push_back(QPoint(12, 2));
                polygon.push_back(QPoint(6, 8));
                polygon.push_back(QPoint(12, 12));
                polygon.push_back(QPoint(18, 8));
                painter->drawPolygon(polygon);
            }
            {
                QPolygon polygon;
                polygon.push_back(QPoint(6, 8));
                polygon.push_back(QPoint(2, 18));
                polygon.push_back(QPoint(10, 20));
                polygon.push_back(QPoint(12, 12));
                painter->drawPolygon(polygon);
            }
            {
                QPolygon polygon;
                polygon.push_back(QPoint(12, 12));
                polygon.push_back(QPoint(10, 20));
                polygon.push_back(QPoint(16, 20));
                polygon.push_back(QPoint(18, 8));
                painter->drawPolygon(polygon);
            }
        }
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYGON:
            {
                const int hh(height / 2);
                const int hw(width / 2);
                QPolygon polygon;
                polygon.push_back(QPoint(2, 2));
                polygon.push_back(QPoint(width - 2, 5));
                polygon.push_back(QPoint(width - 5, height - 2));
                polygon.push_back(QPoint(hh, hw));
                polygon.push_back(QPoint(2, height - 4));
                painter->drawPolygon(polygon);
            }
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_POLYLINE:
            {
                const int hh(height / 2);
                const int hw(width / 2);
                QPolygon polyLine;
                polyLine.push_back(QPoint(2, hh));
                polyLine.push_back(QPoint(6, 3));
                polyLine.push_back(QPoint(hw + 5, height - 3));
                polyLine.push_back(QPoint(width - 2, hh - 3));
                painter->drawPolyline(polyLine);
            }
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SHAPE_TEXT:
            {
                QFont font = painter->font();
                font.setPixelSize(20);
                painter->setFont(font);
                painter->drawText(pixmap.rect(),
                                  (Qt::AlignCenter),
                                  "A");
            }
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_CHART:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::CHART));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_HISTOLOGY:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::HISTOLOGY));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_MEDIA_FILE_NAME_AND_PIXEL:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_STEREOTAXIC:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::STEREOTAXIC));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_SURFACE:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::SURFACE));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_TAB:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::TAB));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_WINDOW:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::WINDOW));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_ORIENTATION_HORIZONTAL:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_ORIENTATION_VERTICAL:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_CENTER:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_LEFT:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_BOTTOM:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_MIDDLE:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_TOP:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_STYLE_BOLD:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_STYLE_ITALIC:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_STYLE_UNDERLINE:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_ANTERIOR:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_DORSAL:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_MEDIAL:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_POSTERIOR:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REDO:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REGION:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_UNDO:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_VENTRAL:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_COLOR_BAR:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_CONSTRUCT:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_WRENCH:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::TABBAR_DATA_TOOLTIPS:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::TABBAR_FEATURES:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/features_toolbox.png",
                          "F",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_HELP:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/help.png",
                          "H",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_IDENTIFY_BRAINORDINATE:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/identify.png",
                          "I",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_INFORMATION:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/info.png",
                          "I",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_MACROS_SCROLL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/macro.png",
                          "M",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_MOVIE:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/movie.png",
                          "M",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_OVERLAYS:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/overlay_toolbox.png",
                          "O",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_SCENES_CLAP_BOARD:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/clapboard.png",
                          "S",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TABBAR_TOOLBAR:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/toolbar.png",
                          "T",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_LIGHT_BULB:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/lighting.png",
                          "L",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_RELOCK_ARROW:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/undo.png",
                          "L",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_RULER:
        {
            /*
             * Create a small, square pixmap that will contain
             * the foreground color around the pixmap's perimeter.
             */
            float width  = 24.0;
            float height = 24.0;
            float margin = 1.0;
                        
            const float leftX(margin);
            const float rightX(width - margin);
            const float centerX((leftX + rightX) / 2.0);
            const float leftCenterX((leftX + centerX) / 2.0);
            const float rightCenterX((rightX + centerX) / 2.0);
            const float y(margin + 2.0);
            const float smallTickHeight(8.0);
            const float mediumTickHeight(smallTickHeight * 1.3);
            const float bigTickHeight(smallTickHeight * 1.65);
            
            QPen pen = painter->pen();
            pen.setWidth(4);
            painter->setPen(pen);
            
            /* bottom line */
            painter->drawLine(QLineF(leftX, y,
                                     rightX, y));
            
            pen.setWidth(2);
            painter->setPen(pen);
            
            /* left tick */
            painter->drawLine(QLineF(leftX, y,
                                     leftX, y + bigTickHeight));
            
            /* right tick */
            painter->drawLine(QLineF(rightX, margin,
                                     rightX, y + bigTickHeight));
            
            /* center tick */
            painter->drawLine(QLineF(centerX, y,
                                     centerX, y + mediumTickHeight));
            
            /* left-center tick */
            painter->drawLine(QLineF(leftCenterX, y,
                                     leftCenterX, y + smallTickHeight));
            
            /* right-center tick */
            painter->drawLine(QLineF(rightCenterX, y,
                                     rightCenterX, y + smallTickHeight));
        }
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_SCISSORS:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/clipping.png",
                          "C",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/volume-crosshair-pointer.png",
                          "X",
                          darkThemeFlag,
                          REPLACE_WHITE_PIXELS_WITH_TRANSPARENT_NO);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ANTERIOR:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIRS:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ALL:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_CORONAL:
            CaretAssertToDoFatal();
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL:
            CaretAssertToDoFatal();
            break;
    }

    return pixmap;
}

/**
 * Set the font height
 * @param painter
 *    The QPainter
 * @param fontHeight
 *    Hight of the font
 */
void
WorkbenchAction::setFontHeight(QSharedPointer<QPainter>& painter,
                             const int32_t fontHeight)
{
    QFont font = painter->font();
    font.setPixelSize(fontHeight);
    painter->setFont(font);
}

/**
 * @return The current dark/light theme
 */
GuiDarkLightThemeModeEnum::Enum
WorkbenchAction::getCurrentDarkLightThemeMode() const
{
    EventDarkLightThemeModeGet themeGetEvent;
    EventManager::get()->sendEvent(themeGetEvent.getPointer());
    return themeGetEvent.getDarkLightThemeMode();
}

/**
 * Replace any white pixels in pixmap with transparent pixels
 * @param imageFileName
 *    Name of the image file from which the pixmap was read
 * @param pixmapInOut
 *    The pixmap
 */
void
WorkbenchAction::replaceWhiteWithTransparent(const AString& imageFileName,
                                             QPixmap& pixmapInOut)
{
    std::cout << "In replace white with transparent for " << imageFileName << std::endl;
    bool changedFlag(false);
    QImage image(pixmapInOut.toImage());
    const int32_t w(image.width());
    const int32_t h(image.height());
    for (int32_t x = 0; x < w; x++) {
        for (int32_t y = 0; y < h; y++) {
            QColor rgb(image.pixel(x, y));
            std::cout << "   " << rgb.red() << ", " << rgb.green() << " " << rgb.blue() << " " << rgb.alpha() << std::endl;
            if (rgb.alpha() != 0) {
                if ((rgb.red() == 255)
                    && (rgb.green() == 255)
                    && (rgb.blue() == 255)) {
                    rgb.setRgb(0, 0, 0, 0);
                    image.setPixel(x, y, rgb.rgba());
                    changedFlag = true;
                }
            }
        }
    }
    
    if (changedFlag) {
        std::cout << "Changed pixels in " << imageFileName << std::endl;
        QPixmap newPixmap;
        if (newPixmap.convertFromImage(image)) {
            pixmapInOut = newPixmap;
            std::cout << "Replaced pixels in " << imageFileName << std::endl;
        }
        else {
            CaretLogSevere("Failure to convert image to pixmap when replacing white with transparent for "
                           + imageFileName);
        }
    }
}

/**
 * Set the pixmap to the content from the given image file.  If loading the image
 * fails or conversion of image to dark mode fails, set the pixmap to the
 * alternative text.
 * @param pixmap
 *     The pixmap that is set to image or alternative text
 * @param painter
 *     The painter used for setting pixmap to text
 * @param imageFileName
 *     Name of the image file (usually a Qt resource path)
 * @param alternativeTextForPixmap
 *     If there is an image failure, set the pixmap to this text (should be one character)
 * @param darkThemeFlag
 *     True if dark theme for pixmap
 * @param replaceWhiteWithTransparentFlag
 *     If true, replace any white pixels in the image with transparent pixels
 */
void
WorkbenchAction::setPixmapIcon(QPixmap& pixmap,
                               QPainter* painter,
                               const AString& imageFileName,
                               const AString& alternativeTextForPixmap,
                               const bool darkThemeFlag,
                               const bool replaceWhiteWithTransparentFlag)
{
    bool imageSuccessFlag(false);
    QPixmap imageFilePixmap;
    if (WuQtUtilities::loadPixmap(imageFileName,
                                  imageFilePixmap)) {
        imageSuccessFlag = true;
        
        if (replaceWhiteWithTransparentFlag) {
            replaceWhiteWithTransparent(imageFileName,
                                        imageFilePixmap);
        }
        
        if (darkThemeFlag) {
            /*
             * Invert light pixmap to create dark pixmap
             */
            QImage image(imageFilePixmap.toImage());
            image.invertPixels(QImage::InvertRgb);
            if ( ! imageFilePixmap.convertFromImage(image)) {
                imageSuccessFlag = false;
                CaretLogSevere("Converting to pixmap from image failed, filename="
                               + imageFileName);
            }
        }
    }
    else {
        CaretLogSevere("Failed to load image for pixmap, filename="
                       + imageFileName);
    }
    if (imageSuccessFlag) {
        /*
         * Must end painter attached to pixmap to allow
         * assignment to pixmap
         */
        painter->end();
        pixmap = imageFilePixmap;
    }
    else {
        painter->drawText(pixmap.rect(),
                          (Qt::AlignCenter),
                          alternativeTextForPixmap);
    }

}

