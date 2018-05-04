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

#include "NiftiHeader.h"

#include "ByteSwapping.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FloatMatrix.h"
#include "MathFunctions.h"

#include <cmath>
#include <cstring>
#include <limits>
#include "stdint.h"

using namespace std;
using namespace caret;

NiftiHeader::NiftiHeader()
{
    if (sizeof(nifti_1_header) != 348 || sizeof(nifti_2_header) != 540)//these should be made into static asserts when we move to c++11 or decide to use boost
    {
        throw DataFileException("internal error: nifti header structs are the wrong size");//this is not a runtime assert, because we want this checked in release builds too
    }
    memset(&m_header, 0, sizeof(m_header));
    for (int i = 0; i < 8; ++i)
    {
        m_header.dim[i] = 1;//we maintain 1s on unused dimensions to make some stupid nifti readers happy
        m_header.pixdim[i] = 1.0f;//also set pixdims to 1 by default, to make some other nifti readers happy when reading cifti headers
    }//note that we still warn when asking for spacing info and qform and sform codes are both 0, so this doesn't prevent us from catching non-volume files loaded as volumes
    m_header.xyzt_units = SPACE_TIME_TO_XYZT(NIFTI_UNITS_MM, NIFTI_UNITS_SEC);
    m_header.scl_slope = 1.0;//default to identity scaling
    m_header.scl_inter = 0.0;
    m_header.datatype = NIFTI_TYPE_FLOAT32;
    m_header.bitpix = typeToNumBits(m_header.datatype);
    m_version = 0;
    m_isSwapped = false;
}

AbstractHeader* NiftiHeader::clone() const
{
    return new NiftiHeader(*this);
}

NiftiHeader::NiftiHeader(const NiftiHeader& rhs)
{
    m_header = rhs.m_header;
    m_isSwapped = rhs.m_isSwapped;
    m_version = rhs.m_version;
    m_extensions.reserve(rhs.m_extensions.size());
    for (size_t i = 0; i < rhs.m_extensions.size(); ++i)
    {
        m_extensions.push_back(CaretPointer<NiftiExtension>(new NiftiExtension(*(rhs.m_extensions[i]))));
    }
}

NiftiHeader& NiftiHeader::operator=(const NiftiHeader& rhs)
{
    if (this == &rhs) return *this;
    m_header = rhs.m_header;
    m_isSwapped = rhs.m_isSwapped;
    m_version = rhs.m_version;
    m_extensions.clear();
    m_extensions.reserve(rhs.m_extensions.size());
    for (size_t i = 0; i < rhs.m_extensions.size(); ++i)
    {
        m_extensions.push_back(CaretPointer<NiftiExtension>(new NiftiExtension(*(rhs.m_extensions[i]))));
    }
    return *this;
}

bool NiftiHeader::canWriteVersion(const int& version) const
{
    if (computeVoxOffset(version) < 0) return false;//error condition, can happen if an extension is longer than 2^31
    if (version == 2) return true;//our internal state is nifti-2, return early
    if (version != 1) return false;//we can only write 1 and 2
    vector<int64_t> dims = getDimensions();
    for (int i = 0; i < (int)dims.size(); ++i)
    {
        if (dims[i] > numeric_limits<int16_t>::max()) return false;
    }
    if (m_header.intent_code > numeric_limits<int16_t>::max() || m_header.intent_code < numeric_limits<int16_t>::min()) return false;
    if (m_header.slice_code > numeric_limits<char>::max() || m_header.slice_code < numeric_limits<char>::min()) return false;
    if (m_header.xyzt_units > numeric_limits<char>::max() || m_header.xyzt_units < numeric_limits<char>::min()) return false;
    if (m_header.qform_code > numeric_limits<int16_t>::max() || m_header.qform_code < numeric_limits<int16_t>::min()) return false;
    if (m_header.sform_code > numeric_limits<int16_t>::max() || m_header.sform_code < numeric_limits<int16_t>::min()) return false;
    return true;
}

int64_t NiftiHeader::computeVoxOffset(const int& version) const
{
    int64_t ret;
    switch (version)
    {
        case 1:
            ret = 4 + sizeof(nifti_1_header);//the 4 is the extender bytes
            break;
        case 2:
            ret = 4 + sizeof(nifti_2_header);
            break;
        default:
            return -1;
    }
    int numExtensions = (int)m_extensions.size();
    for (int i = 0; i < numExtensions; ++i)
    {
        CaretAssert(m_extensions[i] != NULL);
        int64_t thisSize = 8 + m_extensions[i]->m_bytes.size();//8 is for the int32_t size and ecode for nifti-1 style extensions
        if (thisSize % 16 != 0)//round up to nearest multiple of 16
        {
            int paddingBytes = 16 - (thisSize % 16);
            thisSize += paddingBytes;
        }
        if (thisSize > numeric_limits<int32_t>::max()) return -1;//since we don't have nifti-2 style extensions yet, always fail
        ret += thisSize;
    }
    if (version == 1)//need to put it into a float exactly (yes, really)
    {
        float temp = ret;
        if (ret != (int64_t)temp) return -1;//for now, just fail, until it actually becomes a problem
    }
    return ret;
}

