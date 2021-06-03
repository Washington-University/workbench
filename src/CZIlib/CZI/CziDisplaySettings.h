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
#include "splines.h"
#include "pugixml.hpp"

class CCziDisplaySettings : public libCZI::IDisplaySettings
{
public:
	struct ChannelDisplaySetting
	{
		bool isEnabled;
		float weight;
		float blackPoint, whitePoint;
		bool enableTinting;
		libCZI::Rgb8Color tintingColor;
		libCZI::IDisplaySettings::GradationCurveMode gradationCurveMode;
		float gamma;
		std::vector<libCZI::IDisplaySettings::SplineControlPoint> splineControlPoints;
	};

public:
	explicit CCziDisplaySettings(std::function<bool(int no,int&, ChannelDisplaySetting& dispSetting)> getChannelDisplaySettings);
	static std::shared_ptr<libCZI::IDisplaySettings> CreateFromXml(pugi::xml_node node);
public:	// interface IDisplaySettings
	void EnumChannels(std::function<bool(int)> func) const override;
	std::shared_ptr<libCZI::IChannelDisplaySetting> GetChannelDisplaySettings(int chIndex) const override;
private:
	std::map<int, ChannelDisplaySetting> channelDisplaySettings;

	static std::vector<libCZI::IDisplaySettings::SplineControlPoint> ParseSplinePoints(const wchar_t* szString);

	static std::vector<libCZI::IDisplaySettings::SplineData> GetSplineDataFromXmlString(const wchar_t* szString);

};

class CCziChannelDisplaySettings : public libCZI::IChannelDisplaySetting
{
private:
	CCziDisplaySettings::ChannelDisplaySetting chDsplSetting;
public:
	explicit CCziChannelDisplaySettings(const CCziDisplaySettings::ChannelDisplaySetting& chDsplSetting)
		: chDsplSetting(chDsplSetting)
	{}

	bool	GetIsEnabled() const override;
	float	GetWeight() const override;
	bool	TryGetTintingColorRgb8(libCZI::Rgb8Color* pColor) const override;
	void	GetBlackWhitePoint(float* pBlack, float* pWhite) const override;
	libCZI::IDisplaySettings::GradationCurveMode GetGradationCurveMode() const override;
	bool	TryGetGamma(float* gamma) const override;
	bool	TryGetSplineControlPoints(std::vector<libCZI::IDisplaySettings::SplineControlPoint>* ctrlPts) const override;
	bool	TryGetSplineData(std::vector<libCZI::IDisplaySettings::SplineData>* data) const override;
};
