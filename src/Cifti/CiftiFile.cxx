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
//#include "byteswap.h"

#include <iostream>
using namespace caret;

/**
 * Default Constructor
 *
 * Default Constructor
 *
 */
CiftiFile::CiftiFile() throw (CiftiFileException)
{
    init();
}

/**
 * constructor
 *
 * Constructor
 *
 * @param fileName name and path of the Cifti File
 *
 */
CiftiFile::CiftiFile(const AString &fileName) throw (CiftiFileException)
{
    init();
    this->openFile(fileName);
}

void CiftiFile::init()
{   
    this->m_swapNeeded = false;
}

/**
 *
 *
 * open a Cifti file
 *
 * @param fileName name and path of the Cifti File
 */
void CiftiFile::openFile(const AString &fileName) throw (CiftiFileException)
{
    //Read Nifti2Header


    //read XML
    //TODO m_swapNeeded = m_Nifti2Header->getSwapNeeded();
    {
        QFile inputFile(fileName);
        inputFile.setFileName(fileName);
        inputFile.open(QIODevice::ReadWrite);
        char extensions[4];
        inputFile.read(extensions,4);
        unsigned int length;
        inputFile.read((char *)&length, 4);
        if(m_swapNeeded)ByteSwapping::swapBytes(&length,1);
        unsigned int ecode;
        inputFile.read((char *)&ecode,4);
        if(m_swapNeeded)ByteSwapping::swapBytes(&ecode,1);
        if(ecode != NIFTI_ECODE_CIFTI) throw CiftiFileException("Error reading extensions.  Extension Code is not Cifti.");
        QByteArray bytes = inputFile.read(length-8);//we substract 8 since the length includes the ecode and length
        inputFile.close();
        m_xml.readXML(bytes);

    }

    //set up Matrix for reading..

}

/** 
 *
 *
 * write the Cifti File
 *
 * @param fileName specifies the name and path of the file to write to
 */
void CiftiFile::writeFile(const AString &fileName) const throw (CiftiFileException)
{


    //Get XML string and length, which is needed to calculate the vox_offset stored in the Nifti Header
    QByteArray xmlBytes;
    CiftiXML xml = m_xml;
    xml.writeXML(xmlBytes);
    int length = 8 + xmlBytes.length();

    /*
   nifti_2_header header;
   m_Nifti2Header->getHeaderStruct(header);
   header.vox_offset = 544 + length;
   int remainder = header.vox_offset % 8;
   int padding = 0;
   if (remainder) padding = 8 - remainder;//for 8 byte alignment
   header.vox_offset += padding;
   length += padding;
   m_Nifti2Header->setHeaderStuct(header);
   */

    //write out the file
    //write out header


    //write out the xml extension
    /*{
       int ecode = 32;//NIFTI_ECODE_CIFTI
       char bytes[4] = { 0x01, 0x00,0x00, 0x00};

       QFile outputFile(fileName);
       outputFile.open(QIODevice::WriteOnly);
       outputFile.write(bytes,4);
       outputFile.write((char *)&length,4);
       outputFile.write((char *)&ecode,4);
       outputFile.write(xmlBytes);
       char junk[] = "         ";//filler for 8 byte alignment
       char* junk2 = &(junk[0]);
       if (padding) outputFile.write(junk2, padding);
       outputFile.close();
   }*/
    //write the matrix

}

/**
 * destructor
 */
CiftiFile::~CiftiFile()
{

}

/**
 *
 *
 * set the Nifti2Header
 *
 * @param header
 */
// Header IO
void CiftiFile::setHeader(const Nifti2Header &header) throw (CiftiFileException)
{   
    m_headerIO.setHeader(header);
}

/**
 *
 *
 * get a copy of the Nifti2Header
 *
 * @param header
 */
void CiftiFile::getHeader(Nifti2Header &header) throw (CiftiFileException)
{
    m_headerIO.getHeader(header);
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
    this->m_xml = xml;

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
    xml = this->m_xml;
}
