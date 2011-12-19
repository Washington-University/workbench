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
#include "Nifti1Header.h"
#include <vector>
#include "stdint.h"

using namespace caret;



/**
 * Constructor
 *
 * Constructor that takes an input nifti_1_header struct
 *
 * @param header
 */
Nifti1Header::Nifti1Header(const nifti_1_header &header) throw (NiftiException)
{
    if(header.sizeof_hdr != NIFTI1_HEADER_SIZE) throw NiftiException( "Invalid Nifti Header.");
    memcpy((void *)&m_header,&header,sizeof(m_header));
}

/**
 * Default Constructor
 *
 * Default Constructor
 */
Nifti1Header::Nifti1Header() throw (NiftiException)
{
    initHeaderStruct(m_header);
}

/**
 * Destructor
 *
 * Destructor
 */
Nifti1Header::~Nifti1Header()
{
}

/**
 * getHeaderAsString
 *
 * creates a formatted string containing the Nifti2 Header data, this
 * is useful for printing out the Nifti2 Header in a human readable
 * format.
 * @return string containing human readable Nifti 2 Header
 */
void Nifti1Header::getHeaderAsString(QString &string)
{
    string += "header size: " + QString::number (m_header.sizeof_hdr) + "\n";
    string += "Data Type: " + QString::number(m_header.datatype) + "\n";
    string += "bitpix: " + QString::number(m_header.bitpix) + "\n";
    string += "dim_info: " + QString::number(m_header.dim_info) + "\n";
    string += "dim[0]: " + QString::number(m_header.dim[0]) + "\n";
    string += "intent_p1: " + QString::number(m_header.intent_p1) + "\n";
    string += "intent_p2: " + QString::number(m_header.intent_p2) + "\n";
    string += "intent_p3: " + QString::number(m_header.intent_p3) + "\n";
    string += "pixdim[0]: " + QString::number(m_header.pixdim[0] ) + "\n";
    string += "pixdim[1]: " + QString::number(m_header.pixdim[1]) + "\n";
    string += "pixdim[2]: " + QString::number(m_header.pixdim[2]) + "\n";
    string += "pixdim[3]: " + QString::number(m_header.pixdim[3]) + "\n";
    string += "pixdim[4]: " + QString::number(m_header.pixdim[4]) + "\n";
    string += "pixdim[5]: " + QString::number(m_header.pixdim[5]) + "\n";
    string += "pixdim[6]: " + QString::number(m_header.pixdim[6]) + "\n";
    string += "pixdim[7]: " + QString::number(m_header.pixdim[7]) + "\n";
    string += "vox_offset: " + QString::number(m_header.vox_offset) + "\n";
    string += "scl_scope: " + QString::number(m_header.scl_slope) + "\n";
    string += "scl_inter: " + QString::number(m_header.scl_inter) + "\n";
    string += "cal_max: " + QString::number(m_header.cal_max) + "\n";
    string += "cal_min: " + QString::number(m_header.cal_min) + "\n";
    string += "slice_duration: " + QString::number(m_header.slice_duration) + "\n";
    string += "toffset: " + QString::number(m_header.toffset) + "\n";
    string += "slice_start: " + QString::number(m_header.slice_start) + "\n";
    string += "slice_end: " + QString::number(m_header.slice_end) + "\n";
    string += "descrip: " + QString::fromAscii(m_header.descrip) + "\n";
    string += "aux_file: " + QString::fromAscii(m_header.aux_file) + "\n";
    string += "qform_code: " + QString::number(m_header.qform_code) + "\n";
    string += "sform_code: " + QString::number(m_header.sform_code) + "\n";
    string += "quatern_b: " + QString::number(m_header.quatern_b) + "\n";
    string += "quatern_c: " + QString::number(m_header.quatern_c) + "\n";
    string += "quatern_d: " + QString::number(m_header.quatern_d) + "\n";
    string += "qoffset_x: " + QString::number(m_header.qoffset_x) + "\n";
    string += "qoffset_y: " + QString::number(m_header.qoffset_y) + "\n";
    string += "qoffset_z: " + QString::number(m_header.qoffset_z) + "\n";
    string += "srow_x[0]: " + QString::number(m_header.srow_x[0]) + "\n";
    string += "srow_x[1]: " + QString::number(m_header.srow_x[1]) + "\n";
    string += "srow_x[2]: " + QString::number(m_header.srow_x[2]) + "\n";
    string += "srow_x[3]: " + QString::number(m_header.srow_x[3]) + "\n";
    string += "srow_y[0]: " + QString::number(m_header.srow_y[0]) + "\n";
    string += "srow_y[1]: " + QString::number(m_header.srow_y[1]) + "\n";
    string += "srow_y[2]: " + QString::number(m_header.srow_y[2]) + "\n";
    string += "srow_y[3]: " + QString::number(m_header.srow_y[3]) + "\n";
    string += "srow_z[0]: " + QString::number(m_header.srow_z[0]) + "\n";
    string += "srow_z[1]: " + QString::number(m_header.srow_z[1]) + "\n";
    string += "srow_z[2]: " + QString::number(m_header.srow_z[2]) + "\n";
    string += "srow_z[3]: " + QString::number(m_header.srow_z[3]) + "\n";
    string += "slice_code: " + QString::number(m_header.slice_code) + "\n";
    string += "xyzt_units: " + QString::number(m_header.xyzt_units) + "\n";
    string += "intent_code: " + QString::number(m_header.intent_code) + "\n";
    string += "intent_name: " + QString::fromAscii(m_header.intent_name) + "\n";
    string += "magic : " + QString::fromAscii(m_header.magic) + "\n";
    //string + m_header.unused_str + "\n";    
}

