#ifndef __BORDER_OPTIMIZE_DIALOG_H__
#define __BORDER_OPTIMIZE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "WuQDialogModal.h"

class QCheckBox;
class QDoubleSpinBox;


namespace caret {

    class Border;
    class CaretMappableDataFile;
    class Surface;
    
    class BorderOptimizeDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        BorderOptimizeDialog(QWidget* parent,
                             const Surface* surface,
                             const std::vector<Border*>& bordersInsideROI,
                             const std::vector<int32_t>& nodesInsideROI,
                             const std::vector<CaretMappableDataFile*>& optimizeDataFiles);
        
        virtual ~BorderOptimizeDialog();
        

        // ADD_NEW_METHODS_HERE

    protected:
        void okButtonClicked();
        
    private:
        BorderOptimizeDialog(const BorderOptimizeDialog&);

        BorderOptimizeDialog& operator=(const BorderOptimizeDialog&);
        
        bool run(std::vector<Border*>& borders,
                 std::vector<CaretMappableDataFile*>& dataFiles,
                 const float smoothingLevel,
                 const bool invertGradientFlag,
                 AString& errorMessageOut);
        
        QWidget* createBorderSelectionWidget();
        
        QWidget* createDataFilesWidget();
        
        QWidget* createOptionsWidget();
        
        const Surface* m_surface;
        
        const std::vector<Border*> m_bordersInsideROI;
        
        const std::vector<int32_t> m_nodesInsideROI;
        
        const std::vector<CaretMappableDataFile*> m_optimizeDataFiles;
        
        std::vector<QCheckBox*> m_borderCheckBoxes;
        
        std::vector<QCheckBox*> m_optimizeDataFileCheckBoxes;
        
        QCheckBox* m_invertedGradientCheckBox;
        
        QDoubleSpinBox* m_smoothingLevelSpinBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BORDER_OPTIMIZE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BORDER_OPTIMIZE_DIALOG_DECLARE__

} // namespace
#endif  //__BORDER_OPTIMIZE_DIALOG_H__
