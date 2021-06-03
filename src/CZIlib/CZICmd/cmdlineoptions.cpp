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
#include "cmdlineoptions.h"
#include "getOpt.h"
#include <clocale>
#include <locale>
#include <regex>
#include <iostream>
#include <utility>
#include <cstring>
#if defined(LINUXENV)
#include <libgen.h>
#endif
#include "inc_rapidjson.h"

using namespace std;

/*static*/const char* ItemValue::SelectionItem_Name = "name";
/*static*/const char* ItemValue::SelectionItem_Index = "index";

CCmdLineOptions::CCmdLineOptions(std::shared_ptr<ILog> log)
	: log(std::move(log))
{
	this->Clear();
}

#if defined(WIN32ENV)
bool CCmdLineOptions::Parse(int argc, wchar_t** argv)
#endif
#if defined(LINUXENV)
bool CCmdLineOptions::Parse(int argc, char** argv)
#endif
{
#if defined(WIN32ENV)
#define OPTTEXTSPEC(x) L##x
	static const struct optionW long_options[] =
#endif
#if defined(LINUXENV)
#define OPTTEXTSPEC(x) x
		static const struct option long_options[] =
#endif
	{
		{ OPTTEXTSPEC("help"),						 no_argument,		0, OPTTEXTSPEC('?') },
		{ OPTTEXTSPEC("command"),					 required_argument, 0, OPTTEXTSPEC('c') },
		{ OPTTEXTSPEC("source"),					 required_argument, 0, OPTTEXTSPEC('s') },
		{ OPTTEXTSPEC("output"),					 required_argument, 0, OPTTEXTSPEC('o') },
		{ OPTTEXTSPEC("plane-coordinate"),			 required_argument, 0, OPTTEXTSPEC('p') },
		{ OPTTEXTSPEC("rect"),						 required_argument, 0, OPTTEXTSPEC('r') },
		{ OPTTEXTSPEC("display-settings"),			 required_argument, 0, OPTTEXTSPEC('d') },
		{ OPTTEXTSPEC("calc-hash"),					 no_argument,		0, OPTTEXTSPEC('h') },
		{ OPTTEXTSPEC("drawtileboundaries"),		 no_argument,		0, OPTTEXTSPEC('t') },
		{ OPTTEXTSPEC("jpgxrcodec"),				 required_argument, 0, OPTTEXTSPEC('j') },
		{ OPTTEXTSPEC("verbosity"),					 required_argument, 0, OPTTEXTSPEC('v') },
		{ OPTTEXTSPEC("background"),				 required_argument, 0, OPTTEXTSPEC('b') },
		{ OPTTEXTSPEC("pyramidinfo"),				 required_argument, 0, OPTTEXTSPEC('y') },
		{ OPTTEXTSPEC("zoom"),						 required_argument, 0, OPTTEXTSPEC('z') },
		{ OPTTEXTSPEC("info-level"),				 required_argument, 0, OPTTEXTSPEC('i') },
		{ OPTTEXTSPEC("selection"),					 required_argument, 0, OPTTEXTSPEC('e') },
		{ OPTTEXTSPEC("tile-filter"),				 required_argument, 0, OPTTEXTSPEC('f') },
		{ OPTTEXTSPEC("channelcompositionformat"),	 required_argument, 0, OPTTEXTSPEC('m') },
		{ 0, 0, 0, 0 }
	};

#undef OPTTEXTSPEC

	for (;;)
	{
		int option_index;
#if defined(WIN32ENV)
		int c = getoptW_long(argc, argv, L"?v:j:s:c:p:r:o:d:htb:y:z:i:e:f:m:", long_options, &option_index);
#endif
#if defined(LINUXENV)
		int c = getopt_long(argc, argv, "?v:j:s:c:p:r:o:d:htb:y:z:i:e:f:m:", long_options, &option_index);
#endif
		if (c == -1)
		{
			break;
		}

		switch ((char)c)
		{
		case 'c':
			this->command = CCmdLineOptions::ParseCommand(optarg);
			break;
		case 's':
#if defined(WIN32ENV)
			this->cziFilename = optarg;
#endif
#if defined(LINUXENV)
			this->cziFilename = convertUtf8ToUCS2(optarg);
#endif
			break;
		case '?':
			this->PrintUsage(
				sizeof(long_options) / sizeof(long_options[0]) - 1,
				[&](int idx) ->	tuple<wstring, wstring>
			{
#if defined(WIN32ENV)
				return make_tuple(wstring(1, (wchar_t)long_options[idx].val), wstring(long_options[idx].name));
#endif
#if defined(LINUXENV)
				return make_tuple(wstring(1, (wchar_t)long_options[idx].val), convertUtf8ToUCS2(long_options[idx].name));
#endif
			});
			return true;
		case 'p':
			this->planeCoordinate = this->ParseDimCoordinate(optarg);
			break;
		case 'r':
			this->ParseRect(optarg);
			break;
		case 'o':
			this->SetOutputFilename(optarg);
			break;
		case 'd':
			this->ParseDisplaySettings(optarg);
			this->useDisplaySettingsFromDocument = false;
			break;
		case 'h':
			this->calcHashOfResult = true;
			break;
		case 't':
			this->drawTileBoundaries = true;
			break;
		case 'v':
			this->enabledOutputLevels = this->ParseVerbosityLevel(optarg);
			break;
		case 'j':
			this->useWicJxrDecoder = this->ParseJxrCodec(optarg);
			break;
		case 'b':
			this->backGroundColor = this->ParseBackgroundColor(optarg);
			break;
		case 'y':
			this->ParsePyramidInfo(optarg);
			break;
		case 'z':
			this->ParseZoom(optarg);
			break;
		case 'i':
			this->ParseInfoLevel(optarg);
			break;
		case 'e':
			this->ParseSelection(optarg);
			break;
		case 'f':
			this->ParseTileFilter(optarg);
			break;
		case 'm':
			this->ParseChannelCompositionFormat(optarg);
			break;
		default:
			break;
		}
	}

	return this->CheckArgumentConsistency();
}

