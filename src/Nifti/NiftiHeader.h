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

#ifndef NIFTI_HEADER_H
#define NIFTI_HEADER_H

#include <QtCore>
#include "nifti1.h"
#include "nifti2.h"
#include "iostream"
#include "NiftiException.h"
#include "ByteSwapping.h"
#include <vector>

namespace caret {

/// Simple Container class for storing NiftiHeader data
class NiftiHeader {
public:
   NiftiHeader() throw (NiftiException);
   NiftiHeader(const QString &inputFileName) throw (NiftiException);
   NiftiHeader(QFile &inputFile) throw (NiftiException);
   NiftiHeader(const nifti_2_header &header) throw (NiftiException);
   ~NiftiHeader();
   void readFile(const QString &inputFileName) throw (NiftiException);
   void readFile(QFile &inputFile) throw (NiftiException);
   void getHeaderStruct(nifti_2_header &header) const throw (NiftiException);
   void setHeaderStuct(const nifti_2_header &header) throw (NiftiException);
   int getCiftiType(const nifti_2_header &header) const throw (NiftiException);
   int getCiftiType() const throw (NiftiException);
   void writeFile(QFile &outputFile) const throw (NiftiException);
   void writeFile(const QString &outputFileName) const throw (NiftiException);
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
   nifti_1_header m_header1;
   bool m_swapNeeded;
};

} // namespace caret

#endif // NIFTI_HEADER_H
