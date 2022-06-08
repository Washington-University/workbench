#ifndef __IDENTIFICATION_DISPLAY_WIDGET_H__
#define __IDENTIFICATION_DISPLAY_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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



#include <memory>

#include <QWidget>

#include "EventListenerInterface.h"
#include "SceneableInterface.h"

class QAbstractButton;
class QButtonGroup;
class QCheckBox;
class QDoubleSpinBox;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QTextBrowser;

namespace caret {
    class CaretColorEnumComboBox;
    class EnumComboBoxTemplate;
    class IdentificationFileFilteringTableWidget;
    class IdentificationHistoryManager;
    class SceneClassAssistant;

    class IdentificationDisplayWidget : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        enum class Location {
            Dialog,
            HorizontalToolBox,
            VerticalToolBox
        };
        IdentificationDisplayWidget(const Location location,
                                    QWidget* parent = 0);
        
        virtual ~IdentificationDisplayWidget();
        
        IdentificationDisplayWidget(const IdentificationDisplayWidget&) = delete;

        IdentificationDisplayWidget& operator=(const IdentificationDisplayWidget&) = delete;
        
        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private slots:
        void filteringChanged();
        
        void symbolChanged();
        
        void infoShowHistoryCountSpinBoxValueChanged(int value);
        
        void infoClearHistoryToolButtonClicked();
        
        void infoRemoveSymbolsButtonClicked();
        
        void tabFilterRadioButtonClicked(int buttonID);
        
        void chartLineLayerSymbolChanged();
        
        void symbolSizeTypeButtonIdClicked(int id);
        
    private:
        void updateContent(const bool scrollTextToEndFlag);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        QWidget* createInfoWidget();
        
        QWidget* createFilteringSettingsWidget();
        
        QWidget* createFilteringFilesWidget();
        
        QWidget* createSymbolsWidget();
        
        QWidget* createChartLineLayerSymbolsWidget();
        
        void updateInfoWidget(const bool scrollTextToEndFlag);
        
        void updateFilteringWidget();
        
        void updateSymbolsWidget();
        
        void updateChartLineLayerSymbolsWidget();
        
        IdentificationHistoryManager* getHistoryManager();
        
        const Location m_location;
        
        QTabWidget* m_tabWidget;
        
        QWidget* m_infoWidget;
        
        QTextBrowser* m_infoTextBrowser;
        
        QSpinBox* m_infoShowHistoryCountSpinBox;
        
        QWidget* m_filteringFilesWidget;

        QWidget* m_filteringSettingsWidget;
        
        QButtonGroup* m_tabFilterButtonGroup;
        
        std::vector<QRadioButton*> m_tabFilteringRadioButtons;
        
        QCheckBox* m_filteringCiftiLoadingCheckBox;
        
        QCheckBox* m_filteringBorderCheckBox;
        
        QCheckBox* m_filteringFociCheckBox;
        
        QCheckBox* m_filteringVertexVoxelCheckBox;
        
        IdentificationFileFilteringTableWidget* m_fileFilteringTableWidget = NULL;
        
        QWidget* m_symbolsWidget;
        
        QCheckBox* m_symbolsShowMediaCheckbox;
        
        QCheckBox* m_symbolsShowSurfaceIdCheckBox;
        
        QCheckBox* m_symbolsShowVolumeIdCheckBox;
        
        CaretColorEnumComboBox* m_symbolsIdColorComboBox;
        
        CaretColorEnumComboBox* m_symbolsContralateralIdColorComboBox;
        
        QRadioButton* m_symbolSizeMillimeterRadioButton;
        
        QRadioButton* m_symbolSizePercentageRadioButton;
        
        QDoubleSpinBox* m_symbolsMillimetersDiameterSpinBox;
        
        QDoubleSpinBox* m_symbolsMillimetersMostRecentDiameterSpinBox;
        
        QDoubleSpinBox* m_symbolsPercentageDiameterSpinBox;
        
        QDoubleSpinBox* m_symbolsPercentageMostRecentDiameterSpinBox;
        
        QDoubleSpinBox* m_symbolsMediaPercentageDiameterSpinBox;
        
        QDoubleSpinBox* m_symbolsMediaPercentageMostRecentDiameterSpinBox;
        
        QCheckBox* m_symbolsSurfaceContralateralVertexCheckBox;
        
        QWidget* m_chartLineLayerSymbolWidget;
        
        QDoubleSpinBox* m_chartLineLayerSymbolSizeSpinBox;
        
        QDoubleSpinBox* m_chartLineLayerToolTipTextSizeSpinBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __IDENTIFICATION_DISPLAY_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IDENTIFICATION_DISPLAY_WIDGET_DECLARE__

} // namespace
#endif  //__IDENTIFICATION_DISPLAY_WIDGET_H__