/**
 * getHeaderStruct
 *
 * get the raw nifti_1_header struct, as defined in nifti2.h
 * @param header
 */
void Nifti1Header::getHeaderStruct(nifti_1_header &header) const throw (NiftiException)
{
    memcpy(&header, &m_header, sizeof(m_header));
}

/**
 * setHeaderStruct
 *
 * sets the raw nifti_1_header struct, as defined in nifti2.h
 * @param header
 */
void Nifti1Header::setHeaderStuct(const nifti_1_header &header) throw (NiftiException)
{
    memcpy(&m_header, &header, sizeof(m_header));
}



void Nifti1Header::initHeaderStruct()
{
    initHeaderStruct(this->m_header);
}

/**
 * initHeaderStruct
 *
 * initializes the supplied nifti 1 header struct to sensible default
 * @param header
 */
void Nifti1Header::initHeaderStruct(nifti_1_header &header)
{
    header.sizeof_hdr = NIFTI1_HEADER_SIZE;
    memcpy(header.magic, "n+1\0",4);
    header.datatype = 0;
    header.bitpix = 0;//TODO
    header.dim[0] = 0;//TODO
    header.intent_p1 = 0;
    header.intent_p2 = 0;
    header.intent_p3 = 0;
    header.pixdim[0] = 0.0;header.pixdim[1] = 1.0;
    header.pixdim[2] = 1.0;header.pixdim[3] = 1.0;
    header.pixdim[4] = 1.0;header.pixdim[5] = 1.0;
    header.pixdim[6] = 1.0;header.pixdim[7] = 1.0;
    header.vox_offset = 544;//TODO, currently set to minimum value
    header.scl_slope = 1;
    header.scl_inter = 0;
    header.cal_max = 0;
    header.cal_min = 0;
    header.slice_duration = 0;
    header.toffset = 0;
    header.slice_start = 0;
    header.slice_end = 0;
    memset(header.descrip,0x00,80);
    memset(header.aux_file,0x00,24);
    header.qform_code =0.0;
    header.sform_code =0.0;
    header.quatern_b = 0.0;
    header.quatern_c = 0.0;
    header.quatern_d = 0.0;
    header.qoffset_x = 0.0;
    header.qoffset_y = 0.0;
    header.qoffset_z = 0.0;
    header.srow_x[0] = 0.0;
    header.srow_x[1] = 0.0;
    header.srow_x[2] = 0.0;
    header.srow_x[3] = 0.0;
    header.srow_y[0] = 0.0;
    header.srow_y[1] = 0.0;
    header.srow_y[2] = 0.0;
    header.srow_y[3] = 0.0;
    header.srow_z[0] = 0.0;
    header.srow_z[1] = 0.0;
    header.srow_z[2] = 0.0;
    header.srow_z[3] = 0.0;
    header.slice_code = 0;
    header.xyzt_units = 0;//TODO
    header.intent_code = NIFTI_INTENT_NONE;
    memset(header.intent_name,0x00,16);
    header.dim_info = 0;
    needsSwapping = false;
    needsSwappingSet = false;
}

