
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

#include <cstdio>
#include <fstream>

#ifdef HAVE_OSMESA
#include <GL/osmesa.h>
#endif // HAVE_OSMESA

#include <QImage>
#include <QColor>


#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLViewportContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "DummyFontTextRenderer.h"
#include "FtglFontTextRenderer.h"
#include "ImageFile.h"
#include "OperationShowScene.h"
#include "OperationException.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneFile.h"
#include "ScenePrimitiveArray.h"
#include "SessionManager.h"
#include "TileTabsConfiguration.h"
#include "VolumeFile.h"

//#include "workbench_png.h"

using namespace caret;

/**
 * \class caret::OperationShowScene 
 * \brief Offscreen rendering of scene to an image file
 *
 * Render a scene into an image file using the Offscreen Mesa Library
 */

/**
 * @return Command line switch
 */
AString
OperationShowScene::getCommandSwitch()
{
    return "-show-scene";
}

/**
 * @return Short description of operation
 */
AString
OperationShowScene::getShortDescription()
{
    return ("OFFSCREEN RENDERING OF SCENE TO AN IMAGE FILE");
}

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationShowScene::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "scene-file", "scene file");
    
    ret->addStringParameter(2, "scene-name-or-number", "name or number (starting at one) of the scene in the scene file");
    
    ret->addStringParameter(3, "image-file-name", "output image file name");
    
    ret->addIntegerParameter(4, "image-width", "width of output image(s)");

    ret->addIntegerParameter(5, "image-height", "height of output image(s)");
    
    const QString windowSizeSwitch("-use-window-size");
    ret->createOptionalParameter(6, windowSizeSwitch, "Override image size with window size");
    
    AString helpText("Render content of browser windows displayed in a scene "
                     "into image file(s).  The image file name should be "
                     "similar to \"capture.png\".  If there is only one image "
                     "to render, the image name will not change.  If there is "
                     "more than one image to render, an index will be inserted "
                     "into the image name: \"capture_01.png\", \"capture_02.png\" "
                     "etc.\n"
                     "\n"
                     "The image format is determined by the image file extension.\n"
                     "The available image formats may vary by operating system.\n"
                     "Image formats available on this system are:\n");
    std::vector<AString> imageFileExtensions;
    AString defaultExtension;
    ImageFile::getImageFileExtensions(imageFileExtensions,
                                      defaultExtension);
    
    for (std::vector<AString>::iterator iter = imageFileExtensions.begin();
         iter != imageFileExtensions.end();
         iter++) {
        const AString ext = *iter;
        helpText += ("    "
                     + ext
                     + "\n");
    }
    
    helpText += ("\n"
                 "The result of using the \"" + windowSizeSwitch + "\" option\n"
                 "is dependent upon the version used to create the scene.\n"
                 "    * Versions newer than 1.2.0-pre1 contain the width and \n"
                 "      height of the graphics region.  The output image  \n"
                 "      will be the width and height from the scene and\n"
                 "      the image width and height specified on the command\n"
                 "      line is ignored.\n"
                 "    * If the scene does not contain the width and height\n"
                 "      of the graphics region, the width and height specified\n"
                 "      on the command line is used for the size of the \n"
                 "      output image.\n"
                 );
    
    
    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
#ifndef HAVE_OSMESA
void
OperationShowScene::useParameters(OperationParameters* /*myParams*/,
                                          ProgressObject* /*myProgObj*/)
{
    throw OperationException("Show scene command not available due to this software version "
                             "not being built with the Mesa OffScreen Library");
}
#else // HAVE_OSMESA
void
OperationShowScene::useParameters(OperationParameters* myParams,
                                  ProgressObject* myProgObj)
{
    const bool optionalImageSizeFlag = true;
    
    if (optionalImageSizeFlag) {
        useParametersOptionalImageSize(myParams, myProgObj);
    }
    else {
        useParametersOld(myParams, myProgObj);
    }
}

