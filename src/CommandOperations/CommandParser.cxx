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

#include "CommandParser.h"
#include "CaretAssert.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "LabelFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"
#include <iostream>

using namespace caret;
using namespace std;

CommandParser::CommandParser(AutoAlgorithmInterface* myAutoAlg) :
    CommandOperation(myAutoAlg->getCommandSwitch(), myAutoAlg->getShortDescription()),
    AlgorithmParserInterface(myAutoAlg)
{
}

void CommandParser::executeOperation(ProgramParameters& parameters) throw (CommandException, ProgramParametersException)
{
    AlgorithmParameters* myAlgParams = m_autoAlg->getParameters();
    vector<OutputAssoc> myOutAssoc;
    
    try
    {
        parseComponent(myAlgParams, parameters);//parsing block
        parseOutputAssoc(myAlgParams, parameters, myOutAssoc);
        parseRemainingOptions(myAlgParams, parameters);
    } catch (ProgramParametersException e) {
        delete myAlgParams;
        throw e;
    }
    //code to show what arguments map to what parameters should go here
    
    m_autoAlg->useParameters(myAlgParams, NULL);//TODO: progress status for caret_command? would probably get messed up by any command info output
    
    writeOutput(myAlgParams, myOutAssoc);//TODO: some way of having outputs that are only optional? probably would make parsing harder to get right
    
    delete myAlgParams;
}

