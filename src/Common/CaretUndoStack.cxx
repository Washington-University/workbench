
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
#define __CARET_UNDO_STACK_DECLARE__
#include "CaretUndoStack.h"
#undef __CARET_UNDO_STACK_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretUndoCommand.h"

using namespace caret;


    
/**
 * \class caret::CaretUndoStack 
 * \brief An abstract class for an undo stack implementation.,
 * \ingroup Common
 *
 * Stack for undo and redo of commands.
 * 
 * Design is based off Qt's QUndoStack.  
 *
 * While using QUndoStack would be preferred, it cannot be used because:
 * (1) It is located in Qt's GUI module which is not accessible to 
 * Caret's "lower level" modules.
 * (2) QUndoStack is targeted to text documents.
 *
 * An undo stack maintains a stack of commands that have been 
 * applied to a document.
 *
 * New commands are pushed on the stack using push(). Commands can be undone 
 * and redone using undo() and redo().
 *
 * CaretUndoStack keeps track of the current command. This is the command which 
 * will be executed by the next call to redo(). The index of this command is 
 * returned by index().  If the top-most command on the stack has already
 * been redone, index() is equal to count().
 */

/**
 * Constructs an empty undo stack in the "clean" state.
 */
CaretUndoStack::CaretUndoStack()
: CaretObject()
{
    m_undoLimit      =  0;
    m_undoStackIndex =  0;
}

/**
 * Destroys the undo stack, deleting any commands that are on it.
 */
CaretUndoStack::~CaretUndoStack()
{
    clear();
}

/**
 * Returns true if there is a command available for redo; 
 * otherwise returns false.
 *
 * This function returns false if the stack is empty or if the top 
 * command on the stack has already been redone.
 *
 * While the Qt documentation states "Synonymous with index() == count()"
 * the actual QUndoStack code indicates the comment should be
 * "Synonymous with index() < count()".
 */
bool
CaretUndoStack::canRedo() const
{
    if (m_undoStack.empty()) {
        return false;
    }
    
    if (index() < count()) {
        return true;
    }
    
    return false;
}

/**
 * Returns true if there is a command available for undo; otherwise 
 * returns false.
 * 
 * This function returns false if the stack is empty, or if the bottom
 * command on the stack has already been undone.
 * 
 * While the Qt documentation staties "Synonymous with index() == 0", viewing
 * the actual QUndoStack code indicates the comment should be
 * "Synonymous with index() > 0".
 */
bool
CaretUndoStack::canUndo() const
{
    if (m_undoStack.empty()) {
        return false;
    }
    
    if (index() > 0) {
        return true;
    }
    
    return false;
}

/**
 * @return Number of commands on the undo stack.
 */
int32_t
CaretUndoStack::count() const
{
    return m_undoStack.size();
}


/**
 * Returns the index of the current command. This is the command that will 
 * be executed on the next call to redo(). It is not always the top-most 
 * command on the stack, since a number of commands may have been undone.
 *
 * @return Index of current command.  This value ranges from 1 to count()
 * when the stack contains elements and zero when the stack is empty.
 */
int32_t
CaretUndoStack::index() const
{
    return m_undoStackIndex;
}

/**
 * Delete the command at the given index.  The index of the current
 * command may change (if it is greater than the index).  After 
 * calling this method, the command that was at the index has
 * been deleted, so NEVER use it after calling this method.
 *
 * @param index
 *     Index of command that is removed.
 */
void
CaretUndoStack::deleteCommandAtIndex(const int32_t index)
{
    if ((index >= 0)
        || (index < static_cast<int32_t>(m_undoStack.size()))) {
        CaretUndoCommand* undoCommand = m_undoStack.at(index);
        m_undoStack.erase(m_undoStack.begin() + index);
        delete undoCommand;
    }
    else {
        CaretLogWarning("CaretUndoStack::command() called with invalid index="
                        + AString::number(index));
    }
}


/**
 * Clears the command stack by deleting all commands on it, and returns
 * the stack to the clean state.
 * 
 * Commands are not undone or redone; the state of the edited object 
 * remains unchanged.
 * 
 * This function is usually used when the contents of the document 
 * are abandoned.
 */
void
CaretUndoStack::clear()
{
    for (std::deque<CaretUndoCommand*>::iterator iter = m_undoStack.begin();
         iter != m_undoStack.end();
         iter++) {
        delete *iter;
    }
    
    m_undoStack.clear();
    m_undoStackIndex = 0;
}

/**
 * Returns a const pointer to the command at index.
 *
 * This function returns a const pointer, because modifying a command, once 
 * it has been pushed onto the stack and executed, almost always causes 
 * corruption of the state of the document, if the command is later
 * undone or redone.
 *
 * @param index
 *     Index of the item in the undo stack.
 * @return
 *     Item at the given index or NULL if the index is invalid.
 */
const CaretUndoCommand*
CaretUndoStack::command(const int32_t index) const
{
    if ((index >= 0)
        || (index < static_cast<int32_t>(m_undoStack.size()))) {
        return m_undoStack.at(index);
    }
    else {
        CaretLogWarning("CaretUndoStack::command() called with invalid index="
                        + AString::number(index));
    }
    
    return NULL;
}

