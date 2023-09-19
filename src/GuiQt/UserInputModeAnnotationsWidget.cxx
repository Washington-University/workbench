
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

#define __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__
#include "UserInputModeAnnotationsWidget.h"
#undef __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__

#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "AnnotationBackgroundTypeWidget.h"
#include "AnnotationBoundsWidget.h"
#include "AnnotationBrowserTab.h"
#include "AnnotationCoordinateCenterXYWidget.h"
#include "AnnotationCoordinateSpaceWidget.h"
#include "AnnotationCoordinatesWidget.h"
#include "AnnotationColorWidget.h"
#include "AnnotationDeleteWidget.h"
#include "AnnotationDepthWidget.h"
#include "AnnotationFinishCancelWidget.h"
#include "AnnotationFontWidget.h"
#include "AnnotationFormatWidget.h"
#include "AnnotationInsertNewWidget.h"
#include "AnnotationLine.h"
#include "AnnotationLineArrowTipsWidget.h"
#include "AnnotationManager.h"
#include "AnnotationNameWidget.h"
#include "AnnotationTwoCoordinateShape.h"
#include "AnnotationRedoUndoWidget.h"
#include "AnnotationRotationWidget.h"
#include "AnnotationText.h"
#include "AnnotationTextAlignmentWidget.h"
#include "AnnotationTextEditorWidget.h"
#include "AnnotationTextOrientationWidget.h"
#include "AnnotationWidthHeightWidget.h"
#include "Brain.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "EventAnnotationCreateNewType.h"
#include "EventBrainReset.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "TileTabsLayoutConfigurationTypeWidget.h"
#include "UserInputModeAnnotations.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::UserInputModeAnnotationsWidget 
 * \brief Toolbar widget for annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param inputModeAnnotations
 *    Annotation mode input processor.
 * @param browserWindowIndex
 *    Index of browser window using this widget.
 */
UserInputModeAnnotationsWidget::UserInputModeAnnotationsWidget(UserInputModeAnnotations* inputModeAnnotations,
                                                               const int32_t browserWindowIndex)
: QWidget(),
m_browserWindowIndex(browserWindowIndex),
m_inputModeAnnotations(inputModeAnnotations)
{
    CaretAssert(inputModeAnnotations);
    
    switch (inputModeAnnotations->getUserInputMode()) {
        case UserInputModeEnum::Enum::ANNOTATIONS:
            createAnnotationWidget();
            break;
        case UserInputModeEnum::Enum::SAMPLES_EDITING:
            createSamplesEditingWidget();
            break;
        case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            createTileTabsEditingWidget();
            break;
        case UserInputModeEnum::Enum::BORDERS:
        case UserInputModeEnum::Enum::FOCI:
        case UserInputModeEnum::Enum::IMAGE:
        case UserInputModeEnum::Enum::INVALID:
        case UserInputModeEnum::Enum::VIEW:
        case UserInputModeEnum::Enum::VOLUME_EDIT:
            CaretAssert(0);
            return;
            break;
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BRAIN_RESET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_CREATE_NEW_TYPE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    setFixedHeight(sizeHint().height());
}

/**
 * Destructor.
 */
UserInputModeAnnotationsWidget::~UserInputModeAnnotationsWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
UserInputModeAnnotationsWidget::receiveEvent(Event* event)
{
    bool updateAnnotationWidgetsFlag = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_RESET) {
        EventBrainReset* brainEvent = dynamic_cast<EventBrainReset*>(event);
        CaretAssert(brainEvent);
        
        updateAnnotationWidgetsFlag = true;
        
        brainEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_CREATE_NEW_TYPE) {
        EventAnnotationCreateNewType* annotationEvent = dynamic_cast<EventAnnotationCreateNewType*>(event);
        CaretAssert(annotationEvent);
        updateAnnotationWidgetsFlag = true;
        annotationEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* updateEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(updateEvent);
        updateAnnotationWidgetsFlag = true;
        updateEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE) {
        updateAnnotationWidgetsFlag = true;
    }
    else {
        return;
    }

    if (! updateAnnotationWidgetsFlag) {
        return;
    }
    
    updateWidget();
    
}

/**
 * Create the samples mode widget
 */
