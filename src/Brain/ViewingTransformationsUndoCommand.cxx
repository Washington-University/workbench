
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __VIEWING_TRANSFORMATIONS_UNDO_COMMAND_DECLARE__
#include "ViewingTransformationsUndoCommand.h"
#undef __VIEWING_TRANSFORMATIONS_UNDO_COMMAND_DECLARE__

#include "CaretAssert.h"
#include "ViewingTransformations.h"
using namespace caret;


    
/**
 * \class caret::ViewingTransformationsUndoCommand 
 * \brief Redo / Undo command for viewing transformations
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param viewingTransform
 *    Viewing transform that is 'redone' and 'undone'
 */
ViewingTransformationsUndoCommand::ViewingTransformationsUndoCommand(ViewingTransformations* viewingTransform)
: CaretUndoCommand(),
m_viewingTransform(viewingTransform)
{
    CaretAssert(m_viewingTransform);
    m_redoValue.reset(new ViewingTransformations());
    m_undoValue.reset(new ViewingTransformations());
}

/**
 * Destructor.
 */
ViewingTransformationsUndoCommand::~ViewingTransformationsUndoCommand()
{
}

/**
 * Operation that "redoes" the command.
 *
 * @param errorMessageOut
 *     Output containing error message.
 * @return
 *     True if the command executed successfully, else false.
 */
bool
ViewingTransformationsUndoCommand::redo(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    m_viewingTransform->copyFromOther(*m_redoValue);
    
    return true;
}

/**
 * Operation that "undoes" the command.
 */
bool
ViewingTransformationsUndoCommand::undo(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    m_viewingTransform->copyFromOther(*m_undoValue);
    
    return true;
}

/**
 * Set the redo and undo values for this instance
 * @param redoValue
 *    The redo value
 * @param undoValue
 *    The undo value
 */
void
ViewingTransformationsUndoCommand::setRedoUndoValues(const ViewingTransformations& redoValue,
                                                     const ViewingTransformations& undoValue)
{
    m_redoValue->copyFromOther(redoValue);
    m_undoValue->copyFromOther(undoValue);
}

