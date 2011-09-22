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
#include "CiftiFile.h"
#include "CiftiXML.h"
#include <algorithm>
#include "CiftiXMLReader.h"
#include <vector>
#include "byteswap.h"

#include <iostream>
using namespace caret;

/**
 * Default Constructor
 * 
 * Default Constructor
 * @param CACHE_LEVEL specifies whether the file is loaded into memory, or kept on disk
 * currently only IN_MEMORY is supported 
 */
CiftiFile::CiftiFile(CACHE_LEVEL clevel) throw (CiftiFileException)
{
   init();
   m_clevel = clevel;
}

/**
 * constructor
 * 
 * Constructor
 * 
 * @param fileName name and path of the Cifti File
 * @param CACHE_LEVEL specifies whether the file is loaded into memory, or kept on disk
 * currently only IN_MEMORY is supported
 */
CiftiFile::CiftiFile(const QString &fileName,CACHE_LEVEL clevel) throw (CiftiFileException) 
{
   init();   
   this->openFile(fileName, clevel);
}

void CiftiFile::init()
{
   this->m_niftiHeader = NULL;
   this->m_clevel = IN_MEMORY; 
   this->m_xml = NULL;
   this->m_matrix = NULL;
   this->m_swapNeeded = false;
   this->m_copyMatrix = false;
}

/**
 * 
 * 
 * open a Cifti file
 * 
 * @param fileName name and path of the Cifti File
 */
void CiftiFile::openFile(const QString &fileName) throw (CiftiFileException)
{
   m_inputFile.setFileName(fileName);
   m_inputFile.open(QIODevice::ReadOnly);
   readHeader();
   
   //read XML
   m_swapNeeded = m_niftiHeader->getSwapNeeded();
   char extensions[4];
   m_inputFile.read(extensions,4);
   unsigned int length;
   m_inputFile.read((char *)&length, 4);
   if(m_swapNeeded)ByteSwapping::swapBytes(&length,1);
   unsigned int ecode;
   m_inputFile.read((char *)&ecode,4);
   if(m_swapNeeded)ByteSwapping::swapBytes(&ecode,1);
   if(ecode != NIFTI_ECODE_CIFTI) throw CiftiFileException("Error reading extensions.  Extension Code is not Cifti.");
   QByteArray bytes = m_inputFile.read(length-8);//we substract 8 since the length includes the ecode and length
   m_xml = new CiftiXML(bytes);
   
   //read Matrix
   readCiftiMatrix();
}

/**
 * 
 * 
 * open a Cifti file
 * 
 * @param fileName name and path of the Cifti File
 * @param CACHE_LEVEL specifies whether the file is loaded into memory, or kept on disk
 * currently only IN_MEMORY is supported 
 */
void CiftiFile::openFile(const QString &fileName, CACHE_LEVEL clevel) throw (CiftiFileException)
{
   m_clevel = clevel;
   this->openFile(fileName);
}

/** 
 * 
 * 
 * write the Cifti File
 * 
 * @param fileName specifies the name and path of the file to write to
 */
void CiftiFile::writeFile(const QString &fileName) const throw (CiftiFileException)
{
   QFile outputFile(fileName);
   outputFile.open(QIODevice::WriteOnly);
   //Get XML string and length, which is needed to calculate the vox_offset stored in the Nifti Header
   QByteArray xmlBytes;
   m_xml->writeXML(xmlBytes);
   int length = 8 + xmlBytes.length();
   int ecode = 32;//NIFTI_ECODE_CIFTI
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
   outputFile.close();
}

/**
 * destructor
 */
CiftiFile::~CiftiFile()
{
   if(m_niftiHeader) delete m_niftiHeader;
}

/**
 * 
 * 
 * set the NiftiHeader
 * 
 * @param header
 */
// Head IO
void CiftiFile::setHeader(const NiftiHeader &header) throw (CiftiFileException)
{
   if(m_niftiHeader) delete m_niftiHeader;
   m_niftiHeader = new NiftiHeader(header);
}


void CiftiFile::readHeader() throw (CiftiFileException)
{
   if(m_niftiHeader != NULL) delete m_niftiHeader;
   m_niftiHeader = new NiftiHeader(m_inputFile);
}

/**
 * 
 * 
 * get a newly allocated copy of the NiftiHeader
 * 
 * @return NiftiHeader*
 */
NiftiHeader *CiftiFile::getHeader() throw (CiftiFileException)
{
   if(m_niftiHeader == NULL) readHeader();
   return new NiftiHeader(*m_niftiHeader);
}

/**
 * 
 * 
 * get a copy of the NiftiHeader
 * 
 * @param header
 */
void CiftiFile::getHeader(NiftiHeader &header) throw (CiftiFileException)
{
   if(m_niftiHeader == NULL) readHeader();
   header = *m_niftiHeader;
}

// Matrix IO
/**
 * 
 * 
 * set the CiftiMatrix
 * 
 * @param matrix
 */
void CiftiFile::setCiftiMatrix(CiftiMatrix & matrix) throw (CiftiFileException)
{
   if(this->m_matrix) delete this->m_matrix;
   if(this->m_copyMatrix)      
   {
      this->m_matrix = new CiftiMatrix(matrix);
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
   m_niftiHeader->setCiftiDimensions(tempvec);
}

/**
 * 
 * 
 * get a pointer to the CiftiMatrix
 * 
 * @return CiftiMatrix* 
 */
CiftiMatrix * CiftiFile::getCiftiMatrix() throw (CiftiFileException)
{
   if(!this->m_matrix) readCiftiMatrix();//TODO check reset extension offset if needed
   if(this->m_copyMatrix)
   {
      return new CiftiMatrix(*m_matrix);
   }
   else
   {
      CiftiMatrix *temp = this->m_matrix;
      this->m_matrix = NULL;
      return temp;
   }      
}

void CiftiFile::readCiftiMatrix() throw (CiftiFileException)
{
   if(m_matrix != NULL) delete m_matrix;
   std::vector <int> dimensions;
   m_niftiHeader->getCiftiDimensions(dimensions);
   m_matrix = new CiftiMatrix(m_inputFile, dimensions,m_clevel);
   m_matrix->setCopyData(m_copyMatrix);
   if(m_swapNeeded) m_matrix->swapByteOrder();
}

// XML IO
/** 
 * 
 * 
 * setter for CiftiFile's CiftiXML object
 * 
 * @param ciftixml
 */
void CiftiFile::setCiftiXML(CiftiXML & xml) throw (CiftiFileException)
{
   if(this->m_xml) delete this->m_xml;
   this->m_xml = new CiftiXML(xml);
   
}

/** 
 * 
 * 
 * get a pointer to a newly allocated copy of the CiftiFile's CiftiXML class
 * 
 * @return CiftiXML*
 */

CiftiXML * CiftiFile::getCiftiXML() throw (CiftiFileException)
{
   if(!this->m_xml) return NULL;//readCiftiXML();//TODO check reset extension offset if needed
   return new CiftiXML(*m_xml);
   
}

/** 
 * 
 * 
 * gets a copy of the object's internal CiftiXML object
 * 
 * @param xml 
 */
void CiftiFile::getCiftiXML(CiftiXML &xml) throw (CiftiFileException)
{
   if(!this->m_xml) return;//readCiftiXML();//TODO check reset extension offset if needed
   xml = *m_xml;  
}
