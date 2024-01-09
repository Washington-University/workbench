#ifndef __CURSOR_MANAGER__H_
#define __CURSOR_MANAGER__H_

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

#include <QCursor>

#include "CaretObject.h"
#include "CursorEnum.h"

class QWidget;

namespace caret {

    class CursorManager : public CaretObject {
        
    public:
        CursorManager();
        
        virtual ~CursorManager();
        
        void setCursorForWidget(QWidget* widget,
                                const CursorEnum::Enum cursor) const;
        
    private:
        CursorManager(const CursorManager&);

        CursorManager& operator=(const CursorManager&);
        
        QCursor loadCursor(const QString& filename,
                           const int hotSpotX,
                           const int hotSpotY,
                           const Qt::CursorShape& cursorShapeIfImageLoadingFails) const;
        
        QCursor createFourArrowsCursor();
        
        QCursor createRotationCursor();
        
        QCursor createRotationCursorText(const AString& textCharacter);
        
        QCursor createDeleteCursor();
        
        QCursor m_deleteCursor;
        
        QCursor m_penCursor;
        
        QCursor m_fourArrowsCursor;
        
        QCursor m_rotationCursor;
        
        QCursor m_rotationCursor2D;
    };
    
#ifdef __CURSOR_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CURSOR_MANAGER_DECLARE__

} // namespace
#endif  //__CURSOR_MANAGER__H_
