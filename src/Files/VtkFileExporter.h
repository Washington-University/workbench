#ifndef __VTK_FILE_EXPORTER_H__
#define __VTK_FILE_EXPORTER_H__

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
#include <vector>

namespace caret {
    class SurfaceFile;
    
    class VtkFileExporter {
        
    public:
        static void writeSurfaces(const std::vector<SurfaceFile*>& surfaceFiles,
                                  const std::vector<const float*>& surfaceFilesColoring,
                                  const AString& vtkFileName);
        
    private:
        VtkFileExporter();
        
        virtual ~VtkFileExporter();
        
        VtkFileExporter(const VtkFileExporter&);

        VtkFileExporter& operator=(const VtkFileExporter&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VTK_FILE_EXPORTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VTK_FILE_EXPORTER_DECLARE__

} // namespace
#endif  //__VTK_FILE_EXPORTER_H__
