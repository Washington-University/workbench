
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

#include "OperationShowSceneTwo.h"

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
#include "EventImageCapture.h"
#include "FtglFontTextRenderer.h"
#include "ImageCaptureDialogSettings.h"
#include "ImageFile.h"
#include "MapYokingGroupEnum.h"
#include "OffScreenSceneRendererOSMesa.h"
#include "OperationException.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneFile.h"
#include "ScenePrimitiveArray.h"
#include "SessionManager.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsLayoutManualConfiguration.h"
#include "VolumeFile.h"

//#include "workbench_png.h"

using namespace caret;

/**
 * \class caret::OperationShowSceneTwo
 * \brief Offscreen rendering of scene to an image file
 *
 * Render a scene into an image file using the Offscreen Mesa Library
 */

/**
 * @return Command line switch
 */
AString
OperationShowSceneTwo::getCommandSwitch()
{
    return "-show-scene-two";
}

/**
 * @return Short description of operation
 */
AString
OperationShowSceneTwo::getShortDescription()
{
    return ("OFFSCREEN RENDERING OF SCENE TO AN IMAGE FILE VERSION TWO");
}

enum ParamKeys : int32_t {
    PARAM_KEY_SCENE_FILE,
    PARAM_KEY_SCENE_NAME_NUMBER,
    PARAM_KEY_IMAGE_FILE_NAME,
    PARAM_KEY_SIZE_IMAGE_WIDTH,
    PARAM_KEY_SIZE_IMAGE_HEIGHT,
    PARAM_KEY_SIZE_IMAGE_WIDTH_AND_HEIGHT,
    PARAM_KEY_SIZE_CAPTURE_DIALOG,
    PARAM_KEY_SIZE_WINDOW,
    PARAM_KEY_OPTION_MARGIN,
    PARAM_KEY_OPTION_NO_SCENE_COLORS,
    PARAM_KEY_OPTION_SET_MAP_YOKING,
    PARAM_KEY_OPTION_CONN_DB_LOGIN,
    PARAM_KEY_SHOW_CAPTURE_SETTINGS,
    PARAM_KEY_OPTION_RENDERER
};

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationShowSceneTwo::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(PARAM_KEY_SCENE_FILE,
                            "scene-file",
                            "scene file");
    
    ret->addStringParameter(PARAM_KEY_SCENE_NAME_NUMBER,
                            "scene-name-or-number",
                            "name or number (starting at one) of the scene in the scene file");
    
    ret->addStringParameter(PARAM_KEY_IMAGE_FILE_NAME,
                            "image-file-name",
                            "output image file name");
    
    const QString windowSizeSwitch("-size-window");
    ret->createOptionalParameter(PARAM_KEY_SIZE_WINDOW,
                                 windowSizeSwitch,
                                 "Output image is size of window's graphics region from when scene was created");
    
    const QString captureDialogSizeSwitch("-size-capture");
    ret->createOptionalParameter(PARAM_KEY_SIZE_CAPTURE_DIALOG,
                                 captureDialogSizeSwitch,
                                 "Output image uses size from Capture Dialog when scene was created");
    
    OptionalParameter* imageWidthAndHeightOpt = ret->createOptionalParameter(PARAM_KEY_SIZE_IMAGE_WIDTH_AND_HEIGHT,
                                                                    "-size-width-height",
                                                                    "Width and height for output image");
    imageWidthAndHeightOpt->addDoubleParameter(1,
                                      "width",
                                      "Width for output image");
    imageWidthAndHeightOpt->addDoubleParameter(2,
                                               "height",
                                               "Height for output image");

    const QString aspectMsg(" is computed using the aspect ratio from the window's width and height saved in the scene.");
    OptionalParameter* imageWidthOpt = ret->createOptionalParameter(PARAM_KEY_SIZE_IMAGE_WIDTH,
                                                                    "-size-width",
                                                                    "Width for output image.  Height" + aspectMsg);
    imageWidthOpt->addDoubleParameter(1,
                                      "width",
                                       "Width for output image");
    
    OptionalParameter* imageHeightOpt = ret->createOptionalParameter(PARAM_KEY_SIZE_IMAGE_HEIGHT,
                                                                     "-size-height",
                                                                     "Height for output image.  Width" + aspectMsg);
    imageHeightOpt->addDoubleParameter(1,
                                        "height",
                                        "Height for output image");

    OptionalParameter* marginOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_MARGIN,
                                                                "margin",
                                                                "Add a margin around the image using the window's background color.");
    marginOpt->addIntegerParameter(1,
                                   "size",
                                   "size of margin added to all sides of output image");
    
    ret->createOptionalParameter(static_cast<int32_t>(PARAM_KEY_OPTION_NO_SCENE_COLORS),
                                 "-no-scene-colors",
                                 "Do not use background and foreground colors in scene");
    
    OptionalParameter* mapYokeOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_SET_MAP_YOKING,
                                                                 "-set-map-yoke",
                                                                 "Override selected map index for a map yoking group.");
    mapYokeOpt->addStringParameter(1,
                                   "map yoking roman numeral",
                                   "Roman numeral identifying the map yoking group (I, II, III, IV, V, VI, VII, VIII, IX, X)");
    mapYokeOpt->addIntegerParameter(2,
                                    "map undex",
                                    "Map index for yoking group.  Indices start at 1 (one)");
    
    OptionalParameter* connDbOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_CONN_DB_LOGIN,
                                                                "-conn-db-login",
                                                                "Login for scenes with files in Connectome Database");
    connDbOpt->addStringParameter(1,
                                  "username",
                                  "Connectome DB Username");
    connDbOpt->addStringParameter(2,
                                  "password",
                                  "Connectome DB Password");
    
    const QString showCaptureSettingsSwitch("-show-capture-settings");
    ret->createOptionalParameter(PARAM_KEY_SHOW_CAPTURE_SETTINGS,
                                 showCaptureSettingsSwitch,
                                 "Print settings from Capture Dialog only, DO NOT create image file");

    const QString rendererSwitch("-renderer");
    OptionalParameter* rendererOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_RENDERER,
                                                                  rendererSwitch,
                                                                  "Select renderer for drawing image");
    rendererOpt->addStringParameter(1, "Renderer", "Name of renderer to use for drawing image");
    

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
                     "\n"
                     "To view settings from Image Capture Dialog \n"
                     "use the " + showCaptureSettingsSwitch + " option (no image file is created).\n"
                     );
    std::vector<AString> readImageFileExtensions, writeImageFileExtensions;
    AString defaultExtension;
    ImageFile::getWorkbenchSupportedImageFileExtensions(readImageFileExtensions,
                                                        writeImageFileExtensions,
                                                        defaultExtension);
    
    for (std::vector<AString>::iterator iter = writeImageFileExtensions.begin();
         iter != writeImageFileExtensions.end();
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
    
    std::vector<std::unique_ptr<OffScreenSceneRendererBase>> allOffScreenRenderers(getOffScreenRenderers());
    if (allOffScreenRenderers.empty()) {
        helpText += ("\n"
                     "No off screen renderers are available on this system.  Command WILL FAIL.");
    }
    else {
        helpText += ("\n"
                     "Renderers available on this computer are (first is default):\n");
        for (const auto& osr : allOffScreenRenderers) {
            helpText += ("   " + osr->getSwitchName() + + " - " + osr->getDescriptiveName() + "\n");
        }
        helpText += ("\n");
        helpText += ("To use a renderer other than the default, use the \""
                     + rendererSwitch
                     + "\" option.");
    }
    
    ret->setHelpText(helpText);
    
    return ret;
}

