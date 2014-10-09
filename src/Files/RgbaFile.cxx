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

#include "CaretAssert.h"

#include "DataFileTypeEnum.h"
#include "GiftiFile.h"
#include "MathFunctions.h"
#include "RgbaFile.h"

using namespace caret;

/**
 * Constructor.
 */
RgbaFile::RgbaFile()
: GiftiTypeFile(DataFileTypeEnum::RGBA)
{
    this->initializeMembersRgbaFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
RgbaFile::RgbaFile(const RgbaFile& sf)
: GiftiTypeFile(sf)
{
    this->copyHelperRgbaFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the RgbaFile parameter.
 */
RgbaFile& 
RgbaFile::operator=(const RgbaFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperRgbaFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
RgbaFile::~RgbaFile()
{
    
}

/**
 * Clear the surface file.
 */
void 
RgbaFile::clear()
{
    GiftiTypeFile::clear();
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
RgbaFile::validateDataArraysAfterReading()
{
    this->initializeMembersRgbaFile();
    
    this->verifyDataArraysHaveSameNumberOfRows(3, 4);
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int32_t
RgbaFile::getNumberOfNodes() const
{
    int32_t numNodes = 0;
    int32_t numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays > 0) {
        numNodes = this->giftiFile->getDataArray(0)->getNumberOfRows();
    }
    return numNodes;
}

/**
 * Get the number of columns.
 *
 * @return
 *   The number of columns.
 */
int32_t
RgbaFile::getNumberOfColumns() const
{
    const int32_t numCols = this->giftiFile->getNumberOfDataArrays();
    return numCols;
}


/**
 * Initialize members of this class.
 */
void 
RgbaFile::initializeMembersRgbaFile()
{
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
RgbaFile::copyHelperRgbaFile(const RgbaFile& /*sf*/)
{
    this->validateDataArraysAfterReading();
}


