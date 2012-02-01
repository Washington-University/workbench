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

#include "CiftiMatrix.h"
#include "zlib.h"
#include "QFile"
#include "ByteSwapping.h"
#include "qtemporaryfile.h"
#ifdef CARET_OS_WINDOWS
#include <io.h>
#else //not CARET_OS_WINDOWS
#include <unistd.h>
#endif //ifdef CARET_OS_WINDOWS
using namespace caret;
using namespace std;
CiftiMatrix::CiftiMatrix()
{
    init();
}

CiftiMatrix::CiftiMatrix(const AString &fileNameIn, const CacheEnum e)
{
    init();
    m_fileName = fileNameIn;
    m_caching = e;
}

void CiftiMatrix::init()
{
    m_beenInitialized = false;
    m_caching = IN_MEMORY;
    m_matrix = NULL;
    m_matrixOffset = 0;
    file = NULL;
}

void CiftiMatrix::deleteCache()
{
    if(m_matrix) delete [] m_matrix;
    m_matrix = NULL;
}

void CiftiMatrix::setup(vector<int64_t> &dimensions, const int64_t &offsetIn, const CacheEnum &e, const bool &needsSwapping) throw (CiftiFileException)
{
    m_needsSwapping = needsSwapping;
    
    if(dimensions.size() != 2)
    {
        throw CiftiFileException("Cifti only supports 2 dimensional matrices currently");
    }
    m_dimensions = dimensions;

    m_matrixOffset = offsetIn;
    m_caching = e;
    m_beenInitialized = true;    
    if(m_caching == IN_MEMORY)
    {
        int64_t matrixSize = m_dimensions[0]*m_dimensions[1];
        deleteCache();
        m_matrix = new float[matrixSize];
        if(!QFile::exists(m_fileName)) return;
        if(file) delete file;
        file = new QFile();
        file->setFileName(m_fileName);
        if(file->isWritable())
        {
            file->open(QIODevice::ReadWrite);
        }
        else
        {
            file->open(QIODevice::ReadOnly);
            if(e==ON_DISK)
            {
                std::cerr << "ON_DISK read/write support will throw errors if used on readonly files!" << std::endl;
            }
        }

#if 0        
        file->seek(m_matrixOffset);//TODO, see if QT has fixed reading large files
        //otherwise use stdio for this read...
        file->read((char *)m_matrix,matrixSize*sizeof(float));
#endif
        //QT sucks and is unable to handle reading from files over 2GB's in size
        int fh = file->handle();
        lseek(fh,m_matrixOffset,0);
        int64_t rowSize = this->m_dimensions[m_dimensions.size()-1];
        int64_t columnSize = this->m_dimensions[m_dimensions.size()-2];
        for(int64_t i=0;i<columnSize;i++)//apparently read also is unable to handle reading in more than 2GB's at a time, reading a row at a time to get around this
        {
            read(fh,(char *)&m_matrix[i*rowSize],rowSize*sizeof(float));
        }
        file->close();
        if(m_needsSwapping)ByteSwapping::swapBytes(m_matrix,matrixSize);
    }
    else
    {
        if(file) delete file;        
        if(!QFile::exists(m_fileName))
        {
            QTemporaryFile *tf = new QTemporaryFile();
            tf->setAutoRemove(true);
            file = tf;
            file->open(QIODevice::ReadWrite);
            return;
        }
        file = new QFile;
        file->setFileName(m_fileName);
        file->open(QIODevice::ReadWrite);//keep file open for speed
    }    
}

void CiftiMatrix::setMatrixFile(const AString &fileNameIn)
{
    deleteCache();
    if(file) delete file;
    init();
    m_fileName = fileNameIn;
}

void CiftiMatrix::getMatrixFile(AString &fileNameOut)
{
    fileNameOut = m_fileName;
}

void CiftiMatrix::getMatrixDimensions(vector<int64_t> &dimensions) const
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    dimensions = m_dimensions;
}

/*void CiftiMatrix::setCaching(const CacheEnum &e)
{
    m_caching = e;
}*/

void CiftiMatrix::getCaching(CacheEnum &e)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    e = m_caching;
}

void CiftiMatrix::getMatrixOffset(int64_t &offsetOut)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    offsetOut = m_matrixOffset;
}

//Matrix IO
void CiftiMatrix::getRow(float *rowOut, const int64_t &rowIndex) const throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)rowOut, (char *)&m_matrix[rowIndex*m_dimensions[1]], m_dimensions[1]*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {
        file->seek(m_matrixOffset+rowIndex*m_dimensions[1]*sizeof(float));
        file->read((char *)rowOut,m_dimensions[1]*sizeof(float));
        if(m_needsSwapping) ByteSwapping::swapBytes(rowOut,m_dimensions[1]);
    }
}

void CiftiMatrix::setRow(float *rowIn, const int64_t &rowIndex) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)&m_matrix[rowIndex*m_dimensions[1]], (char *)rowIn, m_dimensions[1]*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {
        if(m_needsSwapping) ByteSwapping::swapBytes(rowIn,m_dimensions[1]);
        file->seek(m_matrixOffset+rowIndex*m_dimensions[1]*sizeof(float));
        file->write((char *)rowIn,m_dimensions[1]*sizeof(float));
        if(m_needsSwapping) ByteSwapping::swapBytes(rowIn,m_dimensions[1]);
    }
}