bool NiftiHeader::getDataScaling(double& mult, double& offset) const
{
    if (m_header.datatype == NIFTI_TYPE_RGB24 || m_header.scl_slope == 0.0 || (m_header.scl_slope == 1.0 && m_header.scl_inter == 0.0))//the "if slope is zero" case is in the nifti spec
    {
        mult = 1.0;//in case someone ignores the boolean
        offset = 0.0;
        return false;
    }
    mult = m_header.scl_slope;
    offset = m_header.scl_inter;
    return true;
}

vector<int64_t> NiftiHeader::getDimensions() const
{
    CaretAssert(m_header.dim[0] >= 0 && m_header.dim[0] <= 7);//because storage is private and initialized to zero, so it should never be invalid
    vector<int64_t> ret(m_header.dim[0]);
    for (int i = 0; i < m_header.dim[0]; ++i)
    {
        ret[i] = m_header.dim[i + 1];
    }
    return ret;
}

vector<std::vector<float> > NiftiHeader::getFSLSpace() const
{//don't look at me, blame analyze and flirt
    vector<int64_t> dimensions = getDimensions();
    if (dimensions.size() < 3) throw DataFileException("NiftiHeaderIO has less than 3 dimensions, can't generate the FSL space for it");
    FloatMatrix ret;
    vector<vector<float> > sform = getSForm();
    float determinant = sform[0][0] * sform[1][1] * sform[2][2] +
                        sform[0][1] * sform[1][2] * sform[2][0] +
                        sform[0][2] * sform[1][0] * sform[2][1] -
                        sform[0][2] * sform[1][1] * sform[2][0] -
                        sform[0][0] * sform[1][2] * sform[2][1] -
                        sform[0][1] * sform[1][0] * sform[2][2];//just write out the 3x3 determinant rather than packing it into a FloatMatrix first - and I haven't put a determinant function in yet
    ret = FloatMatrix::identity(4);//generate a 4x4 with 0 0 0 1 last row via FloatMatrix for convenience
    if (determinant > 0.0f)
    {
        ret[0][0] = -m_header.pixdim[1];//yes, they really use pixdim, despite checking the SForm/QForm for flipping - ask them, not me
        ret[0][3] = (dimensions[0] - 1) * m_header.pixdim[1];//note - pixdim[1] is for i, pixdim[0] is qfac
    } else {
        ret[0][0] = m_header.pixdim[1];
    }
    ret[1][1] = m_header.pixdim[2];
    ret[2][2] = m_header.pixdim[3];
    int32_t spaceUnit = XYZT_TO_SPACE(m_header.xyzt_units);
    switch (spaceUnit)
    {
        case NIFTI_UNITS_METER:
            ret *= 1000.0f;
            ret[3][3] = 1.0f;
            break;
        case NIFTI_UNITS_MICRON:
            ret *= 0.001f;
            ret[3][3] = 1.0f;
            break;
        case 0://will already have warned in getSForm()
        case NIFTI_UNITS_MM:
            break;
        default:
            break;//will already have warned in getSForm()
    }
    return ret.getMatrix();
}

bool NiftiHeader::operator==(const NiftiHeader& rhs) const
{
    if (m_version != rhs.m_version) return false;//this is to test for consistency, not to test if two headers mean the same thing
    if (m_isSwapped != rhs.m_isSwapped) return false;
    return memcmp(&m_header, &(rhs.m_header), sizeof(m_header)) == 0;
}

vector<vector<float> > NiftiHeader::getSForm() const
{
    FloatMatrix ret = FloatMatrix::zeros(4, 4);
    ret[3][3] = 1.0f;//force 0 0 0 1 last row
    if (m_header.sform_code != 0)//prefer sform
    {
        for(int i = 0; i < 4; i++)
        {
            ret[0][i] = m_header.srow_x[i];
            ret[1][i] = m_header.srow_y[i];
            ret[2][i] = m_header.srow_z[i];
        }
    } else if (m_header.qform_code != 0) {//fall back to qform
        float rotmat[3][3], quat[4];
        quat[1] = m_header.quatern_b;
        quat[2] = m_header.quatern_c;
        quat[3] = m_header.quatern_d;
        float checkquat = quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3];
        if (checkquat <= 1.01f)//make sure qform is sane
        {
            if (checkquat > 1.0f)
            {
                quat[0] = 0.0f;
            } else {
                quat[0] = sqrt(1.0f - checkquat);
            }
            MathFunctions::quaternToMatrix(quat, rotmat);
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    rotmat[i][j] *= m_header.pixdim[j + 1];
                }
            }
            if (m_header.pixdim[0] < 0.0f)//left handed coordinate system, flip the kvec
            {
                rotmat[0][2] = -rotmat[0][2];
                rotmat[1][2] = -rotmat[1][2];
                rotmat[2][2] = -rotmat[2][2];
            }
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    ret[i][j] = rotmat[i][j];
                }
            }
            ret[0][3] = m_header.qoffset_x;
            ret[1][3] = m_header.qoffset_y;
            ret[2][3] = m_header.qoffset_z;
        } else {
            CaretLogWarning("found quaternion with length greater than 1 in nifti header, using ANALYZE coordinates!");
            ret[0][0] = m_header.pixdim[1];
            ret[1][1] = m_header.pixdim[2];
            ret[2][2] = m_header.pixdim[3];
        }
    } else {//fall back to analyze and complain
        CaretLogWarning("no sform or qform code found, using ANALYZE coordinates!");
        ret[0][0] = m_header.pixdim[1];
        ret[1][1] = m_header.pixdim[2];
        ret[2][2] = m_header.pixdim[3];
    }
    int32_t spaceUnit = XYZT_TO_SPACE(m_header.xyzt_units);
    switch (spaceUnit)
    {
        case NIFTI_UNITS_METER:
            ret *= 1000.0f;
            ret[3][3] = 1.0f;
            break;
        case NIFTI_UNITS_MICRON:
            ret *= 0.001f;
            ret[3][3] = 1.0f;
            break;
        case 0:
            CaretLogFine("found spatial unit of '0' in nifti header, assuming millimeters");
        case NIFTI_UNITS_MM:
            break;
        default:
            CaretLogWarning("unrecognized spatial unit in nifti header");
    }
    return ret.getMatrix();
}

