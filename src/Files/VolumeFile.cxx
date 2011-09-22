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
#include "CaretAssert.h"
#include "FloatMatrix.h"
#include <cmath>
#include "nifti1.h"
#include "nifti2.h"

using namespace caret;
using namespace std;

void VolumeFile::reinitialize(const vector<float>& dimensionsIn, const vector<vector<float> >& indexToSpace)
{
   if (m_data != NULL)
   {
      delete[] m_data;
      m_data = NULL;
   }
   CaretAssert(dimensionsIn.size() >= 3);
   CaretAssert(indexToSpace.size() >= 3);//support using the 3x4 part of a 4x4 matrix
   CaretAssert(indexToSpace[0].size() == 4);
   CaretAssert(indexToSpace[1].size() == 4);
   CaretAssert(indexToSpace[2].size() == 4);
   m_indexToSpace = indexToSpace;
   m_indexToSpace.resize(3);//drop the fourth row if it exists
   FloatMatrix temp(m_indexToSpace);
   FloatMatrix temp2 = temp.getRange(0, 0, 3, 3);//get the multiplicative part
   FloatMatrix temp3 = temp.getRange(0, 3, 3, 4);//get the additive part
   temp = temp2.inverse();//invert multiplicative part
   temp2 = temp * -temp3;//multiply the reversed vector by the inverse of the spacing to get the reverse origin
   temp3 = temp.concatHoriz(temp2);//concatenate reverse origin with inverse spacing to get inverted affine
   m_spaceToIndex = temp3.getMatrix();
   m_dimensions[0] = dimensionsIn[0];
   m_dimensions[1] = dimensionsIn[1];
   m_dimensions[2] = dimensionsIn[2];
   m_dimensions[3] = 1;
   for (int i = 3; i < (int)dimensionsIn.size(); ++i)
   {
      if (dimensionsIn[i] != 0)
      {
         m_dimensions[3] *= dimensionsIn[i];
      }
   }
   int64_t totalSize = m_dimensions[0] * m_dimensions[1] * m_dimensions[2] * m_dimensions[3];
   m_data = new float[totalSize];
   CaretAssert(m_data != NULL);
   //TODO: adjust any existing nifti header to match, or remove nifti header?
}

VolumeFile::VolumeFile()
{
   m_data = NULL;
   m_N1Header = NULL;
   m_N2Header = NULL;
   m_headerType = NONE;
   m_dimensions[0] = 0;
   m_dimensions[1] = 0;
   m_dimensions[2] = 0;
   m_dimensions[3] = 0;
   m_indexToSpace.resize(3);
   for (int i = 0; i < 3; ++i)
   {
      m_indexToSpace[i].resize(4);
      for (int j = 0; j < 4; ++j)
      {
         m_indexToSpace[i][j] = ((i == j) ? 1.0f : 0.0f);//default 1mm spacing, no origin
      }
   }
   m_spaceToIndex = m_indexToSpace;
}

VolumeFile::VolumeFile(const std::vector< float >& dimensionsIn, const std::vector< std::vector< float > >& indexToSpace)
{
   m_data = NULL;
   m_N1Header = NULL;
   m_N2Header = NULL;
   m_headerType = NONE;
   reinitialize(dimensionsIn, indexToSpace);
}

void VolumeFile::getOrientAndSpacingForPlumb(OrientTypes* orientOut, float* spacingOut, float* centerOut)
{
   CaretAssert(isPlumb());//this will fail MISERABLY on non-plumb volumes, so assert plumb
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         if (m_indexToSpace[i][j] != 0.0f)
         {
            spacingOut[j] = m_indexToSpace[i][j];
            centerOut[j] = m_indexToSpace[i][3];
            bool negative;
            if (m_indexToSpace[i][j] > 0.0f)
            {
               negative = true;
            } else {
               negative = false;
            }
            switch (i)
            {
               case 0:
                  //left/right
                  orientOut[j] = (negative ? RIGHT_TO_LEFT : LEFT_TO_RIGHT);
                  break;
               case 1:
                  //forward/back
                  orientOut[j] = (negative ? ANTERIOR_TO_POSTERIOR : POSTERIOR_TO_ANTERIOR);
                  break;
               case 2:
                  //up/down
                  orientOut[j] = (negative ? SUPERIOR_TO_INFERIOR : INFERIOR_TO_SUPERIOR);
                  break;
               default:
                  //will never get called
                  break;
            };
         }
      }
   }
}

