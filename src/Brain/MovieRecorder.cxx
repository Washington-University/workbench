
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __MOVIE_RECORDER_DECLARE__
#include "MovieRecorder.h"
#undef __MOVIE_RECORDER_DECLARE__

#include <QDir>
#include <QFile>
#include <QImage>
#include <QProcess>

#include <QtConcurrent/QtConcurrent>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "MovieRecorderVideoFormatTypeEnum.h"
#include "ImageFile.h"
#include "TextFile.h"

using namespace caret;


    
/**
 * \class caret::MovieRecorder 
 * \brief Records images and creates movie file from images
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param brain
 *     The brain
 */
MovieRecorder::MovieRecorder(Brain* brain)
: CaretObject(),
m_brain(brain)
{
    m_temporaryImagesDirectory = SystemUtilities::getTempDirectory();
    m_tempImageFileNamePrefix  = "movie";
    m_tempImageFileNameSuffix = ".png";
    removeTemporaryImages();
}

/**
 * Destructor.
 */
MovieRecorder::~MovieRecorder()
{
    removeTemporaryImages();
}

/**
 * Add an image to the movie, typically used during automatic mode recording
 *
 * @param image
 *     Image that is added
 */
void
MovieRecorder::addImageToMovie(const QImage* image)
{
    if (image == NULL) {
        CaretLogSevere("Attempting to add NULL image to movie");
        return;
    }
    
    if (getNumberOfFrames() <= 0) {
        std::cout << "Temporary Directory for movie images: "
        << std::endl
        << "   " << m_temporaryImagesDirectory << std::endl << std::flush;
    }
    
    CaretAssert(m_tempImageSequenceNumberOfDigits > 0);
    
    /*
     * First image starts at 1 and is padded with zeros on the left
     */
    const int32_t imageIndexInt = getNumberOfFrames() + 1;
    const QString imageIndex = QString::number(imageIndexInt).rightJustified(m_tempImageSequenceNumberOfDigits, '0');
    const QString imageFileName(m_temporaryImagesDirectory
                                + "/"
                                + m_tempImageFileNamePrefix
                                + imageIndex
                                + m_tempImageFileNameSuffix);
    
    switch (m_imageWriteMode) {
        case ImageWriteMode::IMMEDITATE:
            if (image->save(imageFileName)) {
                if (m_imageFileNames.empty()) {
                    m_firstImageWidth  = image->width();
                    m_firstImageHeight = image->height();
                }
                
                if ((image->width()     == m_firstImageWidth)
                    && (image->height() == m_firstImageHeight)) {
                    m_imageFileNames.push_back(imageFileName);
                }
                else {
                    CaretLogSevere("Attempting to create movie with images that are different sizes.  "
                                   "First image width=" + QString::number(m_firstImageWidth)
                                   + ", height=" + QString::number(m_firstImageHeight)
                                   + "  Image number=" + QString::number(imageIndexInt)
                                   + ", width=" + QString::number(image->width())
                                   + ", height=" + QString::number(image->height()));
                }
            }
            else {
                CaretLogSevere("Saving temporary image failed: "
                               + imageFileName);
            }
            break;
        case ImageWriteMode::PARALLEL:
        {
            ImageWriter* iw = new ImageWriter(image, imageFileName);
            m_imageWriters.push_back(iw);
            QFuture<bool> f = QtConcurrent::run(iw, &ImageWriter::writeImage);
            m_imageWriteResultFutures.push_back(f);
            m_imageFileNames.push_back(imageFileName);
        }
            break;
    }
}

/**
 * Add copies of an image to the movie for the given number of copies.
 * Typically used during manual mode recording.
 *
 * @param image
 *     Image that is added
 * @param numberOfCopies
 *     Number of copies for the image.
 */
void
MovieRecorder::addImageToMovieWithCopies(const QImage* image,
                                         const int32_t numberOfCopies)
{
    for (int32_t i = 0; i < numberOfCopies; i++) {
        addImageToMovie(image);
    }
}

/**
 * @return True if all images were written succussfully
 * if parallel image file writing is enabled.  Returns
 * true if image writing mode is immediate.
 */
bool
MovieRecorder::waitForImagesToFinishWriting()
{
    bool allValid(true);
    
    switch (m_imageWriteMode) {
        case ImageWriteMode::IMMEDITATE:
            break;
        case ImageWriteMode::PARALLEL:
        {
            for (auto f : m_imageWriteResultFutures) {
                f.waitForFinished();
            }
        }
            break;
    }
    
    return allValid;
}

/**
 * Remove all images and starting a new movie
 */
