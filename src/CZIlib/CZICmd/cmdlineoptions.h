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

#include <cmath>
#include <type_traits>
#include "consoleio.h"
#include "inc_libCZI.h"
#include "utils.h"

enum class Command
{
	Invalid,

	PrintInformation,

	ExtractSubBlock,

	SingleChannelTileAccessor,

	ChannelComposite,

	SingleChannelPyramidTileAccessor,

	SingleChannelScalingTileAccessor,

	ScalingChannelComposite,

	ExtractAttachment
};

enum class InfoLevel
{
	None = 0,

	Statistics = 1,
	RawXML = 2,
	DisplaySettings = 4,
	DisplaySettingsJson = 8,
	AllSubBlocks = 16,
	AttachmentInfo = 32,
	AllAttachments = 64,
	PyramidStatistics = 128,
	GeneralInfo = 256,
	ScalingInfo = 512,

	All = Statistics | RawXML | DisplaySettings | DisplaySettingsJson | AllSubBlocks | AttachmentInfo | AllAttachments | PyramidStatistics | GeneralInfo | ScalingInfo
};

struct ChannelDisplaySettings
{

	float		 weight;

	bool		 enableTinting;
	libCZI::Compositors::TintingColor tinting;

	float		 blackPoint;
	float		 whitePoint;

	float		 gamma;
	std::vector<std::tuple<double, double>> splinePoints;

	bool IsGammaValid() const
	{
		return !std::isnan(gamma);
	}

	bool IsSplinePointsValid() const
	{
		if (IsGammaValid())
		{
			// gamma takes precedence, if it is valid, we consider it to be "more important"
			return false;
		}

		return this->splinePoints.size() >= 2;
	}

	void Clear()
	{
		this->weight = 0;
		this->enableTinting = false;
		this->blackPoint = 0;
		this->whitePoint = 1;
		this->gamma = std::numeric_limits<float>::quiet_NaN();
		this->splinePoints.clear();
	}
};

struct ItemValue
{
	static const char* SelectionItem_Name;// = "name";
	static const char* SelectionItem_Index;// = "index";

	enum class Type
	{
		Invalid, String, Number, Boolean
	};

	ItemValue() : type(Type::Invalid) {}
	explicit ItemValue(const char* str) : type(Type::String), strValue(str) {}
	explicit ItemValue(const std::string& str) : type(Type::String), strValue(str) {}
	explicit ItemValue(double v) : type(Type::Number), doubleValue(v) {}
	explicit ItemValue(bool b) : type(Type::Boolean), boolValue(b) {}

	Type		type;
	std::string strValue;
	double      doubleValue;
	bool        boolValue;

	bool	IsValid() const { return this->type != Type::Invalid; }
	bool	IsNumber() const { return this->type == Type::Number; }
	bool	IsString() const { return this->type == Type::String; }
	bool	IsBoolean() const { return this->type == Type::Boolean; }

	bool TryGetNumber(double* p) const { if (this->IsNumber()) { if (p != nullptr) { *p = this->doubleValue; }; return true; } return false; }
	bool TryGetString(std::string* p) const { if (this->IsString()) { if (p != nullptr) { *p = this->strValue; }; return true; } return false; }
	bool TryGetBoolean(std::string* p) const { if (this->IsBoolean()) { if (p != nullptr) { *p = this->boolValue; }; return true; } return false; }
};

class CCmdLineOptions
{
private:
	std::shared_ptr<ILog> log;

	Command command;
	std::wstring cziFilename;

	libCZI::CDimCoordinate planeCoordinate;

	bool	rectModeAbsoluteOrRelative;	// true->absolute, false->relative
	int		rectX, rectY, rectW, rectH;

	std::wstring outputPath;
	std::wstring outputFilename;

	std::map<int, ChannelDisplaySettings> multiChannelCompositeChannelInfos;
	bool useDisplaySettingsFromDocument;

	bool calcHashOfResult;

	bool drawTileBoundaries;

	std::uint32_t enabledOutputLevels;

	bool useWicJxrDecoder;

	libCZI::RgbFloatColor	backGroundColor;

	int pyramidMinificationFactor;
	int pyramidLayerNo;

	float zoom;

	InfoLevel infoLevel;

	libCZI::PixelType channelCompositePixelType;
	std::uint8_t channelCompositeAlphaValue;

	std::map<std::string, ItemValue> mapSelection;
	std::shared_ptr<libCZI::IIndexSet> sceneIndexSet;
public:
	explicit CCmdLineOptions(std::shared_ptr<ILog> log);

	void Clear();

#if defined(WIN32ENV)
	bool Parse(int argc, wchar_t** argv);
#endif
#if defined(LINUXENV)
	bool Parse(int argc, char** argv);
#endif

