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

#include "NiftiIO.h"

#include "DataFileException.h"

using namespace std;
using namespace caret;

void NiftiIO::openRead(const QString& filename)
{
    m_file.open(filename);
    m_header.read(m_file);
    if (m_header.getDataType() == DT_BINARY)
    {
        throw DataFileException("file uses the binary datatype, which is unsupported: " + filename);
    }
    m_dims = m_header.getDimensions();
    int64_t filesize = m_file.size();//returns -1 if it can't efficiently determine size
    int64_t elemCount = getNumComponents();
    for (int i = 0; i < (int)m_dims.size(); ++i)
    {
        elemCount *= m_dims[i];
    }
    if (filesize >= 0 && filesize < m_header.getDataOffset() + numBytesPerElem() * elemCount)
    {
        throw DataFileException("nifti file is truncated: " + filename);
    }
}

void NiftiIO::writeNew(const QString& filename, const NiftiHeader& header, const int& version, const bool& withRead, const bool& swapEndian)
{
    if (header.getDataType() == DT_BINARY)
    {
        throw DataFileException("writing NIFTI with binary datatype is unsupported");
    }
    if (withRead)
    {
        m_file.open(filename, CaretBinaryFile::READ_WRITE_TRUNCATE);//for cifti on-disk writing, replace structure with along row needs to RMW
    } else {
        m_file.open(filename, CaretBinaryFile::WRITE_TRUNCATE);
    }
    m_header = header;
    m_header.write(m_file, version, swapEndian);
    m_dims = m_header.getDimensions();
}

void NiftiIO::close()
{
    m_file.close();
    m_dims.clear();
}

int NiftiIO::getNumComponents() const
{
    return m_header.getNumComponents();
}

int NiftiIO::numBytesPerElem()
{
    switch (m_header.getDataType())
    {
        case NIFTI_TYPE_INT8:
        case NIFTI_TYPE_UINT8:
        case NIFTI_TYPE_RGB24:
            return 1;
            break;
        case NIFTI_TYPE_INT16:
        case NIFTI_TYPE_UINT16:
            return 2;
            break;
        case NIFTI_TYPE_INT32:
        case NIFTI_TYPE_UINT32:
        case NIFTI_TYPE_FLOAT32:
        case NIFTI_TYPE_COMPLEX64:
            return 4;
            break;
        case NIFTI_TYPE_INT64:
        case NIFTI_TYPE_UINT64:
        case NIFTI_TYPE_FLOAT64:
        case NIFTI_TYPE_COMPLEX128:
            return 8;
            break;
        case NIFTI_TYPE_FLOAT128:
        case NIFTI_TYPE_COMPLEX256:
            return 16;
            break;
        default:
            CaretAssert(0);
            throw DataFileException("internal error, report what you did to the developers");
    }
}
