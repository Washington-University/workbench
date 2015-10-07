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

#include <fstream>
#include <ostream>
#include <iostream>

#include "CaretAssertion.h"
#include "CommandClassCreateAlgorithm.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "ProgramParameters.h"
#include "TextFile.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassCreateAlgorithm::CommandClassCreateAlgorithm()
: CommandClassCreateBase("-class-create-algorithm",
                         "CREATE SOURCE CODE CLASS FILES (.h, .cxx) FOR ALGORITHM")
{
    
}

/**
 * Destructor.
 */
CommandClassCreateAlgorithm::~CommandClassCreateAlgorithm()
{
    
}

/**
 * @return The help information.
 */
AString 
CommandClassCreateAlgorithm::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Create Algorithm Class header (.h) and implementation (.cxx) files.\n"
                        "\n"
                        "Usage:  <algorithm-class-name>\n"
                        "        <command-line-switch>\n"
                        "        <short-description>\n"
                        "\n"
                        "    algorithm-class-name\n"
                        "        Required name of the algorithm class that MUST start with \"Algorithm\"\n"
                        "    \n"
                        "    command-line-switch\n"
                        "        Required command line switch for algorithm.\n"
                        "    \n"
                        "    short-description\n"
                        "        Required short description within double quotes.\n"
                        "    \n"
                        );
    return helpInfo; 
}

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
void 
CommandClassCreateAlgorithm::executeOperation(ProgramParameters& parameters)
{
    const AString algorithmClassName = parameters.nextString("Algorithm Class Name");
    const AString commandLineSwitch  = parameters.nextString("Command Line Switch");
    const AString shortDescription   = parameters.nextString("Short Description");
    if (parameters.hasNext()) {
        const AString param = parameters.nextString("Parameter");
        throw CommandException("Unexpected extra parameter: " + param);
    }
    
    AString errorMessage;
    if (algorithmClassName.isEmpty()) {
        throw CommandException("Algorithm Class Name is empty.");
    }
    else {
        if (algorithmClassName.startsWith("Algorithm") == false) {
            throw CommandException("Algorithm Class Name must start with \"Algorithm\".\n");
        }
        if (algorithmClassName == "Algorithm") {
            throw CommandException("\"Algorithm\" is not allowed for Algorithm Class Name");
        }
    }
    
    if (commandLineSwitch.isEmpty()) {
        throw CommandException("Command line switch is empty.");
    }
    else if (commandLineSwitch.startsWith("-") == false) {
        throw CommandException("Command line must begin with \"-\".");
    }
    
    if (shortDescription.isEmpty()) {
        throw CommandException("Short description is empty.");
    }
    const AString headerFileName = algorithmClassName + ".h";
    const AString implementationFileName = algorithmClassName + ".cxx";
    
    FileInformation headerInfo(headerFileName);
    if (headerInfo.exists()) {
        errorMessage += headerFileName + " exists and this command will not overwrite it.\n";
    }
    FileInformation impInfo(implementationFileName);
    if (impInfo.exists()) {
        errorMessage += implementationFileName + " exists and this command will not overwrite it.\n";
    }
    
    if (errorMessage.isEmpty() == false) {
        throw CommandException(errorMessage);
    }    
    
    AString ifndefName;
    AString ifdefNameStaticDeclarations;
    this->getIfDefNames(algorithmClassName,
                        ifndefName, 
                        ifdefNameStaticDeclarations);
    
    this->createHeaderFile(headerFileName,
                           algorithmClassName,
                           ifndefName);
    
    this->createImplementationFile(implementationFileName,
                                   algorithmClassName,
                                   commandLineSwitch,
                                   shortDescription);
}

/**
 * Create and write the header (.h) file.
 *     
 * @param outputFileName
 *    Name for file that is written.
 * @param algorithmClassName
 *    Name of algorithm type class.
 * @param ifndefName
 *    Name of "ifndef" value.
 */
