#ifndef __OVERLAY_SETTINGS_EDITOR_DIALOG__H_
#define __OVERLAY_SETTINGS_EDITOR_DIALOG__H_

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

#include "ChartTwoOverlay.h"
#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
class QLabel;
class QLayout;
class QPushButton;
class QTabWidget;

namespace caret {
    
    class CaretMappableDataFile;
    class ChartTwoOverlay;
    class LabelTableEditorWidget;
    class MapSettingsColorBarWidget;
    class MapSettingsFiberTrajectoryWidget;
    class MapSettingsLabelsWidget;
    class MapSettingsLayerWidget;
    class MapSettingsPaletteColorMappingWidget;
    class MapSettingsParcelsWidget;
    class Overlay;
    
    class OverlaySettingsEditorDialog : public WuQDialogNonModal, public EventListenerInterface {
        Q_OBJECT
        
    public:
        OverlaySettingsEditorDialog(QWidget* parent);
        
        void updateDialogContent(Overlay* brainordinateOverlay,
                                 ChartTwoOverlay* chartOverlay);
        
        void updateIfThisOverlayIsInDialog(Overlay* brainordinateOverlay,
                                           ChartTwoOverlay* chartOverlay);
        
        void updateDialog();
        
        virtual ~OverlaySettingsEditorDialog();

        bool isDoNotReplaceSelected() const;
        
        virtual void receiveEvent(Event* event);
        
    protected:
        virtual void closeButtonPressed();
        
        virtual void focusInEvent(QFocusEvent* event);
        
    private:
        OverlaySettingsEditorDialog(const OverlaySettingsEditorDialog&);

        OverlaySettingsEditorDialog& operator=(const OverlaySettingsEditorDialog&);
        
    private slots:
        void doNotReplaceCheckBoxStateChanged(int state);
        
        void editLabelTablePushButtonClicked();
        
    private:
        void updateDialogContentPrivate(Overlay* brainordinateOverlay,
                                        ChartTwoOverlay* chartOverlay);
        
        QWidget* createWindowOptionsSection();
        
        QWidget* createMapFileAndNameSection();
        
        QWidget* createLabelsSection();
        
        void setLayoutSpacingAndMargins(QLayout* layout);
        
        QTabWidget* m_tabWidget;
        
        QCheckBox* m_doNotReplaceCheckBox;
        
        CaretMappableDataFile* m_caretMappableDataFile;
        
        Overlay* m_brainordinateOverlay;
        
        ChartTwoOverlay* m_chartOverlay;
        
        ChartTwoOverlay::SelectedIndexType m_chartOverlaySelectedIndexType;
        
        int32_t m_selectedMapFileIndex;
        
        MapSettingsPaletteColorMappingWidget* m_paletteColorMappingWidget;
        
        MapSettingsParcelsWidget* m_parcelsWidget;
        
        MapSettingsColorBarWidget* m_colorBarWidget;
        
        LabelTableEditorWidget* m_labelTableEditorWidget;
        
        MapSettingsFiberTrajectoryWidget* m_fiberTrajectoryWidget;
        
        MapSettingsLayerWidget* m_layerWidget;
        
        MapSettingsLabelsWidget* m_labelsWidget;
        
        QPushButton* m_editLabelTablePushButton;
        
        QLabel* m_selectedMapFileNameLabel;
        
        QLabel* m_selectedMapNameLabel;
        
        int32_t m_colorBarWidgetTabIndex;
        
        int32_t m_labelsWidgetTabIndex;
        
        int32_t m_layersWidgetTabIndex;
        
        int32_t m_metadataWidgetTabIndex;
        
        int32_t m_paletteWidgetTabIndex;
        
        int32_t m_parcelsWidgetTabIndex;
        
        int32_t m_trajectoryWidgetTabIndex;
        
    };
    
#ifdef __OVERLAY_SETTINGS_EDITOR_DIALOG_DECLARE__
#endif // __OVERLAY_SETTINGS_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__OVERLAY_SETTINGS_EDITOR_DIALOG__H_
