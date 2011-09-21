
#ifndef __FLOAT_MATRIX_H__
#define __FLOAT_MATRIX_H__

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

#include <vector>
#include "stdint.h"

namespace caret {

   class FloatMatrix
   {
      std::vector<std::vector<float> > m_matrix;
      bool checkDimensions();//put this inside asserts at the end of functions
   public:
      FloatMatrix() { };//to make the compiler happy
      FloatMatrix(const std::vector<std::vector<float> >& matrixIn);
      std::vector<float>& operator[](const int64_t& index);
      FloatMatrix& operator+=(const FloatMatrix& right);
      FloatMatrix& operator-=(const FloatMatrix& right);
      FloatMatrix& operator*=(const FloatMatrix& right);
      FloatMatrix& operator+=(const float& right);
      FloatMatrix& operator-=(const float& right);
      FloatMatrix& operator*=(const float& right);
      FloatMatrix& operator/=(const float& right);
      FloatMatrix operator+(const FloatMatrix& right) const;
      FloatMatrix operator-(const FloatMatrix& right) const;
      FloatMatrix operator-() const;
      FloatMatrix operator*(const FloatMatrix& right) const;
      bool operator==(const FloatMatrix& right) const;
      bool operator!=(const FloatMatrix& right) const;
      FloatMatrix inverse();
      FloatMatrix reducedRowEchelon();
      FloatMatrix transpose();
      void resize(const int64_t rows, const int64_t cols, const bool destructive = false);
      static FloatMatrix zeros(const int64_t rows, const int64_t cols);
      static FloatMatrix identity(const int64_t rows);
      FloatMatrix getRange(const int64_t firstRow, const int64_t afterLastRow, const int64_t firstCol, const int64_t afterLastCol);
      FloatMatrix concatHoriz(const FloatMatrix& right);
      FloatMatrix concatVert(const FloatMatrix& bottom);
      void getDimensions(int64_t& rows, int64_t& cols) const;
      const std::vector<std::vector<float> >& getMatrix();
   };

}

#endif //__FLOAT_MATRIX_H__