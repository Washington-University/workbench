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
#include "MovieRecorderModeEnum.h"
#include "MovieRecorderVideoDimensionsTypeEnum.h"
#include "MovieRecorderVideoFormatTypeEnum.h"

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
        
        MovieRecorderVideoDimensionsTypeEnum::Enum getVideoDimensionsType() const;
        
        void setVideoDimensionsType(const MovieRecorderVideoDimensionsTypeEnum::Enum dimensionsType);

        void getVideoDimensions(int32_t& widthOut,
                                int32_t& heightOut) const;
        
        void getCustomDimensions(int32_t& widthOut,
                                 int32_t& heightOut) const;
        
        void setCustomDimensions(const int32_t width,
                                 const int32_t height);
        
        MovieRecorderVideoFormatTypeEnum::Enum getVideoFormatType() const;
        
        void setVideoFormatType(const MovieRecorderVideoFormatTypeEnum::Enum formatType);

        AString getMovieFileName() const;
        
        void setMovieFileName(const AString& filename);
        
        int32_t getNumberOfFrames() const;
        
        float getFramesRate() const;
        
        void setFramesRate(const float frameRate);
        
        float getManualRecordingDurationSeconds() const;
        
        void setManualRecordingDurationSeconds(const float seconds);
        
        bool isManualRecordingOfImageRequested() const;
        
        void setManualRecordingOfImageRequested(const bool requestFlag);
        
        void reset();
        
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
        
        MovieRecorderVideoDimensionsTypeEnum::Enum m_dimensionsType = MovieRecorderVideoDimensionsTypeEnum::SD_640_480;
        
        MovieRecorderVideoFormatTypeEnum::Enum m_formatType = MovieRecorderVideoFormatTypeEnum::MPEG_4;
        
        int32_t m_windowIndex = 0;
        
        int32_t m_customWidth = 640;
        
        int32_t m_customHeight = 480;
        
        std::vector<AString> m_imageFileNames;
        
        mutable AString m_movieFileName;
        
        std::vector<AString> m_imageFrameFileNames;
        
        float m_frameRate = 20.0f;
        
        float m_manualRecordingDurationSeconds = 5.0f;
        
        bool m_manualRecordingOfImageRequested = false;
        
        AString m_temporaryImagesDirectory;
        
        AString m_tempImageFileNamePrefix;
        
        AString m_tempImageFileNameSuffix;
        
        const int32_t m_tempImageSequenceNumberOfDigits = 6;

    };
    
#ifdef __MOVIE_RECORDER_DECLARE__
#endif // __MOVIE_RECORDER_DECLARE__

} // namespace
#endif  //__MOVIE_RECORDER_H__