void
UserInputModeAnnotationsWidget::createSamplesEditingWidget()
{
    m_colorWidget                = new AnnotationColorWidget(m_inputModeAnnotations->getUserInputMode(),
                                                             AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                             m_browserWindowIndex);
    m_insertNewWidget            = new AnnotationInsertNewWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                 m_browserWindowIndex);
    
    m_deleteWidget               = new AnnotationDeleteWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              m_browserWindowIndex);

    m_redoUndoWidget             = new AnnotationRedoUndoWidget(Qt::Horizontal,
                                                                m_inputModeAnnotations->getUserInputMode(),
                                                                m_browserWindowIndex);
    
    m_finishCancelWidget         = new AnnotationFinishCancelWidget(Qt::Horizontal,
                                                                    m_inputModeAnnotations->getUserInputMode(),
                                                                    m_browserWindowIndex);
    
    m_depthWidget = new  AnnotationDepthWidget(m_inputModeAnnotations->getUserInputMode(),
                                               m_browserWindowIndex);
    
    QGridLayout* leftLayout(new QGridLayout());
    WuQtUtilities::setLayoutSpacingAndMargins(leftLayout, 2, 0);
    leftLayout->addWidget(m_colorWidget,
                          0, 0);
    leftLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                          0, 1);
    leftLayout->addWidget(m_depthWidget,
                          0, 2, Qt::AlignLeft);
    
    QGridLayout* rightLayout(new QGridLayout());
    WuQtUtilities::setLayoutSpacingAndMargins(rightLayout, 2, 0);
    rightLayout->addWidget(m_insertNewWidget,
                           0, 0, 1, 5, Qt::AlignLeft);
    rightLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                           1, 0, 1, 5);
    rightLayout->addWidget(m_finishCancelWidget,
                           2, 0);
    rightLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                           2, 1);
    rightLayout->addWidget(m_deleteWidget,
                           2, 2);
    rightLayout->addWidget(WuQtUtilities::createVerticalLineWidget(),
                           2, 3);
    rightLayout->addWidget(m_redoUndoWidget,
                           2, 4);
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(8);
    layout->addLayout(leftLayout);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addLayout(rightLayout);
    layout->addStretch();
}

/**
 * Create annotation mode widget
 */
void
UserInputModeAnnotationsWidget::createTileTabsEditingWidget()
{
    m_layoutTypeWidget           = new TileTabsLayoutConfigurationTypeWidget(TileTabsLayoutConfigurationTypeWidget::ParentType::BROWSER_WINDOW_TOOLBAR);
    
    QWidget* layoutTypeToolBarWidget = BrainBrowserWindowToolBar::createToolWidget("Layout Type",
                                                                            m_layoutTypeWidget,
                                                                            BrainBrowserWindowToolBar::WIDGET_PLACEMENT_NONE,
                                                                            BrainBrowserWindowToolBar::WIDGET_PLACEMENT_TOP,
                                                                            0);

    m_nameWidget                 = new AnnotationNameWidget(m_inputModeAnnotations->getUserInputMode(),
                                                            m_browserWindowIndex);
    
    QWidget* nameToolBarWidget = BrainBrowserWindowToolBar::createToolWidget("Edit Tab(s)",
                                                                            m_nameWidget,
                                                                            BrainBrowserWindowToolBar::WIDGET_PLACEMENT_NONE,
                                                                            BrainBrowserWindowToolBar::WIDGET_PLACEMENT_TOP,
                                                                            0);
    
    m_boundsWidget               = new AnnotationBoundsWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                              m_browserWindowIndex);
    
    QWidget* boundsToolBarWidget = BrainBrowserWindowToolBar::createToolWidget("Bounds",
                                                                               m_boundsWidget,
                                                                               BrainBrowserWindowToolBar::WIDGET_PLACEMENT_NONE,
                                                                               BrainBrowserWindowToolBar::WIDGET_PLACEMENT_TOP,
                                                                               0);
    
    m_coordinateCenterXYWidget   = new AnnotationCoordinateCenterXYWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                          AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                          AnnotationCoordinateCenterXYWidget::COORDINATE_ONE,
                                                                          m_browserWindowIndex);
    
    m_widthHeightWidget          = new AnnotationWidthHeightWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                   AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                   m_browserWindowIndex,
                                                                   Qt::Vertical);
    
    m_backgroundTypeWidget       = new AnnotationBackgroundTypeWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                      AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                      m_browserWindowIndex);
    
    QWidget* backgroundTypeToolBarWidget = BrainBrowserWindowToolBar::createToolWidget("Background",
                                                                                       m_backgroundTypeWidget,
                                                                                       BrainBrowserWindowToolBar::WIDGET_PLACEMENT_NONE,
                                                                                       BrainBrowserWindowToolBar::WIDGET_PLACEMENT_TOP,
                                                                                       0);

    m_formatWidget               = new AnnotationFormatWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              m_browserWindowIndex);
    
    m_redoUndoWidget             = new AnnotationRedoUndoWidget(Qt::Vertical,
                                                                m_inputModeAnnotations->getUserInputMode(),
                                                                m_browserWindowIndex);

    QWidget* centerSizeWidget = new QWidget();
    QVBoxLayout* centerSizeLayout = new QVBoxLayout(centerSizeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(centerSizeLayout, 2, 0);
    centerSizeLayout->addWidget(m_coordinateCenterXYWidget, 0, Qt::AlignRight);
    centerSizeLayout->addSpacing(10);
    centerSizeLayout->addWidget(m_widthHeightWidget, 0, Qt::AlignRight);
    centerSizeLayout->addStretch();
    QWidget* centerSizeToolBarWidget = BrainBrowserWindowToolBar::createToolWidget("Center",
                                                                                   centerSizeWidget,
                                                                                   BrainBrowserWindowToolBar::WIDGET_PLACEMENT_NONE,
                                                                                   BrainBrowserWindowToolBar::WIDGET_PLACEMENT_TOP,
                                                                                   0);

    QVBoxLayout* formatRedoLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(formatRedoLayout, 2, 0);
    formatRedoLayout->addWidget(m_formatWidget);
    formatRedoLayout->addSpacing(12);
    formatRedoLayout->addWidget(m_redoUndoWidget);
    formatRedoLayout->addStretch();
    
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(8);
    layout->addWidget(layoutTypeToolBarWidget);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addWidget(nameToolBarWidget);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addWidget(boundsToolBarWidget);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addWidget(centerSizeToolBarWidget);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addWidget(backgroundTypeToolBarWidget);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget());
    layout->addLayout(formatRedoLayout);
    layout->addStretch();
}

