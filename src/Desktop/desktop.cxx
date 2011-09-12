

#include <QApplication>
#include <QGLFormat>

#include "GuiGlobals.h"
#include "SessionManager.h"
#include "SystemUtilities.h"
#include "WindowMain.h"


using namespace caret;

int 
main(int argc, char* argv[])
{
    /*
     * Handle uncaught exceptions
     */
    SystemUtilities::setHandlersForUnexpected();
    
    QApplication app(argc, argv);
    QApplication::setApplicationName("Caret 7");
    QApplication::setApplicationVersion("7");
    QApplication::setOrganizationDomain("brainvis.wustl.edu");
    QApplication::setOrganizationName("Van Essen Lab");
    
    
    /*
     * Make sure OpenGL is available.
     */
    if (!QGLFormat::hasOpenGL()) { 
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }
        
    /*
     * Setup OpenGL
     */
    QGLFormat glfmt;
    glfmt.setDoubleBuffer(true);
    glfmt.setDirectRendering(true);
    QGLFormat::setDefaultFormat(glfmt);
    
    /*
     * Create the session manager.
     */
    SessionManager::createSessionManager();
    
    /*
     * Create and display the main window.
     */
    WindowMain* theMainWindow = new WindowMain(500, 500);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    theMainWindow->show();
    
    /*
     * Start the app which will launch the main window.
     */
    int result = app.exec();
    
    /*
     * Hiding the window removes it from the event loop on Windows, which is necessary to
     * prevent paint events from causing assertion errors when the Window is destroyed
     * Although this is a Window's only bug, it's probably good practice to do on all platforms
     */
    theMainWindow->hide();
    
    /*
     * Clean up any globally allocated objects.
     */
    GuiGlobals::deleteAllAtProgramExit();
    
    /*
     * Delete the session manager.
     */
    SessionManager::deleteSessionManager();
    
    /*
     * See if any objects were not deleted.
     */
    CaretObject::printListOfObjectsNotDeleted(true);
    
    return result;
}
