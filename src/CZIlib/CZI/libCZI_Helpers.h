//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************

#pragma once

#include "libCZI.h"

namespace libCZI
{


	/// This is an utility in order to prepare the information
	/// required for the multi-channel-composition functions from
	/// the display-settings (e. g. retrieved from metadata).
	/// If the gradation curve is given as gamma or as a spline,
	/// we calculate here the lookup-table. In addition, we 
	/// make sure that only channels which are "enabled" (in the
	/// display-settings) are considered.
	class CDisplaySettingsHelper
	{
	private:
		std::vector<libCZI::Compositors::ChannelInfo> channelInfos;
		std::vector<int> activeChannels;
		std::vector<std::vector<std::uint8_t>> lutStore;
	public:

		/// Enumerate the enabled channels. The functor will be called for each active channel
		/// (with the channel-index passed in). 
		///
		/// \param ptrDspSetting	  The display settings.
		/// \param funcEnabledChannel A functor which will be called for each active channel.
		static void EnumEnabledChannels(const libCZI::IDisplaySettings* ptrDspSetting, std::function<bool(int)> funcEnabledChannel)
		{
			ptrDspSetting->EnumChannels(
				[&](int chIndex)->bool
			{
				auto chDsplSetting = ptrDspSetting->GetChannelDisplaySettings(chIndex);
				if (chDsplSetting->GetIsEnabled() == true)
				{
					return funcEnabledChannel(chIndex);
				}

				return true;
			});
		}

		/// Gets a vector containing the channel indices of the  active channels.
		///
		/// \param ptrDspSetting The display settings.
		///
		/// \return A vector containing the channel indices of the  active channels.
		static std::vector<int> GetActiveChannels(const libCZI::IDisplaySettings* ptrDspSetting)
		{
			std::vector<int> result;
      CDisplaySettingsHelper::EnumEnabledChannels(ptrDspSetting, [&](int chIdx)->bool {result.push_back(chIdx); return true; });
			return result;
		}

		/// Initializes this object with a display-settings object.
		/// 
		/// After this object is initialized, it will provide the Compositors::ChannelInfo information
		/// for all 'enabled' channels in the specified display-settings object.
		///
		/// \param ptrDspSetting A pointer to a display-settings object.
		/// \param getPixelTypeForChannelIndex A functor which is called in order to retrieve the pixeltype of the bitmap passed in for the channel with the specified channel index.
		void Initialize(const libCZI::IDisplaySettings* ptrDspSetting, std::function<libCZI::PixelType(int chIndex)> getPixelTypeForChannelIndex)
		{
			this->Clear();
			ptrDspSetting->EnumChannels(
				[&](int chIndex)->bool
			{
				auto chDsplSetting = ptrDspSetting->GetChannelDisplaySettings(chIndex);
				this->AddChannelSetting(chIndex, chDsplSetting.get(), getPixelTypeForChannelIndex);
				return true;
			});
		}

		/// Initializes this object by specified a set of ChannelDisplaySetting-objects (and their respective channel-index).
		/// 
		/// After this object is initialized, it will provide the Compositors::ChannelInfo information
		/// for all 'enabled' channels.
		///
		/// \param getChDisplaySettingAndChannelIdx A functor which is used to retrieve a ChannelDisplaySetting-objects and its respective channels-index.
		/// 										If the functor returns false, the enumeration is cancelled (and the functor is not called any more).
		void Initialize(std::function<bool(int&, std::shared_ptr<libCZI::IChannelDisplaySetting>&)> getChDisplaySettingAndChannelIdx, std::function<libCZI::PixelType(int chIndex)> getPixelTypeForChannelIndex)
		{
			this->Clear();
			for (;;)
			{
				int chIdx; std::shared_ptr<libCZI::IChannelDisplaySetting> chDsplSetting;
				if (getChDisplaySettingAndChannelIdx(chIdx, chDsplSetting) == false)
				{
					break;
				}

				this->AddChannelSetting(chIdx, chDsplSetting.get(), getPixelTypeForChannelIndex);
			}
		}

		/// Gets a vector containing the channel-indices of the active channels.
		///
		/// \return The vector with the channel-indices of the active channels.
		const std::vector<int>& GetActiveChannels() const { return this->activeChannels; }

