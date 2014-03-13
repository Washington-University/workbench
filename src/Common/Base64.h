#ifndef __BASE64_H__
#define __BASE64_H__

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
  Module:    $RCSfile: Base64.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Base64 - base64 encode and decode utilities.
// .SECTION Description
// Base64 implements base64 encoding and decoding.

#include <stdint.h>
#include "CaretObject.h"

namespace caret {
    
/**
 * This is copied directly from VTK's vtkBase64Utilities class.
 */
class Base64 : public CaretObject
{
private:
    Base64();

    ~Base64();
    
public:

  // Description:
  // Encode 'length' bytes from the input buffer and store the
  // encoded stream into the output buffer. Return the length of
  // the encoded stream. Note that the output buffer must be allocated
  // by the caller (length * 1.5 should be a safe estimate).
  // If 'mark_end' is true than an extra set of 4 bytes is added
  // to the end of the stream if the input is a multiple of 3 bytes.
  // These bytes are invalid chars and therefore they will stop the decoder
  // thus enabling the caller to decode a stream without actually knowing 
  // how much data to expect (if the input is not a multiple of 3 bytes then
  // the extra padding needed to complete the encode 4 bytes will stop the 
  // decoding anyway).
  static uint64_t encode(const unsigned char *input, 
                              uint64_t length, 
                              unsigned char *output,
                              int32_t mark_end = 0);


  // Description:  
  // Decode bytes from the input buffer and store the decoded stream 
  // into the output buffer until 'length' bytes have been decoded. 
  // Return the real length of the decoded stream (which should be equal to 
  // 'length'). Note that the output buffer must be allocated by the caller.
  // If 'max_input_length' is not null, then it specifies the number of
  // encoded bytes that should be at most read from the input buffer. In
  // that case the 'length' parameter is ignored. This enables the caller 
  // to decode a stream without actually knowing how much decoded data to 
  // expect (of course, the buffer must be large enough).
  static uint64_t decode(const unsigned char *input, 
                              uint64_t length, 
                              unsigned char *output,
                              uint64_t max_input_length = 0);
    
private:
    // Description:  
    // Decode 4 bytes into 3 bytes.
    static int DecodeTriplet(unsigned char i0,
                             unsigned char i1,
                             unsigned char i2,
                             unsigned char i3,
                             unsigned char *o0,
                             unsigned char *o1,
                             unsigned char *o2);
    
    // Description:  
    // Encode 3 bytes into 4 bytes
    static void EncodeTriplet(unsigned char i0,
                              unsigned char i1,
                              unsigned char i2,
                              unsigned char *o0,
                              unsigned char *o1,
                              unsigned char *o2,
                              unsigned char *o3);
    
    // Description:  
    // Encode 2 bytes into 4 bytes
    static void EncodePair(unsigned char i0,
                           unsigned char i1,
                           unsigned char *o0,
                           unsigned char *o1,
                           unsigned char *o2,
                           unsigned char *o3);
    
    // Description:  
    // Encode 1 byte into 4 bytes
    static void EncodeSingle(unsigned char i0,
                             unsigned char *o0,
                             unsigned char *o1,
                             unsigned char *o2,
                             unsigned char *o3);
    

};

} // namespace
#endif // __BASE64_H__