/*
 * Create tile tabs mode widget
 */
void
UserInputModeAnnotationsWidget::createAnnotationWidget()
{
    m_textEditorWidget           = new AnnotationTextEditorWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                  m_browserWindowIndex);
    
    m_lineArrowTipsWidget        = new AnnotationLineArrowTipsWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                     m_browserWindowIndex);
    
    m_fontWidget                 = new AnnotationFontWidget(AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                            m_inputModeAnnotations->getUserInputMode(),
                                                            m_browserWindowIndex);
    
    m_colorWidget                = new AnnotationColorWidget(m_inputModeAnnotations->getUserInputMode(),
                                                             AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                             m_browserWindowIndex);
    
    m_textAlignmentWidget        = new AnnotationTextAlignmentWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                     m_browserWindowIndex);
    
    m_textOrientationWidget      = new AnnotationTextOrientationWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                       m_browserWindowIndex);
    
    QLabel* coordinateSpaceLabel = new QLabel("Space");
    m_coordinateSpaceWidget      = new AnnotationCoordinateSpaceWidget(m_browserWindowIndex);
    
    m_coordinatesWidget        = new AnnotationCoordinatesWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                  AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                  m_browserWindowIndex);
    
    m_widthHeightWidget          = new AnnotationWidthHeightWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                   AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET,
                                                                   m_browserWindowIndex,
                                                                   Qt::Vertical);
    
    m_rotationWidget             = new AnnotationRotationWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                m_browserWindowIndex);
    
    m_formatWidget               = new AnnotationFormatWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              m_browserWindowIndex);
    
    m_insertNewWidget            = new AnnotationInsertNewWidget(m_inputModeAnnotations->getUserInputMode(),
                                                                 m_browserWindowIndex);
    
    m_finishCancelWidget         = new AnnotationFinishCancelWidget(Qt::Horizontal,
                                                                    m_inputModeAnnotations->getUserInputMode(),
                                                                    m_browserWindowIndex);
    
    m_deleteWidget               = new AnnotationDeleteWidget(m_inputModeAnnotations->getUserInputMode(),
                                                              m_browserWindowIndex);
    
    m_redoUndoWidget             = new AnnotationRedoUndoWidget(Qt::Horizontal,
                                                                m_inputModeAnnotations->getUserInputMode(),
                                                                m_browserWindowIndex);
    
    /*
     * Layout top row of widgets
     */
    QHBoxLayout* leftTopRowLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(leftTopRowLayout, 2, 0);
    leftTopRowLayout->addWidget(m_colorWidget, 0, Qt::AlignTop);
    leftTopRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    leftTopRowLayout->addWidget(m_lineArrowTipsWidget, 0, Qt::AlignTop);
    leftTopRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    leftTopRowLayout->addWidget(m_textEditorWidget, 100, Qt::AlignTop);
    leftTopRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    leftTopRowLayout->addWidget(m_fontWidget, 0, Qt::AlignTop);
    leftTopRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    leftTopRowLayout->addWidget(m_textAlignmentWidget, 0, Qt::AlignTop);
    leftTopRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    leftTopRowLayout->addWidget(m_textOrientationWidget, 0, Qt::AlignTop);
    
    QHBoxLayout* rightTopRowLayout(new QHBoxLayout());
    rightTopRowLayout->addWidget(m_insertNewWidget, 0, Qt::AlignTop);
    rightTopRowLayout->addStretch();
    
    /*
     * Layout bottom row of widgets
     */
    QGridLayout* leftBottomRowLayout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(leftBottomRowLayout, 2, 0);
    int32_t lbColumn(0);
    leftBottomRowLayout->addWidget(coordinateSpaceLabel, 0, lbColumn);
    leftBottomRowLayout->addWidget(m_coordinateSpaceWidget, 1, lbColumn, Qt::AlignHCenter);
    lbColumn++;
    leftBottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, lbColumn, 2, 1);
    lbColumn++;
    leftBottomRowLayout->addWidget(m_coordinatesWidget, 0, lbColumn, 2, 1);
    lbColumn++;
    leftBottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, lbColumn, 2, 1);
    lbColumn++;
    leftBottomRowLayout->addWidget(m_widthHeightWidget, 0, lbColumn, 2, 1);
    lbColumn++;
    leftBottomRowLayout->addWidget(m_rotationWidget, 0, lbColumn, 2, 1, Qt::AlignTop);
    lbColumn++;
    
    QHBoxLayout* rightBottomRowLayout(new QHBoxLayout());
    WuQtUtilities::setLayoutSpacingAndMargins(rightBottomRowLayout, 2, 0);
    rightBottomRowLayout->addWidget(m_finishCancelWidget, 0, Qt::AlignTop);
    rightBottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    rightBottomRowLayout->addWidget(m_deleteWidget, 0, Qt::AlignTop);
    rightBottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    rightBottomRowLayout->addWidget(m_formatWidget, 0, Qt::AlignTop);
    rightBottomRowLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    rightBottomRowLayout->addWidget(m_redoUndoWidget, 0, Qt::AlignTop);
    
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(leftTopRowLayout, 0, 0);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget(), 1, 0);
    layout->addLayout(leftBottomRowLayout, 2, 0);
    layout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0, 1, 3, 1);
    layout->addLayout(rightTopRowLayout, 0, 2);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget(), 1, 2);
    layout->addLayout(rightBottomRowLayout, 2, 2);

    setSizePolicy(QSizePolicy::Fixed,
                  sizePolicy().verticalPolicy());
}

