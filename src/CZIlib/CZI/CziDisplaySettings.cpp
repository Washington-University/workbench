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

#include "stdafx.h"
#include <cctype>
#include "CziDisplaySettings.h"
#include "splines.h"
#include "utilities.h"
#include "Site.h"

using namespace std;
using namespace libCZI;

class CCziDisplaySettingsUtils
{
public:
	template <typename tString>
	static bool TryGetRgb8ColorFromString(const tString& strXml, Rgb8Color& color)
	{
		auto str = Utilities::Trim(strXml);
		if (str.size() > 1 && str[0] == '#')
		{
			// TODO: do we have to deal with shorter string (e.g. without alpha, or just '#FF')?
			std::uint8_t r, g, b, a;
			r = g = b = a = 0;
			for (size_t i = 1; i < (std::max)((size_t)9, (size_t)str.size()); ++i)
			{
				if (!isxdigit(str[i]))
				{
					return false;
				}

				switch (i)
				{
				case 1: a = Utilities::HexCharToInt((char)str[i]); break;
				case 2: a = (a << 4) + Utilities::HexCharToInt((char)str[i]); break;
				case 3: r = Utilities::HexCharToInt((char)str[i]); break;
				case 4: r = (r << 4) + Utilities::HexCharToInt((char)str[i]); break;
				case 5: g = Utilities::HexCharToInt((char)str[i]); break;
				case 6: g = (g << 4) + Utilities::HexCharToInt((char)str[i]); break;
				case 7: b = Utilities::HexCharToInt((char)str[i]); break;
				case 8: b = (b << 4) + Utilities::HexCharToInt((char)str[i]); break;
				}
			}

			color = Rgb8Color{ r,g,b };
			return true;
		}

		return false;
	}
};

CCziDisplaySettings::CCziDisplaySettings(std::function<bool(int no, int&, ChannelDisplaySetting& dispSetting)> getChannelDisplaySettings)
{
	if (getChannelDisplaySettings)
	{
		int no = 0;
		for (;; ++no)
		{
			int chIdx; ChannelDisplaySetting chDsplSettings;
			if (!getChannelDisplaySettings(no, chIdx, chDsplSettings))
			{
				break;
			}

			this->channelDisplaySettings[chIdx] = chDsplSettings;
		}
	}
}

/*static*/std::shared_ptr<libCZI::IDisplaySettings> CCziDisplaySettings::CreateFromXml(pugi::xml_node node)
{
	auto channels = node.child(L"Channels");
	if (channels.empty())
	{
		return std::shared_ptr<libCZI::IDisplaySettings>();
	}

	int channelIdx = 0;
	vector<tuple<int, ChannelDisplaySetting>> displSettings;
	for (auto ch : channels.children(L"Channel"))
	{
		ChannelDisplaySetting chDsplSetting;
		chDsplSetting.gradationCurveMode = GradationCurveMode::Linear;
		auto subNode = ch.child(L"Low");
		if (!subNode.empty())
		{
			chDsplSetting.blackPoint = subNode.text().as_float();
		}
		else
		{
			chDsplSetting.blackPoint = 0;
		}

		subNode = ch.child(L"High");
		if (!subNode.empty())
		{
			chDsplSetting.whitePoint = subNode.text().as_float();
		}
		else
		{
			chDsplSetting.whitePoint = 1;
		}

		subNode = ch.child(L"Weight");
		if (!subNode.empty())
		{
			chDsplSetting.weight = subNode.text().as_float();
		}
		else
		{
			chDsplSetting.weight = 1;
		}

		chDsplSetting.enableTinting = true;
		subNode = ch.child(L"ColorMode");
		if (!subNode.empty())
		{
			auto s = Utilities::Trim(subNode.text().as_string());
			if (Utilities::icasecmp(s, L"none"))
			{
				chDsplSetting.enableTinting = false;
			}
		}

		subNode = ch.child(L"Color");
		chDsplSetting.tintingColor.r = chDsplSetting.tintingColor.g = chDsplSetting.tintingColor.b = 0xff;
		if (!subNode.empty())
		{
			bool b = CCziDisplaySettingsUtils::TryGetRgb8ColorFromString(subNode.text().as_string(), chDsplSetting.tintingColor);
			if (b == false)
			{
				GetSite()->Log(LOGLEVEL_WARNING, "Could not parse the content of the 'Color'-field ");
			}
		}

		subNode = ch.child(L"IsSelected");
		if (!subNode.empty())
		{
			chDsplSetting.isEnabled = subNode.text().as_bool();
		}
		else
		{
			chDsplSetting.isEnabled = true;
		}

		subNode = ch.child(L"Gamma");
		if (!subNode.empty())
		{
			chDsplSetting.gamma = subNode.text().as_float();
			chDsplSetting.gradationCurveMode = GradationCurveMode::Gamma;
		}
		else
		{
			chDsplSetting.gamma = std::numeric_limits<float>::quiet_NaN();
		}

		subNode = ch.child(L"Mode");
		if (!subNode.empty())
		{
			auto s = Utilities::Trim(subNode.text().as_string());
			if (Utilities::icasecmp(s, L"spline"))
			{
				auto subNodePoints = ch.child(L"Points");
				if (!subNodePoints.empty())
				{
					try
					{
						chDsplSetting.splineControlPoints = ParseSplinePoints(subNodePoints.text().as_string());
					}
					catch (invalid_argument&)
					{
						GetSite()->Log(LOGLEVEL_WARNING, "Could not parse the content of the 'Point'-field");
					}

					if (!chDsplSetting.splineControlPoints.empty())
					{
						chDsplSetting.gradationCurveMode = GradationCurveMode::Spline;
					}
					else
					{
						GetSite()->Log(LOGLEVEL_WARNING, "The 'Point'-field was found to be empty");
					}

					//chDsplSetting.splineData = GetSplineDataFromXmlString(subNodePoints.text().as_string());
					//if (!chDsplSetting.splineData.empty())
					//{
					//	chDsplSetting.gradationCurveMode = GradationCurveMode::Spline;
					//}
					//else
					//{
					//	GetSite()->Log(LOGLEVEL_WARNING, "Could not parse the content of the 'Point'-field ");
					//}
				}
			}
		}

		displSettings.emplace_back(make_tuple(channelIdx, chDsplSetting));
		channelIdx++;

		// TODO: what is authorative? The position of the node, the channel-Id or the channel-Name?
	}

	return std::make_shared<CCziDisplaySettings>(
		[&](int no, int& chIdx, ChannelDisplaySetting& chDsplSettings) -> bool
	{
		if (no < (int)displSettings.size())
		{
			chIdx = get<0>(displSettings[no]);
			chDsplSettings = get<1>(displSettings[no]);
			return true;
		}

		return false;
	});
}