bool CCmdLineOptions::CheckArgumentConsistency() const
{
	stringstream ss;
	static const char* ERRORPREFIX = "Argument error: ";
	Command cmd = this->GetCommand();
	if (cmd == Command::Invalid)
	{
		ss << ERRORPREFIX << "no command specified";
		this->GetLog()->WriteStdErr(ss.str());
		return false;
	}

	// in all other cases we need the "source" argument
	if (this->GetCZIFilename().empty())
	{
		ss << ERRORPREFIX << "no source file specified";
		this->GetLog()->WriteStdErr(ss.str());
		return false;
	}

	if (cmd != Command::PrintInformation)
	{
		auto str = this->MakeOutputFilename(nullptr, nullptr);
		if (str.empty())
		{
			ss << ERRORPREFIX << "no output file specified";
			this->GetLog()->WriteStdErr(ss.str());
			return false;
		}
	}

	// TODO: there is probably more to be checked

	return true;
}

void CCmdLineOptions::Clear()
{
	this->command = Command::Invalid;
	this->useDisplaySettingsFromDocument = true;
	this->calcHashOfResult = false;
	this->drawTileBoundaries = false;
	this->enabledOutputLevels = 0;
	this->useWicJxrDecoder = false;
	this->backGroundColor.r = this->backGroundColor.g = this->backGroundColor.b = numeric_limits<float>::quiet_NaN();
	this->infoLevel = InfoLevel::Statistics;
	this->channelCompositePixelType = libCZI::PixelType::Bgr24;
	this->channelCompositeAlphaValue = 0xff;
}

