#ifndef __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG__H_
#define __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG__H_

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

#include <map>
#include <set>

#include "CaretMappableDataFileAndMapSelector.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QButtonGroup;
class QCheckBox;
class QDoubleSpinBox;
class QLineEdit;
class QStackedWidget;

namespace caret {

    class Border;
    class Surface;
    
    class RegionOfInterestCreateFromBorderDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        RegionOfInterestCreateFromBorderDialog(Border* border,
                                               Surface* surface,
                                               QWidget* parent);
        
        virtual ~RegionOfInterestCreateFromBorderDialog();
        
    private slots:
        void fileSelectionWasChanged(CaretMappableDataFileAndMapSelector*);
        
    protected:
        virtual void okButtonClicked();
        
    private:
        RegionOfInterestCreateFromBorderDialog(const RegionOfInterestCreateFromBorderDialog&);

        RegionOfInterestCreateFromBorderDialog& operator=(const RegionOfInterestCreateFromBorderDialog&);
        
        void createDialog(const std::vector<Border*>& borders,
                          std::vector<Surface*>& surfaces);
        
        typedef std::map<StructureEnum::Enum,
                         CaretMappableDataFileAndMapSelector*> STRUCTURE_MAP_FILE_SELECTOR_MAP;
        
        typedef STRUCTURE_MAP_FILE_SELECTOR_MAP::iterator STRUCTURE_MAP_FILE_SELECTOR_ITERATOR;
        
        QWidget* createSelectors(std::set<StructureEnum::Enum>& requiredStructures,
                                 std::vector<Surface*>& surfaces,
                                 STRUCTURE_MAP_FILE_SELECTOR_MAP& mapFileSelectors); 
        
        STRUCTURE_MAP_FILE_SELECTOR_MAP mapFileTypeSelectors;
        
        std::vector<Surface*> surfaces;
        
        std::vector<Border*> borders;
        
        QCheckBox* inverseCheckBox;
        
    };
    
#ifdef __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__

} // namespace
#endif  //__REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG__H_