void VolumeFile::closestVoxel(const float* coordIn, int64_t* indexOut)
{
   closestVoxel(coordIn[0], coordIn[1], coordIn[2], indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t* indexOut)
{
   closestVoxel(coordIn1, coordIn2, coordIn3, indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::closestVoxel(const float* coordIn, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3)
{
   closestVoxel(coordIn[0], coordIn[1], coordIn[2], indexOut1, indexOut2, indexOut3);
}

void VolumeFile::closestVoxel(const float& coordIn1, const float& coordIn2, const float& coordIn3, int64_t& indexOut1, int64_t& indexOut2, int64_t& indexOut3)
{
   float tempInd1, tempInd2, tempInd3;
   spaceToIndex(coordIn1, coordIn2, coordIn3, tempInd1, tempInd2, tempInd3);
   indexOut1 = (int32_t)floor(0.5f + tempInd1);
   indexOut2 = (int32_t)floor(0.5f + tempInd2);
   indexOut3 = (int32_t)floor(0.5f + tempInd3);
}

float VolumeFile::getValue(const int64_t* indexIn, const int64_t timeIndex)
{
   return getValue(indexIn[0], indexIn[1], indexIn[2], timeIndex);
}

float VolumeFile::getValue(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t timeIndex)
{
   //for now, do it the slow way
   CaretAssert(indexValid(indexIn1, indexIn2, indexIn3, timeIndex));//assert so release version isn't slowed by checking
   int64_t index = getIndex(indexIn1, indexIn2, indexIn3, timeIndex);
   return m_data[index];
}

void VolumeFile::getDimensions(std::vector< float >& dimOut)
{
   dimOut.resize(4);
   getDimensions(dimOut[0], dimOut[1], dimOut[2], dimOut[3]);
}

void VolumeFile::getDimensions(float& dimOut1, float& dimOut2, float& dimOut3, float& dimOut4)
{
   dimOut1 = m_dimensions[0];
   dimOut2 = m_dimensions[1];
   dimOut3 = m_dimensions[2];
   dimOut4 = m_dimensions[3];
}

int64_t VolumeFile::getIndex(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t timeIndex)
{
   return indexIn1 + m_dimensions[0] * (indexIn2 + m_dimensions[1] * (indexIn3 + timeIndex * m_dimensions[2]));
}

void VolumeFile::indexToSpace(const int64_t* indexIn, float* coordOut)
{
   indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float* coordOut)
{
   indexToSpace(indexIn1, indexIn2, indexIn3, coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const int64_t* indexIn, float& coordOut1, float& coordOut2, float& coordOut3)
{
   indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut1, coordOut2, coordOut3);
}

void VolumeFile::indexToSpace(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3)
{
   coordOut1 = m_indexToSpace[0][0] * indexIn1 + m_indexToSpace[0][1] * indexIn2 + m_indexToSpace[0][2] * indexIn3 + m_indexToSpace[0][3];
   coordOut2 = m_indexToSpace[1][0] * indexIn1 + m_indexToSpace[1][1] * indexIn2 + m_indexToSpace[1][2] * indexIn3 + m_indexToSpace[1][3];
   coordOut3 = m_indexToSpace[2][0] * indexIn1 + m_indexToSpace[2][1] * indexIn2 + m_indexToSpace[2][2] * indexIn3 + m_indexToSpace[2][3];
}

void VolumeFile::indexToSpace(const float* indexIn, float* coordOut)
{
   indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float* coordOut)
{
   indexToSpace(indexIn1, indexIn2, indexIn3, coordOut[0], coordOut[1], coordOut[2]);
}

void VolumeFile::indexToSpace(const float* indexIn, float& coordOut1, float& coordOut2, float& coordOut3)
{
   indexToSpace(indexIn[0], indexIn[1], indexIn[2], coordOut1, coordOut2, coordOut3);
}

void VolumeFile::indexToSpace(const float& indexIn1, const float& indexIn2, const float& indexIn3, float& coordOut1, float& coordOut2, float& coordOut3)
{
   coordOut1 = m_indexToSpace[0][0] * indexIn1 + m_indexToSpace[0][1] * indexIn2 + m_indexToSpace[0][2] * indexIn3 + m_indexToSpace[0][3];
   coordOut2 = m_indexToSpace[1][0] * indexIn1 + m_indexToSpace[1][1] * indexIn2 + m_indexToSpace[1][2] * indexIn3 + m_indexToSpace[1][3];
   coordOut3 = m_indexToSpace[2][0] * indexIn1 + m_indexToSpace[2][1] * indexIn2 + m_indexToSpace[2][2] * indexIn3 + m_indexToSpace[2][3];
}

bool VolumeFile::isPlumb()
{
   char axisUsed = 0;
   char indexUsed = 0;
   for (int i = 0; i < 3; ++i)
   {
      for (int j = 0; j < 3; ++j)
      {
         if (m_indexToSpace[i][j] != 0.0f)
         {
            if (axisUsed & (1<<i))
            {
               return false;
            }
            if (indexUsed & (1<<j))
            {
               return false;
            }
            axisUsed &= (1<<i);
            indexUsed &= (1<<j);
         }
      }
   }
   return true;
}

void VolumeFile::spaceToIndex(const float* coordIn, float* indexOut)
{
   spaceToIndex(coordIn[0], coordIn[1], coordIn[2], indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float* indexOut)
{
   spaceToIndex(coordIn1, coordIn2, coordIn3, indexOut[0], indexOut[1], indexOut[2]);
}

void VolumeFile::spaceToIndex(const float* coordIn, float& indexOut1, float& indexOut2, float& indexOut3)
{
   spaceToIndex(coordIn[0], coordIn[1], coordIn[2], indexOut1, indexOut2, indexOut3);
}

void VolumeFile::spaceToIndex(const float& coordIn1, const float& coordIn2, const float& coordIn3, float& indexOut1, float& indexOut2, float& indexOut3)
{
   indexOut1 = m_spaceToIndex[0][0] * coordIn1 + m_spaceToIndex[0][1] * coordIn2 + m_spaceToIndex[0][2] * coordIn3 + m_spaceToIndex[0][3];
   indexOut2 = m_spaceToIndex[1][0] * coordIn1 + m_spaceToIndex[1][1] * coordIn2 + m_spaceToIndex[1][2] * coordIn3 + m_spaceToIndex[1][3];
   indexOut3 = m_spaceToIndex[2][0] * coordIn1 + m_spaceToIndex[2][1] * coordIn2 + m_spaceToIndex[2][2] * coordIn3 + m_spaceToIndex[2][3];
}

void VolumeFile::setValue(const float& valueIn, const int64_t* indexIn, const int64_t timeIndex)
{
   setValue(valueIn, indexIn[0], indexIn[1], indexIn[2], timeIndex);
}

void VolumeFile::setValue(const float& valueIn, const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t timeIndex)
{
   //for now, the slow way
   CaretAssert(indexValid(indexIn1, indexIn2, indexIn3, timeIndex));//assert so release version isn't slowed by checking
   int64_t index = getIndex(indexIn1, indexIn2, indexIn3, timeIndex);
   m_data[index] = valueIn;
}

bool VolumeFile::indexValid(const int64_t& indexIn1, const int64_t& indexIn2, const int64_t& indexIn3, const int64_t timeIndex)
{
   if (indexIn1 < 0 || indexIn1 >= m_dimensions[0]) return false;
   if (indexIn2 < 0 || indexIn2 >= m_dimensions[1]) return false;
   if (indexIn3 < 0 || indexIn3 >= m_dimensions[2]) return false;
   if (timeIndex < 0 || timeIndex >= m_dimensions[3]) return false;
   return true;
}
