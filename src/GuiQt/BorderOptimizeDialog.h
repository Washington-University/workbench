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

#include <map>

#include "BorderOptimizeExecutor.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QCheckBox;
class QDoubleSpinBox;
class QGridLayout;
class QGroupBox;
class QLineEdit;
class QSpinBox;
class QVBoxLayout;

namespace caret {

    class Border;
    class BorderOptimizeDataFileSelector;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class CaretMappableDataFile;
    class CaretMappableDataFileAndMapSelectorObject;
    class Surface;
    class SurfaceSelectionModel;
    class SurfaceSelectionViewController;
    
    class BorderOptimizeDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        BorderOptimizeDialog(QWidget* parent);
        
        virtual ~BorderOptimizeDialog();
        
        void getModifiedBorders(std::vector<Border*>& modifiedBordersOut) const;

        bool isKeepBoundaryBorderSelected() const;
        
        void updateDialog(const int32_t browserTabIndex,
                          Surface* surface,
                          std::vector<std::pair<int32_t, Border*> >& bordersInsideROI,
                          Border* borderEnclosingROI,
                          std::vector<int32_t>& nodesInsideROI);
        
    protected:
        virtual void okButtonClicked();
        
        virtual void cancelButtonClicked();
        
    private slots:
        void addDataFileRowToolButtonClicked();
        
        void gradientComputatonSurfaceSelected(Surface* surface);
        
        void bordersDisableAllSelected();
        
        void bordersEnableAllSelected();
        
        void dataFilesDisableAllSelected();
        
        void dataFilesEnableAllSelected();
        
        void saveBrowseButtonClicked();
        
    private:
        BorderOptimizeDialog(const BorderOptimizeDialog&);

        BorderOptimizeDialog& operator=(const BorderOptimizeDialog&);
        
        QWidget* createBorderSelectionWidget();
        
        QWidget* createDataFilesWidget();
        
        QWidget* createVertexAreasMetricWidget();
        
        QWidget* createOptionsWidget();
        
        QWidget* createSurfaceSelectionWidget();
        
        QWidget* createSphericalUpsamplingWidget();
        
        QWidget* createSavingWidget();
        
        void addDataFileRow(CaretMappableDataFile* mapFile);
        
        void setAllBorderEnabledSelections(const bool status);
        
        void setAllDataFileEnabledSelections(const bool status);
        
        void preserveDialogSizeAndPositionWhenReOpened();
        
        QString m_objectNamePrefix;
        
        QWidget* m_dialogWidget;
        
        Surface* m_surface;
        
        std::vector<Border*> m_bordersInsideROI;
        std::vector<int32_t> m_borderPointsInsideROICount;
        
        std::vector<Border*> m_selectedBorders;
        
        QGridLayout* m_bordersInsideROIGridLayout;
        
        Border* m_borderEnclosingROI;
        
        std::vector<int32_t> m_nodesInsideROI;
        
        std::vector<BorderOptimizeDataFileSelector*> m_optimizeDataFileSelectors;
        
        CaretDataFileSelectionComboBox* m_borderPairFileSelectionComboBox;
        CaretDataFileSelectionModel* m_borderPairFileSelectionModel;
        
        QCheckBox* m_borderPairCheckBox;
        
        std::vector<QCheckBox*> m_borderCheckBoxes;
        
        std::vector<DataFileTypeEnum::Enum> m_optimizeDataFileTypes;
        
        QGridLayout* m_borderOptimizeDataFileGridLayout;
        
        StructureEnum::Enum m_surfaceSelectionStructure;
        
        SurfaceSelectionModel* m_surfaceSelectionModel;
        
        SurfaceSelectionViewController* m_surfaceSelectionControl;
        
        CaretDataFileSelectionComboBox* m_vertexAreasMetricFileComboBox;
        
        CaretDataFileSelectionModel* m_vertexAreasMetricFileSelectionModel;
        
        QDoubleSpinBox* m_gradientFollowingStrengthSpinBox;
        
        int32_t m_browserTabIndex;
        
        QCheckBox* m_keepRegionBorderCheckBox;
        
        QCheckBox* m_outputGradientMapCheckBox;
        
        QSize m_defaultDataFilesWidgetSize;
        
        QGroupBox* m_upsamplingGroupBox;
        
        SurfaceSelectionModel* m_upsamplingSurfaceSelectionModel;
        
        StructureEnum::Enum m_upsamplingSurfaceStructure;
        
        SurfaceSelectionViewController* m_upsamplingSurfaceSelectionControl;
        
        QSpinBox* m_upsamplingResolutionSpinBox;
        
        QGroupBox* m_savingGroupBox;
        
        QLineEdit* m_savingBaseNameLineEdit;
        
        QLineEdit* m_savingDirectoryLineEdit;
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
        
        void setSelected(const bool selectedStatus);
        
    private slots:
        void selectionCheckBoxToggled(bool checked);
        
        void allMapsCheckBoxToggled(bool checked);

        void mapFileSelectionChanged();
        
    public:
        CaretMappableDataFileAndMapSelectorObject* m_mapFileAndIndexSelectorObject;
        
        QCheckBox*      m_allMapsCheckBox;
        QDoubleSpinBox* m_exclusionDistanceSpinBox;
        QCheckBox*      m_invertGradientCheckBox;
        QCheckBox*      m_skipGradientCheckBox;
        QCheckBox*      m_selectionCheckBox;
        QDoubleSpinBox* m_smoothingSpinBox;
        QDoubleSpinBox* m_weightSpinBox;
    };

#ifdef __BORDER_OPTIMIZE_DIALOG_DECLARE__
#endif // __BORDER_OPTIMIZE_DIALOG_DECLARE__

} // namespace
#endif  //__BORDER_OPTIMIZE_DIALOG_H__