void
OperationShowScene::useParametersOptionalImageSize(OperationParameters* myParams,
                                            ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString sceneFileName = FileInformation(myParams->getString(1)).getAbsoluteFilePath();
    AString sceneNameOrNumber = myParams->getString(2);
    AString imageFileName = FileInformation(myParams->getString(3)).getAbsoluteFilePath();
    const int32_t userImageWidth  = myParams->getInteger(4);
    const int32_t userImageHeight = myParams->getInteger(5);
    
    OptionalParameter* useWindowSizeParam = myParams->getOptionalParameter(6);
    const bool useWindowSizeForImageSizeFlag = useWindowSizeParam->m_present;
    
    if ( ! useWindowSizeForImageSizeFlag) {
        if ((userImageWidth <= 0)
            || (userImageHeight <= 0)) {
            throw OperationException("Invalid image size width="
                                     + QString::number(userImageWidth)
                                     + " height="
                                     + QString::number(userImageHeight));
        }
    }
    
    /*
     * Read the scene file and load the scene
     */
    SceneFile sceneFile;
    sceneFile.readFile(sceneFileName);
    Scene* scene = sceneFile.getSceneWithName(sceneNameOrNumber);
    if (scene == NULL) {
        bool valid = false;
        const int32_t sceneIndexStartAtOne = sceneNameOrNumber.toInt(&valid);
        if (valid) {
            const int32_t sceneIndex = sceneIndexStartAtOne - 1;
            if ((sceneIndex >= 0)
                && (sceneIndex < sceneFile.getNumberOfScenes())) {
                scene = sceneFile.getSceneAtIndex(sceneIndex);
            }
            else {
                throw OperationException("Scene index is invalid");
            }
        }
        else {
            throw OperationException("Scene name is invalid");
        }
    }

    /*
     * Enable voxel coloring since it is defaulted off for commands
     */
    VolumeFile::setVoxelColoringEnabled(true);
    
    SceneAttributes sceneAttributes(SceneTypeEnum::SCENE_TYPE_FULL);
    
    /*
     * Restore the scene
     */
    const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
    if (guiManagerClass->getName() != "guiManager") {
        throw OperationException("Top level scene class should be guiManager but it is: "
                                 + guiManagerClass->getName());
    }
    
    SessionManager* sessionManager = SessionManager::get();
    sessionManager->restoreFromScene(&sceneAttributes,
                                     guiManagerClass->getClass("m_sessionManager"));
    
    
    if (sessionManager->getNumberOfBrains() <= 0) {
        throw OperationException("Scene loading failure, SessionManager contains no Brains");
    }
    Brain* brain = SessionManager::get()->getBrain(0);
    
    const GapsAndMargins* gapsAndMargins = brain->getGapsAndMargins();
    
    bool missingWindowMessageHasBeenDisplayed = false;
    
    /*
     * Restore windows
     */
    const SceneClassArray* browserWindowArray = guiManagerClass->getClassArray("m_brainBrowserWindows");
    if (browserWindowArray != NULL) {
        const int32_t numBrowserClasses = browserWindowArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numBrowserClasses; i++) {
            const SceneClass* browserClass = browserWindowArray->getClassAtIndex(i);
            
            const bool restoreToTabTiles = browserClass->getBooleanValue("m_viewTileTabsAction",
                                                                         false);
            const int32_t windowIndex = browserClass->getIntegerValue("m_browserWindowIndex", 0);
            
            int32_t imageWidth  = userImageWidth;
            int32_t imageHeight = userImageHeight;
            
            if (useWindowSizeForImageSizeFlag) {
                /*
                 * Requires version AFTER 1.2.0-pre1
                 */
                const SceneClass* graphicsGeometry = browserClass->getClass("openGLWidgetGeometry");
                if (graphicsGeometry != NULL) {
                    const int32_t windowGeometryWidth  = graphicsGeometry->getIntegerValue("geometryWidth", -1);
                    const int32_t windowGeometryHeight = graphicsGeometry->getIntegerValue("geometryHeight", -1);
                    
                    if ((windowGeometryWidth > 0)
                        && (windowGeometryHeight > 0)) {
                        imageWidth  = windowGeometryWidth;
                        imageHeight = windowGeometryHeight;
                    }
                }
                else {
                    if ((imageWidth <= 0)
                        || (imageHeight <= 0)) {
                        const QString msg("Option "
                                          + useWindowSizeParam->m_optionSwitch
                                          + " is used but window size not found in scene and width="
                                          + QString::number(imageWidth)
                                          + " height="
                                          + QString::number(imageWidth)
                                          + " on command line is invalid.");
                        
                        throw OperationException(msg);
                    }
                    
                    if ( ! missingWindowMessageHasBeenDisplayed) {
                        const QString msg("Option \""
                                          + useWindowSizeParam->m_optionSwitch
                                          + "\" is used but window size not found in scene.\n"
                                          "   Scene was created prior to implementation of this option.\n"
                                          "   Image size will be width="
                                          + QString::number(imageWidth)
                                          + " and height="
                                          + QString::number(imageHeight)
                                          + " as specified on command line.\n"
                                          "   Recreating the scene will allow use of the option.\n");
                        CaretLogWarning(msg);
                        
                        /*
                         * Avoid message being displayed more than once when
                         * there are more than one windows.
                         */
                        missingWindowMessageHasBeenDisplayed = true;
                    }
                }
            }
            
            if ((imageWidth <= 0)
                || (imageHeight <= 0)) {
                throw OperationException("Invalid image size width="
                                         + QString::number(imageWidth)
                                         + " height="
                                         + QString::number(imageHeight));
            }
            
            int windowViewport[4] = { 0.0, 0.0, imageWidth, imageHeight };
            
            float aspectRatio = -1.0;
            const bool windowAspectRatioLocked = browserClass->getBooleanValue("m_aspectRatioLockedStatus");
            if (windowAspectRatioLocked) {
                aspectRatio = browserClass->getFloatValue("m_aspectRatio", -1.0);
            }
            
            const int windowWidth  = windowViewport[2];
            const int windowHeight = windowViewport[3];
            
            //
            // Create the Mesa Context
            //
            const int depthBits = 16;
            const int stencilBits = 0;
            const int accumBits = 0;
            OSMesaContext mesaContext = OSMesaCreateContextExt(OSMESA_RGBA,
                                                               depthBits,
                                                               stencilBits,
                                                               accumBits,
                                                               NULL);
            if (mesaContext == 0) {
                throw ("Creating Mesa Context failed.");
            }
            
            //
            // Allocate image buffer
            //
            const int32_t imageBufferSize =imageWidth * imageHeight * 4 * sizeof(unsigned char);
            unsigned char* imageBuffer = new unsigned char[imageBufferSize];
            if (imageBuffer == 0) {
                throw OperationException("Allocating image buffer size="
                                         + QString::number(imageBufferSize)
                                         + " failed.");
            }
            
            //
            // Assign buffer to Mesa Context and make current
            //
            if (OSMesaMakeCurrent(mesaContext,
                                  imageBuffer,
                                  GL_UNSIGNED_BYTE,
                                  imageWidth,
                                  imageHeight) == 0) {
                throw OperationException("Assigning buffer to context and make current failed.");
            }
            CaretPointer<BrainOpenGL> brainOpenGL(createBrainOpenGL(windowIndex));
            
            /*
             * If tile tabs was saved to the scene, restore it as the scenes tile tabs configuration
             */
            if (restoreToTabTiles) {
                const AString tileTabsConfigString = browserClass->getStringValue("m_sceneTileTabsConfiguration");
                if ( ! tileTabsConfigString.isEmpty()) {
                    TileTabsConfiguration tileTabsConfiguration;
                    tileTabsConfiguration.decodeFromXML(tileTabsConfigString);
                    
                    /*
                     * Restore toolbar
                     */
                    const SceneClass* toolbarClass = browserClass->getClass("m_toolbar");
                    if (toolbarClass != NULL) {
                        /*
                         * Index of selected browser tab (NOT the tabBar)
                         */
                        std::vector<BrowserTabContent*> allTabContent;
                        const ScenePrimitiveArray* tabIndexArray = toolbarClass->getPrimitiveArray("tabIndices");
                        if (tabIndexArray != NULL) {
                            const int32_t numTabs = tabIndexArray->getNumberOfArrayElements();
                            for (int32_t iTab = 0; iTab < numTabs; iTab++) {
                                const int32_t tabIndex = tabIndexArray->integerValue(iTab);
                                
                                EventBrowserTabGet getTabContent(tabIndex);
                                EventManager::get()->sendEvent(getTabContent.getPointer());
                                BrowserTabContent* tabContent = getTabContent.getBrowserTab();
                                if (tabContent == NULL) {
                                    throw OperationException("Failed to obtain tab number "
                                                             + AString::number(tabIndex + 1)
                                                             + " for window "
                                                             + AString::number(windowIndex + 1));
                                }
                                allTabContent.push_back(tabContent);
                            }
                        }
                        
                        const int32_t numTabContent = static_cast<int32_t>(allTabContent.size());
                        if (numTabContent <= 0) {
                            throw OperationException("Failed to find any tab content");
                        }
                        std::vector<int32_t> rowHeights;
                        std::vector<int32_t> columnWidths;
                        if ( ! tileTabsConfiguration.getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                                               windowHeight,
                                                                                               numTabContent,
                                                                                               rowHeights,
                                                                                               columnWidths)) {
                            throw OperationException("Tile Tabs Row/Column sizing failed !!!");
                        }
                        
                        const int32_t tabIndexToHighlight = -1;
                        std::vector<BrainOpenGLViewportContent*> viewports = BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabContent,
                                                                                                                                          windowIndex,
                                                                                                                                          //brain,
                                                                                                                                          windowViewport,
                                                                                                                                          rowHeights,
                                                                                                                                          columnWidths,
                                                                                                                                          tabIndexToHighlight,
                                                                                                                                          gapsAndMargins);
                        
                        brainOpenGL->drawModels(brain,
                                                viewports);
                        
                        const int32_t outputImageIndex = ((numBrowserClasses > 1)
                                                          ? i
                                                          : -1);
                        
                        writeImage(imageFileName,
                                   outputImageIndex,
                                   imageBuffer,
                                   imageWidth,
                                   imageHeight);
                        
                        for (std::vector<BrainOpenGLViewportContent*>::iterator vpIter = viewports.begin();
                             vpIter != viewports.end();
                             vpIter++) {
                            delete *vpIter;
                        }
                        viewports.clear();
                    }
                }
                else {
                    throw OperationException("Tile tabs configuration is corrupted.");
                }
            }
            else {
                /*
                 * Restore toolbar
                 */
                const SceneClass* toolbarClass = browserClass->getClass("m_toolbar");
                if (toolbarClass != NULL) {
                    /*
                     * Index of selected browser tab (NOT the tabBar)
                     */
                    const int32_t selectedTabIndex = toolbarClass->getIntegerValue("selectedTabIndex", -1);
                    
                    EventBrowserTabGet getTabContent(selectedTabIndex);
                    EventManager::get()->sendEvent(getTabContent.getPointer());
                    BrowserTabContent* tabContent = getTabContent.getBrowserTab();
                    if (tabContent == NULL) {
                        throw OperationException("Failed to obtain tab number "
                                                 + AString::number(selectedTabIndex + 1)
                                                 + " for window "
                                                 + AString::number(i + 1));
                    }
                    
                    CaretPointer<BrainOpenGLViewportContent> content(BrainOpenGLViewportContent::createViewportForSingleTab(windowViewport,
                                                                                                                            windowViewport,
                                                                                                                            windowIndex,
                                                                                                                            false, // highlight the tab
                                                                                                                            gapsAndMargins,
                                                                                                                            tabContent));
                    std::vector<BrainOpenGLViewportContent*> viewportContents;
                    viewportContents.push_back(content);
                    
                    brainOpenGL->drawModels(brain,
                                            viewportContents);
                    
                    const int32_t outputImageIndex = ((numBrowserClasses > 1)
                                                      ? i
                                                      : -1);
                    
                    writeImage(imageFileName,
                               outputImageIndex,
                               imageBuffer,
                               imageWidth,
                               imageHeight);
                    
                }
            }
            
            /*
             * Free image memory and Mesa context
             */
            delete[] imageBuffer;
            OSMesaDestroyContext(mesaContext);
        }
    }
}