void Nifti1Header::getDimensions(std::vector< int64_t > &dimensionsOut) const
{
    dimensionsOut.clear();
    dimensionsOut.resize(m_header.dim[0]);
    for(int i = 0;i<(int)dimensionsOut.size();i++)
    {
        dimensionsOut[i]=m_header.dim[i+1];
    }
}

void Nifti1Header::setDimensions(const std::vector<int64_t> &dimensionsIn) throw (NiftiException)
{
    if(dimensionsIn.size()>7) throw NiftiException("Number of dimensions exceeds currently allowed nift1 dimension number.");
    m_header.dim[0] = dimensionsIn.size();
    for(int i =0;i<(int)dimensionsIn.size();i++)
    {
        m_header.dim[i+1]=dimensionsIn[i];
    }
}
void Nifti1Header::getNiftiDataTypeEnum(NiftiDataTypeEnum::Enum &enumOut) const
{
    bool isValid;
    enumOut = NiftiDataTypeEnum::fromIntegerCode( m_header.datatype,&isValid);
}
void Nifti1Header::setNiftiDataTypeEnum(const NiftiDataTypeEnum::Enum &enumIn)
{
    m_header.datatype = (short) NiftiDataTypeEnum::toIntegerCode(enumIn);
}

void Nifti1Header::getComponentDimensions(int32_t &componentDimensionsOut) const
{
    componentDimensionsOut = 1;
    if(m_header.datatype == NIFTI_TYPE_RGB24) componentDimensionsOut = 3;
}

void Nifti1Header::getValueByteSize(int32_t &valueByteSizeOut) const throw(NiftiException)
{
    //for the sake of clarity, the Size suffix refers to size of bytes in memory, and Length suffix refers to the length of an array
    switch(m_header.datatype) {
    case NIFTI_TYPE_FLOAT32:
        valueByteSizeOut = sizeof(float);
        break;
    case NIFTI_TYPE_FLOAT64:
        valueByteSizeOut = sizeof(double);
        break;
    case NIFTI_TYPE_RGB24:
        valueByteSizeOut = 1;
        break;
    default:
        throw NiftiException("Unsupported Data Type.");
    }
}

void Nifti1Header::getSForm(std::vector < std::vector <float> > &sForm)
{
    sForm.resize(4);
    for(uint i = 0;i<sForm.size();i++) sForm[i].resize(4);
    for(int i = 0;i<4;i++)
    {
        sForm[0][i] = m_header.srow_x[i];
        sForm[1][i] = m_header.srow_y[i];
        sForm[2][i] = m_header.srow_z[i];
        sForm[3][i] = 0.0f;
    }
    sForm[3][3] = 1.0f;
}

void Nifti1Header::setSForm(const std::vector < std::vector <float> > &sForm)
{
    if(sForm.size()<3) return;//TODO should throw an exception
    for(uint i = 0;i<sForm.size();i++) if(sForm.size() <4 ) return;
    for(int i = 0;i<4;i++)
    {
        m_header.srow_x[i] = sForm[0][i];
        m_header.srow_y[i] = sForm[1][i];
        m_header.srow_z[i] = sForm[2][i];
    }
}

