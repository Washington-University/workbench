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

#include "CommandParser.h"

#include "AlgorithmException.h"
#include "ApplicationInformation.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretCommandLine.h"
#include "CaretDataFileHelper.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "FociFile.h"
#include "GiftiMetaData.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "OperationException.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include <QDir>

#include <iostream>

using namespace caret;
using namespace std;

const AString CommandParser::PROVENANCE_NAME = "Provenance";
const AString CommandParser::PARENT_PROVENANCE_NAME = "ParentProvenance";
const AString CommandParser::PROGRAM_PROVENANCE_NAME = "ProgramProvenance";
const AString CommandParser::CWD_PROVENANCE_NAME = "WorkingDirectory";

CommandParser::CommandParser(AutoOperationInterface* myAutoOper) :
    CommandOperation(myAutoOper->getCommandSwitch(), myAutoOper->getShortDescription()),
    OperationParserInterface(myAutoOper)
{
    m_doProvenance = true;
    m_ciftiScale = false;
    m_ciftiDType = NIFTI_TYPE_FLOAT32;
    m_ciftiMax = -1.0;//these values won't get used, but don't leave them uninitialized
    m_ciftiMin = -1.0;
}

void CommandParser::disableProvenance()
{
    m_doProvenance = false;
}

void CommandParser::setCiftiOutputDTypeAndScale(const int16_t& dtype, const double& minVal, const double& maxVal)
{
    m_ciftiDType = dtype;
    m_ciftiMin = minVal;
    m_ciftiMax = maxVal;
    m_ciftiScale = true;
}

void CommandParser::setCiftiOutputDTypeNoScale(const int16_t& dtype)
{
    m_ciftiDType = dtype;
    m_ciftiMax = -1.0;//for sanity, don't keep any previous value around
    m_ciftiMin = -1.0;
    m_ciftiScale = false;
}

void CommandParser::executeOperation(ProgramParameters& parameters)
{
    CaretPointer<OperationParameters> myAlgParams(m_autoOper->getParameters());//could be an autopointer, but this is safer
    vector<OutputAssoc> myOutAssoc;
    m_provenance = caret_global_commandLine;
    //the idea is to have m_provenance set before the command executes, so it can be overridden, but have m_parentProvenance set AFTER the processing is complete
    //the parent provenance should never be generated manually
    m_parentProvenance = "";//in case someone tries to use the same instance more than once
    m_workingDir = QDir::currentPath();//get the current path, in case some stupid command changes the working directory
    //these get set on output files during writeOutput (and for on-disk in provenanceBeforeOperation)
    parseComponent(myAlgParams.getPointer(), parameters, myOutAssoc);//parsing block
    parameters.verifyAllParametersProcessed();
    makeOnDiskOutputs(myOutAssoc);//check for input on-disk files used as output on-disk files
    //code to show what arguments map to what parameters should go here
    if (m_doProvenance) provenanceBeforeOperation(myOutAssoc);
    m_autoOper->useParameters(myAlgParams.getPointer(), NULL);//TODO: progress status for caret_command? would probably get messed up by any command info output
    vector<AString> uncheckedWarnings = myAlgParams->findUncheckedParams("the command");
    for (size_t i = 0; i < uncheckedWarnings.size(); ++i)
    {
        CaretLogWarning("developer warning: " + uncheckedWarnings[i]);
    }
    if (m_doProvenance) provenanceAfterOperation(myOutAssoc);
    //TODO: deallocate input files - give abstract parameter a virtual deallocate method? use CaretPointer and rely on reference counting?
    writeOutput(myOutAssoc);
}

void CommandParser::showParsedOperation(ProgramParameters& parameters)
{
    CaretPointer<OperationParameters> myAlgParams(m_autoOper->getParameters());//could be an autopointer, but this is safer
    vector<OutputAssoc> myOutAssoc;
    
    parseComponent(myAlgParams.getPointer(), parameters, myOutAssoc, true);//parsing block
    parameters.verifyAllParametersProcessed();
    //don't execute or write parsed output
}

