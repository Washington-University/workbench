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


#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#ifndef WORKBENCH_USE_QT5_QOPENGL_WIDGET
#include <QGLPixelBuffer>
#endif
#include <QSplashScreen>
#include <QStyleFactory>
#include <QThread>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "ApplicationInformation.h"
#include "BrainBrowserWindow.h"
#include "BrainOpenGLWidget.h"
#include "CaretAssert.h"
#include "CaretCommandLine.h"
#include "CaretHttpManager.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CommandOperationManager.h"
#include "EventBrowserWindowNew.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GuiManager.h"
#include "MacApplication.h"
#include "ProgramParameters.h"
#include "SessionManager.h"
#include "SplashScreen.h"
#include "SystemUtilities.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

static bool caretLoggerIsValid = false;

using namespace caret;
using namespace std;

#if QT_VERSION >= 0x050000
/**
 * Handles message produced by Qt 5
 */
static void
messageHandlerForQt5(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    const AString backtrace = SystemUtilities::getBackTrace();
    
    const AString contextInfo = ("   Context Info File ("
                                 + QString(context.file)
                                 + ") Function (" + QString(context.function)
                                 + ") Line (" + QString::number(context.line)
                                 + ") Version (" + QString::number(context.version)
                                 + ") Category (" + QString(context.category)
                                 + ")");
    const AString message = (AString(msg) + "\n"
                             + contextInfo + "\n"
                             + backtrace);
    
    if (caretLoggerIsValid) {
        bool abortFlag = false;
        bool displayedFlag = false;
        switch (type) {
            case QtDebugMsg:
                CaretLogInfo(message);
                displayedFlag = CaretLogger::getLogger()->isInfo();
                break;
            case QtWarningMsg:
                CaretLogWarning(message);
                displayedFlag = CaretLogger::getLogger()->isWarning();
                break;
            case QtCriticalMsg:
                CaretLogSevere(message);
                displayedFlag = CaretLogger::getLogger()->isSevere();
                break;
            case QtFatalMsg:
                cerr << "Qt Fatal: " << message << endl;
                abortFlag = true;//fatal will cause an abort, so always display it, bypassing logger entirely
                displayedFlag = true;
                break;
#if QT_VERSION >= 0x050500
            case QtInfoMsg:
                CaretLogInfo(message);
                displayedFlag = CaretLogger::getLogger()->isInfo();
                break;
#endif
        }
        
        /*
         * Beep to alert user about an error!!!
         */
        if (displayedFlag && (type != QtDebugMsg))//don't beep for debug
        {
            GuiManager::beep();
        }
#ifndef NDEBUG
        if (!displayedFlag)
        {
            cerr << "DEBUG: Qt ";
            switch (type)
            {
                case QtDebugMsg:
                    cerr << "Debug ";
                    break;
                case QtWarningMsg:
                    cerr << "Warning ";
                    break;
                case QtCriticalMsg:
                    cerr << "Critical ";
                    break;
                case QtFatalMsg:
                    cerr << "FATAL (?!?) ";//should never happen
                    break;
#if QT_VERSION >= 0x050500
                case QtInfoMsg:
                    std::cerr << "Info ";
                    break;
#endif
            }
            cerr << "message hidden" << endl;
        }
#endif
        
        if (abortFlag) {
            std::abort();
        }
    }
    else {
        switch (type) {
            case QtDebugMsg:
                std::cerr << "Qt Debug: " << message << std::endl;
                break;
            case QtWarningMsg:
                std::cerr << "Qt Warning: " << message << std::endl;
                break;
            case QtCriticalMsg:
                std::cerr << "Qt Critical: " << message << std::endl;
                break;
            case QtFatalMsg:
                std::cerr << "Qt Fatal: " << message << std::endl;
                std::abort();
                break;
#if QT_VERSION >= 0x050500
            case QtInfoMsg:
                std::cerr << "Qt Info: " << message << std::endl;
                break;
#endif
        }
    }
}

#else // QT_VERSION

/**
 * Handles message produced by Qt 4.
 */
