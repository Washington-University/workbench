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
#ifndef __CIFTI_MATRIX
#define __CIFTI_MATRIX
#include <QtCore>
#include <CiftiFileException.h>
#include <vector>

namespace caret {

enum  CACHE_LEVEL {
  IN_MEMORY,
  ON_DISK
};
/// Class for reading and writing Cifti Matrix Data
//warning!!! when using ON_DISK cache level, don't plan on using the file handle again, consider it gone, once handing it to a CiftiMatrix object.
class CiftiMatrix
{ 
public:
   CiftiMatrix(QFile &file, std::vector<int> &dimensions,CACHE_LEVEL clevel=IN_MEMORY) throw (CiftiFileException);
   CiftiMatrix(const QString &fileName, std::vector<int> &dimensions, unsigned long long int offset, CACHE_LEVEL clevel=IN_MEMORY) throw (CiftiFileException);
   CiftiMatrix(const QString &fileName, std::vector<int> &dimensions, CACHE_LEVEL clevel=IN_MEMORY) throw (CiftiFileException);
   CiftiMatrix() throw (CiftiFileException);
   ~CiftiMatrix();
   void swapByteOrder();
   void readMatrix(QFile &file, std::vector<int> &dimensions);
   void readMatrix(const QString &fileName, std::vector<int> &dimensions, unsigned long long offset);
   void readMatrix(const QString &fileName, std::vector<int> &dimensions);
   void writeMatrix(QFile &file);
   void getMatrixData(float *&data, std::vector <int> &dimensions);//gets the entire matrix, depending on the copy data preferences,
                           //either copies all of the data
   void setMatrixData(float *data, std::vector <int> &dimensions);   
   void getDimensions(std::vector <int>& dimensions);
   void setCopyData(bool copyData);
   bool getCopyData();
protected:
   void freeMatrix();
   void initMatrix();
   void init();
   void setDimensions(std::vector <int> dimensions);
   float * m_matrix;
   unsigned long long m_length;
   std::vector <int> m_dimensions;
   CACHE_LEVEL m_clevel;
   bool m_copyData;
};

}

#endif //__CIFTI_MATRIX
