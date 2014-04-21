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
#include "FileInformation.h"
#include "FloatMatrix.h"
#include "NiftiIO.h"

using namespace caret;
using namespace std;

void WarpfieldFile::readWorld(const AString& warpname)
{
    CaretPointer<VolumeFile> newFile(new VolumeFile());
    newFile->readFile(warpname);
    vector<int64_t> dims;
    newFile->getDimensions(dims);
    if (dims[3] != 3)
    {
        throw DataFileException("volume file '" + warpname + "' has the wrong number of subvolumes for a warpfield");
    }
    if (dims[4] != 1)
    {
        throw DataFileException("volume file '" + warpname + "' has multiple components, which is not allowed in a warpfield");
    }
    m_warpfield = newFile;//drop the previous warpfield, and replace with the new one
    m_warpfield->setMapName(0, "x displacement");
    m_warpfield->setMapName(1, "y displacement");
    m_warpfield->setMapName(2, "z displacement");
}

void WarpfieldFile::readFnirt(const AString& warpName, const AString& sourceName)
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
    newFile->readFile(warpName);
    vector<int64_t> dims;
    newFile->getDimensions(dims);
    if (dims[3] != 3)
    {
        throw DataFileException("volume file '" + warpName + "' has the wrong number of subvolumes for a warpfield");
    }
    if (dims[4] != 1)
    {
        throw DataFileException("volume file '" + warpName + "' has multiple components, which is not allowed in a warpfield");
    }
    FloatMatrix sourceTransform = sourceSform * sourceFSL.inverse();//goes from FSL source space to real source space
    Vector3D sourceTransX, sourceTransY, sourceTransZ, sourceTransOff;
    sourceTransform.getAffineVectors(sourceTransX, sourceTransY, sourceTransZ, sourceTransOff);
    Vector3D fslX, fslY, fslZ, fslOff;
    refFSL.getAffineVectors(fslX, fslY, fslZ, fslOff);
    for (int k = 0; k < dims[2]; ++k)
    {
        for (int j = 0; j < dims[1]; ++j)
        {
            for (int i = 0; i < dims[0]; ++i)
            {
                Vector3D fslcoord = i * fslX + j * fslY + k * fslZ + fslOff;
                Vector3D coord, fsldisplacement;
                newFile->indexToSpace(i, j, k, coord);
                fsldisplacement[0] = newFile->getValue(i, j, k, 0);
                fsldisplacement[1] = newFile->getValue(i, j, k, 1);
                fsldisplacement[2] = newFile->getValue(i, j, k, 2);
                Vector3D fslTransAbsolute = fslcoord + fsldisplacement;
                Vector3D transAbsolute = fslTransAbsolute[0] * sourceTransX + fslTransAbsolute[1] * sourceTransY + fslTransAbsolute[2] * sourceTransZ + sourceTransOff;
                Vector3D transdisplace = transAbsolute - coord;
                newFile->setValue(transdisplace[0], i, j, k, 0);//overwrite vectors in place to save memory
                newFile->setValue(transdisplace[1], i, j, k, 1);
                newFile->setValue(transdisplace[2], i, j, k, 2);
            }
        }
    }
    m_warpfield = newFile;//drop the previous warpfield, and replace with the new one
    m_warpfield->setMapName(0, "x displacement");
    m_warpfield->setMapName(1, "y displacement");
    m_warpfield->setMapName(2, "z displacement");
}

void WarpfieldFile::writeWorld(const AString& warpname)
{
    if (m_warpfield == NULL) throw DataFileException("writeWorld called on uninitialized warpfield");
    m_warpfield->writeFile(warpname);
}

void WarpfieldFile::writeFnirt(const AString& warpname, const AString& sourceName)
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
    for (int k = 0; k < dims[2]; ++k)
    {
        for (int j = 0; j < dims[1]; ++j)
        {
            for (int i = 0; i < dims[0]; ++i)
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
                outFile.setValue(fsldisplace[0], i, j, k, 0);
                outFile.setValue(fsldisplace[1], i, j, k, 1);
                outFile.setValue(fsldisplace[2], i, j, k, 2);
            }
        }
    }
    outFile.writeFile(warpname);
}
