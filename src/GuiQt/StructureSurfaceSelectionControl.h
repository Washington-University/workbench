#ifndef __STRUCTURE_SURFACE_SELECTION_CONTROL__H_
#define __STRUCTURE_SURFACE_SELECTION_CONTROL__H_

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
        StructureSurfaceSelectionControl(const bool showLabels);
        
        virtual ~StructureSurfaceSelectionControl();

        ModelSurface* getSelectedSurfaceController();
        
        StructureEnum::Enum getSelectedStructure();
        
        //void setSelectedSurfaceController(ModelSurface* surfaceController);
        
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
