
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <cstdlib>
#include <iostream>

#define __WU_Q_IMAGE_LABEL_DECLARE__
#include "WuQImageLabel.h"
#undef __WU_Q_IMAGE_LABEL_DECLARE__

using namespace caret;


    
/**
 * \class caret::WuQImageLabel 
 * \brief Displays an icon in a pixmap and issues clicked signal.
 * \ingroup GuiQt
 *
 * Displays an icon in a QLabel.  If the icon is clicked by the user
 * the clicked signal is emitted.  This ImageLabel is intended use is
 * within a QTableWidget cell as a QButton'ish class would have a 
 * background.
 *
 */

#include <QIcon>
#include <QImage>
#include <QMouseEvent>
#include <QPixmap>

/**
 * Default constructor creates a label with no image nor text.
 */
WuQImageLabel::WuQImageLabel()
: QLabel()
{
    setAlignment(Qt::AlignCenter);
}


/**
 * Constructor constructs an image label with either the given image if the
 * image is valid (not NULL).  If the image is invalid (NULL), the text
 * will be displayed.
 *
 * @param image
 *     Image that is displayed.
 * @param text
 *     Text that is displayed if image is not valid (NULL).
 */
WuQImageLabel::WuQImageLabel(const QImage* image,
                             const QString& text)
: QLabel()
{
    updateImageText(image,
                    text);
    setAlignment(Qt::AlignCenter);
}


/**
 * Constructor constructs an image label with either the given icon if the
 * icon is valid (not NULL).  If the icon is invalid (NULL), the text 
 * will be displayed.
 *
 * @param icon
 *     Icon that is displayed.
 * @param text
 *     Text that is displayed if icon is not valid (NULL).
 */
WuQImageLabel::WuQImageLabel(const QIcon* icon,
                             const QString& text)
: QLabel()
{
    updateIconText(icon,
                   text);
    setAlignment(Qt::AlignCenter);
}

/**
 * Constructor constructs an image label with either the given icon if the
 * icon is valid (not NULL).  If the icon is invalid (NULL), the text
 * will be displayed.
 *
 * @param icon
 *     Icon that is displayed.
 * @param text
 *     Text that is displayed if icon is not valid (NULL).
 */
WuQImageLabel::WuQImageLabel(const QIcon& icon,
                             const QString& text)
: QLabel()
{
    updateIconText(&icon,
                   text);
    setAlignment(Qt::AlignCenter);
}

/**
 * Destructor.
 */
WuQImageLabel::~WuQImageLabel()
{
    
}

/*
 * Update image label with either the given icon if the
 * icon is valid (not NULL).  If the icon is invalid (NULL), the text
 * will be displayed.
 *
 * @param icon
 *     Icon that is displayed.
 * @param text
 *     Text that is displayed if icon is not valid (NULL).
 */
void
WuQImageLabel::updateIconText(const QIcon* icon,
                              const QString& text)
{
    if (icon != NULL) {
        setPixmap(icon->pixmap(16));
    }
    else {
        setText(text);
    }
}

/*
 * Update image label with either the given image if the
 * image is valid (not NULL).  If the image is invalid (NULL), the text
 * will be displayed.
 *
 * @param image
 *     Image that is displayed.
 * @param text
 *     Text that is displayed if icon is not valid (NULL).
 */
void
WuQImageLabel::updateImageText(const QImage* image,
                               const QString& text)
{
    if (image != NULL) {
        setPixmap(QPixmap::fromImage(*image));
    }
    else {
        setPixmap(QPixmap());
    }
    setText(text);
}

/**
 * Called when the mouse is moved.
 *
 * @param ev
 *    The mouse event.
 */
void
WuQImageLabel::mouseMoveEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::NoButton) {
        if (ev->buttons() == Qt::LeftButton) {
            const int x = ev->x();
            const int y = ev->y();
            
            if (x < m_mouseMinX) m_mouseMinX = x;
            if (x > m_mouseMaxX) m_mouseMaxX = x;
            if (y < m_mouseMinY) m_mouseMinY = y;
            if (y > m_mouseMaxY) m_mouseMaxY = y;
        }
    }
}

/**
 * Called when the mouse button is pressed.
 *
 * @param ev
 *    The mouse event.
 */
void
WuQImageLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int dx = std::abs(m_mouseMaxX - m_mouseMinX);
        const int dy = std::abs(m_mouseMaxY - m_mouseMinY);
        
        const int tolerance = 5;
        
        if ((dx < tolerance)
            && (dy < tolerance)) {
            emit doubleClicked();
        }
    }
}

/**
 * Called when the mouse button is pressed.
 *
 * @param ev
 *    The mouse event.
 */
void
WuQImageLabel::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        m_mouseMinX = ev->x();
        m_mouseMaxX = ev->x();
        m_mouseMinY = ev->y();
        m_mouseMaxY = ev->y();
    }
}

/**
 * Called when the mouse button is released.
 *
 * @param ev
 *    The mouse event.
 */
void
WuQImageLabel::mouseReleaseEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton) {
        const int dx = std::abs(m_mouseMaxX - m_mouseMinX);
        const int dy = std::abs(m_mouseMaxY - m_mouseMinY);
        
        const int tolerance = 5;
        
        if ((dx < tolerance)
            && (dy < tolerance)) {
            emit clicked();
        }
    }
}


