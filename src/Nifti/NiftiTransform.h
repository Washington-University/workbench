#ifndef __NIFTITRANSFORM_H__
#define __NIFTITRANSFORM_H__

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



#include <stdint.h>

#include <vector>
#include <string>

namespace caret {

/**
 * NIFTI Transform.
 */
class NiftiTransform {

public:
    /**  NIFTI Transform.
 */
    enum Enum {
        /** Arbitrary Coordinates  */
        NIFTI_XFORM_UNKNOWN,
        /** Scanner-base anatomical coordinates  */
        NIFTI_XFORM_SCANNER_ANAT,
        /** Coordinates aligned to another file's or anatomial "truth"  */
        NIFTI_XFORM_ALIGNED_ANAT,
        /** Coordinates aligned to Talairach-Tournoux Atlas: (0,0,0) = Anterior Commissure  */
        NIFTI_XFORM_TALAIRACH,
        /** MNI 152 Normalize Coordinates  */
        NIFTI_XFORM_MNI_152
    };


    ~NiftiTransform();

    static std::string toName(Enum e);
    
    static Enum fromName(const std::string& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

private:
    NiftiTransform(const Enum e, const int32_t integerCode, const std::string& name);

    static std::vector<NiftiTransform> enumData;

    static const NiftiTransform* findData(const Enum e);
    
    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    std::string name;
};

#ifdef __NIFTITRANSFORM_DECLARE__
std::vector<NiftiTransform> enumData;
bool NiftiTransform::initializedFlag = false;
#endif // __NIFTITRANSFORM_DECLARE__

} // namespace

#endif // __NIFTITRANSFORM_H__
