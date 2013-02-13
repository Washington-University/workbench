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

#ifndef NIFTI_HEADER_H
#define NIFTI_HEADER_H

#include <QtCore>
#include "NiftiEnums.h"
#include "iostream"
#include "NiftiException.h"
#include "ByteSwapping.h"
#include <vector>

#include "nifti1.h"

#define NIFTI1_HEADER_SIZE 348

namespace caret {

/// Simple Container class for storing Nifti1Header data
class Nifti1Header {
public:
    Nifti1Header() throw (NiftiException);

    Nifti1Header(const nifti_1_header &header) throw (NiftiException);
    ~Nifti1Header();
    void getHeaderStruct(nifti_1_header &header) const throw (NiftiException);
    void setHeaderStuct(const nifti_1_header &header) throw (NiftiException);
    void getHeaderAsString(QString &string);
    void initHeaderStruct(nifti_1_header &header);
    void initHeaderStruct();
    //helper methods
    void getDimensions(std::vector <int64_t> &dimensionsOut) const;
    void setDimensions(const std::vector <int64_t> &dimensionsIn) throw (NiftiException) ;
    void getNiftiDataTypeEnum(NiftiDataTypeEnum::Enum &enumOut) const;
    void setNiftiDataTypeEnum(const NiftiDataTypeEnum::Enum &enumIn);
    void getComponentDimensions(int32_t &componentDimensionsOut) const;
    void getValueByteSize(int32_t &valueByteSizeOut) const throw(NiftiException);
    void getNeedsSwapping(bool &needsSwappingOut) const { needsSwappingOut = needsSwapping; }
    void getScaling(double &slopeOut, double &interceptOut) const { slopeOut=m_header.scl_slope;interceptOut=m_header.scl_inter; }
    void setNeedsSwapping(bool &needsSwappingIn) throw (NiftiException) {
        if(needsSwappingSet) throw NiftiException("This attribute is read-only, and can only be set when reading the header.");
        needsSwappingSet = true;
        needsSwapping = needsSwappingIn;
    }
    void getSForm(std::vector < std::vector <float> > &sForm) const;
    void setSForm(const std::vector < std::vector <float> > &sForm);

    void setVolumeOffset(const int64_t &offsetIn) { m_header.vox_offset = offsetIn; }
    int64_t getVolumeOffset() { return m_header.vox_offset; }

private:
    nifti_1_header m_header;
    //this hack was added in so that Nifti matrix could get all the information it needed for reading/writing matrix with just the header, otherwise the user
    //would need to call a separate function to set byte order reading, which is error prone
    bool needsSwapping;
    bool needsSwappingSet;//this can only be set once, making it essentially a read only attribute since we don't support big-endian byte order writing, only reading
};

} // namespace caret

#endif // NIFTI_HEADER_H
