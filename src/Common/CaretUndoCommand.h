#ifndef __CARET_UNDO_COMMAND_H__
#define __CARET_UNDO_COMMAND_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

/*
 * NOTE: The Qt license is included because the CaretUndoStack API
 * including the method comments is copied from QUndoStack.
 */

/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtOpenGL module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/


#include "CaretObject.h"



namespace caret {

    class CaretUndoCommand : public CaretObject {
        
    public:
        CaretUndoCommand();
        
        virtual ~CaretUndoCommand();
        
        AString getDescription() const;
        
        void setDescription(const AString& description);
        
        bool isMergeEnabled() const;
        
        void setMergeEnabled(const bool mergeStatus);
        
        virtual bool mergeWith(const CaretUndoCommand* command);

        int32_t getWindowIndex() const;
        
        void setWindowIndex(const int32_t windowIndex);
        
        /**
         * Operation that "redoes" the command.
         */
        virtual void redo() = 0;
        
        /**
         * Operation that "undoes" the command.
         */
        virtual void undo() = 0;

        // ADD_NEW_METHODS_HERE

    private:
        CaretUndoCommand(const CaretUndoCommand&);

        CaretUndoCommand& operator=(const CaretUndoCommand&);
        
        AString m_description;
        
        int32_t m_windowIndex;
        
        bool m_mergeFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_UNDO_COMMAND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_UNDO_COMMAND_DECLARE__

} // namespace
#endif  //__CARET_UNDO_COMMAND_H__
