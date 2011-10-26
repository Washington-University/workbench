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

#include "CiftiHeader.h"

using namespace caret;
CiftiHeader::CiftiHeader()
{
}


void CiftiHeader::initHeaderStruct()
{
    initHeaderStruct(this->m_header);
}

/**
 * initHeaderStruct
 *
 * initializes the supplied nifti 2 header struct to sensible defaults for Nifti
 * @param header
 */
void CiftiHeader::initHeaderStruct(nifti_2_header &header)
{
    header.sizeof_hdr = NIFTI2_HEADER_SIZE;
    memcpy(header.magic, "n+2\0\r\n\032\n",8);
    header.datatype = NIFTI_TYPE_FLOAT32;
    header.bitpix = 32;//TODO
    header.dim[0] = 0;//TODO
    for(int i=1;i<8;i++) header.dim[i]=1;
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
    header.qform_code = NIFTI_XFORM_UNKNOWN;
    header.sform_code = NIFTI_XFORM_UNKNOWN;
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
    header.xyzt_units = 0xC;//TODO
    header.intent_code = NIFTI_INTENT_NONE;
    memset(header.intent_name,0x00,16);
    header.dim_info = 0;
    memset(header.unused_str,0x00,15);
    needsSwapping = false;
    needsSwappingSet = false;
}

void CiftiHeader::initDenseTimeSeries()
{
    initHeaderStruct();
    m_header.intent_code = NIFTI_INTENT_CONNECTIVITY_DENSE_TIME;
    memcpy(m_header.intent_name,"ConnDenseTime",sizeof("ConnDenseTime"));
    m_header.dim[0]=6;
}

void CiftiHeader::initDenseConnectivity()
{
    initHeaderStruct();
    m_header.intent_code = NIFTI_INTENT_CONNECTIVITY_DENSE;
    memcpy(m_header.intent_name,"ConnDense",sizeof("ConnDense"));
    m_header.dim[0]=6;
}
