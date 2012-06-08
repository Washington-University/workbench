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
#include "OperationException.h"
#include "AlgorithmException.h"
#include "DataFileException.h"
#include <iostream>

using namespace caret;
using namespace std;

CommandParser::CommandParser(AutoOperationInterface* myAutoOper) :
    CommandOperation(myAutoOper->getCommandSwitch(), myAutoOper->getShortDescription()),
    OperationParserInterface(myAutoOper)
{
}

void CommandParser::executeOperation(ProgramParameters& parameters) throw (CommandException, ProgramParametersException)
{
    try
    {
        CaretPointer<OperationParameters> myAlgParams(m_autoOper->getParameters());//could be an autopointer, but this is safer
        vector<OutputAssoc> myOutAssoc;
        
        parseComponent(myAlgParams.getPointer(), parameters, myOutAssoc);//parsing block
        parameters.verifyAllParametersProcessed();
        //code to show what arguments map to what parameters should go here
        
        m_autoOper->useParameters(myAlgParams.getPointer(), NULL);//TODO: progress status for caret_command? would probably get messed up by any command info output
        
        writeOutput(myOutAssoc);
    } catch (ProgramParametersException& e) {
        throw e;
    } catch (CaretException& e) {
        throw CommandException(e);//rethrow all other caret exceptions as CommandException
    } catch (exception& e) {
        throw CommandException(e.what());//rethrow std::exception and derived as CommandException
    } catch (...) {
        throw CommandException("unknown exception type thrown");//throw dummy CommandException for anything else
    }
}

void CommandParser::showParsedOperation(ProgramParameters& parameters) throw (CommandException, ProgramParametersException)
{
    CaretPointer<OperationParameters> myAlgParams(m_autoOper->getParameters());//could be an autopointer, but this is safer
    vector<OutputAssoc> myOutAssoc;
    
    parseComponent(myAlgParams.getPointer(), parameters, myOutAssoc, true);//parsing block
    parameters.verifyAllParametersProcessed();
    //don't execute or write parsed output
}