void CCmdLineOptions::PrintUsage(int switchesCnt, std::function<std::tuple<std::wstring, std::wstring>(int idx)> getSwitch)
{
	static const char* Synopsis1 =
		"usage: CZIcmd -c COMMAND -s SOURCEFILE -o OUTPUTFILE [-p PLANECOORDINATE]";
	static const char* Synopsis2 =
		"                 [-r ROI] [-d DISPLAYSETTINGS] [-h] [-b] [-t] [-j DECODERNAME] ";
	static const char* Synopsis3 =
		"                 [-v VERBOSITYLEVEL] [-y PYRAMIDINFO] [-z ZOOM] [-i INFOLEVEL]";
	static const char* Synopsis4 =
		"                 [-e SELECTION] [-f FILTER] [-p CHANNELCOMPOSITIONFORMAT]";
	this->GetLog()->WriteStdOut(Synopsis1);
	this->GetLog()->WriteStdOut(Synopsis2);
	this->GetLog()->WriteStdOut(Synopsis3);
	this->GetLog()->WriteStdOut(Synopsis4);
	this->GetLog()->WriteStdOut("");
	stringstream ss;
	int majorVersion, minorVersion;
	libCZI::GetLibCZIVersion(&majorVersion, &minorVersion);
	ss << "  using libCZI version " << majorVersion << "." << minorVersion;
	this->GetLog()->WriteStdOut(ss.str());
	this->GetLog()->WriteStdOut("");

	static const struct
	{
		const wchar_t* option;
		const wchar_t* argument;
		const wchar_t* explanation;
	} OptionAndExplanation[] =
	{
		{
			L"?",
			L"",
			LR"(Show this help message and exit.)"
		},
		{
			L"c",
			L"COMMAND",
			LR"(COMMAND can be any of 'PrintInformation', 'ExtractSubBlock', 'SingleChannelTileAccessor', 'ChannelComposite', 
                'SingleChannelPyramidTileAccessor', 'SingleChannelScalingTileAccessor', 'ScalingChannelComposite' and 'ExtractAttachment'. 
				\n'PrintInformation' will print information about the CZI-file to the console. The argument 'info-level' can be used
				to specify which information is to be printed. 
                \n'ExtractSubBlock' will write the bitmap contained in the specified sub-block to the OUTPUTFILE. 
				\n'ChannelComposite' will create a 
				channel-composite of the specified region and plane and apply display-settings to it. The resulting bitmap will be written
				to the specified OUTPUTFILE. 
				\n'SingleChannelTileAccessor' will create a tile-composite (only from sub-blocks on pyramid-layer 0) of the specified region and plane.
                The resulting bitmap will be written to the specified OUTPUTFILE. 
				\n'SingleChannelPyramidTileAccessor' adds to the previous command the ability to explictely address a specific pyramid-layer (which must
				exist in the CZI-document). 
				\n'SingleChannelScalingTileAccessor' gets the specified region with an arbitrary zoom factor. It uses the pyramid-layers in the CZI-document
				and scales the bitmap if neccessary. The resulting bitmap will be written to the specified OUTPUTFILE. 
				\n'ScalingChannelComposite' operates like the previous command, but in addition gets all channels and creates a multi-channel-composite from them
                using display-settings.
				\n'ExtractAttachment' allows to extract (and save to a file) the contents of attachments.)"
		},
		{
			L"s",
			L"SOURCEFILE",
			LR"(SOURCEFILE specifies the source CZI-file.)"
		},
		{
			L"p",
			L"PLANE-COORDINATES",
			LR"(Uniquely select a 2D-plane from the document. It is given in the form [DimChar][number], where 'DimChar' specifies a dimension and
                can be any of 'Z', 'C', 'T', 'R', 'I', 'H', 'V' or 'B'. 'number' is an integer. \nExamples: C1T3, C0T-2, C1T44Z15H1.
			)"
		},
		{
			L"r",
			L"ROI",
			LR"(Select a paraxial rectangular region as the region-of-interest. The coordinates may be given either absolute or relative. If using relative
			    coordinates, they are relative to what is determined as the upper-left point in the document. \nRelative coordinates are specified with
                the syntax 'rel([x],[y],[width],[height])', absolute coordinates are specified 'abs([x],[y],[width],[height])'.
                \nExamples: rel(0,0,1024,1024), rel(-100,-100,500,500), abs(-230,100,800,800).
			)"
		},
		{
			L"o",
			L"OUTPUTFILE",
			LR"(OUTPUTFILE specifies the output-filename. A suffix will be appended to the name given here depending on the type of the file.)"
		},
		{
			L"d",
			L"DISPLAYSETTINGS",
			LR"(Specifies the display-settings used for creating a channel-composite. The data is given in JSON-notation.)"
		},
		{
			L"h",
			L"",
			LR"(Calculate a hash for the output-picture. The MD5Sum-algorithm is used for this.)"
		},
		{
			L"b",
			L"",
			LR"(Draw a one-pixel black line around each tile.)"
		},
		{
			L"j",
			L"DECODERNAME",
			LR"(Choose which decoder implementation is used. Specifying "WIC" will request the Windows-provided decoder - which 
			is only available on Windows. By default the internal JPG-XR-decoder is used.)"
		},
		{
			L"v",
			L"VERBOSITYLEVEL",
			LR"(Set the verbosity of this program. The argument is a comma- or semicolon-separated list of the
			    following strings: 'All', 'Errors', 'Warnings', 'Infos', 'Errors1', 'Warnings1', 'Infos1', 
				'Errors2', 'Warnings2', 'Infos2'.)"
		},
		{
			L"z",
			L"ZOOM",
			LR"(The zoom-factor (which is used for the commands 'SingleChannelScalingTileAccessor' and 'ScalingChannelComposite'). 
            It is a float between 0 and 1.)"
		},
		{
			L"i",
			L"INFO-LEVEL",
			LR"(When using the command 'PrintInformation' the INFO-LEVEL can be used to specify which information is printed. Possible
			values are "Statistics", "RawXML", "DisplaySettings", "DisplaySettingsJson", "AllSubBlocks", "Attachments", "AllAttachments", 
            "PyramidStatistics", "GeneralInfo", "ScalingInfo" and "All". 
			The values are given as a list separated by comma or semicolon.)"
		},
		{
			L"b",
			L"BACKGROUND",
			LR"(Specify the background color. BACKGROUND is either a single float or three floats, separated by a comma or semicolon. In case of
			a single float, it gives a grayscale value, in case of three floats it gives a RGB-value. The floats are given normalized to a range
			from 0 to 1.)"
		},
		{
			L"y",
			L"PYRAMIDINFO",
			LR"(For the command 'SingleChannelPyramidTileAccessor' the argument PYRAMIDINFO specifies the pyramid layer. It consists of two
			integers (separated by a comma, semicolon or pipe-symbol), where the first specifies the minification-factor (between pyramid-layers) and
			the second the pyramid-layer (starting with 0 for the layer with the highest resolution).)"
		},
		{
			L"e",
			L"SELECTION",
			LR"(For the command 'ExtractAttachment' this allows to specify a subset which is to be extracted (and saved to a file). 
            It is possible to specify the name and the index - only attachments for which the name/index is equal to those values
			specified are processed. The arguments are given in JSON-notation, e.g. {"name":"Thumbnail"} or {"index":3.0}.)"
		},
		{
			L"f",
			L"FILTER",
			LR"(Specify to filter subblocks according to the scene-index. A comma seperated list of either an interval or a single
			integer may be given here, e.g. "2,3" or "2-4,6" or "0-3,5-8".)"
		},
		{
			L"m",
			L"CHANNELCOMPOSITIONFORMAT",
			LR"_(In case of a channel-composition, specifies the pixeltype of the output. Possible values are "bgr24" (the default) and "bgra32".
			If specifying "bgra32" it is possible to give the value of the alpha-pixels in the form "bgra32(128)" - for an alpha-value of 128.)_"
		}
	};

	this->PrintSynopsis(switchesCnt, getSwitch,
		[&](const wchar_t* option)->std::tuple<std::wstring, std::wstring>
	{
		for (size_t i = 0; i < sizeof(OptionAndExplanation) / sizeof(OptionAndExplanation[0]); ++i)
		{
			if (__wcasecmp(OptionAndExplanation[i].option, option))
			{
				return make_tuple(wstring(OptionAndExplanation[i].argument), wstring(OptionAndExplanation[i].explanation));
			}
		}

		return make_tuple(wstring(), wstring());
	});
}

