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
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: DataCompressZLib.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "DataCompressZLib.h"
#include "MathFunctions.h"
#include "zlib.h"

using namespace caret;

//----------------------------------------------------------------------------
DataCompressZLib::DataCompressZLib()
{
  this->compressionLevel = Z_DEFAULT_COMPRESSION;
}

//----------------------------------------------------------------------------
DataCompressZLib::~DataCompressZLib()
{ 
}

int32_t 
DataCompressZLib::getCompressionLevel()
{
    return this->compressionLevel;
}

void 
DataCompressZLib::setCompressionLevel(const int32_t compressionLevel)
{
    this->compressionLevel = MathFunctions::clamp(compressionLevel, 0, 9);
}

//----------------------------------------------------------------------------
uint64_t
DataCompressZLib::compressData(const unsigned char* uncompressedData,
                                      uint64_t uncompressedSize,
                                      unsigned char* compressedData,
                                      const uint64_t compressionSpace)
{
  uLongf compressedSize = compressionSpace;
  Bytef* cd = reinterpret_cast<Bytef*>(compressedData);
  const Bytef* ud = reinterpret_cast<const Bytef*>(uncompressedData);
  
  // Call zlib's compress function.
  if(compress2(cd, &compressedSize, ud, uncompressedSize, this->compressionLevel) != Z_OK)
    {
    //vtkErrorMacro("Zlib error while compressing data.");
    return 0;
    }
  
  return compressedSize;
}

//----------------------------------------------------------------------------
uint64_t
DataCompressZLib::uncompressData(const unsigned char* compressedData,
                                        uint64_t compressedSize,
                                        unsigned char* uncompressedData,
                                        uint64_t uncompressedSize)
{  
  uLongf decSize = uncompressedSize;
  Bytef* ud = reinterpret_cast<Bytef*>(uncompressedData);
  const Bytef* cd = reinterpret_cast<const Bytef*>(compressedData);
  
  // Call zlib's uncompress function.
  if(uncompress(ud, &decSize, cd, compressedSize) != Z_OK)
    {    
    //vtkErrorMacro("Zlib error while uncompressing data.");
    return 0;
    }
  
  // Make sure the output size matched that expected.
  if(decSize != uncompressedSize)
    {
    //vtkErrorMacro("Decompression produced incorrect size.\n"
    //              "Expected " << uncompressedSize << " and got " << decSize);
    return 0;
    }
  
  return decSize;
}

//----------------------------------------------------------------------------
unsigned long
DataCompressZLib::getMaximumCompressionSpace(unsigned long size)
{
  // ZLib specifies that destination buffer must be 0.1% larger + 12 bytes.
  return size + (size+999)/1000 + 12;
}