static void
messageHandlerForQt4(QtMsgType type, const char* msg)
{
    const AString backtrace = SystemUtilities::getBackTrace();
    
    const AString message = (AString(msg) + "\n" + backtrace);
    
    if (caretLoggerIsValid) {
        bool abortFlag = false;
        bool displayedFlag = false;
        switch (type) {
            case QtDebugMsg:
                CaretLogInfo(message);
                displayedFlag = CaretLogger::getLogger()->isInfo();
                break;
            case QtWarningMsg:
                CaretLogWarning(message);
                displayedFlag = CaretLogger::getLogger()->isWarning();
                break;
            case QtCriticalMsg:
                CaretLogSevere(message);
                displayedFlag = CaretLogger::getLogger()->isSevere();
                break;
            case QtFatalMsg:
                cerr << "Qt Fatal: " << message << endl;
                abortFlag = true;//fatal will cause an abort, so always display it, bypassing logger entirely
                displayedFlag = true;
                break;
        }
        
        /*
         * Beep to alert user about an error!!!
         */
        if (displayedFlag && (type != QtDebugMsg))//don't beep for debug
        {
            GuiManager::beep();
        }
#ifndef NDEBUG
        if (!displayedFlag)
        {
            cerr << "DEBUG: Qt ";
            switch (type)
            {
                case QtDebugMsg:
                    cerr << "Debug ";
                    break;
                case QtWarningMsg:
                    cerr << "Warning ";
                    break;
                case QtCriticalMsg:
                    cerr << "Critical ";
                    break;
                case QtFatalMsg:
                    cerr << "FATAL (?!?) ";//should never happen
                    break;
            }
            cerr << "message hidden" << endl;
        }
#endif
        
        if (abortFlag) {
            std::abort();
        }
    }
    else {
        switch (type) {
            case QtDebugMsg:
                std::cerr << "Qt Debug: " << message << std::endl;
                break;
            case QtWarningMsg:
                std::cerr << "Qt Warning: " << message << std::endl;
                break;
            case QtCriticalMsg:
                std::cerr << "Qt Critical: " << message << std::endl;
                break;
            case QtFatalMsg:
                std::cerr << "Qt Fatal: " << message << std::endl;
                std::abort();
                break;
        }
    }
}

#endif // QT_VERSION

//struct for communicating stuff back to main from parseCommandLine
struct ProgramState
{
    vector<AString> fileList;
    int windowSizeXY[2];
    int windowPosXY[2];
    int graphicsSizeXY[2];
    bool showSplash;
    
    AString specFileNameLoadWithDialog;
    AString specFileNameLoadAll;
    
    AString sceneFileName;
    AString sceneNameOrNumber;
    
    ProgramState();
};


