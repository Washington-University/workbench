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
    QDir fpath(this->m_fileName);
	m_fileName = fpath.toNativeSeparators(this->m_fileName);


    if(!QFile::exists(m_fileName))//opening file for writing
    {        
        newFile=true;
        return;
    }
    QFile file;
    gzFile zFile;
    if(isCompressed())
    {
		zFile = gzopen(m_fileName.toAscii().data(), "rb");
    }
    else
    {
        file.setFileName(m_fileName);
        file.open(QIODevice::ReadOnly);
    }

    //read header
    if(isCompressed())
        headerIO.readFile(zFile);
    else
        headerIO.readFile(file);

    //read Extension Bytes, eventually a class will handle this
    int64_t vOffset = headerIO.getVolumeOffset();
    int64_t eOffset = headerIO.getExtensionsOffset();
    int64_t eLength = vOffset-eOffset;
    if(extension_bytes) delete [] extension_bytes;
    extension_bytes = new int8_t[eLength];
    if(this->isCompressed())
    {
        gzseek(zFile,eOffset,0);
        gzread(zFile,extension_bytes,eLength);
    }
    else
    {
        file.seek(eOffset);
        file.read((char *)extension_bytes, eLength);
    }

    if(headerIO.getSwapNeeded())
    {
        swapExtensionsBytes(extension_bytes, eLength);
    }

    //set up Matrix    
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
    if(isCompressed())
        matrix.readFile(zFile);
    else
        matrix.readFile(file);
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
	this->m_fileName = fileName;
    QDir fpath(this->m_fileName);
	m_fileName = fpath.toNativeSeparators(this->m_fileName);
    
    //write extension code

    int64_t vOffset = headerIO.getVolumeOffset();
    int64_t eOffset = headerIO.getExtensionsOffset();
    int64_t eLength = vOffset-eOffset;
    if(eLength >4)
    {
        //check for NATIVE_BYTE_ORDER and if it needs swapping
        if(byteOrder != NATIVE_BYTE_ORDER)
        {
            swapExtensionsBytes(extension_bytes, eLength);
        }

    }
    else if(!extension_bytes)//extension doesn't exist, just write four 0x00
    {
        extension_bytes = new int8_t [4];
        memset(extension_bytes,0x00,4);
    }    

    //uggh, needs an output layout
    //until then, will hack around this...
    
    LayoutType layoutOrig;
    matrix.getMatrixLayoutOnDisk(layoutOrig);
    
    LayoutType newLayout = layoutOrig;
    //yes, layouttype needs a method for defining the default
    //layout....
    if(byteOrder != NATIVE_BYTE_ORDER)
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
    matrix.setMatrixLayoutOnDisk(newLayout);
    matrix.setMatrixOffset(headerIO.getVolumeOffset());
    //need better handling for different matrices, for later
    
    QFile file;
    gzFile zFile;
    if(isCompressed())
    {
        zFile = gzopen(m_fileName.toAscii().data(), "wb");
        headerIO.writeFile(zFile,byteOrder);
        gzwrite(zFile,extension_bytes,eLength);
        matrix.writeFile(zFile);
        gzclose(zFile);
    }
    else
    {
        file.setFileName(m_fileName);
        file.open(QIODevice::WriteOnly);
        headerIO.writeFile(file, byteOrder);
        file.write((char *)extension_bytes,eLength);
        matrix.writeFile(file);
        file.close();
    }

    matrix.setMatrixLayoutOnDisk(layoutOrig);
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

void NiftiFile::readVolumeFile(VolumeBase &vol, const AString &filename) throw (NiftiException)
{
    NiftiAbstractHeader *aHeader = new NiftiAbstractHeader();
    NiftiAbstractVolumeExtension *aVolumeExtension = new NiftiAbstractVolumeExtension;

	this->m_fileName = filename;
    QDir fpath(this->m_fileName);
	m_fileName = fpath.toNativeSeparators(this->m_fileName);
    this->openFile(m_fileName);

    headerIO.getAbstractHeader(*aHeader);
    this->getAbstractVolumeExtension(*aVolumeExtension);
    vol.m_header = aHeader;
    vol.m_extensions.push_back(aVolumeExtension);

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
    matrix.getVolume(vol);
}

void NiftiFile::writeVolumeFile(VolumeBase &vol, const AString &filename) throw (NiftiException)
{
    headerIO.setAbstractHeader(*(NiftiAbstractHeader*)&(vol.m_header));
    this->setAbstractVolumeExtension(*(NiftiAbstractVolumeExtension*)(vol.m_extensions[0]));

    Nifti2Header header;
    headerIO.getHeader(header);
    matrix.setMatrixLayoutOnDisk(header);
    matrix.setVolume(vol);
	this->m_fileName = filename;
    QDir fpath(this->m_fileName);
	m_fileName = fpath.toNativeSeparators(this->m_fileName);
    writeFile(filename);
}



