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
#include "CaretLogger.h"

#include <iostream>

using namespace caret;
using namespace std;

/**
 * Open the given URL.
 * 
 * @param url
 *    The URL.
 *
 * Examples:
 *    https://db.humanconnectome.org/data/services/cifti-average?searchID=PILOT1_AVG_xnat:subjectData
 *    http://hcpx-demo.humanconnectome.org/spring/cifti-average?resource=HCP_Q1:Q1:Demo_HCP_unrelated20_FunctionalConnectivity_mgt-regression
 */
void CiftiXnat::openURL(const AString& url) throw (CiftiFileException)
{
    m_baseRequest.m_url = url;
    int32_t start = url.indexOf('?');
    bool foundSearchID = false;
    bool foundResource = false;
    while ((!foundSearchID) && (!foundResource))
    {
        if (start == -1)
        {
            throw CiftiFileException("Error: searchID not found in URL string");
        }
        if (url.mid(start + 1, 9) == "searchID=")
        {
            foundSearchID = true;
        }
        else if (url.mid(start + 1, QString("resource=").length()) == "resource=")
        {
            foundResource = true;
        }
        start = url.indexOf('&', start + 1);
    }
    m_baseRequest.m_queries.push_back(make_pair(AString("type"), AString("dconn")));
    CaretHttpRequest metadata = m_baseRequest;
    if (foundResource)
    {
        metadata.m_queries.push_back(make_pair(AString("metadata"), AString("true")));
    }
    else
    {
        metadata.m_queries.push_back(make_pair(AString("metadata"), AString("")));
    }
    CaretHttpResponse myResponse;
    CaretHttpManager::httpRequest(metadata, myResponse);
    if (!myResponse.m_ok)
    {
        throw CiftiFileException("Error opening URL, response code: " + AString::number(myResponse.m_responseCode));
    }
    myResponse.m_body.push_back('\0');//null terminate it so we can construct an AString easily - CaretHttpManager is nice and pre-reserves this room for this purpose
    AString theBody(myResponse.m_body.data());
    m_xml.readXML(theBody);
    bool fixedDims = false;
    m_numberOfColumns = m_xml.getNumberOfColumns();
    m_numberOfRows = m_xml.getNumberOfRows();
    if (m_xml.getColumnMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS && m_numberOfColumns < 1)
    {
        CaretHttpRequest rowRequest = m_baseRequest;
        rowRequest.m_queries.push_back(make_pair(AString("row-index"), AString("0")));
        m_numberOfColumns = getSizeFromReq(rowRequest);
        m_xml.setRowNumberOfTimepoints(m_numberOfColumns);//number of timepoints along a row is the number of columns
        fixedDims = true;
    }
    if (m_xml.getColumnMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS && m_numberOfRows < 1)
    {
        CaretHttpRequest columnRequest = m_baseRequest;
        columnRequest.m_queries.push_back(make_pair(AString("column-index"), AString("0")));
        m_numberOfRows = getSizeFromReq(columnRequest);
        m_xml.setColumnNumberOfTimepoints(m_numberOfRows);//see above
        fixedDims = true;
    }
    if (fixedDims && m_xml.getVersion() != CiftiVersion(1, 0))
    {
        CaretLogWarning("fixed missing time points dimension in version \"" + m_xml.getVersion().toString() + "\" cifti xml");
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
    getReqAsFloats(columnOut, m_numberOfRows, columnRequest);
}

void CiftiXnat::getRow(float* rowOut, const int64_t& rowIndex) const throw (CiftiFileException)
{
    CaretHttpRequest rowRequest = m_baseRequest;
    rowRequest.m_queries.push_back(make_pair(AString("row-index"), AString::number(rowIndex)));
    getReqAsFloats(rowOut, m_numberOfColumns, rowRequest);
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
