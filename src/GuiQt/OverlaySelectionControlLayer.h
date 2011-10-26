#ifndef __OVERLAY_SELECTION_CONTROL_LAYER__H_
#define __OVERLAY_SELECTION_CONTROL_LAYER__H_

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


#include <QObject>

#include "OverlaySelectionControl.h"

class QAction;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QToolButton;

namespace caret {

    class BrowserTabContent;
    class PaletteEditorDialog;
    class WuQWidgetObjectGroup;
    
    /// One layer in a overlay selection control
    class OverlaySelectionControlLayer : public QObject {
        Q_OBJECT
    public:
        OverlaySelectionControlLayer(const int32_t browserWindowIndex,
                                     OverlaySelectionControl* overlaySelectionControl,
                                     const int32_t layerIndex);
        
        ~OverlaySelectionControlLayer();
        
        void addWidget(QWidget* w);
        
        bool isVisible() const;
        void setVisible(const bool visible);
        
        void updateControl(BrowserTabContent* browserTabContent);
        void updateOverlayControl(BrowserTabContent* browserTabContent);
        
        int32_t browserWindowIndex;
        int32_t layerIndex;
        
        QCheckBox* enabledCheckBox;
        QComboBox* fileSelectionComboBox;
        QComboBox* columnSelectionComboBox;
        QToolButton* histogramToolButton;
        QToolButton* settingsToolButton;
        QToolButton* metadataToolButton;
        QDoubleSpinBox* opacityDoubleSpinBox;            
        
        QToolButton* deleteToolButton;
        QToolButton* upArrowToolButton;
        QToolButton* downArrowToolButton;
        
        QAction* histogramAction;
        QAction* settingsAction;
        QAction* metadataAction;
        QAction* moveUpAction;
        QAction* moveDownAction;
        QAction* removeAction;
        
        WuQWidgetObjectGroup* widgetGroup;
        
        PaletteEditorDialog* paletteEditorDialog;
    signals:
        /** Emitted when a control is removed and container should shrink */
        void controlRemoved();
        
    public slots:
        void enableCheckBoxToggled(bool toggled);
        void opacityValueChanged(double value);
        void fileSelected(int fileIndex);
        void columnSelected(int columnIndex);
        
        void moveLayerUpToolButtonPressed();
        void moveLayerDownToolButtonPressed();
        void removeLayerToolButtonPressed();
        void settingsToolButtonPressed();
        void metadataToolButtonPressed();
        void histogramToolButtonPressed();
        
    private:
        OverlaySelectionControlLayer(const OverlaySelectionControlLayer&);
        OverlaySelectionControlLayer& operator=(const OverlaySelectionControlLayer&);
        
        /** selection control that contains this layer */
        OverlaySelectionControl* overlaySelectionControl;
    };
#ifdef __OVERLAY_SELECTION_CONTROL_LAYER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OVERLAY_SELECTION_CONTROL_LAYER_DECLARE__

} // namespace
#endif  //__OVERLAY_SELECTION_CONTROL_LAYER__H_
