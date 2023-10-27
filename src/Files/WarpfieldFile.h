#ifndef __WARPFIELD_FILE_H__
#define __WARPFIELD_FILE_H__

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

#include "AString.h"
#include "CaretPointer.h"
#include "VolumeFile.h"

namespace caret {
    
    class WarpfieldFile
    {
        CaretPointer<VolumeFile> m_warpfield;
    public:
        const VolumeFile* getWarpfield() const { return m_warpfield.getPointer(); }
        CaretPointer<VolumeFile> generateAbsolute() const;
        void readWorld(const AString& warpname, const bool absolute = false);
        void readITK(const AString& warpname);
        void readFnirt(const AString& warpName, const AString& sourceName, const bool& absolute = false);
        void writeWorld(const AString& warpname) const;//for completeness
        void writeITK(const AString& warpname) const;
        void writeFnirt(const AString& warpname, const AString& sourceName) const;
    };
    
}

#endif //__WARPFIELD_FILE_H__