/**
 * Update the widget.
 */
void
UserInputModeAnnotationsWidget::updateWidget()
{
    /*
     * Show the proper widget
     */
    switch (m_inputModeAnnotations->getMode()) {
        case UserInputModeAnnotations::Mode::MODE_DRAWING_NEW_SIMPLE_SHAPE_INITIALIZE:
            break;
        case UserInputModeAnnotations::Mode::MODE_DRAWING_NEW_POLY_TYPE:
            break;
        case UserInputModeAnnotations::Mode::MODE_DRAWING_NEW_POLY_TYPE_INITIALIZE:
            break;
        case UserInputModeAnnotations::Mode::MODE_DRAWING_NEW_POLY_TYPE_STEREOTAXIC:
            break;
        case UserInputModeAnnotations::Mode::MODE_DRAWING_NEW_POLY_TYPE_STEREOTAXIC_INITIALIZE:
            break;
        case UserInputModeAnnotations::Mode::MODE_DRAWING_NEW_SIMPLE_SHAPE:
            break;
        case UserInputModeAnnotations::Mode::MODE_PASTE:
            break;
        case UserInputModeAnnotations::Mode::MODE_PASTE_SPECIAL:
            break;
        case UserInputModeAnnotations::Mode::MODE_SELECT:
            break;
    }
    
    AnnotationManager* annotationManager = GuiManager::get()->getBrain()->getAnnotationManager(m_inputModeAnnotations->getUserInputMode());
    
    
    std::vector<Annotation*> selectedAnnotations = annotationManager->getAnnotationsSelectedForEditing(m_browserWindowIndex);
    
    std::vector<AnnotationBrowserTab*> browserTabAnnotations;
    std::vector<AnnotationLine*> lineAnnotations;
    std::vector<AnnotationText*> textAnnotations;
    std::vector<AnnotationFontAttributesInterface*> fontStyleAnnotations;
    std::vector<AnnotationOneCoordinateShape*> twoDimAnnotations;
    std::vector<AnnotationTwoCoordinateShape*> oneDimAnnotations;
    
    for (std::vector<Annotation*>::iterator iter = selectedAnnotations.begin();
         iter != selectedAnnotations.end();
         iter++) {
        Annotation* ann = *iter;
        CaretAssert(ann);
        
        if (ann->getType() == AnnotationTypeEnum::BROWSER_TAB) {
            AnnotationBrowserTab* abt = dynamic_cast<AnnotationBrowserTab*>(ann);
            if (abt != NULL) {
                browserTabAnnotations.push_back(abt);
            }
        }
        
        AnnotationText* annText = dynamic_cast<AnnotationText*>(ann);
        if (annText != NULL) {
            textAnnotations.push_back(annText);
        }
        
        AnnotationFontAttributesInterface* annFontStyle = dynamic_cast<AnnotationFontAttributesInterface*>(ann);
        if (annFontStyle != NULL) {
            fontStyleAnnotations.push_back(annFontStyle);
        }
        
        AnnotationTwoCoordinateShape* annOne = dynamic_cast<AnnotationTwoCoordinateShape*>(ann);
        if (annOne != NULL) {
            oneDimAnnotations.push_back(annOne);
        }
        
        AnnotationOneCoordinateShape* annTwo= dynamic_cast<AnnotationOneCoordinateShape*>(ann);
        if (annTwo != NULL) {
            twoDimAnnotations.push_back(annTwo);
        }
        
        AnnotationLine* annLine = dynamic_cast<AnnotationLine*>(ann);
        if (annLine != NULL) {
            lineAnnotations.push_back(annLine);
        }
    }
    
    /*
     * Note: pointers are initialized to NULL in the header file
     */
    if (m_layoutTypeWidget != NULL) m_layoutTypeWidget->updateContent(m_browserWindowIndex);
    if (m_nameWidget != NULL) m_nameWidget->updateContent(selectedAnnotations);
    if (m_boundsWidget != NULL) m_boundsWidget->updateContent(browserTabAnnotations);
    if (m_coordinateSpaceWidget != NULL) m_coordinateSpaceWidget->updateContent(selectedAnnotations);
    if (m_fontWidget != NULL) m_fontWidget->updateContent(fontStyleAnnotations);
    if (m_textEditorWidget != NULL) m_textEditorWidget->updateContent(textAnnotations);
    if (m_colorWidget != NULL) m_colorWidget->updateContent(selectedAnnotations);
    if (m_lineArrowTipsWidget != NULL) m_lineArrowTipsWidget->updateContent(lineAnnotations);
    if (m_textAlignmentWidget != NULL) m_textAlignmentWidget->updateContent(textAnnotations);
    if (m_textOrientationWidget != NULL) m_textOrientationWidget->updateContent(textAnnotations);
    if (m_widthHeightWidget != NULL) m_widthHeightWidget->updateContent(twoDimAnnotations);
    if (m_rotationWidget != NULL) m_rotationWidget->updateContent(selectedAnnotations);
    if (m_insertNewWidget != NULL) m_insertNewWidget->updateContent();
    if (m_deleteWidget != NULL) m_deleteWidget->updateContent();
    if (m_depthWidget != NULL) m_depthWidget->updateContent(selectedAnnotations);
    if (m_finishCancelWidget != NULL) m_finishCancelWidget->updateContent(selectedAnnotations);
    
    Annotation* coordEditAnnotation = NULL;
    if (selectedAnnotations.size() == 1) {
        coordEditAnnotation = selectedAnnotations[0];
    }
    if (m_coordinateCenterXYWidget != NULL) m_coordinateCenterXYWidget->updateContent(selectedAnnotations);
    if (m_coordinatesWidget != NULL) m_coordinatesWidget->updateContent(coordEditAnnotation);    
    if (m_backgroundTypeWidget != NULL) m_backgroundTypeWidget->updateContent(browserTabAnnotations);
    if (m_formatWidget != NULL) m_formatWidget->updateContent(selectedAnnotations);
    if (m_redoUndoWidget != NULL) m_redoUndoWidget->updateContent(selectedAnnotations);
}


