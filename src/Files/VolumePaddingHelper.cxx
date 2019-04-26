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

#include "VolumePaddingHelper.h"

#include "CaretAssert.h"
#include "CaretException.h"
#include "FloatMatrix.h"
#include "GiftiLabelTable.h"
#include "PaletteColorMapping.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <cmath>

using namespace caret;
using namespace std;

VolumePaddingHelper VolumePaddingHelper::padVoxels(const VolumeFile* orig, const int& ipad, const int& jpad, const int& kpad)
{
    VolumePaddingHelper ret;
    orig->getDimensions(ret.m_origDims);
    ret.m_ipad = ipad;
    ret.m_jpad = jpad;
    ret.m_kpad = kpad;
    ret.m_origDims.resize(3);//we only care about spatial dimensions
    ret.m_paddedDims = ret.m_origDims;
    ret.m_paddedDims[0] += ipad * 2;
    ret.m_paddedDims[1] += jpad * 2;
    ret.m_paddedDims[2] += kpad * 2;
    ret.m_origSform = orig->getSform();
    FloatMatrix origSpace(ret.m_origSform);
    FloatMatrix padSpace = origSpace;
    for (int i = 0; i < origSpace.getNumberOfRows(); ++i)
    {
        padSpace[i][3] -= ipad * origSpace[i][0] + jpad * origSpace[i][1] + kpad * origSpace[i][2];
    }
    ret.m_paddedSform = padSpace.getMatrix();
    return ret;
}

VolumePaddingHelper VolumePaddingHelper::padMM(const VolumeFile* orig, const float& mmpad)
{
    vector<vector<float> > volSpace = orig->getSform();
    Vector3D ivec, jvec, kvec, origin, ijorth, jkorth, kiorth;
    FloatMatrix(volSpace).getAffineVectors(ivec, jvec, kvec, origin);
    ijorth = ivec.cross(jvec).normal();//conceptually put a sphere on each corner of the volume, find how many voxels are needed to fully enclose them
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    int ipad = (int)floor(abs(mmpad / ivec.dot(jkorth))) + 1;
    int jpad = (int)floor(abs(mmpad / jvec.dot(kiorth))) + 1;
    int kpad = (int)floor(abs(mmpad / kvec.dot(ijorth))) + 1;
    return padVoxels(orig, ipad, jpad, kpad);
}

void VolumePaddingHelper::doPadding(const VolumeFile* orig, VolumeFile* padded, const float& padval) const
{
    CaretAssert(padded != orig);
    bool labelMode = (orig->getType() == SubvolumeAttributes::LABEL);
    if (!orig->matchesVolumeSpace(m_origDims.data(), m_origSform)) throw CaretException("attempted to pad a volume that doesn't match the one initialized with");
    vector<int64_t> newdims = m_paddedDims, curdims = orig->getOriginalDimensions();
    while (newdims.size() < curdims.size()) newdims.push_back(curdims[newdims.size()]);//add the nonspatial dimensions from orig
    padded->reinitialize(newdims, m_paddedSform, orig->getNumberOfComponents(), orig->getType());
    vector<int64_t> loopdims;
    orig->getDimensions(loopdims);
    for (int c = 0; c < loopdims[4]; ++c)
    {
        for (int s = 0; s < loopdims[3]; ++s)
        {
            if (c == 0)
            {
                if (labelMode)
                {
                    *(padded->getMapLabelTable(s)) = *(orig->getMapLabelTable(s));
                } else {
                    *(padded->getMapPaletteColorMapping(s)) = *(orig->getMapPaletteColorMapping(s));
                }
                padded->setMapName(s, orig->getMapName(s));
            }
            float mypadval = padval;
            if (labelMode) mypadval = orig->getMapLabelTable(s)->getUnassignedLabelKey();
            vector<float> padframe(m_paddedDims[0] * m_paddedDims[1] * m_paddedDims[2], mypadval);
            int64_t ijk[3], inIndex = 0;//we scan the frame linearly, so we can do this
            const float* inFrame = orig->getFrame(s, c);
            for (ijk[2] = 0; ijk[2] < m_origDims[2]; ++ijk[2])
            {
                for (ijk[1] = 0; ijk[1] < m_origDims[1]; ++ijk[1])
                {
                    for (ijk[0] = 0; ijk[0] < m_origDims[0]; ++ijk[0])
                    {
                        int64_t outIndex = padded->getIndex(ijk[0] + m_ipad, ijk[1] + m_jpad, ijk[2] + m_kpad);
                        padframe[outIndex] = inFrame[inIndex];//I could use pointer math instead, but that is needlessly obtuse
                        ++inIndex;
                    }
                }
            }
            padded->setFrame(padframe.data(), s, c);
        }
    }
}

void VolumePaddingHelper::undoPadding(const VolumeFile* padded, VolumeFile* orig) const
{
    CaretAssert(orig != padded);
    if (!padded->matchesVolumeSpace(m_paddedDims.data(), m_paddedSform)) throw CaretException("attempted to unpad a volume that doesn't match padding");
    vector<int64_t> newdims = m_origDims, curdims = padded->getOriginalDimensions();
    while (newdims.size() < curdims.size()) newdims.push_back(curdims[newdims.size()]);//add the nonspatial dimensions from padded
    orig->reinitialize(newdims, m_origSform, padded->getNumberOfComponents(), padded->getType());
    vector<float> unpadframe(m_origDims[0] * m_origDims[1] * m_origDims[2]);
    vector<int64_t> loopdims;
    padded->getDimensions(loopdims);
    for (int c = 0; c < loopdims[4]; ++c)
    {
        for (int s = 0; s < loopdims[3]; ++s)
        {
            if (c == 0)
            {
                if (padded->getType() == SubvolumeAttributes::LABEL)
                {
                    *(orig->getMapLabelTable(s)) = *(padded->getMapLabelTable(s));
                } else {
                    *(orig->getMapPaletteColorMapping(s)) = *(padded->getMapPaletteColorMapping(s));
                }
                orig->setMapName(s, padded->getMapName(s));
            }
            int64_t ijk[3], outIndex = 0;//we scan the frame linearly, so we can do this
            for (ijk[2] = 0; ijk[2] < m_origDims[2]; ++ijk[2])
            {
                for (ijk[1] = 0; ijk[1] < m_origDims[1]; ++ijk[1])
                {
                    for (ijk[0] = 0; ijk[0] < m_origDims[0]; ++ijk[0])
                    {
                        unpadframe[outIndex] = padded->getValue(ijk[0] + m_ipad, ijk[1] + m_jpad, ijk[2] + m_kpad, s, c);
                        ++outIndex;
                    }
                }
            }
            orig->setFrame(unpadframe.data(), s, c);
        }
    }
}