void CCmdLineOptions::PrintSynopsis(int switchesCnt, std::function<std::tuple<std::wstring, std::wstring>(int idx)> getSwitch, std::function<std::tuple<std::wstring, std::wstring>(const wchar_t* option)> getExplanation)
{
	const int COLUMN_FOR_EXPLANATION = 22;// 24;

	wchar_t arg[2];
	arg[1] = L'\0';
	for (int idx = 0; idx < switchesCnt; ++idx)
	{
		wstringstream ss;
		auto argswitch = getSwitch(idx);	// 1st is short, 2nd is long switch
		arg[0] = get<0>(argswitch).at(0);
		auto expl = getExplanation(arg);
		if (get<0>(expl).empty())
		{
			ss << L"  " << L'-' << get<0>(argswitch) << L", --" << get<1>(argswitch);
		}
		else
		{
			ss << L"  " << L'-' << get<0>(argswitch) << L" " << get<0>(expl) << L", --" << get<1>(argswitch) << L" " << get<0>(expl);
		}

		if (!get<1>(expl).empty())
		{
			wstring prefix;
			if (ss.str().size() < COLUMN_FOR_EXPLANATION - 3)
			{
				prefix = ss.str() + wstring(COLUMN_FOR_EXPLANATION - ss.str().size(), L' ');
			}
			else
			{
				this->GetLog()->WriteStdOut(ss.str());
				prefix = wstring(COLUMN_FOR_EXPLANATION, L' ');
			}

			// subtract 1 in order not to run into trouble if outputing a complete line (80 chars normally), where in the end we get two linefeeds...
			auto lines = wrap(get<1>(expl).c_str(), 80 - COLUMN_FOR_EXPLANATION - 1);
			bool isFirstLine = true;
			for (const auto& l : lines)
			{
				auto line = prefix + l;
				this->GetLog()->WriteStdOut(line);
				if (isFirstLine == true)
				{
					prefix = wstring(COLUMN_FOR_EXPLANATION, L' ');
					isFirstLine = false;
				}
			}
		}
		else
		{
			this->GetLog()->WriteStdOut(ss.str());
		}
	}

}

