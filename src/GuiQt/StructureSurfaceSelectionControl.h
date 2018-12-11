#ifndef __STRUCTURE_SURFACE_SELECTION_CONTROL__H_
#define __STRUCTURE_SURFACE_SELECTION_CONTROL__H_

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


#include <QWidget>

#include "StructureEnum.h"

class QComboBox;

namespace caret {
    
    class ModelSurfaceSelector;
    class ModelSurface;
    class Structure;
    
    class StructureSurfaceSelectionControl : public QWidget {
        
        Q_OBJECT

    public:
        StructureSurfaceSelectionControl(const bool showLabels,
                                         const QString& objectNamePrefix,
                                         QWidget* parent);
        
        virtual ~StructureSurfaceSelectionControl();

        ModelSurface* getSelectedSurfaceModel();
        
        StructureEnum::Enum getSelectedStructure();
        
        //void setSelectedSurfaceModel(ModelSurface* surfaceController);
        
        //void setSelectedStructure(const StructureEnum::Enum selectedStructure);
        
        void updateControl(ModelSurfaceSelector* surfaceControllerSelector);
        
    signals:
        void selectionChanged(const StructureEnum::Enum selectedStructure,
                              ModelSurface* surfaceController);
        
    private slots:
        void structureSelected(int currentIndex);
        
        void surfaceControllerSelected(int currentIndex);
        
    private:
        void updateControlAfterSelection();
        
        void emitSelectionChangedSignal();
        
        StructureSurfaceSelectionControl(const StructureSurfaceSelectionControl&);

        StructureSurfaceSelectionControl& operator=(const StructureSurfaceSelectionControl&);
        
        QComboBox* structureSelectionComboBox;
        
        QComboBox* surfaceControllerSelectionComboBox;

        ModelSurfaceSelector* surfaceControllerSelector;
    public:
    private:
    };
    
#ifdef __STRUCTURE_SURFACE_SELECTION_CONTROL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __STRUCTURE_SURFACE_SELECTION_CONTROL_DECLARE__

} // namespace
#endif  //__STRUCTURE_SURFACE_SELECTION_CONTROL__H_
