#ifndef __WB_MACRO_CUSTOM_OPERATION_BASE_H__
#define __WB_MACRO_CUSTOM_OPERATION_BASE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"

class QString;
class QWidget;

namespace caret {
    class Surface;
    class WuQMacroCommand;

    class WbMacroCustomOperationBase : public CaretObject {
    public:
        ~WbMacroCustomOperationBase();
        
        WbMacroCustomOperationBase(const WbMacroCustomOperationBase&) = delete;
        
        WbMacroCustomOperationBase& operator=(const WbMacroCustomOperationBase&) = delete;
        
        /**
         * Execute the macro command
         *
         * @param parent
         *     Parent widget for any dialogs
         * @param macroCommand
         *     macro command to run
         * @return
         *     True if command executed successfully, else false
         *     Use getErrorMessage() for error information if false returned
         */
        virtual bool executeCommand(QWidget* parent,
                                    const WuQMacroCommand* macroCommand) = 0;
        
        /**
         * Get a new instance of the macro command
         *
         * @return
         *     Pointer to command or NULL if not valid
         *     Use getErrorMessage() for error information if NULL returned
         */
        virtual WuQMacroCommand* createCommand() = 0;
        
        QString getErrorMessage() const;
        
    protected:
        WbMacroCustomOperationBase();
        
        bool validateCorrectNumberOfParameters(const WuQMacroCommand* command);
        
        Surface* findSurface(const QString& surfaceName,
                             const QString& errorMessagePrefix);
        
        void appendToErrorMessage(const QString& text);
        
        void updateGraphics();
        
        void updateUserInterface();
        
        QString m_errorMessage;
    };
    

#ifdef __WB_MACRO_CUSTOM_OPERATION_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WB_MACRO_CUSTOM_OPERATION_BASE_DECLARE__

} // namespace
#endif  //__WB_MACRO_CUSTOM_OPERATION_BASE_H__
