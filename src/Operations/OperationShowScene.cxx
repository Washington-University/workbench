
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

#ifdef HAVE_GLEW
#include <GL/glew.h>
#endif

#ifdef HAVE_OSMESA
#include <GL/osmesa.h>
#endif // HAVE_OSMESA

#include <QImage>
#include <QColor>


#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLViewportContent.h"
#include "BrainOpenGLWindowContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserWindowContent.h"
#include "EventMapYokingSelectMap.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "DummyFontTextRenderer.h"
#include "FtglFontTextRenderer.h"
#include "ImageFile.h"
#include "MapYokingGroupEnum.h"
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
    
    ret->createOptionalParameter(7, "-no-scene-colors", "Do not use background and foreground colors in scene");
    
    OptionalParameter* mapYokeOpt = ret->createOptionalParameter(8, "-set-map-yoke", "Override selected map index for a map yoking group.");
    mapYokeOpt->addStringParameter(1, "Map Yoking Roman Numeral", "Roman numeral identifying the map yoking group (I, II, III, IV, V, VI, VII, VIII, IX, X)");
    mapYokeOpt->addIntegerParameter(2, "Map Index", "Map index for yoking group.  Indices start at 1 (one)");
    
    OptionalParameter* connDbOpt = ret->createOptionalParameter(9, "-conn-db-login", "Login for scenes with files in Connectome Database");
    connDbOpt->addStringParameter(1, "Username", "Connectome DB Username");
    connDbOpt->addStringParameter(2, "Password", "Connectome DB Password");
    
    AString helpText("Render content of browser windows displayed in a scene "
                     "into image file(s).  The image file name should be "
                     "similar to \"capture.png\".  If there is only one image "
                     "to render, the image name will not change.  If there is "
                     "more than one image to render, an index will be inserted "
                     "into the image name: \"capture_01.png\", \"capture_02.png\" "
                     "etc.\n"
                     "\n"
                     "If the scene references files in the Connectome Database,\n"
                     "the \"-conn-db-login\" option is available for providing the \n"
                     "username and password.  If this options is not specified, \n"
                     "the username and password stored in the user's preferences\n"
                     "is used.\n"
                     "\n"
                     "The image format is determined by the image file extension.\n"
                     "The available image formats may vary by operating system.\n"
                     "Image formats available on this system are:\n"
                     );
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
                 "    * Versions 1.2 and newer contain the width and \n"
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
    LevelProgress myProgress(myProgObj);
    AString sceneFileName = FileInformation(myParams->getString(1)).getAbsoluteFilePath();
    AString sceneNameOrNumber = myParams->getString(2);
    AString imageFileName = FileInformation(myParams->getString(3)).getAbsoluteFilePath();
    const int32_t userImageWidth  = myParams->getInteger(4);
    const int32_t userImageHeight = myParams->getInteger(5);
    
    OptionalParameter* useWindowSizeParam = myParams->getOptionalParameter(6);
    const bool useWindowSizeForImageSizeFlag = useWindowSizeParam->m_present;
    
    const bool doNotUseSceneColorsFlag = myParams->getOptionalParameter(7)->m_present;
    
    MapYokingGroupEnum::Enum mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    int32_t mapYokingMapIndex = -1;
    OptionalParameter* mapYokeOpt = myParams->getOptionalParameter(8);
    if (mapYokeOpt->m_present) {
        const AString romanNumeral = mapYokeOpt->getString(1);
        bool validFlag = false;
        mapYokingGroup = MapYokingGroupEnum::fromGuiName(romanNumeral, &validFlag);
        if ( ! validFlag) {
            throw OperationException(romanNumeral
                                     + " does not identify a valid Map Yoking Group.  ");
        }
        mapYokingMapIndex = mapYokeOpt->getInteger(2);
        if (mapYokingMapIndex < 1) {
            throw OperationException("Map yoking map index must be one or greater.");
        }
        
        /*
         * Map indice in code start at zero
         */
        mapYokingMapIndex--;
    }
    
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
     * Need to set username/password for files in ConnectomeDB
     */
    AString username;
    AString password;
    OptionalParameter* connDbOpt = myParams->getOptionalParameter(9);
    if (connDbOpt->m_present) {
        username = connDbOpt->getString(1);
        password = connDbOpt->getString(2);
    }
    else {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->getRemoteFileUserNameAndPassword(username,
                                                password);
    }
    CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                     password);

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
    
    SceneAttributes sceneAttributes(SceneTypeEnum::SCENE_TYPE_FULL,
                                    scene);
    
    if (doNotUseSceneColorsFlag) {
        sceneAttributes.setUseSceneForegroundAndBackgroundColors(false);
    }
    
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
    
    /*
     * Get the error message but continue processing since the error
     * may not affect the scene.  Print error message later.
     */
    const AString sceneErrorMessage = sceneAttributes.getErrorMessage();
    
    if (sessionManager->getNumberOfBrains() <= 0) {
        throw OperationException("Scene loading failure, SessionManager contains no Brains");
    }
    Brain* brain = SessionManager::get()->getBrain(0);
    
    const GapsAndMargins* gapsAndMargins = brain->getGapsAndMargins();
    
    bool missingWindowMessageHasBeenDisplayed = false;
    
    /*
     * Apply map yoking
     */
    if (mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        MapYokingGroupEnum::setSelectedMapIndex(mapYokingGroup, mapYokingMapIndex);
        
        EventMapYokingSelectMap yokeEvent(mapYokingGroup,
                                          NULL,
                                          NULL,
                                          mapYokingMapIndex,
                                          true);
        EventManager::get()->sendEvent(yokeEvent.getPointer());
    }
    
    std::vector<BrowserWindowContent*> allBrowserWindowContent;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        std::unique_ptr<EventBrowserWindowContent> browserContentEvent = EventBrowserWindowContent::getWindowContent(i);
        EventManager::get()->sendEvent(browserContentEvent->getPointer());
        BrowserWindowContent* bwc = browserContentEvent->getBrowserWindowContent();
        CaretAssert(bwc);
        if (bwc->isValid()) {
            allBrowserWindowContent.push_back(bwc);
        }
    }
    const int32_t numberOfWindows = static_cast<int32_t>(allBrowserWindowContent.size());
    if (numberOfWindows <= 0) {
        throw OperationException("No BrowserWindowContent was found for showing as scene");
    }
    
    /*
     * Restore windows
     */
    for (int32_t iWindow = 0; iWindow < numberOfWindows; iWindow++) {
        CaretAssertVectorIndex(allBrowserWindowContent, iWindow);
        auto bwc = allBrowserWindowContent[iWindow];
        
        const bool restoreToTabTiles = bwc->isTileTabsEnabled();
        const int32_t windowIndex = bwc->getWindowIndex();
        
        int32_t imageWidth  = userImageWidth;
        int32_t imageHeight = userImageHeight;
        
        if (useWindowSizeForImageSizeFlag) {
            /*
             * Requires version AFTER 1.2.0-pre1
             */
            const float geomWidth = bwc->getSceneGraphicsWidth();
            const float geomHeight = bwc->getSceneGraphicsHeight();
            if ((geomWidth > 0)
                && (geomHeight > 0)) {
                imageWidth = geomWidth;
                imageHeight = geomHeight;
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
        
        int windowViewport[4] = { 0, 0, imageWidth, imageHeight };
        
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
        
        /*
         * If tile tabs was saved to the scene, restore it as the scenes tile tabs configuration
         */
        if (restoreToTabTiles) {
            CaretPointer<BrainOpenGL> brainOpenGL(createBrainOpenGL());
            
            TileTabsConfiguration* tileTabsConfiguration = bwc->getSelectedTileTabsConfiguration();
            CaretAssert(tileTabsConfiguration);
            if ((tileTabsConfiguration->getMaximumNumberOfRows() > 0)
                && (tileTabsConfiguration->getMaximumNumberOfColumns() > 0)) {
                
                const std::vector<int32_t> tabIndices = bwc->getSceneTabIndices();
                if ( ! tabIndices.empty()) {
                    std::vector<BrowserTabContent*> allTabContent;
                    const int32_t numTabs = static_cast<int32_t>(tabIndices.size());
                    for (int32_t iTab = 0; iTab < numTabs; iTab++) {
                        CaretAssertVectorIndex(tabIndices, iTab);
                        const int32_t tabIndex = tabIndices[iTab];
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
                    
                    const int32_t numTabContent = static_cast<int32_t>(allTabContent.size());
                    if (numTabContent <= 0) {
                        throw OperationException("Failed to find any tab content");
                    }
                    std::vector<int32_t> rowHeights;
                    std::vector<int32_t> columnWidths;
                    if ( ! tileTabsConfiguration->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                                            windowHeight,
                                                                                            numTabContent,
                                                                                            bwc->getTileTabsConfigurationMode(),
                                                                                            rowHeights,
                                                                                            columnWidths)) {
                        throw OperationException("Tile Tabs Row/Column sizing failed !!!");
                    }
                    
                    const int32_t tabIndexToHighlight = -1;
                    std::vector<BrainOpenGLViewportContent*> viewports =
                    BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabContent,
                                                                                 bwc,
                                                                                 gapsAndMargins,
                                                                                 windowViewport,
                                                                                 tabIndexToHighlight);
                    
                    std::vector<const BrainOpenGLViewportContent*> constViewports(viewports.begin(),
                                                                                  viewports.end());
                    brainOpenGL->drawModels(windowIndex,
                                            brain,
                                            mesaContext,
                                            constViewports);
                    
                    const int32_t outputImageIndex = ((numberOfWindows > 1)
                                                      ? iWindow
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
            CaretPointer<BrainOpenGL> brainOpenGL(createBrainOpenGL());
            
            const int32_t selectedTabIndex = bwc->getSceneSelectedTabIndex();
            
            EventBrowserTabGet getTabContent(selectedTabIndex);
            EventManager::get()->sendEvent(getTabContent.getPointer());
            BrowserTabContent* tabContent = getTabContent.getBrowserTab();
            if (tabContent == NULL) {
                throw OperationException("Failed to obtain tab number "
                                         + AString::number(selectedTabIndex + 1)
                                         + " for window "
                                         + AString::number(iWindow + 1));
            }
            
            CaretPointer<BrainOpenGLViewportContent> content(NULL);
            std::vector<BrowserTabContent*> allTabs;
            allTabs.push_back(tabContent);
            content.grabNew(BrainOpenGLViewportContent::createViewportForSingleTab(allTabs,
                                                                                   tabContent,
                                                                                   gapsAndMargins,
                                                                                   windowIndex,
                                                                                   windowViewport));
            std::vector<const BrainOpenGLViewportContent*> viewportContents;
            viewportContents.push_back(content);
            
            brainOpenGL->drawModels(windowIndex,
                                    brain,
                                    mesaContext,
                                    viewportContents);
            
            const int32_t outputImageIndex = ((numberOfWindows > 1)
                                              ? iWindow
                                              : -1);
            
            writeImage(imageFileName,
                       outputImageIndex,
                       imageBuffer,
                       imageWidth,
                       imageHeight);
        }
        
        /*
         * Free image memory and Mesa context
         */
        delete[] imageBuffer;
        OSMesaDestroyContext(mesaContext);
    }
    
    /*
     * Print error messages
     */
    if ( ! sceneErrorMessage.isEmpty()) {
        std::cerr << "ERRORS loading scene, output image may be incorrect." << std::endl;
        std::cerr << sceneErrorMessage << std::endl;
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

/**
 * Create OpenGL Rendering.
 *
 * @return
 *     BrainOpenGL.
 */
BrainOpenGLFixedPipeline*
OperationShowScene::createBrainOpenGL()
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
    BrainOpenGLFixedPipeline* brainOpenGL = new BrainOpenGLFixedPipeline(textRenderer);
    brainOpenGL->initializeOpenGL();
    
    return brainOpenGL;
}

#endif // HAVE_OSMESA

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

