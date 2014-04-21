#ifndef __NIFTI_HEADER_H__
#define __NIFTI_HEADER_H__

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

#include "CaretBinaryFile.h"
#include "VolumeBase.h" //for AbstractHeader, AbstravtVolumeExtension

#include "nifti1.h"
#include "nifti2.h"

#include <vector>

namespace caret
{
    
    struct NiftiExtension : public AbstractVolumeExtension
    {
        int32_t m_ecode;
        ExtensionType getType() const { return NIFTI; }//should probably just say NIFTI, and have a separate version field for when NIFTI2 gets an updated extension format (probably with int64_t)
    };
    
    struct NiftiHeader : public AbstractHeader
    {
        std::vector<CaretPointer<NiftiExtension> > m_extensions;//allow direct access to the extensions
        
        NiftiHeader();
        void read(CaretBinaryFile& inFile);
        int64_t write(CaretBinaryFile& outFile, const int& version = 1, const bool& swapEndian = false) const;//returns new vox_offset, doesn't set it internally
        bool canWriteVersion(const int& version) const;
        bool wasSwapped() const { return m_readSwapped; }
        int versionRead() const { return m_readVersion; }
        HeaderType getType() const { return NIFTI; }//TODO: this should be changed to just "NIFTI", we don't deal with them separately - also, change to getHeaderType()
        
        std::vector<int64_t> getDimensions() const;
        std::vector<std::vector<float> > getSForm() const;
        int64_t getDataOffset() const { return m_header.vox_offset; }
        int16_t getDataType() const { return m_header.datatype; }
        int32_t getIntentCode() const { return m_header.intent_code; }
        const char* getIntentName() const { return m_header.intent_name; }//NOTE: MAY NOT HAVE A NULL TERMINATOR
        bool getDataScaling(double& mult, double& offset) const;//returns false if scaling not needed
        AString toString() const;
        
        void setDimensions(const std::vector<int64_t>& dimsIn);
        void setSForm(const std::vector<std::vector<float> > &sForm);
        void setIntent(const int32_t& code, const char name[16]);
        void setDataType(const int16_t& type);
        void clearDataScaling();
        void setDataScaling(const double& mult, const double& offset);
        ///get the FSL "scale" space
        std::vector<std::vector<float> > getFSLSpace() const;
        
        bool operator==(const NiftiHeader& rhs) const;//for testing purposes
        bool operator!=(const NiftiHeader& rhs) const { return !((*this) == rhs); }
    private:
        nifti_2_header m_header;//storage for header values regardless of version
        int m_readVersion;
        bool m_readSwapped;
        static void swapHeaderBytes(nifti_1_header &header);
        static void swapHeaderBytes(nifti_2_header &header);
        void prepareHeader(nifti_1_header& header) const;//transform internal state into ready to write header struct
        void prepareHeader(nifti_2_header& header) const;
        void setupFrom(const nifti_1_header& header);//error check provided header, and populate members from it
        void setupFrom(const nifti_2_header& header);
        static int typeToNumBits(const int64_t& type);
        int64_t computeVoxOffset(const int& version) const;
    };
    
}

#endif //__NIFTI_HEADER_H__
