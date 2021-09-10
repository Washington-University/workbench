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

#include "inc_libCZI.h"
#include <memory>

class ChannelDisplaySettingsWrapper :public libCZI::IChannelDisplaySetting
{
private:
	const ChannelDisplaySettings& chDisplSettings;
public:
	explicit ChannelDisplaySettingsWrapper(const ChannelDisplaySettings& dspl)
		: chDisplSettings(dspl)
	{}

public:
	bool GetIsEnabled()const  override { return true; }
	float GetWeight() const override { return this->chDisplSettings.weight; }
	bool TryGetTintingColorRgb8(libCZI::Rgb8Color* pColor) const override
	{
		if (this->chDisplSettings.enableTinting == true)
		{
			if (pColor != nullptr)
			{
				*pColor = this->chDisplSettings.tinting.color;
			}

			return true;
		}

		return false;
	}

	void GetBlackWhitePoint(float* pBlack, float* pWhite) const override
	{
		if (pBlack != nullptr) { *pBlack = this->chDisplSettings.blackPoint; }
		if (pWhite != nullptr) { *pWhite = this->chDisplSettings.whitePoint; }
	}

	libCZI::IDisplaySettings::GradationCurveMode GetGradationCurveMode() const override
	{
		if (this->chDisplSettings.IsGammaValid())
		{
			return libCZI::IDisplaySettings::GradationCurveMode::Gamma;
		}

		if (this->chDisplSettings.IsSplinePointsValid())
		{
			return libCZI::IDisplaySettings::GradationCurveMode::Spline;
		}

		return libCZI::IDisplaySettings::GradationCurveMode::Linear;
	}

	bool TryGetGamma(float* gamma)const override
	{
		if (this->GetGradationCurveMode() == libCZI::IDisplaySettings::GradationCurveMode::Gamma)
		{
			if (gamma != nullptr) { *gamma = this->chDisplSettings.gamma; }
			return true;
		}

		return false;
	}

	bool TryGetSplineControlPoints(std::vector<libCZI::IDisplaySettings::SplineControlPoint>* /*ctrlPts*/) const override
	{
		throw std::runtime_error("not implemented");
	}

	bool TryGetSplineData(std::vector<libCZI::IDisplaySettings::SplineData>* data) const override
	{
		if (this->GetGradationCurveMode() == libCZI::IDisplaySettings::GradationCurveMode::Spline)
		{
			if (data != nullptr)
			{
				*data = libCZI::Utils::CalcSplineDataFromPoints(
					(int)this->chDisplSettings.splinePoints.size(),
					[&](int idx)->std::tuple<double, double>
				{
					return this->chDisplSettings.splinePoints.at(idx);
				});
			}

			return true;
		}

		return false;
	}
};

class CDisplaySettingsWrapper : public libCZI::IDisplaySettings
{
private:
	std::map<int, std::shared_ptr<libCZI::IChannelDisplaySetting>> chDsplSettings;
public:
	explicit CDisplaySettingsWrapper(const CCmdLineOptions& options)
	{
		const auto& cmdLineChDsplSettingsMap = options.GetMultiChannelCompositeChannelInfos();
		for (std::map<int, ChannelDisplaySettings>::const_iterator it = cmdLineChDsplSettingsMap.cbegin(); it != cmdLineChDsplSettingsMap.cend(); ++it)
		{
			this->chDsplSettings[it->first] = std::make_shared<ChannelDisplaySettingsWrapper>(it->second);
		}
	}

	void EnumChannels(std::function<bool(int)> func) const override
	{
		for (std::map<int, std::shared_ptr<libCZI::IChannelDisplaySetting>>::const_iterator it = this->chDsplSettings.cbegin();
			it != this->chDsplSettings.cend(); ++it)
		{
			func(it->first);
		}
	}

	std::shared_ptr<libCZI::IChannelDisplaySetting> GetChannelDisplaySettings(int chIndex) const override
	{
		return this->chDsplSettings.at(chIndex);
	}
};
