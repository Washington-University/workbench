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

#include <QDir>

#include "CaretAssertion.h"
#include "CommandDevCreateResourceFile.h"
#include "FileInformation.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandDevCreateResourceFile::CommandDevCreateResourceFile()
: CommandOperation("-dev-create-resource-file",
                         "DEVELOPMENT CREATE RESOURCE FILE (.qrc)")
{
    
}

/**
 * Destructor.
 */
CommandDevCreateResourceFile::~CommandDevCreateResourceFile()
{
    
}

AString 
CommandDevCreateResourceFile::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Development - Create resource file (.qrc) file.\n"
                        "\n"
                        "Usage:  <class-name> \n"
                        "        [-copy] \n"
                        "        [-event-class <event-type-enum>]\n"
                        "        [-event-listener] \n"
                        "        [-no-parent] \n"
                        "        [-parent <parent-class-name>] \n"
                        "\n"
                        "\n"
                        "Options: \n"
                        "    -copy\n"
                        "        Adds copy constructor and assignment operator\n"
                        "    \n"
                        "    -event-class <event-type-enum>\n"
                        "        When creating an Event subclass, using this\n"
                        "        option will automatically set the parent\n"
                        "        class to Event and place the given event\n"
                        "        enumerated type value into the parameter\n"
                        "        for the Event class constructor.\n"
                        "        \n"
                        "        For the <event-type-enum> there is no need\n"
                        "        to prepend it with \"EventTypeEnum::\".\n"
                        "        \n"
                        "    -event-listener \n"
                        "        Implement the EventListenerInterface so\n"
                        "        that the class may listen for events.\n"
                        "        \n"
                        "    -no-parent\n"
                        "        Created class is not derived from any other\n"
                        "        class.  By default, the parent class is\n"
                        "        CaretObject.\n"
                        "    \n"
                        "    -parent <parent-class-name>\n"
                        "        Specify the parent (derived from) class.\n"
                        "        By default, the parent class is CaretObject.\n"
                        "    \n"
                        "    -scene\n"
                        "        Implement the SceneableInterface so that \n"
                        "        instances of the class can be restored from \n"
                        "        and saved to scenes. \n"
                        "    \n"
                        "    -scene-sub-class\n"
                        "        Adds methods that can be called by the super- \n"
                        "        class so that this sub-class can save and \n"
                        "        restore data to and from scenes.  \n"
                        "        \n"
                        "        This option should only be used when creating\n"
                        "        a class whose super class implements the \n"
                        "        SceneableInterface\n"
                        "        \n"
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
CommandDevCreateResourceFile::executeOperation(ProgramParameters& parameters) throw (CommandException,
                                                               ProgramParametersException)
{
    const AString className = parameters.nextString("Class Name");
    AString derivedFromClassName = "CaretObject";
    AString eventTypeEnumName;
    
    bool hasCopyAndAssignment = false;
    bool hasEventListener = false;
    bool hasScenes = false;
    bool hasScenesSubClass = false;
    
    while (parameters.hasNext()) {
        const AString& param = parameters.nextString("Create Class Parameter");
        if (param == "-copy") {
            hasCopyAndAssignment = true;
        }
        else if (param == "-event-class") {
            eventTypeEnumName = parameters.nextString("Event Type Enum Name");
            if (eventTypeEnumName.contains("::") == false) {
                eventTypeEnumName.insert(0,
                                         "EventTypeEnum::");
            }
        }
        else if (param == "-event-listener") {
            hasEventListener = true;
        }
        else if (param == "-parent") {
            derivedFromClassName = parameters.nextString("Parent Class Name");
            if (derivedFromClassName.isEmpty()) {
                throw CommandException("Parent class name is empty.");
            }
            else {
                if (derivedFromClassName[0].isUpper() == false) {
                    throw CommandException("Parent class name must begin with a Capital Letter");
                }
            }
        }
        else if (param == "-no-parent") {
            derivedFromClassName = "";
        }
        else if (param == "-scene") {
            hasScenes = true;
        }
        else if (param == "-scene-sub-class") {
            hasScenesSubClass = true;
        }
        else {
            throw CommandException("Invalid parameter: " + param);
        }
    }
    
    if (hasScenes
        && hasScenesSubClass) {
        throw CommandException("Only one, but not both scene options "
                               "may be specified: "
                               "-scene  -scene-sub-class");
    }
    
    if (className.isEmpty()) {
        throw CommandException("class name is empty.");
    }
    
    AString errorMessage;
    if (eventTypeEnumName.isEmpty() == false) {
        derivedFromClassName = "Event";
        if (className.startsWith("Event") == false) {
            errorMessage += ("Event classes must being with \"Event\"\n");
        }
    }
    if (className[0].isUpper() == false) {
        errorMessage += "First letter of class name must be upper case.\n";
    }
    
    const AString headerFileName = className + ".h";
    const AString implementationFileName = className + ".cxx";
    
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
    
}

