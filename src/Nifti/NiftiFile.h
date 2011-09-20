/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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
#ifndef NIFTIFILE_H
#define NIFTIFILE_H

#include <QtCore>
#include "nifti2.h"
#include "iostream"
#include "NiftiException.h"
#include "NiftiHeader.h"
#include "ByteSwapping.h"
#include "CiftiMatrix.h"
#include "CiftiXML.h"

/// Class for opening, reading, and writing generic Nifti1/2 Data

namespace caret {

class NiftiFile
{
public:
    NiftiFile();
public:
   /// Constructor
   NiftiFile(CACHE_LEVEL clevel = IN_MEMORY) throw (NiftiException);
   /// Constructor
   NiftiFile(const QString &fileName,CACHE_LEVEL clevel = IN_MEMORY) throw (NiftiException);
   /// Open the Nifti File
   virtual void openFile(const QString &fileName) throw (NiftiException);
   /// Open the Nifti File
   virtual void openFile(const QString &fileName, CACHE_LEVEL clevel) throw (NiftiException);
   /// Write the Nifti File
   virtual void writeFile(const QString &fileName) const throw (NiftiException);
   /// set NiftiHeader
   virtual void setHeader(const NiftiHeader &header) throw (NiftiException);
   /// get NiftiHeader
   virtual NiftiHeader * getHeader() throw (NiftiException);
   /// get NiftiHeader
   virtual void getHeader(NiftiHeader &header) throw (NiftiException);
   /// set CiftiMatrix
   virtual void setNiftiMatrix(CiftiMatrix &matrix) throw (NiftiException);
   /// get CiftiMatrix
   virtual CiftiMatrix * getNiftiMatrix() throw (NiftiException);
   /// Destructor
   virtual ~NiftiFile();
protected:
   virtual void init();
   virtual void readHeader() throw (NiftiException);
   //virtual void readNiftiExtension() throw (NiftiException)
   //virtual void readNiftiMatrix() throw (NiftiException);
   CACHE_LEVEL m_clevel;
   bool m_copyMatrix;

   QFile m_inputFile;
   NiftiHeader *m_niftiHeader;

   bool m_swapNeeded;
};


}

#endif // NIFTIFILE_H
