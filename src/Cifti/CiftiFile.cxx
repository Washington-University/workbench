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
CiftiFile::CiftiFile()
{
    init();    
}

CiftiFile::CiftiFile(const CacheEnum &caching, const AString &cacheFile)
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
            inputFile.seek(sizeof(nifti_2_header));
            char extensions[4];
            inputFile.read(extensions,4);
            if (extensions[0] == 0)
            {
                throw CiftiFileException("cifti files require a nifti extension");
            }
            if (extensions[0] != 1 || extensions[1] != 0 || extensions[2] != 0 || extensions[3] != 0)
            {
                throw CiftiFileException("unrecognized nifti extender bytes");
            }
            unsigned int length;
            inputFile.read((char *)&length, 4);
            if(m_swapNeeded)ByteSwapping::swapBytes(&length,1);
            unsigned int ecode;
            inputFile.read((char *)&ecode,4);
            if(m_swapNeeded)ByteSwapping::swapBytes(&ecode,1);
            if((int32_t)ecode != NIFTI_ECODE_CIFTI) throw CiftiFileException("Error reading extension.  Extension Code is not Cifti.");//TODO: not assume that the first (only) extension is cifti
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
        int64_t offset = header.getVolumeOffset();
        if (vec.size() < 1) throw CiftiFileException("cannot use 0-dimensional cifti");
        if (m_xml.getParsedVersion().hasReversedFirstDims())//deal with cifti-1 nastiness
        {
            if (vec.size() == 1)
            {
                vec.push_back(vec[0]);
                vec[0] = 1;
            } else {
                int64_t temp = vec[0];
                vec[0] = vec[1];
                vec[1] = temp;
            }
        }
        if ((int)vec.size() != m_xml.getNumberOfDimensions()) throw CiftiFileException("xml and nifti header disagree on cifti matrix dimensionality");
        for (int i = 0; i < (int)vec.size(); ++i)
        {
            if (m_xml.getDimensionLength(i) < 1)//should only happen with cifti-1
            {
                m_xml.getSeriesMap(i).setLength(vec[0]);
            }
        }
        while (vec.size() < 2) vec.push_back(1);//HACK: in case we get 1D cifti, which CiftiMatrix will choke on
        int64_t temp = vec[0];//HACK: CiftiMatrix expects reversed dims
        vec[0] = vec[1];
        vec[1] = temp;
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

void CiftiFile::setupMatrix()
{
    invalidateDataRange();

    //Get XML string and length, which is needed to calculate the vox_offset stored in the Nifti Header
    QByteArray xmlBytes = m_xml.writeXMLToQByteArray();//HACK: if writing on-disk, this must match the value later computed in writeFile()
    int length = 8 + xmlBytes.length();


    // update header struct dimensions and vox_offset
    CiftiHeader ciftiHeader;
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::SERIES || m_xml.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SERIES)
    {
        ciftiHeader.initDenseTimeSeries();//TODO: replace with code to generate the correct intent code
    } else {
        ciftiHeader.initDenseConnectivity();
    }
    //populate header dimensions from the layout
    vector<int64_t> dim;
    int numDims = m_xml.getNumberOfDimensions();
    if (numDims < 1) throw CiftiFileException("cannot use 0-dimensional cifti");
    for (int i = 0; i < numDims; ++i)
    {
        dim.push_back(m_xml.getDimensionLength(i));//0 is along row, 1 is along column, etc
    }
    if (m_xml.getParsedVersion().hasReversedFirstDims())//deal with cifti-1 nastiness
    {
        vector<int64_t> reversed = dim;
        if (reversed.size() < 2)
        {
            reversed.push_back(dim[0]);
            reversed[0] = 1;
        } else {
            reversed[0] = dim[1];
            reversed[1] = dim[0];
        }
        ciftiHeader.setDimensions(reversed);
    } else {
        ciftiHeader.setDimensions(dim);
    }

    int64_t vox_offset = 544 + length;
    int remainder = vox_offset % 16;
    int padding = 0;
    if (remainder) padding = 16 - remainder;//for 8 byte alignment
    vox_offset += padding;
    length += padding;


    ciftiHeader.setVolumeOffset(vox_offset);
    m_headerIO.setHeader(ciftiHeader);
    
    //setup the matrix
    m_matrix.setMatrixFile(m_fileName, m_cacheFileName);
    while (dim.size() < 2) dim.push_back(1);//HACK: CiftiMatrix won't take 1D, and we need to swap dims to make CiftiMatrix happy
    int64_t temp = dim[0];
    dim[0] = dim[1];
    dim[1] = temp;
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
    QByteArray xmlBytes = m_xml.writeXMLToQByteArray();//HACK: if writing on-disk, this must match the value from setupMatrix()
    int length = 8 + xmlBytes.length();


    // update header struct dimensions and vox_offset
    CiftiHeader ciftiHeader;
    this->m_headerIO.getHeader(ciftiHeader);


    int64_t vox_offset = 544 + length;
    int remainder = vox_offset % 16;
    int padding = 0;
    if (remainder) padding = 16 - remainder;//for 16 byte alignment
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
    char nulls[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
void CiftiFile::getHeader(CiftiHeader &header)
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
void CiftiFile::setCiftiXML(const CiftiXML & xml, bool useOldMetadata)
{
    if (useOldMetadata)
    {
        const GiftiMetaData* oldmd = m_xml.getFileMetaData();
        if (oldmd != NULL)
        {
            GiftiMetaData newmd = *oldmd;
            oldmd = NULL;//don't leave a dangling pointer around
            m_xml = xml;//because this will probably result in a new pointer for the metadata
            GiftiMetaData* changemd = m_xml.getFileMetaData();
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

void CiftiFile::setCiftiXML(const CiftiXMLOld& xml, const bool useOldMetadata)
{
    QString xmlText;
    xml.writeXML(xmlText);
    CiftiXML tempXML;//so that we can use the same code path
    tempXML.readXML(xmlText);
    if (tempXML.getDimensionLength(CiftiXML::ALONG_ROW) < 1)
    {
        CiftiSeriesMap tempMap = tempXML.getSeriesMap(CiftiXML::ALONG_ROW);
        tempMap.setLength(xml.getDimensionLength(CiftiXMLOld::ALONG_ROW));
        tempXML.setMap(CiftiXML::ALONG_ROW, tempMap);
    }
    if (tempXML.getDimensionLength(CiftiXML::ALONG_COLUMN) < 1)
    {
        CiftiSeriesMap tempMap = tempXML.getSeriesMap(CiftiXML::ALONG_COLUMN);
        tempMap.setLength(xml.getDimensionLength(CiftiXMLOld::ALONG_COLUMN));
        tempXML.setMap(CiftiXML::ALONG_COLUMN, tempMap);
    }
    setCiftiXML(tempXML, useOldMetadata);
}

bool CiftiFile::setRowTimestep(const float& seconds)
{
    if (m_xml.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::SERIES) return false;
    m_xml.getSeriesMap(CiftiXML::ALONG_ROW).setStep(seconds);
    return true;
}

bool CiftiFile::setColumnTimestep(const float& seconds)
{
    if (m_xml.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::SERIES) return false;
    m_xml.getSeriesMap(CiftiXML::ALONG_COLUMN).setStep(seconds);
    return true;
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

