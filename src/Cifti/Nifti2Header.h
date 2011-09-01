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

#ifndef __NIFTI_2_HEADER
#define __NIFTI_2_HEADER
#include <QtCore>
#include "nifti2.h"
#include "iostream"
#include "CiftiFileException.h"
#include "CiftiByteSwap.h"
#include <vector>

/// Simple Container class for storing Nifti2Header data
class Nifti2Header {
public:
   Nifti2Header() throw (CiftiFileException);
   Nifti2Header(const QString &inputFileName) throw (CiftiFileException);
   Nifti2Header(QFile &inputFile) throw (CiftiFileException);
   Nifti2Header(const nifti_2_header &header) throw (CiftiFileException);
   ~Nifti2Header();
   void readFile(const QString &inputFileName) throw (CiftiFileException);
   void readFile(QFile &inputFile) throw (CiftiFileException);
   void getHeaderStruct(nifti_2_header &header) const throw (CiftiFileException);
   void setHeaderStuct(const nifti_2_header &header) throw (CiftiFileException);
   int getCiftiType(const nifti_2_header &header) const throw (CiftiFileException);
   int getCiftiType() const throw (CiftiFileException);
   void writeFile(QFile &outputFile) const throw (CiftiFileException);
   void writeFile(const QString &outputFileName) const throw (CiftiFileException);
   QString *getHeaderAsString();
   void initHeaderStruct(nifti_2_header &header);
   void initHeaderStruct();
   void initTimeSeriesHeaderStruct(nifti_2_header &header);
   void initTimeSeriesHeaderStruct();
   bool getSwapNeeded();
   void getCiftiDimensions(std::vector <int> &dimensions);
   void setCiftiDimensions(std::vector <int> &dimensions);
private:
   nifti_2_header m_header; 
   bool m_swapNeeded;
};

#endif //__NIFTI_2_HEADER