QString NiftiHeader::toString() const
{
    QString ret;
    if (isSwapped())
    {
        ret += "native endian: false\n";
    } else {
        ret += "native endian: true\n";
    }
    ret += "sizeof_hdr: " + QString::number(m_header.sizeof_hdr) + "\n";//skip the fields that aren't important, like intent_p1, cal_max, etc
    ret += "magic: " + QByteArray(m_header.magic, 8);//quirk: QByteArray supports embedded nulls, so adding "\n" here doesn't result in a newline in the string
    ret += "\ndatatype: " + QString::number(m_header.datatype) + "\n";
    ret += "bitpix: " + QString::number(m_header.bitpix) + "\n";
    CaretAssert(m_header.dim[0] < 8);
    for (int i = 0; i <= m_header.dim[0]; ++i)
    {
        ret += "dim[" + QString::number(i) + "]: " + QString::number(m_header.dim[i]) + "\n";
    }
    for (int i = 0; i <= m_header.dim[0]; ++i)
    {
        ret += "pixdim[" + QString::number(i) + "]: " + QString::number(m_header.pixdim[i]) + "\n";
    }
    ret += "vox_offset: " + QString::number(m_header.vox_offset) + "\n";
    ret += "scl_slope: " + QString::number(m_header.scl_slope) + "\n";
    ret += "scl_inter: " + QString::number(m_header.scl_inter) + "\n";
    ret += "sform_code: " + QString::number(m_header.sform_code) + "\n";
    if (m_header.sform_code != NIFTI_XFORM_UNKNOWN)
    {
        ret += "srow_x:";
        for (int i = 0; i < 4; ++i)
        {
            ret += " " + QString::number(m_header.srow_x[i]);
        }
        ret += "\nsrow_y:";
        for (int i = 0; i < 4; ++i)
        {
            ret += " " + QString::number(m_header.srow_y[i]);
        }
        ret += "\nsrow_z:";
        for (int i = 0; i < 4; ++i)
        {
            ret += " " + QString::number(m_header.srow_z[i]);
        }
        ret += "\n";
    }
    ret += "qform_code: " + QString::number(m_header.qform_code) + "\n";
    if (m_header.qform_code != NIFTI_XFORM_UNKNOWN)
    {
        ret += "quatern_b: " + QString::number(m_header.quatern_b) + "\n";
        ret += "quatern_c: " + QString::number(m_header.quatern_c) + "\n";
        ret += "quatern_d: " + QString::number(m_header.quatern_d) + "\n";
        ret += "qoffset_x: " + QString::number(m_header.qoffset_x) + "\n";
        ret += "qoffset_y: " + QString::number(m_header.qoffset_y) + "\n";
        ret += "qoffset_z: " + QString::number(m_header.qoffset_z) + "\n";
    }
    ret += "effective sform:\n";
    vector<vector<float> > tempSform = getSForm();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            ret += " " + QString::number(tempSform[i][j]);
        }
        ret += "\n";
    }
    ret += "xyzt_units: " + QString::number(m_header.xyzt_units) + "\n";
    ret += "intent_code: " + QString::number(m_header.intent_code) + "\n";
    ret += "intent_name: " + QByteArray(m_header.intent_name, 16);//same quirk
    ret += "\n";
    int numExts = (int)m_extensions.size();
    ret += QString::number(numExts) + " extension";
    if (numExts != 1) ret += "s";
    if (numExts == 0)
    {
        ret += "\n";
    } else {
        ret += ":\n";
        for (int i = 0; i < numExts; ++i)
        {
            CaretAssert(m_extensions[i] != NULL);
            ret += "\n";
            ret += "code: " + QString::number(m_extensions[i]->m_ecode) + "\n";
            ret += "length: " + QString::number(m_extensions[i]->m_bytes.size()) + "\n";
        }
    }
    return ret;
}

void NiftiHeader::setDataType(const int16_t& type)
{
    m_header.bitpix = typeToNumBits(m_header.datatype);//to check for errors
    m_header.datatype = type;
}

void NiftiHeader::setDimensions(const vector<int64_t>& dimsIn)
{
    if (dimsIn.size() > 7 || dimsIn.empty()) throw DataFileException("Number of dimensions must be between 1 and 7, inclusive.");
    m_header.dim[0] = dimsIn.size();
    int i = 0;
    for(; i < (int)dimsIn.size(); i++)
    {
        if (dimsIn[i] < 1) throw DataFileException("all dimension lengths must be positive");//maybe these should be asserts?
        m_header.dim[i + 1] = dimsIn[i];
    }
    for (; i < 7; ++i)
    {
        m_header.dim[i + 1] = 1;//we maintain 1s on unused dimensions to make some stupid nifti readers happy
    }
}

