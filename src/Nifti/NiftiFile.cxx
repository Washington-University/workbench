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
NiftiFile::NiftiFile(const QString &fileName) throw (NiftiException)
{
   init();
   this->openFile(fileName);
}

void NiftiFile::init()
{
    extension_bytes = NULL;

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
    //read header
    headerIO.readFile(fileName);

    //read Extension Bytes, eventually a class will handle this
    int64_t vOffset = headerIO.getVolumeOffset();
    int64_t eOffset = headerIO.getExtensionsOffset();
    int64_t eLength = vOffset-eOffset;
    QFile ext(fileName);
    ext.open(QIODevice::ReadWrite);
    ext.seek(eOffset);
    ext.read((char *)extension_bytes, eLength);
    ext.close();

    //set up Matrix
    matrix.setMatrixFile(fileName);
    Nifti2Header header;
    headerIO.getHeader(header);
    matrix.setMatrixLayoutOnDisk(header);
    matrix.setMatrixOffset((header.getVolumeOffset()));
}

/**
 *
 *
 * write the Nifti File
 *
 * @param fileName specifies the name and path of the file to write to
 */
void NiftiFile::writeFile(const QString &fileName, NIFTI_BYTE_ORDER byteOrder) throw (NiftiException)
{
    if(fileName == m_fileName)
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
        QFile ext(fileName);
        ext.open(QIODevice::ReadWrite);
        ext.seek(eOffset);
        ext.write((char *)extension_bytes, eLength);
        ext.close();
        //write matrix
        //need to flush last frame when writing in place, all other writes
        //have occured
        matrix.flushCurrentFrame();
    }
    else //assume we are writing a new file
    {
        headerIO.writeFile(fileName,byteOrder);
        //write extension code

        int64_t vOffset = headerIO.getVolumeOffset();
        int64_t eOffset = headerIO.getExtensionsOffset();
        int64_t eLength = vOffset-eOffset;
        QFile ext(fileName);
        ext.open(QIODevice::ReadWrite);
        ext.seek(eOffset);
        ext.write((char *)extension_bytes, eLength);
        ext.close();

        //uggh, needs an output layout
        //until then, will hack around this...
        //oh, and this won't work for RGB...
        LayoutType layoutOrig;
        matrix.getMatrixLayoutOnDisk(layoutOrig);
        NiftiMatrix outMatrix(fileName);
        LayoutType newLayout = layoutOrig;
        //yes, layouttype needs a method for defining the default
        //layout....
        newLayout.needsSwapping = false;
        bool valid = true;
        newLayout.niftiDataType = NiftiDataTypeEnum::fromIntegerCode(NIFTI_TYPE_FLOAT32,&valid);
        if(!valid) throw NiftiException("Nifti Enum bites it again.");
        newLayout.valueByteSize = 4;
        outMatrix.setMatrixLayoutOnDisk(newLayout);
        //need to check if we're dealing with a time series, otherwise
        //dim4 may be zero

        int64_t totalTimeSlices= 1;
        if(newLayout.dimensions[4]!=0 && newLayout.dimensions[0]==4) {
            totalTimeSlices = newLayout.dimensions[4];
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


// Volume IO

void NiftiFile::getVolumeFrame(VolumeFile &frameOut, const int64_t timeSlice, const int64_t component)
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

void NiftiFile::setVolumeFrame(VolumeFile &frameIn, const int64_t & timeSlice, const int64_t component)
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



