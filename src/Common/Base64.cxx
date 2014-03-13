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
  Module:    $RCSfile: Base64.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "CaretAssert.h"

#include "Base64.h"

using namespace caret;

//----------------------------------------------------------------------------
static const unsigned char Base64EncodeTable[65] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

//----------------------------------------------------------------------------
inline static unsigned char Base64EncodeChar(unsigned char c)
{
  CaretAssert( c < 65 );
  return Base64EncodeTable[c];
}

Base64::Base64() {}

Base64::~Base64() {}

//----------------------------------------------------------------------------
void Base64::EncodeTriplet(unsigned char i0,
                                       unsigned char i1,
                                       unsigned char i2,
                                       unsigned char *o0,
                                       unsigned char *o1,
                                       unsigned char *o2,
                                       unsigned char *o3)
{
  *o0 = Base64EncodeChar((i0 >> 2) & 0x3F);
  *o1 = Base64EncodeChar(((i0 << 4) & 0x30)|((i1 >> 4) & 0x0F));
  *o2 = Base64EncodeChar(((i1 << 2) & 0x3C)|((i2 >> 6) & 0x03));
  *o3 = Base64EncodeChar(i2 & 0x3F);
}

//----------------------------------------------------------------------------
void Base64::EncodePair(unsigned char i0,
                                    unsigned char i1,
                                    unsigned char *o0,
                                    unsigned char *o1,
                                    unsigned char *o2,
                                    unsigned char *o3)
{
  *o0 = Base64EncodeChar((i0 >> 2) & 0x3F);
  *o1 = Base64EncodeChar(((i0 << 4) & 0x30)|((i1 >> 4) & 0x0F));
  *o2 = Base64EncodeChar(((i1 << 2) & 0x3C));
  *o3 = '=';
}

//----------------------------------------------------------------------------
void Base64::EncodeSingle(unsigned char i0,
                                      unsigned char *o0,
                                      unsigned char *o1,
                                      unsigned char *o2,
                                      unsigned char *o3)
{
  *o0 = Base64EncodeChar((i0 >> 2) & 0x3F);
  *o1 = Base64EncodeChar(((i0 << 4) & 0x30));
  *o2 = '=';
  *o3 = '=';
}

//----------------------------------------------------------------------------
uint64_t Base64::encode(const unsigned char *input, 
                             uint64_t length, 
                             unsigned char *output,
                             int32_t mark_end)
{
  
  const unsigned char *ptr = input;
  const unsigned char *end = input + length;
  unsigned char *optr = output;

  // Encode complete triplet

  while ((end - ptr) >= 3)
    {
    Base64::EncodeTriplet(ptr[0], ptr[1], ptr[2], 
                                      &optr[0], &optr[1], &optr[2], &optr[3]);
    ptr += 3;
    optr += 4;
    }

  // Encodes a 2-byte ending into 3 bytes and 1 pad byte and writes.

  if (end - ptr == 2)
    {
    Base64::EncodePair(ptr[0], ptr[1],
                                   &optr[0], &optr[1], &optr[2], &optr[3]);
    optr += 4;
    }

  // Encodes a 1-byte ending into 2 bytes and 2 pad bytes
  
  else if (end - ptr == 1)
    {
    Base64::EncodeSingle(ptr[0],
                                     &optr[0], &optr[1], &optr[2], &optr[3]);
    optr += 4;
    }

  // Do we need to mark the end

  else if (mark_end)
    {
    optr[0] = optr[1] = optr[2] = optr[3] = '=';
    optr += 4;
    }

  return optr - output;
}

//----------------------------------------------------------------------------
static const unsigned char Base64DecodeTable[256] =
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0x3E,0xFF,0xFF,0xFF,0x3F,
  0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
  0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
  0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,
  0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
  0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
  0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,
  0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
  0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,
  0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,
  //-------------------------------------
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

//----------------------------------------------------------------------------
inline static unsigned char Base64DecodeChar(unsigned char c)
{
  return Base64DecodeTable[c];
}

//----------------------------------------------------------------------------
int Base64::DecodeTriplet(unsigned char i0,
                                      unsigned char i1,
                                      unsigned char i2,
                                      unsigned char i3,
                                      unsigned char *o0,
                                      unsigned char *o1,
                                      unsigned char *o2)
{
  unsigned char d0, d1, d2, d3;

  d0 = Base64DecodeChar(i0);
  d1 = Base64DecodeChar(i1);
  d2 = Base64DecodeChar(i2);
  d3 = Base64DecodeChar(i3);
  
  // Make sure all characters were valid

  if (d0 == 0xFF || d1 == 0xFF || d2 == 0xFF || d3 == 0xFF)
    { 
    return 0; 
    }
  
  // Decode the 3 bytes

  *o0 = ((d0 << 2) & 0xFC) | ((d1 >> 4) & 0x03);
  *o1 = ((d1 << 4) & 0xF0) | ((d2 >> 2) & 0x0F);
  *o2 = ((d2 << 6) & 0xC0) | ((d3 >> 0) & 0x3F);
  
  // Return the number of bytes actually decoded

  if (i2 == '=') 
    { 
    return 1; 
    }
  if (i3 == '=') 
    { 
    return 2; 
    }
  return 3;
}

//----------------------------------------------------------------------------
uint64_t Base64::decode(const unsigned char *input, 
                             uint64_t length, 
                             unsigned char *output,
                             uint64_t max_input_length)
{
  const unsigned char *ptr = input;
  unsigned char *optr = output;

  // Decode complete triplet

  if (max_input_length)
    {
    const unsigned char *end = input + max_input_length;
    while (ptr < end)
      {
      int len = 
        Base64::DecodeTriplet(ptr[0], ptr[1], ptr[2], ptr[3], 
                                          &optr[0], &optr[1], &optr[2]);
      optr += len;
      if(len < 3)
        {
        return optr - output;
        }
      ptr += 4;
      }
    } 
  else 
    {
    unsigned char *oend = output + length;
    while ((oend - optr) >= 3)
      {
      int len = 
        Base64::DecodeTriplet(ptr[0], ptr[1], ptr[2], ptr[3], 
                                          &optr[0], &optr[1], &optr[2]);
      optr += len;
      if(len < 3)
        {
        return optr - output;
        }
      ptr += 4;
      }

    // Decode the last triplet
  
    unsigned char temp;
    if (oend - optr == 2)
      {
      int len = 
        Base64::DecodeTriplet(ptr[0], ptr[1], ptr[2], ptr[3], 
                                          &optr[0], &optr[1], &temp);
      optr += (len > 2 ? 2 : len); 
      }
    else if (oend - optr == 1)
      {
      unsigned char temp2;
      int len = 
        Base64::DecodeTriplet(ptr[0], ptr[1], ptr[2], ptr[3], 
                                          &optr[0], &temp, &temp2);
      optr += (len > 2 ? 2 : len); 
      }
    }

  return optr - output;
}
