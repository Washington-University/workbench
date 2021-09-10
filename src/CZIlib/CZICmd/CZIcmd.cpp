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

// CZICmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "consoleio.h"
#include "cmdlineoptions.h"
#include "execute.h"
#include "inc_libCZI.h"

#if defined(LINUXENV)
#include <clocale>
#endif

class CLibCZISite : public libCZI::ISite
{
	libCZI::ISite* pSite;
	const CCmdLineOptions& options;
public:
	explicit CLibCZISite(const CCmdLineOptions& opts) : options(opts)
	{
#if defined(WIN32ENV)
		if (options.GetUseWICJxrDecoder())
		{
			this->pSite = libCZI::GetDefaultSiteObject(libCZI::SiteObjectType::WithWICDecoder);
		}
		else
		{
			this->pSite = libCZI::GetDefaultSiteObject(libCZI::SiteObjectType::WithJxrDecoder);
		}
#else
		this->pSite = libCZI::GetDefaultSiteObject(libCZI::SiteObjectType::Default);
#endif
	}

	bool IsEnabled(int logLevel) override
	{
		return this->options.IsLogLevelEnabled(logLevel);
	}

	void Log(int /*level*/, const char* szMsg) override
	{
		this->options.GetLog()->WriteStdOut(szMsg);
	}

	std::shared_ptr<libCZI::IDecoder> GetDecoder(libCZI::ImageDecoderType type, const char* arguments) override
	{
		return this->pSite->GetDecoder(type, arguments);
	}

	std::shared_ptr<libCZI::IBitmapData> CreateBitmap(libCZI::PixelType pixeltype, std::uint32_t width, std::uint32_t height, std::uint32_t stride, std::uint32_t extraRows, std::uint32_t extraColumns) override
	{
		return this->pSite->CreateBitmap(pixeltype, width, height, stride, extraRows, extraColumns);
	}
};

//int _tmain(int argc, _TCHAR* argv[])
int czi_main(int argc, char** _argv)
{
#if !defined(__EMSCRIPTEN__)
#if defined(WIN32ENV)
	wchar_t** argv;
	CoInitialize(NULL);
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#endif
#if defined(LINUXENV)
	setlocale(LC_CTYPE, "");
	char** argv = _argv;
#endif

	int retVal = 0;
	auto log = CConsoleLog::CreateInstance();
	try
	{
		CCmdLineOptions options(log);
		bool cmdLineParsedOk = options.Parse(argc, argv);
		if (cmdLineParsedOk == true)
		{
			if (options.GetCommand() != Command::Invalid)
			{
				// Important: We have to ensure that the object passed in here has a lifetime greater than
				// any usage of the libCZI.
				CLibCZISite site(options);
				libCZI::SetSiteObject(&site);

				execute(options);
			}
		}
		else
		{
			log->WriteStdErr("There were errors parsing the arguments -> exiting.");
		}
	}
	catch (std::exception& excp)
	{
		std::stringstream ss;
		ss << "Exception caught -> \"" << excp.what() << "\"";
		log->WriteStdErr(ss.str());
		retVal = 1000;
	}

#if defined(WIN32ENV)
	CoUninitialize();
	LocalFree(argv);
#endif
#endif
	return retVal;
}

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <emscripten/bind.h>
using namespace emscripten;

extern int optind;

int myMain(std::vector<std::string> args)
{
#if defined(LINUXENV)
	setlocale(LC_CTYPE, "");
#endif

	auto log = CConsoleLog::CreateInstance();

	CCmdLineOptions options(log);

	char** argv = (char**)malloc(sizeof(char*)*(1 + args.size()));
	for (int i = 0; i < args.size(); ++i)
	{
		argv[i + 1] = (char*)args[i].c_str();
	}

	char empty[1]; empty[0] = '\0';
	argv[0] = empty;

	options.Parse((int)args.size(), argv);

	// Important: We have to ensure that the object passed in here has a lifetime greater than
	// any usage of the libCZI.
	CLibCZISite* pSite = new CLibCZISite(options);
	try
	{
		libCZI::SetSiteObject(pSite);
	}
	catch (std::logic_error& e)
	{
		delete pSite;
	}

	execute(options);

	free(argv);
	optind = 0;
	return 0;
}

EMSCRIPTEN_BINDINGS(LibCZIModule2) {
	register_vector<std::string>("VectorString");
	function("myMain", &myMain);
	}
#endif
