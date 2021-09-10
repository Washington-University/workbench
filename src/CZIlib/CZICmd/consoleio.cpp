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
#include <cstdarg>
#include "consoleio.h"
#include <iostream>

using namespace std;

//const wchar_t LINEFEED[] = L"\r\n";

/*static*/std::shared_ptr<ILog> CConsoleLog::CreateInstance()
{
	return std::make_shared<CConsoleLog>();
}

void CConsoleLog::WriteStdOut(const char* sz)
{
	std::cout << sz << endl;
}

void CConsoleLog::WriteStdOut(const wchar_t* sz)
{
	std::wcout << sz << endl;
}


void CConsoleLog::WriteStdErr(const char* sz)
{
	std::cout << sz << endl;
}

void CConsoleLog::WriteStdErr(const wchar_t* sz)
{
	std::wcout << sz << endl;
}


#if 0
void CConsoleLog::WriteStdOut(const wchar_t* fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	this->Write(stdout, fmt, argptr);
	va_end(argptr);
}

void CConsoleLog::WriteStdOutLine(const wchar_t* fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	this->Write(stdout, fmt, argptr);
	this->WriteString(stdout, LINEFEED);
	va_end(argptr);
}

void CConsoleLog::WriteStdErr(const wchar_t* fmt, ...)
{
	va_list argptr; /* Set up the variable argument list here */
	va_start(argptr, fmt); /* Start up variable arguments */
	this->Write(stderr, fmt, argptr);
	va_end(argptr);
}

bool CConsoleLog::IsInfoEnabled(int level) const
{
	return true;
}

void CConsoleLog::WriteInfo(int level, const wchar_t* fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);
	this->Write(stdout, fmt, argptr);
	va_end(argptr);
}

void CConsoleLog::WriteInfoString(int level, const wchar_t* str)
{
	fputws(str, stdout);
}

void CConsoleLog::Write(FILE* fp, const wchar_t* fmt, va_list argptr)
{
#if defined(WIN32ENV)
	vfwprintf_s(fp, fmt, argptr);
#endif
#if defined(LINUXENV)
    vfwprintf(fp,fmt,argptr);
#endif
}

void CConsoleLog::WriteString(FILE* fp, const wchar_t* s)
{
	fputws(s, fp);
}
#endif
