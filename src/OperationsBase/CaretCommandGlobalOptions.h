#ifndef __CARET_COMMAND_GLOBAL_OPTIONS_H__
#define __CARET_COMMAND_GLOBAL_OPTIONS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020  Washington University School of Medicine
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

#include "nifti1.h"

#include <cstdint>

namespace caret {

    struct CommandGlobalOptions
    {
        bool m_ciftiReadMemory = false;
        bool m_disableProvenance = false;
        int16_t m_volumeDType = NIFTI_TYPE_FLOAT32;
        int16_t m_ciftiDType = NIFTI_TYPE_FLOAT32;
        bool m_volumeScale = false, m_ciftiScale = false;
        float m_volumeMin = -1.0f, m_volumeMax = -1.0f;//these values won't get used, but don't leave them uninitialized
        float m_ciftiMin = -1.0f, m_ciftiMax = -1.0f;
    };

    extern CommandGlobalOptions caret_global_command_options;

}

#endif //__CARET_COMMAND_GLOBAL_OPTIONS_H__