void 
CommandClassCreateAlgorithm::createHeaderFile(const AString& outputFileName,
                                              const AString& algorithmClassName,
                                              const AString& ifndefName)
{
    AString t;

    t += ("#ifndef " + ifndefName + "\n");
    t += ("#define " + ifndefName + "\n");
    t += this->getCopyright();
    t += ("\n");
    
    t += ("#include \"AbstractAlgorithm.h\"\n");
    t += ("\n");
    t += ("namespace caret {\n");
    t += ("\n");
    t += ("    class " + algorithmClassName + " : public AbstractAlgorithm {\n");
    t += ("\n");
    t += ("    private:\n");
    t += ("        " + algorithmClassName + "(); \n");
    t += ("\n");
    t += ("    protected:\n");
    t += ("        static float getSubAlgorithmWeight();\n");
    t += ("\n");
    t += ("        static float getAlgorithmInternalWeight();\n");
    t += ("\n");
    t += ("    public:\n");
    t += ("        " + algorithmClassName + "(ProgressObject* myProgObj /*INSERT PARAMETERS HERE*/); \n");
    t += ("\n");
    t += ("        static OperationParameters* getParameters();\n");
    t += ("\n");
    t += ("        static void useParameters(OperationParameters* myParams, \n");
    t += ("                                  ProgressObject* myProgObj);\n");
    t += ("\n");
    t += ("        static AString getCommandSwitch();\n");
    t += ("\n");
    t += ("        static AString getShortDescription();\n");
    t += ("\n");
    
    t += ("    };\n");
    t += ("\n");
    t += ("    typedef TemplateAutoOperation<" + algorithmClassName + "> Auto" + algorithmClassName + ";\n");
    t += ("\n");
    t += ("} // namespace\n");
    t += ("\n");
    
    t += ("#endif  //" + ifndefName + "\n");
    t += ("\n");

    TextFile tf;
    tf.replaceText(t);
    
    try {
        tf.writeFile(outputFileName);
    }
    catch (const DataFileException& e) {
        throw CommandException(e);
    }
}

/**
 * Create and write the implementation (.cxx) file.
 *     
 * @param outputFileName
 *    Name for file that is written.
 * @param algorithmClassName
 *    Name of algorithm type class.
 * @param commandLineSwitch
 *    Command line switch for algorithm.
 * @param shortDescription
 *    Short description of algorithm.
 */
