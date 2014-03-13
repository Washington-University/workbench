#ifndef __CIFTI_XNAT_H__
#define __CIFTI_XNAT_H__

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

#include "CaretHttpManager.h"
#include "CiftiFileException.h"
#include "CiftiInterface.h"

namespace caret
{

    class CiftiXnat : public CiftiInterface
    {
        CaretHttpRequest m_baseRequest;
        int64_t m_numberOfColumns, m_numberOfRows;
        ///get a request as a float array, with error checking
        void getReqAsFloats(float* data, const int64_t& dataSize, CaretHttpRequest& request) const;
        ///get just the size field from a request
        int64_t getSizeFromReq(CaretHttpRequest& request);
    public:
        ///initialize base request to POST type
        CiftiXnat();
        
        ///open a URL with existing auth tokens
        void openURL(const AString& url);
        
        ///set auth tokens with "Basic" method, removing all previously existing tokens
        void setAuthentication(const AString& url, const AString& userName, const AString& password);
        
        ///get a row
        void getRow(float* rowOut,const int64_t& rowIndex) const;
        
        ///get a column
        void getColumn(float* columnOut, const int64_t& columnIndex) const;
        
        ///get row size
        int64_t getNumberOfColumns() const { return m_numberOfColumns; }
        
        ///get column size
        int64_t getNumberOfRows() const { return m_numberOfRows; }
        
    };

}

#endif //__CIFTI_XNAT_H__
