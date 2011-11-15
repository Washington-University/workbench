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


#include <algorithm>
#include <vector>
#include <iostream>

using namespace caret;

/**
 * Default Constructor
 *
 */
NiftiFile::NiftiFile() throw (NiftiException)
{
    init();
}

/**
 * constructor
 *
 * Constructor
 *
 * @param fileName name and path of the Nifti File
 * currently only IN_MEMORY is supported
 */
NiftiFile::NiftiFile(const AString &fileName) throw (NiftiException)
{
    init();
    this->openFile(fileName);
}

void NiftiFile::init()
{
    extension_bytes = NULL;
    newFile = false;
}

bool NiftiFile::isCompressed()
{
    if(m_fileName.length()!=0)
    {
        if(m_fileName.endsWith(".gz")) return true;
        else return false;

    }
    else return false;
}

/**
 *
 *
 * open a Nifti file
 *
 * @param fileName name and path of the Nifti File
 */
void NiftiFile::openFile(const AString &fileName) throw (NiftiException)
{
    this->m_fileName = fileName;
    if(!QFile::exists(fileName))//opening file for writing
    {
        matrix.setMatrixFile(m_fileName);
        newFile=true;
        return;
    }
    //read header
    headerIO.readFile(fileName);

    //read Extension Bytes, eventually a class will handle this
    int64_t vOffset = headerIO.getVolumeOffset();
    int64_t eOffset = headerIO.getExtensionsOffset();
    int64_t eLength = vOffset-eOffset;
    if(extension_bytes) delete [] extension_bytes;
    extension_bytes = new int8_t[eLength];
    if(this->isCompressed())
    {
        AString temp = fileName;
        gzFile ext = gzopen(temp.toStdString().c_str(), "rb");
        gzseek(ext,eOffset,0);
        gzread(ext,extension_bytes,eLength);
        gzclose(ext);
    }
    else
    {
        QFile ext(fileName);
        ext.open(QIODevice::ReadWrite);
        ext.seek(eOffset);
        ext.read((char *)extension_bytes, eLength);
        ext.close();
    }

    //set up Matrix
    matrix.setMatrixFile(fileName);
    if(this->getNiftiVersion()==1) {
        Nifti1Header header;
        headerIO.getHeader(header);
        matrix.setMatrixLayoutOnDisk(header);
        matrix.setMatrixOffset((header.getVolumeOffset()));
    }
    else if(this->getNiftiVersion()==2)
    {
        Nifti2Header header;
        headerIO.getHeader(header);
        matrix.setMatrixLayoutOnDisk(header);
        matrix.setMatrixOffset((header.getVolumeOffset()));
    }
}

void NiftiFile::swapExtensionsBytes(int8_t *bytes, const int64_t &extensionLength)
{
    if(!bytes[0]) return;
    int64_t currentIndex = 4;//skip over the extension char array
    while(currentIndex < extensionLength)
    {
        //byte swap esize
        ByteSwapping::swapBytes((int32_t *)&bytes[currentIndex],2);
        currentIndex+=*((int32_t *)(&bytes[currentIndex]));
    }
}

/**
 *
 *
 * write the Nifti File
 *
 * @param fileName specifies the name and path of the file to write to
 */
