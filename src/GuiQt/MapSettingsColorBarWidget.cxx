
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

#include <QGridLayOut>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
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
#include "NumericFormatModeEnum.h"
#include "Overlay.h"
#include "PaletteColorBarValuesModeEnum.h"
#include "PaletteColorMapping.h"
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
    
    
    QWidget* locationDimWidget = this->createLocationDimensionsSection();
    
    QWidget* fontColorWidget = this->createFontColorsSection();
    
    QWidget* colorBarWidget = this->createColorBarSection();
    
    
    
    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 6, 6);
    layout->addWidget(locationDimWidget, 0, 0, 1, 2, Qt::AlignLeft);
    layout->addWidget(fontColorWidget, 1, 0, Qt::AlignLeft);
    layout->addWidget(colorBarWidget, 1, 1, Qt::AlignLeft);
    layout->setSizeConstraint(QLayout::SetFixedSize);

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
    m_paletteColorMapping = NULL;
    
    bool enableWidget = false;
    if (m_overlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = 0;
        m_overlay->getSelectionData(mapFile,
                                    mapIndex);
        if (mapFile != NULL) {
            if (mapFile->isMappedWithPalette()) {
                m_paletteColorMapping = mapFile->getMapPaletteColorMapping(mapIndex);
                
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

        this->updateColorBarAttributes();
        
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
        if (m_paletteColorMapping != NULL) {
            m_paletteColorMapping->setColorBarValuesMode(m_colorBarDataModeComboBox->getSelectedItem<PaletteColorBarValuesModeEnum, PaletteColorBarValuesModeEnum::Enum>());
            m_paletteColorMapping->setNumericFormatMode(m_colorBarNumericFormatModeComboBox->getSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>());
            m_paletteColorMapping->setPrecisionDigits(m_colorBarDecimalsSpinBox->value());
            m_paletteColorMapping->setNumericSubdivisionCount(m_colorBarNumericSubdivisionsSpinBox->value());
        }
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

QWidget*
MapSettingsColorBarWidget::createLocationDimensionsSection()
{
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
    
    const int32_t browserWindowIndex = 0;
    m_coordinateWidget = new AnnotationCoordinateWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
                                                        AnnotationCoordinateWidget::COORDINATE_ONE,
                                                        browserWindowIndex);
    
    m_widthHeightWidget = new AnnotationWidthHeightWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
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
    
    locationAndDimGroupBox->setSizePolicy(QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);
    
    return locationAndDimGroupBox;
}

QWidget*
MapSettingsColorBarWidget::createFontColorsSection()
{
    const int32_t browserWindowIndex = 0;
    m_fontWidget = new AnnotationFontWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
                                            browserWindowIndex);
    
    m_colorWidget = new AnnotationColorWidget(AnnotationWidgetParentEnum::COLOR_BAR_EDITOR_WIDGET,
                                              browserWindowIndex);
    QGroupBox* fontGroupBox = new QGroupBox("Font and Colors");
    QVBoxLayout* fontGroupLayout = new QVBoxLayout(fontGroupBox);
    fontGroupLayout->addWidget(m_fontWidget);
    fontGroupLayout->addWidget(m_colorWidget);
    
    fontGroupBox->setSizePolicy(QSizePolicy::Fixed,
                                QSizePolicy::Fixed);
    
    return fontGroupBox;
}

/**
 * @return Create the color bar section.
 */
QWidget*
MapSettingsColorBarWidget::createColorBarSection()
{
    QLabel* valuesModeLabel = new QLabel("Data Mode");
    m_colorBarDataModeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(m_colorBarDataModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(colorBarItemActivated()));
    m_colorBarDataModeComboBox->setup<PaletteColorBarValuesModeEnum, PaletteColorBarValuesModeEnum::Enum>();
    m_colorBarDataModeComboBox->getWidget()->setToolTip("Format of numbers above color bar\n"
                                                        "   DATA: Numbers show mapping of data to the colobar\n"
                                                        "   PERCENTILE: Numbers show data percentiles\n"
                                                        "   SIGN ONLY: Text above colorbar indicates sign of data");
    
    m_colorBarNumericFormatModeLabel = new QLabel("Numeric Format");
    m_colorBarNumericFormatModeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(m_colorBarNumericFormatModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(colorBarItemActivated()));
    m_colorBarNumericFormatModeComboBox->setup<NumericFormatModeEnum, NumericFormatModeEnum::Enum>();
    m_colorBarNumericFormatModeComboBox->getWidget()->setToolTip("Format of numbers above color bar\n"
                                                                 "   AUTO: Format (decimal or scientific notation)\n"
                                                                 "         and precision digits are automatically\n"
                                                                 "         generated by examination of the data\n"
                                                                 "   DECIMAL: Format as decimal number with the\n"
                                                                 "         selected precision\n"
                                                                 "   SCIENTIFIC: Format as scientific notation\n"
                                                                 "         with the selected precision in the\n"
                                                                 "         significand");
    
    m_colorBarDecimalsLabel = new QLabel("Dec/Sci Decimals"); //Precision Digits");
    m_colorBarDecimalsSpinBox = new QSpinBox();
    m_colorBarDecimalsSpinBox->setMinimum(0);
    m_colorBarDecimalsSpinBox->setMaximum(100);
    m_colorBarDecimalsSpinBox->setSingleStep(1);
    QObject::connect(m_colorBarDecimalsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(applySelections()));
    m_colorBarDecimalsSpinBox->setToolTip("Number of digits right of the decimal point\n"
                                          "in the numbers above the color bar when format\n"
                                          "is " + NumericFormatModeEnum::toGuiName(NumericFormatModeEnum::DECIMAL)
                                          + " or " + NumericFormatModeEnum::toGuiName(NumericFormatModeEnum::SCIENTIFIC));
    
    m_colorBarNumericSubdivisionsLabel = new QLabel("Subdivisions");
    m_colorBarNumericSubdivisionsSpinBox = new QSpinBox();
    m_colorBarNumericSubdivisionsSpinBox->setMinimum(0);
    m_colorBarNumericSubdivisionsSpinBox->setMaximum(100);
    m_colorBarNumericSubdivisionsSpinBox->setSingleStep(1);
    QObject::connect(m_colorBarNumericSubdivisionsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(applySelections()));
    m_colorBarNumericSubdivisionsSpinBox->setToolTip("Adds additional numbers to the negative\n"
                                                     "and positive regions in the color bar");
    
    QGroupBox* colorBarGroupBox = new QGroupBox("Data Numerics");
    QGridLayout* gridLayout = new QGridLayout(colorBarGroupBox);
    int row = gridLayout->rowCount();
//    this->setLayoutSpacingAndMargins(gridLayout);
    gridLayout->addWidget(valuesModeLabel, row, 0);
    gridLayout->addWidget(m_colorBarDataModeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(m_colorBarNumericFormatModeLabel, row, 0);
    gridLayout->addWidget(m_colorBarNumericFormatModeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(m_colorBarDecimalsLabel, row, 0);
    gridLayout->addWidget(m_colorBarDecimalsSpinBox, row, 1);
    row++;
    gridLayout->addWidget(m_colorBarNumericSubdivisionsLabel, row, 0);
    gridLayout->addWidget(m_colorBarNumericSubdivisionsSpinBox, row, 1);
    colorBarGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    colorBarGroupBox->setSizePolicy(QSizePolicy::Fixed,
                                    QSizePolicy::Fixed);
    
    return colorBarGroupBox;
}

/**
 * Gets called when a color bar item is selected.
 */
void
MapSettingsColorBarWidget::colorBarItemActivated()
{
    applySelections();
    
    updateColorBarAttributes();
}

/**
 * Update the colorbar attributes section.
 */
void
MapSettingsColorBarWidget::updateColorBarAttributes()
{
    if (m_paletteColorMapping != NULL) {
        bool numericForatComboBoxEnabled   = true;
        bool precisionDigitsSpinBoxEnabled = true;
        bool subdivisionsSpinBoxEnabled    = true;
        
        const PaletteColorBarValuesModeEnum::Enum valuesMode = m_paletteColorMapping->getColorBarValuesMode();
        switch (valuesMode) {
            case PaletteColorBarValuesModeEnum::DATA:
                break;
            case PaletteColorBarValuesModeEnum::PERCENTILE:
                break;
            case PaletteColorBarValuesModeEnum::SIGN_ONLY:
                numericForatComboBoxEnabled   = false;
                precisionDigitsSpinBoxEnabled = false;
                subdivisionsSpinBoxEnabled    = false;
                break;
        }
        
        const NumericFormatModeEnum::Enum numericFormat = m_paletteColorMapping->getNumericFormatMode();
        switch (numericFormat) {
            case NumericFormatModeEnum::AUTO:
                precisionDigitsSpinBoxEnabled = false;
                break;
            case NumericFormatModeEnum::DECIMAL:
                break;
            case NumericFormatModeEnum::SCIENTIFIC:
                break;
        }
        
        m_colorBarDataModeComboBox->setSelectedItem<PaletteColorBarValuesModeEnum, PaletteColorBarValuesModeEnum::Enum>(valuesMode);
        
        m_colorBarNumericFormatModeLabel->setEnabled(numericForatComboBoxEnabled);
        m_colorBarNumericFormatModeComboBox->setSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>(numericFormat);
        m_colorBarNumericFormatModeComboBox->getWidget()->setEnabled(numericForatComboBoxEnabled);
        
        m_colorBarDecimalsLabel->setEnabled(precisionDigitsSpinBoxEnabled);
        m_colorBarDecimalsSpinBox->blockSignals(true);
        m_colorBarDecimalsSpinBox->setValue(m_paletteColorMapping->getPrecisionDigits());
        m_colorBarDecimalsSpinBox->blockSignals(false);
        m_colorBarDecimalsSpinBox->setEnabled(precisionDigitsSpinBoxEnabled);
        
        m_colorBarNumericSubdivisionsLabel->setEnabled(subdivisionsSpinBoxEnabled);
        m_colorBarNumericSubdivisionsSpinBox->blockSignals(true);
        m_colorBarNumericSubdivisionsSpinBox->setValue(m_paletteColorMapping->getNumericSubdivisionCount());
        m_colorBarNumericSubdivisionsSpinBox->blockSignals(false);
        m_colorBarNumericSubdivisionsSpinBox->setEnabled(subdivisionsSpinBoxEnabled);
        
    }
}