void
MovieRecorder::removeTemporaryImages()
{
    waitForImagesToFinishWriting();
    m_imageWriteResultFutures.clear();
    
    for (auto iw : m_imageWriters) {
        delete iw;
    }
    m_imageWriters.clear();
    
    const QString nameFilter(m_tempImageFileNamePrefix
                             + "*"
                             + m_tempImageFileNameSuffix);
    QStringList allNameFilters;
    allNameFilters.append(nameFilter);
    QDir dir(m_temporaryImagesDirectory);
    QFileInfoList fileInfoList = dir.entryInfoList(allNameFilters,
                                                   QDir::Files,
                                                   QDir::Name);
    QListIterator<QFileInfo> iter(fileInfoList);
    while (iter.hasNext()) {
        QFile file(iter.next().absoluteFilePath());
        if (file.exists()) {
            file.remove();
        }
    }
    
    m_imageFileNames.clear();
    m_firstImageWidth  = -1;
    m_firstImageHeight = -1;
}


/**
 * @return The recording mode
 */
MovieRecorderModeEnum::Enum
MovieRecorder::getRecordingMode() const
{
    return m_recordingMode;
}

/**
 * Set the recording mode
 *
 * @param recordingMode
 *     New recording mode
 */
void
MovieRecorder::setRecordingMode(const MovieRecorderModeEnum::Enum recordingMode)
{
    m_recordingMode = recordingMode;
}

/**
 * @return Index of window that is recorded
 */
int32_t
MovieRecorder::getRecordingWindowIndex() const
{
    return m_windowIndex;
}

/**
 * Set index of window that is recorded
 *
 * @param windowIndex
 *     Index of window
 */
void
MovieRecorder::setRecordingWindowIndex(const int32_t windowIndex)
{
    m_windowIndex = windowIndex;
}

/**
 * @return Video resolution type
 */
MovieRecorderVideoResolutionTypeEnum::Enum
MovieRecorder::getVideoResolutionType() const
{
    return m_resolutionType;
}

/**
 * Set the video resolution type
 *
 * @param resolutionType
 *     New resolution type
 */
void
MovieRecorder::setVideoResolutionType(const MovieRecorderVideoResolutionTypeEnum::Enum resolutionType)
{
    m_resolutionType = resolutionType;
}

/**
 * Get the video width and height
 *
 * @param widthOut
 *     Output width
 * @param heightOut
 *     Output height
 */
void
MovieRecorder::getVideoWidthAndHeight(int32_t& widthOut,
                                      int32_t& heightOut) const
{
    widthOut  = 100;
    heightOut = 100;
    
    const MovieRecorderVideoResolutionTypeEnum::Enum dimType = getVideoResolutionType();
    if (dimType == MovieRecorderVideoResolutionTypeEnum::CUSTOM) {
        getCustomWidthAndHeight(widthOut,
                                heightOut);
    }
    else {
        MovieRecorderVideoResolutionTypeEnum::getWidthAndHeight(dimType,
                                                                widthOut,
                                                                heightOut);
    }
}

/**
 * Get the custom width and height
 *
 * @param widthOut
 *     Output width
 * @param heightOut
 *     Output height
 */
void
MovieRecorder::getCustomWidthAndHeight(int32_t& widthOut,
                                       int32_t& heightOut) const
{
    widthOut  = m_customWidth;
    heightOut = m_customHeight;
}

/**
 * Set the custom width and height
 *
 * @param width
 *     New width
 * @param height
 *     New height
 */
void
MovieRecorder::setCustomWidthAndHeight(const int32_t width,
                                       const int32_t height)
{
    m_customWidth  = width;
    m_customHeight = height;
}

/**
 * @return The capture region type
 */
MovieRecorderCaptureRegionTypeEnum::Enum
MovieRecorder::getCaptureRegionType() const
{
    return m_captureRegionType;
}

/**
 * Set the capture region type
 *
 * @param captureRegionType
 *     New capture region type
 */
void
MovieRecorder::setCaptureRegionType(const MovieRecorderCaptureRegionTypeEnum::Enum captureRegionType)
{
    m_captureRegionType = captureRegionType;
}

/**
 * @return Name of movie file
 */
AString
MovieRecorder::getMovieFileName() const
{
    return m_movieFileName;
}

/**
 * Set name of movie file
 *
 * @param filename
 *     New name for movie file
 */
