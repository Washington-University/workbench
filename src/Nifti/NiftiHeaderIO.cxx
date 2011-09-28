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

#include <QtCore>
#include "nifti1.h"
#include "iostream"
#include "NiftiException.h"
#include "Nifti1Header.h"
#include "NiftiHeaderIO.h"
#include <QFile>

using namespace caret;
/**
 * Constructor
 *
 * Constructor that opens and reads from the specified file
 * @param inputFileName the name of the input file that starts with the NiftiHeaderIO
 */
NiftiHeaderIO::NiftiHeaderIO(const QString& inputFileName) throw (NiftiException)
{
    //test to see if file exists
    if(!QFile::exists(inputFileName)) throw NiftiException("File " + AString(inputFileName) +" does not exist.\n");
    readFile(inputFileName);
}

/**
 * Constructor
 *
 * Constructor that reads from the specified file handle
 * @param inputFile the input handle that starts with the NiftiHeaderIO
 */
NiftiHeaderIO::NiftiHeaderIO(QFile &inputFile) throw (NiftiException)
{
    readFile(inputFile);
}


/**
 * readFile
 *
 * read NiftiHeaderIO data from file name, closes file when finished loading data
 * @param inputFileName name and path of Nifti2 Header file
 */
void NiftiHeaderIO::readFile(const QString& inputFileName) throw (NiftiException)
{
    QFile inputFile;
    inputFile.setFileName(inputFileName);
    inputFile.open(QIODevice::ReadOnly);
    this->readFile(inputFile);
    inputFile.close();

}

/**
 * getSwapNeeded
 *
 * determines whether the byte order needs to be swapped, i.e. if
 * the Nifti file is in big endian format, but the machine architecture
 * is little-endian, then a swap would be necessary
 * @return swapNeeded true if swap is needed
 */
bool NiftiHeaderIO::getSwapNeeded()
{
    return m_swapNeeded;
}



/**
 * Constructor
 *
 * Constructor that takes an input nifti_2_header struct *
 * @param header
 */
void NiftiHeaderIO::readFile(QFile &inputFile) throw (NiftiException)
{
    int bytes_read = 0;
    m_swapNeeded=false;
    niftiVersion = 0;
    nifti_2_header header;
    bytes_read = inputFile.read((char *)&header, NIFTI2_HEADER_SIZE);
    if(bytes_read < NIFTI2_HEADER_SIZE) {
        throw NiftiException("Error reading Nifti header, file is too short.");
    }
    if((NIFTI2_VERSION(header))==1)
    {
        niftiVersion = 1;
        inputFile.seek(0);
        nifti_1_header header;
        inputFile.read((char *)&header, NIFTI1_HEADER_SIZE);
        if(NIFTI2_NEEDS_SWAP(header))
        {
            m_swapNeeded=true;
            ByteSwapping::swapBytes(&(header.sizeof_hdr),1);
            ByteSwapping::swapBytes(&(header.datatype),1);
            ByteSwapping::swapBytes(&(header.bitpix),1);
            ByteSwapping::swapBytes(&(header.dim[0]),8);
            ByteSwapping::swapBytes(&(header.intent_p1),1);
            ByteSwapping::swapBytes(&(header.intent_p2),1);
            ByteSwapping::swapBytes(&(header.intent_p3),1);
            ByteSwapping::swapBytes(&(header.pixdim[0]),8);
            ByteSwapping::swapBytes(&(header.vox_offset),1);
            ByteSwapping::swapBytes(&(header.scl_slope),1);
            ByteSwapping::swapBytes(&(header.scl_inter),1);
            ByteSwapping::swapBytes(&(header.cal_max),1);
            ByteSwapping::swapBytes(&(header.cal_min),1);
            ByteSwapping::swapBytes(&(header.slice_duration),1);
            ByteSwapping::swapBytes(&(header.toffset),1);
            ByteSwapping::swapBytes(&(header.slice_start),1);
            ByteSwapping::swapBytes(&(header.slice_end),1);
            ByteSwapping::swapBytes(&(header.qform_code),1);
            ByteSwapping::swapBytes(&(header.sform_code),1);
            ByteSwapping::swapBytes(&(header.quatern_b),1);
            ByteSwapping::swapBytes(&(header.quatern_c),1);
            ByteSwapping::swapBytes(&(header.quatern_d),1);
            ByteSwapping::swapBytes(&(header.qoffset_x),1);
            ByteSwapping::swapBytes(&(header.qoffset_y),1);
            ByteSwapping::swapBytes(&(header.qoffset_z),1);
            ByteSwapping::swapBytes(&(header.srow_x[0]),4);
            ByteSwapping::swapBytes(&(header.srow_y[0]),4);
            ByteSwapping::swapBytes(&(header.srow_z[0]),4);
            //ByteSwapping::swapBytes(&(header.slice_code),1);
            //ByteSwapping::swapBytes(&(header.xyzt_units),1);
            ByteSwapping::swapBytes(&(header.intent_code),1);
        }
        nifti1Header.setHeaderStuct(header);
    } else if((NIFTI2_VERSION(header))==2)
    {
        niftiVersion = 2;
        if(NIFTI2_NEEDS_SWAP(header))
        {
            m_swapNeeded = true;
            inputFile.seek(0);
            nifti_1_header header;
            bytes_read = inputFile.read((char *)&header, NIFTI2_HEADER_SIZE);
            if(bytes_read < NIFTI2_HEADER_SIZE) {
                throw NiftiException("Error reading Nifti2 header, file is too short.");
            }
            ByteSwapping::swapBytes(&(header.sizeof_hdr),1);
            ByteSwapping::swapBytes(&(header.datatype),1);
            ByteSwapping::swapBytes(&(header.bitpix),1);
            ByteSwapping::swapBytes(&(header.dim[0]),8);
            ByteSwapping::swapBytes(&(header.intent_p1),1);
            ByteSwapping::swapBytes(&(header.intent_p2),1);
            ByteSwapping::swapBytes(&(header.intent_p3),1);
            ByteSwapping::swapBytes(&(header.pixdim[0]),8);
            ByteSwapping::swapBytes(&(header.vox_offset),1);
            ByteSwapping::swapBytes(&(header.scl_slope),1);
            ByteSwapping::swapBytes(&(header.scl_inter),1);
            ByteSwapping::swapBytes(&(header.cal_max),1);
            ByteSwapping::swapBytes(&(header.cal_min),1);
            ByteSwapping::swapBytes(&(header.slice_duration),1);
            ByteSwapping::swapBytes(&(header.toffset),1);
            ByteSwapping::swapBytes(&(header.slice_start),1);
            ByteSwapping::swapBytes(&(header.slice_end),1);
            ByteSwapping::swapBytes(&(header.qform_code),1);
            ByteSwapping::swapBytes(&(header.sform_code),1);
            ByteSwapping::swapBytes(&(header.quatern_b),1);
            ByteSwapping::swapBytes(&(header.quatern_c),1);
            ByteSwapping::swapBytes(&(header.quatern_d),1);
            ByteSwapping::swapBytes(&(header.qoffset_x),1);
            ByteSwapping::swapBytes(&(header.qoffset_y),1);
            ByteSwapping::swapBytes(&(header.qoffset_z),1);
            ByteSwapping::swapBytes(&(header.srow_x[0]),4);
            ByteSwapping::swapBytes(&(header.srow_y[0]),4);
            ByteSwapping::swapBytes(&(header.srow_z[0]),4);
            //ByteSwapping::swapBytes(&(header.slice_code),1);
            //ByteSwapping::swapBytes(&(header.xyzt_units),1);
            ByteSwapping::swapBytes(&(header.intent_code),1);
        }
        nifti2Header.setHeaderStuct(header);
    }
    else
    {
        throw NiftiException("This is not a valid Nifti1/2 File.");
    }
}