/**
 * Pushes the given command onto the stack.  Unlike QUndoStack the
 * command's redo() method IS NOT called.
 *
 * If commands were undone before cmd was pushed, the current command and 
 * all commands above it are deleted. Hence cmd always ends up being the 
 * top-most on the stack.
 *
 * Once a command is pushed, the stack takes ownership of it. There are 
 * no getters to return the command, since modifying it after it has been 
 * executed will almost always lead to corruption of the document's state.
 *
 * @param cmd
 *    Command pushed onto the stack.
 */
void
CaretUndoStack::push(CaretUndoCommand* cmd)
{
    if (index() < count()) {
        /*
         * Delete any commands that have been "undone"
         */
        const int numToDeleteAtBack = count() - index();
        for (int32_t i = 0; i < numToDeleteAtBack; i++) {
            delete m_undoStack.back();
            m_undoStack.pop_back();
        }
    }
    
    m_undoStack.push_back(cmd);
    
    if (m_undoLimit > 0) {
        /*
         * Delete oldest command(s) when undo limit is exceeded
         */
        const int32_t numToDeleteAtFront = count() - m_undoLimit;
        for (int32_t i = 0; i < numToDeleteAtFront; i++) {
            delete m_undoStack.front();
            m_undoStack.pop_front();
        }
    }
    
    m_undoStackIndex = count();
}

/**
 * Redoes the current command by calling QUndoCommand::redo(). 
 * Increments the current command index. 
 *
 * If the stack is empty, or if the top command on the stack has already
 * been redone, this function does nothing.
 */
void
CaretUndoStack::redo()
{
    if (m_undoStack.empty()) {
        return;
    }
    
    if ((m_undoStackIndex >= 0)
        && (m_undoStackIndex < count())) {
        redoCommand(m_undoStack.at(m_undoStackIndex));
        ++m_undoStackIndex;
    }
}

/**
 * @return Returns the description of the command which will be
 * redone in the next call to redo().
 */
AString
CaretUndoStack::redoText()
{
    AString text;
    
    if ( ! m_undoStack.empty()) {
        if ((m_undoStackIndex >= 0)
            && (m_undoStackIndex < count())) {
            text = m_undoStack.at(m_undoStackIndex)->getDescription();
        }
    }
    
    return text;
}


/**
 * Undoes the command below the current command by calling QUndoCommand::undo(). 
 * Decrements the current command index.
 *
 * If the stack is empty, or if the bottom command on the stack has already 
 * been undone, this function does nothing. 
 */
void
CaretUndoStack::undo()
{
    if (m_undoStack.empty()) {
        return;
    }
    
    if ((m_undoStackIndex > 0)
        && (m_undoStackIndex <= count())) {
        --m_undoStackIndex;
        undoCommand(m_undoStack.at(m_undoStackIndex));
    }
}


/**
 * @return Returns the description of the command which will be
 * undone in the next call to undo().
 */
AString
CaretUndoStack::undoText()
{
    AString text;
    
    if ( ! m_undoStack.empty()) {
        if ((m_undoStackIndex > 0)
            && (m_undoStackIndex <= count())) {
            const int32_t undoIndex = m_undoStackIndex - 1;
            text = m_undoStack.at(undoIndex)->getDescription();
        }
    }
    
    return text;
}


/**
 * When the number of commands on a stack exceedes the stack's undo limit,
 * commands are deleted from the bottom of the stack.  The default
 * value is 0, which means that there is no limit.
 * 
 * This property may only be set when the undo stack is empty, since setting 
 * it on a non-empty stack might delete the command at the current index.
 * Calling setUndoLimit() on a non-empty stack and a warning is logged. *
 * @param undoLimit
 *     New value for maximum number of undo commands.
 */
void
CaretUndoStack::setUndoLimit(const int32_t undoLimit)
{
    if (m_undoStack.empty()) {
        if (undoLimit >= 0) {
            m_undoLimit = undoLimit;
        }
        else {
            CaretLogWarning("CaretUndoStack::setUndoLimit() called with invalid value="
                            + AString::number(undoLimit));
        }
    }
    else {
        CaretLogWarning("CaretUndoStack::setUndoLimit() called while undo stack contains elements."
                        "  New undo limit ignored.");
    }
}

/**
 * Apply a 'redo' using the given command.
 *
 * Subclasses may override this method.
 *
 * @param cmd
 *     Command that performs a 'redo'.
 */
void
CaretUndoStack::redoCommand(CaretUndoCommand* cmd)
{
    CaretAssert(cmd);
    
    cmd->redo();
}

/**
 * Apply an 'undo' using the given command.
 *
 * Subclasses may override this method.
 *
 * @param cmd
 *     Command that performs a 'undo'.
 */
void
CaretUndoStack::undoCommand(CaretUndoCommand* cmd)
{
    CaretAssert(cmd);
    
    cmd->undo();
}

/**
 * Undo ALL changes.
 */
void
CaretUndoStack::undoAll()
{
    while (canUndo()) {
        undo();
    }
}