	std::shared_ptr<ILog> GetLog() const { return this->log; }
	Command GetCommand() const { return this->command; }
	const std::wstring& GetCZIFilename() const { return this->cziFilename; }
	const libCZI::CDimCoordinate& GetPlaneCoordinate() const { return this->planeCoordinate; }
	const std::map<int, ChannelDisplaySettings>& GetMultiChannelCompositeChannelInfos() const { return this->multiChannelCompositeChannelInfos; }
	bool GetUseDisplaySettingsFromDocument() const { return this->useDisplaySettingsFromDocument; }

	libCZI::IntRect GetRect() const { return libCZI::IntRect{ this->rectX,this->rectY,this->rectW,this->rectH }; }
	bool GetIsAbsoluteRectCoordinate() const { return this->rectModeAbsoluteOrRelative; }
	bool GetIsRelativeRectCoordinate() const { return !this->rectModeAbsoluteOrRelative; }
	int GetRectX() const { return this->rectX; }
	int GetRectY() const { return this->rectY; }
	int GetRectW() const { return this->rectW; }
	int GetRectH() const { return this->rectH; }
	bool GetCalcHashOfResult() const { return this->calcHashOfResult; }

	bool GetDrawTileBoundaries() const { return this->drawTileBoundaries; }

	std::wstring MakeOutputFilename(const wchar_t* suffix, const wchar_t* extension) const;

	bool GetUseWICJxrDecoder() const { return this->useWicJxrDecoder; }

	libCZI::RgbFloatColor GetBackGroundColor() const { return this->backGroundColor; }

	bool IsLogLevelEnabled(int level) const;

	int GetPyramidInfoMinificationFactor() const { return this->pyramidMinificationFactor; }
	int GetPyramidInfoLayerNo() const { return this->pyramidLayerNo; }
	float GetZoom() const { return this->zoom; }

	InfoLevel GetInfoLevel() const { return this->infoLevel; }
	bool IsInfoLevelEnabled(InfoLevel lvl) const { return (((std::underlying_type<InfoLevel>::type)this->infoLevel)&((std::underlying_type<InfoLevel>::type)lvl)) != 0; }

	ItemValue GetSelectionItemValue(const char* sz) const;

	std::shared_ptr<libCZI::IIndexSet> GetSceneIndexSet() const;

	libCZI::PixelType GetChannelCompositeOutputPixelType() const { return this->channelCompositePixelType; }
	std::uint8_t GetChannelCompositeOutputAlphaValue() const { return this->channelCompositeAlphaValue; }
private:
	void PrintUsage(int switchesCnt, std::function<std::tuple<std::wstring, std::wstring>(int idx)> getSwitch);
	bool CheckArgumentConsistency() const;

	static Command ParseCommand(const wchar_t* s);
	static Command ParseCommand(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); return ParseCommand(sucs2.c_str()); }

	libCZI::CDimCoordinate ParseDimCoordinate(const std::string& s);
	void ParseRect(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); this->ParseRect(sucs2); }
	void SetOutputFilename(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); this->SetOutputFilename(sucs2); }
	libCZI::CDimCoordinate ParseDimCoordinate(const std::wstring& str);
	void ParseRect(const std::wstring& s);
	void SetOutputFilename(const std::wstring& s);

	void ParseDisplaySettings(const std::wstring& s);
	void ParseDisplaySettings(const std::string& s);

	std::uint32_t ParseVerbosityLevel(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); return ParseVerbosityLevel(sucs2.c_str()); }
	std::uint32_t ParseVerbosityLevel(const wchar_t* s);

	bool ParseJxrCodec(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); return ParseJxrCodec(sucs2.c_str()); }
	bool ParseJxrCodec(const wchar_t* s);

	void PrintSynopsis(int switchesCnt, std::function<std::tuple<std::wstring, std::wstring>(int idx)> getSwitch, std::function<std::tuple<std::wstring, std::wstring>(const wchar_t* option)> getExplanation);

	static libCZI::RgbFloatColor ParseBackgroundColor(const wchar_t* s);
	static libCZI::RgbFloatColor ParseBackgroundColor(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); return ParseBackgroundColor(sucs2.c_str()); }

	void ParsePyramidInfo(const wchar_t* sz);
	void ParsePyramidInfo(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); this->ParsePyramidInfo(sucs2.c_str()); }

	void ParseZoom(const wchar_t* sz);
	void ParseZoom(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); this->ParseZoom(sucs2.c_str()); }

	void ParseInfoLevel(const wchar_t* s);
	void ParseInfoLevel(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); this->ParseInfoLevel(sucs2.c_str()); }

	void ParseSelection(const std::wstring& s);
	void ParseSelection(const std::string& s);

	void ParseTileFilter(const wchar_t* s);
	void ParseTileFilter(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); this->ParseTileFilter(sucs2.c_str()); }

	void ParseChannelCompositionFormat(const wchar_t* s);
	void ParseChannelCompositionFormat(const std::string& s) { auto sucs2 = convertUtf8ToUCS2(s); this->ParseChannelCompositionFormat(sucs2.c_str()); }

	static bool TryParseChannelCompositionFormatWithAlphaValue(const std::wstring& s, libCZI::PixelType& channelCompositePixelType, std::uint8_t& channelCompositeAlphaValue);
};
