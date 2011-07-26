

#include <QApplication>
#include <QGLFormat>

#include "Brain.h"
#include "GuiGlobals.h"
#include "WindowMain.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace caret;

int 
main(int argc, char* argv[])
{
    
    QApplication app(argc, argv);
    
    /*
     * Make sure OpenGL is available.
     */
    if (!QGLFormat::hasOpenGL()) { 
        qWarning( "This system has no OpenGL support. Exiting." );
        return -1;
    }
    
    Brain* brain = GuiGlobals::getBrain();
    //set environment variable CARET7_DATA to the data path i.e. CARET7_DATA=/home/jschindl/hg/caret7_test_files
  
//    std::string filePath = "/Users/john/caret_data/caret7_test_files";
//    char* dataDir = std::getenv("CARET7_DATA");
//    if (dataDir != NULL) {
//       filePath = dataDir;
//    }
//    brain->readSurfaceFile(filePath+"/gifti/Human.PALS_B12.LEFT_AVG_B1-12.FIDUCIAL_FLIRT.clean.73730.surf.gii");
    
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
    
    int result = app.exec();
    return result;
}