//declare the functions associated with command line
void printHelp(const AString& progName);
void parseCommandLine(const AString& progName, ProgramParameters* myParams, ProgramState& myState);
int 
main(int argc, char* argv[])
{
    srand(time(NULL));
    int result;
    {
        /*
        * Handle uncaught exceptions
        */
        SystemUtilities::setUnexpectedHandler();
        
        /*
        * Create the session manager.
        */
        SessionManager::createSessionManager(ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE);
        caretLoggerIsValid = true;

        /*
        * Parameters for the program.
        */
        ProgramParameters parameters(argc, argv);
        caret_global_commandLine_init(parameters);

        //begin parsing command line
        ProgramState myState;
        FileInformation progInfo(argv[0]);
        AString progName = progInfo.getFileName();
        parseCommandLine(progName, &parameters, myState);
        
        /*
        * Log the command parameters.
        */
        CaretLogFine("Running: " + caret_global_commandLine);
        
        /*
         * Setup OpenGL if using Qt 5's QOpenGLWidget.
         * QOpenGLWidget's documentation indicates that
         * default format must be initizlied before
         * QApplication is created.
         */
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        BrainOpenGLWidget::initializeDefaultGLFormat();
#endif
        
        //change the default graphics system on mac to avoid rendering performance issues with qwtplotter
#ifdef CARET_OS_MACOSX
        // Qt-Deprecated: QApplication::setGraphicsSystem("raster");
        MacApplication app(argc, argv);
#else //CARET_OS_MACOSX        
        QApplication app(argc, argv);
#endif //CARET_OS_MACOSX
        
        ApplicationInformation applicationInformation;

        QApplication::addLibraryPath(
            QApplication::applicationDirPath()
            + QDir::separator()
            + "plugins");
        QApplication::setApplicationName(applicationInformation.getName());
        QApplication::setApplicationVersion(applicationInformation.getVersion());
        QApplication::setOrganizationDomain("brainvis.wustl.edu");
        QApplication::setOrganizationName("Van Essen Lab");
        /*
         * Override the system local to US - English
         */
        QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
        
        /*
        * Make sure OpenGL is available.
        */
        const QString noOpenGLMessage("OpenGL (3D Graphics System) is not available.  "
                                      "This may be caused by missing or outdated OpenGL libraries."
        
#ifdef CARET_OS_LINUX
                                      "On Linux, this may be caused by a missing plugin "
                                      "library <plugins-path>/xcbglintegrations/libqxcb-glx-integration.so."
#endif
                                      );
        
                                      
#ifndef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        if ( ! QGLFormat::hasOpenGL()) {
            app.processEvents();
            WuQMessageBox::errorOk(NULL,
                                   noOpenGLMessage);
            app.processEvents();
            
            return -1;
        }
#endif
            
        /*
        * Setup OpenGL if NOT using Qt 5's QOpenGLWidget
        */
#ifndef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        BrainOpenGLWidget::initializeDefaultGLFormat();
#endif
        
#if QT_VERSION >= 0x050000
        qInstallMessageHandler(messageHandlerForQt5);//this handler uses CaretLogger and GuiManager, so we must install it after the logger is available and the application is created
#else // QT_VERSION
        qInstallMsgHandler(messageHandlerForQt4);//this handler uses CaretLogger and GuiManager, so we must install it after the logger is available and the application is created
#endif // QT_VERSION
        /*
         * Log debug status
         */
        CaretLogConfig(applicationInformation.getCompiledWithDebugStatus());
        
        /*
         * Enabled the desired splash screen based upon user preferences
         * and command line options.  Do not show selection splash screen
         * if there has listed files on the command line.
         */ 
        CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
        bool showSelectionSplashScreen = preferences->isSplashScreenEnabled();
        bool showImageSplashScreen = (! showSelectionSplashScreen);
        if ( ! myState.showSplash) {
            showSelectionSplashScreen = false;
            showImageSplashScreen = false;
        }
        
        /*
         * DISABLE IMAGE SPLASH SCREEN
         */
        showImageSplashScreen = false;
        
        /*
        * Splash Screen
        */
        QPixmap splashPixmap;
        QSplashScreen splashScreen;
        if (showImageSplashScreen) {
            if (WuQtUtilities::loadPixmap(":/Splash/hcp.png", splashPixmap)) {
                splashScreen.setPixmap(splashPixmap);
                splashScreen.showMessage("Starting Workbench...");
                splashScreen.show();
                app.processEvents();
                SystemUtilities::sleepSeconds(2);
            }
        }
        
        /*
         * Create the GUI Manager.
         */
        GuiManager::createGuiManager();
        
        /*
         * Letting the App process events will allow the message for a 
         * double-clicked spec file in Mac OSX to get processed.
         */
        app.processEvents();
        
        /*
         * Now that events have processed, see if there was a request for
         * a data file to open.
         */
        const AString dataFileNameFromOS = GuiManager::get()->getNameOfDataFileToOpenAfterStartup();
        if ( ! dataFileNameFromOS.isEmpty()) {
            showSelectionSplashScreen = false;
            if (dataFileNameFromOS.endsWith(DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION))) {
                myState.specFileNameLoadWithDialog = dataFileNameFromOS;
            }
            else {
                myState.fileList.push_back(dataFileNameFromOS);
            }
        }
        
        /*
         * Show file selection splash screen if enabled via user's preferences
         */
        if (showSelectionSplashScreen) {
            /*
             * Show selection splash screen.
             * Need to process events since QApplication::exec() has not
             * been called.
             */
            SplashScreen splashScreen(NULL);
            app.processEvents();
            if (splashScreen.exec()) {
                const QString dataFileName = splashScreen.getSelectedDataFileName();
                if ( ! dataFileName.isEmpty()) {
                    myState.fileList.clear();
                    if (dataFileName.endsWith(DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION))) {
                        myState.specFileNameLoadWithDialog = dataFileName;
                    }
                    else {
                        myState.fileList.push_back(dataFileName);
                    }
                }
            }
        }
        
        /*
        * Create and display a main window.
        * If not done as pointer, the event object is listed as an
        * object that was not deleted by CaretObject::printListOfObjectsNotDeleted
        * since it does not go out of scope.
        */
        
        EventBrowserWindowNew newBrowserWindow(NULL, NULL);
        EventManager::get()->sendEvent(newBrowserWindow.getPointer());
        splashScreen.close();

        
        BrainBrowserWindow* myWindow = GuiManager::get()->getBrowserWindowByWindowIndex(0);
        if ( ! myWindow->hasValidOpenGL()) {
            app.processEvents();
            WuQMessageBox::errorOk(NULL,
                                   noOpenGLMessage);
            app.processEvents();
            
            return -1;
        }
        
        if ((myState.windowSizeXY[0] > 0) && (myState.windowSizeXY[1] > 0))
        {
            if (myState.windowPosXY[0] > 0 && myState.windowPosXY[1] > 0)
            {
                myWindow->setGeometry(myState.windowPosXY[0], myState.windowPosXY[1],
                                      myState.windowSizeXY[0], myState.windowSizeXY[1]);
            } else {
                myWindow->setFixedSize(myState.windowSizeXY[0],
                                       myState.windowSizeXY[1]);
            }
        } else {
            if (myState.windowPosXY[0] > 0 && myState.windowPosXY[1] > 0)
            {
                myState.windowSizeXY[0] = myWindow->width();
                myState.windowSizeXY[1] = myWindow->height();
                myWindow->setGeometry(myState.windowPosXY[0], myState.windowPosXY[1],
                                      myState.windowSizeXY[0], myState.windowSizeXY[1]);
            }
        }
        
        if (myState.graphicsSizeXY[0] > 0 && myState.graphicsSizeXY[1] > 0) {
            myWindow->setGraphicsWidgetFixedSize(myState.graphicsSizeXY[0], myState.graphicsSizeXY[1]);
        }
        
        if ( ! myState.specFileNameLoadAll.isEmpty()) {
            myWindow->loadFilesFromCommandLine({ myState.specFileNameLoadAll },
                                               BrainBrowserWindow::LOAD_SPEC_FILE_CONTENTS_VIA_COMMAND_LINE);
        }
        else if ( ! myState.specFileNameLoadWithDialog.isEmpty()) {
            myWindow->loadFilesFromCommandLine({ myState.specFileNameLoadWithDialog },
                                               BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG_VIA_COMMAND_LINE);
        }
        
        if (! myState.fileList.empty()) {
            myWindow->loadFilesFromCommandLine(myState.fileList,
                                               BrainBrowserWindow::LOAD_SPEC_FILE_WITH_DIALOG);
        }
        
        if ( ! myState.sceneFileName.isEmpty()) {
            myWindow->loadSceneFromCommandLine(myState.sceneFileName,
                                               myState.sceneNameOrNumber);
        }
        
