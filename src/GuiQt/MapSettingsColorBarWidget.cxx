
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

#define __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__
#include "MapSettingsColorBarWidget.h"
#undef __MAP_SETTINGS_COLOR_BAR_WIDGET_DECLARE__

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

#include "AnnotationColorBar.h"
#include "AnnotationColorWidget.h"
#include "AnnotationCoordinateWidget.h"
#include "AnnotationFontWidget.h"
#include "AnnotationWidgetParentEnum.h"
#include "AnnotationWidthHeightWidget.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "Overlay.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::MapSettingsColorBarWidget 
 * \brief Contains controls for adjusting a color bar's attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapSettingsColorBarWidget::MapSettingsColorBarWidget(QWidget* parent)
: QWidget(parent)
{
    const int32_t browserWindowIndex = 0;
    
    QLabel* positionModeLabel = new QLabel("Location (XYZ)");
    m_annotationColorBarPositionModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_annotationColorBarPositionModeEnumComboBox->setup<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>();
    QObject::connect(m_annotationColorBarPositionModeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(annotationColorBarPositionModeEnumComboBoxItemActivated()));
    m_annotationColorBarPositionModeEnumComboBox->getWidget()->setToolTip("AUTOMATIC - color bars are stacked\n"
                                                                          "   in lower left corner of Tab/Window\n"
                                                                          "MANUAL - user must set the X and Y\n"
                                                                          "   coordinates in the Tab/Window\n");
    
    std::vector<AnnotationCoordinateSpaceEnum::Enum> supportedSpaces;
    supportedSpaces.push_back(AnnotationCoordinateSpaceEnum::TAB);
    supportedSpaces.push_back(AnnotationCoordinateSpaceEnum::WINDOW);
    
    QLabel* coordinateSpaceLabel = new QLabel("Show Color Bar in ");
    m_annotationCoordinateSpaceEnumComboBox = new EnumComboBoxTemplate(this);
    m_annotationCoordinateSpaceEnumComboBox->setupWithItems<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(supportedSpaces);
    QObject::connect(m_annotationCoordinateSpaceEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(annotationCoordinateSpaceEnumComboBoxItemActivated()));
    m_annotationCoordinateSpaceEnumComboBox->getWidget()->setToolTip("TAB - Restricts location of colorbar\n"
                                                                     "   to within its respective tab's\n"
                                                                     "   region in the graphics area\n"
                                                                     "WINDOW - Allows colorbar in any\n"
                                                                     "   location in the graphics area");
    
    m_coordinateWidget = new AnnotationCoordinateWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
                                                        AnnotationCoordinateWidget::COORDINATE_ONE,
                                                        browserWindowIndex);
    
    m_widthHeightWidget = new AnnotationWidthHeightWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
                                                          browserWindowIndex);
    
    m_fontWidget = new AnnotationFontWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
                                            browserWindowIndex);
    
    m_colorWidget = new AnnotationColorWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
                                              browserWindowIndex);
    
    QWidget* modeSpaceWidget = new QWidget();
    QGridLayout* modeSpaceLayout = new QGridLayout(modeSpaceWidget);
    modeSpaceLayout->setContentsMargins(0, 0, 0, 0);
    modeSpaceLayout->addWidget(coordinateSpaceLabel, 0, 0);
    modeSpaceLayout->addWidget(m_annotationCoordinateSpaceEnumComboBox->getWidget(), 0, 1);
    modeSpaceLayout->addWidget(positionModeLabel, 1, 0);
    modeSpaceLayout->addWidget(m_annotationColorBarPositionModeEnumComboBox->getWidget(), 1, 1);
    
    QGroupBox* locationAndDimGroupBox = new QGroupBox("Location and Dimensions");
    QVBoxLayout* locationAndDimLayout = new QVBoxLayout(locationAndDimGroupBox);
    locationAndDimLayout->setContentsMargins(0, 0, 0, 0);
    locationAndDimLayout->setSpacing(2);
    locationAndDimLayout->addWidget(modeSpaceWidget, 0, Qt::AlignLeft);
    locationAndDimLayout->addWidget(m_coordinateWidget, 0, Qt::AlignLeft);
    locationAndDimLayout->addWidget(m_widthHeightWidget, 0, Qt::AlignLeft);
    
    QGroupBox* fontGroupBox = new QGroupBox("Font and Colors");
    QVBoxLayout* fontGroupLayout = new QVBoxLayout(fontGroupBox);
    fontGroupLayout->addWidget(m_fontWidget);
    fontGroupLayout->addWidget(m_colorWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 6);
    layout->addWidget(locationAndDimGroupBox, 0, Qt::AlignLeft);
    layout->addWidget(fontGroupBox, 0, Qt::AlignLeft);
    layout->addStretch();

    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsColorBarWidget::~MapSettingsColorBarWidget()
{
}

