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
#include <CiftiMatrix.h>
#ifdef Q_OS_WIN32
#include <io.h>
#else //not Q_OST_WIN32
#include <unistd.h>
#endif //ifdef Q_OS_WIN32

using namespace caret;
/**
 * Constructor
 * 
 * Constructor
 * @param file handle to a file that is positioned at the start of the Cifti Matrix
 * @param dimensions an std::vector containing the number and size of all dimensions i.e. x,y,z,t
 * @param CACHE_LEVEL
 */
CiftiMatrix::CiftiMatrix(QFile &file, std::vector<int> &dimensions,CACHE_LEVEL clevel) throw (CiftiFileException)
{
   init();
   m_clevel = clevel;
   readMatrix(file, dimensions);   
}

/**
 * Constructor
 * 
 * Constructor
 * @param fileName name of file that contains the Cifti Matrix
 * @param dimensions an std::vector containing the number and size of all dimensions i.e. x,y,z,t
 * @param offset the position of the beginning of the CiftiMatrix within the file
 * @param CACHE_LEVEL
 */
CiftiMatrix::CiftiMatrix(const QString &fileName, std::vector<int> &dimensions, unsigned long long offset, CACHE_LEVEL clevel) throw (CiftiFileException)
{
   init();
   m_clevel = clevel;
   readMatrix(fileName, dimensions, offset);
}

/**
 * Constructor
 * 
 * Constructor
 * @param fileName name of file that contains the Cifti Matrix
 * @param dimensions an std::vector containing the number and size of all dimensions i.e. x,y,z,t
 * @param CACHE_LEVEL
 */
CiftiMatrix::CiftiMatrix(const QString &fileName, std::vector <int> &dimensions, CACHE_LEVEL clevel) throw (CiftiFileException)
{
   init();
   m_clevel = clevel;
   readMatrix(fileName, dimensions);
}

/**
 * Default Constructor
 * 
 * Default Constructor
 */
CiftiMatrix::CiftiMatrix() throw (CiftiFileException)
{
   init();   
}

/**
 * Destructor
 * 
 * Destructor
 */
CiftiMatrix::~CiftiMatrix()
{
   freeMatrix();
}

/**
 * swap byte order
 * 
 * swap the byte order of the internal matrix from big to little endian, or vice versa
 */
void CiftiMatrix::swapByteOrder()
{
   CiftiByteSwap::swapBytes(m_matrix,m_length);   
}

/**
 * get Matrix Data
 * 
 * gets the entire matrix, depending on the copy data preferences,
 * either copies all of the data to a newly allocated matrix, or
 * returns a pointer the internal matrix data
 * @param matrix
 * @param dimensions 
 */
void CiftiMatrix::getMatrixData(float *&matrix, std::vector <int> &dimensions)
{   
   dimensions = m_dimensions;
   if(m_copyData)
   {
      matrix = new float [m_length];
      memcpy((char *)matrix,(char *)m_matrix,m_length*4);
   }
   else
   {
      matrix = m_matrix;
   }   
}

/**
 * set Matrix Data
 * 
 * sets the matrix data, depending on the copy data preferences,
 * either copies the data to a newly allocated internal matrix, or
 * sets the internal matrix to point to the supplied matrix
 * @param data
 * @param dimensions
 */
void CiftiMatrix::setMatrixData(float *data, std::vector <int> &dimensions)
{
   freeMatrix();
   setDimensions(dimensions);
   
   if(m_copyData)
   {
      m_matrix = new float[m_length];
      memcpy((char *)m_matrix,(char *)data, m_length*4);      
   }
   else
   {
      m_matrix = data;
   }
}

/**
 * set Dimensions
 * 
 * set matrix Dimensions
 * @param dimensions
 */
void CiftiMatrix::setDimensions(std::vector <int> dimensions)
{
   m_dimensions = dimensions;
   m_length = m_dimensions[0];
   for(unsigned int i =1;i<m_dimensions.size();i++)
   {
      m_length*=dimensions[i];
   }
}

void CiftiMatrix::getDimensions(std::vector <int>& dimensions)
{
   dimensions = m_dimensions;
}

/**
 * set Copy Data
 * 
 * set Copy Data value, if set to true, then set/getMatrixData
 * will return copies of the internal matrix data.  If set to
 * false, then pointers to the internal data are set/get
 * @param copyData
 */
void CiftiMatrix::setCopyData(bool copyData)
{
   m_copyData = copyData;
}

/**
 * get Copy Data
 * 
 * get Copy Data value, if set to true, then set/getMatrixData
 * will return copies of the internal matrix data.  If set to
 * false, then the actual pointer to the internal data are affected
 * by set/getMatrixData
 * @return copyData
 */
bool CiftiMatrix::getCopyData()
{
   return m_copyData;
}

void CiftiMatrix::init()
{
   m_clevel = IN_MEMORY;
   m_copyData = false;
   initMatrix();
}

void CiftiMatrix::initMatrix()
{
   m_dimensions.clear();
   m_matrix = NULL;
   m_length = 0;
}

void CiftiMatrix::freeMatrix()
{
   if(m_matrix) delete m_matrix;
   initMatrix();
}

/**
 * readMatrix
 * 
 * read Matrix from file. 
 * @param fileName
 * @param dimensions
 */
void CiftiMatrix::readMatrix(const QString &fileName, std::vector<int> &dimensions)
{
   readMatrix(fileName, dimensions, 0);   
}

/**
 * readMatrix
 * 
 * read Matrix from file, starting at specified offset
 * @param fileName
 * @param dimensions
 * @param offset
 */
void CiftiMatrix::readMatrix(const QString &fileName, std::vector<int> &dimensions, unsigned long long offset)
{
   QFile ciftiFile;
   ciftiFile.setFileName(fileName);   
   if(m_clevel == IN_MEMORY)
   {
      ciftiFile.open(QIODevice::ReadOnly);
      if(offset) ciftiFile.seek(offset);
      readMatrix(ciftiFile,dimensions);
   }
   else if(m_clevel == ON_DISK)
   {
      CiftiFileException("ON_DISK file IO mode not yet implemented.");
      ciftiFile.open(QIODevice::ReadOnly);
      if(offset) ciftiFile.seek(offset);
   }
}

/**
 * readMatrix
 * 
 * read Matrix from file handle. 
 * @param file
 * @param dimensions
 */
void CiftiMatrix::readMatrix(QFile &file, std::vector<int> &dimensions)
{
   freeMatrix();
   setDimensions(dimensions);
   
   if(m_clevel == IN_MEMORY)
   {
      m_matrix = new float [m_length];
      if(!m_matrix) CiftiFileException("Error allocating Cifti Matrix.");
      int fd = file.handle();
      size_t bytes_read = 0;
      size_t bytes_needed = m_length * 4;
      char * position = (char *)m_matrix;
      while(bytes_read < bytes_needed)
      {
         position = (char *)m_matrix + bytes_read;
         bytes_read += read(fd, (void *)position, bytes_needed-bytes_read);         
      }
      //unsigned long long bytes_read = file.readData((char *)(m_matrix),m_length);
      //bytes_read = file.read((char *)(m_matrix),1500000000);
      if(bytes_read != m_length*4) CiftiFileException("Error reading matrix from Cifti File.");
   }
   else if(m_clevel == ON_DISK)
   {
      CiftiFileException("ON_DISK file IO mode not yet implemented.");
   }   
}

/**
 * writeMatrix
 * 
 * write Matrix to specified file handle
 * @param file handle to file for writing matrix data
 */
void CiftiMatrix::writeMatrix(QFile &file)
{   
   file.write((char *)m_matrix,m_length*4);
}