#ifndef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        if (QGLPixelBuffer::hasOpenGLPbuffers()) {
            CaretLogConfig("OpenGL PBuffers are supported");
        }
        else {
            CaretLogConfig("OpenGL PBuffers are NOT supported");
        }
#endif
    
        /*
         * Log local (language, country)
         */
        QLocale sytemLocale = QLocale::system();
        CaretLogConfig("Local Language="
                       + QLocale::languageToString(sytemLocale.language())
                       + " Country="
                       + QLocale::countryToString(sytemLocale.country()));

        /*
         * Resolution of screens
         */
        AString screenSizeText = "Screen Sizes: ";
        QDesktopWidget* dw = QApplication::desktop();
        const int numScreens = dw->screenCount();
        for (int i = 0; i < numScreens; i++) {
            const QRect rect = dw->screenGeometry(i);
            const int x = rect.x();
            const int y = rect.y();
            const int w = rect.width();
            const int h = rect.height();
            screenSizeText.appendWithNewLine("Screen index="
                                             + AString::number(i)
                                             + ", x="
                                             + AString::number(x)
                                             + ", y="
                                             + AString::number(y)
                                             + ", w="
                                             + AString::number(w)
                                             + ", h="
                                             + AString::number(h));
        }
        screenSizeText.appendWithNewLine("Primary Screen="
                                         + AString::number(dw->primaryScreen()));
        if (dw->isVirtualDesktop()) {
            screenSizeText.appendWithNewLine("Virtual Desktop=YES");
        }
        else {
            screenSizeText.appendWithNewLine("Virtual Desktop=NO");
        }
        QWidget* screenWidget = dw->screen();
        QRect screenWidgetRect = screenWidget->geometry();
        screenSizeText.appendWithNewLine("Desktop: x="
                                         + AString::number(screenWidgetRect.x())
                                         + ", y="
                                         + AString::number(screenWidgetRect.y())
                                         + ", w="
                                         + AString::number(screenWidgetRect.width())
                                         + ", h="
                                         + AString::number(screenWidgetRect.height()));
        
        screenSizeText.appendWithNewLine("Logical DPI: x="
                                         + AString::number(dw->logicalDpiX())
                                         + ", y="
                                         + AString::number(dw->logicalDpiY()));
        
        screenSizeText.appendWithNewLine("Physical DPI: x="
                                         + AString::number(dw->physicalDpiX())
                                         + ", y="
                                         + AString::number(dw->physicalDpiY()));
        
        screenSizeText.appendWithNewLine("Width/height (mm): x="
                                         + AString::number(dw->widthMM())
                                         + ", y="
                                         + AString::number(dw->heightMM()));
        
        CaretLogConfig(screenSizeText);
        
        /*
        * Start the app which will launch the main window.
        */
        result = app.exec();
        
        /*
        * Hiding the window removes it from the event loop on Windows, which is necessary to
        * prevent paint events from causing assertion errors when the Window is destroyed
        * Although this is a Window's only bug, it's probably good practice to do on all platforms
        */
        //theMainWindow->hide();
        
        /*
        * Delete the GUI Manager.
        */
        GuiManager::deleteGuiManager();
        
        /*
         * Delete the command manager
         */
        CommandOperationManager::deleteCommandOperationManager();
        
        /*
        * Delete the session manager.
        */
        SessionManager::deleteSessionManager();
        
        CaretHttpManager::deleteHttpManager();
    }
    
    /*
     * See if any objects were not deleted.
     */
    CaretObject::printListOfObjectsNotDeleted(true);
    //FreeConsole();
    
    return result;
}

