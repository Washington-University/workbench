/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include "Base64.h"
#include "ByteOrderEnum.h"
#include "ByteSwapping.h"
#include "CiftiXnat.h"

#include <iostream>

using namespace caret;
using namespace std;

void CiftiXnat::openURL(const AString& url) throw (CiftiFileException)
{
    m_baseRequest.m_url = url;
    int32_t start = url.indexOf('?');
    bool foundSearchID = false;
    while (!foundSearchID)
    {
        if (start == -1)
        {
            throw CiftiFileException("Error: searchID not found in URL string");
        }
        if (url.mid(start + 1, 9) == "searchID=")
        {
            foundSearchID = true;
            int32_t end = url.indexOf('&');//if doesn't exist, -1 is also special case to end of string for mid()
            AString fullArg = url.mid(start + 1, end);
            int32_t equalsPos = fullArg.indexOf('=');//should obviously be 8, but this is more readable
            m_baseRequest.m_arguments.push_back(make_pair(AString(fullArg.mid(0, equalsPos)), AString(fullArg.mid(equalsPos + 1, -1))));
        }
        start = url.indexOf('&', start + 1);
    }
    m_baseRequest.m_queries.push_back(make_pair(AString("type"), AString("dconn")));
    m_baseRequest.m_arguments.push_back(make_pair(AString("type"), AString("dconn")));
    CaretHttpRequest metadata = m_baseRequest;
    metadata.m_queries.push_back(make_pair(AString("metadata"), AString("")));
    metadata.m_arguments.push_back(make_pair(AString("metadata"), AString("")));
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(metadata, myResponse);
    if (!myResponse.m_ok)
    {
        throw CiftiFileException("Error opening URL, response code: " + AString::number(myResponse.m_responseCode));
    }
    cout << "passed first request" << endl;
    myResponse.m_body.push_back('\0');//null terminate it so we can construct an AString easily - CaretHttpManager is nice and pre-reserves this room for this purpose
    AString theBody(myResponse.m_body.data());
    //cout << theBody << endl;
    m_xml.readXML(theBody);
    m_rowSize = 0;
    m_colSize = 0;
    CiftiRootElement myRoot;
    m_xml.getXMLRoot(myRoot);
    vector<CiftiMatrixIndicesMapElement>& myMaps = myRoot.m_matrices[0].m_matrixIndicesMap;
    int64_t numMaps = (int64_t)myMaps.size();
    for (int64_t i = 0; i < numMaps; ++i)
    {
        vector<int>& myDimList = myMaps[i].m_appliesToMatrixDimension;
        for (int64_t j = 0; j < (int64_t)myDimList.size(); ++j)
        {
            if (myMaps[i].m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
            {//we have no length info in any other type
                std::vector<CiftiBrainModelElement>& myModels = myMaps[i].m_brainModels;
                for (int64_t k = 0; k < (int64_t)myModels.size(); ++k)
                {
                    if (myDimList[j] == 0)
                    {
                        m_rowSize += myModels[k].m_indexCount;
                    }
                    if (myDimList[j] == 1)
                    {
                        m_colSize += myModels[k].m_indexCount;
                    }
                }
            }
        }
    }
    if (m_rowSize == 0)
    {
        CaretHttpRequest rowRequest = m_baseRequest;
        rowRequest.m_queries.push_back(make_pair(AString("row-index"), AString("0")));
        rowRequest.m_arguments.push_back(make_pair(AString("row-index"), AString("0")));
        m_rowSize = getSizeFromReq(rowRequest);
    }
    if (m_colSize == 0)
    {
        CaretHttpRequest columnRequest = m_baseRequest;
        columnRequest.m_queries.push_back(make_pair(AString("column-index"), AString("0")));
        columnRequest.m_arguments.push_back(make_pair(AString("column-index"), AString("0")));
        m_colSize = getSizeFromReq(columnRequest);
    }
}

CiftiXnat::CiftiXnat()
{
    m_baseRequest.m_method = CaretHttpManager::POST;
}

void CiftiXnat::getColumn(float* columnOut, const int64_t& columnIndex) const throw (CiftiFileException)
{
    CaretHttpRequest columnRequest = m_baseRequest;
    columnRequest.m_queries.push_back(make_pair(AString("column-index"), AString::number(columnIndex)));
    columnRequest.m_arguments.push_back(make_pair(AString("column-index"), AString::number(columnIndex)));
    getReqAsFloats(columnOut, m_colSize, columnRequest);
}

void CiftiXnat::getRow(float* rowOut, const int64_t& rowIndex) const throw (CiftiFileException)
{
    CaretHttpRequest rowRequest = m_baseRequest;
    rowRequest.m_queries.push_back(make_pair(AString("row-index"), AString::number(rowIndex)));
    rowRequest.m_arguments.push_back(make_pair(AString("row-index"), AString::number(rowIndex)));
    getReqAsFloats(rowOut, m_rowSize, rowRequest);
}

void CiftiXnat::getReqAsFloats(float* data, const int64_t& dataSize, CaretHttpRequest& request) const throw (CiftiFileException)
{
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(request, myResponse);
    if (!myResponse.m_ok)
    {
        throw CiftiFileException("Error getting row, response code: " + AString::number(myResponse.m_responseCode));
    }
    if (myResponse.m_body.size() % 4 != 0)//expect a multiple of 4 bytes
    {
        throw CiftiFileException("Bad reply, number of bytes is not a multiple of 4");
    }
    int32_t numItems = *((int32_t*)myResponse.m_body.data());
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(&numItems, 1);
    }
    if (numItems * 4 + 4 != (int64_t)myResponse.m_body.size())
    {
        throw CiftiFileException("Bad reply, number of items does not match length of reply");
    }
    if (dataSize != numItems)
    {
        throw CiftiFileException("Bad reply, number of items does not match header");
    }
    float* myPointer = (float*)(myResponse.m_body.data() + 4);//skip the first element (which is an int32)
    for (int i = 0; i < numItems; ++i)
    {
        data[i] = myPointer[i];
    }
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(data, numItems);
    }
}

void CiftiXnat::setAuthentication(const AString& url, const AString& userName, const AString& password)
{
    CaretHttpManager::setAuthentication(url, userName, password);
}

int64_t CiftiXnat::getSizeFromReq(CaretHttpRequest& request) throw (CiftiFileException)
{
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(request, myResponse);
    if (!myResponse.m_ok)
    {
        throw CiftiFileException("Error getting row, response code: " + AString::number(myResponse.m_responseCode));
    }
    if (myResponse.m_body.size() % 4 != 0)//expect a multiple of 4 bytes
    {
        throw CiftiFileException("Bad reply, number of bytes is not a multiple of 4");
    }
    int32_t numItems = *((int32_t*)myResponse.m_body.data());
    if (ByteOrderEnum::isSystemBigEndian())
    {
        ByteSwapping::swapBytes(&numItems, 1);
    }
    if (numItems * 4 + 4 != (int64_t)myResponse.m_body.size())
    {
        throw CiftiFileException("Bad reply, number of items does not match length of reply");
    }
    return numItems;
}