/**
 * writeFile
 *
 * writes the nifti 1 header to the output file handle
 * @param outputFile
 */
void NiftiHeaderIO::writeFile(QFile &outputFile) const throw (NiftiException)
{
    if(!outputFile.isOpen())
    {
        if(!outputFile.open(QIODevice::WriteOnly))
        {
            throw NiftiException("There was an error opening the file for writing.");
        }
    }
    if(this->niftiVersion == 1)
    {
        if(outputFile.write((char *)&nifti1Header,NIFTI1_HEADER_SIZE) != NIFTI1_HEADER_SIZE)
        {
            throw NiftiException("The was an error writing the header.");
        }
    }
    else if(this->niftiVersion == 2)
    {
        if(outputFile.write((char *)&nifti2Header,NIFTI1_HEADER_SIZE) != NIFTI1_HEADER_SIZE)
        {
            throw NiftiException("The was an error writing the header.");
        }

    }
    else throw NiftiException("NiftiHeaderIO only currently supports Nifti versions 1 and 2.");
    //outputFile.close();
}

/**
  *
  * returns the niftiVersion of the Header, either 1 or 2.  In the event that a Nifti file
  * hasn't been read, 0 is returned.
  * @return niftiVersion
  */
int NiftiHeaderIO::getNiftiVersion()
{
    return niftiVersion;
}

/**
 * writeFile
 *
 * writes the nifti 2 header to the output file name
 * @param outputFileName
 */
void NiftiHeaderIO::writeFile(const QString &outputFileName) const throw (NiftiException)
{
    QFile outputFile;
    outputFile.setFileName(outputFileName);
    this->writeFile(outputFile);
    outputFile.close();
}

/**
  * getHeader
  *
  * gets a nifti 1 Header, throws an exception if the version does not match.
  * @param header
  **/
void NiftiHeaderIO::getHeader(Nifti1Header &header) const throw (NiftiException)
{
    if(this->niftiVersion !=1) throw NiftiException("Nifti1Header requested, this version is not loaded.");

    header = this->nifti1Header;
}

/**
  * getHeader
  *
  * gets a nifti 2 Header, throws an exception if the version does not match.
  * @param header
  **/
void NiftiHeaderIO::getHeader(Nifti2Header &header) const throw (NiftiException)
{
    if(this->niftiVersion !=2) throw NiftiException("Nifti1Header requested, this version is not loaded.");

    header = this->nifti2Header;
}
