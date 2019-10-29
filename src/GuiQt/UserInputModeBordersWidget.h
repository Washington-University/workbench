#ifndef __USER_INPUT_MODE_BORDERS_WIDGET__H_
#define __USER_INPUT_MODE_BORDERS_WIDGET__H_

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


#include <QWidget>

#include "DisplayGroupEnum.h"
#include "EventListenerInterface.h"

class QAbstractButton;
class QAction;
class QComboBox;
class QRadioButton;
class QStackedWidget;
class QToolButton;

namespace caret {

    class BorderFile;
    class BorderOptimizeDialog;
    class Border;
    class Brain;
    class Surface;
    class UserInputModeBorders;
    
    class UserInputModeBordersWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        UserInputModeBordersWidget(UserInputModeBorders* inputModeBorders,
                                   QWidget* parent = 0);
        
        virtual ~UserInputModeBordersWidget();
        
        virtual void receiveEvent(Event* event);

        void updateWidget();
        
        void executeFinishOperation();
        
        void executeRoiInsideSelectedBorderOperation(Brain* brain,
                                                     Surface* surface,
                                                     Border* border);
        
    private slots:
        void adjustViewActionTriggered();
        
        void drawResetButtonClicked();
        void drawUndoButtonClicked();
        void drawUndoLastEditButtonClicked();
        void drawFinishButtonClicked();
        
        void modeRadioButtonClicked(QAbstractButton* button);
        
        void drawRadioButtonClicked(QAbstractButton* button);
        
        void editRadioButtonClicked(QAbstractButton* button);
        
    private:
        class BorderFileAndBorderMemento {
        public:
            BorderFileAndBorderMemento(BorderFile* borderFile,
                                       Border* border) {
                m_borderFile = borderFile;
                m_border     = border;
            }
            
            BorderFile* m_borderFile;
            Border*     m_border;
        };
        
        UserInputModeBordersWidget(const UserInputModeBordersWidget&);

        UserInputModeBordersWidget& operator=(const UserInputModeBordersWidget&);
        
        QWidget* createModeWidget();
        
        QWidget* createDrawOperationWidget();
        
        QWidget* createEditOperationWidget();
        
        QWidget* createRoiOperationWidget();
        
        void setLastEditedBorder(std::vector<BorderFileAndBorderMemento>& undoFinishBorders);
        
        void resetLastEditedBorder();
        
        void processBorderOptimization(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t browserTabIndex,
                                       Surface* surface,
                                       Border* borderDrawnByUser);
        
        QRadioButton* m_modeDrawRadioButton;
        
        QRadioButton* m_modeEditRadioButton;
        
        QRadioButton* m_modeRoiRadioButton;
        
        QRadioButton* m_drawNewRadioButton;
        
        QRadioButton* m_drawEraseRadioButton;
        
        QRadioButton* m_drawExtendRadioButton;
        
        QRadioButton* m_drawOptimizeRadioButton;
        
        QRadioButton* m_drawReplaceRadioButton;
        
        QRadioButton* m_editDeleteRadioButton;
        
        QRadioButton* m_editPropertiesRadioButton;
        
        QWidget* widgetMode;
        
        QWidget* widgetDrawOperation;
        
        QWidget* widgetEditOperation;
        
        QWidget* widgetRoiOperation;
        
        QStackedWidget* operationStackedWidget;
        
        UserInputModeBorders* inputModeBorders;
        
        QString m_transformToolTipText;
        
        QToolButton* m_undoFinishToolButton;
        
        BorderOptimizeDialog* m_borderOptimizeDialog;
        
        std::vector<BorderFileAndBorderMemento> m_undoFinishBorders;
    };
    
#ifdef __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_BORDERS_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_BORDERS_WIDGET__H_
