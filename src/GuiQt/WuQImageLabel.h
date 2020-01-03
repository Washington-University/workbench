#ifndef __WU_Q_IMAGE_LABEL_H__
#define __WU_Q_IMAGE_LABEL_H__

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


#include <QLabel>

class QIcon;
class QImage;

namespace caret {

    class WuQImageLabel : public QLabel {
        
        Q_OBJECT

    public:
        WuQImageLabel();
        
        WuQImageLabel(const QImage* image,
                      const QString& text);
        
        WuQImageLabel(const QIcon* icon,
                      const QString& text);
        
        WuQImageLabel(const QIcon& icon,
                      const QString& text);
        
        virtual ~WuQImageLabel();
        
        void updateImageText(const QImage* icon,
                             const QString& text);
        
        void updateIconText(const QIcon* icon,
                            const QString& text);
        
        virtual void mouseMoveEvent(QMouseEvent* ev);
        
        virtual void mousePressEvent(QMouseEvent* ev);
        
        virtual void mouseReleaseEvent(QMouseEvent* ev);
        
        // ADD_NEW_METHODS_HERE
        
    signals:
        /**
         * Emitted if the mouse button is clicked over
         * this widget.
         */
        void clicked();
        
        /**
         * Emitted if mouse is double-clicked over
         * this widget (may also get clicked() signal.
         */
        void doubleClicked();
        
    protected:
        void mouseDoubleClickEvent(QMouseEvent *event);
    
    private:
        WuQImageLabel(const WuQImageLabel&);

        WuQImageLabel& operator=(const WuQImageLabel&);

        int m_mouseMinX;
        
        int m_mouseMaxX;
        
        int m_mouseMinY;
        
        int m_mouseMaxY;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_IMAGE_LABEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_IMAGE_LABEL_DECLARE__

} // namespace
#endif  //__WU_Q_IMAGE_LABEL_H__
