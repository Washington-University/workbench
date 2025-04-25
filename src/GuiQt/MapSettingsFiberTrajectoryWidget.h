#ifndef __MAP_SETTINGS_FIBER_TRAJECTORY_WIDGET__H_
#define __MAP_SETTINGS_FIBER_TRAJECTORY_WIDGET__H_

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

#include <set>

#include <QWidget>

#include "FiberTrajectoryDisplayModeEnum.h"

class QButtonGroup;
class QComboBox;
class QDoubleSpinBox;
class QRadioButton;

namespace caret {
    class CaretMappableDataFile;
    class CiftiFiberTrajectoryFile;
    class CiftiFiberTrajectoryMapFile;

    class MapSettingsFiberTrajectoryWidget : public QWidget {
        
        Q_OBJECT

    public:
        MapSettingsFiberTrajectoryWidget(QWidget* parent = 0);
        
        virtual ~MapSettingsFiberTrajectoryWidget();
        
        void updateEditor(CiftiFiberTrajectoryFile* fiberTrajectoryFile,
                          CiftiFiberTrajectoryMapFile* fiberTrajectorMapFile);
        
        void updateWidget();
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void processAttributesChanges();
        
    private:
        MapSettingsFiberTrajectoryWidget(const MapSettingsFiberTrajectoryWidget&);

        MapSettingsFiberTrajectoryWidget& operator=(const MapSettingsFiberTrajectoryWidget&);
        
        QWidget* createAttributesWidget();
        
        QWidget* createDisplayModeWidget();
        
        QWidget* createDataMappingWidget();
        
        CiftiFiberTrajectoryFile* m_fiberTrajectoryFile;
        
        CiftiFiberTrajectoryMapFile* m_fiberTrajectorMapFile;
        
        QComboBox* m_colorSelectionComboBox;
        
        std::vector<QRadioButton*> m_displayModeRadioButtons;
        std::vector<FiberTrajectoryDisplayModeEnum::Enum> m_displayModeRadioButtonData;
        QButtonGroup* m_displayModeButtonGroup;
        
        QDoubleSpinBox* m_proportionStreamlineSpinBox;
        
        QDoubleSpinBox* m_proportionMinimumSpinBox;
        
        QDoubleSpinBox* m_proportionMaximumSpinBox;
        
        QDoubleSpinBox* m_countStreamlineSpinBox;
        
        QDoubleSpinBox* m_countMinimumSpinBox;
        
        QDoubleSpinBox* m_countMaximumSpinBox;
        
        QDoubleSpinBox* m_distanceStreamlineSpinBox;
        
        QDoubleSpinBox* m_distanceMinimumSpinBox;
        
        QDoubleSpinBox* m_distanceMaximumSpinBox;
        
        bool m_updateInProgress;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __MAP_SETTINGS_FIBER_TRAJECTORY_WIDGET_DECLARE__
#endif // __MAP_SETTINGS_FIBER_TRAJECTORY_WIDGET_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_FIBER_TRAJECTORY_WIDGET__H_
