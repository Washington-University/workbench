#ifndef __CZI_NON_LINEAR_TRANSFORM_H__
#define __CZI_NON_LINEAR_TRANSFORM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"
#include "Vector3D.h"

namespace caret {
    class Matrix4x4;
    class VolumeFile;
    
    class CziNonLinearTransform : public CaretObject {
        
    public:
        enum class Mode {
            FROM_MILLIMETERS,
            TO_MILLIMETERS
        };
        
        enum class Status {
            INVALID,
            UNREAD,
            VALID
        };
        
        CziNonLinearTransform(const Mode mode,
                              const AString& filename);
        
        virtual ~CziNonLinearTransform();
        
        CziNonLinearTransform(const CziNonLinearTransform& obj) = delete;

        CziNonLinearTransform& operator=(const CziNonLinearTransform& obj) = delete;

        void getNonLinearOffset(const Vector3D& xyz,
                                Vector3D& offsetXyzOut);
        
        Status getStatus() const;
        
        AString getFilename() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperCziNonLinearTransform(const CziNonLinearTransform& obj);

        void load(const AString& filename);
        
        void getNonLinearOffsetToMillimeters(const Vector3D& planeXYZ,
                                             Vector3D& offsetXyzOut);

        void getNonLinearOffsetFromMillimeters(const Vector3D& stereotaxicXYZ,
                                               Vector3D& offsetXyzOut);
        

        const Mode m_mode;
        
        const AString m_filename;
        
        mutable std::unique_ptr<VolumeFile> m_niftiFile;
        
        mutable std::unique_ptr<Matrix4x4> m_sformMatrix;
        
        mutable std::unique_ptr<Matrix4x4> m_inverseSformMatrix;
        
        Status m_status = Status::UNREAD;
                
        /** Volume is oriented left-to-right for first axis (same as CZI image) */
        bool m_xLeftToRightFlag = false;
        
        /** Volume is oriented top-to-bottom for second axis (same as CZI image) */
        bool m_yTopToBottomFlag = false;
        
        int64_t m_dimensionX = 0.0;
        
        int64_t m_dimensionY = 0.0;
        
        int64_t m_numberOfMaps = 0;
        
        static bool s_debugFlag;
        
        ///static bool
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_NON_LINEAR_TRANSFORM_DECLARE__
    bool CziNonLinearTransform::s_debugFlag = false;
#endif // __CZI_NON_LINEAR_TRANSFORM_DECLARE__

} // namespace
#endif  //__CZI_NON_LINEAR_TRANSFORM_H__
