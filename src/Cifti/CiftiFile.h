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

#ifndef __CIFTI_FILE
#define __CIFTI_FILE
#include <QtCore>
#include "iostream"
#include "CiftiFileException.h"
#include "CiftiHeaderIO.h"
#include "CiftiInterface.h"
#include "CiftiXML.h"
#include "CiftiMatrix.h"

/// Class for opening, reading, and writing generic Cifti Data

namespace caret {

class CiftiFile : public CiftiInterface {
public:
    /// Constructor
    CiftiFile();
    /// Constructor
    CiftiFile(const CacheEnum &caching, const AString &cacheFile = NULL);
    /// Constructor
    CiftiFile(const AString &fileName, const CacheEnum &caching = IN_MEMORY, const AString &cacheFile = NULL);
    /// Open the Cifti File
    virtual void openFile(const AString &fileName, const CacheEnum &caching = IN_MEMORY);
    /// Set Cifti Cache File, must be called BEFORE open file, or set in the constructor
    virtual void setCiftiCacheFile(const AString &cacheFile);
    /// Write the Cifti File
    virtual void writeFile(const AString &fileName);
    
    //check if it is in memory or not
    bool isInMemory() const;
    ///convert to in-memory file
    void convertToInMemory();

    //get/set Nifti2/CiftiHeader
    /// set CiftiHeader
    //virtual void setHeader(const CiftiHeader &header) throw (CiftiFileException);
    /// get CiftiHeader
    virtual void getHeader(CiftiHeader &header);

    //TODO, put some thought into whether we want to hand back an xml tree vs handing back a class that manages
    //the tree in an intelligent way.
    /// set CiftiXML
    virtual void setCiftiXML(const CiftiXML &xml, const bool useOldMetadata = true, const CiftiVersion& writingVersion = CiftiVersion());
    /// set CiftiXML with old structure
    virtual void setCiftiXML(const CiftiXMLOld &xml, const bool useOldMetadata = true, const CiftiVersion& writingVersion = CiftiVersion());

    // Matrix IO, simply passes through to underlying Cifti Matrix
    /// get Row
    void getRow(float * rowOut,const int64_t &rowIndex, const bool& tolerateShortRead) const
    { m_matrix.getRow(rowOut, rowIndex, tolerateShortRead); }
    /// get Row
    void getRow(float * rowOut,const int64_t &rowIndex) const
    { m_matrix.getRow(rowOut, rowIndex); }
    /// set Row
    void setRow(float * rowIn, const int64_t &rowIndex)
    {
        invalidateDataRange();
        m_matrix.setRow(rowIn, rowIndex);
    }
    /// get Column
    void getColumn(float * columnOut, const int64_t &columnIndex) const
    { m_matrix.getColumn(columnOut, columnIndex); }
    /// set Column
    void setColumn(float * columnIn, const int64_t &columnIndex)
    {
        invalidateDataRange();
        m_matrix.setColumn(columnIn, columnIndex);
    }
    /// get Matrix
    void getMatrix(float *matrixOut)
    { m_matrix.getMatrix(matrixOut); }
    /// set Matrix
    void setMatrix(float *matrixIn)
    {
        invalidateDataRange();
        m_matrix.setMatrix(matrixIn);
    }
    // setup Matrix
    //void setupMatrix(vector<int64_t> &dimensions, const int64_t &offsetIn = 0, const CacheEnum &e=IN_MEMORY, const bool &needsSwapping=false) throw (CiftiFileException);
    /// set timestep
    bool setRowTimestep(const float& seconds);
    bool setColumnTimestep(const float& seconds);
    
    ///get row size
    int64_t getNumberOfColumns() const;
    
    ///get column size
    int64_t getNumberOfRows() const;

    /// Destructor
    virtual ~CiftiFile();
protected:
    /// setup Matrix
    void setupMatrix();
    
    virtual void init();
    
    CiftiVersion m_writingVersion;
    QByteArray m_xmlBytes;
    
    AString m_fileName;
    AString m_cacheFileName;
    CiftiHeaderIO m_headerIO;
    CiftiMatrix m_matrix;
    bool m_swapNeeded;
    CacheEnum m_caching;
};
}

#endif //__CIFTI_FILE
