/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#ifndef NIFTI_HEADER_IO_H
#define NIFTI_HEADER_IO_H

#include <QtCore>
#include <iostream>
#include "NiftiException.h"
#include "Nifti1Header.h"
#include "Nifti2Header.h"
#include "NiftiAbstractHeader.h"
#include "zlib.h"

#include <vector>

namespace caret {

/// Class for determining Nifti Header version and return correct (nifti 1 or 2) Header version
class NiftiHeaderIO {
public:
    NiftiHeaderIO() throw (NiftiException) { niftiVersion = 2; m_swapNeeded = false; }//TSC: default to the default constructed nifti 2 header
    NiftiHeaderIO(const AString &inputFile) throw (NiftiException);
    ~NiftiHeaderIO() { }

    void readFile(const AString &inputFile) throw (NiftiException);
    void writeFile(const AString &outputFile, NIFTI_BYTE_ORDER byteOrder = NATIVE_BYTE_ORDER) throw (NiftiException);
    void writeFile(QFile &file, NIFTI_BYTE_ORDER byteOrder = NATIVE_BYTE_ORDER) throw (NiftiException);
    void writeFile(gzFile file, NIFTI_BYTE_ORDER byteOrder = NATIVE_BYTE_ORDER) throw (NiftiException);
    void readFile(QFile &file) throw (NiftiException);
    void readFile(gzFile file) throw (NiftiException);    

    bool isCompressed(const AString &fileName) const;

    void getHeader(Nifti1Header &header) const throw (NiftiException);
    void setHeader(const Nifti1Header &header) throw (NiftiException);

    void getHeader(Nifti2Header &header) const throw (NiftiException);
    void setHeader(const Nifti2Header &header) throw (NiftiException);
    
    
    ///get the true space of the volume as an sform
    std::vector<std::vector<float> > getSForm() const;
    ///get the FSL "scale" space for a nifti header
    std::vector<std::vector<float> > getFSLSpace() const;
    
    void swapHeaderBytes(nifti_1_header &header);
    void swapHeaderBytes(nifti_2_header &header);
    bool getSwapNeeded();
    int getNiftiVersion();
    void setVolumeOffset(const int64_t &offsetIn);
    int64_t getVolumeOffset();
    int64_t getExtensionsOffset();
    void getAbstractHeader(NiftiAbstractHeader &header)
    {
        if(getNiftiVersion() == 1)
        {
            header.m_niftiVersion = 1;
            getHeader(header.n1header);
        }
        else if(getNiftiVersion() == 2)
        {
            header.m_niftiVersion = 2;
            getHeader(header.n2header);
        }          
    }

    void setAbstractHeader(NiftiAbstractHeader &header)
    {
        if(header.m_niftiVersion == 1)
        {
            this->niftiVersion = 1;
            setHeader(header.n1header);
        }
        else if(header.m_niftiVersion == 2)
        {
            this->niftiVersion = 2;
            setHeader(header.n2header);
        }
    }

    void fixDimensions(nifti_1_header &header);
    void fixDimensions(nifti_2_header &header);

private:
    int niftiVersion;
    bool m_swapNeeded;
    Nifti1Header nifti1Header;
    Nifti2Header nifti2Header;
};

} // namespace caret

#endif // NIFTI_HEADER_IO_H
