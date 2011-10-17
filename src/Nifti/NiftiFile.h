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
#include "iostream"
#include "NiftiException.h"
#include "NiftiHeaderIO.h"
#include "ByteSwapping.h"
#include "NiftiMatrix.h"

/// Class for opening, reading, and writing generic Nifti1/2 Data, doesn't support extensions (yet)

namespace caret {

class NiftiFile
{
public:
   /// Constructor
   NiftiFile() throw (NiftiException);
   /// Constructor
   NiftiFile(const QString &fileName) throw (NiftiException);
   /// Open the Nifti File
   virtual void openFile(const QString &fileName) throw (NiftiException);
   /// Write the Nifti File
   virtual void writeFile(const QString &fileName) const throw (NiftiException);

   /// Header Functions
   /// set Nifti1Header
   virtual void setHeader(const Nifti1Header &header) throw (NiftiException);
   /// get Nifti1Header
   void getHeader(Nifti1Header &header) throw (NiftiException);
   /// set Nifti2Header
   virtual void setHeader(const Nifti2Header &header) throw (NiftiException);
   /// get Nifti2Header
   void getHeader(Nifti2Header &header) throw (NiftiException);

   /// Destructor
   virtual ~NiftiFile();
protected:
   virtual void init();   
   //virtual void readNiftiExtension() throw (NiftiException)
   //virtual void readNiftiMatrix() throw (NiftiException);

   AString filename;
   NiftiHeaderIO header;
   NiftiMatrix matrix;

};


}

#endif // NIFTIFILE_H

