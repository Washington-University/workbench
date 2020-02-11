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

#include "WarpfieldFile.h"
#include "CaretAssert.h"
#include "CaretOMP.h"
#include "FileInformation.h"
#include "FloatMatrix.h"
#include "NiftiIO.h"

using namespace caret;
using namespace std;

CaretPointer<VolumeFile> WarpfieldFile::generateAbsolute() const
{
    if (m_warpfield == NULL)
    {
        throw DataFileException("generateAbsolute() called on uninitialized WarpfieldFile");
    }
    CaretPointer<VolumeFile> ret(new VolumeFile());
    vector<int64_t> dims = m_warpfield->getDimensions();
    CaretAssert(dims[3] == 3);//make sure it looks like a warpfield
    dims.resize(4);//drop number of components
    ret->reinitialize(dims, m_warpfield->getSform());
    ret->setMapName(0, "x location");
    ret->setMapName(1, "y location");
    ret->setMapName(2, "z location");
    for (int b = 0; b < 3; ++b)
    {
        for (int64_t k = 0; k < dims[2]; ++k)
        {
            for (int64_t j = 0; j < dims[1]; ++j)
            {
                for (int64_t i = 0; i < dims[0]; ++i)
                {
                    Vector3D voxelCenter;
                    m_warpfield->indexToSpace(i, j, k, voxelCenter);
                    ret->setValue(m_warpfield->getValue(i, j, k, b) + voxelCenter[b], i, j, k, b);
                }
            }
        }
    }
    return ret;
}

namespace
{
    void genericWarpfieldRead(VolumeFile& volOut, const AString& filename)
    {
        volOut.readFile(filename);
        vector<int64_t> dims;
        volOut.getDimensions(dims);
        if (dims[3] != 3)
        {
            throw DataFileException("volume file '" + filename + "' has the wrong number of subvolumes for a warpfield");
        }
        if (dims[4] != 1)
        {
            throw DataFileException("volume file '" + filename + "' has multiple components, which is not allowed in a warpfield");
        }
        volOut.setMapName(0, "x displacement");
        volOut.setMapName(1, "y displacement");
        volOut.setMapName(2, "z displacement");
    }
}

void WarpfieldFile::readWorld(const AString& warpname)
{
    CaretPointer<VolumeFile> newFile(new VolumeFile());
    genericWarpfieldRead(*newFile, warpname);
    m_warpfield = newFile;//drop the previous warpfield, and replace with the new one
}

void WarpfieldFile::readITK(const AString& warpname)
{
    CaretPointer<VolumeFile> newFile(new VolumeFile());
    genericWarpfieldRead(*newFile, warpname);
    vector<int64_t> dims;
    newFile->getDimensions(dims);
    for (int64_t k = 0; k < dims[2]; ++k)
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                newFile->setValue(-newFile->getValue(i, j, k, 0), i, j, k, 0);//negate x and y to convert LPS displacements to RAS
                newFile->setValue(-newFile->getValue(i, j, k, 1), i, j, k, 1);
            }
        }
    }
    m_warpfield = newFile;
}

void WarpfieldFile::readFnirt(const AString& warpName, const AString& sourceName, const bool& absolute)
{
    FloatMatrix sourceSform, sourceFSL, refSform, refFSL;
    NiftiIO myIO;
    myIO.openRead(warpName);
    refSform = FloatMatrix(myIO.getHeader().getSForm());
    refFSL = FloatMatrix(myIO.getHeader().getFSLSpace());
    myIO.openRead(sourceName);
    sourceSform = FloatMatrix(myIO.getHeader().getSForm());
    sourceFSL = FloatMatrix(myIO.getHeader().getFSLSpace());
    CaretPointer<VolumeFile> newFile(new VolumeFile());
    genericWarpfieldRead(*newFile, warpName);
    vector<int64_t> dims;
    newFile->getDimensions(dims);
    FloatMatrix sourceTransform = sourceSform * sourceFSL.inverse();//goes from FSL source space to real source space
    Vector3D sourceTransX, sourceTransY, sourceTransZ, sourceTransOff;
    sourceTransform.getAffineVectors(sourceTransX, sourceTransY, sourceTransZ, sourceTransOff);
    Vector3D fslX, fslY, fslZ, fslOff;
    refFSL.getAffineVectors(fslX, fslY, fslZ, fslOff);
    int64_t numVox = dims[0] * dims[1] * dims[2];
    vector<vector<float>> scratchFrames(3, vector<float>(numVox, 0.0f));
    #pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t k = 0; k < dims[2]; ++k)
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                Vector3D fslcoord = i * fslX + j * fslY + k * fslZ + fslOff;
                Vector3D coord, fslwarpval;
                newFile->indexToSpace(i, j, k, coord);
                fslwarpval[0] = newFile->getValue(i, j, k, 0);
                fslwarpval[1] = newFile->getValue(i, j, k, 1);
                fslwarpval[2] = newFile->getValue(i, j, k, 2);
                Vector3D fslTransAbsolute;
                if (absolute)
                {
                    fslTransAbsolute = fslwarpval;
                } else {
                    fslTransAbsolute = fslcoord + fslwarpval;
                }
                Vector3D transAbsolute = fslTransAbsolute[0] * sourceTransX + fslTransAbsolute[1] * sourceTransY + fslTransAbsolute[2] * sourceTransZ + sourceTransOff;
                Vector3D transdisplace = transAbsolute - coord;//internal format is always relative
                scratchFrames[0][newFile->getIndex(i, j, k)] = transdisplace[0];
                scratchFrames[1][newFile->getIndex(i, j, k)] = transdisplace[1];
                scratchFrames[2][newFile->getIndex(i, j, k)] = transdisplace[2];
            }
        }
    }
    newFile->setFrame(scratchFrames[0].data(), 0);
    newFile->setFrame(scratchFrames[1].data(), 1);
    newFile->setFrame(scratchFrames[2].data(), 2);
    m_warpfield = newFile;//drop the previous warpfield, and replace with the new one
}