/**
 * Estimate the size of the graphics region from scenes that lack
 * an explicit entry for the graphics region size.  Scenes in version
 * 1.2.0-pre1 did not contain graphics window size.
 *
 * @param windowSceneClass
 *     Scene class for the window.
 * @param estimatedWidthOut
 *     Estimated width of graphics region (greater than zero if valid).
 * @param estimatedHeightOut
 *     Estimated height of graphics region (greater than zero if valid).
 */
void
OperationShowScene::estimateGraphicsSize(const SceneClass* windowSceneClass,
                                         float& estimatedWidthOut,
                                         float& estimatedHeightOut)
{
    estimatedWidthOut  = 0;
    estimatedHeightOut = 0;
    
    float winWidth  = -1.0;
    float winHeight = -1.0;
    const SceneClass* winGeometry = windowSceneClass->getClass("geometry");
    if (winGeometry != NULL) {
        winWidth  = winGeometry->getIntegerValue("geometryWidth", -1);
        winHeight = winGeometry->getIntegerValue("geometryHeight", -1);
    }
    else {
        return;
    }
    
    float tbHeight = -1.0;
    bool tbHeightValid = false;
    const SceneClass* tb = windowSceneClass->getClass("m_toolbar");
    if (tb != NULL) {
        if (tb->getBooleanValue("toolBarVisible")) {
            tbHeight = 165.0;
            tbHeightValid = true;
        }
    }
    if ( ! tbHeightValid) {
        return;
    }
    
    float overlayToolBoxWidth  = 0;
    float overlayToolBoxHeight = 0;
    QString overlayToolBoxOrientation;
    bool overlayToolBoxValid = getToolBoxSize(windowSceneClass->getClass("overlayToolBox"),
                                              windowSceneClass->getClass("m_overlayActiveToolBox"),
                                              overlayToolBoxWidth,
                                              overlayToolBoxHeight,
                                              overlayToolBoxOrientation);
    
    
    float featureToolBoxWidth  = 0;
    float featureToolBoxHeight = 0;
    QString featureToolBoxOrientation;
    bool featureToolBoxValid = getToolBoxSize(windowSceneClass->getClass("featureToolBox"),
                                              windowSceneClass->getClass("m_featuresToolBox"),
                                              featureToolBoxWidth,
                                              featureToolBoxHeight,
                                              featureToolBoxOrientation);
    
    if (overlayToolBoxValid
        && featureToolBoxValid) {
        
        estimatedWidthOut = winWidth - overlayToolBoxWidth - featureToolBoxWidth;
        estimatedHeightOut = winHeight - tbHeight - overlayToolBoxHeight - featureToolBoxHeight;
    }
}

