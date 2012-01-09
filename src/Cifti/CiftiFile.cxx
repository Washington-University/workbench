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
#include "CiftiMatrix.h"


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

CiftiFile::CiftiFile(const CacheEnum &caching) throw (CiftiFileException)
{
    init();
    this->m_caching = caching;
}

/**
 * constructor
 *
 * Constructor
 *
 * @param fileName name and path of the Cifti File
 *
 */
CiftiFile::CiftiFile(const AString &fileName, const CacheEnum &caching) throw (CiftiFileException)
{
    init();
    this->m_caching = caching;
    this->openFile(fileName,caching);
}

void CiftiFile::init()
{   
    this->m_swapNeeded = false;
    this->m_caching = IN_MEMORY;
}

/**
 *
 *
 * open a Cifti file
 *
 * @param fileName name and path of the Cifti File
 */
void CiftiFile::openFile(const AString &fileName, const CacheEnum &caching) throw (CiftiFileException)
{
    if(!QFile::exists(fileName)) {
        throw CiftiFileException("Cifti File: " + fileName + " does not exist.");
        return;
    }
    try {
        m_caching = caching;
        //Read CiftiHeader
        m_headerIO.readFile(fileName);
        
        //read XML
        m_swapNeeded = m_headerIO.getSwapNeeded();
        {
            QFile inputFile(fileName);
            inputFile.setFileName(fileName);
            inputFile.open(QIODevice::ReadWrite);
            inputFile.seek(NIFTI2_HEADER_SIZE);
            char extensions[4];
            inputFile.read(extensions,4);
            unsigned int length;
            inputFile.read((char *)&length, 4);
            if(m_swapNeeded)ByteSwapping::swapBytes(&length,1);
            unsigned int ecode;
            inputFile.read((char *)&ecode,4);
            if(m_swapNeeded)ByteSwapping::swapBytes(&ecode,1);
            if((int32_t)ecode != NIFTI_ECODE_CIFTI) throw CiftiFileException("Error reading extensions.  Extension Code is not Cifti.");
            QByteArray bytes = inputFile.read(length-8);//we substract 8 since the length includes the ecode and length
            inputFile.close();
            m_xml.readXML(bytes);
            
        }
        
        //set up Matrix for reading..
        m_matrix.setMatrixFile(fileName);
        CiftiHeader header;
        m_headerIO.getHeader(header);
        std::vector <int64_t> vec;
        
        header.getDimensions(vec);
        int64_t offset = header.getVolumeOffset();
        m_matrix.setup(vec,offset,m_caching,m_swapNeeded);
    }
    catch (NiftiException e) {
        throw CiftiFileException(e.whatString());
    }
}

/**
 *
 * Setup the Cifti Matrix
 *
 * @param header the cifti header used for setting up the matrix
 * @param xml the cifti xml extensions used to calculate vox offset
 *
 */

void CiftiFile::setupMatrix() throw (CiftiFileException)
{

    //Get XML string and length, which is needed to calculate the vox_offset stored in the Nifti Header
    QByteArray xmlBytes;
    CiftiXML xml = m_xml;
    xml.writeXML(xmlBytes);
    int length = 8 + xmlBytes.length();


    // update header struct dimensions and vox_offset
    CiftiHeader ciftiHeader;
    this->m_headerIO.getHeader(ciftiHeader);

    int64_t vox_offset = 544 + length;
    int remainder = vox_offset % 8;
    int padding = 0;
    if (remainder) padding = 8 - remainder;//for 8 byte alignment
    vox_offset += padding;
    length += padding;


    std::vector <int64_t> dim;
    ciftiHeader.getDimensions(dim);
    ciftiHeader.setVolumeOffset(vox_offset);
    m_headerIO.setHeader(ciftiHeader);
    
    //setup the matrix
    m_matrix.setup(dim, vox_offset, this->m_caching, this->m_swapNeeded);
}

