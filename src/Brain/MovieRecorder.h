#ifndef __MOVIE_RECORDER_H__
#define __MOVIE_RECORDER_H__

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



#include <memory>

#include "CaretObject.h"
#include "MovieRecorderCaptureRegionTypeEnum.h"
#include "MovieRecorderModeEnum.h"
#include "MovieRecorderVideoResolutionTypeEnum.h"

class QImage;
class QStringList;

namespace caret {

    class MovieRecorder : public CaretObject {
        
    public:

        MovieRecorder();
        
        virtual ~MovieRecorder();
        
        MovieRecorder(const MovieRecorder&) = delete;

        MovieRecorder& operator=(const MovieRecorder&) = delete;

        MovieRecorderModeEnum::Enum getRecordingMode() const;
        
        void addImageToMovie(const QImage* image);
        
        void addImageToMovieWithManualDuration(const QImage* image);
        
        void setRecordingMode(const MovieRecorderModeEnum::Enum recordingMode);
        
        int32_t getRecordingWindowIndex() const;
        
        void setRecordingWindowIndex(const int32_t windowIndex);
        
        MovieRecorderVideoResolutionTypeEnum::Enum getVideoResolutionType() const;
        
        void setVideoResolutionType(const MovieRecorderVideoResolutionTypeEnum::Enum resolutionType);

        void getVideoWidthAndHeight(int32_t& widthOut,
                                    int32_t& heightOut) const;
        
        void getCustomWidthAndHeight(int32_t& widthOut,
                                     int32_t& heightOut) const;
        
        void setCustomWidthAndHeight(const int32_t width,
                                     const int32_t height);
        
        MovieRecorderCaptureRegionTypeEnum::Enum getCaptureRegionType() const;
        
        void setCaptureRegionType(const MovieRecorderCaptureRegionTypeEnum::Enum captureRegionType);
        
        AString getMovieFileName() const;
        
        void setMovieFileName(const AString& filename);
        
        void initializeMovieFileName(const AString& path);
        
        int32_t getNumberOfFrames() const;
        
        float getFramesRate() const;
        
        void setFramesRate(const float frameRate);
        
        float getManualRecordingDurationSeconds() const;
        
        void setManualRecordingDurationSeconds(const float seconds);
        
        bool isManualRecordingOfImageRequested() const;
        
        void setManualRecordingOfImageRequested(const bool requestFlag);
        
        bool isRemoveTemporaryImagesAfterMovieCreation() const;
        
        void setRemoveTemporaryImagesAfterMovieCreation(const bool status);
        
        void removeTemporaryImages();
        
        bool createMovie(AString& errorMessageOut);
        
        bool createMovieWithSystemCommand(const QString& programName,
                                          const QStringList& arguments,
                                          QString& errorMessageOut);
        
        bool createMovieWithQProcess(const QString& programName,
                                     const QStringList& arguments,
                                     QString& errorMessageOut);

        bool createMovieWithQProcessPipe(const QString& programName,
                                         const QStringList& arguments,
                                         const QString& textFileName,
                                         QString& errorMessageOut);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        // ADD_NEW_MEMBERS_HERE

        MovieRecorderModeEnum::Enum m_recordingMode = MovieRecorderModeEnum::MANUAL;
        
        MovieRecorderVideoResolutionTypeEnum::Enum m_resolutionType = MovieRecorderVideoResolutionTypeEnum::SD_640_480;
        
        MovieRecorderCaptureRegionTypeEnum::Enum m_captureRegionType = MovieRecorderCaptureRegionTypeEnum::GRAPHICS;
        
        int32_t m_windowIndex = 0;
        
        int32_t m_customWidth = 640;
        
        int32_t m_customHeight = 480;
        
        std::vector<AString> m_imageFileNames;
        
        mutable AString m_movieFileName;
        
        std::vector<AString> m_imageFrameFileNames;
        
        float m_frameRate = 30.0f;
        
        float m_manualRecordingDurationSeconds = 5.0f;
        
        bool m_manualRecordingOfImageRequested = false;
        
        AString m_temporaryImagesDirectory;
        
        AString m_tempImageFileNamePrefix;
        
        AString m_tempImageFileNameSuffix;
        
        bool m_removeTemporaryImagesAfterMovieCreationFlag = true;
        
        const int32_t m_tempImageSequenceNumberOfDigits = 6;

        int32_t m_firstImageWidth  = -1;
        int32_t m_firstImageHeight = -1;
    };
    
#ifdef __MOVIE_RECORDER_DECLARE__
#endif // __MOVIE_RECORDER_DECLARE__

} // namespace
#endif  //__MOVIE_RECORDER_H__
