
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
/*LICENSE_END*/

#include "CaretAssert.h"
#include "FloatMatrix.h"
#include "MatrixFunctions.h"

using namespace caret;
using namespace std;

bool caret::FloatMatrix::checkDimensions() const
{
   uint64_t rows = m_matrix.size(), cols;
   if (rows == 0) return true;//treat it as fine for now
   cols = m_matrix[0].size();
   for (uint64_t i = 1; i < rows; ++i)
   {
      if (m_matrix[i].size() != cols)
      {
         return false;
      }
   }
   return true;
}

FloatMatrix::FloatMatrix(const vector<vector<float> >& matrixIn)
{
   m_matrix = matrixIn;
   CaretAssertion(checkDimensions());
}

bool FloatMatrix::operator!=(const FloatMatrix& right) const
{
   return !(*this == right);
}

FloatMatrix FloatMatrix::operator*(const FloatMatrix& right) const
{
   FloatMatrix ret(*this);
   ret *= right;
   return ret;
}

FloatMatrix& FloatMatrix::operator*=(const FloatMatrix& right)
{
   MatrixFunctions mymf;
   mymf.multiply(m_matrix, right.m_matrix, m_matrix);//would need a copy anyway, so let it make the copy internally
   return *this;
}

FloatMatrix FloatMatrix::concatHoriz(const FloatMatrix& right) const
{
   MatrixFunctions mymf;
   FloatMatrix ret;
   mymf.horizCat(m_matrix, right.m_matrix, ret.m_matrix);
   return ret;
}

FloatMatrix FloatMatrix::concatVert(const FloatMatrix& bottom) const
{
   MatrixFunctions mymf;
   FloatMatrix ret;
   mymf.vertCat(m_matrix, bottom.m_matrix, ret.m_matrix);
   return ret;
}

FloatMatrix FloatMatrix::getRange(const int64_t firstRow, const int64_t afterLastRow, const int64_t firstCol, const int64_t afterLastCol) const
{
   MatrixFunctions mymf;
   FloatMatrix ret;
   mymf.getChunk(firstRow, afterLastRow, firstCol, afterLastCol, m_matrix, ret.m_matrix);
   return ret;
}

FloatMatrix FloatMatrix::identity(const int64_t rows)
{
   MatrixFunctions mymf;
   FloatMatrix ret;
   mymf.identity(rows, ret.m_matrix);
   return ret;
}

FloatMatrix FloatMatrix::inverse() const
{
   MatrixFunctions mymf;
   FloatMatrix ret;
   mymf.inverse(m_matrix, ret.m_matrix);
   return ret;
}

FloatMatrix& FloatMatrix::operator*=(const float& right)
{
   MatrixFunctions mymf;
   mymf.multiply(m_matrix, right, m_matrix);//internally makes a copy
   return *this;
}

FloatMatrix FloatMatrix::operator+(const FloatMatrix& right) const
{
   FloatMatrix ret(*this);
   ret += right;
   return ret;
}

FloatMatrix& FloatMatrix::operator+=(const FloatMatrix& right)
{
   MatrixFunctions mymf;
   mymf.add(m_matrix, right.m_matrix, m_matrix);//internally makes a copy
   return *this;
}

FloatMatrix& FloatMatrix::operator+=(const float& right)
{
   MatrixFunctions mymf;
   mymf.add(m_matrix, right, m_matrix);//internally makes a copy
   return *this;
}

FloatMatrix FloatMatrix::operator-(const FloatMatrix& right) const
{
   FloatMatrix ret(*this);
   ret -= right;
   return ret;
}

FloatMatrix& FloatMatrix::operator-=(const FloatMatrix& right)
{
   MatrixFunctions mymf;
   mymf.subtract(m_matrix, right.m_matrix, m_matrix);//internally makes a copy
   return *this;
}

FloatMatrix& FloatMatrix::operator-=(const float& right)
{
   MatrixFunctions mymf;
   mymf.add(m_matrix, (-right), m_matrix);//internally makes a copy
   return *this;
}

FloatMatrix& FloatMatrix::operator/=(const float& right)
{
   return ((*this) *= 1.0f / right);
}

bool FloatMatrix::operator==(const FloatMatrix& right) const
{
   if (this == &right)
   {
      return true;//short circuit true on pointer equivalence
   }
   int64_t i, j, rows = (int64_t)m_matrix.size(), cols;
   if (rows != (int64_t)right.m_matrix.size())
   {
      return false;
   }
   if (rows == 0)
   {
      return true;//don't try to get the second dimension
   }
   cols = (int64_t)m_matrix[0].size();
   if (cols != (int64_t)right.m_matrix[0].size())
   {
      return false;
   }
   for (i = 0; i < rows; ++i)
   {
      for (j = 0; j < cols; ++j)
      {
         if (m_matrix[i][j] != right.m_matrix[i][j])
         {
            return false;
         }
      }
   }
   return true;
}

void FloatMatrix::getDimensions(int64_t& rows, int64_t& cols) const
{
   rows = (int64_t)m_matrix.size();
   if (rows == 0)
   {
      cols = 0;
   } else {
      cols = (int64_t)m_matrix[0].size();
   }
}

FloatMatrixRowRef FloatMatrix::operator[](const int64_t& index)
{
   FloatMatrixRowRef ret(m_matrix[index]);
   return ret;
}

FloatMatrix FloatMatrix::reducedRowEchelon() const
{
   FloatMatrix ret(*this);
   MatrixFunctions mymf;
   mymf.rref(ret.m_matrix);
   return ret;
}

void FloatMatrix::resize(const int64_t rows, const int64_t cols, const bool destructive)
{
   MatrixFunctions mymf;
   mymf.resize(rows, cols, m_matrix, destructive);
}

FloatMatrix FloatMatrix::transpose() const
{
   FloatMatrix ret;
   MatrixFunctions mymf;
   mymf.transpose(m_matrix, ret.m_matrix);
   return ret;
}

FloatMatrix FloatMatrix::zeros(const int64_t rows, const int64_t cols)
{
   FloatMatrix ret;
   MatrixFunctions mymf;
   mymf.zeros(rows, cols, ret.m_matrix);
   return ret;
}

const vector<vector<float> >& FloatMatrix::getMatrix() const
{
   return m_matrix;
}

FloatMatrix FloatMatrix::operator-() const
{
   int64_t rows, cols;
   getDimensions(rows, cols);
   FloatMatrix ret = zeros(rows, cols);
   ret -= *this;
   return ret;
}

FloatMatrixRowRef::FloatMatrixRowRef(vector<float>& therow) : m_row(therow)
{
}

FloatMatrixRowRef& FloatMatrixRowRef::operator=(const FloatMatrixRowRef& right)
{
   if (this == &right)
   {//just in case vector isn't smart enough to check self assignment
      return *this;
   }
   CaretAssert(m_row.size() == right.m_row.size());//maybe this should be an exception, not an assertion?
   m_row = right.m_row;
   return *this;
}

FloatMatrixRowRef& FloatMatrixRowRef::operator=(const float& right)
{
   for (int64_t i = 0; i < (int64_t)m_row.size(); ++i)
   {
      m_row[i] = right;
   }
   return *this;
}

float& caret::FloatMatrixRowRef::operator[](const int64_t& index)
{
   CaretAssert(index < (int64_t)m_row.size());//instead of segfaulting, explicitly check in debug
   return m_row[index];
}

FloatMatrixRowRef::FloatMatrixRowRef(FloatMatrixRowRef& right) : m_row(right.m_row)
{
}
