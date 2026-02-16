
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
#include "WorkbenchIconTypeLoader.h"
#undef __WORKBENCH_ACTION_DECLARE__

#include <QEvent>
#include <QImage>
#include <QPainter>

#include "AnnotationCoordinateSpaceEnum.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventManager.h"
#include "MathFunctions.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WorkbenchIconTypeLoader 
 * \brief Extends QAction to work with light/dark mode for the icon
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WorkbenchIconTypeLoader::WorkbenchIconTypeLoader()
: CaretObject()
{
//    m_darkPixmap = createPixmapForIconType(iconType,
//                                           GuiDarkLightThemeModeEnum::DARK);
//    m_lightPixmap = createPixmapForIconType(iconType,
//                                            GuiDarkLightThemeModeEnum::LIGHT);

//    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
}

/**
 * Destructor.
 */
WorkbenchIconTypeLoader::~WorkbenchIconTypeLoader()
{
}

/**
 * Load and return the pixmap for the given icon type
 * @param darkLightThemeMode
 *    The dark light theme
 * @param iconType
 *    Type of icon
 * @return
 *    Pixmap loaded for the icon
 */
QPixmap
WorkbenchIconTypeLoader::loadPixmapForIconType(const WorkbenchIconTypeEnum::Enum iconType,
                                               const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    return createPixmapForIconType(iconType,
                                   darkLightThemeMode);
}

/**
 * Load and return the pixmap for the given icon type
 * @param darkLightThemeMode
 *    The dark light theme
 * @param iconType
 *    Type of icon
 * @return
 *    Pixmap loaded for the icon
 */
QPixmap
WorkbenchIconTypeLoader::loadPixmapForIconTypeForCurrrentDarkLightTheme(const WorkbenchIconTypeEnum::Enum iconType)
{
    EventDarkLightThemeModeGet themeGetEvent;
    EventManager::get()->sendEvent(themeGetEvent.getPointer());

    return createPixmapForIconType(iconType,
                                   themeGetEvent.getDarkLightThemeMode());
}

/**
 * Create a pixmap of the given size and a painter with origin in the given location
 * @param width
 *    Width for the pixmap
 * @param height
 *    Height for the pixmap
 * @param origin
 *    Location of origin in the pixmap
 * @param fontHeight
 *    If greater than zero, sets font height
 * @param darkLightThemeMode
 *    The dark light theme
 * @param pixmap
 *    The pixmap (output)
 * @param painter
 *    The painter (output)
 */
