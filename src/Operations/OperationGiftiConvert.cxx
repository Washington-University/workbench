/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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

#include "OperationGiftiConvert.h"
#include "OperationException.h"

#include "GiftiFile.h"

using namespace caret;
using namespace std;

AString OperationGiftiConvert::getCommandSwitch()
{
    return "-gifti-convert";
}

AString OperationGiftiConvert::getShortDescription()
{
    return "CONVERT A GIFTI FILE TO A DIFFERENT ENCODING";
}

OperationParameters* OperationGiftiConvert::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "gifti-encoding", "what the output encoding should be");
    
    ret->addStringParameter(2, "input-gifti-file", "the input gifti file");
    
    ret->addStringParameter(3, "output-gifti-file", "output - the output gifti file");//HACK: fake the output formatting
    
    ret->setHelpText(
        AString("The value of <gifti-encoding> must be one of the following:\n\n") +
        "ASCII\nBASE64_BINARY\nGZIP_BASE64_BINARY\nEXTERNAL_FILE_BINARY"
    );
    return ret;
}

void OperationGiftiConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString encodingStr = myParams->getString(1);
    AString inputName = myParams->getString(2);
    AString outputName = myParams->getString(3);
    bool isValidEncoding = false;
    GiftiEncodingEnum::Enum encoding = GiftiEncodingEnum::fromName(encodingStr, &isValidEncoding);
    if (isValidEncoding == false) {
        throw OperationException("Specified GIFTI Encoding is invalid.");
    }
    if (inputName.isEmpty()) {
        throw OperationException("Input GIFTI file name is empty.");
    }
    if (outputName.isEmpty()) {
        throw OperationException("Output GIFTI file name is empty.");
    }
    GiftiFile gf;
    gf.readFile(inputName);
    gf.setEncodingForWriting(encoding);
    gf.writeFile(outputName);
}