void printHelp(const AString& progName)
{
    cout
    << "Usage: " << progName << " [options] [files]" << endl
    << endl
    << "    [files], if present, can be a single spec file, or multiple data files" << endl
    << endl
    << "Options:" << endl
    << "    -help" << endl
    << "        display this usage text" << endl
    << endl
    << "    -graphics-size  <X Y>" << endl
    << "        Set the size of the graphics region." << endl
    << "        If this option is used you WILL NOT be able" << endl
    << "        to change the size of the graphic region. It" << endl
    << "        may be useful when image captures of a particular" << endl
    << "        size are desired." << endl
    << endl
    << "    -logging <level>" << endl
    << "       Set the logging level." << endl
    << "       Valid Levels are:" << endl;
    
    std::vector<LogLevelEnum::Enum> logLevels;
    LogLevelEnum::getAllEnums(logLevels);
    for (std::vector<LogLevelEnum::Enum>::iterator iter = logLevels.begin();
         iter != logLevels.end();
         iter++) {
        cout << "           " << qPrintable(LogLevelEnum::toName(*iter)) << endl;
    }
    
    cout
    << endl
    << "    -no-splash" << endl
    << "        disable all splash screens" << endl
    << endl
    << "    -scene-load <scene-file-name> <scene-name-or-number>" << endl
    << "        load the specified scene file and display the scene " << endl
    << "        in the file that matches by name or number.  Name" << endl
    << "        takes precedence over number.  The scene numbers " << endl
    << "        start at one." << endl
    << "        " << endl
    << endl
    << "    -style <style-name>" << endl
    << "        change the window style to the specified style" << endl
    << "        the following styles are valid on this system:" << endl;
    
    QStringList styleList = QStyleFactory::keys();
    QStringListIterator styleListIterator(styleList);
    while (styleListIterator.hasNext()) {
        cout << "           " << qPrintable(styleListIterator.next()) << endl;
    }
    
    cout
    << "        The selected style is listed on the About wb_view dialog" << endl
    << "        available from the File Menu (On Macs: wb_view Menu). " << endl
    << "        Press the \"More\" button to see the selected style." << endl
    << "        Other styles may be available on other systems." << endl
    << endl
    << "    -spec-load-all" << endl
    << "        load all files in the given spec file, don't show spec file dialog" << endl
    << endl
    << "    -window-size  <X Y>" << endl
    << "        Set the size of the browser window" << endl
    << endl
    << "    -window-pos  <X Y>" << endl
    << "        Set the position of the browser window" << endl
    << endl;
}

