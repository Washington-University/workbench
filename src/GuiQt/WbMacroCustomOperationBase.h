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
#include "WbMacroCustomOperationTypeEnum.h"

class QString;
class QWidget;

namespace caret {
    class Surface;
    class WuQMacroExecutorMonitor;
    class WuQMacroExecutorOptions;
    class WuQMacroCommand;

    class WbMacroCustomOperationBase : public CaretObject {
    public:
        ~WbMacroCustomOperationBase();
        
        WbMacroCustomOperationBase(const WbMacroCustomOperationBase&) = delete;
        
        WbMacroCustomOperationBase& operator=(const WbMacroCustomOperationBase&) = delete;
        
        WbMacroCustomOperationTypeEnum::Enum getOperationType() const;
        
        /**
         * Execute the macro command
         *
         * @param parent
         *     Parent widget for any dialogs
         * @param executorMonitor
         *     the macro executor monitor
         * @param executorOptions,
         *     Options for the executor
         * @param macroCommand
         *     macro command to run
         * @return
         *     True if command executed successfully, else false
         *     Use getErrorMessage() for error information if false returned
         */
        virtual bool executeCommand(QWidget* parent,
                                    const WuQMacroExecutorMonitor* executorMonitor,
                                    const WuQMacroExecutorOptions* executorOptions,
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
        
        void sleepForSecondsAtEndOfIteration(const float seconds);
        
        virtual QString getOperationName() const;
        
    protected:
        WbMacroCustomOperationBase(const WbMacroCustomOperationTypeEnum::Enum operationType);
        
        void getNumberOfStepsAndSleepTime(const WuQMacroExecutorOptions* executorOptions,
                                          const float defaultNumberOfSteps,
                                          const float durationSeconds,
                                          float& numberOfStepsOut,
                                          float& sleepTimeOut);
        
        bool validateCorrectNumberOfParameters(const WuQMacroCommand* command,
                                               const int32_t correctNumberOfParameters);
        
        Surface* findSurface(const QString& surfaceName,
                             const QString& errorMessagePrefix);
        
        void appendToErrorMessage(const QString& text);
        
        void appendUnsupportedVersionToErrorMessage(const int32_t unsupportedVersionNumber);
        
        void updateGraphics();
        
        void updateSurfaceColoring();
        
        void updateUserInterface();
        
        const WbMacroCustomOperationTypeEnum::Enum m_operationType;
        
        QString m_errorMessage;
    };
    

#ifdef __WB_MACRO_CUSTOM_OPERATION_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WB_MACRO_CUSTOM_OPERATION_BASE_DECLARE__

} // namespace
#endif  //__WB_MACRO_CUSTOM_OPERATION_BASE_H__
