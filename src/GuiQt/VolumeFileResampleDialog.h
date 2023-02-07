#ifndef __VOLUME_FILE_RESAMPLE_DIALOG_H__
#define __VOLUME_FILE_RESAMPLE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include "Vector3D.h"
#include "WuQDialogModal.h"

class QDoubleSpinBox;
class QLabel;

namespace caret {

    class VolumeFileResampleDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        VolumeFileResampleDialog(const Vector3D& dimensions,
                                 const Vector3D& origin,
                                 const Vector3D& spacing,
                                 const float spacingSingleStep,
                                 const int32_t spacingDecimals,
                                 QWidget* parent = 0);
        
        virtual ~VolumeFileResampleDialog();
        
        VolumeFileResampleDialog(const VolumeFileResampleDialog&) = delete;

        VolumeFileResampleDialog& operator=(const VolumeFileResampleDialog&) = delete;

        Vector3D getDimensions() const;
        
        Vector3D getOrigin() const;
        
        Vector3D getSpacing() const;

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private slots:
        void updateVoxelEdgeLabels();

    private:
        const Vector3D m_inputDimensions;
        
        const Vector3D m_inputOrigin;
        
        const Vector3D m_inputSpacing;
        
        Vector3D m_newDimensions;
        
        Vector3D m_newOrigin;
        
        Vector3D m_newSpacing;
        
        QDoubleSpinBox* m_spacingXSpinBox;
        
        QDoubleSpinBox* m_spacingYSpinBox;
        
        QDoubleSpinBox* m_spacingZSpinBox;
        
        QLabel* m_newDimensionXLabel;
        
        QLabel* m_newDimensionYLabel;
        
        QLabel* m_newDimensionZLabel;
        
        QLabel* m_newOriginXLabel;
        
        QLabel* m_newOriginYLabel;
        
        QLabel* m_newOriginZLabel;
        
        QLabel* m_xFirstVoxelEdgeLabel;
        
        QLabel* m_xLastVoxelEdgeLabel;
        
        QLabel* m_yFirstVoxelEdgeLabel;
        
        QLabel* m_yLastVoxelEdgeLabel;
        
        QLabel* m_zFirstVoxelEdgeLabel;
        
        QLabel* m_zLastVoxelEdgeLabel;
        
        bool m_blockVoxelEdgeLabelUpdateFlag = true;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_FILE_RESAMPLE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_FILE_RESAMPLE_DIALOG_DECLARE__

} // namespace
#endif  //__VOLUME_FILE_RESAMPLE_DIALOG_H__
