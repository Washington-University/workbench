
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
                     "similar to \"capture.ppm\".  If there is only one image "
                     "to render, the image name will not change.  If there is "
                     "more than one image to render, an index will be inserted "
                     "into the image name: \"capture_01.ppm\", \"capture_02.ppm\" "
                     "etc.\n"
                     "\n"
                     "Images are written in PPM format.");
    
    ret->setHelpText(helpText);
    
    return ret;
}

/**
 * Use Parameters and perform operation
 */
void
OperationShowScene::useParameters(OperationParameters* myParams,
                                          ProgressObject* myProgObj)
{
#ifndef HAVE_OSMESA
    throw OperationException("Show scene command not available due to this software version "
                             "not being built with the Mesa OffScreen Library");
#else // HAVE_OSMESA
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
                                                   brain,
                                                   tabContent);
                std::vector<BrainOpenGLViewportContent*> viewportContents;
                viewportContents.push_back(&content);
                
                brainOpenGL.drawModels(viewportContents);
                
                const int32_t outputImageIndex = ((numBrowserClasses > 1)
                                             ? i
                                             : -1);
                
                if (imageFileName.endsWith(".ppm")) {
                    writeImagePPM(imageFileName,
                                  outputImageIndex,
                                  imageBuffer,
                                  imageWidth,
                                  imageHeight);
                }
                else if (imageFileName.endsWith(".png")) {
                    writeImagePNG(imageFileName,
                                  outputImageIndex,
                                  imageBuffer,
                                  imageWidth,
                                  imageHeight);
                }
                else {
                    throw OperationException("Invalid image file extension");
                }
            }
        }
    }
    
    /*
     * Free image memory and Mesa context
     */
    delete[] imageBuffer;
    OSMesaDestroyContext(mesaContext);
    

#endif // HAVE_OSMESA
}

/**
 * Write the image data to a PPM Image File.
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
OperationShowScene::writeImagePPM(const AString& imageFileName,
                                  const int32_t imageIndex,
                                  const unsigned char* imageContent,
                                  const int32_t imageWidth,
                                  const int32_t imageHeight)
{
    /*
     * text width and height of the image
     */
    QString widthString(QString::number(imageWidth));
    QString heightString(QString::number(imageHeight));
    
    /*
     * Allocate image buffer including space for header
     */
    const int32_t imageBufferSize = (imageWidth * imageHeight * 3) + 1000;
    unsigned char* imageBuffer = new unsigned char[imageBufferSize];
    
    /*
     * index to buffer
     */
    int bufferIndex = 0;
    
    /*
     * Add "P6" to indicate binary PPM
     */
    imageBuffer[bufferIndex++] = 'P';
    imageBuffer[bufferIndex++] = '6';
    imageBuffer[bufferIndex++] = '\n';
    
    /*
     * Add image width and height
     */
    for (int i = 0; i < widthString.length(); i++) {
        imageBuffer[bufferIndex++] = widthString[i].toLatin1();
    }
    imageBuffer[bufferIndex++] = ' ';
    for (int i = 0; i < heightString.length(); i++) {
        imageBuffer[bufferIndex++] = heightString[i].toLatin1();
    }
    imageBuffer[bufferIndex++] = '\n';
    
    /*
     * Add in max value of pixel
     */
    imageBuffer[bufferIndex++] = '2';
    imageBuffer[bufferIndex++] = '5';
    imageBuffer[bufferIndex++] = '5';
    imageBuffer[bufferIndex++] = '\n';
    
    /*
     * Add captured image content to the image buffer.
     * Note that image format has origin at top and
     * OpenGL has origin at bottom.
     */
    for (int32_t row = (imageHeight - 1); row >= 0; row--) {
        for (int32_t col = 0; col < imageWidth; col++) {
            const int32_t contentOffset = (((row * imageWidth) * 4)
                                           + (col * 4));
            imageBuffer[bufferIndex++] = imageContent[contentOffset];
            imageBuffer[bufferIndex++] = imageContent[contentOffset+1];
            imageBuffer[bufferIndex++] = imageContent[contentOffset+2];
        }
    }
    
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
                           + ".ppm");
        }
    }
    
    /*
     * write the image data
     */
    std::ofstream ppmImageFile(outputName.toAscii().constData());
    if (!ppmImageFile) {
        throw OperationException("Error writing image " + outputName);
    }
    ppmImageFile.write((const char*)imageBuffer, bufferIndex);
    ppmImageFile.close();
}