bool CCmdLineOptions::IsLogLevelEnabled(int level) const
{
	if (level < 0)
	{
		level = 0;
	}
	else if (level > 31)
	{
		level = 31;
	}

	return (this->enabledOutputLevels & (1 << level)) ? true : false;;
}

libCZI::CDimCoordinate CCmdLineOptions::ParseDimCoordinate(const std::wstring& str)
{
	return libCZI::CDimCoordinate::Parse(convertToUtf8(str).c_str());
}

libCZI::CDimCoordinate CCmdLineOptions::ParseDimCoordinate(const std::string& s)
{
	return libCZI::CDimCoordinate::Parse(s.c_str());
}

/*static*/Command CCmdLineOptions::ParseCommand(const wchar_t* s)
{
	static const struct
	{
		const wchar_t* cmdName;
		Command		   command;
	} CmdNamesAndCmd[] =
	{
		{ L"PrintInformation",					Command::PrintInformation },
		{ L"ExtractSubBlock",					Command::ExtractSubBlock },
		{ L"SingleChannelTileAccessor",			Command::SingleChannelTileAccessor },
		{ L"ChannelComposite",					Command::ChannelComposite },
		{ L"SingleChannelPyramidTileAccessor",	Command::SingleChannelPyramidTileAccessor },
		{ L"SingleChannelScalingTileAccessor",  Command::SingleChannelScalingTileAccessor },
		{ L"ScalingChannelComposite",			Command::ScalingChannelComposite },
		{ L"ExtractAttachment",                 Command::ExtractAttachment},
	};

	for (size_t i = 0; i < sizeof(CmdNamesAndCmd) / sizeof(CmdNamesAndCmd[0]); ++i)
	{
		if (__wcasecmp(s, CmdNamesAndCmd[i].cmdName))
		{
			return CmdNamesAndCmd[i].command;
		}
	}

	throw std::invalid_argument("Invalid command.");
}

void CCmdLineOptions::ParseRect(const std::wstring& s)
{
	int x, y, w, h;
	bool absOrRel;

	std::wregex rect_regex(LR"(((abs|rel)\(([\+|-]?[[:digit:]]+),([\+|-]?[[:digit:]]+)),([\+]?[[:digit:]]+),([\+]?[[:digit:]]+)\))");
	std::wsmatch pieces_match;

	if (std::regex_match(s, pieces_match, rect_regex))
	{
		if (pieces_match.size() == 7)
		{
			std::wssub_match sub_match = pieces_match[2];
			if (sub_match.compare(L"abs") == 0)
			{
				absOrRel = true;
			}
			else
			{
				absOrRel = false;
			}

			x = std::stoi(pieces_match[3]);
			y = std::stoi(pieces_match[4]);
			w = std::stoi(pieces_match[5]);
			h = std::stoi(pieces_match[6]);

			this->rectModeAbsoluteOrRelative = absOrRel;
			this->rectX = x;
			this->rectY = y;
			this->rectW = w;
			this->rectH = h;
			return;
		}
	}

	throw std::invalid_argument("Invalid rect");
}

void CCmdLineOptions::SetOutputFilename(const std::wstring& s)
{
#if defined(WIN32ENV)
	wchar_t drive[_MAX_DRIVE];
	wchar_t dir[_MAX_DIR];
	wchar_t fname[_MAX_FNAME];
	wchar_t ext[_MAX_EXT];

	_wsplitpath_s(s.c_str(), drive, dir, fname, ext);

	wchar_t path[_MAX_PATH];
	_wmakepath_s(path, _MAX_PATH, drive, dir, L"", L"");
	this->outputPath = path;
	this->outputFilename = fname;
	this->outputFilename += ext;
#endif
#if defined(LINUXENV)
	std::string sutf8 = convertToUtf8(s);
	char* dirName = strdup(sutf8.c_str());
	char* fname = strdup(sutf8.c_str());

	// 'dirname' might modify the string passed in, it might also return a pointer to internally allocated memory
	char* dirNameResult = dirname(dirName);
	char* filename = basename(fname);
	this->outputPath = convertUtf8ToUCS2(dirNameResult);
	this->outputPath += L'/';
	this->outputFilename = convertUtf8ToUCS2(filename);
	free(dirName);
	free(fname);
#endif
}

