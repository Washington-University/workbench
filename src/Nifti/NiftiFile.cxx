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
#include "NiftiFile.h"
#if 0

#include <algorithm>
#include <vector>
#include <iostream>

using namespace caret;

/**
 * Default Constructor
 *
 * Default Constructor
 * @param CACHE_LEVEL specifies whether the file is loaded into memory, or kept on disk
 * currently only IN_MEMORY is supported
 */
NiftiFile::NiftiFile(CACHE_LEVEL clevel) throw (NiftiException)
{
   init();
   m_clevel = clevel;
}

/**
 * constructor
 *
 * Constructor
 *
 * @param fileName name and path of the Nifti File
 * @param CACHE_LEVEL specifies whether the file is loaded into memory, or kept on disk
 * currently only IN_MEMORY is supported
 */
NiftiFile::NiftiFile(const QString &fileName,CACHE_LEVEL clevel) throw (NiftiException)
{
   init();
   this->openFile(fileName, clevel);
}

void NiftiFile::init()
{
   //this->nifti1Header = NULL;
   //this->nifti2Header = NULL;
   this->m_clevel = IN_MEMORY;
   //this->m_matrix = NULL;
   this->m_swapNeeded = false;
   this->m_copyMatrix = false;
}

/**
 *
 *
 * open a Nifti file
 *
 * @param fileName name and path of the Nifti File
 */
void NiftiFile::openFile(const QString &fileName) throw (NiftiException)
{
   m_inputFile.setFileName(fileName);
   m_inputFile.open(QIODevice::ReadOnly);
   readHeader();

   //read Extension

   //read Matrix
   //readNiftiMatrix();
}

/**
 *
 *
 * open a Nifti file
 *
 * @param fileName name and path of the Nifti File
 * @param CACHE_LEVEL specifies whether the file is loaded into memory, or kept on disk
 * currently only IN_MEMORY is supported
 */
void NiftiFile::openFile(const QString &fileName, CACHE_LEVEL clevel) throw (NiftiException)
{
   m_clevel = clevel;
   this->openFile(fileName);
}

/**
 *
 *
 * write the Nifti File
 *
 * @param fileName specifies the name and path of the file to write to
 */
void NiftiFile::writeFile(const QString &fileName) const throw (NiftiException)
{
   QFile outputFile(fileName);
   outputFile.open(QIODevice::WriteOnly);
   //Get XML string and length, which is needed to calculate the vox_offset stored in the Nifti Header
   /*QByteArray xmlBytes;
   m_xml->writeXML(xmlBytes);
   int length = 8 + xmlBytes.length();
   int ecode = 32;//NIFTI_ECODE_Nifti
   char bytes[4] = { 0x01, 0x00,0x00, 0x00};

   nifti_2_header header;
   m_niftiHeader->getHeaderStruct(header);
   header.vox_offset = 544 + length;
   int remainder = header.vox_offset % 8;
   int padding = 0;
   if (remainder) padding = 8 - remainder;//for 8 byte alignment
   header.vox_offset += padding;
   length += padding;
   m_niftiHeader->setHeaderStuct(header);


   //write out the file
   m_niftiHeader->writeFile(outputFile);
   outputFile.write(bytes,4);
   outputFile.write((char *)&length,4);
   outputFile.write((char *)&ecode,4);
   outputFile.write(xmlBytes);
   char junk[] = "         ";//filler for 8 byte alignment
   char* junk2 = &(junk[0]);
   if (padding) outputFile.write(junk2, padding);
   m_matrix->writeMatrix(outputFile);
   outputFile.close();*/
}

/**
 * destructor
 */
NiftiFile::~NiftiFile()
{
   //TODOif(m_niftiHeader) delete m_niftiHeader;
}

/**
 *
 *
 * set the NiftiHeader
 *
 * @param header
 */
// Header IO
void NiftiFile::setHeader(const Nifti1Header &header) throw (NiftiException)
{
   if(m_nifti1Header) delete m_nifti1Header;
   m_nifti1Header = new Nifti1Header(header);
}


void NiftiFile::readHeader() throw (NiftiException)
{
   //if(m_nifti1Header != NULL) delete m_nifti1Header;
   //TODO m_nifti1Header = new Nifti1Header(m_inputFile);
}

/**
 *
 *
 * get a newly allocated copy of the NiftiHeader
 *
 * @return NiftiHeader*
 */
/*Nifti1Header *NiftiFile::getHeader() throw (NiftiException)
{
   if(m_nifti1Header == NULL) readHeader();
   return new Nifti1Header(*m_nifti1Header);
}*/

/**
 *
 *
 * get a copy of the NiftiHeader
 *
 * @param header
 */
void NiftiFile::getHeader(Nifti1Header &header) throw (NiftiException)
{
   if(m_nifti1Header == NULL) readHeader();
   header = *m_nifti1Header;
}

/**
 *
 *
 * set the NiftiHeader
 *
 * @param header
 */
// Header IO
void NiftiFile::setHeader(const Nifti2Header &header) throw (NiftiException)
{
   if(m_nifti2Header) delete m_nifti2Header;
   m_nifti2Header = new Nifti2Header(header);
}


/**
 *
 *
 * get a newly allocated copy of the NiftiHeader
 *
 * @return NiftiHeader*
 */
/*NiftiHeader *NiftiFile::getHeader() throw (NiftiException)
{
   if(m_nifti2Header == NULL) readHeader();
   return new Nifti2Header(*m_nifti2Header);
}*/

/**
 *
 *
 * get a copy of the NiftiHeader
 *
 * @param header
 */
void NiftiFile::getHeader(Nifti2Header &header) throw (NiftiException)
{
   if(m_nifti2Header == NULL) readHeader();
   header = *m_nifti2Header;
}


// Matrix IO
/**
 *
 *
 * set the NiftiMatrix
 *
 * @param matrix
 */
void NiftiFile::setNiftiMatrix(CiftiMatrix & matrix) throw (NiftiException)
{
   /*if(this->m_matrix) delete this->m_matrix;
   if(this->m_copyMatrix)
   {
      this->m_matrix = new NiftiMatrix(matrix);
   }
   else
   {
      this->m_matrix = &matrix;
   }
   if (m_niftiHeader == NULL)
   {
      m_niftiHeader = new NiftiHeader;
      m_niftiHeader->initTimeSeriesHeaderStruct();
   }
   std::vector<int> tempvec;
   matrix.getDimensions(tempvec);
   m_niftiHeader->setNiftiDimensions(tempvec);*/
}

/**
 *
 *
 * get a pointer to the NiftiMatrix
 *
 * @return NiftiMatrix*
 */
/*
NiftiMatrix * NiftiFile::getNiftiMatrix() throw (NiftiException)
{
   if(!this->m_matrix) readNiftiMatrix();//TODO check reset extension offset if needed
   if(this->m_copyMatrix)
   {
      return new NiftiMatrix(*m_matrix);
   }
   else
   {
      NiftiMatrix *temp = this->m_matrix;
      this->m_matrix = NULL;
      return temp;
   }
}

void NiftiFile::readNiftiMatrix() throw (NiftiException)
{
   if(m_matrix != NULL) delete m_matrix;
   std::vector <int> dimensions;
   m_niftiHeader->getNiftiDimensions(dimensions);
   m_matrix = new NiftiMatrix(m_inputFile, dimensions,m_clevel);
   m_matrix->setCopyData(m_copyMatrix);
   if(m_swapNeeded) m_matrix->swapByteOrder();
}
*/
#endif
