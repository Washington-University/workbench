#ifndef __IDENTIFY_BRAINORDINATE_DIALOG_H__
#define __IDENTIFY_BRAINORDINATE_DIALOG_H__

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

#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QLabel;
class QRadioButton;
class QSpinBox;
class QStackedWidget;

namespace caret {

    class BrainordinateRegionOfInterest;
    class CaretDataFileSelectionComboBox;
    class CaretDataFileSelectionModel;
    class CaretMappableDataFileAndMapSelectorObject;
    class CaretMappableDataFile;
    class CiftiParcelSelectionComboBox;
    class GiftiLabelTableSelectionComboBox;
    class StructureEnumComboBox;
    
    class IdentifyBrainordinateDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        IdentifyBrainordinateDialog(QWidget* parent);
        
        virtual ~IdentifyBrainordinateDialog();
        
        virtual void updateDialog();
        
        virtual void receiveEvent(Event* event);
        
    protected:
        virtual void applyButtonClicked();
        
    private:
        IdentifyBrainordinateDialog(const IdentifyBrainordinateDialog&);

        IdentifyBrainordinateDialog& operator=(const IdentifyBrainordinateDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private slots:
        void slotParcelFileOrMapSelectionChanged();
        
        void slotLabelFileOrMapSelectionChanged();
        
        void idTypeRadioButtonClicked(QAbstractButton* button);
        
    private:
        enum Mode {
            MODE_NONE,
            MODE_CIFTI_PARCEL,
            MODE_CIFTI_ROW,
            MODE_SURFACE_VERTEX
        };
        
        enum ParcelSourceDimension {
            PARCEL_SOURCE_INVALID_DIMENSION,
            PARCEL_SOURCE_LOADING_DIMENSION,
            PARCEL_SOURCE_MAPPING_DIMENSION
        };
        
        // ADD_NEW_MEMBERS_HERE
        
        QWidget* createCiftiParcelWidget();
        
        QWidget* createCiftiRowWidget(const std::vector<DataFileTypeEnum::Enum>& supportedFileTypes);
        
        QWidget* createLabelFilesWidget(const std::vector<DataFileTypeEnum::Enum>& supportedFileTypes);
        
        QWidget* createSurfaceVertexWidget();
        
        QWidget* createImagePixelWidget(const std::vector<DataFileTypeEnum::Enum>& supportedFileTypes);
        
        void processCiftiParcelWidget(AString& errorMessageOut);
        
        void processCiftiRowWidget(AString& errorMessageOut);
        
        void processImagePixelSelection(AString& errorMessage);
        
        void processLabelFileWidget(AString& errorMessageOut);
        
        void processSurfaceVertexWidget(AString& errorMessageOut);
        
        void flashBrainordinateHighlightingRegionOfInterest(BrainordinateRegionOfInterest* brainROI);
        
        void updateColoringAndDrawAllWindows(const bool doRepaintFlag = false);

        ParcelSourceDimension getParcelSourceDimensionFromFile(const CaretMappableDataFile* mapFile);
        
        StructureEnumComboBox* m_vertexStructureComboBox;
        
        QWidget* m_surfaceVertexWidget;
        
        QLabel* m_vertexStructureLabel;
        
        QSpinBox* m_vertexIndexSpinBox;
        
        QLabel* m_vertexIndexLabel;
        
        QWidget* m_ciftiRowWidget;
        
        struct LabelFileWidgets {
            QWidget* m_widget;
            
            CaretMappableDataFileAndMapSelectorObject* m_fileSelector;
            
            QLabel* m_fileLabel;
            
            QLabel* m_fileMapLabel;
            
            QWidget* m_fileComboBox;
            
            QWidget* m_fileMapSpinBox;
            
            QWidget* m_fileMapComboBox;
            
            QLabel* m_fileLabellLabel;
            
            GiftiLabelTableSelectionComboBox* m_fileLabelComboBox;
        };
        
        LabelFileWidgets m_labelFileWidgets;
        
        
        
        QLabel* m_ciftiRowFileLabel;
        
        CaretDataFileSelectionComboBox* m_ciftiRowFileComboBox;

        CaretDataFileSelectionModel* m_ciftiRowFileSelectionModel;
        
        QLabel* m_ciftiRowFileIndexLabel;
        
        QSpinBox* m_ciftiRowFileIndexSpinBox;
        
        QWidget* m_ciftiParcelWidget;
        
        QLabel* m_ciftiParcelFileLabel;
        
        QLabel* m_ciftiParcelFileMapLabel;
        
        QWidget* m_ciftiParcelFileComboBox;
        
        QWidget* m_ciftiParcelFileMapSpinBox;
        
        QWidget* m_ciftiParcelFileMapComboBox;
        
        QLabel* m_ciftiParcelFileParcelLabel;
        
        CiftiParcelSelectionComboBox* m_ciftiParcelFileParcelNameComboBox;
        
        CaretMappableDataFileAndMapSelectorObject* m_ciftiParcelFileSelector;
        
        QWidget* m_imagePixelWidget;
        
        CaretDataFileSelectionModel* m_imageFileSelectionModel;
        
        CaretDataFileSelectionComboBox* m_imageFileSelectionComboBox;
        
        QSpinBox* m_imagePixelISpinBox;
        
        QSpinBox* m_imagePixelJSpinBox;
        
        QStackedWidget* m_stackedWidget;
        
        QRadioButton* m_ciftiFileRowRadioButton;
        QRadioButton* m_ciftiFileParcelRadioButton;
        QRadioButton* m_imagePixelRadioButton;
        QRadioButton* m_labelRadioButton;
        QRadioButton* m_surfaceVertexRadioButton;
        
        std::map<DataFileTypeEnum::Enum, ParcelSourceDimension> m_parcelSourceDimensionMap;
        std::vector<DataFileTypeEnum::Enum> m_supportedCiftiRowFileTypes;
        std::vector<DataFileTypeEnum::Enum> m_supportedLabelFileTypes;
    };
    
#ifdef __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__
#endif // __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__

} // namespace
#endif  //__IDENTIFY_BRAINORDINATE_DIALOG_H__