/**
 * Write the image data to a PNG Image File.
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
OperationShowScene::writeImagePNG(const AString& imageFileName,
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
    
    QImage image(imageWidth,
                 imageHeight,
                 QImage::Format_RGB32);
    
    const bool flip = true;
    for (int y = 0; y < imageHeight; y++) {
        int scanLineIndex = y;
        if (flip) {
            scanLineIndex = imageHeight -y -1;
        }
        QRgb* rgbScanLine = (QRgb*)image.scanLine(scanLineIndex);
        
        for (int x = 0; x < imageWidth; x++) {
            const int32_t contentOffset = (((y * imageWidth) * 4)
                                            + (x * 4));
            const int red   = imageContent[contentOffset];
            const int green = imageContent[contentOffset+1];
            const int blue  = imageContent[contentOffset+2];
            const int alpha = 255;
            QColor color(red,
                   green,
                   blue,
                   alpha);
            
            QRgb* pixel = &rgbScanLine[x];
            *pixel = color.rgba();
        }
    }
    
    try {
        ImageFile imageFile(image);
        imageFile.writeFile(outputName);
    }
    catch (const DataFileException& dfe) {
        throw OperationException(dfe);
    }
    
//    OperationException operationException;
//    bool writePngFailed = false;
//    
//    FILE* file = NULL;
//    png_structp pngStruct = NULL;
//    png_infop pngInfo = NULL;
//    png_byte** rowPointers = NULL;
//    
//    try {
//        file = fopen(qPrintable(outputName),
//                     "w");
//        
//        if (file == NULL) {
//            throw OperationException("Unable to open for writing: "
//                                     + outputName);
//        }
//        
//        pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING,
//                                                        NULL,
//                                                        NULL,
//                                                        NULL);
//        if (pngStruct == NULL) {
//            fclose(file);
//            throw OperationException("Creating PNG structure failed.");
//        }
//        
//        pngInfo = png_create_info_struct(pngStruct);
//        if (pngInfo == NULL) {
//            fclose(file);
//            throw OperationException("Creating PNG structure failed.");
//        }
//        
//        png_set_IHDR(pngStruct,
//                     pngInfo,
//                     imageWidth,
//                     imageHeight,
//                     8,  // bit depth
//                     PNG_COLOR_TYPE_RGB,
//                     PNG_INTERLACE_NONE,
//                     PNG_COMPRESSION_TYPE_DEFAULT,
//                     PNG_FILTER_TYPE_DEFAULT);
//        
//        rowPointers = (png_byte**)png_malloc(pngStruct,
//                                 imageHeight * sizeof(png_byte *));
//        for (int y = 0; y < imageHeight; y++) {
//            rowPointers[y] = NULL;
//        }
//        
//        
//        for (int y = 0; y < imageHeight; y++) {
//            png_byte* row = (png_byte*)png_malloc(pngStruct,
//                                       (sizeof(uint8_t) * imageWidth * 3));
//            rowPointers[y] = row;
//            
//            for (int x = 0; x < imageWidth; x++) {
//                const int32_t contentOffset = (((y * imageWidth) * 4)
//                                               + (x * 4));
//                *row++ = imageContent[contentOffset];
//                *row++ = imageContent[contentOffset+1];
//                *row++ = imageContent[contentOffset+2];
//            }
//        }
//        
//        png_init_io(pngStruct,
//                    file);
//        png_set_rows(pngStruct,
//                     pngInfo,
//                     rowPointers);
//        png_write_png(pngStruct,
//                      pngInfo,
//                      PNG_TRANSFORM_IDENTITY,
//                      NULL);
//        
//    }
//    catch (const OperationException& oe) {
//        writePngFailed = true;
//        operationException = oe;
//    }
//    
//    if (rowPointers != NULL) {
//        for (int y = 0; y < imageHeight; y++) {
//            png_free(pngStruct,
//                     rowPointers[y]);
//        }
//        png_free(pngStruct,
//                 rowPointers);
//    }
//    
//    png_destroy_write_struct(&pngStruct,
//                             &pngInfo);
//    
//    if (file != NULL) {
//        fclose(file);
//    }
//    
//    if (writePngFailed) {
//        throw operationException;
//    }
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