void CommandParser::parseComponent(ParameterComponent* myComponent, ProgramParameters& parameters, vector<OutputAssoc>& outAssociation, bool debug)
{
    uint32_t i;
    for (i = 0; i < myComponent->m_paramList.size(); ++i)
    {
        AString nextArg = parameters.nextString(myComponent->m_paramList[i]->m_shortName);
        if (nextArg[0] == '-')
        {
            bool success = parseOption(nextArg, myComponent, parameters, outAssociation, debug);
            if (!success)
            {
                switch (myComponent->m_paramList[i]->getType())
                {
                case OperationParametersEnum::STRING:
                case OperationParametersEnum::INT:
                case OperationParametersEnum::DOUBLE:
                    break;//it is probably a negative number, so don't throw an exception unless it fails to parse as one
                default:
                    throw ProgramParametersException("Invalid option \"" + nextArg + "\" while next required argument is <" + myComponent->m_paramList[i]->m_shortName +
                                                     ">, option is either incorrect, or incorrectly placed");
                };
            } else {
                --i;
                continue;//so skip trying to parse it as a required argument
            }
        }
        switch (myComponent->m_paramList[i]->getType())
        {
            case OperationParametersEnum::BOOL:
            {
                parameters.backup();
                ((BooleanParameter*)myComponent->m_paramList[i])->m_parameter = parameters.nextBoolean(myComponent->m_paramList[i]->m_shortName);
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> parsed as ";
                    cout << (((BooleanParameter*)myComponent->m_paramList[i])->m_parameter ? "true" : "false") << endl;
                }
                break;
            }
            case OperationParametersEnum::CIFTI:
            {
                CaretPointer<CiftiFile> myFile(new CiftiFile());
                myFile->openFile(nextArg, ON_DISK);
                ((CiftiParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                    cout << nextArg << endl;
                }
                break;
            }
            case OperationParametersEnum::DOUBLE:
            {
                parameters.backup();
                ((DoubleParameter*)myComponent->m_paramList[i])->m_parameter = parameters.nextDouble(myComponent->m_paramList[i]->m_shortName);
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> parsed as ";
                    cout << ((DoubleParameter*)myComponent->m_paramList[i])->m_parameter << endl;
                }
                break;
            }
            case OperationParametersEnum::INT:
            {
                parameters.backup();
                ((IntegerParameter*)myComponent->m_paramList[i])->m_parameter = parameters.nextLong(myComponent->m_paramList[i]->m_shortName);
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> parsed as ";
                    cout << ((IntegerParameter*)myComponent->m_paramList[i])->m_parameter << endl;
                }
                break;
            }
            case OperationParametersEnum::LABEL:
            {
                CaretPointer<LabelFile> myFile(new LabelFile());
                myFile->readFile(nextArg);
                ((LabelParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                    cout << nextArg << endl;
                }
                break;
            }
            case OperationParametersEnum::METRIC:
            {
                CaretPointer<MetricFile> myFile(new MetricFile());
                myFile->readFile(nextArg);
                ((MetricParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                    cout << nextArg << endl;
                }
                break;
            }
            case OperationParametersEnum::STRING:
            {
                ((StringParameter*)myComponent->m_paramList[i])->m_parameter = nextArg;
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> parsed as ";
                    cout << ((StringParameter*)myComponent->m_paramList[i])->m_parameter << endl;
                }
                break;
            }
            case OperationParametersEnum::SURFACE:
            {
                CaretPointer<SurfaceFile> myFile(new SurfaceFile());
                myFile->readFile(nextArg);
                ((SurfaceParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                    cout << nextArg << endl;
                }
                break;
            }
            case OperationParametersEnum::VOLUME:
            {
                CaretPointer<VolumeFile> myFile(new VolumeFile());
                myFile->readFile(nextArg);
                ((VolumeParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                if (debug)
                {
                    cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                    cout << nextArg << endl;
                }
                break;
            }
            default:
                CaretAssertMessage(false, "Parsing of this parameter type has not been implemented in this parser");//assert instead of throw because this is a code error, not a user error
                throw CommandException("Internal parsing error, please let the developers know what you just tried to do");//but don't let release pass by it either
        };
    }
    for (i = 0; i < myComponent->m_outputList.size(); ++i)
    {//parse the output options of this component
        AString nextArg = parameters.nextString(myComponent->m_outputList[i]->m_shortName);
        if (nextArg[0] == '-')
        {
            bool success = parseOption(nextArg, myComponent, parameters, outAssociation, debug);
            if (!success)
            {
                throw ProgramParametersException("Invalid option \"" + nextArg + "\" while next reqired argument is <" + myComponent->m_outputList[i]->m_shortName +
                ">, option is either incorrect, or incorrectly placed");
            }
            --i;//options do not set required arguments
            continue;//so rewind the index and skip trying to parse it as a required argument
        }
        OutputAssoc tempItem;
        tempItem.m_fileName = nextArg;
        tempItem.m_param = myComponent->m_outputList[i];
        switch (myComponent->m_outputList[i]->getType())
        {
        case OperationParametersEnum::CIFTI:
            ((CiftiParameter*)myComponent->m_outputList[i])->m_parameter->setCiftiCacheFile(nextArg);
            break;
        default:
            break;
        }
        outAssociation.push_back(tempItem);
        if (debug)
        {
            cout << "Output parameter <" << tempItem.m_param->m_shortName << "> given output name ";
            cout << tempItem.m_fileName << endl;
        }
    }
    parseRemainingOptions(myComponent, parameters, outAssociation, debug);
}

bool CommandParser::parseOption(const AString& mySwitch, ParameterComponent* myComponent, ProgramParameters& parameters, vector<OutputAssoc>& outAssociation, bool debug)
{
    for (uint32_t i = 0; i < myComponent->m_optionList.size(); ++i)
    {
        if (mySwitch == myComponent->m_optionList[i]->m_optionSwitch)
        {
            if (debug)
            {
                cout << "Now parsing option " << myComponent->m_optionList[i]->m_optionSwitch << endl;
            }
            if (myComponent->m_optionList[i]->m_present)
            {
                throw ProgramParametersException("Option \"" + mySwitch + "\" specified more than once");
            }
            myComponent->m_optionList[i]->m_present = true;
            parseComponent(myComponent->m_optionList[i], parameters, outAssociation, debug);
            if (debug)
            {
                cout << "Finished parsing option " << myComponent->m_optionList[i]->m_optionSwitch << endl;
            }
            return true;
        }
    }
    return false;
}

void CommandParser::parseRemainingOptions(ParameterComponent* myComponent, ProgramParameters& parameters, vector<OutputAssoc>& outAssociation, bool debug)
{
    while (parameters.hasNext())
    {
        AString nextArg = parameters.nextString("option");
        if (nextArg[0] == '-')
        {
            bool success = parseOption(nextArg, myComponent, parameters, outAssociation, debug);
            if (!success)
            {
                parameters.backup();
                return;
            }
        } else {
            parameters.backup();
            return;
        }
    }
}

void CommandParser::writeOutput(const vector<OutputAssoc>& outAssociation)
{
    for (uint32_t i = 0; i < outAssociation.size(); ++i)
    {
        AbstractParameter* myParam = outAssociation[i].m_param;
        switch (myParam->getType())
        {
            case OperationParametersEnum::BOOL://ignores the name you give the output for now, but what gives primitive type output and how is it used?
                cout << "Output Boolean \"" << myParam->m_shortName << "\" value is " << ((BooleanParameter*)myParam)->m_parameter << endl;
                break;
            case OperationParametersEnum::CIFTI:
                ((CiftiParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case OperationParametersEnum::DOUBLE:
                cout << "Output Floating Point \"" << myParam->m_shortName << "\" value is " << ((DoubleParameter*)myParam)->m_parameter << endl;
                break;
            case OperationParametersEnum::INT:
                cout << "Output Integer \"" << myParam->m_shortName << "\" value is " << ((IntegerParameter*)myParam)->m_parameter << endl;
                break;
            case OperationParametersEnum::LABEL:
                ((LabelParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case OperationParametersEnum::METRIC:
                ((MetricParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case OperationParametersEnum::STRING:
                cout << "Output String \"" << myParam->m_shortName << "\" value is " << ((StringParameter*)myParam)->m_parameter << endl;
                break;
            case OperationParametersEnum::SURFACE:
                ((SurfaceParameter*)myParam)->m_parameter->writeFile(outAssociation[i].m_fileName);
                break;
            case OperationParametersEnum::VOLUME:
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
    m_minIndent = 0;
    m_indentIncrement = 3;
    m_maxWidth = 79;//leave a space on the right edge of an 80-wide terminal so that it looks better - TODO: get the terminal width from some system call
    m_maxIndent = 31;//don't let indenting take up more than this
    int curIndent = m_minIndent;
    AString ret;
    ret = formatString(getOperationShortDescription(), curIndent, true);
    curIndent += m_indentIncrement;
    ret += getIndentString(curIndent) + programName + " " + getCommandLineSwitch() + "\n";//DO NOT format the command that people may want to copy and paste, added hyphens would be disastrous
    curIndent += m_indentIncrement;
    OperationParameters* myAlgParams = m_autoOper->getParameters();
    addHelpComponent(ret, myAlgParams, curIndent);
    addHelpProse(ret, myAlgParams, curIndent);
    ret += getIndentString(curIndent) + "Descriptions of parameters and options:\n\n";
    addComponentDescriptions(ret, myAlgParams, curIndent);
    delete myAlgParams;
    return ret;
}

void CommandParser::addHelpComponent(AString& info, ParameterComponent* myComponent, int curIndent)
{
    for (int i = 0; i < (int)myComponent->m_paramList.size(); ++i)
    {
        info += formatString("<" + myComponent->m_paramList[i]->m_shortName + ">", curIndent, true);
    }
    for (int i = 0; i < (int)myComponent->m_outputList.size(); ++i)
    {
        info += formatString("<" + myComponent->m_outputList[i]->m_shortName + ">", curIndent, true);
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

void CommandParser::addHelpProse(AString& info, OperationParameters* myAlgParams, int curIndent)
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
        if (endIndex >= in.size())
        {
            ret += curIndentString + in.mid(curIndex, endIndex - curIndex) + "\n";
        } else {
            if (in[endIndex] == '\n')
            {
                while (endIndex < in.size() && in[endIndex] == '\n')
                {//crawl over any additional newlines
                    ++endIndex;
                }
                haveAddedBreak = false;
                ret += curIndentString + in.mid(curIndex, endIndex - curIndex);
            } else {
                int savedEnd = endIndex;
                while (endIndex > curIndex && in[endIndex] != ' ')
                {//crawl in reverse until a space, or reaching curIndex - change this if you want hyphenation to take place more often than lines without any spaces
                    --endIndex;
                }
                if (in[endIndex] == ' ')
                {//found a space, break line at the space
                    while (endIndex > curIndex && in[endIndex] == ' ')
                    {//don't print any of the spaces
                        --endIndex;
                    }
                    if (endIndex > curIndex)
                    {
                        ++endIndex;//print the character before the space
                    }
                    haveAddedBreak = true;
                    ret += curIndentString + in.mid(curIndex, endIndex - curIndex) + "\n";
                } else {//hyphenate
                    endIndex = savedEnd - 1;
                    haveAddedBreak = true;
                    ret += curIndentString + in.mid(curIndex, endIndex - curIndex) + "-\n";
                }
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
    for (int i = 0; i < (int)myComponent->m_outputList.size(); ++i)
    {
        info += formatString("<" + myComponent->m_outputList[i]->m_shortName + "> - output - " + myComponent->m_outputList[i]->m_description, curIndent, true);
    }
    addOptionDescriptions(info, myComponent, curIndent);
}

void CommandParser::addOptionDescriptions(AString& info, ParameterComponent* myComponent, int curIndent)
{
    for (int i = 0; i < (int)myComponent->m_optionList.size(); ++i)
    {
        info += "\n" + formatString("[" + myComponent->m_optionList[i]->m_optionSwitch + "] - " + myComponent->m_optionList[i]->m_description, curIndent, true);
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
