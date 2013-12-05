
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <cstdio>
#include <fstream>

#ifdef HAVE_OSMESA
#include <GL/osmesa.h>
#endif // HAVE_OSMESA

#include <QImage>
#include <QColor>

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLViewportContent.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "ImageFile.h"
#include "OperationShowScene.h"
#include "OperationException.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneFile.h"
#include "SessionManager.h"
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
    
    AString helpText("Render content of browser windows displayed in a scene "
                     "into image file(s).  The image file name should be "
                     "similar to \"capture.png\".  If there is only one image "
                     "to render, the image name will not change.  If there is "
                     "more than one image to render, an index will be inserted "
                     "into the image name: \"capture_01.png\", \"capture_02.png\" "
                     "etc.\n"
                     "\n"
                     "The image format is determined by the image file extension.\n"
                     "Image formats available on this sytem are:\n");
    
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
    helpText += ("Note: Available image formats may vary by operating system.\n");
    
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
    AString sceneFileName = FileInformation(myParams->getString(1)).getFilePath();
    AString sceneNameOrNumber = myParams->getString(2);
    AString imageFileName = FileInformation(myParams->getString(3)).getFilePath();
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
    const int viewport[4] = { 0, 0, imageWidth, imageHeight };

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
    
    /*
     * Performs OpenGL Rendering
     */
    BrainOpenGLFixedPipeline brainOpenGL(NULL);
    brainOpenGL.initializeOpenGL();
    
    /*
     * Restore windows
     */
    const SceneClassArray* browserWindowArray = guiManagerClass->getClassArray("m_brainBrowserWindows");
    if (browserWindowArray != NULL) {
        const int32_t numBrowserClasses = browserWindowArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numBrowserClasses; i++) {
            const SceneClass* browserClass = browserWindowArray->getClassAtIndex(i);
            
            /*
             * Restore toolbar
             */
            const SceneClass* toolbarClass = browserClass->getClass("m_toolbar");
            if (toolbarClass != NULL) {
                /*
                 * Index of selected browser tab (NOT the tabBar)
                 */
                const int32_t selectedTabIndex = toolbarClass->getIntegerValue("selectedTabIndex", -1);
                std::cout << "Restoring tab: " << selectedTabIndex << " in window " << i << std::endl;

                EventBrowserTabGet getTabContent(selectedTabIndex);
                EventManager::get()->sendEvent(getTabContent.getPointer());
                BrowserTabContent* tabContent = getTabContent.getBrowserTab();
                if (tabContent == NULL) {
                    throw OperationException("Failed to obtain tab number "
                                             + AString::number(selectedTabIndex + 1)
                                             + " for window "
                                             + AString::number(i + 1));
                }
                
                BrainOpenGLViewportContent content(viewport,
                                                   viewport,
                                                   false,
                                                   brain,
                                                   tabContent);
                std::vector<BrainOpenGLViewportContent*> viewportContents;
                viewportContents.push_back(&content);
                
                brainOpenGL.drawModels(viewportContents);
                
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
    
    /*
     * Free image memory and Mesa context
     */
    delete[] imageBuffer;
    OSMesaDestroyContext(mesaContext);
    

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