void NiftiHeader::setIntent(const int32_t& code, const char name[16])
{
    m_header.intent_code = code;
    int i;//custom strncpy-like code to fill nulls to the end
    for (i = 0; i < 16 && name[i] != '\0'; ++i) m_header.intent_name[i] = name[i];
    for (; i < 16; ++i) m_header.intent_name[i] = '\0';
}

void NiftiHeader::setSForm(const vector<vector<float> >& sForm)
{
    CaretAssert(sForm.size() >= 3);//programmer error to pass badly sized matrix
    if (sForm.size() < 3) throw DataFileException("internal error: setSForm matrix badly sized");//but make release also throw
    for (int i = 0; i < (int)sForm.size(); i++)
    {
        CaretAssert(sForm[i].size() >= 4);//ditto
        if (sForm[i].size() < 4) throw DataFileException("internal error: setSForm matrix badly sized");
    }
    m_header.xyzt_units = SPACE_TIME_TO_XYZT(NIFTI_UNITS_MM, NIFTI_UNITS_SEC);//overwrite whatever units we read in
    for (int i = 0; i < 4; i++)
    {
        m_header.srow_x[i] = sForm[0][i];
        m_header.srow_y[i] = sForm[1][i];
        m_header.srow_z[i] = sForm[2][i];
    }
    m_header.sform_code = NIFTI_XFORM_MNI_152;
    Vector3D ivec, jvec, kvec;
    ivec[0] = sForm[0][0]; ivec[1] = sForm[1][0]; ivec[2] = sForm[2][0];
    jvec[0] = sForm[0][1]; jvec[1] = sForm[1][1]; jvec[2] = sForm[2][1];
    kvec[0] = sForm[0][2]; kvec[1] = sForm[1][2]; kvec[2] = sForm[2][2];
    m_header.pixdim[0] = 1.0f;
    m_header.pixdim[1] = ivec.length();
    m_header.pixdim[2] = jvec.length();
    m_header.pixdim[3] = kvec.length();
    ivec = ivec.normal();
    jvec = jvec.normal();
    kvec = kvec.normal();
    if (kvec.dot(ivec.cross(jvec)) < 0.0f)//left handed sform!
    {
        m_header.pixdim[0] = -1.0f;
        kvec = -kvec;//because to nifti, "left handed" apparently means "up is down", not "left is right"
    }
    float rotmat[3][3];
    rotmat[0][0] = ivec[0]; rotmat[1][0] = jvec[0]; rotmat[2][0] = kvec[0];
    rotmat[0][1] = ivec[1]; rotmat[1][1] = jvec[1]; rotmat[2][1] = kvec[1];
    rotmat[0][2] = ivec[2]; rotmat[1][2] = jvec[2]; rotmat[2][2] = kvec[2];
    float quat[4];
    if (!MathFunctions::matrixToQuatern(rotmat, quat))
    {
        m_header.qform_code = NIFTI_XFORM_UNKNOWN;//0, implies that there is no qform
        m_header.quatern_b = 0.0;//set dummy values anyway
        m_header.quatern_c = 0.0;
        m_header.quatern_d = 0.0;
        m_header.qoffset_x = sForm[0][3];
        m_header.qoffset_y = sForm[1][3];
        m_header.qoffset_z = sForm[2][3];
    } else {
        m_header.qform_code = NIFTI_XFORM_MNI_152;
        m_header.quatern_b = quat[1];
        m_header.quatern_c = quat[2];
        m_header.quatern_d = quat[3];
        m_header.qoffset_x = sForm[0][3];
        m_header.qoffset_y = sForm[1][3];
        m_header.qoffset_z = sForm[2][3];
    }
}

void NiftiHeader::clearDataScaling()
{
    m_header.scl_slope = 1.0;
    m_header.scl_inter = 0.0;
}

void NiftiHeader::setDataScaling(const double& mult, const double& offset)
{
    m_header.scl_slope = mult;
    m_header.scl_inter = offset;
}

namespace
{
    template<typename T>
    struct Scaling
    {
        double mult, offset;
        Scaling(const double& minval, const double& maxval)
        {
            typedef std::numeric_limits<T> mylimits;
            double mymin = mylimits::lowest();
            mult = (maxval - minval) / ((double)mylimits::max() - mymin);//multiplying is the first step of decoding (after byteswap), so start with the range
            offset = minval - mymin * mult;//offset is added after multiplying the encoded value by mult
        }
    };
}

