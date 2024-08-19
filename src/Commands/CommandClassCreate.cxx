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
#include "CommandClassCreate.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "ProgramParameters.h"
#include "TextFile.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassCreate::CommandClassCreate()
: CommandClassCreateBase("-class-create",
                         "CREATE CLASS SOURCE CODE FILES (.h and .cxx)")
{
    
}

/**
 * Destructor.
 */
CommandClassCreate::~CommandClassCreate()
{
    
}

AString 
CommandClassCreate::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Create class header (.h) and implementation (.cxx) files.\n"
                        "\n"
                        "Usage:  <class-name> \n"
                        "        [-copy] \n"
                        "        [-equal] \n"
                        "        [-event-class <event-type-enum>]\n"
                        "        [-event-listener] \n"
                        "        [-no-parent] \n"
                        "        [-parent <parent-class-name>] \n"
                        "        [-scene] \n"
                        "        [-scene-sub-class] \n"
                        "\n"
                        "\n"
                        "Options: \n"
                        "    -copy\n"
                        "        Adds copy constructor and assignment operator\n"
                        "    \n"
                        "    -equal\n"
                        "        Adds equality operator.\n"
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
CommandClassCreate::executeOperation(ProgramParameters& parameters)
{
    const AString className = parameters.nextString("Class Name");
    AString derivedFromClassName = "CaretObject";
    AString eventTypeEnumName;
    
    bool hasCopyAndAssignment = false;
    bool hasEqualityOperator = false;
    bool hasEventListener = false;
    bool hasScenes = false;
    bool hasScenesSubClass = false;
    
    while (parameters.hasNext()) {
        const AString& param = parameters.nextString("Create Class Parameter");
        if (param == "-copy") {
            hasCopyAndAssignment = true;
        }
        else if (param == "-equal") {
            hasEqualityOperator = true;
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
    
    AString ifndefName;
    AString ifdefNameStaticDeclarations;
    this->getIfDefNames(className, 
                        ifndefName, 
                        ifdefNameStaticDeclarations);
    
    this->createHeaderFile(headerFileName,
                           className, 
                           derivedFromClassName,
                           ifndefName, 
                           ifdefNameStaticDeclarations, 
                           hasCopyAndAssignment,
                           hasEqualityOperator,
                           hasEventListener,
                           hasScenes,
                           hasScenesSubClass);
    
    this->createImplementationFile(implementationFileName,
                                   className, 
                                   derivedFromClassName,
                                   eventTypeEnumName,
                                   ifdefNameStaticDeclarations, 
                                   hasCopyAndAssignment,
                                   hasEqualityOperator,
                                   hasEventListener,
                                   hasScenes,
                                   hasScenesSubClass);
}

/**
 * Create and write the header (.h) file.
 *     
 * @param outputFileName
 *    Name for file that is written.
 * @param className
 *    Name of class.
 * @param derivedFromClassName
 *    Name of class from which this class is derived.
 * @param ifdefName
 *    Name of "ifndef" value.
 * @param ifdefNameStaticDeclaration
 *    Name for "infdef" of static declarations.
 * @param hasCopyAndAssignment
 *    Has copy constructor and assignment operator.
 * @param hasEqualityOperator
 *    Class implements equality operator
 * @param hasEventListener
 *    Class implements the EventListener interface 
 * @param hasSceneInterface
 *    Class implements the SceneableInterface for scene support.
 * @param hasSubClassSceneSaving
 *    Parent class implements the SceneableInterface so add methods
 *    for saving sub-class data to scene that will be called by parent.
 */
void 
CommandClassCreate::createHeaderFile(const AString& outputFileName,
                                     const AString& className,
                                     const AString& derivedFromClassName,
                                     const AString& ifndefName,
                                     const AString& ifdefNameStaticDeclaration,
                                     const bool hasCopyAndAssignment,
                                     const bool hasEqualityOperator,
                                     const bool hasEventListener,
                                     const bool hasSceneInterface,
                                     const bool hasSubClassSceneSaving)
{
    AString t;
    
    AString derivedFromDeclaration;
    if (derivedFromClassName.isEmpty() == false) {
        derivedFromDeclaration = (" : public " + derivedFromClassName);
    }
    
    t += ("#ifndef " + ifndefName + "\n");
    t += ("#define " + ifndefName + "\n");
    t += this->getCopyright();
    t += ("\n");
    if (m_useUniquePtrFlag) {
        t += ("\n");
        t += ("#include <memory>\n");
        t += ("\n");
    }
    
    t += (this->getIncludeDeclaration(derivedFromClassName) + "\n");
    t += ("\n");    
    if (hasEventListener) {
        t += (this->getIncludeDeclaration("EventListenerInterface") + "\n");
        if (derivedFromDeclaration.isEmpty()) {
            derivedFromDeclaration += (" : ");
        }
        else {
            derivedFromDeclaration += (", ");
        }
        derivedFromDeclaration += ("public EventListenerInterface");
    }
    if (hasSceneInterface) {
        t += (this->getIncludeDeclaration("SceneableInterface") + "\n");
        if (derivedFromDeclaration.isEmpty()) {
            derivedFromDeclaration += (" : ");
        }
        else {
            derivedFromDeclaration += (", ");
        }
        derivedFromDeclaration += ("public SceneableInterface");
    }
    t += ("\n");
    t += ("\n");
    t += ("namespace caret {\n");
    if (hasSceneInterface) {
        t += ("    class SceneClassAssistant;\n");
    }
    t += ("\n");

    t += ("    class " + className + derivedFromDeclaration + " {\n");
    t += ("        \n");
    if (derivedFromClassName.startsWith("Q")
        || derivedFromClassName.startsWith("WuQ")) {
        t += ("        Q_OBJECT\n");
        t += ("\n");
    }
    t += ("    public:\n");
    t += ("        " + className + "();\n");
    t += ("        \n");
    t += ("        virtual ~" + className + "();\n");
    t += ("        \n");
    
    if (hasCopyAndAssignment) {
        t += ("        " + className + "(const " + className + "& obj);\n");
        t += ("\n");
        t += ("        " + className + "& operator=(const " + className + "& obj);\n");
        t += ("        \n");
    }
    else {
        t += ("        " + className + "(const " + className + "&) = delete;\n");
        t += ("\n");
        t += ("        " + className + "& operator=(const " + className + "&) = delete;\n");
        t += ("        \n");
    }
    
    if (hasEqualityOperator) {
        t += ("        bool operator==(const " + className + "& obj) const;\n");
        t += ("        \n");
    }
    
    t += ("\n");
    t += ("        " + getNewMethodsString() + "\n");
    t += ("\n");          

    if (derivedFromClassName == "CaretObject") {
        t += ("        virtual AString toString() const;\n");
        t += ("        \n");
    }
    if (hasEventListener) {
        t += ("        virtual void receiveEvent(Event* event);\n");
        t += ("\n");
    }
    if (hasSceneInterface) {
        t += ("        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,\n");
        t += ("                                        const AString& instanceName);\n");
        t += ("\n");
        t += ("        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,\n");
        t += ("                                      const SceneClass* sceneClass);\n");
        t += ("\n");
    }
    
    if (hasSceneInterface
        || hasSubClassSceneSaving) {
        AString comment = "";
        AString virtualZero ="";
        if (hasSceneInterface) {
            comment = "//";
            virtualZero = " = 0";
        }
        t += ("          \n");
        t += ("          \n");
        t += ("          \n");
        t += ("          \n");
        t += ("          \n");
        if (hasSceneInterface) {
            t += ("// If there will be sub-classes of this class that need to save\n");
            t += ("// and restore data from scenes, these pure virtual methods can\n");
            t += ("// be uncommented to force their implementation by sub-classes.\n");
        }
        t += (comment + "    protected: \n");
        t += (comment + "        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,\n");
        t += (comment + "                                             SceneClass* sceneClass)" + virtualZero + ";\n");
        t += (comment + "\n");
        t += (comment + "        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,\n");
        t += (comment + "                                                  const SceneClass* sceneClass)" + virtualZero + ";\n");
        t += ("\n");
    }
    
    t += ("    private:\n");
    
    if (hasCopyAndAssignment) {
        t += ("        void copyHelper" + className + "(const " + className + "& obj);\n");
        t += ("\n");
    }
    if (hasSceneInterface
        || hasSubClassSceneSaving) {
        if (m_useUniquePtrFlag) {
            t += ("        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;\n");
        }
        else {
            t += ("        SceneClassAssistant* m_sceneAssistant;\n");
        }
        t += ("\n");
    }
    t += ("        " + getNewMembersString() + "\n");
    t += ("\n");
    t += ("    };\n");
    t += ("    \n");
    t += ("#ifdef " + ifdefNameStaticDeclaration + "\n");
    t += ("    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>\n");
    t += ("#endif // " + ifdefNameStaticDeclaration + "\n");
    t += ("\n");
    t += ("} // namespace\n");
    
    t += ("#endif  //" + ifndefName + "\n");
    
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
 * @param className
 *    Name of class.
 * @param derivedFromClassName
 *    Name of parent class
 * @param eventTypeEnumName
 *    Name of event type enumerated type (if subclass of Event).
 * @param ifdefNameStaticDeclaration
 *    Name for "infdef" of static declarations.
 * @param hasCopyAndAssignment
 *    Has copy constructor and assignment operator.
 * @param hasEqualityOperator 
 *    Class implements equality operator
 * @param hasEventListener
 *    Class implements the EventListener interface
 * @param hasSceneInterface
 *    Class implements the  SceneableInterface for scene support.
 * @param hasSubClassSceneSaving
 *    Parent class implements the SceneableInterface so add methods
 *    for saving sub-class data to scene that will be called by parent.
 */
void 
CommandClassCreate::createImplementationFile(const AString& outputFileName,
                                             const AString& className,
                                             const AString& derivedFromClassName,
                                             const AString& eventTypeEnumName,
                                             const AString& ifdefNameStaticDeclaration,
                                             const bool hasCopyAndAssignment,
                                             const bool hasEqualityOperator,
                                             const bool hasEventListener,
                                             const bool hasSceneInterface,
                                             const bool hasSubClassSceneSaving)
{
    AString module;
    FileInformation dirInfo(QDir::currentPath());
    if (dirInfo.exists()) {
        if (dirInfo.isDirectory()) {
            module = dirInfo.getFileName();
        }
    }
    AString t;
    
    t += this->getCopyright();
    
    t += ("#define " + ifdefNameStaticDeclaration + "\n");
    t += ("#include \"" + className + ".h\"\n");
    t += ("#undef " + ifdefNameStaticDeclaration + "\n");
    t += ("\n");
    t += (this->getIncludeDeclaration("CaretAssert") + "\n");
    if (eventTypeEnumName.isEmpty() == false) {
        t += ("#include \"EventTypeEnum.h\"\n");
        t += ("\n");
    }
    if (hasEventListener) {
        t += (this->getIncludeDeclaration("EventManager") + "\n");
    }
    if (hasSceneInterface
        || hasSubClassSceneSaving) {
        t += (this->getIncludeDeclaration("SceneClass") + "\n");
        t += (this->getIncludeDeclaration("SceneClassAssistant") + "\n");
        t += ("\n");
    }
    t += ("using namespace caret;\n");
    t += ("\n");
    t += ("\n");
    t += ("    \n");
    t += ("/**\n");
    t += (" * \\class caret::" + className + " \n");
    t += (" * \\brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>\n");
    if (module.isEmpty() == false) {
        t += (" * \\ingroup " + module + "\n");
    }
    t += (" *\n");
    t += (" * <REPLACE-WITH-THOROUGH DESCRIPTION>\n");
    t += (" */\n"); 
    t += ("\n");
    t += ("/**\n");
    t += (" * Constructor.\n");
    t += (" */\n");
    t += ("" + className + "::" + className + "()\n");
    if (derivedFromClassName.isEmpty() == false) {
        t += (": " + derivedFromClassName + "("+ eventTypeEnumName + ")\n");
    }
    t += ("{\n");
    t += ("    \n");
    if (hasSceneInterface
        || hasSubClassSceneSaving) {
        if (m_useUniquePtrFlag) {
            t += ("    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());\n");
        }
        else {
            t += ("    m_sceneAssistant = new SceneClassAssistant();\n");
        }
        t += ("    \n");
    }
    if (hasEventListener) {
        t += ("//    EventManager::get()->addEventListener(this, EventTypeEnum::);\n");
    }
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Destructor.\n");
    t += (" */\n");
    t += ("" + className + "::~" + className + "()\n");
    t += ("{\n");
    if (hasEventListener) {
        t += ("    EventManager::get()->removeAllEventsFromListener(this);\n");
    }
    if (hasSceneInterface
        || hasSubClassSceneSaving) {
        if ( ! m_useUniquePtrFlag) {
            t += ("    delete m_sceneAssistant;\n");
        }
    }
    t += ("}\n");
    t += ("\n");
    
    if (hasCopyAndAssignment) {
        t += ("/**\n");
        t += (" * Copy constructor.\n");
        t += (" * @param obj\n");
        t += (" *    Object that is copied.\n");
        t += (" */\n");
        t += ("" + className + "::" + className + "(const " + className + "& obj)\n");
        if ( ! derivedFromClassName.isEmpty()) {
            t += (": " + derivedFromClassName + "(obj)\n");
        }
        t += ("{\n");
        t += ("    this->copyHelper" + className + "(obj);\n");
        t += ("}\n");
        t += ("\n");
        t += ("/**\n");
        t += (" * Assignment operator.\n");
        t += (" * @param obj\n");
        t += (" *    Data copied from obj to this.\n");
        t += (" * @return \n");
        t += (" *    Reference to this object.\n");
        t += (" */\n");
        t += ("" + className + "&\n");
        t += ("" + className + "::operator=(const " + className + "& obj)\n");
        t += ("{\n");
        t += ("    if (this != &obj) {\n");
        if ( ! derivedFromClassName.isEmpty()) {
            t += ("        " + derivedFromClassName + "::operator=(obj);\n");
        }
        t += ("        this->copyHelper" + className + "(obj);\n");
        t += ("    }\n");
        t += ("    return *this;    \n");
        t += ("}\n");
        t += ("\n");
        t += ("/**\n");
        t += (" * Helps with copying an object of this type.\n");
        t += (" * @param obj\n");
        t += (" *    Object that is copied.\n");
        t += (" */\n");
        t += ("void \n");
        t += ("" + className + "::copyHelper" + className + "(const " + className + "& obj)\n");
        t += ("{\n");
        t += ("    \n");
        t += ("}\n");
        t += ("\n");
    }
    
    if (hasEqualityOperator) {
        t += ("/**\n");
        t += (" * Equality operator.\n");
        t += (" * @param obj\n");
        t += (" *    Instance compared to this for equality.\n");
        t += (" * @return \n");
        t += (" *    True if this instance and 'obj' instance are considered equal.\n");
        t += (" */\n");
        t += ("bool\n");
        t += ("" + className + "::operator==(const " + className + "& obj) const\n");
        t += ("{\n");
        t += ("    if (this == &obj) {\n");
        t += ("        return true;    \n");
        t += ("    }\n");
        t += ("\n");
        t += ("    /* perform equality testing HERE and return true if equal ! */\n");
        t += ("\n");
        t += ("    return false;    \n");
        t += ("}\n");
        t += ("\n");
    }
    
    if (derivedFromClassName == "CaretObject") {
        t += ("/**\n");
        t += (" * Get a description of this object's content.\n");
        t += (" * @return String describing this object's content.\n");
        t += (" */\n");
        t += ("AString \n");
        t += ("" + className + "::toString() const\n");
        t += ("{\n");
        t += ("    return \"" + className + "\";\n");
        t += ("}\n");
        t += ("\n");
    }

    if (hasEventListener) {
        t += ("/**\n");
        t += (" * Receive an event.\n");
        t += (" *\n");
        t += (" * @param event\n");
        t += (" *    An event for which this instance is listening.\n");
        t += (" */\n");
        t += ("void\n");
        t += (className + "::receiveEvent(Event* event)\n");
        t += ("{\n");
        t += ("//    if (event->getEventType() == EventTypeEnum::) {\n");
        t += ("//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);\n");
        t += ("//        CaretAssert(eventName);\n");
        t += ("//\n");
        t += ("//        event->setEventProcessed();\n");
        t += ("//    }\n");
        t += ("}\n");
        t += ("\n");
    }
    
    if (hasSceneInterface) {
        t += ("/**\n");
        t += (" * Save information specific to this type of model to the scene.\n");
        t += (" *\n");
        t += (" * @param sceneAttributes\n");
        t += (" *    Attributes for the scene.  Scenes may be of different types\n");
        t += (" *    (full, generic, etc) and the attributes should be checked when\n");
        t += (" *    saving the scene.\n");
        t += (" *\n");
        t += (" * @param instanceName\n");
        t += (" *    Name of instance in the scene.\n");
        t += (" */\n");
        t += ("SceneClass*\n");
        t += (className + "::saveToScene(const SceneAttributes* sceneAttributes,\n");
        t += ("                                 const AString& instanceName)\n");
        t += ("{\n");
        t += ("    SceneClass* sceneClass = new SceneClass(instanceName,\n");
        t += ("                                            \"" + className + "\",\n");
        t += ("                                            1);\n");
        t += ("    m_sceneAssistant->saveMembers(sceneAttributes,\n");
        t += ("                                  sceneClass);\n");
        t += ("    \n");
        t += ("    // Uncomment if sub-classes must save to scene\n");
        t += ("    //saveSubClassDataToScene(sceneAttributes,\n");
        t += ("    //                        sceneClass);\n");
        t += ("    \n");
        t += ("    return sceneClass;\n");
        t += ("}\n");
        t += ("\n");
        t += ("/**\n");
        t += (" * Restore information specific to the type of model from the scene.\n");
        t += (" *\n");
        t += (" * @param sceneAttributes\n");
        t += (" *    Attributes for the scene.  Scenes may be of different types\n");
        t += (" *    (full, generic, etc) and the attributes should be checked when\n");
        t += (" *    restoring the scene.\n");
        t += (" *\n");
        t += (" * @param sceneClass\n");
        t += (" *     sceneClass from which model specific information is obtained.\n");
        t += (" */\n");
        t += ("void\n");
        t += (className + "::restoreFromScene(const SceneAttributes* sceneAttributes,\n");
        t += ("                                      const SceneClass* sceneClass)\n");
        t += ("{\n");
        t += ("    if (sceneClass == NULL) {\n");
        t += ("        return;\n");
        t += ("    }\n");
        t += ("    \n");
        t += ("    m_sceneAssistant->restoreMembers(sceneAttributes,\n");
        t += ("                                     sceneClass);    \n");    
        t += ("    \n");
        t += ("    //Uncomment if sub-classes must restore from scene\n");
        t += ("    //restoreSubClassDataFromScene(sceneAttributes,\n");
        t += ("    //                             sceneClass);\n");
        t += ("    \n");
        t += ("}\n");
        t += ("\n");
    }
    
    if (hasSubClassSceneSaving) {
        t += ("/**\n");
        t += (" * Save subclass data to the scene.\n");
        t += (" *\n");
        t += (" * @param sceneAttributes\n");
        t += (" *    Attributes for the scene.  Scenes may be of different types\n");
        t += (" *    (full, generic, etc) and the attributes should be checked when\n");
        t += (" *    restoring the scene.\n");
        t += (" *\n");
        t += (" * @param sceneClass\n");
        t += (" *     sceneClass to which data members should be added.  Will always\n");
        t += (" *     be valid (non-NULL).\n");
        t += (" */\n");
        t += ("void\n");
        t += (className + "::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,\n");
        t += ("                                            SceneClass* sceneClass)\n");
        t += ("{\n");
        t += ("    m_sceneAssistant->saveMembers(sceneAttributes,\n");
        t += ("                                  sceneClass);\n");
        t += ("}\n");
        t += ("\n");
        
        t += ("/**\n");
        t += (" * Restore file data from the scene.\n");
        t += (" *\n");
        t += (" * @param sceneAttributes\n");
        t += (" *    Attributes for the scene.  Scenes may be of different types\n");
        t += (" *    (full, generic, etc) and the attributes should be checked when\n");
        t += (" *    restoring the scene.\n");
        t += (" *\n");
        t += (" * @param sceneClass\n");
        t += (" *     sceneClass for the instance of a class that implements\n");
        t += (" *     this interface.  Will NEVER be NULL.\n");
        t += (" */\n");
        t += ("void\n");
        t += (className + "::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,\n");
        t += ("                                                 const SceneClass* sceneClass)\n");
        t += ("{\n");
        t += ("    m_sceneAssistant->restoreMembers(sceneAttributes,\n");
        t += ("                                     sceneClass);\n");
        t += ("}\n");
        t += ("\n");
    }
    
    TextFile tf;
    tf.replaceText(t);
    
    try {
        tf.writeFile(outputFileName);
    }
    catch (const DataFileException& e) {
        throw CommandException(e);
    }
}