/*virtual*/void CCziDisplaySettings::EnumChannels(std::function<bool(int)> func) const
{
	for (std::map<int, ChannelDisplaySetting>::const_iterator it = this->channelDisplaySettings.cbegin(); it != this->channelDisplaySettings.cend(); ++it)
	{
		bool b = func(it->first);
		if (b == false)
		{
			break;
		}
	}
}

/*virtual*/std::shared_ptr<libCZI::IChannelDisplaySetting> CCziDisplaySettings::GetChannelDisplaySettings(int chIndex) const
{
	const auto& it = this->channelDisplaySettings.find(chIndex);
	if (it != this->channelDisplaySettings.cend())
	{
		return std::make_shared<CCziChannelDisplaySettings>(it->second);
	}

	return std::shared_ptr<libCZI::IChannelDisplaySetting>();
}

//////////////////////////////////////////////////////////////////////////////////
/*static*/std::vector<libCZI::IDisplaySettings::SplineControlPoint> CCziDisplaySettings::ParseSplinePoints(const wchar_t* szString)
{
	// example for syntax: 0.362559241706161,0.876190476190476 0.554502369668246,0.561904761904762

	// TODO: parsing not really watertight I suppose...
	std::vector<libCZI::IDisplaySettings::SplineControlPoint> splinePts;
	size_t charsParsed;
	for (; *szString != L'\0';)
	{
		double x, y;
		x = std::stof(szString, &charsParsed);
		szString += charsParsed;

		// now, skip whitespace and exactly one comma
		for (;;)
		{
			wchar_t c = *szString;
			if (std::isspace(c))
			{
				++szString;
				continue;
			}

			if (c == L',')
			{
				++szString;
				break;
			}

			throw std::invalid_argument("invalid syntax");
		}

		y = std::stof(szString, &charsParsed);
		szString += charsParsed;

		splinePts.push_back({ x,y });

		for (;;)
		{
			wchar_t c = *szString;
			if (std::isspace(c))
			{
				++szString;
				continue;
			}

			break;
		}
	}

	return splinePts;
}