void NiftiFile::writeFile(const AString &fileName, NIFTI_BYTE_ORDER byteOrder) throw (NiftiException)
{     
    if(fileName == m_fileName && !newFile)
    {
        //need to honor byte order
        headerIO.writeFile(fileName,ORIGINAL_BYTE_ORDER);

        //write extension code
        //since we never swapped to begin with, and we are
        //sticking with the original byte order we can just
        //write the bytes
        int64_t vOffset = headerIO.getVolumeOffset();
        int64_t eOffset = headerIO.getExtensionsOffset();
        int64_t eLength = vOffset-eOffset;
        if(eLength>4)
        {
            QFile ext(fileName);
            ext.open(QIODevice::ReadWrite);
            ext.seek(eOffset);
            ext.write((char *)extension_bytes, eLength);
            ext.close();
        }
        else //extension doesn't exist, just write four 0x00's
        {
            uint8_t bytes[4] = {0x00,0x00,0x00,0x00};
            QFile ext(fileName);
            ext.open(QIODevice::ReadWrite);
            ext.seek(eOffset);
            ext.write((char *)bytes, 4);
            ext.close();
        }
        //write matrix
        //need to flush last frame when writing in place, all other writes
        //have occured
        matrix.flushCurrentFrame();
    }
    else //we are writing a new file
    {
        //TODO: When extension class is done, Nifti vox offset, and matrix offset will need to be updated to include size of extension, since it will have changed.
        headerIO.writeFile(fileName,byteOrder);

        //write extension code

        int64_t vOffset = headerIO.getVolumeOffset();
        int64_t eOffset = headerIO.getExtensionsOffset();
        int64_t eLength = vOffset-eOffset;
        if(eLength >4)
        {
            //confusing, but we never swapped these bytes when we read them in, so
            //we need to swap them now if the byte order to write is native
            //and swap is needed
            //check for NATIVE_BYTE_ORDER and if it needs swapping
            if(byteOrder == NATIVE_BYTE_ORDER && headerIO.getSwapNeeded())
            {
                swapExtensionsBytes(extension_bytes, eLength);
            }
            QFile ext(fileName);
            ext.open(QIODevice::ReadWrite);
            ext.seek(eOffset);
            ext.write((char *)extension_bytes, eLength);
            ext.close();
        }
        else //extension doesn't exist, just write four 0x00
        {
            uint8_t bytes[4] = {0x00,0x00,0x00,0x00};
            QFile ext(fileName);
            ext.open(QIODevice::ReadWrite);
            ext.seek(eOffset);
            ext.write((char *)bytes, 4);
            ext.close();
        }

        //uggh, needs an output layout
        //until then, will hack around this...
        //oh, and this won't work for RGB...
        LayoutType layoutOrig;
        matrix.getMatrixLayoutOnDisk(layoutOrig);
        NiftiMatrix outMatrix(fileName);
        LayoutType newLayout = layoutOrig;
        //yes, layouttype needs a method for defining the default
        //layout....
        if(byteOrder == ORIGINAL_BYTE_ORDER && headerIO.getSwapNeeded())
        {
            newLayout.needsSwapping = true;
        }
        else
        {
            newLayout.needsSwapping = false;
        }
        bool valid = true;
        newLayout.niftiDataType = NiftiDataTypeEnum::fromIntegerCode(NIFTI_TYPE_FLOAT32,&valid);
        if(!valid) throw NiftiException("Nifti Enum bites it again.");
        outMatrix.setMatrixLayoutOnDisk(newLayout);
        //need to check if we're dealing with a time series, otherwise
        //dim4 may be zero

        int64_t totalTimeSlices= 1;
        if(newLayout.dimensions[3]!=0 && newLayout.dimensions[0]==4) {
            totalTimeSlices = newLayout.dimensions[3];
        }
        //need better handling for different matrices, for later
        float * frame = new float[matrix.getFrameLength()];

        for(int64_t t=0;t<totalTimeSlices;t++)
        {
            matrix.readFrame(t);
            matrix.getFrame(frame);
            outMatrix.setFrame(frame,matrix.getFrameLength(),t);
            outMatrix.flushCurrentFrame();
        }
        delete [] frame;
    }
}

/**
 * destructor
 */
NiftiFile::~NiftiFile()
{
    if(extension_bytes) delete [] extension_bytes;
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
    headerIO.setHeader(header);
    matrix.setMatrixLayoutOnDisk(header);
}

/**
 *
 *
 * get a copy of the NiftiHeader
 *
 * @param header
 */
void NiftiFile::getHeader(Nifti1Header &header) throw (NiftiException)
{
    headerIO.getHeader(header);
    matrix.setMatrixLayoutOnDisk(header);
}

/**
 *
 *
 * set the Nifti2Header
 *
 * @param header
 */
// Header IO
void NiftiFile::setHeader(const Nifti2Header &header) throw (NiftiException)
{
    headerIO.setHeader(header);
}


