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
    CiftiFile() throw (CiftiFileException);
    /// Constructor
    CiftiFile(const CacheEnum &caching) throw (CiftiFileException);
    /// Constructor
    CiftiFile(const AString &fileName, const CacheEnum &caching = IN_MEMORY) throw (CiftiFileException);
    /// Open the Cifti File
    virtual void openFile(const AString &fileName, const CacheEnum &caching = IN_MEMORY) throw (CiftiFileException);
    /// Write the Cifti File
    virtual void writeFile(const AString &fileName) throw (CiftiFileException);

    //get/set Nifti2/CiftiHeader
    /// set CiftiHeader
    virtual void setHeader(const CiftiHeader &header) throw (CiftiFileException);
    /// get CiftiHeader
    virtual void getHeader(CiftiHeader &header) throw (CiftiFileException);

    //TODO, put some thought into whether we want to hand back an xml tree vs handing back a class that manages
    //the tree in an intelligent way.
    /// set CiftiXML
    virtual void setCiftiXML(CiftiXML &ciftixml) throw (CiftiFileException);

    // Matrix IO, simply passes through to underlying Cifti Matrix
    /// get Row
    void getRow(float * rowOut,const int64_t &rowIndex) const throw (CiftiFileException)
    { m_matrix.getRow(rowOut, rowIndex); }
    /// set Row
    void setRow(float * rowIn, const int64_t &rowIndex) throw (CiftiFileException)
    { m_matrix.setRow(rowIn, rowIndex); }
    /// get Column
    void getColumn(float * columnOut, const int64_t &columnIndex) const throw (CiftiFileException)
    { m_matrix.getColumn(columnOut, columnIndex); }
    /// set Column
    void setColumn(float * columnIn, const int64_t &columnIndex) throw (CiftiFileException)
    { m_matrix.setColumn(columnIn, columnIndex); }
    /// get Matrix
    void getMatrix(float *matrixOut) throw (CiftiFileException)
    { m_matrix.getMatrix(matrixOut); }
    /// set Matrix
    void setMatrix(float *matrixIn) throw (CiftiFileException)
    { m_matrix.setMatrix(matrixIn); }
    /// setup Matrix
    void setupMatrix(vector<int64_t> &dimensions, const int64_t &offsetIn = 0, const CacheEnum &e=IN_MEMORY, const bool &needsSwapping=false) throw (CiftiFileException);
    /// setup Matrix
    void setupMatrix() throw(CiftiFileException);
    
    ///get row size
    int64_t getNumberOfColumns() const;
    
    ///get column size
    int64_t getNumberOfRows() const;

    /// Destructor
    virtual ~CiftiFile();
protected:
    virtual void init();

    AString m_fileName;
    CiftiHeaderIO m_headerIO;
    CiftiMatrix m_matrix;
    bool m_swapNeeded;
    CacheEnum m_caching;
};
}

#endif //__CIFTI_FILE
