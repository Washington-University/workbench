#ifndef __DATA_COMPRESS_ZLIB_H__
#define __DATA_COMPRESS_ZLIB_H__

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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: DataCompressZLib.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME DataCompressZLib - Data compression using zlib.
// .SECTION Description
// DataCompressZLib provides a concrete vtkDataCompressor class
// using zlib for compressing and uncompressing data.

#include <stdint.h>
#include "CaretObject.h"

namespace caret {
    
/*
 * Copied from vtkZLibDataCompresson.
 */
class  DataCompressZLib : public CaretObject
{
public:
    DataCompressZLib();
    
    ~DataCompressZLib();

  // Description:  
  // Get the maximum space that may be needed to store data of the
  // given uncompressed size after compression.  This is the minimum
  // size of the output buffer that can be passed to the four-argument
  // Compress method.
  unsigned long getMaximumCompressionSpace(unsigned long size);

  // Description:
  // Get/Set the compression level.
  //vtkSetClampMacro(CompressionLevel, int, 0, 9);
  //vtkGetMacro(CompressionLevel, int);
  
    int32_t getCompressionLevel();
    
    void setCompressionLevel(const int32_t compressionLevel);
  // Compression method required by vtkDataCompressor.
    uint64_t compressData(const unsigned char* uncompressedData,
                               uint64_t uncompressedSize,
                               unsigned char* compressedData,
                               const uint64_t compressionSpace);
  // Decompression method required by vtkDataCompressor.
   uint64_t uncompressData(const unsigned char* compressedData,
                                 uint64_t compressedSize,
                                 unsigned char* uncompressedData,
                                 uint64_t uncompressedSiz);
protected:    
    int compressionLevel;
    
};

} // namespace

#endif // __DATA_COMPRESS_ZLIB_H__