void WarpfieldFile::writeWorld(const AString& warpname) const
{
    if (m_warpfield == NULL) throw DataFileException("writeWorld called on uninitialized warpfield");
    m_warpfield->writeFile(warpname);
}

void WarpfieldFile::writeITK(const AString& warpname) const
{
    if (m_warpfield == NULL) throw DataFileException("writeWorld called on uninitialized warpfield");
    vector<int64_t> dims;
    m_warpfield->getDimensions(dims);
    dims.resize(3);//keep only spatial dims
    dims.push_back(1);//ITK doesn't use time dimension
    dims.push_back(3);//it uses the fifth dimension
    VolumeFile outFile;
    NiftiHeader myHeader;
    myHeader.setIntent(NIFTI_INTENT_VECTOR, "vector");//ITK sets intent to vector
    outFile.reinitialize(dims, m_warpfield->getSform(), 1, SubvolumeAttributes::ANATOMY, &myHeader);
    outFile.setMapName(0, "x displacement");//the rest should still work on fifth dimension, we internally flatten non-spatial dimensions
    outFile.setMapName(1, "y displacement");
    outFile.setMapName(2, "z displacement");
    for (int64_t k = 0; k < dims[2]; ++k)
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                outFile.setValue(-m_warpfield->getValue(i, j, k, 0), i, j, k, 0);//negate x and y to convert RAS displacements to LPS
                outFile.setValue(-m_warpfield->getValue(i, j, k, 1), i, j, k, 1);
                outFile.setValue(m_warpfield->getValue(i, j, k, 2), i, j, k, 2);
            }
        }
    }
    outFile.writeFile(warpname);
}

void WarpfieldFile::writeFnirt(const AString& warpname, const AString& sourceName) const
{
    if (m_warpfield == NULL) throw DataFileException("writeFnirt called on uninitialized warpfield");
    FloatMatrix sourceSform, sourceFSL, refSform, refFSL;
    NiftiHeader myHeader;
    myHeader.setSForm(m_warpfield->getSform());
    myHeader.setDimensions(m_warpfield->getOriginalDimensions());
    refSform = FloatMatrix(myHeader.getSForm());
    refFSL = FloatMatrix(myHeader.getFSLSpace());
    NiftiIO sourceIO;
    sourceIO.openRead(sourceName);
    sourceSform = FloatMatrix(sourceIO.getHeader().getSForm());
    sourceFSL = FloatMatrix(sourceIO.getHeader().getFSLSpace());
    VolumeFile outFile;
    vector<int64_t> dims;
    m_warpfield->getDimensions(dims);
    dims.resize(4);//drop number of components
    outFile.reinitialize(dims, m_warpfield->getSform());
    outFile.setMapName(0, "x displacement");
    outFile.setMapName(1, "y displacement");
    outFile.setMapName(2, "z displacement");
    FloatMatrix FSLTransform = sourceFSL * sourceSform.inverse();//goes from real space to FSL source space
    Vector3D FSLTransX, FSLTransY, FSLTransZ, FSLTransOff;
    FSLTransform.getAffineVectors(FSLTransX, FSLTransY, FSLTransZ, FSLTransOff);
    Vector3D fslX, fslY, fslZ, fslOff;
    refFSL.getAffineVectors(fslX, fslY, fslZ, fslOff);
    int64_t numVox = dims[0] * dims[1] * dims[2];
    vector<vector<float>> scratchFrames(3, vector<float>(numVox, 0.0f));
    #pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t k = 0; k < dims[2]; ++k)
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                Vector3D fslcoord = i * fslX + j * fslY + k * fslZ + fslOff;
                Vector3D coord, realdisplacement;
                m_warpfield->indexToSpace(i, j, k, coord);
                realdisplacement[0] = m_warpfield->getValue(i, j, k, 0);
                realdisplacement[1] = m_warpfield->getValue(i, j, k, 1);
                realdisplacement[2] = m_warpfield->getValue(i, j, k, 2);
                Vector3D realabsolute = coord + realdisplacement;
                Vector3D fslabsolute = realabsolute[0] * FSLTransX + realabsolute[1] * FSLTransY + realabsolute[2] * FSLTransZ + FSLTransOff;
                Vector3D fsldisplace = fslabsolute - fslcoord;
                scratchFrames[0][outFile.getIndex(i, j, k)] = fsldisplace[0];
                scratchFrames[1][outFile.getIndex(i, j, k)] = fsldisplace[1];
                scratchFrames[2][outFile.getIndex(i, j, k)] = fsldisplace[2];
            }
        }
    }
    outFile.setFrame(scratchFrames[0].data(), 0);
    outFile.setFrame(scratchFrames[1].data(), 1);
    outFile.setFrame(scratchFrames[2].data(), 2);
    outFile.writeFile(warpname);
}