void NiftiHeader::setDataTypeAndScaleRange(const int16_t& type, const double& minval, const double& maxval)
{
    setDataType(type);
    switch (type)
    {
        case NIFTI_TYPE_RGB24:
            clearDataScaling();//RGB ignores scaling fields
            break;
        case DT_BINARY://currently not supported in read/write functions anyway
            setDataScaling(maxval - minval, minval);//make the two possible decoded values equal to the min and max
            break;
        case NIFTI_TYPE_INT8:
        {
            Scaling<int8_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_UINT8:
        {
            Scaling<uint8_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_INT16:
        {
            Scaling<int16_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_UINT16:
        {
            Scaling<uint16_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_INT32:
        {
            Scaling<int32_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_UINT32:
        {
            Scaling<uint32_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_INT64:
        {
            Scaling<int64_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_UINT64:
        {
            Scaling<uint64_t> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_FLOAT32:
        case NIFTI_TYPE_COMPLEX64:
        {
            Scaling<float> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_FLOAT64:
        case NIFTI_TYPE_COMPLEX128:
        {
            Scaling<double> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        case NIFTI_TYPE_FLOAT128:
        case NIFTI_TYPE_COMPLEX256:
        {
            Scaling<long double> myscale(minval, maxval);
            setDataScaling(myscale.mult, myscale.offset);
            break;
        }
        default:
            CaretAssert(0);
            throw CaretException("internal error, report what you did to the developers");
    }
}

int NiftiHeader::getNumComponents() const
{
    switch (getDataType())
    {
        case NIFTI_TYPE_RGB24:
            return 3;
            break;
        case NIFTI_TYPE_COMPLEX64:
        case NIFTI_TYPE_COMPLEX128:
        case NIFTI_TYPE_COMPLEX256:
            return 2;
            break;
        case DT_BINARY:
        case NIFTI_TYPE_INT8:
        case NIFTI_TYPE_UINT8:
        case NIFTI_TYPE_INT16:
        case NIFTI_TYPE_UINT16:
        case NIFTI_TYPE_INT32:
        case NIFTI_TYPE_UINT32:
        case NIFTI_TYPE_FLOAT32:
        case NIFTI_TYPE_INT64:
        case NIFTI_TYPE_UINT64:
        case NIFTI_TYPE_FLOAT64:
        case NIFTI_TYPE_FLOAT128:
            return 1;
            break;
        default:
            CaretAssert(0);
            throw CaretException("internal error, report what you did to the developers");
    }
}

bool NiftiHeader::hasGoodSpatialInformation() const
{
    return (m_header.sform_code != 0 || m_header.qform_code != 0);
}

void NiftiHeader::read(CaretBinaryFile& inFile)
{
    nifti_1_header buffer1;
    nifti_2_header buffer2;
    inFile.read(&buffer1, sizeof(nifti_1_header));
    int version = NIFTI2_VERSION(buffer1);
    bool swapped = false;
    Quirks myquirks;
    try
    {
        if (version == 2)
        {
            memcpy(&buffer2, &buffer1, sizeof(nifti_1_header));
            inFile.read(((char*)&buffer2) + sizeof(nifti_1_header), sizeof(nifti_2_header) - sizeof(nifti_1_header));
            if (NIFTI2_NEEDS_SWAP(buffer2))
            {
                swapped = true;
                swapHeaderBytes(buffer2);
            }
            myquirks = setupFrom(buffer2, inFile.getFilename());
        } else if (version == 1) {
            if (NIFTI2_NEEDS_SWAP(buffer1))//yes, this works on nifti-1 also
            {
                swapped = true;
                swapHeaderBytes(buffer1);
            }
            myquirks = setupFrom(buffer1, inFile.getFilename());
        } else {
            throw DataFileException(inFile.getFilename() + " is not a valid NIfTI file");
        }
    } catch (DataFileException& e) {//catch and throw in order to add filename info
        throw DataFileException("error reading NIfTI file " + inFile.getFilename() + ": " + e.whatString());
    }
    m_extensions.clear();
    if (myquirks.no_extender)
    {
        int min_offset = 352;
        if (version == 2) min_offset = 544;
        CaretLogWarning("in file '" + inFile.getFilename() + "', vox_offset is " + AString::number(m_header.vox_offset) +
        ", nifti standard specifies that it should be at least " + AString::number(min_offset) + ", assuming malformed file with no extender");
    } else {
        char extender[4];
        inFile.read(extender, 4);
        int extensions = 0;//if it has extensions in a format we don't know about, don't try to read them
        if (version == 1 && extender[0] != 0) extensions = 1;//sadly, this is the only thing nifti-1 says about the extender bytes
        if (version == 2 && extender[0] == 1 && extender[1] == 0 && extender[2] == 0 && extender[3] == 0) extensions = 1;//from http://nifti.nimh.nih.gov/nifti-2 as of 4/4/2014:
        if (extensions == 1)//"extentions match those of NIfTI-1.1 when the extender bytes are 1 0 0 0"
        {
            int64_t extStart;
            if (version == 1)
            {
                extStart = 352;
            } else {
                CaretAssert(version == 2);
                extStart = 544;
            }
            CaretAssert(inFile.pos() == extStart);
            while(extStart + 2 * sizeof(int32_t) <= (size_t)m_header.vox_offset)
            {
                int32_t esize, ecode;
                inFile.read(&esize, sizeof(int32_t));
                if (swapped) ByteSwapping::swap(esize);
                inFile.read(&ecode, sizeof(int32_t));
                if (swapped) ByteSwapping::swap(ecode);
                if (esize < 8 || esize + extStart > m_header.vox_offset) break;
                CaretPointer<NiftiExtension> tempExtension(new NiftiExtension());
                if ((size_t)esize > 2 * sizeof(int32_t))//don't try to read 0 bytes
                {
                    tempExtension->m_bytes.resize(esize - 2 * sizeof(int32_t));
                    inFile.read(tempExtension->m_bytes.data(), esize - 2 * sizeof(int32_t));
                }
                tempExtension->m_ecode = ecode;
                m_extensions.push_back(tempExtension);
                extStart += esize;//esize includes the two int32_ts
            }
        }
    }
    m_isSwapped = swapped;//now that we know there were no errors (because they throw), complete the internal state
    m_version = version;
}

NiftiHeader::Quirks NiftiHeader::setupFrom(const nifti_1_header& header, const AString& filename)
{
    Quirks ret;
    if (header.sizeof_hdr != sizeof(nifti_1_header)) throw DataFileException(filename, "incorrect sizeof_hdr");
    const char magic[] = "n+1\0";//only support single-file nifti
    if (strncmp(header.magic, magic, 4) != 0) throw DataFileException(filename, "incorrect magic");
    if (header.dim[0] < 1 || header.dim[0] > 7) throw DataFileException(filename, "incorrect dim[0]");
    for (int i = 0; i < header.dim[0]; ++i)
    {
        if (header.dim[i + 1] < 1) throw DataFileException(filename, "dim[" + QString::number(i + 1) + "] < 1");
    }
    if (header.vox_offset < 352)
    {
        if (header.vox_offset < 348)
        {
            throw DataFileException(filename, "invalid vox_offset: " + AString::number(header.vox_offset));
        }
        ret.no_extender = true;
    }
    int numBits = typeToNumBits(header.datatype);
    if (header.bitpix != numBits) CaretLogWarning("datatype disagrees with bitpix in file '" + filename + "'");
    m_header.sizeof_hdr = header.sizeof_hdr;//copy in everything, so we don't have to fake anything to print the header as read
    for (int i = 0; i < 4; ++i)//mostly using nifti-2 field order to make it easier to find if things are missed
    {
        m_header.magic[i] = header.magic[i];
        m_header.srow_x[i] = header.srow_x[i];//slight hack - nifti-1 magic and srows both happen to be 4 long
        m_header.srow_y[i] = header.srow_y[i];
        m_header.srow_z[i] = header.srow_z[i];
    }
    m_header.datatype = header.datatype;
    m_header.bitpix = header.bitpix;
    for (int i = 0; i < 8; ++i)
    {
        m_header.dim[i] = header.dim[i];
        m_header.pixdim[i] = header.pixdim[i];
    }
    m_header.intent_p1 = header.intent_p1;
    m_header.intent_p2 = header.intent_p2;
    m_header.intent_p3 = header.intent_p3;
    m_header.vox_offset = header.vox_offset;//technically, this could result in integer overflow, if the header extensions total exabytes in size
    m_header.scl_slope = header.scl_slope;
    m_header.scl_inter = header.scl_inter;
    m_header.cal_max = header.cal_max;
    m_header.cal_min = header.cal_min;
    m_header.slice_duration = header.slice_duration;
    m_header.toffset = header.toffset;
    m_header.slice_start = header.slice_start;
    m_header.slice_end = header.slice_end;
    for (int i = 0; i < 80; ++i) m_header.descrip[i] = header.descrip[i];
    for (int i = 0; i < 24; ++i) m_header.aux_file[i] = header.aux_file[i];
    m_header.qform_code = header.qform_code;
    m_header.sform_code = header.sform_code;
    m_header.quatern_b = header.quatern_b;
    m_header.quatern_c = header.quatern_c;
    m_header.quatern_d = header.quatern_d;
    m_header.qoffset_x = header.qoffset_x;
    m_header.qoffset_y = header.qoffset_y;
    m_header.qoffset_z = header.qoffset_z;
    m_header.slice_code = header.slice_code;
    m_header.xyzt_units = header.xyzt_units;
    m_header.intent_code = header.intent_code;
    for (int i = 0; i < 16; ++i) m_header.intent_name[i] = header.intent_name[i];
    m_header.dim_info = header.dim_info;
    return ret;
}

NiftiHeader::Quirks NiftiHeader::setupFrom(const nifti_2_header& header, const AString& filename)
{
    Quirks ret;
    if (header.sizeof_hdr != sizeof(nifti_2_header)) throw DataFileException(filename, "incorrect sizeof_hdr");
    const char magic[] = "n+2\0\r\n\032\n";//only support single-file nifti
    for (int i = 0; i < 8; ++i)
    {
        if (header.magic[i] != magic[i]) throw DataFileException(filename, "incorrect magic");
    }
    if (header.dim[0] < 1 || header.dim[0] > 7) throw DataFileException(filename, "incorrect dim[0]");
    for (int i = 0; i < header.dim[0]; ++i)
    {
        if (header.dim[i + 1] < 1) throw DataFileException(filename, "dim[" + QString::number(i + 1) + "] < 1");
    }
    if (header.vox_offset < 544) throw DataFileException(filename, "incorrect vox_offset");//haven't noticed any nifti-2 with bad vox_offset yet, and all cifti files have a big extension, so they have to have used it correctly
    if (header.bitpix != typeToNumBits(header.datatype)) CaretLogWarning("datatype disagrees with bitpix in file '" + filename + "'");
    memcpy(&m_header, &header, sizeof(nifti_2_header));
    return ret;
}

int NiftiHeader::typeToNumBits(const int64_t& type)
{
    switch (type)
    {
        case DT_BINARY:
            return 1;
            break;
        case NIFTI_TYPE_INT8:
        case NIFTI_TYPE_UINT8:
            return 8;
            break;
        case NIFTI_TYPE_INT16:
        case NIFTI_TYPE_UINT16:
            return 16;
            break;
        case NIFTI_TYPE_RGB24:
            return 24;
            break;
        case NIFTI_TYPE_INT32:
        case NIFTI_TYPE_UINT32:
        case NIFTI_TYPE_FLOAT32:
            return 32;
            break;
        case NIFTI_TYPE_INT64:
        case NIFTI_TYPE_UINT64:
        case NIFTI_TYPE_FLOAT64:
        case NIFTI_TYPE_COMPLEX64:
            return 64;
            break;
        case NIFTI_TYPE_FLOAT128:
        case NIFTI_TYPE_COMPLEX128:
            return 128;
            break;
        case NIFTI_TYPE_COMPLEX256:
            return 256;
            break;
        default:
            throw DataFileException("incorrect nifti datatype code");
    }
}

void NiftiHeader::swapHeaderBytes(nifti_1_header& header)
{
    ByteSwapping::swap(header.sizeof_hdr);//by order of fields in nifti-1 header, skip unused because we don't store their data
    ByteSwapping::swapArray(header.dim, 8);
    ByteSwapping::swap(header.intent_p1);
    ByteSwapping::swap(header.intent_p2);
    ByteSwapping::swap(header.intent_p3);
    ByteSwapping::swap(header.intent_code);
    ByteSwapping::swap(header.datatype);
    ByteSwapping::swap(header.bitpix);
    ByteSwapping::swap(header.slice_start);
    ByteSwapping::swapArray(header.pixdim, 8);
    ByteSwapping::swap(header.vox_offset);
    ByteSwapping::swap(header.scl_slope);
    ByteSwapping::swap(header.scl_inter);
    ByteSwapping::swap(header.slice_end);
    ByteSwapping::swap(header.cal_max);
    ByteSwapping::swap(header.cal_min);
    ByteSwapping::swap(header.slice_duration);
    ByteSwapping::swap(header.toffset);
    ByteSwapping::swap(header.qform_code);
    ByteSwapping::swap(header.sform_code);
    ByteSwapping::swap(header.quatern_b);
    ByteSwapping::swap(header.quatern_c);
    ByteSwapping::swap(header.quatern_d);
    ByteSwapping::swap(header.qoffset_x);
    ByteSwapping::swap(header.qoffset_y);
    ByteSwapping::swap(header.qoffset_z);
    ByteSwapping::swapArray(header.srow_x, 4);
    ByteSwapping::swapArray(header.srow_y, 4);
    ByteSwapping::swapArray(header.srow_z, 4);
}

void NiftiHeader::swapHeaderBytes(nifti_2_header& header)
{
    ByteSwapping::swap(header.sizeof_hdr);//by order of fields in nifti-2 header
    ByteSwapping::swap(header.datatype);
    ByteSwapping::swap(header.bitpix);
    ByteSwapping::swapArray(header.dim, 8);
    ByteSwapping::swap(header.intent_p1);
    ByteSwapping::swap(header.intent_p2);
    ByteSwapping::swap(header.intent_p3);
    ByteSwapping::swapArray(header.pixdim, 8);
    ByteSwapping::swap(header.vox_offset);
    ByteSwapping::swap(header.scl_slope);
    ByteSwapping::swap(header.scl_inter);
    ByteSwapping::swap(header.cal_max);
    ByteSwapping::swap(header.cal_min);
    ByteSwapping::swap(header.slice_duration);
    ByteSwapping::swap(header.toffset);
    ByteSwapping::swap(header.slice_start);
    ByteSwapping::swap(header.slice_end);
    ByteSwapping::swap(header.qform_code);
    ByteSwapping::swap(header.sform_code);
    ByteSwapping::swap(header.quatern_b);
    ByteSwapping::swap(header.quatern_c);
    ByteSwapping::swap(header.quatern_d);
    ByteSwapping::swap(header.qoffset_x);
    ByteSwapping::swap(header.qoffset_y);
    ByteSwapping::swap(header.qoffset_z);
    ByteSwapping::swapArray(header.srow_x, 4);
    ByteSwapping::swapArray(header.srow_y, 4);
    ByteSwapping::swapArray(header.srow_z, 4);
    ByteSwapping::swap(header.slice_code);
    ByteSwapping::swap(header.xyzt_units);
    ByteSwapping::swap(header.intent_code);
}

void NiftiHeader::write(CaretBinaryFile& outFile, const int& version, const bool& swapEndian)
{
    if (!canWriteVersion(version)) throw DataFileException("unable to write NIfTI version " + QString::number(version) + " for file " + outFile.getFilename());
    double junk1, junk2;
    int16_t datatype = getDataType();
    if (getDataScaling(junk1, junk2) && ((datatype & 0x70) > 0 || datatype >= 1536))
    {//that hacky expression is to detect 16, 32, 64, 1536, 1792, and 2048
        CaretLogWarning("writing nifti file with scaling factor and floating point datatype");
    }
    const char padding[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int64_t voxOffset;
    if (version == 2)
    {
        nifti_2_header outHeader;
        prepareHeader(outHeader);
        voxOffset = outHeader.vox_offset;
        if (swapEndian) swapHeaderBytes(outHeader);
        outFile.write(&outHeader, sizeof(nifti_2_header));
    } else if (version == 1) {
        nifti_1_header outHeader;
        prepareHeader(outHeader);
        voxOffset = outHeader.vox_offset;
        if (swapEndian) swapHeaderBytes(outHeader);
        outFile.write(&outHeader, sizeof(nifti_1_header));
    } else {
        CaretAssert(0);//canWriteVersion should have said no
        throw DataFileException("internal error: NiftiHeader::canWriteVersion() returned true for unimplemented writing version");
    }
    char extender[] = { 0, 0, 0, 0 };//at least until nifti-2 gets a new extension format, use the same code for both
    int numExtensions = (int)m_extensions.size();
    if (numExtensions != 0) extender[0] = 1;
    outFile.write(extender, 4);
    for (int i = 0; i < numExtensions; ++i)
    {
        CaretAssert(m_extensions[i] != NULL);
        int64_t thisSize = 8 + m_extensions[i]->m_bytes.size();//8 is for the int32_t size and ecode for nifti-1 style extensions
        int paddingBytes = 0;
        if (thisSize % 16 != 0)//round up to nearest multiple of 16
        {
            paddingBytes = 16 - (thisSize % 16);
            thisSize += paddingBytes;
        }
        CaretAssert(thisSize <= numeric_limits<int32_t>::max());
        CaretAssert(thisSize + outFile.pos() <= voxOffset);
        int32_t outSize = thisSize;
        int32_t outEcode = m_extensions[i]->m_ecode;
        if (swapEndian)
        {
            ByteSwapping::swap(outSize);
            ByteSwapping::swap(outEcode);
        }
        outFile.write(&outSize, sizeof(int32_t));
        outFile.write(&outEcode, sizeof(int32_t));
        outFile.write(m_extensions[i]->m_bytes.data(), m_extensions[i]->m_bytes.size());
        if (paddingBytes != 0) outFile.write(padding, paddingBytes);
    }
    CaretAssert(outFile.pos() == voxOffset);
    m_header.vox_offset = voxOffset;//update internal state to reflect the state that was written to the file
    m_version = version;
    m_isSwapped = swapEndian;
}

void NiftiHeader::prepareHeader(nifti_1_header& header) const
{
    CaretAssert(canWriteVersion(1));//programmer error to call this if it isn't possible
    header.sizeof_hdr = sizeof(nifti_1_header);//do static things first
    const char magic[] = "n+1\0";//only support single-file nifti
    for (int i = 0; i < 4; ++i) header.magic[i] = magic[i];
    for (int i = 0; i < 10; ++i) header.data_type[i] = 0;//then zero unused things
    for (int i = 0; i < 18; ++i) header.db_name[i] = 0;
    header.extents = 0;
    header.session_error = 0;
    header.regular = 0;
    header.glmax = 0;
    header.glmin = 0;
    header.dim_info = m_header.dim_info;//by order of fields in nifti-1 header, skipping unused and static
    for (int i = 0; i < 8; ++i) header.dim[i] = m_header.dim[i];//canWriteVersion should have already checked that this is okay, first in write(), then asserted above
    header.intent_p1 = m_header.intent_p1;//theoretically, this could be a problem wih large exponents, or if extremely high precision is required
    header.intent_p2 = m_header.intent_p2;//but we don't use them at all currently, so we don't care
    header.intent_p3 = m_header.intent_p3;
    header.intent_code = m_header.intent_code;
    header.datatype = m_header.datatype;
    header.bitpix = typeToNumBits(m_header.datatype);//in case we ever accept wrong bitpix with a warning, NEVER write wrong bitpix
    header.slice_start = m_header.slice_start;
    for (int i = 0; i < 8; ++i) header.pixdim[i] = m_header.pixdim[i];//more double to float conversion
    header.vox_offset = computeVoxOffset(1);//again, canWriteVersion should have checked that this, and later conversions, are okay
    CaretAssert(header.vox_offset >= 352);
    header.scl_slope = m_header.scl_slope;
    header.scl_inter = m_header.scl_inter;
    header.slice_end = m_header.slice_end;
    header.slice_code = m_header.slice_code;
    header.xyzt_units = m_header.xyzt_units;
    header.cal_min = m_header.cal_min;
    header.cal_max = m_header.cal_max;
    header.slice_duration = m_header.slice_duration;
    header.toffset = m_header.toffset;
    for (int i = 0; i < 80; ++i) header.descrip[i] = m_header.descrip[i];
    for (int i = 0; i < 24; ++i) header.aux_file[i] = m_header.aux_file[i];
    header.qform_code = m_header.qform_code;
    header.sform_code = m_header.sform_code;
    header.quatern_b = m_header.quatern_b;
    header.quatern_c = m_header.quatern_c;
    header.quatern_d = m_header.quatern_d;
    header.qoffset_x = m_header.qoffset_x;
    header.qoffset_y = m_header.qoffset_y;
    header.qoffset_z = m_header.qoffset_z;
    for (int i = 0; i < 4; ++i)
    {
        header.srow_x[i] = m_header.srow_x[i];
        header.srow_y[i] = m_header.srow_y[i];
        header.srow_z[i] = m_header.srow_z[i];
    }
    for (int i = 0; i < 16; ++i) header.intent_name[i] = m_header.intent_name[i];
}

void NiftiHeader::prepareHeader(nifti_2_header& header) const
{
    CaretAssert(canWriteVersion(2));
    memcpy(&header, &m_header, sizeof(nifti_2_header));//first copy everything, then fix static and computed fields
    header.sizeof_hdr = sizeof(nifti_2_header);
    const char magic[] = "n+2\0\r\n\032\n";
    for (int i = 0; i < 8; ++i) header.magic[i] = magic[i];
    header.bitpix = typeToNumBits(header.datatype);
    header.vox_offset = computeVoxOffset(2);
    for (int i = 0; i < 15; ++i) header.unused_str[i] = 0;//in case we read in a header where these bytes weren't zero
}