std::wstring CCmdLineOptions::MakeOutputFilename(const wchar_t* suffix, const wchar_t* extension) const
{
	std::wstring out;
	out += this->outputPath;
	out += this->outputFilename;
	if (suffix != nullptr)
	{
		out += suffix;
	}

	if (extension != nullptr)
	{
		out += L'.';
		out += extension;
	}

	return out;
}

void CCmdLineOptions::ParseDisplaySettings(const std::wstring& s)
{
	auto str = convertToUtf8(s);
	this->ParseDisplaySettings(str);
}

static std::vector<std::tuple<double, double>> ParseSplintPoints(const rapidjson::Value& v)
{
	if (!v.IsArray())
	{
		throw std::logic_error("Invalid JSON");
	}

	std::vector<std::tuple<double, double>> result;

	for (rapidjson::Value::ConstValueIterator it = v.Begin(); it != v.End(); ++it)
	{
		double d1 = it->GetDouble();
		++it;
		if (it == v.End())
			break;
		double d2 = it->GetDouble();
		result.push_back(make_tuple(d1, d2));
	}

	return result;
}

static std::tuple<int, ChannelDisplaySettings> GetChannelInfo(const rapidjson::Value& v)
{
	if (v.HasMember("ch") == false)
	{
		throw std::logic_error("Invalid JSON");
	}

	int chNo = v["ch"].GetInt();
	ChannelDisplaySettings chInfo; chInfo.Clear();
	if (v.HasMember("black-point"))
	{
		chInfo.blackPoint = (float)v["black-point"].GetDouble();
	}
	else
	{
		chInfo.blackPoint = 0;
	}

	if (v.HasMember("white-point"))
	{
		chInfo.whitePoint = (float)v["white-point"].GetDouble();
	}
	else
	{
		chInfo.whitePoint = 1;
	}

	if (v.HasMember("weight"))
	{
		chInfo.weight = (float)v["weight"].GetDouble();
	}
	else
	{
		chInfo.weight = 1;
	}

	if (v.HasMember("tinting"))
	{
		if (v["tinting"].IsString())
		{
			const auto& str = trim(v["tinting"].GetString());
			if (icasecmp(str, "none"))
			{
				chInfo.enableTinting = false;
			}
			else if (str.size() > 1 && str[0] == '#')
			{
				std::uint8_t r, g, b;
				r = g = b = 0;
				for (size_t i = 1; i < (std::max)((size_t)7, (size_t)str.size()); ++i)
				{
					if (!isxdigit(str[i]))
					{
						throw std::logic_error("Invalid JSON");
					}

					switch (i)
					{
					case 1: r = HexCharToInt(str[i]); break;
					case 2: r = (r << 4) + HexCharToInt(str[i]); break;
					case 3: g = HexCharToInt(str[i]); break;
					case 4: g = (g << 4) + HexCharToInt(str[i]); break;
					case 5: b = HexCharToInt(str[i]); break;
					case 6: b = (b << 4) + HexCharToInt(str[i]); break;
					}
				}

				chInfo.tinting.color.r = r;
				chInfo.tinting.color.g = g;
				chInfo.tinting.color.b = b;
				chInfo.enableTinting = true;
			}
		}
	}

	if (v.HasMember("gamma"))
	{
		chInfo.gamma = (float)v["gamma"].GetDouble();
	}

	if (!chInfo.IsGammaValid())
	{
		if (v.HasMember("splinelut"))
		{
			chInfo.splinePoints = ParseSplintPoints(v["splinelut"]);
		}
	}

	return std::make_tuple(chNo, chInfo);
}

void CCmdLineOptions::ParseDisplaySettings(const std::string& s)
{
	// TODO: provide a reasonable error handling
	vector<std::tuple<int, ChannelDisplaySettings>> vecChNoAndChannelInfo;
	rapidjson::Document document;
	document.Parse(s.c_str());
	if (document.HasParseError())
	{
		throw std::logic_error("Invalid JSON");
	}

	bool isObj = document.IsObject();
	bool hasChannels = document.HasMember("channels");
	bool isChannelsArray = document["channels"].IsArray();
	const auto& channels = document["channels"];
	for (decltype(channels.Size()) i = 0; i < channels.Size(); ++i)
	{
		vecChNoAndChannelInfo.emplace_back(GetChannelInfo(channels[i]));
	}

	for (const auto& it : vecChNoAndChannelInfo)
	{
		this->multiChannelCompositeChannelInfos[get<0>(it)] = get<1>(it);
	}
}