		/// Gets count of active channels.
		///
		/// \return The active channels count.
		int GetActiveChannelsCount() const { return (int)this->activeChannels.size(); }

		/// Gets a reference to the channel-info for the channel with the specified index.
		/// The index must be in less than the number returned from GetActiveChannelsCount(),
		/// otherwise the behavior is undefined. Note that the index specified here is NOT
		/// the channel-index, it is just the index into the list of active channels (held in
		/// this class).
		///
		/// \param idx The index.
		///
		/// \return The channel-info for the (active) channel with the specified index.
		const libCZI::Compositors::ChannelInfo& GetActiveChannel(int idx) const
		{
			return this->channelInfos.at(idx);
		}

		/// Gets a pointer to the channel-infos array. Note that the data structure contains
		/// pointers to memory (ptrLookUpTable notably) which refer to memory owned by this
		/// class. So it is safe to use this data structure as long as this object exists,
		/// if this object has been destroyed, the behavior is undefined. 
		/// Also the behavior is undefined if this class has not been successfully initialized.
		///
		/// \return A pointer to the channel-infos array (containing as many elements as determined by GetActiveChannelsCount).
		const libCZI::Compositors::ChannelInfo* GetChannelInfosArray()
		{
			return &this->channelInfos[0];
		}
	private:
		void Clear()
		{
			this->channelInfos.clear();
			this->activeChannels.clear();
			this->lutStore.clear();
		}

		void AddChannelSetting(int chIdx, const libCZI::IChannelDisplaySetting* chDsplSetting, std::function<libCZI::PixelType(int chIndex)> getPixelTypeForChannelIndex)
		{
      // Note that we only add this channel IF IT IS ENABLED. If not, we DO NOT and MUST NOT call the 
      // "getPixelTypeForChannelIndex"-callback!
			if (chDsplSetting->GetIsEnabled() != true)
			{
				return;
			}

			libCZI::Compositors::ChannelInfo ci; ci.Clear();
			ci.weight = chDsplSetting->GetWeight();
			chDsplSetting->GetBlackWhitePoint(&ci.blackPoint, &ci.whitePoint);
			if (chDsplSetting->TryGetTintingColorRgb8(&ci.tinting.color) == true)
			{
				ci.enableTinting = true;
			}

			switch (chDsplSetting->GetGradationCurveMode())
			{
			case libCZI::IDisplaySettings::GradationCurveMode::Gamma:
			{
				int lutSize = GetSizeForLUT(chIdx, getPixelTypeForChannelIndex);
				float gamma;
				chDsplSetting->TryGetGamma(&gamma);
				this->lutStore.emplace_back(
					libCZI::Utils::Create8BitLookUpTableFromGamma(lutSize, ci.blackPoint, ci.whitePoint, gamma));
				ci.ptrLookUpTable = &(this->lutStore.back()[0]);
				ci.lookUpTableElementCount = lutSize;
			}
			break;
			case libCZI::IDisplaySettings::GradationCurveMode::Spline:
			{
				int lutSize = GetSizeForLUT(chIdx, getPixelTypeForChannelIndex);
				std::vector<libCZI::IDisplaySettings::SplineData> splineData;
				chDsplSetting->TryGetSplineData(&splineData);
				this->lutStore.emplace_back(
					libCZI::Utils::Create8BitLookUpTableFromSplines(lutSize, ci.blackPoint, ci.whitePoint, splineData));
				ci.ptrLookUpTable = &(this->lutStore.back()[0]);
				ci.lookUpTableElementCount = lutSize;
			}
			break;
			default:	// silence warnings
				break;
			}

			this->channelInfos.push_back(ci);
			this->activeChannels.push_back(chIdx);
		}

		static int GetSizeForLUT(int chIdx, std::function<libCZI::PixelType(int chIndex)> getPixelTypeForChannelIndex)
		{
			libCZI::PixelType  pxlType = getPixelTypeForChannelIndex(chIdx);
			switch (pxlType)
			{
			case libCZI::PixelType::Gray8:
			case libCZI::PixelType::Bgr24: return 256;
			case libCZI::PixelType::Gray16:
			case libCZI::PixelType::Bgr48: return 256 * 256;
			default: throw std::runtime_error("Pixeltype not supported");
			}
		}
	};
}