/**
 * Get the size of a toolbox.
 * 
 * @param toolBoxClass
 *    The toolbox scene class.
 * @param overlayToolBoxWidthOut
 *    Output with width of toolbox.
 * @param overlayToolBoxHeightOut
 *    Output with height of toolbox.
 * @param overlayToolBoxOrientationOut
 *    Output with orientation of toolbox.
 * @return
 *    True if the toolbox outputs are valid, else false.
 */
bool
OperationShowScene::getToolBoxSize(const SceneClass* toolBoxClass,
                                   const SceneClass* activeToolBoxClass,
               float& overlayToolBoxWidthOut,
               float& overlayToolBoxHeightOut,
               QString& overlayToolBoxOrientationOut)
{
    if (toolBoxClass == NULL) {
        return false;
    }
    if (activeToolBoxClass == NULL) {
        return false;
    }
    
    overlayToolBoxWidthOut  = 0;
    overlayToolBoxHeightOut = 0;
    
    bool overlayToolBoxValid = false;
    if (toolBoxClass != NULL) {
        overlayToolBoxValid = true;
        if (toolBoxClass->getBooleanValue("visible")) {
            overlayToolBoxValid = false;
            if ( ! toolBoxClass->getBooleanValue("floating")) {
                overlayToolBoxOrientationOut = toolBoxClass->getStringValue("orientation");
                overlayToolBoxWidthOut  = activeToolBoxClass->getIntegerValue("toolboxWidth");
                overlayToolBoxHeightOut = activeToolBoxClass->getIntegerValue("toolboxHeight");
                if ((overlayToolBoxWidthOut > 0)
                    && (overlayToolBoxHeightOut > 0)) {
                    
                    if (overlayToolBoxOrientationOut == "horizontal") {
                        /*
                         * Toolbar is on bottom so only need height
                         */
                        overlayToolBoxWidthOut = 0;
                        overlayToolBoxValid = true;
                    }
                    else if (overlayToolBoxOrientationOut == "vertical") {
                        /*
                         * Toolbar is on left side so only need width
                         */
                        overlayToolBoxHeightOut = 0;
                        overlayToolBoxValid = true;
                    }
                }
            }
        }
    }
    
    return overlayToolBoxValid;
}

