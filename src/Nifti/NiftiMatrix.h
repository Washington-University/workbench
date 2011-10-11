/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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

#if 0
#ifndef NIFTIMATRIX_H
#define NIFTIMATRIX_H
#include "NiftiEnums.h"
#include "NiftiException.h"
#include "QFile"
#include "AString.h"
#include "VolumeFile.h"
namespace caret {

class NiftiMatrix
{
public:
    NiftiMatrix();
    NiftiMatrix(const QFile &filein) throw (NiftiException);
    NiftiMatrix(const AString &filename) throw (NiftiException);
    NiftiMatrix(const Astring &filename, int64_t offsetin) throw (NiftiException);
    NiftiMatrix(QFile &filein, const int64_t &offsetin) throw (NiftiException);
    ~NiftiMatrix();

    void init();


    void setSwapNeeded(bool &swapNeededIn) { swapNeeded = swapNeededIn; }
    void getSwapNeeed(bool &swapNeededOut) const { swapNeededOut = swapNeeded; }

    void setMatrixOffset(const int64_t &offsetin) throw (NiftiException);
    void setDataType(const NiftiDataTypeEnum &typein) throw (NiftiException);
    void getDataType(NiftiDataTypeEnum &typeout) const throw (NiftiException);

    void getVolumeFrame(VolumeFile &volume, int64_t &timeslice) throw (NiftiException);
    void setVolumeFrame(const VolumeFile &volume, int64_t &timeslice) throw (NiftiException);

    //will remove this once it becomes more clear what subset of nifti header info is required
    void setNiftiHeader(Nifti1Header &headerin) throw (NiftiException)
    {
        n1Header = headerin;
        niftiVersion = 1;
    }

    void setNiftiHeader(Nifti2Header &headerin) throw (NiftiException)
    {
        n2Header = headerin;
        niftiVersion = 2;
    }

    void LoadFrame(int64_t timeSlice);//for loading a frame at a time
    void LoadMatrix();//for loading the entire matrix
private:
    void translateVoxel(const int64_t &i, const int64_t &j, const int64_t &k, const int64_t &time, const int64_t &frame_size, int64_t &index) const throw (NiftiException);
    float getComponent(const int64_t &index, const int32_t &component_index) const throw (NiftiException);
    void setComponent(const int64_t &index, const int32_t &component_index, const float &value) throw (NiftiException);

#if 0
    array index, byte swap, cast, then data scaling

    0000 1000 0100 1100 0200 1200 0010 1010 0110 1110 0210 1210 0001

    index = 0;
    for t = 0:dim4
            for k = 0:dim3
                    for j = 0:dim2
                            for i = 0:dim1
                                    translateVoxel(i, j, k, t, frame, index);
                                    ++index;
                            end
                    end
            end
    end

    void translateVoxel(i, j, k, t, frame, index)
            for c = 0:components
                    float temp = getComponent(index, component);
                    myVolume.setValue(temp, i, j, k, t, c);
            end
    end
#endif

private:
    QFile file;
    int64_t matrixStartOffset;
    NiftiDataTypeEnum niftiDataType;
    std::vector <int> dimensions;
    Nifti1Header n1Header;
    Nifti2Header n2Header;
    int niftiVersion;
    bool swapNeeded;

    // new approach
    int64_t currentTime;
    float_t *matrix;
    int64_t matrixSize;
};

}
#endif // NIFTIMATRIX_H

#endif
