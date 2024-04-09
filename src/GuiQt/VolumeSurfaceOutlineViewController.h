#ifndef __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER__H_
#define __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER__H_

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

#include <QObject>

#include "VolumeSurfaceOutlineColorOrTabModel.h"
class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;

namespace caret {

    class EnumComboBoxTemplate;
    class Surface;
    class SurfaceSelectionViewController;
    class VolumeSurfaceOutlineModel;
    class VolumeSurfaceOutlineColorOrTabViewController;
    class WuQGridLayoutGroup;
    
    class VolumeSurfaceOutlineViewController : public QObject {
        
        Q_OBJECT

    public:
        VolumeSurfaceOutlineViewController(const Qt::Orientation orientation,
                                           QGridLayout* gridLayout,
                                           const QString& objectNamePrefix,
                                           const QString& descriptivePrefix,
                                           QObject* parent = 0);
        
        virtual ~VolumeSurfaceOutlineViewController();
        
        void setVisible(bool visible);
        
        void updateViewController(VolumeSurfaceOutlineModel* outlineModel);
        
    private slots:
        void enabledCheckBoxChecked(bool checked);
        
        void thicknessSpinBoxValueChanged(double);

        void slicePlaneDepthSpinBoxValueChanged(double);
        
        void surfaceSelected(Surface*);
        
        void colorTabSelected(VolumeSurfaceOutlineColorOrTabModel::Item*);
        
        void volumeSurfaceOutlineDrawingModeEnumComboBoxItemActivated();
        
    private:
        VolumeSurfaceOutlineViewController(const VolumeSurfaceOutlineViewController&);

        VolumeSurfaceOutlineViewController& operator=(const VolumeSurfaceOutlineViewController&);
        
        void updateGraphics();
        
        VolumeSurfaceOutlineModel* outlineModel;
        
        WuQGridLayoutGroup* gridLayoutGroup;
        
        QCheckBox* enabledCheckBox;
        
        VolumeSurfaceOutlineColorOrTabViewController* colorOrTabSelectionControl;
        
        QDoubleSpinBox* thicknessSpinBox;
        
        QDoubleSpinBox* slicePlaneDepthSpinBox;
        
        SurfaceSelectionViewController* surfaceSelectionViewController;
        
        EnumComboBoxTemplate* m_volumeSurfaceOutlineDrawingModeEnumComboBox;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_VIEW_CONTROLLER__H_