void CiftiMatrix::getColumn(float *columnOut, const int64_t &columnIndex) const throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t rowSize = m_dimensions[1];
    int64_t columnSize = m_dimensions[0];
    if(m_caching == IN_MEMORY)
    {
        for(int64_t i=0;i<columnSize;i++){ columnOut[i]=m_matrix[columnIndex+rowSize*i]; }
    }
    else if(m_caching == ON_DISK)
    {
        //let's hope that people aren't stupid enough to think this is fast...
        for(int64_t i=0;i<columnSize;i++)
        {
            file->seek(m_matrixOffset+(columnIndex + i*rowSize)*sizeof(float));
            file->read((char *)&columnOut[i],sizeof(float));
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(columnOut,columnSize);
    }
}

void CiftiMatrix::setColumn(float *columnIn, const int64_t &columnIndex) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t rowSize = m_dimensions[1];
    int64_t columnSize = m_dimensions[0];
    if(m_caching == IN_MEMORY)
    {
        for(int64_t i=0;i<columnSize;i++){ m_matrix[columnIndex+rowSize*i]=columnIn[i]; }
    }
    else if(m_caching == ON_DISK)
    {
        if(m_needsSwapping) ByteSwapping::swapBytes(columnIn,columnSize);
        for(int64_t i=0;i<columnSize;i++)
        {
            file->seek(m_matrixOffset+(columnIndex + i*rowSize)*sizeof(float));
            file->write((char *)&columnIn[i],sizeof(float));
        }
        if(m_needsSwapping) ByteSwapping::swapBytes(columnIn,columnSize);
    }
}

void CiftiMatrix::getMatrix(float *matrixOut) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t matrixLength = m_dimensions[0]*m_dimensions[1];
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)matrixOut,(char *)m_matrix,matrixLength*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {
        file->seek(m_matrixOffset);//TODO, see if QT has fixed reading large files
        //otherwise use stdio for this read...
        file->read((char *)matrixOut,matrixLength*sizeof(float));
        if(m_needsSwapping) ByteSwapping::swapBytes(matrixOut,matrixLength);
    }
}

void CiftiMatrix::setMatrix(float *matrixIn) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    int64_t matrixLength = m_dimensions[0]*m_dimensions[1];
    if(m_caching == IN_MEMORY)
    {
        memcpy((char *)m_matrix,(char *)matrixIn,matrixLength*sizeof(float));
    }
    else if(m_caching == ON_DISK)
    {
        if(m_needsSwapping) ByteSwapping::swapBytes(matrixIn,matrixLength);
        file->seek(m_matrixOffset);//TODO, see if QT has fixed reading large files
        //otherwise use stdio for this read...
        file->write((char *)matrixIn,matrixLength*sizeof(float));
        if(m_needsSwapping) ByteSwapping::swapBytes(matrixIn,matrixLength);
    }
}

CiftiMatrix::~CiftiMatrix()
{
    deleteCache();
    if(file) delete file;
}

void CiftiMatrix::flushCache() throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    if(m_caching == IN_MEMORY)
    {
        int64_t matrixSize = m_dimensions[0]*m_dimensions[1];
        file->setFileName(m_fileName);
        file->open(QIODevice::ReadWrite);
        file->seek(m_matrixOffset);//TODO, see if QT has fixed reading large files
        //otherwise use stdio for this read...
        if(m_needsSwapping)ByteSwapping::swapBytes(m_matrix,matrixSize);
        file->write((char *)m_matrix,matrixSize*sizeof(float));
        file->close();
        if(m_needsSwapping)ByteSwapping::swapBytes(m_matrix,matrixSize);
    }
}

void CiftiMatrix::writeToNewFile(const AString &fileNameIn, const int64_t  &offsetIn, const bool &needsSwappingIn) throw (CiftiFileException)
{
    if(!m_beenInitialized) throw CiftiFileException("Matrix needs to be initialized before using, or after the file name has been changed.");
    QFile outFile;
    outFile.setFileName(fileNameIn);
    outFile.open(QIODevice::ReadWrite);
    outFile.seek(offsetIn);

    if(m_caching == IN_MEMORY)
    {
        int64_t matrixLength = m_dimensions[0]*m_dimensions[1];
        if(needsSwappingIn) ByteSwapping::swapBytes(m_matrix, matrixLength);
        outFile.write((char *)m_matrix,matrixLength*sizeof(float));
        outFile.close();
        if(needsSwappingIn) ByteSwapping::swapBytes(m_matrix, matrixLength);
    }
    else if(m_caching == ON_DISK)
    {
        if(!file) throw CiftiFileException("Cifti matrix is setup to cache on disk, but no cache file exists.");
        int64_t rowSize = m_dimensions[1];
        int64_t columnSize = m_dimensions[0];
        float * row = new float[rowSize];
        file->seek(m_matrixOffset);
        for(int64_t i =0;i<columnSize;i++)
        {            
            file->read((char *)row,rowSize*sizeof(float));
            if(m_needsSwapping != needsSwappingIn) ByteSwapping::swapBytes(row,rowSize);
            outFile.write((char *)row, rowSize*sizeof(float));
        }
        delete[] row;
    }
    outFile.close();
}
