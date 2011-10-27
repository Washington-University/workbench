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

using namespace caret;
using namespace std;

void CiftiXnat::openURL(const AString& url) throw (CiftiFileException)
{
    m_baseRequest.m_url = url;
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(m_baseRequest, myResponse);
    if (!myResponse.m_ok)
    {
        throw CiftiFileException("Error opening URL, response code: " + AString::number(myResponse.m_responseCode));
    }
    myResponse.m_body.push_back('\0');//null terminate it so we can construct an AString easily - CaretHttpManager is nice and pre-reserves this room for this purpose
    AString theBody(myResponse.m_body.data());
    m_theXml.readXML(theBody);
    m_rowSize = 0;
    m_colSize = 0;
    CiftiRootElement myRoot;
    m_theXml.getXMLRoot(myRoot);
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
        throw CiftiFileException("Error opening URL, row size is unknown");
    }
    if (m_rowSize == 0)
    {
        throw CiftiFileException("Error opening URL, column size is unknown");
    }
}

CiftiXnat::CiftiXnat()
{
    m_baseRequest.m_method = CaretHttpManager::POST;
    setAuthBasic("testuser", "testing");//should this be here at all?
}

void CiftiXnat::getCiftiXML(CiftiXML& xml)
{
    xml = m_theXml;
}

void CiftiXnat::getColumn(float* columnOut, const int64_t& columnIndex) throw (CiftiFileException)
{
    CaretHttpRequest columnRequest = m_baseRequest;
    columnRequest.m_arguments.push_back(make_pair(AString("column-index"), AString::number(columnIndex)));
    getReqAsFloats(columnOut, m_colSize, columnRequest);
}

void CiftiXnat::getRow(float* rowOut, const int64_t& rowIndex) throw (CiftiFileException)
{
    CaretHttpRequest rowRequest = m_baseRequest;
    rowRequest.m_arguments.push_back(make_pair(AString("row-index"), AString::number(rowIndex)));
    getReqAsFloats(rowOut, m_rowSize, rowRequest);
}

void CiftiXnat::getReqAsFloats(float* data, const int64_t& dataSize, CaretHttpRequest& request) throw (CiftiFileException)
{
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(request, myResponse);
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

void CiftiXnat::setAuthBasic(const caret::AString& userName, const caret::AString& password)
{
    AString unencoded = userName + ":" + password;
    vector<char> encoded;
    int64_t bytesNeeded = (unencoded.size() / 3 + 1) * 4;//including the end mark
    encoded.reserve(bytesNeeded + 1);//leave room for null terminator
    encoded.resize(bytesNeeded);
    Base64::encode((const unsigned char*)unencoded.c_str(), unencoded.size(), (unsigned char*)encoded.data(), 1);//mark the end, just because
    encoded.push_back('\0');//null terminate
    m_baseRequest.m_arguments.push_back(make_pair(AString("Authorization"), AString("Basic ") + AString(encoded.data())));
}
