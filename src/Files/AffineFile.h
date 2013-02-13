#ifndef __AFFINE_FILE_H__
#define __AFFINE_FILE_H__

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

#include "AString.h"
#include "FloatMatrix.h"

namespace caret {
    
    class AffineFile
    {
        FloatMatrix m_matrix;
        static FloatMatrix read34(const AString& filename);//helper to read a simple text affine
        static void write44(const FloatMatrix& out, const AString& filename);//helper for writing
        static void getFSLQuirks(const AString& niftiName, FloatMatrix& outSform, FloatMatrix& outScale);//just a convenience wrapper around the vector<vector<float> > version in NiftiHeaderIO
    public:
        AffineFile();
        void readWorld(const AString& filename);//forward nifti coordinate transform
        void writeWorld(const AString& filename);
        void readFlirt(const AString& filename, const AString& sourceName, const AString& targetName);//flirt convention matrix, requires source/target volumes
        void writeFlirt(const AString& filename, const AString& sourceName, const AString& targetName) const;
        const FloatMatrix& getMatrix() { return m_matrix; }
        void setMatrix(const FloatMatrix& matrix);//needs to do sanity checking, so don't inline
    };

}

#endif //__AFFINE_FILE_H__