std::uint32_t CCmdLineOptions::ParseVerbosityLevel(const wchar_t* s)
{
	static const struct
	{
		const wchar_t* name;
		std::uint32_t flags;
	} Verbosities[] =
	{
		{ L"All",0xffffffff} ,
		{ L"Errors",(1 << 0) | (1 << 1)},
		{ L"Errors1",(1 << 0) },
		{ L"Errors2",(1 << 1) },
		{ L"Warnings",(1 << 2) | (1 << 3) },
		{ L"Warnings1",(1 << 2)  },
		{ L"Warnings2",(1 << 3) },
		{ L"Infos",(1 << 4) | (1 << 5) },
		{ L"Infos1",(1 << 4)  },
		{ L"Infos2",(1 << 5) }
	};

	std::uint32_t levels = 0;
	static const wchar_t* Delimiters = L",;";

	for (;;)
	{
		size_t length = wcscspn(s, L",;");
		if (length == 0)
			break;

		std::wstring tk(s, length);
		std::wstring tktr = trim(tk);
		if (tktr.length() > 0)
		{
			for (size_t i = 0; i < sizeof(Verbosities) / sizeof(Verbosities[0]); ++i)
			{
				if (__wcasecmp(Verbosities[i].name, tktr.c_str()))
				{
					levels |= Verbosities[i].flags;
					break;
				}
			}
		}

		if (*(s + length) == L'\0')
			break;

		s += (length + 1);
	}

	return levels;
}

void CCmdLineOptions::ParseInfoLevel(const wchar_t* s)
{
	static const struct
	{
		const wchar_t* name;
		InfoLevel flag;
	} Verbosities[] =
	{
		{ L"Statistics", InfoLevel::Statistics },
		{ L"RawXML", InfoLevel::RawXML },
		{ L"DisplaySettings", InfoLevel::DisplaySettings },
		{ L"DisplaySettingsJson", InfoLevel::DisplaySettingsJson },
		{ L"AllSubBlocks", InfoLevel::AllSubBlocks },
		{ L"Attachments", InfoLevel::AttachmentInfo },
		{ L"AllAttachments", InfoLevel::AllAttachments },
		{ L"PyramidStatistics", InfoLevel::PyramidStatistics },
		{ L"GeneralInfo", InfoLevel::GeneralInfo },
		{ L"ScalingInfo", InfoLevel::ScalingInfo },
		{ L"All", InfoLevel::All }
	};

	std::underlying_type<InfoLevel>::type  levels = (std::underlying_type<InfoLevel>::type)InfoLevel::None;
	static const wchar_t* Delimiters = L",;";

	for (;;)
	{
		size_t length = wcscspn(s, L",;");
		if (length == 0)
			break;

		std::wstring tk(s, length);
		std::wstring tktr = trim(tk);
		if (tktr.length() > 0)
		{
			for (size_t i = 0; i < sizeof(Verbosities) / sizeof(Verbosities[0]); ++i)
			{
				if (__wcasecmp(Verbosities[i].name, tktr.c_str()))
				{
					levels |= (std::underlying_type<InfoLevel>::type)Verbosities[i].flag;
					break;
				}
			}
		}

		if (*(s + length) == L'\0')
			break;

		s += (length + 1);
	}

	this->infoLevel = (InfoLevel)levels;
}

bool CCmdLineOptions::ParseJxrCodec(const wchar_t* s)
{
	wstring str = trim(wstring(s));
	if (__wcasecmp(str.c_str(), L"WIC") || __wcasecmp(str.c_str(), L"WICDecoder"))
	{
		return true;
	}

	return false;
}

libCZI::RgbFloatColor CCmdLineOptions::ParseBackgroundColor(const wchar_t* s)
{
	// TODO: somewhat stricter parsing...
	float f[3];
	f[0] = f[1] = f[2] = std::numeric_limits<float>::quiet_NaN();
	for (int i = 0; i < 3; ++i)
	{
		wchar_t* endPtr;
		f[i] = wcstof(s, &endPtr);

		const wchar_t* endPtrSkipped = skipWhiteSpaceAndOneOfThese(endPtr, L";,|");
		if (*endPtrSkipped == L'\0')
			break;
	}

	if (isnan(f[1]) && isnan(f[2]))
	{
		return libCZI::RgbFloatColor{ f[0],f[0],f[0] };
	}

	return libCZI::RgbFloatColor{ f[0],f[1],f[2] };
}