void
OperationShowScene::useParametersOld(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString sceneFileName = FileInformation(myParams->getString(1)).getAbsoluteFilePath();
    AString sceneNameOrNumber = myParams->getString(2);
    AString imageFileName = FileInformation(myParams->getString(3)).getAbsoluteFilePath();
    const int32_t imageWidth  = myParams->getInteger(4);
    if (imageWidth < 0) {
        throw OperationException("image width is invalid");
    }
    const int32_t imageHeight = myParams->getInteger(5);
    if (imageHeight < 0) {
        throw OperationException("image height is invalid");
    }
    
    SceneFile sceneFile;
    sceneFile.readFile(sceneFileName);
    
    Scene* scene = sceneFile.getSceneWithName(sceneNameOrNumber);
    if (scene == NULL) {
        bool valid = false;
        const int32_t sceneIndexStartAtOne = sceneNameOrNumber.toInt(&valid);
        if (valid) {
            const int32_t sceneIndex = sceneIndexStartAtOne - 1;
            if ((sceneIndex >= 0)
                && (sceneIndex < sceneFile.getNumberOfScenes())) {
                scene = sceneFile.getSceneAtIndex(sceneIndex);
            }
            else {
                throw OperationException("Scene index is invalid");
            }
        }
        else {
            throw OperationException("Scene name is invalid");
        }
    }
    
    //
    // Create the Mesa Context
    //
    const int depthBits = 16;
    const int stencilBits = 0;
    const int accumBits = 0;
    OSMesaContext mesaContext = OSMesaCreateContextExt(OSMESA_RGBA,
                                                       depthBits,
                                                       stencilBits,
                                                       accumBits,
                                                       NULL);
    if (mesaContext == 0) {
        std::cout << "Creating Mesa Context failed." << std::endl;
        exit(-1);
    }
    
    //
    // Allocate image buffer
    //
    unsigned char* imageBuffer = new unsigned char[imageWidth * imageHeight * 4 *
                                                   sizeof(unsigned char)];
    if (imageBuffer == 0) {
        std::cout << "Allocating image buffer failed." << std::endl;
        exit(-1);
    }
    
    //
    // Assign buffer to Mesa Context and make current
    //
    if (OSMesaMakeCurrent(mesaContext,
                          imageBuffer,
                          GL_UNSIGNED_BYTE,
                          imageWidth,
                          imageHeight) == 0) {
        std::cout << "Assigning buffer to context and make current failed." << std::endl;
        exit(-1);
    }
    
    /*
     * Set the viewport
     */
    const int imageViewport[4] = { 0, 0, imageWidth, imageHeight };

    /**
     * Enable voxel coloring since it is defaulted off for commands
     */
    VolumeFile::setVoxelColoringEnabled(true);    
    
    SceneAttributes sceneAttributes(SceneTypeEnum::SCENE_TYPE_FULL);
    
    const SceneClass* guiManagerClass = scene->getClassWithName("guiManager");
    if (guiManagerClass->getName() != "guiManager") {
        throw OperationException("Top level scene class should be guiManager but it is: "
                               + guiManagerClass->getName());
    }
    
    SessionManager* sessionManager = SessionManager::get();
    sessionManager->restoreFromScene(&sceneAttributes,
                                            guiManagerClass->getClass("m_sessionManager"));

    
    if (sessionManager->getNumberOfBrains() <= 0) {
        throw OperationException("Scene loading failure, SessionManager contains no Brains");
    }
    Brain* brain = SessionManager::get()->getBrain(0);
    
    const GapsAndMargins* gapsAndMargins = brain->getGapsAndMargins();
    
//    /*
//     * The OpenGL rendering takes ownership of the text renderer
//     * and will delete the text renderer when OpenGL itself
//     * is deleted.
//     */
//    BrainOpenGLTextRenderInterface* textRenderer = NULL;
//    if (textRenderer == NULL) {
//        textRenderer = new FtglFontTextRenderer();
//        if (! textRenderer->isValid()) {
//            delete textRenderer;
//            textRenderer = NULL;
//            CaretLogWarning("Unable to create FTGL Font Renderer.\n"
//                            "No text will be available in graphics window.");
//        }
//    }
//    if (textRenderer == NULL) {
//        textRenderer = new DummyFontTextRenderer();
//    }
//    
//    /*
//     * Performs OpenGL Rendering
//     * Allocated dynamically so that it can be destroyed prior to OSMesa being
//     * destroyed.  Otherwise, if OpenGL is destroyed after OSMesa, errors
//     * will occur as the OpenGL context is invalid when things such as
//     * display lists or buffers are deleted.
//     */
//    BrainOpenGLFixedPipeline* brainOpenGL = new BrainOpenGLFixedPipeline(textRenderer);
//    brainOpenGL->initializeOpenGL();
    
    /*
     * Restore windows
     */
    const SceneClassArray* browserWindowArray = guiManagerClass->getClassArray("m_brainBrowserWindows");
    if (browserWindowArray != NULL) {
        const int32_t numBrowserClasses = browserWindowArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numBrowserClasses; i++) {
            const SceneClass* browserClass = browserWindowArray->getClassAtIndex(i);
            
            const bool restoreToTabTiles = browserClass->getBooleanValue("m_viewTileTabsAction",
                                                                         false);
            const int32_t windowIndex = browserClass->getIntegerValue("m_browserWindowIndex", 0);
            
            int windowViewport[4] = {
                imageViewport[0], imageViewport[1], imageViewport[2], imageViewport[3]
            };
            
            float aspectRatio = -1.0;
            const bool windowAspectRatioLocked = browserClass->getBooleanValue("m_aspectRatioLockedStatus");
            if (windowAspectRatioLocked) {
                aspectRatio = browserClass->getFloatValue("m_aspectRatio", -1.0);
            }
            
            const int windowWidth  = windowViewport[2];
            const int windowHeight = windowViewport[3];
            
            CaretPointer<BrainOpenGL> brainOpenGL(createBrainOpenGL(windowIndex));
            
            /*
             * If tile tabs was saved to the scene, restore it as the scenes tile tabs configuration
             */
            if (restoreToTabTiles) {
                const AString tileTabsConfigString = browserClass->getStringValue("m_sceneTileTabsConfiguration");
                if ( ! tileTabsConfigString.isEmpty()) {
                    TileTabsConfiguration tileTabsConfiguration;
                    tileTabsConfiguration.decodeFromXML(tileTabsConfigString);
                    
                    /*
                     * Restore toolbar
                     */
                    const SceneClass* toolbarClass = browserClass->getClass("m_toolbar");
                    if (toolbarClass != NULL) {
                        /*
                         * Index of selected browser tab (NOT the tabBar)
                         */
                        std::vector<BrowserTabContent*> allTabContent;
                        const ScenePrimitiveArray* tabIndexArray = toolbarClass->getPrimitiveArray("tabIndices");
                        if (tabIndexArray != NULL) {
                            const int32_t numTabs = tabIndexArray->getNumberOfArrayElements();
                            for (int32_t iTab = 0; iTab < numTabs; iTab++) {
                                const int32_t tabIndex = tabIndexArray->integerValue(iTab);
                                
                                EventBrowserTabGet getTabContent(tabIndex);
                                EventManager::get()->sendEvent(getTabContent.getPointer());
                                BrowserTabContent* tabContent = getTabContent.getBrowserTab();
                                if (tabContent == NULL) {
                                    throw OperationException("Failed to obtain tab number "
                                                             + AString::number(tabIndex + 1)
                                                             + " for window "
                                                             + AString::number(windowIndex + 1));
                                }
                                allTabContent.push_back(tabContent);
                            }
                        }
                        
                        const int32_t numTabContent = static_cast<int32_t>(allTabContent.size());
                        if (numTabContent <= 0) {
                            throw OperationException("Failed to find any tab content");
                        }
                        std::vector<int32_t> rowHeights;
                        std::vector<int32_t> columnWidths;
                        if ( ! tileTabsConfiguration.getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                                               windowHeight,
                                                                                               numTabContent,
                                                                                               rowHeights,
                                                                                               columnWidths)) {
                            throw OperationException("Tile Tabs Row/Column sizing failed !!!");
                        }
                        
                        const int32_t tabIndexToHighlight = -1;
                        std::vector<BrainOpenGLViewportContent*> viewports = BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabContent,
                                                                                                                                          windowIndex,
                                                                                                                                          //brain,
                                                                                                                                          windowViewport,
                                                                                                                                          rowHeights,
                                                                                                                                          columnWidths,
                                                                                                                                          tabIndexToHighlight,
                                                                                                                                          gapsAndMargins);
                        
                        brainOpenGL->drawModels(brain,
                                                viewports);

                        const int32_t outputImageIndex = ((numBrowserClasses > 1)
                                                          ? i
                                                          : -1);
                        
                        writeImage(imageFileName,
                                   outputImageIndex,
                                   imageBuffer,
                                   imageWidth,
                                   imageHeight);
                        
                        for (std::vector<BrainOpenGLViewportContent*>::iterator vpIter = viewports.begin();
                             vpIter != viewports.end();
                             vpIter++) {
                            delete *vpIter;
                        }
                        viewports.clear();
                    }
                }
                else {
                    throw OperationException("Tile tabs configuration is corrupted.");
                }
            }
            else {
                /*
                 * Restore toolbar
                 */
                const SceneClass* toolbarClass = browserClass->getClass("m_toolbar");
                if (toolbarClass != NULL) {
                    /*
                     * Index of selected browser tab (NOT the tabBar)
                     */
                    const int32_t selectedTabIndex = toolbarClass->getIntegerValue("selectedTabIndex", -1);
                    
                    EventBrowserTabGet getTabContent(selectedTabIndex);
                    EventManager::get()->sendEvent(getTabContent.getPointer());
                    BrowserTabContent* tabContent = getTabContent.getBrowserTab();
                    if (tabContent == NULL) {
                        throw OperationException("Failed to obtain tab number "
                                                 + AString::number(selectedTabIndex + 1)
                                                 + " for window "
                                                 + AString::number(i + 1));
                    }
                    
                    CaretPointer<BrainOpenGLViewportContent> content(BrainOpenGLViewportContent::createViewportForSingleTab(                                                       windowViewport,
                                                       windowViewport,
                                                       windowIndex,
                                                       false, // highlight the tab
                                                       gapsAndMargins,
                                                       tabContent));
                    std::vector<BrainOpenGLViewportContent*> viewportContents;
                    viewportContents.push_back(content);
                    
                    brainOpenGL->drawModels(brain,
                                            viewportContents);
                    
                    const int32_t outputImageIndex = ((numBrowserClasses > 1)
                                                      ? i
                                                      : -1);
                    
                    writeImage(imageFileName,
                               outputImageIndex,
                               imageBuffer,
                               imageWidth,
                               imageHeight);
                    
                }
            }
        }
    }
    
