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
