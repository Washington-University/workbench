#ifndef __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG__H_
#define __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <map>
#include <set>

#include "CaretMappableDataFileAndMapSelector.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QButtonGroup;
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
        virtual void okButtonPressed();
        
    private:
        RegionOfInterestCreateFromBorderDialog(const RegionOfInterestCreateFromBorderDialog&);

        RegionOfInterestCreateFromBorderDialog& operator=(const RegionOfInterestCreateFromBorderDialog&);
        
        void createDialog(const std::vector<Border*>& borders,
                          std::vector<Surface*>& surfaces);
        
        QWidget* createMetricValueWidget();
        
        QWidget* createLabelValueWidget();
        
        typedef std::map<StructureEnum::Enum,
                         CaretMappableDataFileAndMapSelector*> STRUCTURE_MAP_FILE_SELECTOR_MAP;
        
        typedef STRUCTURE_MAP_FILE_SELECTOR_MAP::iterator STRUCTURE_MAP_FILE_SELECTOR_ITERATOR;
        
        QWidget* createSelectors(std::set<StructureEnum::Enum>& requiredStructures,
                                 std::vector<Surface*>& surfaces,
                                 STRUCTURE_MAP_FILE_SELECTOR_MAP& mapFileSelectors); 
        
        STRUCTURE_MAP_FILE_SELECTOR_MAP mapFileTypeSelectors;
        
        std::vector<Surface*> surfaces;
        
        std::vector<Border*> borders;
        
        QDoubleSpinBox* metricValueSpinBox;
        
        QLineEdit* labelNameLineEdit;
        
        QStackedWidget* valueEntryStackedWidget;
        
        QWidget* valueWidgetMetric;
        
        QWidget* valueWidgetLabel;
        
    };
    
#ifdef __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__

} // namespace
#endif  //__REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG__H_