void
WorkbenchIconTypeLoader::createPixmapPainter(const int32_t width,
                                     const int32_t height,
                                     const Origin origin,
                                     const int32_t fontHeight,
                                     const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode,
                                     QPixmap& pixmapOut,
                                     QSharedPointer<QPainter>& painterOut)
{
    /*
     * If painter is valid, we need to end it before
     * recreating it and before recreating the pixmap
     */
    if (painterOut) {
        painterOut->end();
    }
    
    pixmapOut = QPixmap(width,
                        height);
    
    pixmapOut.fill(QColor(0, 0, 0, 0));
    
    painterOut.reset(new QPainter(&pixmapOut));
    CaretAssert(painterOut);
    
    switch (origin) {
        case Origin::CENTER:
            /*
             * Note: QPainter has its origin at the top left.
             * Using a negative for the Y-scale value will
             * move the origin to the bottom.
             */
            painterOut->translate(pixmapOut.width() / 2.0,
                                  pixmapOut.height() / 2.0);
            painterOut->scale(1.0,
                              -1.0);
            break;
        case Origin::TOP_LEFT:
            break;
    }
    
    painterOut->setRenderHint(QPainter::Antialiasing,
                              true);
    painterOut->setBackgroundMode(Qt::TransparentMode);
    
    QColor foregroundColor(255, 255, 255, 255);
    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            foregroundColor.setRgb(0, 0, 0, 255);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            foregroundColor.setRgb(255, 255, 255);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            foregroundColor.setRgb(0, 0, 0, 255);
            break;
    }
    
    painterOut->setPen(foregroundColor);
    
    /*
     * Default height of text
     */
    if (fontHeight > 0) {
        setFontHeight(painterOut,
                      fontHeight);
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
WorkbenchIconTypeLoader::createPixmapForIconType(const WorkbenchIconTypeEnum::Enum iconType,
                                         const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    /*
     * Create a pixmap and attach a painter to the pixmap
     * with the origin at the top left
     */
    QPixmap pixmap;
    QSharedPointer<QPainter> painter;
    int32_t width(24);
    int32_t height(24);
    const int32_t fontHeight20(20);
    const int32_t fontHeightDefault(-1);
    createPixmapPainter(width,
                        height,
                        Origin::TOP_LEFT,
                        fontHeight20,
                        darkLightThemeMode,
                        pixmap,
                        painter);
    CaretAssert(pixmap.width() > 0);
    CaretAssert(pixmap.height() > 0);
    CaretAssert(painter);
    
    QColor textColor(255, 255, 255, 216);  /* on macOS alpha for text is 216 */
    bool lightThemeFlag(false);
    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            lightThemeFlag = true;
            textColor.setRgb(0, 0, 0, 216);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            lightThemeFlag = false;
            textColor.setRgb(255, 255, 255, 216);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            lightThemeFlag = true;
            textColor.setRgb(0, 0, 0, 216);
            break;
    }
    const bool darkThemeFlag( ! lightThemeFlag);

    const int32_t width12(12);
    const int32_t height12(12);
    const int32_t width24(24);
    const int32_t height30(30);

    switch (iconType) {
        case WorkbenchIconTypeEnum::NO_ICON:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_DELETE:
        {
            /* trash can */
            painter->drawLine(4, 6, 4, 22);
            painter->drawLine(4, 22, 20, 22);
            painter->drawLine(20, 22, 20, 6);
            
            /* trash can lines */
            painter->drawLine(12, 8, 12, 20);
            painter->drawLine(8,  8,  8, 20);
            painter->drawLine(16, 8, 16, 20);
            
            /* trash can lid and handle */
            painter->drawLine(2, 6, 22, 6);
            painter->drawLine(8, 6, 8, 2);
            painter->drawLine(8, 2, 16, 2);
            painter->drawLine(16, 2, 16, 6);
        }
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_DOWN:
            createArrowPixmap(pixmap,
                              painter.get(),
                              ArrowType::DOWN);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_UP:
            createArrowPixmap(pixmap,
                              painter.get(),
                              ArrowType::UP);
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
                painter->setPen(textColor);
                painter->drawText(pixmap.rect(),
                                  (Qt::AlignCenter),
                                  "A");
            }
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_CHART:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::CHART));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_HISTOLOGY:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::HISTOLOGY));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_MEDIA_FILE_NAME_AND_PIXEL:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_STEREOTAXIC:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::STEREOTAXIC));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_SURFACE:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::SURFACE));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_TAB:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::TAB));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_NEW_SPACE_WINDOW:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(AnnotationCoordinateSpaceEnum::WINDOW));
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ORIENTATION_HORIZONTAL:
            createPixmapPainter(width24,
                                height30,
                                Origin::TOP_LEFT,
                                fontHeightDefault,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createTextOrientationPixmap(pixmap,
                                        painter.get(),
                                        AnnotationTextOrientationEnum::HORIZONTAL);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ORIENTATION_VERTICAL:
            createPixmapPainter(width24,
                                height30,
                                Origin::TOP_LEFT,
                                fontHeightDefault,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createTextOrientationPixmap(pixmap,
                                        painter.get(),
                                        AnnotationTextOrientationEnum::STACKED);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_CENTER:
            createPixmapPainter(width12,
                                height12,
                                Origin::TOP_LEFT,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createHorizontalAlignmentPixmap(pixmap,
                                            painter.get(),
                                            AnnotationTextAlignHorizontalEnum::CENTER);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_LEFT:
            createPixmapPainter(width12,
                                height12,
                                Origin::TOP_LEFT,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createHorizontalAlignmentPixmap(pixmap,
                                            painter.get(),
                                            AnnotationTextAlignHorizontalEnum::LEFT);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT:
            createPixmapPainter(width12,
                                height12,
                                Origin::TOP_LEFT,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createHorizontalAlignmentPixmap(pixmap,
                                            painter.get(),
                                            AnnotationTextAlignHorizontalEnum::RIGHT);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_BOTTOM:
            createPixmapPainter(width12,
                                height12,
                                Origin::TOP_LEFT,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createVerticalAlignmentPixmap(pixmap,
                                          painter.get(),
                                          AnnotationTextAlignVerticalEnum::BOTTOM);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_MIDDLE:
            createPixmapPainter(width12,
                                height12,
                                Origin::TOP_LEFT,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createVerticalAlignmentPixmap(pixmap,
                                          painter.get(),
                                          AnnotationTextAlignVerticalEnum::MIDDLE);
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_TOP:
            createPixmapPainter(width12,
                                height12,
                                Origin::TOP_LEFT,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            createVerticalAlignmentPixmap(pixmap,
                                          painter.get(),
                                          AnnotationTextAlignVerticalEnum::TOP);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_ANTERIOR:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-anterior.png",
                          "A",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_DORSAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-dorsal.png",
                          "D",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-left.png",
                          "L",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT_LATERAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-left-lateral.png",
                          "LL",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT_MEDIAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-left-medial.png",
                          "LM",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_POSTERIOR:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-posterior.png",
                          "P",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REDO:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/redo.png",
                          "R",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REGION:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/select-region.png",
                          "R",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_RIGHT:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-right.png",
                          "R",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_RIGHT_LATERAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-right-lateral.png",
                          "RL",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_RIGHT_MEDIAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-right-medial.png",
                          "RM",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_UNDO:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/undo.png",
                          "U",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_VENTRAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-ventral.png",
                          "V",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_COLOR_BAR:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/LayersPanel/colorbar.png",
                          "F",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_CONSTRUCT:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/LayersPanel/construction.png",
                          "F",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_WRENCH:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/LayersPanel/wrench.png",
                          "F",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_DELETE:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/SpecFileDialog/delete_icon.png",
                          "D",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_LOAD:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/SpecFileDialog/load_icon.png",
                          "L",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_OPTIONS:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/SpecFileDialog/options_icon.png",
                          "O",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::SPEC_FILE_DIALOG_RELOAD:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/SpecFileDialog/reload_icon.png",
                          "R",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_DATA_TOOLTIPS:
        {
            const int32_t pixmapWidth(32);
            const int32_t pixmapHeight(32);
            createPixmapPainter(pixmapWidth,
                                pixmapHeight,
                                Origin::CENTER,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            const int leftX(-14);
            const int rightX(14);
            const int bottomY(-8);
            const int topY(14);
            const int tipLeftX(-6);
            const int tipRightX(6);
            const int tipY(-14);
            const int tipX(0);
            
            QPen pen(painter->pen());
            pen.setWidth(2);
            painter->setPen(pen);
            
            /*
             * Outline of icon
             */
            QPolygon polygon;
            polygon.push_back(QPoint(leftX, topY));
            polygon.push_back(QPoint(leftX, bottomY));
            polygon.push_back(QPoint(tipLeftX, bottomY));
            polygon.push_back(QPoint(tipX, tipY));
            polygon.push_back(QPoint(tipRightX, bottomY));
            polygon.push_back(QPoint(rightX, bottomY));
            polygon.push_back(QPoint(rightX, topY));
            painter->drawPolygon(polygon);
            
            /*
             * Horizontal lines inside outline
             */
            const int lineLeftX(-8);
            const int lineRightX(8);
            const int lineOneY(6);
            const int lineTwoY(0);
            painter->drawLine(lineLeftX, lineOneY, lineRightX, lineOneY);
            painter->drawLine(lineLeftX, lineTwoY, lineRightX, lineTwoY);
        }
            break;
        case WorkbenchIconTypeEnum::TABBAR_FEATURES:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/features_toolbox.png",
                          "F",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_HELP:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/help.png",
                          "H",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_IDENTIFY_BRAINORDINATE:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/identify.png",
                          "I",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_INFORMATION:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/info.png",
                          "I",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_MACROS_SCROLL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/macro.png",
                          "M",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_MOVIE:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/movie.png",
                          "M",
                          darkThemeFlag,
                          PixelModification::REPLACE_BLACK_WITH_DARK_GRAY);
            break;
        case WorkbenchIconTypeEnum::TABBAR_OVERLAYS:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/overlay_toolbox.png",
                          "O",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_SCENES_CLAP_BOARD:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/clapboard.png",
                          "S",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TABBAR_TOOLBAR:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/toolbar.png",
                          "T",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_LIGHT_BULB:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/lighting.png",
                          "L",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_RELOCK_ARROW:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/undo.png",
                          "L",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_RULER:
        {
            /*
             * Create a small, square pixmap that will contain
             * the foreground color around the pixmap's perimeter.
             */
            float width  = 24.0;
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
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS:
        {
            createPixmapPainter(width,
                                height,
                                Origin::CENTER,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            const int pixmapSize = pixmap.width();
            const int halfSize = pixmapSize / 2;
            
            const int startXY = 3;
            const int endXY   = 8;
            QPen pen(painter->pen());
            pen.setWidth(2);
            painter->setPen(pen);
            const int tx(-3);
            const int ty(3);
            painter->translate(tx, ty);
            painter->drawLine(-startXY, 0, -endXY, 0);
            painter->drawLine( startXY, 0,  endXY, 0);
            painter->drawLine(0, -startXY, 0, -endXY);
            painter->drawLine(0,  startXY, 0,  endXY);
            painter->translate(-tx, -ty);
            
            const int tipX(3);
            const int tipY(-3);
            const int tailX(halfSize);
            const int tailY(-halfSize);
            painter->drawLine(tipX, tipY, tailX, tailY);
            
            const int headLength(3);
            painter->drawLine(tipX, tipY, tipX + headLength, tipY);
            painter->drawLine(tipX, tipY, tipX, tipY - headLength);
        }
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_AXIAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-plane-axial.png",
                          "A",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIRS:
        {
            int32_t width(22);
            int32_t height(22);
            createPixmapPainter(width,
                                height,
                                Origin::CENTER,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);

            const int startXY = 4;
            const int endXY   = 10;
            QPen pen(painter->pen());
            pen.setWidth(2);
            pen.setColor(textColor);
            painter->setPen(pen);
            painter->drawLine(-startXY, 0, -endXY, 0);
            painter->drawLine( startXY, 0,  endXY, 0);
            painter->drawLine(0, -startXY, 0, -endXY);
            painter->drawLine(0,  startXY, 0,  endXY);
        }
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS:
        {
            int32_t width(22);
            int32_t height(22);
            createPixmapPainter(width,
                                height,
                                Origin::TOP_LEFT,
                                fontHeight20,
                                darkLightThemeMode,
                                pixmap,
                                painter);
            
            const float pixmapSize(pixmap.width());
            const float fullXY = pixmapSize;
            const float halfXY = fullXY / 2.0f;
            
            const float boxWH = 8.0f;
            const float halfBoxWH = boxWH / 2.0f;
            
            QFont font = painter->font();
            font.setPixelSize(12);
            painter->setFont(font);
            painter->setPen(textColor);
            
            const float edgeOffset = 1.0f;
            painter->drawText(QRectF(edgeOffset, halfXY - halfBoxWH, boxWH, boxWH),
                              "L", QTextOption(Qt::AlignCenter));
            painter->drawText(QRectF(fullXY - boxWH - edgeOffset + 1, halfXY - halfBoxWH - 1, boxWH, boxWH),
                              "R", QTextOption(Qt::AlignCenter));
            
            painter->drawText(QRectF(halfXY - halfBoxWH, edgeOffset, boxWH, boxWH),
                              "A", QTextOption(Qt::AlignCenter));
            painter->drawText(QRectF(halfXY - halfBoxWH + 1, fullXY - boxWH - edgeOffset, boxWH, boxWH),
                              "P", QTextOption(Qt::AlignCenter));
        }
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ALL:
            painter->setPen(textColor);
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              VolumeSliceViewPlaneEnum::toGuiNameAbbreviation(VolumeSliceViewPlaneEnum::ALL));
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_CORONAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-plane-coronal.png",
                          "A",
                          darkThemeFlag,
                          PixelModification::NONE);
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL:
            setPixmapIcon(pixmap,
                          painter.get(),
                          ":/ToolBar/view-plane-parasagittal.png",
                          "A",
                          darkThemeFlag,
                          PixelModification::NONE);
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
WorkbenchIconTypeLoader::setFontHeight(QSharedPointer<QPainter>& painter,
                             const int32_t fontHeight)
{
    QFont font = painter->font();
    font.setPixelSize(fontHeight);
    painter->setFont(font);
}

/**
 * Replace any white pixels in pixmap with transparent pixels
 * @param imageFileName
 *    Name of the image file from which the pixmap was read
 * @param pixmapInOut
 *    The pixmap
 */
void
WorkbenchIconTypeLoader::replacePixmapPixelColor(const AString& imageFileName,
                                         QPixmap& pixmapInOut,
                                         const QColor& findColor,
                                         const QColor& replaceColor)
{
    bool changedFlag(false);
    QImage image(pixmapInOut.toImage());
    const int32_t w(image.width());
    const int32_t h(image.height());
    for (int32_t x = 0; x < w; x++) {
        for (int32_t y = 0; y < h; y++) {
            QColor color(image.pixelColor(x, y));
            if (color.alpha() != 0) {
                if ((color.red() == findColor.red())
                    && (color.green() == findColor.green())
                    && (color.blue() == findColor.blue())) {
                    image.setPixelColor(x, y, replaceColor.rgba());
                    changedFlag = true;
                }
            }
        }
    }
    
    if (changedFlag) {
        QPixmap newPixmap;
        if (newPixmap.convertFromImage(image)) {
            pixmapInOut = newPixmap;
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
 * @param pixelModification
 *     Optional pixel modification
 */
void
WorkbenchIconTypeLoader::setPixmapIcon(QPixmap& pixmap,
                               QPainter* painter,
                               const AString& imageFileName,
                               const AString& alternativeTextForPixmap,
                               const bool darkThemeFlag,
                               const PixelModification pixelModification)
{
    CaretAssert(pixmap.width() > 0);
    CaretAssert(painter);
    
    bool imageSuccessFlag(false);
    QPixmap imageFilePixmap;
    if (WuQtUtilities::loadPixmap(imageFileName,
                                  imageFilePixmap)) {
        imageSuccessFlag = true;
        
        switch (pixelModification) {
            case PixelModification::NONE:
                break;
            case PixelModification::REPLACE_BLACK_WITH_DARK_GRAY:
                replacePixmapPixelColor(imageFileName,
                                        imageFilePixmap,
                                        QColor(0, 0, 0),
                                        QColor(55, 55, 55, 55));
                break;
        }
//        if (replaceWhiteWithTransparentFlag) {
//            replaceWhiteWithTransparent(imageFileName,
//                                        imageFilePixmap);
//        }
        
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

/**
 * Create a horizontal alignment pixmap.
 *
 * @param pixmap
 *    Pixmap that is painted
 * @param painter
 *    The painter
 * @param horizontalAlignment
 *    The horizontal alignment.
 */
void
WorkbenchIconTypeLoader::createHorizontalAlignmentPixmap(QPixmap& pixmap,
                                                 QPainter* painter,
                                                 const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment)
{
    float width    = pixmap.width();
    float height   = pixmap.height();
    const int32_t numLines = 3;
    
    const qreal margin          = width * 0.05;
    const qreal longLineLength  = width - (margin * 2.0);
    const qreal shortLineLength = width / 2.0;
    const qreal yStep = MathFunctions::round(height / (numLines + 1));
    
    for (int32_t i = 1; i <= numLines; i++) {
        const qreal lineLength = (((i % 2) == 0)
                                  ? shortLineLength
                                  : longLineLength);
        const qreal y = yStep * i;
        
        qreal xStart = 0.0;
        qreal xEnd   = width;
        
        switch (horizontalAlignment) {
            case AnnotationTextAlignHorizontalEnum::CENTER:
                xStart = (width - lineLength) / 2.0;
                xEnd   = xStart + lineLength;
                break;
            case AnnotationTextAlignHorizontalEnum::LEFT:
                xStart = margin;
                xEnd   = xStart + lineLength;
                break;
            case AnnotationTextAlignHorizontalEnum::RIGHT:
                xEnd   = width - margin;
                xStart = xEnd - lineLength;
                break;
        }
        
        painter->drawLine(QLineF(xStart,
                                 y,
                                 xEnd,
                                 y));
    }
}

/**
 * Create a vertical alignment pixmap.
 *
 * @param pixmap
 *    Pixmap that is painted
 * @param painter
 *    The painter
 * @param verticalAlignment
 *    The vertical alignment.
 */
void
WorkbenchIconTypeLoader::createVerticalAlignmentPixmap(QPixmap& pixmap,
                                               QPainter* painter,
                                               const AnnotationTextAlignVerticalEnum::Enum verticalAlignment)
{
    float width    = pixmap.width();
    float height   = pixmap.height();
    
    const qreal margin          = width * 0.05;
    
    float yStep = 3.0;
    float y1 = 0.0;
    float y2 = 0.0;
    switch (verticalAlignment) {
        case AnnotationTextAlignVerticalEnum::BOTTOM:
            y1 = (height - 1 - yStep);
            y2 = y1 + yStep;
            break;
        case AnnotationTextAlignVerticalEnum::MIDDLE:
            y1 = MathFunctions::round((height / 2.0) - (yStep / 2.0));
            y2 = y1 + yStep;
            break;
        case AnnotationTextAlignVerticalEnum::TOP:
            y1 = yStep;
            y2 = y1 + yStep;
            break;
    }
    
    const float xStart = margin;
    const float xEnd   = width - (margin * 2.0);
    
    painter->drawLine(QLineF(xStart, y1,
                             xEnd,   y1));
    painter->drawLine(QLineF(xStart, y2,
                             xEnd,   y2));
}

/**
 * Draw pixmap for text orientation
 *
 * @param pixmap
 *    Pixmap that is painted
 * @param painter
 *    The painter
 * @param orientation
 *     The horizontal alignment.
 */
void
WorkbenchIconTypeLoader::createTextOrientationPixmap(QPixmap& pixmap,
                                             QPainter* painter,
                                             const AnnotationTextOrientationEnum::Enum orientation)
{
//    /*
//     * Create a small, square pixmap that will contain
//     * the foreground color around the pixmap's perimeter.
//     */
//    float width  = 24.0;
//    float height = 30.0;
    
    switch (orientation) {
        case AnnotationTextOrientationEnum::HORIZONTAL:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              "ab");
            
            break;
        case AnnotationTextOrientationEnum::STACKED:
            painter->drawText(pixmap.rect(),
                              (Qt::AlignCenter),
                              "a\nb");
            break;
    }
}

/**
 * Draw pixmap for annotation line arrow
 *
 * @param pixmap
 *    Pixmap that is painted
 * @param painter
 *    The painter
 * @param arrowType
 *     Type of arrow
 */
void
WorkbenchIconTypeLoader::createArrowPixmap(QPixmap& pixmap,
                                   QPainter* painter,
                                   const ArrowType arrowType)
{
    const float width(pixmap.width());
    const float height(pixmap.height());
    
    const bool fillShapeFlag = false;
    if (fillShapeFlag) {
        QBrush brush = painter->brush();
        brush.setColor(painter->pen().color());
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);
    }
    
    const float percentage = 0.10f;
    const float left   = width  * percentage;
    const float right  = width  * (1.0 - percentage);
    const float bottom = height * percentage;
    const float top    = height * (1.0 - percentage);
    const float centerX = width * 0.5;
    QPolygonF triangle;
    switch (arrowType) {
        case ArrowType::DOWN:
            triangle.push_back(QPointF(right, top));
            triangle.push_back(QPointF(left, top));
            triangle.push_back(QPointF(centerX, bottom));
            break;
        case ArrowType::UP:
            triangle.push_back(QPointF(left, bottom));
            triangle.push_back(QPointF(right, bottom));
            triangle.push_back(QPointF(centerX, top));
            break;
    }
    painter->drawPolygon(triangle);
}

