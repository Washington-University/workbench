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

#include "OperationParameters.h"
#include "AbstractOperation.h"
#include "CommandOperation.h"
#include "ProgramParameters.h"
#include "CommandException.h"
#include "ProgramParametersException.h"
#include <vector>
#include <set>

namespace caret {

    class CommandParser : public CommandOperation, OperationParserInterface
    {
        int m_minIndent, m_maxIndent, m_indentIncrement, m_maxWidth;
        AString m_provenance, m_parentProvenance, m_workingDir;
        bool m_doProvenance;
        const static AString PROVENANCE_NAME, PARENT_PROVENANCE_NAME, PROGRAM_PROVENANCE_NAME, CWD_PROVENANCE_NAME;//TODO: put this elsewhere?
        std::set<AString> m_inputCiftiNames;
        struct OutputAssoc
        {//how the output is stored is up to the parser, in the GUI it should load into memory without writing to disk
            AString m_fileName;
            AbstractParameter* m_param;
        };
        void parseComponent(ParameterComponent* myComponent, ProgramParameters& parameters, std::vector<OutputAssoc>& outAssociation, bool debug = false);
        bool parseOption(const AString& mySwitch, ParameterComponent* myComponent, ProgramParameters& parameters, std::vector<OutputAssoc>& outAssociation, bool debug);
        void parseRemainingOptions(ParameterComponent* myAlgParams, ProgramParameters& parameters, std::vector<OutputAssoc>& outAssociation, bool debug);
        void provenanceBeforeOperation(const std::vector<OutputAssoc>& outAssociation);
        void provenanceAfterOperation(const std::vector<OutputAssoc>& outAssociation);
        void makeOnDiskOutputs(const std::vector<OutputAssoc>& outAssociation);//ensures on-disk inputs aren't used as on-disk outputs, converting outputs to in-memory when needed
        void writeOutput(const std::vector<OutputAssoc>& outAssociation);
        AString getIndentString(int desired);
        void addHelpComponent(AString& info, ParameterComponent* myComponent, int curIndent);
        void addHelpOptions(AString& info, ParameterComponent* myAlgParams, int curIndent);
        void addHelpProse(AString& info, OperationParameters* myAlgParams, int curIndent);
        void addComponentDescriptions(AString& info, ParameterComponent* myComponent, int curIndent);
        void addOptionDescriptions(AString& info, ParameterComponent* myComponent, int curIndent);
        AString formatString(const AString& in, int curIndent, bool addIndent);
    public:
        CommandParser(AutoOperationInterface* myAutoOper);
        void disableProvenance();
        void executeOperation(ProgramParameters& parameters) throw (CommandException, ProgramParametersException);
        void showParsedOperation(ProgramParameters& parameters) throw (CommandException, ProgramParametersException);
        AString getHelpInformation(const AString& programName);
        bool takesParameters();
    };

};

#endif //__COMMAND_PARSER_H__
