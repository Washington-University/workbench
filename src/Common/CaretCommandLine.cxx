/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "CaretCommandLine.h"
#include "ProgramParameters.h"

using namespace caret;

AString caret::caret_global_commandLine;

namespace
{//private namespace
    void add_parameter(const AString& param)
    {
        if (caret_global_commandLine.size() != 0)
        {
            caret_global_commandLine += " ";
        }
        if (param.indexOfAnyChar(" $();&<>\"`*?{|") != -1)//check for things that the shell is likely to treat specially EXCEPT for ' itself - assume bash for now, but ignore some more specialized cases
        {//NOTE: not checking for \ or replacing with \\, because it is rare except in windows native paths where it will wreak havok to double it
            if (param.indexOf('\'') != -1)//oh joy, ' also
            {//we COULD check if it is safe to use "", but "" and non-CDATA xml text don't look nice (we avoid CDATA in CIFTI because the matlab GIFTI toolbox at least used to choke on it after conversion)
                AString replaced = param;
                replaced.replace('\'', "'\\''");//that is '\''
                caret_global_commandLine += "'" + replaced + "'";
            } else {
                caret_global_commandLine += "'" + param + "'";
            }
        } else {
            if (param.indexOf('\'') != -1)//has ' but no other problems, doesn't need quoting
            {
                AString replaced = param;
                replaced.replace('\'', "\\'");//that is \'
                caret_global_commandLine += replaced;
            } else {
                caret_global_commandLine += param;
            }
        }
    }
}

void caret::caret_global_commandLine_init(const ProgramParameters& params)
{
    int32_t numParams = params.getNumberOfParameters();
    caret_global_commandLine = "";
    add_parameter(params.getProgramName());
    for (int32_t i = 0; i < numParams; ++i)
    {
        add_parameter(params.getParameter(i));
    }
}

void caret::caret_global_commandLine_init(const int& argc, const char *const * argv)
{
    ProgramParameters params(argc, argv);
    caret_global_commandLine_init(params);
}
