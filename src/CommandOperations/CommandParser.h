#ifndef __COMMAND_PARSER_H__
#define __COMMAND_PARSER_H__

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

#include "AlgorithmParameters.h"
#include "AbstractAlgorithm.h"
#include "CommandOperation.h"
#include "ProgramParameters.h"
#include "CommandException.h"
#include "ProgramParametersException.h"
#include <vector>

namespace caret {

    class CommandParser : public CommandOperation, AlgorithmParserInterface
    {
        struct OutputAssoc
        {//how the output is stored is up to the parser, in the GUI it should load into memory without writing to disk
            AString m_fileName;
            AbstractParameter::parameterType m_type;
            int32_t m_outputKey;
        };
        void parseComponent(ParameterComponent* myComponent, ProgramParameters& parameters);
        void parseOutputAssoc(AlgorithmParameters* myComponent, ProgramParameters& parameters, std::vector<OutputAssoc>& outAssociation);
        bool parseOption(const AString& mySwitch, ParameterComponent* myComponent, ProgramParameters& parameters);
        void parseRemainingOptions(AlgorithmParameters* myAlgParams, ProgramParameters& parameters);
        void writeOutput(AlgorithmParameters* myAlgParams, const std::vector<OutputAssoc>& outAssociation);
    public:
        CommandParser(AutoAlgorithmInterface* myAutoAlg);
        void executeOperation(ProgramParameters& parameters) throw (CommandException, ProgramParametersException);
    };

};

#endif //__COMMAND_PARSER_H__