void CCmdLineOptions::ParsePyramidInfo(const wchar_t* sz)
{
	int minificationFactor, layerNo;
	const wchar_t* endPtr;
	minificationFactor = wcstol(sz, (wchar_t**)&endPtr, 10);
	sz = skipWhiteSpaceAndOneOfThese(endPtr, L";,|");
	if (*sz == L'\0')
	{
		throw std::logic_error("Invalid pyramidinfo argument");
	}

	layerNo = wcstol(sz, (wchar_t**)&endPtr, 10);

	// TODO: check arguments...
	this->pyramidLayerNo = layerNo;
	this->pyramidMinificationFactor = minificationFactor;
}

void CCmdLineOptions::ParseZoom(const wchar_t* sz)
{
	// TODO: error handling
	float zoom = stof(sz);
	this->zoom = zoom;
}

void CCmdLineOptions::ParseSelection(const std::wstring& s)
{
	auto str = convertToUtf8(s);
	this->ParseSelection(str);
}

void CCmdLineOptions::ParseSelection(const std::string& s)
{
	std::map<string, ItemValue> map;
	rapidjson::Document document;
	document.Parse(s.c_str());
	if (document.HasParseError() || !document.IsObject())
	{
		throw std::logic_error("Invalid JSON");
	}

	for (rapidjson::Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
	{
		if (!itr->name.IsString())
		{
			throw std::logic_error("Invalid JSON");
		}

		string name = itr->name.GetString();
		ItemValue iv;
		if (itr->value.IsString())
		{
			iv = ItemValue(itr->value.GetString());
		}
		else if (itr->value.IsDouble())
		{
			iv = ItemValue(itr->value.GetDouble());
		}
		else if (itr->value.IsBool())
		{
			iv = ItemValue(itr->value.GetBool());
		}
		else
		{
			throw std::logic_error("Invalid JSON");
		}

		map[name] = iv;
	}

	std::swap(this->mapSelection, map);
}

ItemValue CCmdLineOptions::GetSelectionItemValue(const char* sz) const
{
	const auto it = this->mapSelection.find(sz);
	if (it != this->mapSelection.end())
	{
		return it->second;
	}

	return ItemValue();
}

void CCmdLineOptions::ParseTileFilter(const wchar_t* s)
{
	this->sceneIndexSet = libCZI::Utils::IndexSetFromString(s);
}

std::shared_ptr<libCZI::IIndexSet> CCmdLineOptions::GetSceneIndexSet() const
{
	return this->sceneIndexSet;
}

void CCmdLineOptions::ParseChannelCompositionFormat(const wchar_t* s)
{
	auto arg = trim(s);
	if (__wcasecmp(arg.c_str(), L"bgr24"))
	{
		this->channelCompositePixelType = libCZI::PixelType::Bgr24;
		return;
	}
	else if (__wcasecmp(arg.c_str(), L"bgra32"))
	{
		this->channelCompositePixelType = libCZI::PixelType::Bgra32;
		this->channelCompositeAlphaValue = 0xff;
		return;
	}
	else if (TryParseChannelCompositionFormatWithAlphaValue(arg, this->channelCompositePixelType, this->channelCompositeAlphaValue))
	{
		return;
	}

	throw std::invalid_argument("Invalid channel-composition-format.");
}

/*static*/bool CCmdLineOptions::TryParseChannelCompositionFormatWithAlphaValue(const std::wstring& s, libCZI::PixelType& channelCompositePixelType, std::uint8_t& channelCompositeAlphaValue)
{
	std::wregex regex(LR"_(bgra32\((\d+|0x[\d|a-f|A-F]+)\))_", regex_constants::ECMAScript | regex_constants::icase);
	std::wsmatch pieces_match;
	if (std::regex_match(s, pieces_match, regex))
	{
		if (pieces_match.size() == 2)
		{
			std::wssub_match sub_match = pieces_match[1];
			if (sub_match.length() > 2)
			{
				if (sub_match.str()[0] == L'0' && (sub_match.str()[1] == L'x' || sub_match.str()[0] == L'X'))
				{
					auto hexStr = convertToUtf8(sub_match);
					std::uint32_t value;
					if (!ConvertHexStringToInteger(hexStr.c_str() + 2, &value) || value > 0xff)
					{
						return false;;
					}

					channelCompositePixelType = libCZI::PixelType::Bgra32;
					channelCompositeAlphaValue = static_cast<std::uint8_t>(value);
					return true;
				}
			}

			int i = std::stoi(sub_match);
			if (i < 0 || i > 255)
			{
				return false;
			}

			channelCompositePixelType = libCZI::PixelType::Bgra32;
			channelCompositeAlphaValue = static_cast<std::uint8_t>(i);
			return true;
		}
	}

	return false;
}