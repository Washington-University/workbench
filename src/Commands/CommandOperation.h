#ifndef __COMMAND_OPERATION_H__
#define __COMMAND_OPERATION_H__

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


#include "CaretObject.h"
#include "CommandException.h"
#include "ProgramParametersException.h"
#include "AString.h"

#include "nifti1.h"

namespace caret {

    class ProgramParameters;
    
    /// Abstract class for a command operation.
    class CommandOperation : public CaretObject {

    public:
        virtual ~CommandOperation();
        
        void execute(ProgramParameters& parameters, const bool& preventProvenance);
        
        virtual void setCiftiOutputDTypeAndScale(const int16_t& dtype, const double& minVal, const double& maxVal);
        
        virtual void setCiftiOutputDTypeNoScale(const int16_t& dtype);
        
        virtual AString doCompletion(ProgramParameters& parameters, const bool& useExtGlob);
        
    protected:
        /**
         * Execute the operation.
         * 
         * @param parameters
         *   Parameters for the operation.
         * @throws CommandException
         *   If the command failed.
         * @throws ProgramParametersException
         *   If there is an error in the parameters.
         */
        virtual void executeOperation(ProgramParameters& parameters) = 0;
        
        virtual void disableProvenance();
        
        CommandOperation(const AString& commandLineSwitch,
                         const AString& operationShortDescription);
        
    private:
        CommandOperation();
        
        CommandOperation(const CommandOperation&);

        CommandOperation& operator=(const CommandOperation&);
        
    public:
        
        AString getOperationShortDescription() const;
        
        AString getCommandLineSwitch() const;
        
        virtual AString getHelpInformation(const AString& programName) = 0;
        
        virtual bool takesParameters();
        
    private:
        /** Short description listing commands purpose */
        AString operationShortDescription;
        
        /** Switch on command line */
        AString commandLineSwitch;
    };
    
} // namespace

#endif // __COMMAND_OPERATION_H__
