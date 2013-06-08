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
#include "CaretAssert.h"
#include "CiftiXML.h"
#include <algorithm>
#include "CiftiXMLReader.h"
#include <vector>
#include "CiftiMatrix.h"


#include <iostream>
#include <FileInformation.h>
using namespace caret;
using namespace std;

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

CiftiFile::CiftiFile(const CacheEnum &caching, const AString &cacheFile) throw (CiftiFileException)
{
    init();
    this->m_caching = caching;
    this->m_cacheFileName = cacheFile;
}

/**
 * constructor
 *
 * Constructor
 *
 * @param fileName name and path of the Cifti File
 *
 */
CiftiFile::CiftiFile(const AString &fileName, const CacheEnum &caching, const AString &cacheFile)
{
    init();
    this->m_caching = caching;
    this->m_cacheFileName = cacheFile;
    this->openFile(fileName,caching);
}

void CiftiFile::setCiftiCacheFile(const AString &fileName)
{
    this->m_cacheFileName = fileName;
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
void CiftiFile::openFile(const AString &fileName, const CacheEnum &caching)
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
            inputFile.open(QIODevice::ReadOnly);

            if (!inputFile.isOpen())
            {
                throw CiftiFileException("unable to open cifti file");//so permissions problems result in an exception, not an abort later
            }
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
        m_matrix.setMatrixFile(fileName, m_cacheFileName);
        CiftiHeader header;
        m_headerIO.getHeader(header);
        std::vector <int64_t> vec;
        
        header.getDimensions(vec);
        if (vec.size() > 0 && m_xml.getColumnMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS)
        {
            m_xml.setColumnNumberOfTimepoints(vec[0]);//vec[0] is number of rows, so length of column
        }
        if (vec.size() > 1 && m_xml.getRowMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS)
        {
            m_xml.setRowNumberOfTimepoints(vec[1]);
        }
        int64_t offset = header.getVolumeOffset();
        m_matrix.setup(vec,offset,m_caching,m_swapNeeded);
    }
    catch (CaretException& e) {
        throw CiftiFileException("Error reading file \"" + fileName + "\": " + e.whatString());
    }
    catch (std::exception& e) {
        throw CiftiFileException("Error reading file \"" + fileName + "\": " + e.what());
    }
    catch (...) {
        throw CiftiFileException("Unknown error reading file \"" + fileName + "\"");
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
    invalidateDataRange();

    //Get XML string and length, which is needed to calculate the vox_offset stored in the Nifti Header
    QByteArray xmlBytes;
    m_xml.writeXML(xmlBytes);
    int length = 8 + xmlBytes.length();


    // update header struct dimensions and vox_offset
    CiftiHeader ciftiHeader;
    if (m_xml.getColumnMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS || m_xml.getRowMappingType() == CIFTI_INDEX_TYPE_TIME_POINTS)
    {
        ciftiHeader.initDenseTimeSeries();
    } else {
        ciftiHeader.initDenseConnectivity();
    }
    //populate header dimensions from the layout
    std::vector<int64_t> dim;
    dim.push_back(m_xml.getNumberOfRows());//rows/columns get put in nifti header backwards, but CiftiHeader doesn't contain handling for this quirk
    dim.push_back(m_xml.getNumberOfColumns());
    ciftiHeader.setDimensions(dim);

    int64_t vox_offset = 544 + length;
    int remainder = vox_offset % 8;
    int padding = 0;
    if (remainder) padding = 8 - remainder;//for 8 byte alignment
    vox_offset += padding;
    length += padding;


    ciftiHeader.setVolumeOffset(vox_offset);
    m_headerIO.setHeader(ciftiHeader);
    
    //setup the matrix
    m_matrix.setMatrixFile(m_fileName, m_cacheFileName);
    m_matrix.setup(dim, vox_offset, this->m_caching, this->m_swapNeeded);
}

/*
 *
 * setup the Cifti Matrix
 * @param dimensions the dimensions of the cifti matrix
 * @param offsetIn the offset, in bytes, of the start of the matrix within the cifti file
 * @param e the caching type of the matrix (ON_DISK or IN_MEMORY)
 * @param needsSwapping the byte order of the matrix (native or swapped)
 */
/*void CiftiFile::setupMatrix(vector<int64_t> &dimensions, const int64_t &offsetIn, const CacheEnum &e, const bool &needsSwapping) throw (CiftiFileException)
{
    m_matrix.setup(dimensions, offsetIn, e, needsSwapping);
}//*/

