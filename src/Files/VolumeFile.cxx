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

#include "VolumeFile.h"
#include "FloatMatrix.h"
#include <algorithm>
#include <cmath>
#include "NiftiFile.h"
#include "DescriptiveStatistics.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "GiftiXmlElements.h"
#include "Palette.h"
#include "PaletteColorMapping.h"

using namespace caret;
using namespace std;

const float VolumeFile::INVALID_INTERP_VALUE = 0.0f;//we may want NaN or something more obvious

VolumeFile::VolumeFile()
: VolumeBase(), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
}

VolumeFile::VolumeFile(const vector<uint64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const uint64_t numComponents)
: VolumeBase(dimensionsIn, indexToSpace, numComponents), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
}

VolumeFile::VolumeFile(const vector<int64_t>& dimensionsIn, const vector<vector<float> >& indexToSpace, const int64_t numComponents)
: VolumeBase(dimensionsIn, indexToSpace, numComponents), CaretMappableDataFile(DataFileTypeEnum::VOLUME)
{
}

VolumeFile::~VolumeFile()
{
}

void VolumeFile::readFile(const AString& filename) throw (DataFileException)
{
    try {
        NiftiFile myNifti;
        myNifti.readVolumeFile(*this, filename);
        this->setFileName(filename);
    }
    catch (NiftiException e) {
        throw DataFileException(e);
    }
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void 
VolumeFile::writeFile(const AString& filename) throw (DataFileException)
{
    try {
        NiftiFile myNifti;
        myNifti.writeVolumeFile(*this, filename);
    }
    catch (NiftiException e) {
        throw DataFileException(e);
    }
}

float VolumeFile::interpolateValue(const float* coordIn, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component)
{
    return interpolateValue(coordIn[0], coordIn[1], coordIn[2], interp, validOut, brickIndex, component);
}

float VolumeFile::interpolateValue(const float coordIn1, const float coordIn2, const float coordIn3, InterpType interp, bool* validOut, const int64_t brickIndex, const int64_t component)
{
    switch (interp)
    {
        case TRILINEAR:
        {
            float index1, index2, index3;
            spaceToIndex(coordIn1, coordIn2, coordIn3, index1, index2, index3);
            int ind1low = floor(index1);
            int ind2low = floor(index2);
            int ind3low = floor(index3);
            int ind1high = ind1low + 1;
            int ind2high = ind2low + 1;
            int ind3high = ind3low + 1;
            if (!indexValid(ind1low, ind2low, ind3low, brickIndex, component) || !indexValid(ind1high, ind2high, ind3high, brickIndex, component))
            {
                if (validOut != NULL) *validOut = false;
                return INVALID_INTERP_VALUE;
            }
            float xhighWeight = index1 - ind1low;
            float xlowWeight = 1.0f - xhighWeight;
            float xinterp[2][2];//manually unrolled, because loops of 2 seem silly
            xinterp[0][0] = xlowWeight * getValue(ind1low, ind2low, ind3low, brickIndex, component) + xhighWeight * getValue(ind1high, ind2low, ind3low, brickIndex, component);
            xinterp[1][0] = xlowWeight * getValue(ind1low, ind2high, ind3low, brickIndex, component) + xhighWeight * getValue(ind1high, ind2high, ind3low, brickIndex, component);
            xinterp[0][1] = xlowWeight * getValue(ind1low, ind2low, ind3high, brickIndex, component) + xhighWeight * getValue(ind1high, ind2low, ind3high, brickIndex, component);
            xinterp[1][1] = xlowWeight * getValue(ind1low, ind2high, ind3high, brickIndex, component) + xhighWeight * getValue(ind1high, ind2high, ind3high, brickIndex, component);
            float yhighWeight = index2 - ind2low;
            float ylowWeight = 1.0f - yhighWeight;
            float yinterp[2];
            yinterp[0] = ylowWeight * xinterp[0][0] + yhighWeight * xinterp[1][0];
            yinterp[1] = ylowWeight * xinterp[0][1] + yhighWeight * xinterp[1][1];
            float zlowWeight = index3 - ind3low;
            float zhighWeight = 1.0f - zlowWeight;
            float ret = zlowWeight * yinterp[0] + zhighWeight * yinterp[1];
            if (validOut != NULL) *validOut = true;
            return ret;
        }
        break;
        case NEAREST_NEIGHBOR:
        {
            int64_t index1, index2, index3;
            closestVoxel(coordIn1, coordIn2, coordIn3, index1, index2, index3);
            if (indexValid(index1, index2, index3, brickIndex, component))
            {
                if (validOut != NULL) *validOut = true;
                return getValue(index1, index2, index3, brickIndex, component);
            } else {
                if (validOut != NULL) *validOut = false;
                return INVALID_INTERP_VALUE;
            }
        }
        break;
    }
    if (validOut != NULL) *validOut = false;
    return INVALID_INTERP_VALUE;
}
