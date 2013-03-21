
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

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
#include <QMouseEvent>

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


