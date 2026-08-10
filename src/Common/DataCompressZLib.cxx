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


#define GZIP_WINDOWS_BIT 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024


/**
 * From: https://stackoverflow.com/questions/2690328/qt-quncompress-gzip-data/24949005#24949005
 *
 * Uncompress data without knowing the uncompressed data length beforehand.
 * @param input The buffer to be decompressed
 * @param output The result of the decompression
 * @return @c true if the decompression was successfull, @c false otherwise
 */
bool
DataCompressZLib::uncompressData(QByteArray input, QByteArray &output)
{
    // Prepare output
    output.clear();
    
    // Is there something to do?
    if(input.length() > 0)
    {
        // Prepare inflater status
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;
        
        // Initialize inflater
        int ret = inflateInit2(&strm, GZIP_WINDOWS_BIT);
        
        if (ret != Z_OK)
            return(false);
        
        // Extract pointer to input data
        char *input_data = input.data();
        int input_data_left = input.length();
        
        // Decompress data until available
        do {
            // Determine current chunk size
            int chunk_size = qMin(GZIP_CHUNK_SIZE, input_data_left);
            
            // Check for termination
            if(chunk_size <= 0)
                break;
            
            // Set inflater references
            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;
            
            // Update interval variables
            input_data += chunk_size;
            input_data_left -= chunk_size;
            
            // Inflate chunk and cumulate output
            do {
                
                // Declare vars
                char out[GZIP_CHUNK_SIZE];
                
                // Set inflater references
                strm.next_out = (unsigned char*)out;
                strm.avail_out = GZIP_CHUNK_SIZE;
                
                // Try to inflate chunk
                ret = inflate(&strm, Z_NO_FLUSH);
                
                switch (ret) {
                    case Z_NEED_DICT:
                        ret = Z_DATA_ERROR;
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                    case Z_STREAM_ERROR:
                        // Clean-up
                        inflateEnd(&strm);
                        
                        // Return
                        return(false);
                }
                
                // Determine decompressed size
                int have = (GZIP_CHUNK_SIZE - strm.avail_out);
                
                // Cumulate result
                if(have > 0)
                    output.append((char*)out, have);
                
            } while (strm.avail_out == 0);
            
        } while (ret != Z_STREAM_END);
        
        // Clean-up
        inflateEnd(&strm);
        
        // Return
        return (ret == Z_STREAM_END);
    }
    else
        return(true);
}