//    delete brainOpenGL;
    
    /*
     * Free image memory and Mesa context
     */
    delete[] imageBuffer;
    OSMesaDestroyContext(mesaContext);
}
#endif // HAVE_OSMESA

/**
 * Create OpenGL Rendering.
 *
 * @param windowIndex
 *     Index of window.
 * @return
 *     BrainOpenGL.
 */
BrainOpenGLFixedPipeline*
OperationShowScene::createBrainOpenGL(const int32_t windowIndex)
{
    /*
     * The OpenGL rendering takes ownership of the text renderer
     * and will delete the text renderer when OpenGL itself
     * is deleted.
     */
    BrainOpenGLTextRenderInterface* textRenderer = NULL;
    if (textRenderer == NULL) {
        textRenderer = new FtglFontTextRenderer();
        if (! textRenderer->isValid()) {
            delete textRenderer;
            textRenderer = NULL;
            CaretLogWarning("Unable to create FTGL Font Renderer.\n"
                            "No text will be available in graphics window.");
        }
    }
    if (textRenderer == NULL) {
        textRenderer = new DummyFontTextRenderer();
    }
    
    /*
     * Performs OpenGL Rendering
     * Allocated dynamically so that it can be destroyed prior to OSMesa being
     * destroyed.  Otherwise, if OpenGL is destroyed after OSMesa, errors
     * will occur as the OpenGL context is invalid when things such as
     * display lists or buffers are deleted.
     */
    BrainOpenGLFixedPipeline* brainOpenGL = new BrainOpenGLFixedPipeline(windowIndex,
                                                                         textRenderer);
    brainOpenGL->initializeOpenGL();
    
    return brainOpenGL;
}


