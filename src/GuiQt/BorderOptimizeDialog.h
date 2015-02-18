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

#include "BorderOptimizeExecutor.h"
#include "DataFileTypeEnum.h"
#include "WuQDialogModal.h"

class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QVBoxLayout;

namespace caret {

    class Border;
    class BorderOptimizeDataFileSelector;
    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectorObject;
    class Surface;
    
    class BorderOptimizeDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        BorderOptimizeDialog(QWidget* parent);
        
        virtual ~BorderOptimizeDialog();
        
        void getSelectedBorders(std::vector<Border*>& selectedBordersOut) const;

        void updateDialog(Surface* surface,
                          std::vector<Border*>& bordersInsideROI,
                          Border* borderEnclosingROI,
                          std::vector<int32_t>& nodesInsideROI);
        
    protected:
        virtual void okButtonClicked();
        
        virtual void cancelButtonClicked();
        
    private:
        BorderOptimizeDialog(const BorderOptimizeDialog&);

        BorderOptimizeDialog& operator=(const BorderOptimizeDialog&);
        
        QWidget* createBorderSelectionWidget();
        
        QWidget* createDataFilesWidget();
        
        QWidget* createOptionsWidget();
        
        Surface* m_surface;
        
        std::vector<Border*> m_bordersInsideROI;
        
        QVBoxLayout* m_bordersInsideROILayout;
        
        Border* m_borderEnclosingROI;
        
        std::vector<int32_t> m_nodesInsideROI;
        
        std::vector<BorderOptimizeDataFileSelector*> m_optimizeDataFileSelectors;
        
        std::vector<QCheckBox*> m_borderCheckBoxes;
        
        std::vector<DataFileTypeEnum::Enum> m_optimizeDataFileTypes;

    };
    
    class BorderOptimizeDataFileSelector : public QObject {
        Q_OBJECT
        
    public:
        BorderOptimizeDataFileSelector(const int32_t itemIndex,
                                       const std::vector<DataFileTypeEnum::Enum>& optimizeDataFileTypes,
                                       CaretMappableDataFile* defaultFile,
                                       QGridLayout* gridLayout,
                                       QObject* parent);
        
        ~BorderOptimizeDataFileSelector();
        
        void updateFileData();
    
        CaretPointer<BorderOptimizeExecutor::DataFileInfo> getSelections() const;
        
    public slots:
        void selectionCheckBoxToggled(bool checked);
        
        void allMapsCheckBoxToggled(bool checked);
        
    public:
        CaretMappableDataFileAndMapSelectorObject* m_mapFileAndIndexSelectorObject;
        
        QCheckBox*      m_allMapsCheckBox;
        QCheckBox*      m_invertGradientCheckBox;
        QCheckBox*      m_selectionCheckBox;
        QDoubleSpinBox* m_smoothingSpinBox;
        QDoubleSpinBox* m_weightSpinBox;
    };

#ifdef __BORDER_OPTIMIZE_DIALOG_DECLARE__
#endif // __BORDER_OPTIMIZE_DIALOG_DECLARE__

} // namespace
#endif  //__BORDER_OPTIMIZE_DIALOG_H__
