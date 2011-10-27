#ifndef __CIFTI_XNAT_H__
#define __CIFTI_XNAT_H__

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

#include "CaretHttpManager.h"
#include "CiftiXML.h"
#include "CiftiFileException.h"

namespace caret
{

    class CiftiXnat
    {
        CaretHttpRequest m_baseRequest;
        CiftiXML m_theXml;
        int64_t m_rowSize, m_colSize;
        ///get a request as a float array, with error checking
        void getReqAsFloats(float* data, const int64_t& dataSize, CaretHttpRequest& request) throw (CiftiFileException);
    public:
        ///initialize base request to POST type
        CiftiXnat();
        
        ///open a URL with existing auth tokens
        void openURL(const AString& url) throw (CiftiFileException);
        
        ///set auth tokens with "Basic" method, removing all previously existing tokens
        void setAuthBasic(const AString& userName, const AString& password);
        
        ///get the XML data
        void getCiftiXML(CiftiXML &xml);
        
        ///get a row
        void getRow(float* rowOut,const int64_t& rowIndex) throw (CiftiFileException);
        
        ///get a column
        void getColumn(float* columnOut, const int64_t& columnIndex) throw (CiftiFileException);
        
        ///get row size
        int64_t getRowSize() { return m_rowSize; }
        
        ///get column size
        int64_t getColumnSize() { return m_colSize; }
        
    };

}

#endif //__CIFTI_XNAT_H__
