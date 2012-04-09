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
        zFile = gzopen64(m_fileName.toAscii().data(), "rb");
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

    //read extensions, maybe a class should handle this
    int64_t vOffset = headerIO.getVolumeOffset();
    int64_t eOffset = headerIO.getExtensionsOffset();
    if (vOffset > eOffset + 4)//handle incorrect nifti that starts voxel data at the end of the header without the 4 byte extender, I have seen it happen
    {
        char extender[4];
        if (isCompressed())
        {
            if (gzread(zFile, extender, 4) != 4)
            {
                throw NiftiException("failed to read bytes from file");
            }
        } else {
            if (file.read(extender, 4) != 4)
            {
                throw NiftiException("failed to read bytes from file");
            }
        }
        if (extender[0] != 0)
        {
            int64_t curOffset = eOffset + 4;
            while (curOffset + 8 <= vOffset)//ensure reading the elength, ecode doesn't go into the voxel data
            {
                int32_t esize, ecode;
                if (isCompressed())
                {
                    if (gzread(zFile, (void*)&esize, sizeof(int32_t)) != (int)sizeof(int32_t))
                    {//maybe we should wrap gz functions into a class that throws exceptions instead of this C-style error checking
                        throw NiftiException("failed to read bytes from file");
                    }
                    if (gzread(zFile, (void*)&ecode, sizeof(int32_t)) != (int)sizeof(int32_t))
                    {
                        throw NiftiException("failed to read bytes from file");
                    }
                } else {
                    if (file.read((char*)&esize, sizeof(int32_t)) != (qint64)sizeof(int32_t))
                    {//QT didn't make file IO throw on errors? the shame...
                        throw NiftiException("failed to read bytes from file");
                    }
                    if (file.read((char*)&ecode, sizeof(int32_t)) != (qint64)sizeof(int32_t))
                    {
                        throw NiftiException("failed to read bytes from file");
                    }
                }
                if (headerIO.getSwapNeeded())
                {
                    ByteSwapping::swapBytes(&esize, 1);
                    ByteSwapping::swapBytes(&ecode, 1);
                }
                if (esize < 8 || curOffset + esize > vOffset)//esize is signed according to spec, and should take into account the 8 bytes of esize, ecode
                {
                    break;//so, stop immediately if esize doesn't make sense, but allow an extension with no data bytes
                }
                CaretPointer<NiftiAbstractVolumeExtension> tempExt(new NiftiAbstractVolumeExtension());
                tempExt->m_niftiVersion = getNiftiVersion();
                tempExt->m_ecode = ecode;
                if (esize > 8)//don't try to read 0 bytes (CaretArray is smart enough to initialize to NULL with nonpositive size, though)
                {
                    tempExt->m_bytes = CaretArray<char>(esize - 8);
                    if (isCompressed())
                    {
                        if (gzread(zFile, tempExt->m_bytes, esize - 8) != esize - 8)
                        {
                            throw NiftiException("failed to read bytes from file");
                        }
                    } else {
                        if (file.read(tempExt->m_bytes, esize - 8) != esize - 8)
                        {
                            throw NiftiException("failed to read bytes from file");
                        }
                    }
                }
                m_extensions.push_back(tempExt);//NOTE: according to nifti spec, DO NOT swap extension byte data, because we don't know the encoding
                curOffset += esize;//move to the next extension
            }
        }
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

    int64_t eOffset = headerIO.getExtensionsOffset();
    QByteArray extensionBytes;
    extensionBytes.resize(4);
    extensionBytes[0] = 0;
    extensionBytes[1] = 0;
    extensionBytes[2] = 0;
    extensionBytes[3] = 0;
    int numExtensions = (int)m_extensions.size();
    if(numExtensions != 0)
    {
        char padding[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        extensionBytes[0] = 1;
        for (int i = 0; i < numExtensions; ++i)
        {
            int totalBytes = m_extensions[i]->m_bytes.size() + 8, extraBytes = 0;//include size for esize, ecode
            if (totalBytes % 16 != 0)
            {
                extraBytes = 16 - (totalBytes % 16);
                totalBytes += extraBytes;
            }
            int32_t esize = totalBytes;
            int32_t ecode = m_extensions[i]->m_ecode;
            if (byteOrder == SWAPPED_BYTE_ORDER)
            {
                ByteSwapping::swapBytes(&esize, 1);
                ByteSwapping::swapBytes(&ecode, 1);
            }
            extensionBytes.append((char*)&esize, sizeof(int32_t));
            extensionBytes.append((char*)&ecode, sizeof(int32_t));
            extensionBytes.append(m_extensions[i]->m_bytes, m_extensions[i]->m_bytes.size());
            extensionBytes.append(padding, extraBytes);
        }
    }
    headerIO.setVolumeOffset(eOffset + extensionBytes.size());

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
    switch (headerIO.getNiftiVersion())
    {
        case 1:
            {
                Nifti1Header myheader;
                headerIO.getHeader(myheader);
                myheader.setNiftiDataTypeEnum(newLayout.niftiDataType);
                headerIO.setHeader(myheader);
            }
            break;
        case 2:
            {
                Nifti2Header myheader;
                headerIO.getHeader(myheader);
                myheader.setNiftiDataTypeEnum(newLayout.niftiDataType);
                headerIO.setHeader(myheader);
            }
            break;
        default:
            throw NiftiException("error, header uninitialized in NiftiFile::writeFile");
    }
    matrix.setMatrixLayoutOnDisk(newLayout);
    matrix.setMatrixOffset(headerIO.getVolumeOffset());
    //need better handling for different matrices, for later
    
    QFile file;
    gzFile zFile;
    if(isCompressed())
    {
        zFile = gzopen64(m_fileName.toAscii().data(), "wb");
        headerIO.writeFile(zFile,byteOrder);
        if (gzwrite(zFile, extensionBytes.constData(), extensionBytes.size()) != extensionBytes.size())
        {
            throw NiftiException("failed to write bytes to file");
        }
        matrix.writeFile(zFile);
        gzclose(zFile);
    }
    else
    {
        file.setFileName(m_fileName);
        file.open(QIODevice::WriteOnly);
        headerIO.writeFile(file, byteOrder);
        if (file.write(extensionBytes.constData(), extensionBytes.size()) != extensionBytes.size())
        {
            throw NiftiException("failed to write bytes to file");
        }
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
    CaretPointer<NiftiAbstractHeader> aHeader(new NiftiAbstractHeader());

    this->m_fileName = filename;
    QDir fpath(this->m_fileName);
    m_fileName = fpath.toNativeSeparators(this->m_fileName);
    this->openFile(m_fileName);

    headerIO.getAbstractHeader(*aHeader);
    vol.m_header = aHeader;
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
    int numExtensions = (int)m_extensions.size();
    for (int i = 0; i < numExtensions; ++i)
    {
        vol.m_extensions.push_back(m_extensions[i]);
    }

    matrix.getVolume(vol);
}

void NiftiFile::writeVolumeFile(VolumeBase &vol, const AString &filename) throw (NiftiException)
{
    if (vol.m_header != NULL)
    {
        switch (vol.m_header->getType())
        {
            case AbstractHeader::NIFTI1:
            case AbstractHeader::NIFTI2:
                headerIO.setAbstractHeader(*(NiftiAbstractHeader*)vol.m_header.getPointer());
                break;
            default:
                break;//discard unknown header types
        };
    }
    int numExtensions = (int)vol.m_extensions.size();
    for (int i = 0; i < numExtensions; ++i)
    {
        switch (vol.m_extensions[i]->getType())
        {
            case AbstractVolumeExtension::NIFTI1://these are actually fully compatible as far as we know, so handle them together
            case AbstractVolumeExtension::NIFTI2://inheritance works only one way, so we need to make a new CaretPointer to a new NiftiAbstractExtension and point its m_bytes to the same place
                {
                    CaretPointer<NiftiAbstractVolumeExtension> tempExt(new NiftiAbstractVolumeExtension());
                    NiftiAbstractVolumeExtension* toCopy = (NiftiAbstractVolumeExtension*)vol.m_extensions[i].getPointer();
                    tempExt->m_ecode = toCopy->m_ecode;
                    tempExt->m_niftiVersion = toCopy->m_niftiVersion;
                    tempExt->m_bytes = toCopy->m_bytes;//make a dumb shallow copy, thats okay because we don't modify it
                    m_extensions.push_back(tempExt);
                }//have make it a block because switch hates initializations
                break;
            default:
                break;//ignore unknown extensions since we don't know the ecode for them
        }
    }

    if (vol.m_header == NULL)
    {//default to nifti1 for now
        Nifti1Header header;
        header.setNiftiDataTypeEnum(NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32);
        header.setSForm(vol.getVolumeSpace());
        std::vector<int64_t> myOrigDims;
        myOrigDims = vol.getOriginalDimensions();
        int components = vol.getNumberOfComponents();
        if (components != 1) throw NiftiException("writing multi-component volumes not implemented");
        header.setDimensions(myOrigDims);
        headerIO.setHeader(header);
        matrix.setMatrixLayoutOnDisk(header);
    } else if (vol.m_header->getType() == AbstractHeader::NIFTI1) {
        Nifti1Header header;
        headerIO.getHeader(header);
        header.setSForm(vol.getVolumeSpace());
        std::vector<int64_t> myOrigDims;
        myOrigDims = vol.getOriginalDimensions();
        int components = vol.getNumberOfComponents();
        if (components != 1) throw NiftiException("writing multi-component volumes not implemented");
        header.setDimensions(myOrigDims);
        headerIO.setHeader(header);
        matrix.setMatrixLayoutOnDisk(header);
    } else if (vol.m_header->getType() == AbstractHeader::NIFTI2) {
        Nifti2Header header;
        headerIO.getHeader(header);
        header.setSForm(vol.getVolumeSpace());
        std::vector<int64_t> myOrigDims;
        myOrigDims = vol.getOriginalDimensions();
        int components = vol.getNumberOfComponents();
        if (components != 1) throw NiftiException("writing multi-component volumes not implemented");
        header.setDimensions(myOrigDims);
        headerIO.setHeader(header);
        matrix.setMatrixLayoutOnDisk(header);
    }
    matrix.setVolume(vol);
    this->m_fileName = filename;
    QDir fpath(this->m_fileName);
    m_fileName = fpath.toNativeSeparators(this->m_fileName);
    writeFile(filename);
}
