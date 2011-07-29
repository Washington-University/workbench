

#include <QApplication>
#include <QGLFormat>

#include "Brain.h"
#include "GuiGlobals.h"
#include "WindowMain.h"

using namespace caret;

int 
main(int argc, char* argv[])
{
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
     * Clean up any globally allocated objects.
     */
    GuiGlobals::deleteAllAtProgramExit();
    
    /*
     * See if any objects were not deleted.
     */
    CaretObject::printListOfObjectsNotDeleted(true);
    
    return result;
}
