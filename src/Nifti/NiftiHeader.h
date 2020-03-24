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
#include "VolumeBase.h" //for AbstractHeader, AbstractVolumeExtension

#include "nifti1.h"
#include "nifti2.h"

#include <vector>

namespace caret
{
    
    struct NiftiExtension
    {
        int32_t m_ecode;
        std::vector<char> m_bytes;
    };
    
    struct NiftiHeader : public AbstractHeader
    {
        std::vector<CaretPointer<NiftiExtension> > m_extensions;//allow direct access to the extensions
        
        NiftiHeader();
        NiftiHeader(const NiftiHeader& rhs);
        NiftiHeader& operator=(const NiftiHeader& rhs);
        void read(CaretBinaryFile& inFile);
        void write(CaretBinaryFile& outFile, const int& version = 1, const bool& swapEndian = false);
        bool canWriteVersion(const int& version) const;
        bool isSwapped() const { return m_isSwapped; }
        int version() const { return m_version; }
        HeaderType getType() const { return NIFTI; }
        AbstractHeader* clone() const;
        
        std::vector<int64_t> getDimensions() const;
        std::vector<std::vector<float> > getSForm() const;
        VolumeSpace getVolumeSpace() const;//convenience function
        double getTimeStep() const;//seconds
        int64_t getDataOffset() const { return m_header.vox_offset; }
        int16_t getDataType() const { return m_header.datatype; }
        int32_t getIntentCode() const { return m_header.intent_code; }
        const char* getIntentName() const { return m_header.intent_name; }//NOTE: 16 BYTES, MAY NOT HAVE A NULL TERMINATOR
        const char* getDescription() const { return m_header.descrip; }//NOTE: 80 BYTES, MAY NOT HAVE A NULL TERMINATOR
        bool getDataScaling(double& mult, double& offset) const;//returns false if scaling not needed
        int getNumComponents() const;
        bool hasGoodSpatialInformation() const;
        QString toString() const;
        
        void setDimensions(const std::vector<int64_t>& dimsIn);
        void setSForm(const std::vector<std::vector<float> > &sForm);
        void setTimeStep(const double& seconds);
        void setIntent(const int32_t& code, const char name[16]);
        void setDescription(const char descrip[80]);
        void setDataType(const int16_t& type);
        void clearDataScaling();
        void setDataScaling(const double& mult, const double& offset);
        void setDataTypeAndScaleRange(const int16_t& type, const double& minval, const double& maxval);
        
        ///get the FSL "scale" space
        std::vector<std::vector<float> > getFSLSpace() const;
        
        bool operator==(const NiftiHeader& rhs) const;//for testing purposes
        bool operator!=(const NiftiHeader& rhs) const { return !((*this) == rhs); }
    private:
        struct Quirks
        {
            bool no_extender;
            Quirks() { no_extender = false; }
        };
        nifti_2_header m_header;//storage for header values regardless of version
        int m_version;
        bool m_isSwapped;
        static void swapHeaderBytes(nifti_1_header &header);
        static void swapHeaderBytes(nifti_2_header &header);
        void prepareHeader(nifti_1_header& header) const;//transform internal state into ready to write header struct
        void prepareHeader(nifti_2_header& header) const;
        Quirks setupFrom(const nifti_1_header& header, const AString& filename);//error check provided header, and populate members from it
        Quirks setupFrom(const nifti_2_header& header, const AString& filename);
        static int typeToNumBits(const int64_t& type);
        int64_t computeVoxOffset(const int& version) const;
    };
    
}

#endif //__NIFTI_HEADER_H__
