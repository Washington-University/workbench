
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
#include "EventGraphicsOpenGLDeleteTextureName.h"
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
#include "OperationShowScene.h"
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
    return "-scene-capture-image";
}

/**
 * @return Short description of operation
 */
AString
OperationShowSceneTwo::getShortDescription()
{
    return ("OFFSCREEN RENDERING OF SCENE TO AN IMAGE FILE");
}

enum ParamKeys : int32_t {
    PARAM_KEY_SCENE_FILE,
    PARAM_KEY_SCENE_NAME_NUMBER,
    PARAM_KEY_IMAGE_FILE_NAME,
    PARAM_OPTION_KEY_SIZE_IMAGE_WIDTH,
    PARAM_OPTION_KEY_SIZE_IMAGE_HEIGHT,
    PARAM_OPTION_KEY_SIZE_IMAGE_WIDTH_AND_HEIGHT,
    PARAM_OPTION_KEY_SIZE_CAPTURE_DIALOG,
    PARAM_OPTION_KEY_SIZE_WINDOW,
    PARAM_KEY_OPTION_CONN_DB_LOGIN,
    PARAM_KEY_OPTION_MARGIN,
    PARAM_KEY_OPTION_NO_SCENE_COLORS,
    PARAM_KEY_OPTION_PRINT_IMAGE_INFO,
    PARAM_KEY_OPTION_RENDERER,
    PARAM_KEY_OPTION_RESOLUTION,
    PARAM_KEY_OPTION_SET_MAP_YOKING,
    PARAM_KEY_OPTION_SIZE_UNITS,
    PARAM_KEY_SHOW_CAPTURE_SETTINGS
};

/**
 * @return Parameters for operation
 */
