
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

#define __WU_Q_TOOL_BUTTON_DECLARE__
#include "WuQToolButton.h"
#undef __WU_Q_TOOL_BUTTON_DECLARE__

#include <QEvent>
#include <QImage>
#include <QPainter>

#include "AnnotationCoordinateSpaceEnum.h"
#include "CaretAssert.h"
#include "EventDarkLightThemeModeChanged.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WuQToolButton 
 * \brief Extends QToolButton to work with light/dark mode and fixes macOS style
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param iconType
 *    Type of icon for tool button
 * @param parent
 *    Parent widget
 * @param checkableToolButtonFlag
 *    Toolbutton is checkable
 * @param menuInToolButtonFlag
 *    Toolbutton will have a menu
 */
WuQToolButton::WuQToolButton(const WorkbenchIconTypeEnum::Enum iconType,
                             QWidget* parent)
: QToolButton(parent)
{
    static bool firstFlag(true);
    if (firstFlag) {
        firstFlag = false;
    }

    m_darkPixmap = createPixmap(iconType,
                                 GuiDarkLightThemeModeEnum::DARK);
    m_lightPixmap = createPixmap(iconType,
                                 GuiDarkLightThemeModeEnum::LIGHT);
    
    QAction* action(new QAction());
    setDefaultAction(action);
    
    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
}

/**
 * Destructor.
 */
WuQToolButton::~WuQToolButton()
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
WuQToolButton::receiveEvent(Event* event)
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
WuQToolButton::updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            defaultAction()->setIcon(m_lightPixmap);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            defaultAction()->setIcon(m_darkPixmap);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            defaultAction()->setIcon(m_lightPixmap);
            break;
    }
    
#ifdef CARET_OS_MACOSX
    updateButtonForMacOS(darkLightThemeMode);
#endif
}

/**
 * Update the button for MacOS that does not properly decorate the button
 * @param darkLightThemeMode
 *    The dark / light theme for the button
 */
void
WuQToolButton::updateButtonForMacOS(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    QColor backgroundColor(52, 52, 52, 255);
    QColor borderColor(85, 85, 85, 255);
    QColor checkedPressedColor(74, 74, 74, 255);

    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            backgroundColor.setRgb(255, 255, 255);
            borderColor.setRgb(196, 196, 196);
            checkedPressedColor.setRgb(222, 222, 222);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            backgroundColor.setRgb(52, 52, 52);
            borderColor.setRgb(85, 85, 85);
            checkedPressedColor.setRgb(74, 74, 74);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            backgroundColor.setRgb(255, 255, 255);
            borderColor.setRgb(196, 196, 196);
            checkedPressedColor.setRgb(222, 222, 222);
            break;
    }

    QString toolButtonStyleSheet(" QToolButton { "
                                 "   background: " + backgroundColor.name() + "; ");
    toolButtonStyleSheet.append("   border-style: solid; "
                                "   border-width: 1px; "
                                "   border-color: " + borderColor.name() + "; "
                                "   padding-top:    2px; "
                                "   padding-bottom: 2px; "
                                "   padding-right:  3px; "
                                "   padding-left:   3px; ");
    toolButtonStyleSheet.append(" } ");
    
    /*
     * Background color when button is "checked"
     */
    toolButtonStyleSheet.append(" QToolButton:checked { "
                                "   background-color: " + checkedPressedColor.name() + "; "
                                " } ");
    /*
     * Background color when button is "pressed"
     */
    toolButtonStyleSheet.append(" QToolButton:pressed { "
                                "   background-color: " + checkedPressedColor.name() + "; "
                                " } ");
    
    this->setStyleSheet(toolButtonStyleSheet);
}

/**
 * Called to show the toolbutton (overrides parent)
 * @param event
 *   The show event
 */
void
WuQToolButton::showEvent(QShowEvent *event)
{
    QToolButton::showEvent(event);
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
WuQToolButton::createPixmap(const WorkbenchIconTypeEnum::Enum iconType,
                            const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    /*
     * Create the pixmap the painter for drawing in the pixmap
     */
    const int32_t width(24);
    const int32_t height(24);
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainter(this,
                                                                                pixmap,
                                                                                WuQtUtilities::TransparentBackground);

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
    painter->setPen(foregroundColor);

    /*
     * Default height of text
     */
    const int32_t fontHeight(20);
    setFontHeight(painter,
                  fontHeight);
    
    QIcon icon;
    
    switch (iconType) {
        case WorkbenchIconTypeEnum::NO_ICON:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_COLOR_BACKGROUND:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_COLOR_FOREGROUND:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_DELETE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_DOWN:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_LINE_ARROW_UP:
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
            if ( ! lightThemeFlag) {
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
            if ( ! lightThemeFlag) {
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
            if ( ! lightThemeFlag) {
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
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_ORIENTATION_VERTICAL:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_CENTER:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_LEFT:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_HORIZ_RIGHT:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_BOTTOM:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_MIDDLE:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_ALIGN_VERT_TOP:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_STYLE_BOLD:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_STYLE_ITALIC:
            break;
        case WorkbenchIconTypeEnum::ANNOTATION_TEXT_STYLE_UNDERLINE:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_ANTERIOR:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_DORSAL:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_LEFT:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_MEDIAL:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_POSTERIOR:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REDO:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_REGION:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_UNDO:
            break;
        case WorkbenchIconTypeEnum::ORIENTATION_VENTRAL:
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_COLOR_BAR:
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_CONSTRUCT:
            break;
        case WorkbenchIconTypeEnum::OVERLAY_TOOLBOX_WRENCH:
            break;
        case WorkbenchIconTypeEnum::TABBAR_BRAIN_TIPS:
            break;
        case WorkbenchIconTypeEnum::TABBAR_DATA_TOOLTIPS:
            break;
        case WorkbenchIconTypeEnum::TABBAR_FEATURES:
            break;
        case WorkbenchIconTypeEnum::TABBAR_INFORMATION:
            break;
        case WorkbenchIconTypeEnum::TABBAR_MACROS_SCROLL:
            break;
        case WorkbenchIconTypeEnum::TABBAR_MOVIE:
            break;
        case WorkbenchIconTypeEnum::TABBAR_OVERLAYS:
            break;
        case WorkbenchIconTypeEnum::TABBAR_QUESTION_MARK:
            break;
        case WorkbenchIconTypeEnum::TABBAR_SCENES_CLAP_BOARD:
            break;
        case WorkbenchIconTypeEnum::TABBAR_TOOLBAR:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_LIGHT_BULB:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_RELOCK_ARROW:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_RULER:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_MISC_SCISSORS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_SLICE_INDICES_MOVE_CROSSHAIRS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ANTERIOR:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIRS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_CROSSHAIR_LABELS:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_ALL:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_CORONAL:
            break;
        case WorkbenchIconTypeEnum::TOOLBAR_VOLUME_SLICE_PLANE_PARASAGITTAL:
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
WuQToolButton::setFontHeight(QSharedPointer<QPainter>& painter,
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
WuQToolButton::getCurrentDarkLightThemeMode() const
{
    EventDarkLightThemeModeGet themeGetEvent;
    EventManager::get()->sendEvent(themeGetEvent.getPointer());
    return themeGetEvent.getDarkLightThemeMode();
}