/**
 * Write the image data to a Image File.
 *
 * @param imageFileName
 *     Name of image file.
 * @param imageIndex
 *     Index of image.
 * @param imageContent
 *     content of image.
 * @param imageWidth
 *     width of image.
 * @param imageHeight
 *     height of image.
 */
void
OperationShowScene::writeImage(const AString& imageFileName,
                               const int32_t imageIndex,
                               const unsigned char* imageContent,
                               const int32_t imageWidth,
                               const int32_t imageHeight)
{
    /*
     * Create name of image
     */
    QString outputName(imageFileName);
    if (imageIndex >= 0) {
        const AString imageNumber = QString("_%1").arg((int)(imageIndex + 1),
                                                       2, // width
                                                       10, // base
                                                       QChar('0')); // fill character
        const int dotOffset = outputName.lastIndexOf(".");
        if (dotOffset >= 0) {
            outputName.insert(dotOffset,
                              imageNumber);
        }
        else {
            outputName += (imageNumber
                           + ".png");
        }
    }
    
    try {
        //ImageFile imageFile(image);
        ImageFile imageFile(imageContent,
                            imageWidth,
                            imageHeight,
                            ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM);
        imageFile.writeFile(outputName);
    }
    catch (const DataFileException& dfe) {
        throw OperationException(dfe);
    }
}

/**
 * Is the show scene command available?
 */
bool
OperationShowScene::isShowSceneCommandAvailable()
{
#ifdef HAVE_OSMESA
    return true;
#else  // HAVE_OSMESA
    return false;
#endif  // HAVE_OSMESA
    
}