OperationParameters*
OperationShowSceneTwo::getParameters()
{
    /*
     * Get valid image file extenions
     */
    std::vector<AString> readImageFileExtensions, writeImageFileExtensions;
    AString defaultExtension;
    ImageFile::getWorkbenchSupportedImageFileExtensions(readImageFileExtensions,
                                                        writeImageFileExtensions,
                                                        defaultExtension);
    
    AString imageExtensionList("The file name must end with a valid extension that identifies the image file format.  "
                               "Valid extensions on this system are: (");
    bool firstExtFlag(true);
    for (auto& ext : writeImageFileExtensions) {
        if ( ! firstExtFlag) {
            imageExtensionList += " ";
        }
        imageExtensionList += ("." + ext);
        firstExtFlag = false;
    }
    imageExtensionList += (").");
    
    /*
     * Required parameters
     */
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(PARAM_KEY_SCENE_FILE,
                            "scene-file",
                            "scene file");
    
    ret->addStringParameter(PARAM_KEY_SCENE_NAME_NUMBER,
                            "scene-name-or-number",
                            "name or number (starting at one) of the scene in the scene file");
    
    ret->addStringParameter(PARAM_KEY_IMAGE_FILE_NAME,
                            "image-file-name",
                            ("output - image file name\n"
                             "   "
                             + imageExtensionList
                             + "\n\n   "
                             "If there is more than one window in the scene, multiple image files are output "
                             "with the window's number inserted into the name of the image file immediately before "
                             "the image file's extension."));
    
    /*
     * Image size parameters (one and only one must be specified)
     */
    const QString windowSizeSwitch("-size-window");
    ret->createOptionalParameter(PARAM_OPTION_KEY_SIZE_WINDOW,
                                 windowSizeSwitch,
                                 "Output image is size of window's graphics region from when scene was created.");
    
    const QString captureDialogSizeSwitch("-size-capture");
    ret->createOptionalParameter(PARAM_OPTION_KEY_SIZE_CAPTURE_DIALOG,
                                 captureDialogSizeSwitch,
                                 "Output image uses size from Capture Dialog when scene was created");
    
    const QString widthHeightSizeSwitch("-size-width-height");
    OptionalParameter* imageWidthAndHeightOpt = ret->createOptionalParameter(PARAM_OPTION_KEY_SIZE_IMAGE_WIDTH_AND_HEIGHT,
                                                                    widthHeightSizeSwitch,
                                                                    "Width and height for output image");
    imageWidthAndHeightOpt->addDoubleParameter(1,
                                      "width",
                                      "Width for output image");
    imageWidthAndHeightOpt->addDoubleParameter(2,
                                               "height",
                                               "Height for output image");

    const QString aspectMsg(" is computed using the aspect ratio from the window's width and height saved in the scene.");
    OptionalParameter* imageWidthOpt = ret->createOptionalParameter(PARAM_OPTION_KEY_SIZE_IMAGE_WIDTH,
                                                                    "-size-width",
                                                                    "Width for output image.  Height" + aspectMsg);
    imageWidthOpt->addDoubleParameter(1,
                                      "width",
                                       "Width for output image");
    
    OptionalParameter* imageHeightOpt = ret->createOptionalParameter(PARAM_OPTION_KEY_SIZE_IMAGE_HEIGHT,
                                                                     "-size-height",
                                                                     "Height for output image.  Width" + aspectMsg);
    imageHeightOpt->addDoubleParameter(1,
                                        "height",
                                        "Height for output image");

    /*
     * Option to set the units for the image width/height
     */
    std::vector<ImageSpatialUnitsEnum::Enum> spatialUnits;
    ImageSpatialUnitsEnum::getAllEnums(spatialUnits);
    AString spatialUnitsList("  Valid units are:\n");
    for (const auto unitEnum : spatialUnits) {
        
        spatialUnitsList += ("   " + ImageSpatialUnitsEnum::toName(unitEnum) + "\n");
    }
    const QString unitsSwitchName("-units");
    const AString defaultSpatialUnitsText("      Default is "
                                          + ImageSpatialUnitsEnum::toName(s_defaultImageWidthHeightUnits));
    OptionalParameter* sizeUnitsOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_SIZE_UNITS,
                                                                   unitsSwitchName,
                                                                   ("Units for image width/height\n"
                                                                    + defaultSpatialUnitsText));
    sizeUnitsOpt->addStringParameter(1,
                                     "units",
                                     ("Name of units for image width/height."
                                      + spatialUnitsList));
    
    /*
     * Option for image resolution
     */
    const QString resolutionSwitchName("-resolution");
    std::vector<ImageResolutionUnitsEnum::Enum> allResolutions;
    ImageResolutionUnitsEnum::getAllEnums(allResolutions);
    AString resolutionsList("  Valid resolution unit names are:\n");
    for (const auto resEnum : allResolutions) {
        resolutionsList += ("   " + ImageResolutionUnitsEnum::toName(resEnum) + "\n");
    }
    const AString defaultResText("      Default is "
                                 + AString::number(s_defaultResolutionNumberOfPixels, 'f', 0)
                                 + " "
                                 + ImageResolutionUnitsEnum::toName(ImageResolutionUnitsEnum::PIXELS_PER_INCH));
    OptionalParameter* resolutionOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_RESOLUTION,
                                                                    resolutionSwitchName,
                                                                    ("Image resolution (number pixels per size unit)\n"
                                                                     + defaultResText));
    resolutionOpt->addDoubleParameter(1, "Number of pixels", "number of pixels");
    resolutionOpt->addStringParameter(2,
                                      "Units Name",
                                      ("Name of resolution units."
                                       + resolutionsList));
    
    /*
     * Option to add margin to output image
     */
    OptionalParameter* marginOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_MARGIN,
                                                                "-margin",
                                                                "Add a margin to sides of the image using the window's background color.");
    marginOpt->addIntegerParameter(1,
                                   "size",
                                   "size of margin, in pixels, added to all sides of output image");
    
    /*
     * Option for not using colors in scene
     */
    ret->createOptionalParameter(static_cast<int32_t>(PARAM_KEY_OPTION_NO_SCENE_COLORS),
                                 "-no-scene-colors",
                                 "Do not use background and foreground colors in scene");
    
    /*
     * Option for setting map yoking
     */
    OptionalParameter* mapYokeOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_SET_MAP_YOKING,
                                                                 "-set-map-yoke",
                                                                 "Override selected map index for a map yoking group.");
    mapYokeOpt->addStringParameter(1,
                                   "map yoking roman numeral",
                                   "Roman numeral identifying the map yoking group (I, II, III, IV, V, VI, VII, VIII, IX, X)");
    mapYokeOpt->addIntegerParameter(2,
                                    "map undex",
                                    "Map index for yoking group.  Indices start at 1 (one)");
    
    /*
     * Option for connectome DB login
     */
    OptionalParameter* connDbOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_CONN_DB_LOGIN,
                                                                "-conn-db-login",
                                                                "Login for scenes with files in Connectome Database.  If "
                                                                "this option is not specified, the login and password stored "
                                                                "in the user's preferences is used.");
    connDbOpt->addStringParameter(1,
                                  "username",
                                  "Connectome DB Username");
    connDbOpt->addStringParameter(2,
                                  "password",
                                  "Connectome DB Password");
    
    /*
     * Option to show settings but not output image
     */
    const QString showCaptureSettingsSwitch("-show-capture-settings");
    ret->createOptionalParameter(PARAM_KEY_SHOW_CAPTURE_SETTINGS,
                                 showCaptureSettingsSwitch,
                                 "Print settings from Capture Dialog only, DO NOT create image file(s)");

    /*
     * Option to select renderer
     */
    std::vector<std::unique_ptr<OffScreenSceneRendererBase>> allOffScreenRenderers(getOffScreenRenderers());
    AString renderersListText;
    if ( ! allOffScreenRenderers.empty()) {
        renderersListText += ("\n   Available renderers are (first is default):\n");
        for (const auto& osr : allOffScreenRenderers) {
            renderersListText += (  "   " + osr->getSwitchName() + " - "
                                  + osr->getDescriptiveName() + "\n");
        }
    }
    const QString rendererSwitch("-renderer");
    OptionalParameter* rendererOpt = ret->createOptionalParameter(PARAM_KEY_OPTION_RENDERER,
                                                                  rendererSwitch,
                                                                  "Select renderer for drawing image");
    rendererOpt->addStringParameter(1, "Renderer",
                                    ("Name of renderer to use for drawing image"
                                     + renderersListText));
    
    ret->createOptionalParameter(PARAM_KEY_OPTION_PRINT_IMAGE_INFO,
                                 "-print-image-info",
                                 ("Print the size and other information about output images only and "
                                  "DO NOT create any output images"));

    /*
     * The help text printed when command run with no parameters
     */
    AString helpText(QString(70, '-') /* dashes to separate options from help text */
                     + "\n\n"
                     "Render content of browser windows displayed in a scene "
                     "into image file(s)."
                     "\n"
                     "\n"
                     "If none of the \"-size\" options are specified, the default is \"-size-window\" "
                     "(Output image is size of the window that was saved in the scene)."
                     "\n"
                     "\n"
                     "For the \"-size\" options that accept a width and/or height, the values default to number of pixels.  "
                     "To express the width and/or height in physical units (inches, centimeters, etc.), use the "
                     "\"" + unitsSwitchName + "\" option.  When physical units are used, the pixel width and height "
                     "are derived using the physical width/height and the image resolution "
                     "(see the \"" + resolutionSwitchName + "\" option)."
                     "\n"
                     "\n"
                     "Note that scenes created prior to version 1.2 (May 2016) do not contain information "
                     "about the size of the window.  Therefore, one must use the \"" + widthHeightSizeSwitch  + "\" "
                     "option.");
    
    helpText += ("\n"
                 "\n"
                 "Examples:"
                 "\n"
                 "\n"
                  + getExamplesOfUsage()
                 + "\n");
    
    if (allOffScreenRenderers.empty()) {
        helpText += ("\n"
                     "\n"
                     "NO OFF SCREEN RENDERERS AVAILABLE ON THIS SYSTEM.  COMMAND WILL FAIL !!!!"
                     "\n");
    }
    