/**
 *
 * setup the Cifti Matrix
 * @param dimensions the dimensions of the cifti matrix
 * @param offsetIn the offset, in bytes, of the start of the matrix within the cifti file
 * @param e the caching type of the matrix (ON_DISK or IN_MEMORY)
 * @param needsSwapping the byte order of the matrix (native or swapped)
 */
void CiftiFile::setupMatrix(vector<int64_t> &dimensions, const int64_t &offsetIn, const CacheEnum &e, const bool &needsSwapping) throw (CiftiFileException)
{
    m_matrix.setup(dimensions, offsetIn, e, needsSwapping);
}

/** 
 *
 *
 * write the Cifti File
 *
 * @param fileName specifies the name and path of the file to write to
 */
void CiftiFile::writeFile(const AString &fileName)
{
    if(QFile::exists(fileName))
    {
        throw CiftiFileException("in place writes aren't supported.");
        return;
    }
    //Get XML string and length, which is needed to calculate the vox_offset stored in the Nifti Header
    QByteArray xmlBytes;
    CiftiXML xml = m_xml;
    xml.writeXML(xmlBytes);
    int length = 8 + xmlBytes.length();


    // update header struct dimensions and vox_offset
    CiftiHeader ciftiHeader;
    this->m_headerIO.getHeader(ciftiHeader);


    int64_t vox_offset = 544 + length;
    int remainder = vox_offset % 8;
    int padding = 0;
    if (remainder) padding = 8 - remainder;//for 8 byte alignment
    vox_offset += padding;
    length += padding;


    std::vector <int64_t> dim;
    m_matrix.getMatrixDimensions(dim);
    ciftiHeader.setDimensions(dim);
    ciftiHeader.setVolumeOffset(vox_offset);
    m_headerIO.setHeader(ciftiHeader);


    //write out the file
    //write out header
    m_headerIO.writeFile(fileName);

    //write out the xml extension
    QFile file;
    file.setFileName(fileName);
    file.open(QIODevice::ReadWrite);
    file.seek(540);
    char eflags[4] = {1,0x00,0x00,0x00};
    int32_t ecode = NIFTI_ECODE_CIFTI;
    
    if(this->m_swapNeeded) {
        ByteSwapping::swapBytes(&length,1);
        ByteSwapping::swapBytes(&ecode,1);
    }
    file.write(eflags,4);
    file.write((char *)&length,4);
    file.write((char *)&ecode, 4);
    file.write(xmlBytes);
    char nulls[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    file.write(nulls,padding);//pad out null values to 8 byte boundary
    file.close();

    //write the matrix
    m_matrix.writeToNewFile(fileName,vox_offset, false);
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
 * set the CiftiHeader
 *
 * @param header
 */
// Header IO
void CiftiFile::setHeader(const CiftiHeader &header) throw (CiftiFileException)
{   
    m_headerIO.setHeader(header);
    setupMatrix();
}

/**
 *
 *
 * get a copy of the CiftiHeader
 *
 * @param header
 */
void CiftiFile::getHeader(CiftiHeader &header) throw (CiftiFileException)
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
void CiftiFile::setCiftiXML(const CiftiXML & xml) throw (CiftiFileException)
{
    this->m_xml = xml;
    setupMatrix();
}

int64_t CiftiFile::getNumberOfColumns() const
{
    vector<int64_t> myDims;
    m_matrix.getMatrixDimensions(myDims);
    if (myDims.size() < 2)
    {
        throw CiftiFileException("Matrix has less than 2 dimensions");
    }
    return myDims[1];
}

int64_t CiftiFile::getNumberOfRows() const
{
    vector<int64_t> myDims;
    m_matrix.getMatrixDimensions(myDims);
    if (myDims.size() < 1)
    {
        throw CiftiFileException("Matrix has no dimensions");
    }
    return myDims[0];
}

//Matrix IO, simply passes through to CiftiMatrix, see header for more info..

