#ifndef __OPERATION_ZIP_SPEC_FILE_H__
#define __OPERATION_ZIP_SPEC_FILE_H__

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

#include "AbstractOperation.h"

namespace caret {
    
    class OperationZipSpecFile : public AbstractOperation
    {
    public:
        enum ProgressMode {
            PROGRESS_COMMAND_LINE,
            PROGRESS_GUI_EVENT
        };
        
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
        
        static void createZipFile(ProgressObject* myProgObj,
                                  const AString& inputSpecFileName,
                                  const AString& specFileName,
                                  const AString& outputSubDirectory,
                                  const AString& zipFileName,
                                  const AString& myBaseDirIn,
                                  const ProgressMode progressMode,
                                  const bool skipMissing);
        
    private:
        static void addChildDataFiles(const AString& dataFileName,
                                      std::vector<AString>& childDataFileNamesOut);
    };
    typedef TemplateAutoOperation<OperationZipSpecFile> AutoOperationZipSpecFile;
}

#endif //__OPERATION_ZIP_SPEC_FILE_H__