void
OperationShowSceneTwo::useParameters(OperationParameters* myParams,
                                  ProgressObject* myProgObj)
{
    /*
     * Default to the first renderer
     */
    std::vector<std::unique_ptr<OffScreenSceneRendererBase>> allOffScreenRenderers(getOffScreenRenderers());
    if (allOffScreenRenderers.empty()) {
        throw OperationException("No offscreen renderers are available");
    }
    CaretAssertVectorIndex(allOffScreenRenderers, 0);
    OffScreenSceneRendererBase* offscreenRenderer(allOffScreenRenderers[0].get());

    LevelProgress myProgress(myProgObj);
    AString sceneFileName = FileInformation(myParams->getString(PARAM_KEY_SCENE_FILE)).getAbsoluteFilePath();
    AString sceneNameOrNumber = myParams->getString(PARAM_KEY_SCENE_NAME_NUMBER);
    AString imageFileName = FileInformation(myParams->getString(PARAM_KEY_IMAGE_FILE_NAME)).getAbsoluteFilePath();
    
    const QString moreThanOneSizeOptionMsg("More than one \"-size\" option is specified; must specify one and only one \"size\" option.");
    enum class ImageSizeMode {
        MODE_INVALID,
        MODE_CAPTURE_DIALOG,
        MODE_WINDOW,
        MODE_HEIGHT_FROM_WIDTH,
        MODE_WIDTH_FROM_HEIGHT,
        MODE_WIDTH_AND_HEIGHT
    };
    ImageSizeMode imageSizeMode = ImageSizeMode::MODE_INVALID;
    
    OptionalParameter* imageCaptureDialogSizeOption = myParams->getOptionalParameter(PARAM_KEY_SIZE_CAPTURE_DIALOG);
    OptionalParameter* imageWindowSizeOption = myParams->getOptionalParameter(PARAM_KEY_SIZE_WINDOW);
    OptionalParameter* imageWidthAndHeightOption = myParams->getOptionalParameter(PARAM_KEY_SIZE_IMAGE_WIDTH_AND_HEIGHT);
    OptionalParameter* imageWidthOption  = myParams->getOptionalParameter(PARAM_KEY_SIZE_IMAGE_WIDTH);
    OptionalParameter* imageHeightOption = myParams->getOptionalParameter(PARAM_KEY_SIZE_IMAGE_HEIGHT);
    if (imageCaptureDialogSizeOption->m_present) {
        if (imageSizeMode != ImageSizeMode::MODE_INVALID) {
            throw (moreThanOneSizeOptionMsg);
        }
        imageSizeMode = ImageSizeMode::MODE_CAPTURE_DIALOG;
    }
    if (imageWindowSizeOption->m_present) {
        if (imageSizeMode != ImageSizeMode::MODE_INVALID) {
            throw (moreThanOneSizeOptionMsg);
        }
        imageSizeMode = ImageSizeMode::MODE_WINDOW;
    }
    if (imageWidthAndHeightOption->m_present) {
        if (imageSizeMode != ImageSizeMode::MODE_INVALID) {
            throw (moreThanOneSizeOptionMsg);
        }
        imageSizeMode = ImageSizeMode::MODE_WIDTH_AND_HEIGHT;
    }
    if (imageWidthOption->m_present) {
        if (imageSizeMode != ImageSizeMode::MODE_INVALID) {
            throw (moreThanOneSizeOptionMsg);
        }
        imageSizeMode = ImageSizeMode::MODE_HEIGHT_FROM_WIDTH;
    }
    if (imageHeightOption->m_present) {
        if (imageSizeMode != ImageSizeMode::MODE_INVALID) {
            throw (moreThanOneSizeOptionMsg);
        }
        imageSizeMode = ImageSizeMode::MODE_WIDTH_FROM_HEIGHT;
    }

    float floatImageWidth(-1.0f);
    float floatImageHeight(-1.0f);
    switch (imageSizeMode) {
        case ImageSizeMode::MODE_INVALID:
            throw OperationException("Must specify one and only one of the size options");
            break;
        case ImageSizeMode::MODE_CAPTURE_DIALOG:
            CaretAssert(imageCaptureDialogSizeOption->m_present);
            break;
        case ImageSizeMode::MODE_HEIGHT_FROM_WIDTH:
            CaretAssert(imageWidthOption->m_present);
            floatImageWidth = imageWidthOption->getDouble(1);
            if (floatImageWidth <= 0.0f) {
                throw OperationException("Image width is invalid (zero or less)");
            }
            break;
        case ImageSizeMode::MODE_WIDTH_AND_HEIGHT:
            CaretAssert(imageWidthAndHeightOption->m_present);
            floatImageWidth = imageWidthAndHeightOption->getDouble(1);
            if (floatImageWidth <= 0.0f) {
                throw OperationException("Image width is invalid (zero or less)");
            }
            floatImageHeight = imageWidthAndHeightOption->getDouble(2);
            if (floatImageHeight <= 0.0f) {
                throw OperationException("Image height is invalid (zero or less)");
            }
            break;
        case ImageSizeMode::MODE_WIDTH_FROM_HEIGHT:
            CaretAssert(imageHeightOption->m_present);
            floatImageHeight = imageHeightOption->getDouble(1);
            if (floatImageHeight <= 0.0f) {
                throw OperationException("Image height is invalid (zero or less)");
            }
            break;
        case ImageSizeMode::MODE_WINDOW:
            CaretAssert(imageWindowSizeOption->m_present);
            break;
    }

    const bool doNotUseSceneColorsFlag = myParams->getOptionalParameter(PARAM_KEY_OPTION_NO_SCENE_COLORS)->m_present;
    int32_t marginValue(0);
    OptionalParameter* marginOption = myParams->getOptionalParameter(PARAM_KEY_OPTION_MARGIN);
    if (marginOption->m_present) {
        marginValue = marginOption->getInteger(1);
        if (marginValue < 1) {
            throw OperationException("Margin="
                                     + QString::number(marginValue)
                                     + " is invalid.");
        }
    }
    
    MapYokingGroupEnum::Enum mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    int32_t mapYokingMapIndex = -1;
    OptionalParameter* mapYokeOpt = myParams->getOptionalParameter(PARAM_KEY_OPTION_SET_MAP_YOKING);
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
        
    /*
     * Need to set username/password for files in ConnectomeDB
     */
    AString username;
    AString password;
    OptionalParameter* connDbOpt = myParams->getOptionalParameter(PARAM_KEY_OPTION_CONN_DB_LOGIN);
    if (connDbOpt->m_present) {
        username = connDbOpt->getString(1);
        password = connDbOpt->getString(2);
    }
    setRemoteLoginAndPassword(username,
                              password);

    OptionalParameter* showSettingsOpt = myParams->getOptionalParameter(PARAM_KEY_SHOW_CAPTURE_SETTINGS);

    OptionalParameter* rendererOpt = myParams->getOptionalParameter(PARAM_KEY_OPTION_RENDERER);
    if (rendererOpt->m_present) {
        const AString rendererName(rendererOpt->getString(1).toLower());
        
        bool foundFlag(false);
        for (auto& osr : allOffScreenRenderers) {
            if (rendererName == osr->getSwitchName().toLower()) {
                offscreenRenderer = osr.get();
                foundFlag = true;
            }
        }
        if ( ! foundFlag) {
            throw OperationException("Selected renderer with name \""
                                     + rendererName
                                     + " is not the name of a valid renderer on this system.");
        }
    }
    
    AString sceneErrorMessage;
    loadSceneFileAndRestoreScene(sceneFileName,
                                 sceneNameOrNumber,
                                 doNotUseSceneColorsFlag,
                                 sceneErrorMessage);
    
    applyMapYoking(mapYokingGroup,
                   mapYokingMapIndex);

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
        throw OperationException("No BrowserWindowContent was found for showing as scene.  "
                                 "This may occur with very old scene files.  Loading, Showing, and "
                                 "Saving the scenes in Workbench will fix this problem.");
    }
    
    const ImageCaptureDialogSettings* sceneCaptureSettings(SessionManager::get()->getImageCaptureDialogSettings());
    
    /*
     * Showing the settings must be done after the scene has been loaded since
     * it uses information from the scene
     */
    if (showSettingsOpt->m_present) {
        std::vector<int32_t> windowIndices;
        std::vector<int32_t> windowWidths;
        std::vector<int32_t> windowHeights;
        for (int32_t iWindow = 0; iWindow < numberOfWindows; iWindow++) {
            CaretAssertVectorIndex(allBrowserWindowContent, iWindow);
            auto bwc = allBrowserWindowContent[iWindow];
            windowIndices.push_back(bwc->getWindowIndex());
            windowWidths.push_back(bwc->getSceneGraphicsWidth());
            windowHeights.push_back(bwc->getSceneGraphicsHeight());
        }
        CaretAssert(windowWidths.size() == windowHeights.size());
        
        std::cout << sceneCaptureSettings->getSettingsAsText(windowIndices,
                                                        windowWidths,
                                                        windowHeights) << std::endl;
        return;
    }
    
    if ( ! offscreenRenderer) {
        throw OperationException("No offscreen renderer is selected");
    }
    if ( ! offscreenRenderer->isAvailable()) {
        throw OperationException(offscreenRenderer->getSwitchName() + " is not available on this system.");
    }

    /*
     * Restore windows
     */
    for (int32_t iWindow = 0; iWindow < numberOfWindows; iWindow++) {
        CaretAssertVectorIndex(allBrowserWindowContent, iWindow);
        auto bwc = allBrowserWindowContent[iWindow];
        
        std::unique_ptr<ImageCaptureDialogSettings> imageSettings;
        
        int32_t imageWidth(-1);
        int32_t imageHeight(-1);
        
        const float windowWidth = bwc->getSceneGraphicsWidth();
        const float windowHeight = bwc->getSceneGraphicsHeight();
        const float aspectRatio((windowWidth > 0.0)
                                ? (windowHeight / windowWidth)
                                : 0.0);
        switch (imageSizeMode) {
            case ImageSizeMode::MODE_INVALID:
                CaretAssert(0);
                break;
            case ImageSizeMode::MODE_CAPTURE_DIALOG:
                CaretAssertToDoFatal();
                break;
            case ImageSizeMode::MODE_HEIGHT_FROM_WIDTH:
                if (aspectRatio <= 0.0) {
                    throw OperationException("Width and/or height of window from scene is invalid or missing (old scene)"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both width and height options must be specified");
                }
                imageHeight = static_cast<int32_t>(aspectRatio * floatImageWidth);
                if (imageHeight <= 0.0) {
                    throw OperationException("Failed to compute height from width.  Width and/or height of window from scene may be invalid"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both width and height options must be specified");
                }
                imageWidth = static_cast<int32_t>(floatImageWidth);
                break;
            case ImageSizeMode::MODE_WIDTH_AND_HEIGHT:
                imageWidth  = static_cast<int32_t>(floatImageWidth);
                imageHeight = static_cast<int32_t>(floatImageHeight);
                break;
            case ImageSizeMode::MODE_WIDTH_FROM_HEIGHT:
                break;
                if (aspectRatio <= 0.0) {
                    throw OperationException("Width and/or height of window from scene is invalid or missing (old scene)"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both width and height options must be specified");
                }
                imageWidth = static_cast<int32_t>(floatImageWidth / aspectRatio);
                if (imageWidth <= 0.0) {
                    throw OperationException("Failed to compute width from height.  Width and/or height of window from scene may be invalid"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both width and height options must be specified");
                }
                imageHeight = static_cast<int32_t>(floatImageHeight);
            case ImageSizeMode::MODE_WINDOW:
                imageWidth  = static_cast<int32_t>(windowWidth);
                imageHeight = static_cast<int32_t>(windowHeight);
                if ((imageWidth <= 0)
                    || (imageHeight <= 0)) {
                    throw OperationException("Width and/or height of window from scene is invalid or missing (old scene)"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both  width and height options must be specified");
                }
                break;
        }

        if ((imageWidth < 0)
            || (imageHeight < 0)) {
            throw OperationException("Image width="
                                     + QString::number(imageWidth)
                                     + " and/or height="
                                     + QString::number(imageHeight)
                                     + " is invalid.");
        }
        
        /*
         * Setup capture event instance with image parameters
         * (event is not sent, just its instance is used)
         */
        EventImageCapture captureEvent(bwc->getWindowIndex(),
                                       0,
                                       0,
                                       imageWidth,
                                       imageHeight,
                                       imageWidth,
                                       imageHeight);
        captureEvent.setMargin(marginValue);
        
        const int32_t outputImageIndex = ((numberOfWindows > 1)
                                          ? iWindow
                                          : -1);
        Inputs inputs(offscreenRenderer,
                      bwc,
                      &captureEvent,
                      imageFileName,
                      outputImageIndex,
                      doNotUseSceneColorsFlag);
        
        renderWindowToImage(inputs);
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
 * Override map yoking group and map index in the scene
 * @param mapYokingGroup
 *    The map yoking group
 * @param mapYokingMapIndex
 *    The map index
 */
void
OperationShowSceneTwo::applyMapYoking(const MapYokingGroupEnum::Enum mapYokingGroup,
                                      const int32_t mapYokingMapIndex)
{
    if (mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        MapYokingGroupEnum::setSelectedMapIndex(mapYokingGroup, mapYokingMapIndex);
        
        EventMapYokingSelectMap yokeEvent(mapYokingGroup,
                                          NULL,
                                          NULL,
                                          mapYokingMapIndex,
                                          true);
        EventManager::get()->sendEvent(yokeEvent.getPointer());
    }
}

/**
 * Load the scene file followed by restoring the scene
 * @param sceneFileName
 *    Name of scene file
 * @param sceneNameOrNumber
 *    Either the scene number or name
 * @param doNotUseSceneColorsFlag
 *    Do not use background/foreground colors from the scene
 * @param errorMessageOut
 *    Output with error information
 */
void
OperationShowSceneTwo::loadSceneFileAndRestoreScene(const AString& sceneFileName,
                                                    const AString& sceneNameOrNumber,
                                                    const bool doNotUseSceneColorsFlag,
                                                    AString& errorMessageOut)
{
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
    errorMessageOut = sceneAttributes.getErrorMessage();
    
    if (sessionManager->getNumberOfBrains() <= 0) {
        throw OperationException("Scene loading failure, SessionManager contains no Brains");
    }

}


/**
 * Setup remote login username and password for reading files.
 * If username is empty, both username and password are obtained from the user's preferences.
 */
void
OperationShowSceneTwo::setRemoteLoginAndPassword(const AString& username,
                                                 const AString& password)
{
    AString loginName(username);
    AString loginPassword(password);
    
    if (username.isEmpty()) {
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        prefs->getRemoteFileUserNameAndPassword(loginName,
                                                loginPassword);
    }
    CaretDataFile::setFileReadingUsernameAndPassword(loginName,
                                                     loginPassword);
}


///*
// * Get width and height for image
// * @param inputs
// *     Inputs for processing
// * @param widthOut
// *     Output image width
// * @param heightOut
// *     Output image height
// */
//void
//OperationShowSceneTwo::getImageWidthAndHeight(const Inputs& inputs,
//                                              int32_t& widthOut,
//                                              int32_t& heightOut)
//{
//    BrowserWindowContent* bwc(inputs.m_browserWindowContent);
//    CaretAssert(bwc);
//
//    int32_t imageWidth(inputs.m_imageWidth);
//    int32_t imageHeight(inputs.m_imageHeight);
//
//    const float geomWidth = bwc->getSceneGraphicsWidth();
//    const float geomHeight = bwc->getSceneGraphicsHeight();
//
//    if (inputs.m_useWindowSizeForImageSizeFlag) {
//        if ((geomWidth > 0)
//            && (geomHeight > 0)) {
//            imageWidth = geomWidth;
//            imageHeight = geomHeight;
//        }
//        else {
//            throw OperationException("Scene is very old and does not contain size of window.  Use option with width/height.");
//        }
//    }
//    else if (imageHeight <= 0) {
//        /*
//         * Use aspect from graphics region to set image height
//         */
//        if ((geomWidth > 0)
//            && (geomHeight > 0)) {
//            const float aspect(static_cast<float>(geomHeight) / static_cast<float>(geomWidth));
//            imageHeight = imageWidth * aspect;
//        }
//        else {
//            throw OperationException("Scene is very old and does not contain size of window.  Must specify height.");
//        }
//    }
//
//    if ((imageWidth <= 0)
//        || (imageHeight <= 0)) {
//        throw OperationException("Invalid image size width="
//                                 + QString::number(imageWidth)
//                                 + ", height="
//                                 + QString::number(imageHeight));
//    }
//
//    widthOut  = imageWidth;
//    heightOut = imageHeight;
//}

/**
 * Render a window to an image
 * @param inputs
 *  Inputs for procssing
 */
void
OperationShowSceneTwo::renderWindowToImage(Inputs& inputs)
{
    BrowserWindowContent* bwc(inputs.m_browserWindowContent);
    CaretAssert(bwc);
    
    Brain* brain = SessionManager::get()->getBrain(0);
    
    const bool restoreToTabTiles = bwc->isTileTabsEnabled();
    const int32_t windowIndex = bwc->getWindowIndex();
    
    int32_t imageWidth(inputs.m_imageCaptureEvent->getOutputWidthExcludingMargin());
    int32_t imageHeight(inputs.m_imageCaptureEvent->getOutputHeightExcludingMargin());
//    getImageWidthAndHeight(inputs,
//                           imageWidth,
//                           imageHeight);
    CaretAssert(imageWidth > 0);
    CaretAssert(imageHeight > 0);
    
    //
    // Allocate image buffer
    //
    const int32_t imageBufferSize = imageWidth * imageHeight * 4 * sizeof(unsigned char);
    unsigned char* imageBuffer = new unsigned char[imageBufferSize];
    if (imageBuffer == 0) {
        throw OperationException("Allocating image buffer size="
                                 + QString::number(imageBufferSize)
                                 + " failed.");
    }
    
    std::unique_ptr<BrainOpenGL> brainOpenGL;
    
    if ( ! inputs.m_offscreenRenderer->initialize(imageWidth,
                                                  imageHeight)) {
        throw OperationException(inputs.m_offscreenRenderer->getErrorMessage());
    }
    
    brainOpenGL.reset(createBrainOpenGL());
    
    int windowViewport[4] = { 0, 0, imageWidth, imageHeight };
    const int windowBeforeAspectLockingViewport[4] = { 0, 0, imageWidth, imageHeight };
    
    const int windowWidth  = windowViewport[2];
    const int windowHeight = windowViewport[3];
    
    /*
     * If tile tabs was saved to the scene, restore it as the scenes tile tabs configuration
     */
    if (restoreToTabTiles) {
//        CaretPointer<BrainOpenGL> brainOpenGL(createBrainOpenGL());
        
        TileTabsLayoutGridConfiguration* gridConfig = NULL;
        bool manualFlag(false);
        switch (bwc->getTileTabsConfigurationMode()) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                gridConfig = bwc->getCustomGridTileTabsConfiguration();
                break;
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
                gridConfig = bwc->getCustomGridTileTabsConfiguration();
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
                manualFlag = true;
                break;
        }
        
        if ((gridConfig != NULL)
            || manualFlag) {
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
                
                if (gridConfig != NULL) {
                    std::vector<int32_t> rowHeights;
                    std::vector<int32_t> columnWidths;
                    if ( ! gridConfig->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                                 windowHeight,
                                                                                 numTabContent,
                                                                                 bwc->getTileTabsConfigurationMode(),
                                                                                 rowHeights,
                                                                                 columnWidths)) {
                        throw OperationException("Tile Tabs Row/Column sizing failed !!!");
                    }
                }
                
                const int32_t tabIndexToHighlight = -1;
                std::vector<BrainOpenGLViewportContent*> viewports =
                BrainOpenGLViewportContent::createViewportContentForTileTabs(allTabContent,
                                                                             bwc,
                                                                             brain->getGapsAndMargins(),
                                                                             windowBeforeAspectLockingViewport,
                                                                             windowViewport,
                                                                             windowIndex,
                                                                             tabIndexToHighlight);
                
                std::vector<const BrainOpenGLViewportContent*> constViewports(viewports.begin(),
                                                                              viewports.end());
                brainOpenGL->drawModels(windowIndex,
                                        UserInputModeEnum::Enum::VIEW,
                                        brain,
                                        inputs.m_offscreenRenderer->getDrawingContext(),
                                        constViewports);

                /*
                 * Image capture event will set resolution and add margin
                 */
                inputs.m_imageCaptureEvent->setCapturedImage(*(inputs.m_offscreenRenderer->getImageFile()->getAsQImage()));
                QImage outputImage = inputs.m_imageCaptureEvent->getCapturedImage();
                ImageFile outputImageFile;
                outputImageFile.setFromQImage(outputImage);
                
                /*
                 * Write the image file
                 */
                writeImageFile(inputs.m_imageFileName,
                               inputs.m_outputImageIndex,
                               &outputImageFile);
                
                for (std::vector<BrainOpenGLViewportContent*>::iterator vpIter = viewports.begin();
                     vpIter != viewports.end();
                     vpIter++) {
                    delete *vpIter;
                }
                viewports.clear();
            }
        }
        else {
            throw OperationException("Tile tabs configuration is neither Grid nor Manual");
        }
    }
    else {
        const int32_t selectedTabIndex = bwc->getSceneSelectedTabIndex();
        
        EventBrowserTabGet getTabContent(selectedTabIndex);
        EventManager::get()->sendEvent(getTabContent.getPointer());
        BrowserTabContent* tabContent = getTabContent.getBrowserTab();
        if (tabContent == NULL) {
            throw OperationException("Failed to obtain tab number "
                                     + AString::number(selectedTabIndex + 1)
                                     + " for window "
                                     + AString::number(windowIndex + 1));
        }
        
        CaretPointer<BrainOpenGLViewportContent> content(NULL);
        std::vector<BrowserTabContent*> allTabs;
        allTabs.push_back(tabContent);
        content.grabNew(BrainOpenGLViewportContent::createViewportForSingleTab(allTabs,
                                                                               tabContent,
                                                                               brain->getGapsAndMargins(),
                                                                               windowIndex,
                                                                               windowBeforeAspectLockingViewport,
                                                                               windowViewport));
        std::vector<const BrainOpenGLViewportContent*> viewportContents;
        viewportContents.push_back(content);
        
        brainOpenGL->drawModels(windowIndex,
                                UserInputModeEnum::Enum::VIEW,
                                brain,
                                inputs.m_offscreenRenderer->getDrawingContext(),
                                viewportContents);

        writeImageFile(inputs.m_imageFileName,
                       inputs.m_outputImageIndex,
                       inputs.m_offscreenRenderer->getImageFile());
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
OperationShowSceneTwo::estimateGraphicsSize(const SceneClass* windowSceneClass,
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
OperationShowSceneTwo::getToolBoxSize(const SceneClass* toolBoxClass,
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
OperationShowSceneTwo::createBrainOpenGL()
{
    /*
     * The OpenGL rendering takes ownership of the text renderer
     * and will delete the text renderer when OpenGL itself
     * is deleted.
     */
    BrainOpenGLTextRenderInterface* textRenderer = NULL;
    if (textRenderer == NULL) {
#ifdef HAVE_FREETYPE
        textRenderer = new FtglFontTextRenderer();
        if (! textRenderer->isValid()) {
            delete textRenderer;
            textRenderer = NULL;
            CaretLogWarning("Unable to create FTGL Font Renderer.\n"
                            "No text will be available in graphics window.");
        }
#else
        CaretLogWarning("Unable to create FTGL Font Renderer due to FreeType not found during configuration.\n"
                        "No text will be available in graphics window.");
#endif
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

/**
 * Write the image data to a Image File.
 *
 * @param imageFileName
 *     Name of image file.
 * @param imageIndex
 *     Index of image.
 * @param imageFile
 *     Image file for writing.
 */
void
OperationShowSceneTwo::writeImageFile(const AString& imageFileName,
                                      const int32_t imageIndex,
                                      const ImageFile* imageFile)
{
    CaretAssert(imageFile);
    
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
        const QImage* qImage = imageFile->getAsQImage();
        if (qImage == NULL) {
            throw OperationException("QImage in ImageFile is missing (NULL)");
        }
        if (qImage->isNull()) {
            throw OperationException("QImage in ImageFile created by offscreen renderer is NULL");
        }
        const_cast<ImageFile*>(imageFile)->writeFile(outputName);
    }
    catch (const DataFileException& dfe) {
        throw OperationException(dfe);
    }
}

/**
 * @return Instances of all available renderers
 */
std::vector<std::unique_ptr<OffScreenSceneRendererBase>>
OperationShowSceneTwo::getOffScreenRenderers()
{
    std::vector<std::unique_ptr<OffScreenSceneRendererBase>> renderers;
    
    std::unique_ptr<OffScreenSceneRendererBase> mesaRenderer(new OffScreenSceneRendererOSMesa());
    if (mesaRenderer->isAvailable()) {
        renderers.push_back(std::move(mesaRenderer));
    }
    
    return renderers;
}

/**
 * Is the show scene command available?
 */
bool
OperationShowSceneTwo::isShowSceneCommandAvailable()
{
    /*
     * True if at least one renderer is available
     */
    return ( ! getOffScreenRenderers().empty());
}