void
MovieRecorder::setMovieFileName(const AString& filename)
{
    m_movieFileName = filename;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
MovieRecorder::toString() const
{
    return "MovieRecorder";
}

/**
 * @return The frame rate (number of frames per second)
 */
float
MovieRecorder::getFramesRate() const
{
    return m_frameRate;
}

/**
 * Set the frame rate (number of frames per second)
 *
 * @param frameRate
 *     New frame rate
 */
void
MovieRecorder::setFramesRate(const float frameRate)
{
    m_frameRate = frameRate;
}

/**
 * @return True if temporary images should be removed
 * after creation of a movie
 */
bool
MovieRecorder::isRemoveTemporaryImagesAfterMovieCreation() const
{
    return m_removeTemporaryImagesAfterMovieCreationFlag;
}

/**
 * Set temporary images should be removed after creation of a movie
 *
 * @param status
 *     New status
 */
void
MovieRecorder::setRemoveTemporaryImagesAfterMovieCreation(const bool status)
{
    m_removeTemporaryImagesAfterMovieCreationFlag = status;
}

/**
 * @return Number of frames (images) that have been recorded
 */
int32_t
MovieRecorder::getNumberOfFrames() const
{
    return m_imageFileNames.size();
}

/**
 * Create the movie using images captured thus far
 *
 * @param filename
 *     File name for movie.
 * @param errorMessageOut
 *     Contains information if movie creation failed
 * @return
 *     True if successful, else false
 */
bool
MovieRecorder::createMovie(const AString& filename,
                           AString& errorMessageOut)
{
    errorMessageOut.clear();
 
    if ( ! waitForImagesToFinishWriting()) {
        errorMessageOut = "There was a problem writing the image files.";
        return false;
    }
    
    m_movieFileName = filename;
    
    if (m_movieFileName.isEmpty()) {
        errorMessageOut = "Movie file name is invalid or empty";
        return false;
    }
    
    FileInformation fileInfo(m_movieFileName);
    if (fileInfo.exists()) {
        errorMessageOut = ("Movie file exists, delete or change name: "
                           + m_movieFileName);
        return false;
    }
    
    if (m_imageFileNames.empty()) {
        errorMessageOut.appendWithNewLine("No images have been recorded for the movie.");
    }
    if (m_movieFileName.isEmpty()) {
        errorMessageOut.appendWithNewLine("Movie file name is empty.");
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    const AString sequenceDigitsPattern("%0"
                                        + AString::number(m_tempImageSequenceNumberOfDigits)
                                        + "d");
    
    const AString imagesRegularExpressionMatch(m_temporaryImagesDirectory
                                               + "/"
                                               + m_tempImageFileNamePrefix
                                               + sequenceDigitsPattern
                                               + m_tempImageFileNameSuffix);
    QString workbenchHomeDir = SystemUtilities::getWorkbenchHome();

    /* Qt after 5.? const QString ffmpegDir = qEnvironmentVariable("WORKBENCH_FFMPEG_DIR"); */
    const QString ffmpegDir = qgetenv("WORKBENCH_FFMPEG_DIR").constData();
    if ( ! ffmpegDir.isEmpty()) {
        workbenchHomeDir = ffmpegDir;
    }

    const bool qProcessPipeFlag(false);
    const QString textFileName(m_temporaryImagesDirectory
                               + "/"
                               + "images.txt");
    
    const QString programName(workbenchHomeDir
                              + "/ffmpeg");
    FileInformation ffmpegInfo(programName);
    if ( ! ffmpegInfo.exists()) {
        errorMessageOut = ("Invalid path for ffmpeg: "
                           + programName
                           + "\n  WORKBENCH_FFMPEG_DIR can be set to directory containing ffmpeg.");
        return false;
    }
    
    QStringList arguments;
    arguments.append("-threads");
    arguments.append("4");
    arguments.append("-framerate");
    arguments.append(AString::number(m_frameRate));
    if (qProcessPipeFlag) {
        /* list of images in file */
        arguments.append("-f");
        arguments.append("concat");
//        arguments.append("-safe");
//        arguments.append("0");
        arguments.append("-i");
        arguments.append(textFileName);
    }
    else {
        arguments.append("-i");
        arguments.append(imagesRegularExpressionMatch);
    }
    arguments.append("-q:v");
    arguments.append("1");
    arguments.append(m_movieFileName);
    
    bool successFlag(false);
    if (qProcessPipeFlag) {
        successFlag = createMovieWithQProcessPipe(programName,
                                                  arguments,
                                                  textFileName,
                                                  errorMessageOut);
    }
    else {
        const bool useQProcessFlag(true);
        if (useQProcessFlag) {
            successFlag = createMovieWithQProcess(programName,
                                                  arguments,
                                                  errorMessageOut);
        }
        else {
            successFlag = createMovieWithSystemCommand(programName,
                                                       arguments,
                                                       errorMessageOut);
        }
    }
    
    if (successFlag) {
        if (m_removeTemporaryImagesAfterMovieCreationFlag) {
            removeTemporaryImages();
        }
    }
    
    return successFlag;
}

/**
 * Create the movie by using Qt's QProcess and using a
 * pipe to send the images to ffmpeg
 *
 * @param programName
 *     Name of program
 * @param arguments
 *     Arguments to program
 * @param errorMessageOut
 *     Output containing error message
 * @return
 *     True if movie was created or false if there was an error
 */
bool
MovieRecorder::createMovieWithQProcessPipe(const QString& programName,
                                           const QStringList& arguments,
                                           const QString& textFileName,
                                           QString& errorMessageOut)
{
    /*
     * https://trac.ffmpeg.org/wiki/Concatenate
     * https://trac.ffmpeg.org/wiki/Slideshow
     */
    bool successFlag(false);

    TextFile textFile;
    try {
        for (const auto name : m_imageFileNames) {
            textFile.addLine("file "
                             + name);
        }
        textFile.writeFile(textFileName);
    }
    catch (const DataFileException& dfe) {
        errorMessageOut = ("Error creating text file containing image names: "
                           + dfe.whatString());
        return false;
    }

    QProcess process;
    process.start(programName,
                  arguments);
    process.closeWriteChannel();
    
    const int noTimeout(-1);
    const bool finishedFlag = process.waitForFinished(noTimeout);
    if (finishedFlag) {
        if (process.exitStatus() == QProcess::NormalExit) {
            const int resultCode = process.exitCode();
            if (resultCode == 0) {
                successFlag = true;
            }
            else {
                QByteArray results = process.readAllStandardError();
                errorMessageOut = QString(results);
            }
        }
        else if (process.exitStatus() == QProcess::CrashExit) {
            errorMessageOut = "Running ffmpeg crashed";
        }
    }
    else {
        errorMessageOut = "Creating movie was terminated for unknown reason";
    }
    
    return successFlag;
}

/**
 * Create the movie by using Qt's QProcess
 *
 * @param programName
 *     Name of program
 * @param arguments
 *     Arguments to program
 * @param errorMessageOut
 *     Output containing error message
 * @return
 *     True if movie was created or false if there was an error
 */
bool
MovieRecorder::createMovieWithQProcess(const QString& programName,
                                       const QStringList& arguments,
                                       QString& errorMessageOut)
{
    bool successFlag(false);
    
    QProcess process;
    process.start(programName,
                  arguments);
    process.closeWriteChannel();
    
    const int noTimeout(-1);
    const bool finishedFlag = process.waitForFinished(noTimeout);
    if (finishedFlag) {
        if (process.exitStatus() == QProcess::NormalExit) {
            const int resultCode = process.exitCode();
            if (resultCode == 0) {
                successFlag = true;
            }
            else {
                QByteArray results = process.readAllStandardError();
                errorMessageOut = QString(results);
            }
        }
        else if (process.exitStatus() == QProcess::CrashExit) {
            errorMessageOut = "Running ffmpeg crashed";
        }
    }
    else {
        errorMessageOut = "Creating movie was terminated for unknown reason";
    }
    
    return successFlag;
}

/**
 * Create the movie by using the system command
 *
 * @param programName
 *     Name of program
 * @param arguments
 *     Arguments to program
 * @param errorMessageOut
 *     Output containing error message
 * @return
 *     True if movie was created or false if there was an error
 */
bool
MovieRecorder::createMovieWithSystemCommand(const QString& programName,
                                            const QStringList& arguments,
                                            QString& errorMessageOut)
{
    const AString commandString(programName
                                + " "
                                + arguments.join(" "));
    const int result = system(commandString.toLatin1().constData());
    
    bool successFlag(false);
    if (result == 0) {
        successFlag = true;
    }
    else {
        errorMessageOut = ("Running ffmpeg failed with code="
                           + AString::number(result));
    }
    return successFlag;
}


/**
 * Constructor for image writer
 *
 * @param image
 *     The image file
 * @param filename
 *     Name of file
 */
MovieRecorder::ImageWriter::ImageWriter(const QImage* image,
                                        const QString& filename)
: m_image(new QImage(*image)),
m_filename(filename)
{
    CaretAssert(m_image);
    
}

/**
 * Destructor
 */
MovieRecorder::ImageWriter::~ImageWriter()
{
}

/**
 * Write the image
 *
 * @return True if written, false if error.
 */
bool
MovieRecorder::ImageWriter::writeImage()
{
    CaretAssert(m_image);
    return m_image->save(m_filename);
}


