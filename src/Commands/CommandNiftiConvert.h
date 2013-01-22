#ifndef __COMMAND_NIFTI_CONVERT__H__
#define __COMMAND_NIFTI_CONVERT__H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "CommandOperation.h"

namespace caret {

/// Command that converts NIFTI files to different encodings
class CommandNiftiConvert : public CommandOperation {

public:
    CommandNiftiConvert();

    virtual ~CommandNiftiConvert();

    virtual void executeOperation(ProgramParameters& parameters)
    throw (CommandException,
           ProgramParametersException);

    AString getHelpInformation(const AString& /*programName*/);

private:

    CommandNiftiConvert(const CommandNiftiConvert&);

    CommandNiftiConvert& operator=(const CommandNiftiConvert&);

};

} // namespace

#endif // __COMMAND_NIFTI_CONVERT__H__