/**
 *
 *
 * get a copy of the Nifti2Header
 *
 * @param header
 */
void NiftiFile::getHeader(Nifti2Header &header) throw (NiftiException)
{
    headerIO.getHeader(header);
}

/**
 *
 *
 * get a copy of the Nifti2Header
 *
 * @return version
 */

int NiftiFile::getNiftiVersion()
{
    return headerIO.getNiftiVersion();
}

// Volume IO

void NiftiFile::getVolumeFrame(VolumeBase &frameOut, const int64_t timeSlice, const int64_t component)
{
    //get dimensions, sform and component size
    Nifti2Header header;
    headerIO.getHeader(header);
    std::vector< std::vector<float> >  sForm(4);
    for(uint i=0;i<sForm.size();i++) sForm[i].resize(4);
    header.getSForm(sForm);
    std::vector<int64_t> dim;
    header.getDimensions(dim);
    int32_t components;
    header.getComponentDimensions(components);
    //set up frame out dimensions
    frameOut.reinitialize(dim,sForm,components);

    matrix.getVolumeFrame(frameOut,timeSlice,component);
}

void NiftiFile::setVolumeFrame(VolumeBase &frameIn, const int64_t & timeSlice, const int64_t component)
{
    //get dimensions, sform and component size
    Nifti2Header header;
    headerIO.getHeader(header);
    std::vector< std::vector<float> > sForm(4);
    for(uint i=0;i<sForm.size();i++) sForm[i].resize(4);
    header.getSForm(sForm);
    std::vector<int64_t> dim;
    header.getDimensions(dim);
    int32_t components;
    header.getComponentDimensions(components);

    frameIn.reinitialize(dim,sForm,components);

    matrix.setVolumeFrame(frameIn,timeSlice,component);
}

void NiftiFile::readVolumeFile(VolumeBase &vol, const AString &filename) throw (NiftiException)
{
    this->openFile(filename);

    //get dimensions, sform and component size
    Nifti2Header header;
    headerIO.getHeader(header);
    std::vector< std::vector<float> > sForm(4);
    for(uint i=0;i<sForm.size();i++) sForm[i].resize(4);
    header.getSForm(sForm);
    std::vector<int64_t> dim;
    header.getDimensions(dim);
    int32_t components;
    header.getComponentDimensions(components);

    vol.reinitialize(dim,sForm,components);
    int64_t timeSlices = 1;
    if(dim.size()==4) timeSlices = dim[3];
    //TODO, for now components are always 0, rewrite for RGB
    for(int64_t t=0;t<timeSlices;t++)
    {
        matrix.getVolumeFrame(vol,t,0);
    }
}

void NiftiFile::getHeaderFromVolumeFile(VolumeBase &vol, Nifti1Header &header)
{
    /*need to get the following:
     dimensions
     spacing (store to pixdim)
     sform_row_*
     orientation
    */
    /* implied defaults:
      DATA TYPE IS FLOAT32
      INTENT CODE IS NIFTI_INTENT_NONE
      vox_offset is 548 for now, since we don't support labels yet
      qform code is NIFTI_XFORM_TALAIRACH
      sform code is NIFTI_XFORM_TALAIRACH??
      qform (how do get that from sform, or is it necessary?)
      quatern_?, again how do we get that?
      intent_name "NIFTI_INTENT_NONE"
      scl_slope = 0
      scl_inter = 0
      slice_start = 0
      slice_end = 0
      slice_code =0
      xyzt_units = ??
      intent_p1,2,3 = 0
      */
}

void NiftiFile::getHeaderFromVolumeFile(VolumeBase &vol, Nifti2Header &header)
{



}

void NiftiFile::writeVolumeFile(VolumeBase &vol, const AString &filename) throw (NiftiException)
{
    //get dimensions, sform and component size
    std::vector <int64_t> dim;

    int64_t timeSlices = 1;
    if(dim.size()==4) timeSlices = dim[3];
    //TODO, for now components are always 0, rewrite for RGB
    for(int64_t t=0;t<timeSlices;t++)
    {
        matrix.setVolumeFrame(vol,t,0);
    }
}



