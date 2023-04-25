
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

#define __MEDIA_FILE_CHANNEL_INFO_DECLARE__
#include "MediaFileChannelInfo.h"
#undef __MEDIA_FILE_CHANNEL_INFO_DECLARE__

#include "CaretAssert.h"
#include "DataFileContentInformation.h"

using namespace caret;


    
/**
 * \class caret::MediaFileChannelInfo 
 * \brief Information about channels (components)  in a media fil
 * \ingroup Files
 */

/**
 * Constructor for file that does not support channels
 */
MediaFileChannelInfo::MediaFileChannelInfo()
: CaretObject(),
m_channelsSupportedFlag(false)
{
    
}

/**
 * Constructor for file that supports channels
 * @param allChannelsSelectionSupportedFlag
 * @param singleChannelSelectionSupportedFlag
 */
MediaFileChannelInfo::MediaFileChannelInfo(const bool allChannelsSelectionSupportedFlag,
                                           const bool singleChannelSelectionSupportedFlag)
: CaretObject(),
m_channelsSupportedFlag(true),
m_allChannelsSelectionSupportedFlag(allChannelsSelectionSupportedFlag),
m_singleChannelSelectionSupportedFlag(singleChannelSelectionSupportedFlag)
{
    
}

/**
 * Destructor.
 */
MediaFileChannelInfo::~MediaFileChannelInfo()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
MediaFileChannelInfo::MediaFileChannelInfo(const MediaFileChannelInfo& obj)
: CaretObject(obj)
{
    this->copyHelperMediaFileChannelInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
MediaFileChannelInfo&
MediaFileChannelInfo::operator=(const MediaFileChannelInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperMediaFileChannelInfo(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
MediaFileChannelInfo::copyHelperMediaFileChannelInfo(const MediaFileChannelInfo& obj)
{
    m_channelsSupportedFlag = obj.m_channelsSupportedFlag;
    m_allChannelsSelectionSupportedFlag = obj.m_allChannelsSelectionSupportedFlag;
    m_singleChannelSelectionSupportedFlag = obj.m_singleChannelSelectionSupportedFlag;
    m_channelInfo = obj.m_channelInfo;
}

/**
 * Add a channel
 * @param channelName
 *    Name of channel
 */
void
MediaFileChannelInfo::addChannel(const AString& channelName)
{
    m_channelInfo.emplace_back(channelName);
}

/**
 * @retrurn True if channels are supported
 */
bool
MediaFileChannelInfo::isChannelsSupported() const
{
    return m_channelsSupportedFlag;
}

/**
 * @retrurn True if all channels selection is supported
 */
bool
MediaFileChannelInfo::isAllChannelsSelectionSupported() const
{
    return m_allChannelsSelectionSupportedFlag;
}

/**
 * @retrurn True if single channel selection is supported
 */
bool
MediaFileChannelInfo::isSingleChannelSelectionSupported() const
{
    return m_singleChannelSelectionSupportedFlag;
}

/**
 * @retrurn Number of channels
 */
int32_t
MediaFileChannelInfo::getNumberOfChannels() const
{
    return m_channelInfo.size();
}

/**
 * @retrurn Name of channel at the given index
 * @param channelIndex
 *    Index of the channel
 */
AString
MediaFileChannelInfo::getChannelName(const int32_t channelIndex) const
{
    CaretAssertVectorIndex(m_channelInfo, channelIndex);
    return m_channelInfo[channelIndex].m_name;
}

/**
 * Add to the data file information.
 * @param dataFileInformation
 *    Item to which information is added.
 */
void
MediaFileChannelInfo::addToDataFileContentInformation(DataFileContentInformation& dfi)
{
    dfi.addNameAndValue("Channels Supported", m_channelsSupportedFlag);
    if (m_channelsSupportedFlag) {
        dfi.addNameAndValue("Select All Supported", m_allChannelsSelectionSupportedFlag);
        dfi.addNameAndValue("Select Single Supported", m_singleChannelSelectionSupportedFlag);
        dfi.addNameAndValue("Number of Channels", getNumberOfChannels());
        for (int32_t i = 0; i < getNumberOfChannels(); i++) {
            const AString prefix("Chan " + AString::number(i));
            dfi.addNameAndValue(prefix + " Name", getChannelName(i));
        }
    }
}