void parseCommandLine(const AString& progName, ProgramParameters* myParams, ProgramState& myState)
{
    bool hasFatalError = false;
    
    const AString moreThanOneSpecFileErrorMessage("More than one spec file is NOT allowed in options");
    
    try {
        while (myParams->hasNext())
        {
            AString thisParam = myParams->nextString("option");
            if (thisParam[0] == '-')
            {
                if (thisParam == "-style")
                {
                    myParams->nextString("style");//discard, QApplication handles this
                } else if (thisParam == "-help") {
                    printHelp(progName);
                    exit(0);
                } else if (thisParam == "-logging") {
                    if (myParams->hasNext()) {
                        const AString logLevelName = myParams->nextString("Logging Level").toUpper();
                        bool valid = false;
                        const LogLevelEnum::Enum level = LogLevelEnum::fromName(logLevelName, &valid);
                        if (valid)
                        {
                            /*
                             * Note settings logging level in preferences will also
                             * set logging level in the caret logger.
                             */
                            CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
                            prefs->setLoggingLevel(level);
                        }
                        else {
                            cerr << "Invalid logging level \""
                            << qPrintable(logLevelName)
                            << "\" for \"-logging\" option" << std::endl;
                            hasFatalError = true;
                        }
                    }
                } else if (thisParam == "-no-splash") {
                    myState.showSplash = false;
                } else if (thisParam == "-scene-load") {
                    if (myParams->hasNext()) {
                        myState.sceneFileName = myParams->nextString("Scene File Name");
                        if (myParams->hasNext()) {
                            myState.sceneNameOrNumber = myParams->nextString("Scene Name or Number");
                        }
                        else {
                            cerr << "Missing scene name/number for \"-scene\" option" << std::endl;
                            hasFatalError = true;
                        }
                    }
                    else {
                        cerr << "Missing scene file name for \"-scene\" option" << std::endl;
                        hasFatalError = true;
                    }
                } else if (thisParam == "-spec-load-all") {
                    if ( ! myState.specFileNameLoadAll.isEmpty()) {
                        cerr << qPrintable(moreThanOneSpecFileErrorMessage) << endl;
                    }
                    else if (myParams->hasNext()) {
                        myState.specFileNameLoadAll = myParams->nextString("Spec File Name");
                    }
                    else {
                        cerr << "Missing spec file name for \"-spec\" option" << endl;
                        hasFatalError = true;
                    }
                } else if (thisParam == "-graphics-size") {
                    if (myParams->hasNext()) {
                        myState.graphicsSizeXY[0] = myParams->nextInt("Graphics Size X");
                    }
                    else {
                        cerr << "Missing X & Y sizes for graphics" << endl;
                        hasFatalError = true;
                    }
                    if (myParams->hasNext()) {
                        myState.graphicsSizeXY[1] = myParams->nextInt("Graphics Size Y");
                    }
                    else {
                        cerr << "Missing Y sizes for graphics" << endl;
                        hasFatalError = true;
                    }
                } else if (thisParam == "-window-size") {
                    if (myParams->hasNext()) {
                        myState.windowSizeXY[0] = myParams->nextInt("Window Size X");
                    }
                    else {
                        cerr << "Missing X & Y sizes for window" << endl;
                        hasFatalError = true;
                    }
                    if (myParams->hasNext()) {
                        myState.windowSizeXY[1] = myParams->nextInt("Window Size Y");
                    }
                    else {
                        cerr << "Missing Y sizes for window" << endl;
                        hasFatalError = true;
                    }
                } else if (thisParam == "-window-pos") {
                    if (myParams->hasNext()) {
                        myState.windowPosXY[0] = myParams->nextInt("Window Position X");
                    }
                    else {
                        cerr << "Missing X & Y position for window" << endl;
                        hasFatalError = true;
                    }
                    if (myParams->hasNext()) {
                        myState.windowPosXY[1] = myParams->nextInt("Window Position Y");
                    }
                    else {
                        cerr << "Missing Y position for window" << endl;
                        hasFatalError = true;
                    }
                } else if (thisParam.startsWith("-psn")) {
                    /*
                     * 21 April 2014 (Did not have this problem before this date)
                     *
                     * IGNORE this parameter.  For some reason, when a Mac
                     * version is started from Finder, a "-psn" parameter
                     * is being added to the parameters.  If this parameter
                     * is not ignored, Workbench starts, the icon bounces 
                     * a few times, and then Workbench quits (due to
                     * "unrecognized option", below), and the user is 
                     * not given any error message.
                     *
                     * http://stackoverflow.com/questions/10242115/os-x-strange-psn-command-line-parameter-when-launched-from-finder
                     * http://trac.wxwidgets.org/ticket/15432
                     */
                } else {
                    cerr << "unrecognized option \"" << thisParam << "\"" << endl;
                    printHelp(progName);
                    hasFatalError = true;
                }
            } else {
                if (thisParam.endsWith(DataFileTypeEnum::toFileExtension(DataFileTypeEnum::SPECIFICATION))) {
                    if ( ! myState.specFileNameLoadWithDialog.isEmpty()) {
                        cerr << qPrintable(moreThanOneSpecFileErrorMessage) << endl;
                        hasFatalError = true;
                    }
                    else {
                        myState.specFileNameLoadWithDialog = thisParam;
                    }
                }
                else {
                    myState.fileList.push_back(thisParam);
                }
            }
        }
    }
    catch (const ProgramParametersException& e) {
        cerr << e.whatString() << std::endl;
        hasFatalError = true;
    }
    
    if ( ( ! myState.specFileNameLoadWithDialog.isEmpty())
        && ( ! myState.specFileNameLoadAll.isEmpty())) {
        cerr << qPrintable(moreThanOneSpecFileErrorMessage) << endl;
        hasFatalError = true;
    }
    
    if (hasFatalError) {
        exit(-1);
    }
    
    /*
     * If any files are listed, disable splash screen
     */
    if ( ! myState.fileList.empty()) {
        myState.showSplash = false;
    }
    if ( ! myState.sceneFileName.isEmpty()) {
        myState.showSplash = false;
    }
    if ( ! myState.specFileNameLoadWithDialog.isEmpty()) {
        myState.showSplash = false;
    }
    if ( ! myState.specFileNameLoadAll.isEmpty()) {
        myState.showSplash = false;
    }
}

ProgramState::ProgramState()
{
    sceneFileName = "";
    sceneNameOrNumber = "";
    windowSizeXY[0] = -1;
    windowSizeXY[1] = -1;
    windowPosXY[0] = -1;
    windowPosXY[1] = -1;
    graphicsSizeXY[0] = -1;
    graphicsSizeXY[1] = -1;
    showSplash = true;
}