/*static*/std::vector<libCZI::IDisplaySettings::SplineData> CCziDisplaySettings::GetSplineDataFromXmlString(const wchar_t* szString)
{
	auto splinePoints = ParseSplinePoints(szString);
	if (splinePoints.size() <= 1)
	{
		return std::vector<libCZI::IDisplaySettings::SplineData>();
	}

	auto coeffs = CSplines::GetSplineCoefficients(
		(int)splinePoints.size() + 2,
		[&](int index, double* px, double* py)->void
	{
		if (index == 0)
		{
			if (px != nullptr) { *px = 0; }
			if (py != nullptr) { *py = 0; }
		}
		else if (index == (splinePoints.size() + 1))
		{
			if (px != nullptr) { *px = 1; }
			if (py != nullptr) { *py = 1; }
		}
		else
		{
			if (px != nullptr) { *px = splinePoints[index - 1].x; }
			if (py != nullptr) { *py = splinePoints[index - 1].y; }
		}
	});

	std::vector<libCZI::IDisplaySettings::SplineData> splineData; splineData.reserve(coeffs.size());

	for (size_t i = 0; i < coeffs.size(); ++i)
	{
		IDisplaySettings::SplineData spD{ i == 0 ? 0 : splinePoints.at(i - 1).x ,coeffs.at(i) };
		splineData.push_back(spD);
	}

	return splineData;
}

//----------------------------------------------------------------------------------------------------

bool CCziChannelDisplaySettings::GetIsEnabled() const
{
	return this->chDsplSetting.isEnabled;
}

float CCziChannelDisplaySettings::GetWeight() const
{
	return this->chDsplSetting.weight;
}

bool CCziChannelDisplaySettings::TryGetTintingColorRgb8(libCZI::Rgb8Color* pColor) const
{
	if (this->chDsplSetting.enableTinting)
	{
		if (pColor != nullptr)
		{
			*pColor = this->chDsplSetting.tintingColor;
		}

		return true;
	}

	return false;
}

void CCziChannelDisplaySettings::GetBlackWhitePoint(float* pBlack, float* pWhite) const
{
	if (pBlack != nullptr)
	{
		*pBlack = this->chDsplSetting.blackPoint;
	}

	if (pWhite != nullptr)
	{
		*pWhite = this->chDsplSetting.whitePoint;
	}
}

/*virtual*/IDisplaySettings::GradationCurveMode CCziChannelDisplaySettings::GetGradationCurveMode() const
{
	return this->chDsplSetting.gradationCurveMode;
}

/*virtual*/bool CCziChannelDisplaySettings::TryGetGamma(float* gamma)const
{
	if (this->GetGradationCurveMode() == IDisplaySettings::GradationCurveMode::Gamma)
	{
		if (gamma != nullptr)
		{
			*gamma = this->chDsplSetting.gamma;
		}

		return true;
	}

	return false;
}


/*virtual*/bool	CCziChannelDisplaySettings::TryGetSplineControlPoints(std::vector<libCZI::IDisplaySettings::SplineControlPoint>* ctrlPts) const
{
	if (this->GetGradationCurveMode() == IDisplaySettings::GradationCurveMode::Spline)
	{
		if (ctrlPts!=nullptr)
		{
			*ctrlPts = this->chDsplSetting.splineControlPoints;
		}

		return true;
	}

	return false;
}

/*virtual*/bool CCziChannelDisplaySettings::TryGetSplineData(std::vector<libCZI::IDisplaySettings::SplineData>* data) const
{
	if (this->GetGradationCurveMode() == IDisplaySettings::GradationCurveMode::Spline)
	{
		if (data != nullptr)
		{
			auto coeffs = CSplines::GetSplineCoefficients(
				(int)this->chDsplSetting.splineControlPoints.size() + 2,
				[&](int index, double* px, double* py)->void
			{
				if (index == 0)
				{
					if (px != nullptr) { *px = 0; }
					if (py != nullptr) { *py = 0; }
				}
				else if (index == this->chDsplSetting.splineControlPoints.size() + 1)
				{
					if (px != nullptr) { *px = 1; }
					if (py != nullptr) { *py = 1; }
				}
				else
				{
					if (px != nullptr) { *px = this->chDsplSetting.splineControlPoints[index - 1].x; }
					if (py != nullptr) { *py = this->chDsplSetting.splineControlPoints[index - 1].y; }
				}
			});

			std::vector<libCZI::IDisplaySettings::SplineData> splineData; splineData.reserve(coeffs.size());

			data->clear();
			data->reserve(coeffs.size());
			for (size_t i = 0; i < coeffs.size(); ++i)
			{
				IDisplaySettings::SplineData spD{ i == 0 ? 0 : this->chDsplSetting.splineControlPoints.at(i - 1).x ,coeffs.at(i) };
				data->push_back(spD);
			}
		}

		return true;
	}

	return false;
}