void CommandParser::parseComponent(ParameterComponent* myComponent, ProgramParameters& parameters)
{
    uint32_t i = 0;
    while (true)//so that we can get suboptions that are placed on the end of an option (and its degenerate case, options that contain only suboptions and no required arguments)
    {
        AString nextArg = parameters.nextString(myComponent->m_paramList[i]->m_shortName);
        if (nextArg[0] == '-')
        {
            bool success = parseOption(nextArg, myComponent, parameters);
            if (success)
            {
                continue;//so skip trying to parse it as a required argument
            } else {
                //if we reach the end of a component and the next thing is an option, it could be either an option in THIS component, or an option in the component ABOVE this
                //so, test if we finished this component when we find an option that this component doesn't recognize
                if (i < myComponent->m_paramList.size())
                {//unknown option while more arguments required is an error
                    throw ProgramParametersException("Invalid Option switch \"" + nextArg + "\" while next non-option argument is <" + myComponent->m_paramList[i]->m_shortName +
                        ">, option switch is either incorrect, or incorrectly placed");
                }
            }
        }
        if (i == myComponent->m_paramList.size())
        {
            parameters.backup();
            return;
        }
        switch (myComponent->m_paramList[i]->getType())
        {
            case AlgorithmParametersEnum::BOOL:
            {
                parameters.backup();
                ((BooleanParameter*)myComponent->m_paramList[i])->m_parameter = parameters.nextBoolean(myComponent->m_paramList[i]->m_shortName);
                break;
            }
            case AlgorithmParametersEnum::CIFTI:
            {
                CiftiFile* myFile = new CiftiFile();
                myFile->openFile(nextArg);
                ((CiftiParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                break;
            }
            case AlgorithmParametersEnum::DOUBLE:
            {
                parameters.backup();
                ((DoubleParameter*)myComponent->m_paramList[i])->m_parameter = parameters.nextDouble(myComponent->m_paramList[i]->m_shortName);
                break;
            }
            case AlgorithmParametersEnum::INT:
            {
                parameters.backup();
                ((IntParameter*)myComponent->m_paramList[i])->m_parameter = parameters.nextLong(myComponent->m_paramList[i]->m_shortName);
                break;
            }
            case AlgorithmParametersEnum::LABEL:
            {
                LabelFile* myFile = new LabelFile();
                myFile->readFile(nextArg);
                ((LabelParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                break;
            }
            case AlgorithmParametersEnum::METRIC:
            {
                MetricFile* myFile = new MetricFile();
                myFile->readFile(nextArg);
                ((MetricParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                break;
            }
            case AlgorithmParametersEnum::STRING:
            {
                ((StringParameter*)myComponent->m_paramList[i])->m_parameter = nextArg;
                break;
            }
            case AlgorithmParametersEnum::SURFACE:
            {
                SurfaceFile* myFile = new SurfaceFile();
                myFile->readFile(nextArg);
                ((SurfaceParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                break;
            }
            case AlgorithmParametersEnum::VOLUME:
            {
                VolumeFile* myFile = new VolumeFile();
                myFile->readFile(nextArg);
                ((VolumeParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                break;
            }
            default:
                CaretAssertMessage(false, "Parsing of this parameter type has not been implemented in this parser");//assert instead of throw because this is a code error, not a user error
                throw CommandException("Internal parsing error, please let the developers know what you just tried to do");//but don't let release pass by it either
        };
        ++i;//next required parameter
    }
}

bool CommandParser::parseOption(const AString& mySwitch, ParameterComponent* myComponent, ProgramParameters& parameters)
{
    for (uint32_t i = 0; i < myComponent->m_optionList.size(); ++i)
    {
        if (mySwitch == myComponent->m_optionList[i]->m_optionSwitch)
        {
            myComponent->m_optionList[i]->m_present = true;
            parseComponent(myComponent->m_optionList[i], parameters);
            return true;
        }
    }
    return false;
}

void CommandParser::parseOutputAssoc(AlgorithmParameters* myAlgParams, ProgramParameters& parameters, vector<OutputAssoc>& outAssociation)
{
    for (uint32_t i = 0; i < myAlgParams->m_outputList.size(); ++i)
    {
        AString nextArg = parameters.nextString(myAlgParams->m_outputList[i]->m_shortName);
        if (nextArg[0] == '-')
        {
            bool success = parseOption(nextArg, myAlgParams, parameters);
            if (!success)
            {//we don't currently have optional outputs, so this must be the base level, failure to parse an argument on base level is fatal
                throw ProgramParametersException("Unknown option: " + nextArg);
            }
            --i;//options do not set required arguments
            continue;//so rewind the index and skip trying to parse it as a required argument
        }
        OutputAssoc tempItem;
        tempItem.m_fileName = nextArg;
        tempItem.m_type = myAlgParams->m_outputList[i]->getType();
        tempItem.m_outputKey = myAlgParams->m_outputList[i]->m_key;
        outAssociation.push_back(tempItem);
    }
}

void CommandParser::parseRemainingOptions(AlgorithmParameters* myAlgParams, ProgramParameters& parameters)
{
    while (parameters.hasNext())
    {
        AString nextArg = parameters.nextString("option");
        if (nextArg[0] == '-')
        {
            bool success = parseOption(nextArg, myAlgParams, parameters);
            if (!success)
            {//we don't currently have optional outputs, so this must be the base level, failure to parse an argument on base level is fatal
                throw ProgramParametersException("Unknown option: " + nextArg);
            }
        } else {
            throw ProgramParametersException("Unexpected non-option parameter: \"" + nextArg +"\"");
        }
    }
}

void CommandParser::writeOutput(AlgorithmParameters* myAlgParams, const vector<OutputAssoc>& outAssociation)
{
    for (uint32_t i = 0; i < outAssociation.size(); ++i)
    {
        AbstractParameter* myParam = myAlgParams->getOutputParameter(outAssociation[i].m_outputKey, outAssociation[i].m_type);
        switch (outAssociation[i].m_type)
        {
            case AlgorithmParametersEnum::BOOL://ignores the name you give the output for now, but what gives primitive type output and how is it used?
                cout << "Output Boolean \"" << myParam->m_shortName << "\" value is " << ((BooleanParameter*)myParam)->m_parameter << endl;
                break;
            case AlgorithmParametersEnum::CIFTI:
                ((CiftiParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case AlgorithmParametersEnum::DOUBLE:
                cout << "Output Floating Point \"" << myParam->m_shortName << "\" value is " << ((DoubleParameter*)myParam)->m_parameter << endl;
                break;
            case AlgorithmParametersEnum::INT:
                cout << "Output Integer \"" << myParam->m_shortName << "\" value is " << ((IntParameter*)myParam)->m_parameter << endl;
                break;
            case AlgorithmParametersEnum::LABEL:
                ((LabelParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case AlgorithmParametersEnum::METRIC:
                ((MetricParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case AlgorithmParametersEnum::STRING:
                cout << "Output String \"" << myParam->m_shortName << "\" value is " << ((StringParameter*)myParam)->m_parameter << endl;
                break;
            case AlgorithmParametersEnum::SURFACE:
                ((SurfaceParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case AlgorithmParametersEnum::VOLUME:
                ((VolumeParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            default:
                CaretAssertMessage(false, "Writing of this parameter type has not been implemented in this parser");//assert instead of throw because this is a code error, not a user error
                throw CommandException("Internal parsing error, please let the developers know what you just tried to do");//but don't let release pass by it either
        }
    }
}

AString CommandParser::getHelpInformation(const AString& programName)
{
    m_minIndent = 3;
    m_indentIncrement = 3;
    m_maxWidth = 80;
    m_maxIndent = 31;//don't let indenting take up more than this
    int curIndent = m_minIndent;
    AString ret;
    ret = formatString(getOperationShortDescription(), curIndent, true);
    curIndent += m_indentIncrement;
    ret += getIndentString(curIndent) + programName + " " + getCommandLineSwitch() + "\n";//DO NOT format the command that people may want to copy and paste, added hyphens would be disastrous
    curIndent += m_indentIncrement;
    AlgorithmParameters* myAlgParams = m_autoAlg->getParameters();
    for (int i = 0; i < (int)myAlgParams->m_paramList.size(); ++i)
    {
        ret += formatString("<" + myAlgParams->m_paramList[i]->m_shortName + ">", curIndent, true);
    }
    for (int i = 0; i < (int)myAlgParams->m_outputList.size(); ++i)
    {
        ret += formatString("<" + myAlgParams->m_outputList[i]->m_shortName + ">", curIndent, true);
    }
    addHelpOptions(ret, myAlgParams, curIndent);
    addHelpProse(ret, myAlgParams, curIndent);
    ret += getIndentString(curIndent) + "Description of parameters and options:\n\n";
    for (int i = 0; i < (int)myAlgParams->m_paramList.size(); ++i)
    {
        ret += formatString(myAlgParams->m_paramList[i]->m_shortName + " - " + myAlgParams->m_paramList[i]->m_description, curIndent, true);
    }
    for (int i = 0; i < (int)myAlgParams->m_outputList.size(); ++i)
    {
        ret += formatString(myAlgParams->m_outputList[i]->m_shortName + " - out - " + myAlgParams->m_outputList[i]->m_description, curIndent, true);
    }
    addOptionDescriptions(ret, myAlgParams, curIndent);
    delete myAlgParams;
    return ret;
}

void CommandParser::addHelpComponent(AString& info, ParameterComponent* myComponent, int curIndent)
{
    for (int i = 0; i < (int)myComponent->m_paramList.size(); ++i)
    {
        info += formatString("<" + myComponent->m_paramList[i]->m_shortName + ">", curIndent, true);
    }
    addHelpOptions(info, myComponent, curIndent);
}

void CommandParser::addHelpOptions(AString& info, ParameterComponent* myComponent, int curIndent)
{
    for (int i = 0; i < (int)myComponent->m_optionList.size(); ++i)
    {
        info += formatString("[" + myComponent->m_optionList[i]->m_optionSwitch + "]", curIndent, true);
        addHelpComponent(info, myComponent->m_optionList[i], curIndent + m_indentIncrement);//indent arguments to options
    }
}

void CommandParser::addHelpProse(AString& info, AlgorithmParameters* myAlgParams, int curIndent)
{//NOTE: does not currently format tabs well, don't use them
    AString* rawProse = &(myAlgParams->getHelpText());//friendlier name
    info += "\n";//separate prose with another newline
    info += formatString(*rawProse, curIndent, false);//don't indent on added newlines in the prose
    info += "\n";//additional newline
}

AString CommandParser::formatString(const AString& in, int curIndent, bool addIndent)
{//NOTE: does not currently format tabs well, don't use them
    AString curIndentString = getIndentString(curIndent);
    bool haveAddedBreak = false;
    AString ret;
    int charMax = m_maxWidth - curIndent;
    int curIndex = 0;
    while (curIndex < in.size())
    {
        if (addIndent)
        {
            if (haveAddedBreak)
            {
                curIndentString = getIndentString(curIndent + m_indentIncrement);
                charMax = m_maxWidth - curIndent - m_indentIncrement;
            } else {
                curIndentString = getIndentString(curIndent);
                charMax = m_maxWidth - curIndent;
            }
        }
        int endIndex = curIndex;
        while (endIndex - curIndex < charMax && endIndex < in.size() && in[endIndex] != '\n')
        {//start by crawling until newline or at max width
            ++endIndex;
        }
        if (in[endIndex - 1] == '\n')
        {
            while (endIndex < in.size() && in[endIndex] == '\n')
            {//crawl over any additional newlines
                ++endIndex;
            }
            haveAddedBreak = false;
            ret += curIndentString + in.mid(curIndex, endIndex - curIndex);
        } else {
            if (endIndex < in.size())
            {
                int savedEnd = endIndex;
                while (endIndex > curIndex && in[endIndex] != ' ')
                {//crawl in reverse until a space, or reaching curIndex
                    --endIndex;
                }
                if (in[endIndex] == ' ')
                {//found a space, break line at the space
                    while (endIndex > curIndex && in[endIndex] == ' ')
                    {//don't print any of the spaces
                        ++endIndex;
                    }
                haveAddedBreak = true;
                ret += curIndentString + in.mid(curIndex, endIndex - curIndex) + "\n";
                } else {//hyphenate
                    endIndex = savedEnd - 1;
                    haveAddedBreak = true;
                    ret += curIndentString + in.mid(curIndex, endIndex - curIndex) + "-\n";
                }
            } else {
                ret += curIndentString + in.mid(curIndex, endIndex - curIndex) + "\n";
            }
        }
        curIndex = endIndex;
        while (curIndex < in.size() && in[curIndex] == ' ')
        {//skip spaces
            ++curIndex;
        }
    }
    return ret;
}

void CommandParser::addComponentDescriptions(AString& info, ParameterComponent* myComponent, int curIndent)
{
    for (int i = 0; i < (int)myComponent->m_paramList.size(); ++i)
    {
        info += formatString("<" + myComponent->m_paramList[i]->m_shortName + "> - " + myComponent->m_paramList[i]->m_description, curIndent, true);
    }
    addOptionDescriptions(info, myComponent, curIndent);
}

void CommandParser::addOptionDescriptions(AString& info, ParameterComponent* myComponent, int curIndent)
{
    for (int i = 0; i < (int)myComponent->m_optionList.size(); ++i)
    {
        info += formatString("[" + myComponent->m_optionList[i]->m_optionSwitch + "] - " + myComponent->m_optionList[i]->m_shortName + " - " + myComponent->m_optionList[i]->m_description, curIndent, true);
        addComponentDescriptions(info, myComponent->m_optionList[i], curIndent + m_indentIncrement);//indent arguments to options
    }
}

AString CommandParser::getIndentString(int desired)
{
    AString space(" ");
    int num = desired;
    if (num > m_maxIndent) num = m_maxIndent;
    if (num < m_minIndent) num = m_minIndent;
    return space.repeated(num);
}

bool CommandParser::takesParameters()
{
    return CommandOperation::takesParameters();
}