#ifndef HAVE_OSMESA
    helpText += ("\n\nERROR: "
                 + OperationShowScene::getCommandNotAvailableMessage(OperationShowSceneTwo::getCommandSwitch())
                 + "\n");
#endif

    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
#ifndef HAVE_OSMESA
void
OperationShowSceneTwo::useParameters(OperationParameters* /*myParams*/,
                                  ProgressObject* /*myProgObj*/)
{
    throw OperationException(OperationShowScene::getCommandNotAvailableMessage(OperationShowSceneTwo::getCommandSwitch()));
}
#else // HAVE_OSMESA
void
OperationShowSceneTwo::useParameters(OperationParameters* myParams,
                                  ProgressObject* myProgObj)
{
    EventGraphicsOpenGLDeleteTextureName::setDisableFailureToDeleteWarningMessages(true);

    /*
     * Default to the first renderer
     */
    std::vector<std::unique_ptr<OffScreenSceneRendererBase>> allOffScreenRenderers(getOffScreenRenderers());
    if (allOffScreenRenderers.empty()) {
        throw OperationException("No offscreen renderers are available");
    }
    CaretAssertVectorIndex(allOffScreenRenderers, 0);
    OffScreenSceneRendererBase* offscreenRenderer(allOffScreenRenderers[0].get());

    /*
     * Required parameters
     */
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
    
    /*
     * One size parameter must be specified
     */
    OptionalParameter* imageCaptureDialogSizeOption = myParams->getOptionalParameter(PARAM_OPTION_KEY_SIZE_CAPTURE_DIALOG);
    OptionalParameter* imageWindowSizeOption = myParams->getOptionalParameter(PARAM_OPTION_KEY_SIZE_WINDOW);
    OptionalParameter* imageWidthAndHeightOption = myParams->getOptionalParameter(PARAM_OPTION_KEY_SIZE_IMAGE_WIDTH_AND_HEIGHT);
    OptionalParameter* imageWidthOption  = myParams->getOptionalParameter(PARAM_OPTION_KEY_SIZE_IMAGE_WIDTH);
    OptionalParameter* imageHeightOption = myParams->getOptionalParameter(PARAM_OPTION_KEY_SIZE_IMAGE_HEIGHT);
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

    /*
     * If no size mode, use window size
     */
    bool useWindowSizeDefaultFlag(false);
    if (imageSizeMode == ImageSizeMode::MODE_INVALID) {
        imageSizeMode = ImageSizeMode::MODE_WINDOW;
        useWindowSizeDefaultFlag = true;
    }
    
    /*
     * Width and height MUST be float since they may be spatial values (eg 2.5 inches)
     */
    float inputImageWidth(-1.0f);
    float inputImageHeight(-1.0f);
    switch (imageSizeMode) {
        case ImageSizeMode::MODE_INVALID:
            throw OperationException("Must specify one and only one of the size options");
            break;
        case ImageSizeMode::MODE_CAPTURE_DIALOG:
            CaretAssert(imageCaptureDialogSizeOption->m_present);
            break;
        case ImageSizeMode::MODE_HEIGHT_FROM_WIDTH:
            CaretAssert(imageWidthOption->m_present);
            inputImageWidth = imageWidthOption->getDouble(1);
            if (inputImageWidth <= 0.0f) {
                throw OperationException("Image width is invalid (zero or less)");
            }
            break;
        case ImageSizeMode::MODE_WIDTH_AND_HEIGHT:
            CaretAssert(imageWidthAndHeightOption->m_present);
            inputImageWidth = imageWidthAndHeightOption->getDouble(1);
            if (inputImageWidth <= 0.0f) {
                throw OperationException("Image width is invalid (zero or less)");
            }
            inputImageHeight = imageWidthAndHeightOption->getDouble(2);
            if (inputImageHeight <= 0.0f) {
                throw OperationException("Image height is invalid (zero or less)");
            }
            break;
        case ImageSizeMode::MODE_WIDTH_FROM_HEIGHT:
            CaretAssert(imageHeightOption->m_present);
            inputImageHeight = imageHeightOption->getDouble(1);
            if (inputImageHeight <= 0.0f) {
                throw OperationException("Image height is invalid (zero or less)");
            }
            break;
        case ImageSizeMode::MODE_WINDOW:
            if ( ! useWindowSizeDefaultFlag) {
                CaretAssert(imageWindowSizeOption->m_present);
            }
            break;
    }

    /*
     * Units for image width/height
     */
    ImageSpatialUnitsEnum::Enum imageWidthHeightUnits(s_defaultImageWidthHeightUnits);
    OptionalParameter* unitsSizeOption = myParams->getOptionalParameter(PARAM_KEY_OPTION_SIZE_UNITS);
    if (unitsSizeOption->m_present) {
        const AString unitsName(unitsSizeOption->getString(1).toUpper());
        bool validFlag(false);
        imageWidthHeightUnits = ImageSpatialUnitsEnum::fromName(unitsName,
                                                                &validFlag);
        if ( ! validFlag) {
            throw OperationException("\"" + unitsName + "\" is not a valid units for width/height");
        }
    }

    /*
     * Image resolution units and 'pixels per unit'
     */
    float imageResolutionNumberOfPixels(s_defaultResolutionNumberOfPixels);
    ImageResolutionUnitsEnum::Enum imageResolutionUnits = s_defaultResolutionUnits;
    OptionalParameter* resolutionOption = myParams->getOptionalParameter(PARAM_KEY_OPTION_RESOLUTION);
    if (resolutionOption->m_present) {
        imageResolutionNumberOfPixels = resolutionOption->getDouble(1);
        const AString resName(resolutionOption->getString(2).toUpper());
        bool validFlag(false);
        imageResolutionUnits = ImageResolutionUnitsEnum::fromName(resName,
                                                                  &validFlag);
        if ( ! validFlag) {
            throw OperationException("\"" + resName + "\" is not a valid units for resolution");
        }
    }
    if (imageResolutionNumberOfPixels <= 0.0) {
        throw OperationException("Resolution number of pixels="
                                 + AString::number(imageResolutionNumberOfPixels, 'f', 6)
                                 + " must be greater than zero");
    }

    /*
     * Option to ignore scene colors
     */
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
    
    /*
     * Option for map yoking
     */
    MapYokingGroupEnum::Enum mapYokingGroup = MapYokingGroupEnum::MAP_YOKING_GROUP_OFF;
    int32_t mapYokingMapIndex = -1;
    OptionalParameter* mapYokeOpt = myParams->getOptionalParameter(PARAM_KEY_OPTION_SET_MAP_YOKING);
    if (mapYokeOpt->m_present) {
        const AString romanNumeral = mapYokeOpt->getString(1).toUpper();
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
     * Option for connectome db login
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

    /*
     * ???
     */
    OptionalParameter* showSettingsOpt = myParams->getOptionalParameter(PARAM_KEY_SHOW_CAPTURE_SETTINGS);

    /*
     * Option for renderer
     */
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
    
    /*
     * Option to print info about output image but DO NOT create images
     */
    const bool printImageInfoOnlyFlag(myParams->getOptionalParameter(PARAM_KEY_OPTION_PRINT_IMAGE_INFO)->m_present);

    /*
     * Restore the scene file
     */
    AString sceneErrorMessage;
    loadSceneFileAndRestoreScene(sceneFileName,
                                 sceneNameOrNumber,
                                 doNotUseSceneColorsFlag,
                                 sceneErrorMessage);
    
    /*
     * Maybe update with map yoking option
     */
    applyMapYoking(mapYokingGroup,
                   mapYokingMapIndex);

    /*
     * Get all windows in the scene
     */
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
    
    /*
     * Get settings of capture dialog from scene
     */
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
    
    /*
     * Verify that an offscreen renderer is available
     */
    if ( ! offscreenRenderer) {
        throw OperationException("No offscreen renderer is selected");
    }
    if ( ! offscreenRenderer->isAvailable()) {
        throw OperationException(offscreenRenderer->getSwitchName() + " is not available on this system.");
    }

    /*
     * Restore each window
     */
    for (int32_t iWindow = 0; iWindow < numberOfWindows; iWindow++) {
        CaretAssertVectorIndex(allBrowserWindowContent, iWindow);
        auto bwc = allBrowserWindowContent[iWindow];
        
        std::unique_ptr<ImageCaptureDialogSettings> imageSettings;
        
        float imageWidth(-1.0);
        float imageHeight(-1.0);
        
        const float windowWidth = bwc->getSceneGraphicsWidth();
        const float windowHeight = bwc->getSceneGraphicsHeight();
        const float aspectRatio((windowWidth > 0.0)
                                ? (windowHeight / windowWidth)
                                : 0.0);
//        std::cout << "Window width=" << windowWidth
//        << " height=" << windowHeight
//        << " aspect=" << aspectRatio << std::endl;
        
        /*
         * Depending upon the "-size-" option, may need to determine
         * a width/height now that we have window width/hight from scene
         */
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
                imageWidth = inputImageWidth;
                imageHeight = aspectRatio * imageWidth;
                if (imageHeight <= 0.0) {
                    throw OperationException("Failed to compute height from width.  Width and/or height of window from scene may be invalid"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both width and height options must be specified");
                }
                break;
            case ImageSizeMode::MODE_WIDTH_AND_HEIGHT:
                imageWidth  = inputImageWidth;
                imageHeight = inputImageHeight;
                break;
            case ImageSizeMode::MODE_WIDTH_FROM_HEIGHT:
                if (aspectRatio <= 0.0) {
                    throw OperationException("Width and/or height of window from scene is invalid or missing (old scene)"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both width and height options must be specified");
                }
                imageHeight = inputImageHeight;
                imageWidth = imageHeight / aspectRatio;
                if (imageWidth <= 0.0) {
                    throw OperationException("Failed to compute width from height.  Width and/or height of window from scene may be invalid"
                                             "width=" + QString::number(imageWidth)
                                             + "height=" + QString::number(imageHeight)
                                             + ".  Both width and height options must be specified");
                }
                break;
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
        
        uint8_t backgroundColor[4];
        const CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        const auto foregroundBackgroundColors = prefs->getBackgroundAndForegroundColors();
        foregroundBackgroundColors->getColorBackgroundWindow(backgroundColor);

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
                                       imageHeight,
                                       imageWidthHeightUnits,
                                       imageResolutionUnits,
                                       imageResolutionNumberOfPixels);
        captureEvent.setMargin(marginValue);
        captureEvent.setBackgroundColor(backgroundColor);
        
        /*
         * Just print the information about the output image
         * but do not create it?
         */
        if (printImageInfoOnlyFlag) {
            std::cout << captureEvent.toImageInfoText(imageFileName) << std::endl;
            
            /*
             * Do not capture images, print only
             */
            continue;
        }
        
        /*
         * Create the image of the scene
         */
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
#endif // HAVE_OSMESA

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
                                          NULL,
                                          NULL,
                                          mapYokingMapIndex,
                                          MapYokingGroupEnum::MediaAllFramesStatus::ALL_FRAMES_OFF,
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
    CaretAssert(imageWidth > 0);
    CaretAssert(imageHeight > 0);
    
    /*
     * Allocate image buffer
     */
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
                const GraphicsFramesPerSecond* noGraphicsTiming(NULL);
                brainOpenGL->drawModels(windowIndex,
                                        UserInputModeEnum::Enum::VIEW,
                                        brain,
                                        inputs.m_offscreenRenderer->getDrawingContext(),
                                        constViewports,
                                        noGraphicsTiming);

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
        
        const GraphicsFramesPerSecond* noGraphicsTiming(NULL);
        brainOpenGL->drawModels(windowIndex,
                                UserInputModeEnum::Enum::VIEW,
                                brain,
                                inputs.m_offscreenRenderer->getDrawingContext(),
                                viewportContents,
                                noGraphicsTiming);

        writeImageFile(inputs.m_imageFileName,
                       inputs.m_outputImageIndex,
                       inputs.m_offscreenRenderer->getImageFile());
    }
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

/**
 * Get examples of using command
 */
AString
OperationShowSceneTwo::getExamplesOfUsage()
{
    const QString programName("wb_command");
    AString txt;
    
    txt += ("Generate an image of the second scene.  Width and height of image is width and height of window "
            "saved in the scene.  "
            "\n"
            "   " + programName
            + " -show-scene-two myscene.scene 2 image2.jpg"
            + "\n"
            + "\n"
            "Generate an image of the second scene with a margin around sides of the image. "
            "Width and height of image is width and height of window saved in the scene.  "
            "\n"
            "   " + programName
            + " -show-scene-two myscene.scene 2 image2.jpg  -margin 10"
            + "\n"
            + "\n"
            "Generate an image of the second scene that is 6 inches width with 300 pixels per inch.  The resulting "
            "width is 1800 pixels.  The resulting height of the image is a function of the width and the aspect "
            "ratio (height divided by width) of the window size saved in the scene."
            "\n"
            "   " + programName
            + " -show-scene-two myscene.scene 2 image21.jpg \\ \n"
            "   -size-width 6 -units INCHES -resolution 300 PIXELS_PER_INCH"
            + "\n"
            "\n"
            "Print information about the size of the output image for the second scene "
            "(no image file is created) using a width of 4.5 centimeters. "
            "\n"
            "   " + programName
            + " -show-scene-two myscene.scene 2 test.jpg \\ \n"
            "   -size-width 4.5 -units CENTIMETERS -print-image-info"
            + "\n"
            + "\n"
            );

            return txt;
}

