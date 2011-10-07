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
#include "NiftiMatrix.h"

using namespace caret;

NiftiMatrix::NiftiMatrix()
{
}

NiftiMatrix::NiftiMatrix(const AString filename) throw (NiftiException)
{
    if(!QFile.exists(filename))

}

NiftiMatrix::NiftiMatrix(Astring filename, int64_t offset)
{


}

NiftiMatrix::NiftiMatrix(QFile file)
{


}

NiftiMatrix::NiftiMatrix(QFile file, int64_t offset)
{


}

void NiftiMatrix::getDataType(NiftiDataTypeEnum &type)
{


}

void NiftiMatrix::setDataType(NiftiDataTypeEnum type)
{


}

void NiftiMatrix::getVolumeFrame(int64_t time, double &frame)
{


}

void NiftiMatrix::setVolumeFrame(int64_t time, const double &frame)
{


}

void NiftiMatrix::getVolumeFrame(int64_t time, float &frame)
{


}

void NiftiMatrix::setVolumeFrame(int64_t time, const float &frame)
{


}
#endif