void CommandParser::parseComponent(ParameterComponent* myComponent, ProgramParameters& parameters, vector<OutputAssoc>& outAssociation, bool debug)
{//IMPORTANT: update completionComponent() and friends with any change to parsing logic
    for (int i = 0; i < (int)myComponent->m_paramList.size(); ++i)
    {
        bool hyphenReplaced = false;
        //TSC: until someone complains, I say non-unicode dashes don't belong on the command line, EVER
        AString rawArg = parameters.nextString(myComponent->m_paramList[i]->m_shortName);
        AString nextArg = rawArg.fixUnicodeHyphens(&hyphenReplaced);
        if (hyphenReplaced)
        {
            CaretLogWarning("replaced non-ascii hyphen/dash characters in argument '" + rawArg + "' with ascii '-'");
        }
        if (!nextArg.isEmpty() && nextArg[0] == '-')
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
        const OperationParametersEnum::Enum nextType = myComponent->m_paramList[i]->getType();// need in catch statement below
        try {
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
                case OperationParametersEnum::BORDER:
                {
                    CaretPointer<BorderFile> myFile(new BorderFile());
                    myFile->readFile(nextArg);
                    if (m_doProvenance)
                    {
                        const GiftiMetaData* md = myFile->getFileMetaData();
                        if (md != NULL)
                        {
                            AString prov = md->get(PROVENANCE_NAME);
                            if (prov != "")
                            {
                                m_parentProvenance += nextArg + ":\n" + prov + "\n\n";
                            }
                        }
                    }
                    ((BorderParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                    if (debug)
                    {
                        cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                        cout << nextArg << endl;
                    }
                    break;
                }
                case OperationParametersEnum::CIFTI:
                {
                    FileInformation myInfo(nextArg);
                    CaretPointer<CiftiFile> myFile(new CiftiFile());
                    myFile->openFile(nextArg);
                    m_inputCiftiNames[myInfo.getCanonicalFilePath()] = myFile;//track input cifti, so we can check their size
                    if (m_doProvenance)//just an optimization, if we aren't going to write provenance, don't generate it, either
                    {
                        const GiftiMetaData* md = myFile->getCiftiXML().getFileMetaData();
                        if (md != NULL)
                        {
                            if (md->exists(PROVENANCE_NAME))
                            {
                                AString provenance = md->get(PROVENANCE_NAME);
                                if (provenance != "")
                                {
                                    m_parentProvenance += nextArg + ":\n" + provenance + "\n\n";
                                }
                            }
                        }
                    }
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
                case OperationParametersEnum::FOCI:
                {
                    CaretPointer<FociFile> myFile(new FociFile());
                    myFile->readFile(nextArg);
                    if (m_doProvenance)
                    {
                        const GiftiMetaData* md = myFile->getFileMetaData();
                        if (md != NULL)
                        {
                            AString prov = md->get(PROVENANCE_NAME);
                            if (prov != "")
                            {
                                m_parentProvenance += nextArg + ":\n" + prov + "\n\n";
                            }
                        }
                    }
                    ((FociParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                    if (debug)
                    {
                        cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                        cout << nextArg << endl;
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
                    if (m_doProvenance)
                    {
                        const GiftiMetaData* md = myFile->getFileMetaData();
                        if (md != NULL)
                        {
                            AString prov = md->get(PROVENANCE_NAME);
                            if (prov != "")
                            {
                                m_parentProvenance += nextArg + ":\n" + prov + "\n\n";
                            }
                        }
                    }
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
                    if (m_doProvenance)
                    {
                        const GiftiMetaData* md = myFile->getFileMetaData();
                        if (md != NULL)
                        {
                            AString prov = md->get(PROVENANCE_NAME);
                            if (prov != "")
                            {
                                m_parentProvenance += nextArg + ":\n" + prov + "\n\n";
                            }
                        }
                    }
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
                    if (m_doProvenance)
                    {
                        const GiftiMetaData* md = myFile->getFileMetaData();
                        if (md != NULL)
                        {
                            AString prov = md->get(PROVENANCE_NAME);
                            if (prov != "")
                            {
                                m_parentProvenance += nextArg + ":\n" + prov + "\n\n";
                            }
                        }
                    }
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
                    if (m_doProvenance)
                    {
                        const GiftiMetaData* md = myFile->getFileMetaData();
                        if (md != NULL)
                        {
                            AString prov = md->get(PROVENANCE_NAME);
                            if (prov != "")
                            {
                                m_parentProvenance += nextArg + ":\n" + prov + "\n\n";
                            }
                        }
                    }
                    ((VolumeParameter*)myComponent->m_paramList[i])->m_parameter = myFile;
                    if (debug)
                    {
                        cout << "Parameter <" << myComponent->m_paramList[i]->m_shortName << "> opened file with name ";
                        cout << nextArg << endl;
                    }
                    break;
                }
            };
        }
        catch (const bad_alloc&) {
            switch (nextType)
            {
                case OperationParametersEnum::BORDER:
                case OperationParametersEnum::CIFTI:
                case OperationParametersEnum::FOCI:
                case OperationParametersEnum::LABEL:
                case OperationParametersEnum::METRIC:
                case OperationParametersEnum::SURFACE:
                case OperationParametersEnum::VOLUME:
                    /*
                     * Provide information to the user about which
                     * file caused the std::bad_alloc including
                     * the size of the file.
                     */
                    throw DataFileException(nextArg,
                                            CaretDataFileHelper::createBadAllocExceptionMessage(nextArg));

                    break;
                case OperationParametersEnum::DOUBLE:
                case OperationParametersEnum::INT:
                case OperationParametersEnum::STRING:
                case OperationParametersEnum::BOOL:
                    throw DataFileException("Unable to allocate memory for input: "
                                            + nextArg);
                    break;
            }
        }
    }
    for (int i = 0; i < (int)myComponent->m_outputList.size(); ++i)
    {//parse the output options of this component
        bool hyphenReplaced = false;
        //TSC: until someone complains, I say non-unicode dashes don't belong on the command line, EVER
        AString rawArg = parameters.nextString(myComponent->m_outputList[i]->m_shortName);
        AString nextArg = rawArg.fixUnicodeHyphens(&hyphenReplaced);
        if (hyphenReplaced)
        {
            CaretLogWarning("replaced non-ascii hyphen/dash characters in argument '" + rawArg + "' with ascii '-'");
        }
        if (!nextArg.isEmpty() && nextArg[0] == '-')
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
        switch (myComponent->m_outputList[i]->getType())//allocate outputs that only have in-memory implementations
        {
            case OperationParametersEnum::BORDER:
            {
                CaretPointer<BorderFile>& myFile = ((BorderParameter*)(myComponent->m_outputList[i]))->m_parameter;
                myFile.grabNew(new BorderFile());
                break;
            }
            case OperationParametersEnum::CIFTI:
                break;//we create this in makeOnDiskOutputs(), and do the metadata stuff in provenanceForOnDiskOutputs() for this type
            case OperationParametersEnum::FOCI:
            {
                CaretPointer<FociFile>& myFile = ((FociParameter*)(myComponent->m_outputList[i]))->m_parameter;
                myFile.grabNew(new FociFile());
                break;
            }
            case OperationParametersEnum::LABEL:
            {
                CaretPointer<LabelFile>& myFile = ((LabelParameter*)(myComponent->m_outputList[i]))->m_parameter;
                myFile.grabNew(new LabelFile());
                break;
            }
            case OperationParametersEnum::METRIC:
            {
                CaretPointer<MetricFile>& myFile = ((MetricParameter*)(myComponent->m_outputList[i]))->m_parameter;
                myFile.grabNew(new MetricFile());
                break;
            }
            case OperationParametersEnum::SURFACE:
            {
                CaretPointer<SurfaceFile>& myFile = ((SurfaceParameter*)(myComponent->m_outputList[i]))->m_parameter;
                myFile.grabNew(new SurfaceFile());
                break;
            }
            case OperationParametersEnum::VOLUME:
            {
                CaretPointer<VolumeFile>& myFile = ((VolumeParameter*)(myComponent->m_outputList[i]))->m_parameter;
                myFile.grabNew(new VolumeFile());
                break;
            }
            case OperationParametersEnum::DOUBLE://ignore these output types
            case OperationParametersEnum::INT:
            case OperationParametersEnum::STRING:
            case OperationParametersEnum::BOOL:
                CaretLogWarning("encountered ignored output type, " + OperationParametersEnum::toName(myComponent->m_outputList[i]->getType()));
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
    for (uint32_t i = 0; i < myComponent->m_repeatableOptions.size(); ++i)
    {
        if (mySwitch == myComponent->m_repeatableOptions[i]->m_optionSwitch)
        {
            if (debug)
            {
                cout << "Now parsing repeatable option " << myComponent->m_repeatableOptions[i]->m_optionSwitch << endl;
            }
            myComponent->m_repeatableOptions[i]->m_instances.push_back(new ParameterComponent(myComponent->m_repeatableOptions[i]->m_template));
            parseComponent(myComponent->m_repeatableOptions[i]->m_instances.back(), parameters, outAssociation, debug);
            if (debug)
            {
                cout << "Finished parsing repeatable option " << myComponent->m_repeatableOptions[i]->m_optionSwitch << endl;
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
        bool hyphenReplaced = false;
        //TSC: until someone complains, I say non-unicode dashes don't belong on the command line, EVER
        AString rawArg = parameters.nextString("option");
        AString nextArg = rawArg.fixUnicodeHyphens(&hyphenReplaced);
        if (hyphenReplaced)
        {
            CaretLogWarning("replaced non-ascii hyphen/dash characters in argument '" + rawArg + "' with ascii '-'");
        }
        if (!nextArg.isEmpty() && nextArg[0] == '-')
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

AString CommandParser::doCompletion(ProgramParameters& parameters, const bool& useExtGlob)
{
    CaretPointer<OperationParameters> myAlgParams(m_autoOper->getParameters());//could be an autopointer, but this is safer
    CompletionInfo ret = completionComponent(myAlgParams.getPointer(), parameters, useExtGlob);
    if (parameters.hasNext()) return "";//you're off the edge of the map, find the monsters yourself
    return ret.completionHints;
}

CommandParser::CompletionInfo CommandParser::completionComponent(ParameterComponent* myComponent, ProgramParameters& parameters, const bool& useExtGlob)
{
    CompletionInfo ret;//initializes complete to false
    for (int i = 0; i < (int)myComponent->m_paramList.size(); ++i)
    {
        //a bit complicated...
        //if there is no next parameter, obviously we should do completion based on current mandatory parameter
        //if the next parameter starts with -, it must either be an option, numeric, or string
        // if the option hasn't been completed, return its completion info
        // *if the option is completed, but no parameters remain, need to add the mandatory argument completion to the returned completion info
        // if parameters remain after the option is completed, restart the iteration in order to be able take another option immediately
        if (parameters.hasNext())
        {
            //TSC: until someone complains, I say non-unicode dashes don't belong on the command line, EVER
            AString rawArg = parameters.nextString(myComponent->m_paramList[i]->m_shortName);
            AString nextArg = rawArg.fixUnicodeHyphens();
            if (!nextArg.isEmpty() && nextArg[0] == '-')
            {
                CompletionInfo optionInfo = completionOption(nextArg, myComponent, parameters, useExtGlob);
                if (!optionInfo.found)
                {
                    switch (myComponent->m_paramList[i]->getType())
                    {
                    case OperationParametersEnum::STRING:
                    case OperationParametersEnum::INT:
                    case OperationParametersEnum::DOUBLE:
                        break;//it is probably a negative number, so let it parse as one
                    default:
                        //NOTE: we know the command will fail to parse, now what?
                        break;//pretend it will parse as a filename, and continue?
                    };
                    continue;//assume the parameter works in the given position and move on
                } else {//specified option switch did match an option
                    if (!optionInfo.complete)
                    {//if the option's mandatory parameters weren't completed, don't add any completion from the current context, just pass the result through
                        return optionInfo;
                    }
                    if (!parameters.hasNext())
                    {//save these hints, then let it proceed to the completion section
                        ret.completionHints = optionInfo.completionHints;
                    } else {
                        --i;
                        continue;//so skip trying to parse it as a required argument, and restart the loop on the same iteration
                    }
                }
            } else {
                continue;//parameter is empty or doesn't start with -, assume it works fine
            }
        }//the above conditional does a continue unless we need to do completion now
        switch (myComponent->m_paramList[i]->getType())
        {
            case OperationParametersEnum::BOOL:
                if (ret.completionHints != "") ret.completionHints += " ";
                ret.completionHints += "wordlist true\\ TRUE\\ false\\ FALSE";
                break;
            case OperationParametersEnum::BORDER:
                if (ret.completionHints != "") ret.completionHints += " ";
                if (useExtGlob)
                {
                    ret.completionHints += "fileglob *.?(wb_)border";
                } else {
                    ret.completionHints += "fileglob *.border fileglob *.wb_border";
                }
                break;
            case OperationParametersEnum::CIFTI:
                if (ret.completionHints != "") ret.completionHints += " ";
                ret.completionHints += "fileglob *.*.nii";//cifti standard allows arbitrary secondary extensions, and volume files are nearly always .nii.gz anyway
                break;//could restrict it if someone complains (maybe a preference)
            case OperationParametersEnum::FOCI:
                if (ret.completionHints != "") ret.completionHints += " ";
                if (useExtGlob)
                {
                    ret.completionHints += "fileglob *.?(wb_)foci";
                } else {
                    ret.completionHints += "fileglob *.foci fileglob *.wb_foci";
                }
                break;
            case OperationParametersEnum::LABEL://is there a caret5 extension for this gifti file type?
                if (ret.completionHints != "") ret.completionHints += " ";
                ret.completionHints += "fileglob *.label.gii";
                break;
            case OperationParametersEnum::METRIC://include the caret5 extension that is often the same format
                if (ret.completionHints != "") ret.completionHints += " ";
                if (useExtGlob)//also allow label files for now, see if anyone complains (maybe a preference)
                {
                    ret.completionHints += "fileglob *.@(@(func|shape|label).gii|metric)";
                } else {
                    ret.completionHints += "fileglob *.func.gii fileglob *.shape.gii fileglob *.metric fileglob *.label.gii";
                }
                break;
            case OperationParametersEnum::SURFACE:
                if (ret.completionHints != "") ret.completionHints += " ";
                ret.completionHints += "fileglob *.surf.gii";
                break;
            case OperationParametersEnum::VOLUME:
                if (ret.completionHints != "") ret.completionHints += " ";
                if (useExtGlob)
                {//special functionality - when we have extglob, we can exclude common cifti extensions
                    ret.completionHints += "fileglob !(*.dconn|*.dtseries|*.pconn|*.ptseries|*.dscalar|*.dlabel|*.pscalar|*.pdconn|*.dpconn|*.pconnseries|*.pconnscalar|*.plabel).nii?(.gz)";
                } else {//when we don't...sorry - maybe have a preference to exclude uncompressed nifti in this condition (not great either)
                    //if it is really a problem, we could introduce a "fileregexp" response type and do it manually with sed or grep
                    ret.completionHints += "fileglob *.nii.gz fileglob *.nii";
                }
                break;
            case OperationParametersEnum::STRING://for strings, since we use them for filenames sometimes, glob to filenames, I guess
                if (ret.completionHints != "") ret.completionHints += " ";
                ret.completionHints += "fileglob *";
                break;
            case OperationParametersEnum::DOUBLE:
            case OperationParametersEnum::INT:
                //numeric types don't get any completion hints
                break;
        }
        AString optionHints = completionOptionHints(myComponent, useExtGlob);
        if (optionHints != "")
        {
            if (ret.completionHints != "") ret.completionHints += " ";
            ret.completionHints += optionHints;
        }
        return ret;
    }
    for (int i = 0; i < (int)myComponent->m_outputList.size(); ++i)
    {//parse the output options of this component
        if (parameters.hasNext())
        {
            //TSC: until someone complains, I say non-unicode dashes don't belong on the command line, EVER
            AString rawArg = parameters.nextString(myComponent->m_outputList[i]->m_shortName);
            AString nextArg = rawArg.fixUnicodeHyphens();
            if (!nextArg.isEmpty() && nextArg[0] == '-')
            {
                CompletionInfo optionInfo = completionOption(nextArg, myComponent, parameters, useExtGlob);
                if (!optionInfo.found)
                {
                    //NOTE: we know the command will fail to parse, now what?
                    continue;//pretend the parameter works in the given position and move on?
                } else {//specified option switch did match an option
                    if (!optionInfo.complete)
                    {//if the option's mandatory parameters weren't completed, don't add any completion from the current context, just pass the result through
                        return optionInfo;
                    }
                    if (!parameters.hasNext())
                    {//save these hints, then let it proceed to the completion section
                        ret.completionHints = optionInfo.completionHints;
                    } else {
                        --i;
                        continue;//so skip trying to parse it as a required argument, and restart the loop on the same iteration
                    }
                }
            } else {
                continue;//parameter is empty or doesn't start with -, assume it works fine
            }
        }//the above conditional does a continue unless we need to do completion now
        if (ret.completionHints != "") ret.completionHints += " ";
        ret.completionHints += "fileglob *";//we are specifying an output file, so glob to everything so they can easily reuse names from other format files
        AString optionHints = completionOptionHints(myComponent, useExtGlob);
        if (optionHints != "")
        {
            ret.completionHints += " " + optionHints;
        }
        return ret;
    }
    CompletionInfo remainInfo = completionRemainingOptions(myComponent, parameters, useExtGlob);//returns last option parsed (so, the one to do completion on, if applicable)
    if (remainInfo.found)
    {
        if (!remainInfo.complete) return remainInfo;
        if (!parameters.hasNext())
        {
            AString localOpts = completionOptionHints(myComponent, useExtGlob);
            if (remainInfo.completionHints != "") remainInfo.completionHints += " ";
            remainInfo.completionHints += localOpts;
            return remainInfo;
        }
    }
    ret.complete = true;
    if (!parameters.hasNext())//no remaining options matched, but we are out of parameters, so fill in any options that could come next
    {
        ret.completionHints = completionOptionHints(myComponent, useExtGlob);
    }
    return ret;
}

CommandParser::CompletionInfo CommandParser::completionOption(const AString& mySwitch, ParameterComponent* myComponent, ProgramParameters& parameters, const bool& useExtGlob)
{
    for (uint32_t i = 0; i < myComponent->m_optionList.size(); ++i)
    {
        if (mySwitch == myComponent->m_optionList[i]->m_optionSwitch)
        {
            if (myComponent->m_optionList[i]->m_present)
            {
                //NOTE: we know the parsing will fail here, now what?
                //pretend it is repeatable and move on, I guess
            }
            myComponent->m_optionList[i]->m_present = true;
            CompletionInfo optionInfo = completionComponent(myComponent->m_optionList[i], parameters, useExtGlob);
            optionInfo.found = true;
            return optionInfo;
        }
    }
    for (uint32_t i = 0; i < myComponent->m_repeatableOptions.size(); ++i)
    {
        if (mySwitch == myComponent->m_repeatableOptions[i]->m_optionSwitch)
        {
            myComponent->m_repeatableOptions[i]->m_instances.push_back(new ParameterComponent(myComponent->m_repeatableOptions[i]->m_template));
            CompletionInfo optionInfo = completionComponent(myComponent->m_repeatableOptions[i]->m_instances.back(), parameters, useExtGlob);
            optionInfo.found = true;
            return optionInfo;
        }
    }
    return CompletionInfo();//found initializes to false
}

AString CommandParser::completionOptionHints(ParameterComponent* myComponent, const bool& /*useExtGlob*/)
{
    AString ret;
    for (uint32_t i = 0; i < myComponent->m_optionList.size(); ++i)
    {
        if (!myComponent->m_optionList[i]->m_present)
        {//don't suggest non-repeatable options we already have
            if (ret == "")
            {
                ret = "wordlist ";
            } else {
                ret += "\\ ";
            }
            ret += myComponent->m_optionList[i]->m_optionSwitch;
        }
    }
    for (uint32_t i = 0; i < myComponent->m_repeatableOptions.size(); ++i)
    {//include all repeatable options
        if (ret == "")
        {
            ret = "wordlist ";
        } else {
            ret += "\\ ";
        }
        ret += myComponent->m_repeatableOptions[i]->m_optionSwitch;
    }
    return ret;
}

CommandParser::CompletionInfo CommandParser::completionRemainingOptions(ParameterComponent* myComponent, ProgramParameters& parameters, const bool& useExtGlob)
{//NOTE: completionComponent will complete the local options if needed when found is false
    CompletionInfo prev;
    while (parameters.hasNext())
    {
        //TSC: until someone complains, I say non-unicode dashes don't belong on the command line, EVER
        AString rawArg = parameters.nextString("option");
        AString nextArg = rawArg.fixUnicodeHyphens();
        if (!nextArg.isEmpty() && nextArg[0] == '-')
        {
            CompletionInfo temp = completionOption(nextArg, myComponent, parameters, useExtGlob);
            if (!temp.found)
            {
                parameters.backup();
                return prev;//more things to parse, could return default constructed instead
            }
            prev = temp;//it was found, so update what we found
        } else {//more things to parse, but they don't go here
            parameters.backup();
            return CompletionInfo();//found initializes to false
        }
    }
    return prev;//no parameters remain, return whatever was found
}

void CommandParser::provenanceBeforeOperation(const vector<OutputAssoc>& outAssociation)
{
    vector<AString> versionInfo;//need this for on-disk outputs, because we have to set it before the command executes
    ApplicationInformation myInfo;
    myInfo.getAllInformation(versionInfo);
    AString versionProvenance;
    for (int i = 0; i < (int)versionInfo.size(); ++i)
    {
        versionProvenance += versionInfo[i] + "\n";
    }
    for (uint32_t i = 0; i < outAssociation.size(); ++i)
    {
        AbstractParameter* myParam = outAssociation[i].m_param;
        switch (myParam->getType())
        {
            case OperationParametersEnum::CIFTI:
            {
                CiftiFile* myFile = ((CiftiParameter*)myParam)->m_parameter;
                CiftiXML myXML;
                CiftiSeriesMap tempMap;
                tempMap.setLength(1);
                tempMap.setStep(1.0f);
                tempMap.setStart(0.0f);
                tempMap.setUnit(CiftiSeriesMap::SECOND);
                myXML.setNumberOfDimensions(2);
                myXML.setMap(0, tempMap);
                myXML.setMap(1, tempMap);
                GiftiMetaData* mymd = myXML.getFileMetaData();
                mymd->set(PROVENANCE_NAME, m_provenance);
                mymd->set(PROGRAM_PROVENANCE_NAME, versionProvenance);//cifti is on-disk, so set all provenance now, because we can't later
                mymd->set(CWD_PROVENANCE_NAME, m_workingDir);
                if (m_parentProvenance != "")
                {
                    mymd->set(PARENT_PROVENANCE_NAME, m_parentProvenance);
                }
                myFile->setCiftiXML(myXML, false);//tells it to use this new metadata, rather than copying metadata from the old XML (which is default so that provenance metadata persists through naive usage)
                break;
            }
            default:
                break;
        }
    }
}

void CommandParser::provenanceAfterOperation(const vector<OutputAssoc>& outAssociation)
{
    vector<AString> versionInfo;//now we need this information for outputs that are in memory until written
    ApplicationInformation myInfo;
    myInfo.getAllInformation(versionInfo);
    AString versionProvenance;
    for (int i = 0; i < (int)versionInfo.size(); ++i)
    {
        versionProvenance += versionInfo[i] + "\n";
    }
    for (uint32_t i = 0; i < outAssociation.size(); ++i)
    {
        AbstractParameter* myParam = outAssociation[i].m_param;
        GiftiMetaData* md = NULL;
        switch (myParam->getType())
        {
            case OperationParametersEnum::BORDER:
            {
                BorderFile* myFile = ((BorderParameter*)myParam)->m_parameter;
                md = myFile->getFileMetaData();
                break;
            }
            case OperationParametersEnum::FOCI:
            {
                FociFile* myFile = ((FociParameter*)myParam)->m_parameter;
                md = myFile->getFileMetaData();
                break;
            }
            case OperationParametersEnum::LABEL:
            {
                LabelFile* myFile = ((LabelParameter*)myParam)->m_parameter;
                md = myFile->getFileMetaData();
                break;
            }
            case OperationParametersEnum::METRIC:
            {
                MetricFile* myFile = ((MetricParameter*)myParam)->m_parameter;
                md = myFile->getFileMetaData();
                break;
            }
            case OperationParametersEnum::SURFACE:
            {
                SurfaceFile* myFile = ((SurfaceParameter*)myParam)->m_parameter;
                md = myFile->getFileMetaData();
                break;
            }
            case OperationParametersEnum::VOLUME:
            {
                VolumeFile* myFile = ((VolumeParameter*)myParam)->m_parameter;
                md = myFile->getFileMetaData();
                break;
            }
            default:
                break;
        }
        if (md != NULL)
        {
            md->set(PROVENANCE_NAME, m_provenance);
            md->set(PROGRAM_PROVENANCE_NAME, versionProvenance);
            md->set(CWD_PROVENANCE_NAME, m_workingDir);
            if (m_parentProvenance != "")
            {
                md->set(PARENT_PROVENANCE_NAME, m_parentProvenance);
            }
        }
    }
}

void CommandParser::makeOnDiskOutputs(const vector<OutputAssoc>& outAssociation)
{
    for (uint32_t i = 0; i < outAssociation.size(); ++i)
    {
        AbstractParameter* myParam = outAssociation[i].m_param;
        switch (myParam->getType())
        {
            case OperationParametersEnum::CIFTI:
            {
                CiftiParameter* myCiftiParam = (CiftiParameter*)myParam;
                FileInformation myInfo(outAssociation[i].m_fileName);
                map<AString, const CiftiFile*>::iterator iter = m_inputCiftiNames.find(myInfo.getCanonicalFilePath());
                if (iter != m_inputCiftiNames.end())
                {
                    vector<int64_t> dims = iter->second->getDimensions();
                    int64_t totalSize = sizeof(float);
                    for (int j = 0; j < (int)dims.size(); ++j)
                    {
                        totalSize *= dims[j];
                    }
                    if (totalSize > ((int64_t)2) * 1024 * 1024 * 1024)//suppress the message for non-large input files, on the assumption that the output file will be the same size
                    {
                        CaretLogInfo("Computing output file '" + outAssociation[i].m_fileName + "' in memory due to collision with input file");
                    }
                    myCiftiParam->m_parameter.grabNew(new CiftiFile());
               } else {
                    myCiftiParam->m_parameter.grabNew(new CiftiFile());
                    myCiftiParam->m_parameter->setWritingFile(outAssociation[i].m_fileName);
                }
                if (m_ciftiScale)
                {
                    myCiftiParam->m_parameter->setWritingDataTypeAndScaling(m_ciftiDType, m_ciftiMin, m_ciftiMax);
                } else {
                    myCiftiParam->m_parameter->setWritingDataTypeNoScaling(m_ciftiDType);
                }
                break;
            }
            default:
                break;
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
            case OperationParametersEnum::BORDER:
            {
                BorderFile* myFile = ((BorderParameter*)myParam)->m_parameter;
                myFile->writeFile(outAssociation[i].m_fileName);
                break;
            }
            case OperationParametersEnum::CIFTI:
            {
                CiftiFile* myFile = ((CiftiParameter*)myParam)->m_parameter;//we can't set metadata here because the XML is already on disk, see provenanceForOnDiskOutputs
                myFile->writeFile(outAssociation[i].m_fileName);//this is basically a noop unless outputs and inputs collide, we opened ON_DISK and set cache file to this name back in makeOnDiskOutputs
                myFile->close();//if there is a problem flushing the file, let it throw here instead of doing a severe log message
                break;
            }
            case OperationParametersEnum::DOUBLE:
                cout << "Output Floating Point \"" << myParam->m_shortName << "\" value is " << ((DoubleParameter*)myParam)->m_parameter << endl;
                break;
            case OperationParametersEnum::INT:
                cout << "Output Integer \"" << myParam->m_shortName << "\" value is " << ((IntegerParameter*)myParam)->m_parameter << endl;
                break;
            case OperationParametersEnum::FOCI:
            {
                FociFile* myFile = ((FociParameter*)myParam)->m_parameter;
                myFile->writeFile(outAssociation[i].m_fileName);
                break;
            }
            case OperationParametersEnum::LABEL:
            {
                LabelFile* myFile = ((LabelParameter*)myParam)->m_parameter;
                myFile->writeFile(outAssociation[i].m_fileName);
                break;
            }
            case OperationParametersEnum::METRIC:
            {
                MetricFile* myFile = ((MetricParameter*)myParam)->m_parameter;
                myFile->writeFile(outAssociation[i].m_fileName);
                break;
            }
            case OperationParametersEnum::STRING:
                cout << "Output String \"" << myParam->m_shortName << "\" value is " << ((StringParameter*)myParam)->m_parameter << endl;
                break;
            case OperationParametersEnum::SURFACE:
            {
                SurfaceFile* myFile = ((SurfaceParameter*)myParam)->m_parameter;
                myFile->writeFile(outAssociation[i].m_fileName);
                break;
            }
            case OperationParametersEnum::VOLUME:
            {
                VolumeFile* myFile = ((VolumeParameter*)myParam)->m_parameter;
                myFile->writeFile(outAssociation[i].m_fileName);
                break;
            }
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
    addComponentDescriptions(ret, myAlgParams, curIndent);
    ret += "\n";//separate prose with a newline
    addHelpProse(ret, myAlgParams, curIndent);
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
    for (int i = 0; i < (int)myComponent->m_repeatableOptions.size(); ++i)
    {
        info += formatString("[" + myComponent->m_repeatableOptions[i]->m_optionSwitch + "] (repeatable)", curIndent, true);
        addHelpComponent(info, &(myComponent->m_repeatableOptions[i]->m_template), curIndent + m_indentIncrement);//indent arguments to options
    }
}

void CommandParser::addHelpProse(AString& info, OperationParameters* myAlgParams, int curIndent)
{//NOTE: does not currently format tabs well, don't use them
    AString* rawProse = &(myAlgParams->getHelpText());//friendlier name
    info += formatString(*rawProse, curIndent, false);//don't indent on added newlines in the prose
}

AString CommandParser::formatString(const AString& in, int curIndent, bool addIndent)
{//NOTE: does not currently format tabs well, don't use them
    AString curIndentString = getIndentString(curIndent);
    bool haveAddedBreak = false;
    AString ret;
    int charMax = m_maxWidth - curIndentString.size();
    int curIndex = 0;
    while (curIndex < in.size())
    {
        if (addIndent)
        {
            if (haveAddedBreak)
            {
                curIndentString = getIndentString(curIndent + m_indentIncrement);
                charMax = m_maxWidth - curIndentString.size();
            } else {
                curIndentString = getIndentString(curIndent);
                charMax = m_maxWidth - curIndentString.size();
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
                if (endIndex > curIndex)
                {//found a space we can break at
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
        if (haveAddedBreak)//don't skip spaces after literal newlines
        {
            while (curIndex < in.size() && in[curIndex] == ' ')
            {//skip spaces
                ++curIndex;
            }
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
    for (int i = 0; i < (int)myComponent->m_repeatableOptions.size(); ++i)
    {
        info += "\n" + formatString("[" + myComponent->m_repeatableOptions[i]->m_optionSwitch + "] - repeatable - " + myComponent->m_repeatableOptions[i]->m_description, curIndent, true);
        addComponentDescriptions(info, &(myComponent->m_repeatableOptions[i]->m_template), curIndent + m_indentIncrement);//indent arguments to options
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
    return m_autoOper->takesParameters();
}