/**
 * Update the content of widget.
 *
 * @param overlay
 *     Overlay for display in this widget.
 */
void
MapSettingsColorBarWidget::updateContent(Overlay* overlay)
{
    m_overlay = overlay;
    
    bool enableWidget = false;
    if (m_overlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = 0;
        m_overlay->getSelectionData(mapFile,
                                    mapIndex);
        if (mapFile != NULL) {
            if (mapFile->isMappedWithPalette()) {
                AnnotationColorBar* colorBar = overlay->getColorBar();
                
                const AnnotationColorBarPositionModeEnum::Enum positionMode = colorBar->getPositionMode();
                m_annotationColorBarPositionModeEnumComboBox->setSelectedItem<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>(positionMode);
                
                const AnnotationCoordinateSpaceEnum::Enum coordinateSpace = colorBar->getCoordinateSpace();
                m_annotationCoordinateSpaceEnumComboBox->setSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>(coordinateSpace);
                
                std::vector<Annotation*> annotationVector;
                annotationVector.push_back(colorBar);
                
                std::vector<AnnotationTwoDimensionalShape*> annotationTwoDimVector;
                annotationTwoDimVector.push_back(colorBar);
                
                m_coordinateWidget->updateContent(colorBar);
                m_widthHeightWidget->updateContent(annotationTwoDimVector);
                m_fontWidget->updateAnnotationColorBarContent(colorBar);
                m_colorWidget->updateContent(annotationVector);
                
                bool enableSelections = false;
                switch (positionMode) {
                    case AnnotationColorBarPositionModeEnum::AUTOMATIC:
                        break;
                    case AnnotationColorBarPositionModeEnum::MANUAL:
                        enableSelections = true;
                        break;
                }
                
                m_coordinateWidget->setEnabled(enableSelections);
                
                enableWidget = true;
            }
        }
    }
    
    setEnabled(enableWidget);
}

/**
 * Called when a control is changed.
 */
void
MapSettingsColorBarWidget::applySelections()
{
    if (m_overlay != NULL) {
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when a position mode combo box item is selected.
 */
void
MapSettingsColorBarWidget::annotationColorBarPositionModeEnumComboBoxItemActivated()
{
    const AnnotationColorBarPositionModeEnum::Enum positionMode = m_annotationColorBarPositionModeEnumComboBox->getSelectedItem<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>();
    if (m_overlay != NULL) {
        m_overlay->getColorBar()->setPositionMode(positionMode);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    updateContent(m_overlay);
}

/**
 * Gets called when a coordinate space combo box item is selected.
 */
void
MapSettingsColorBarWidget::annotationCoordinateSpaceEnumComboBoxItemActivated()
{
    const AnnotationCoordinateSpaceEnum::Enum coordinateSpace = m_annotationCoordinateSpaceEnumComboBox->getSelectedItem<AnnotationCoordinateSpaceEnum,AnnotationCoordinateSpaceEnum::Enum>();
    if (m_overlay != NULL) {
        m_overlay->getColorBar()->setCoordinateSpace(coordinateSpace);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

