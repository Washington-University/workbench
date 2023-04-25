#ifndef __MEDIA_FILE_CHANNEL_INFO_H__
#define __MEDIA_FILE_CHANNEL_INFO_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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



namespace caret {
    class DataFileContentInformation;

    class MediaFileChannelInfo : public CaretObject {
        
    public:
        MediaFileChannelInfo();
        
        MediaFileChannelInfo(const bool allChannelsSelectionSupportedFlag,
                             const bool singleChannelSelectionSupportedFlag);
        
        virtual ~MediaFileChannelInfo();
        
        MediaFileChannelInfo(const MediaFileChannelInfo& obj);

        MediaFileChannelInfo& operator=(const MediaFileChannelInfo& obj);
        
        void addChannel(const AString& channelName);
        
        bool isChannelsSupported() const;
        
        bool isAllChannelsSelectionSupported() const;
        
        bool isSingleChannelSelectionSupported() const;
        
        int32_t getNumberOfChannels() const;
        
        AString getChannelName(const int32_t channelIndex) const;
        
        void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        // ADD_NEW_METHODS_HERE
        
    private:
        class ChannelInfo {
        public:
            ChannelInfo(const AString& name)
            : m_name(name)
            { }
            
            AString m_name;
        };
        
        void copyHelperMediaFileChannelInfo(const MediaFileChannelInfo& obj);

        bool m_channelsSupportedFlag = false;
        
        bool m_allChannelsSelectionSupportedFlag = false;
        
        bool m_singleChannelSelectionSupportedFlag = false;
        
        std::vector<ChannelInfo> m_channelInfo;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MEDIA_FILE_CHANNEL_INFO_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MEDIA_FILE_CHANNEL_INFO_DECLARE__

} // namespace
#endif  //__MEDIA_FILE_CHANNEL_INFO_H__