/** 
 *
 *
 * write the Cifti File
 *
 * @param fileName specifies the name and path of the file to write to
 */
void CiftiFile::writeFile(const AString &fileName)
{
    QFile *file = this->m_matrix.getCacheFile();
    bool writingNewFile = true;
    bool shouldSwap = false;
    
    if(file && m_caching == ON_DISK) 
    {
        AString cacheFileName = file->fileName();
        if(QDir::toNativeSeparators(cacheFileName) == QDir::toNativeSeparators(fileName))
        {
            writingNewFile = false;
            shouldSwap = m_matrix.getNeedsSwapping();
        }
    }    
    
    if(writingNewFile)
    {
        file = new QFile();
        file->setFileName(fileName);
        if(m_caching == IN_MEMORY)
        {
            if (!file->open(QIODevice::WriteOnly))//this function is writeFile, try to open writable at all times
            {
                throw CiftiFileException("encountered error reopening file as writable");
            }
        }
        else
        {
            if (!file->open(QIODevice::ReadWrite))//this function is writeFile, try to open writable at all times
            {
                throw CiftiFileException("encountered error reopening file as writable");
            }
        }        
    }
    file->seek(0);
    
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
    if (shouldSwap)
    {
        m_headerIO.writeFile(*file, SWAPPED_BYTE_ORDER);
    } else {
        m_headerIO.writeFile(*file, NATIVE_BYTE_ORDER);
    }
    file->flush();

    //write out the xml extension
    
    //isWritable DOES NOT CHECK PERMISSIONS
    
    file->seek(540);
    char eflags[4] = {1,0x00,0x00,0x00};
    int32_t ecode = NIFTI_ECODE_CIFTI;
    
    if(shouldSwap) {
        ByteSwapping::swapBytes(&length,1);
        ByteSwapping::swapBytes(&ecode,1);
    }
    file->write(eflags,4);
    file->write((char *)&length,4);
    file->write((char *)&ecode, 4);
    file->write(xmlBytes);
    char nulls[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    file->write(nulls,padding);//pad out null values to 8 byte boundary
    file->flush();

    //write the matrix
    if(writingNewFile) delete file;
    m_matrix.writeToNewFile(fileName,vox_offset);
    
}

bool CiftiFile::isInMemory() const
{
    CacheEnum test;
    m_matrix.getCaching(test);
    return (test == IN_MEMORY);
}

void CiftiFile::convertToInMemory()
{
    if (isInMemory()) return;
    CiftiMatrix newMatrix;
    vector<int64_t> dims;
    m_matrix.getMatrixDimensions(dims);
    newMatrix.setup(dims);
    vector<float> rowScratch(dims[1], 0.0f);//yes, really, row length is second dimension
    for (int64_t i = 0; i < dims[0]; ++i)
    {
        m_matrix.getRow(rowScratch.data(), i, true);//allow it to work on unallocated on-disk cifti
        newMatrix.setRow(rowScratch.data(), i);
    }
    m_matrix = newMatrix;
    CaretAssert(isInMemory());//make sure it knows it is in memory
}

/**
 * destructor
 */
CiftiFile::~CiftiFile()
{

}

/*
 *
 *
 * set the CiftiHeader
 *
 * @param header
 */
// Header IO
/*void CiftiFile::setHeader(const CiftiHeader &header) throw (CiftiFileException)
{   
    m_headerIO.setHeader(header);
    setupMatrix();
}//*/

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
void CiftiFile::setCiftiXML(const CiftiXML & xml, bool useOldMetadata) throw (CiftiFileException)
{
    if (useOldMetadata)
    {
        const map<AString, AString>* oldmd = m_xml.getFileMetaData();
        if (oldmd != NULL)
        {
            map<AString, AString> newmd = *oldmd;
            oldmd = NULL;//don't leave a dangling pointer around
            m_xml = xml;//because this will probably result in a new pointer for the metadata
            map<AString, AString>* changemd = m_xml.getFileMetaData();
            if (changemd != NULL)
            {
                *changemd = newmd;
            }
        } else {
            m_xml = xml;
        }
    } else {
        this->m_xml = xml;
    }
    setupMatrix();//this also populates the header with the dimensions from the CiftiXML object
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

