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
#include "iostream"
#include "NiftiHeaderIO.h"
#include <QFile>

using namespace caret;
/**
 * Constructor
 *
 * Constructor that reads from the specified file handle
 * @param inputFile the input handle that starts with the NiftiHeaderIO
 */
NiftiHeaderIO::NiftiHeaderIO(const QFile &inputFileIn) throw (NiftiException)
{
    QFile inputFile(inputFileIn.fileName());
    if(inputFile.exists()) readFile(inputFile);
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
void NiftiHeaderIO::readFile(const QFile &inputFileIn) throw (NiftiException)
{
    QFile inputFile(inputFileIn.fileName());
    int bytes_read = 0;
    m_swapNeeded=false;
    niftiVersion = 0;
    nifti_2_header header;
    inputFile.open(QIODevice::ReadOnly);
    bytes_read = inputFile.read((char *)&header, NIFTI1_HEADER_SIZE);
        if(bytes_read < NIFTI1_HEADER_SIZE)
        {
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
            swapHeaderBytes(header);
        }
        nifti1Header.setHeaderStuct(header);
        nifti1Header.setNeedsSwapping(m_swapNeeded)        ;
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
        }
        nifti2Header.setHeaderStuct(header);
        nifti2Header.setNeedsSwapping(m_swapNeeded);
    }
    else
    {
        throw NiftiException("This is not a valid Nifti1/2 File.");
    }
    inputFile.close();
}

void NiftiHeaderIO::swapHeaderBytes(nifti_1_header &header)
{
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

void NiftiHeaderIO::swapHeaderBytes(nifti_2_header &header)
{
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

/**
 * writeFile
 *
 * writes the nifti 1 header to the output file handle
 * @param outputFile
 */
void NiftiHeaderIO::writeFile(const QFile &outputFileIn, NIFTI_BYTE_ORDER byte_order) throw (NiftiException)
{
    QFile outputFile(outputFileIn.fileName());
    if(!outputFile.isOpen())
    {
        if(!outputFile.open(QIODevice::ReadWrite))
        {
            throw NiftiException("There was an error opening the file for writing.");
        }
    }
    if(this->niftiVersion == 1)
    {
        //swap for write if needed
        nifti_1_header header;
        nifti1Header.getHeaderStruct(header);
        if(byte_order == ORIGINAL_BYTE_ORDER && m_swapNeeded) swapHeaderBytes(header);

        if(outputFile.write((char *)&header,NIFTI1_HEADER_SIZE) != NIFTI1_HEADER_SIZE)
        {
            throw NiftiException("The was an error writing the header.");
        }

    }
    else if(this->niftiVersion == 2)
    {
        //swap for write if needed
        nifti_2_header header;
        nifti2Header.getHeaderStruct(header);
        if(byte_order == ORIGINAL_BYTE_ORDER && m_swapNeeded) swapHeaderBytes(header);

        if(outputFile.write((char *)&header,NIFTI2_HEADER_SIZE) != NIFTI2_HEADER_SIZE)
        {
            throw NiftiException("The was an error writing the header.");
        }
    }
    else throw NiftiException("NiftiHeaderIO only currently supports Nifti versions 1 and 2.");
    outputFile.close();
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
    if(this->niftiVersion !=2 && this->niftiVersion != 1) throw NiftiException("Nifti1Header requested, this version is not loaded.");
    if(niftiVersion == 2) {
        header = this->nifti2Header;
    }
    else {
        header = this->nifti1Header;
    }
}

/**
  * setHeader
  *
  * sets a nifti 1 Header, throws an exception if the header is invalid
  * @param header
  **/
void NiftiHeaderIO::setHeader(const Nifti1Header &header) throw (NiftiException)
{
    niftiVersion = 1;
    this->nifti1Header = header;
}

/**
  * setHeader
  *
  * sets a nifti 2 Header, throws an exception if the header is invalid
  * @param header
  **/

void NiftiHeaderIO::setHeader(const Nifti2Header &header) throw (NiftiException)
{
    niftiVersion = 2;
    this->nifti2Header = header;
}

uint64_t NiftiHeaderIO::getVolumeOffset()
{
    if(niftiVersion == 1)
        return this->nifti1Header.getVolumeOffset();
    else if(niftiVersion == 2)
        return this->nifti2Header.getVolumeOffset();
    else return 0;
}

uint64_t NiftiHeaderIO::getExtensionsOffset()
{
    if(niftiVersion == 1)
        return 348;
    else if(niftiVersion == 2)
        return 540;
    else return 0;
}