void 
CommandClassCreateAlgorithm::createImplementationFile(const AString& outputFileName,
                                                      const AString& algorithmClassName,
                                                      const AString& commandLineSwitch,
                                                      const AString& shortDescription)
{
    AString t;
    
    t += this->getCopyright();
    
    t += ("#include \"CaretAssert.h\"\n");
    t += ("#include \"CaretLogger.h\"\n");
    t += ("\n");
    t += ("#include \"" + algorithmClassName + ".h\"\n");
    t += ("#include \"AlgorithmException.h\"\n");
    t += ("\n");
    t += ("using namespace caret;\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * \\class caret::" + algorithmClassName + " \n");
    t += (" * \\brief " + shortDescription + "\n");
    t += (" *\n");
    t += (" * <REPLACE-WITH-THOROUGH DESCRIPTION>\n");
    t += (" */\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * @return Command line switch\n");
    t += (" */\n");
    t += ("AString\n");
    t += ("" + algorithmClassName + "::getCommandSwitch()\n");
    t += ("{\n");
    t += ("    return \"" + commandLineSwitch + "\";\n");
    t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * @return Short description of algorithm\n");
    t += (" */\n");
    t += ("AString\n");
    t += ("" + algorithmClassName + "::getShortDescription()\n");
    t += ("{\n");
    t += ("    return \"" + shortDescription.toUpper() + "\";\n");
    t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * @return Parameters for algorithm\n");
    t += (" */\n");
    t += ("OperationParameters*\n");
    t += ("" + algorithmClassName + "::getParameters()\n");
    t += ("{\n");
    t += ("    OperationParameters* ret = new OperationParameters();\n");
    t += ("    \n");
    t += ("    /*\n");
    t += ("     * Example parameters:\n");
    t += ("     *\n");
    t += ("     * ret->addSurfaceParameter(1, \"surface\", \"the surface to compute on\");\n");
    t += ("     *\n");
    t += ("     * ret->addMetricOutputParameter(2, \"metric\", \"the output metric\");\n");
    t += ("     *\n");
    t += ("     * OptionalParameter* columnSelect = ret->createOptionalParameter(3, \"-column\", \"select a single column\");\n");
    t += ("     *\n");
    t += ("     * columnSelect->addStringParameter(1, \"column\", \"the column number or name\")\n");
    t += ("     */\n");
    t += ("    AString helpText = (\"This is where you set the help text.  DO NOT add the info about what the command line format is\"\n");
    t += ("                        \"and do not give the command switch, short description, or the short descriptions of parameters.  Do not indent,\"\n");
    t += ("                        \"add newlines, or format the text in any way other than to separate paragraphs within the help text prose\");\n");
    t += ("\n");
    t += ("    ret->setHelpText(helpText);\n");
    t += ("    \n");
    t += ("    return ret;\n");
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Use Parameters and perform algorithm\n");
    t += (" * @param myParams\n");
    t += (" *     Parameters for algorithm\n");
    t += (" * @param myProgObj\n");
    t += (" *     The progress object\n");
    t += (" * @throws\n");
    t += (" *     AlgorithmException if errors\n");
    t += (" */\n");
    t += ("void\n");
    t += ("" + algorithmClassName + "::useParameters(OperationParameters* myParams,\n");
    t += ("                                          ProgressObject* myProgObj)\n");
    t += ("{\n");
    t += ("    /*\n");
    t += ("     * Example parameter processing:\n");
    t += ("     *\n");
    t += ("     * Gets the surface with key 1\n");
    t += ("     * SurfaceFile* mySurf = myParams->getSurface(1);\n");
    t += ("     *\n");
    t += ("     * Gets the output metric with key 2\n");
    t += ("     * MetricFile* myMetricOut = myParams->getOutputMetric(2);\n");
    t += ("     *\n");
    t += ("     * Gets optional parameter with key 3\n");
    t += ("     * OptionalParameter* columnSelect = myParams->getOptionalParameter(3);\n");
    t += ("     * int columnNum = -1;\n");
    t += ("     * if (columnSelect->m_present) {\n");
    t += ("     *     columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));\n");
    t += ("     *     if (columnNum < 0) {\n");
    t += ("     *          throw AlgorithmException(\"invalid column specified\");\n");
    t += ("     *     }\n");
    t += ("     * }\n");
    t += ("     */\n");
    t += ("    \n");
    t += ("    /*\n");
    t += ("     * Constructs and executes the algorithm \n");
    t += ("     */\n");
    t += ("    " + algorithmClassName + "(myProgObj /* INSERT PARAMETERS HERE */);\n");
    t += ("    \n");
    t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * Constructor\n");
    t += (" *\n");
    t += (" * Calling the constructor will execute the algorithm\n");
    t += (" *\n");
    t += (" * @param myProgObj\n");
    t += (" *     Parameters for algorithm\n");
    t += (" */\n");
    t += (algorithmClassName + "::" + algorithmClassName + "(ProgressObject* myProgObj /* INSERT PARAMETERS HERE - may get compilation error if no parameters added */)\n");
    t += ("   : AbstractAlgorithm(myProgObj)\n");
    t += ("{\n");
    t += ("    /*\n");
    t += ("     * Uncomment these if you use another algorithm inside here\n");
    t += ("     *\n");
    t += ("     * ProgressObject* subAlgProgress1 = NULL;\n");
    t += ("     * if (myProgObj != NULL) {\n");
    t += ("     *    subAlgProgress1 = myProgObj->addAlgorithm(AlgorithmInsertNameHere::getAlgorithmWeight());\n");
    t += ("     * }\n");
    t += ("     */\n");
    t += ("    \n");
    t += ("    /*\n");
    t += ("     * Sets the algorithm up to use the progress object, and will \n");
    t += ("     * finish the progress object automatically when the algorithm terminates\n");
    t += ("     */\n");
    t += ("    LevelProgress myProgress(myProgObj);\n");
    t += ("    \n");
    t += ("    /*\n");
    t += ("     * How you say you are halfway done with the INTERNAL work of the algorithm\n");
    t += ("     * will report finished automatically when this function ends (myProgress goes out \n");
    t += ("     of scope, destructor triggers finish\n");
    t += ("     */\n");
    t += ("    //myProgress.reportProgress(0.5f);\n");
    t += ("    \n");
    t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * @return Algorithm internal weight\n");
    t += (" */\n");
    t += ("float\n");
    t += ("" + algorithmClassName + "::getAlgorithmInternalWeight()\n");
    t += ("{\n");
    t += ("    /*\n");
    t += ("     * override this if needed, if the progress bar isn't smooth\n");
    t += ("     */\n");
    t += ("    return 1.0f;\n");
    t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * @return Algorithm sub-algorithm weight\n");
    t += (" */\n");
    t += ("float\n");
    t += ("" + algorithmClassName + "::getSubAlgorithmWeight()\n");
    t += ("{\n");
    t += ("    /*\n");
    t += ("     * If you use a subalgorithm\n");
    t += ("     */\n");
    t += ("    //return AlgorithmInsertNameHere::getAlgorithmWeight()\n");
    t += ("    return 0.0f;\n");
    t += ("}\n");
    t += ("\n");
    
    TextFile tf;
    tf.replaceText(t);
    
    try {
        tf.writeFile(outputFileName);
    }
    catch (const DataFileException& e) {
        throw CommandException(e);
    }
    
    std::cout << std::endl;
    std::cout << "Algorithm was created successfully." << std::endl;
    std::cout << std::endl;
    std::cout << "Add the class files to Algorithms/CMakeLists.txt:" << std::endl;
    std::cout << "   " << qPrintable(algorithmClassName) << ".h" << std::endl;
    std::cout << "   " << qPrintable(algorithmClassName) << ".cxx" << std::endl;
    std::cout << std::endl;
    std::cout << "Add the header file and algorithm to Commands/CommandOperationManager.cxx:" << std::endl;
    std::cout << "   #include \"" << qPrintable(algorithmClassName) << ".h\"" << std::endl;
    std::cout << "   this->commandOperations.push_back(new CommandParser(new Auto" << algorithmClassName << "()));" << std::endl;
    std::cout << std::endl;
}

