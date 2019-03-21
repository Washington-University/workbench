
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_EXECUTOR_OPTIONS_DECLARE__
#include "WuQMacroExecutorOptions.h"
#undef __WU_Q_MACRO_EXECUTOR_OPTIONS_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::WuQMacroExecutorOptions 
 * \brief Options for execution of macros
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WuQMacroExecutorOptions::WuQMacroExecutorOptions()
: CaretObject()
{
    /* members initialized in header file */
}

/**
 * Destructor.
 */
WuQMacroExecutorOptions::~WuQMacroExecutorOptions()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacroExecutorOptions::WuQMacroExecutorOptions(const WuQMacroExecutorOptions& obj)
: CaretObject(obj)
{
    this->copyHelperWuQMacroExecutorOptions(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
WuQMacroExecutorOptions&
WuQMacroExecutorOptions::operator=(const WuQMacroExecutorOptions& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperWuQMacroExecutorOptions(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
WuQMacroExecutorOptions::copyHelperWuQMacroExecutorOptions(const WuQMacroExecutorOptions& obj)
{
    m_loopingFlag                        = obj.m_loopingFlag;
    m_stopOnErrorFlag                    = obj.m_stopOnErrorFlag;
    m_showMouseMovementFlag              = obj.m_showMouseMovementFlag;
    m_recordMovieDuringExecutionFlag     = obj.m_recordMovieDuringExecutionFlag;
    m_createMovieAfterMacroExecutionFlag = obj.m_createMovieAfterMacroExecutionFlag;
    m_stopAfterSelectedCommandFlag       = obj.m_stopAfterSelectedCommandFlag;
    m_ignoreDelaysAndDurationsFlag       = obj.m_ignoreDelaysAndDurationsFlag;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
WuQMacroExecutorOptions::toString() const
{
    return "WuQMacroExecutorOptions";
}

/**
 * @return Show mouse movement while running macro
 */
bool
WuQMacroExecutorOptions::isShowMouseMovement() const
{
    return m_showMouseMovementFlag;
}

/**
 * Set show mouse movement activity that was recorded in the macro
 *
 * @param status
 *     New status
 */
void
WuQMacroExecutorOptions::setShowMouseMovement(const bool status)
{
    m_showMouseMovementFlag = status;
}

/**
 * @return True if exector should stop macro if an error occurs
 */
bool
WuQMacroExecutorOptions::isStopOnError() const
{
    return m_stopOnErrorFlag;
}

/**
 * Stop runningh of macro if there is an error while running the macro
 *
 * @param status
 *     New status
 */
void
WuQMacroExecutorOptions::setStopOnError(const bool status)
{
    m_stopOnErrorFlag = status;
}

/**
 * @return True if looping is on.
 */
bool
WuQMacroExecutorOptions::isLooping() const
{
    return m_loopingFlag;
}

/**
 * Set looping on/off
 *
 * @param status
 *     New status
 */
void
WuQMacroExecutorOptions::setLooping(const bool status)
{
    m_loopingFlag = status;
}

/**
 * @return True if record movie while macro executes is on.
 */
bool
WuQMacroExecutorOptions::isRecordMovieDuringExecution() const
{
    return m_recordMovieDuringExecutionFlag;
}

/**
 * Set record movie during macro execution
 *
 * @param status
 *     New status
 */
void
WuQMacroExecutorOptions::setRecordMovieDuringExecution(const bool status)
{
    m_recordMovieDuringExecutionFlag = status;
}

/**
 * @return True if create movie after macro executes is on.
 */
bool
WuQMacroExecutorOptions::isCreateMovieAfterMacroExecution() const
{
    return m_createMovieAfterMacroExecutionFlag;
}

/**
 * Set create movie after macro execution
 *
 * @param status
 *     New status
 */
void
WuQMacroExecutorOptions::setCreateMovieAfterMacroExecution(const bool status)
{
    m_createMovieAfterMacroExecutionFlag = status;
}

/**
 * @return True if execution should stop after the command selected in macro dialog
 */
bool
WuQMacroExecutorOptions::isStopAfterSelectedCommand() const
{
    return m_stopAfterSelectedCommandFlag;
}

/**
 * Set execution should stop after selected command in macro dialog
 *
 * @param status
 *     New status
 */
void
WuQMacroExecutorOptions::setStopAfterSelectedCommand(const bool status)
{
    m_stopAfterSelectedCommandFlag = status;
}

/**
 * @return True if delays and durations should be ignored,
 * usually for editing and debugging macros.
 */
bool
WuQMacroExecutorOptions::isIgnoreDelaysAndDurations() const
{
    return m_ignoreDelaysAndDurationsFlag;
}

/**
 * Set delays and durations should be ignored,
 * usually for editing and debugging macros
 *
 * @param status
 *     New status
 */
void
WuQMacroExecutorOptions::setIgnoreDelaysAndDurations(const bool status)
{
    m_ignoreDelaysAndDurationsFlag = status;
}
