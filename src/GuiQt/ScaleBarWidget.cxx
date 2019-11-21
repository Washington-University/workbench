
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

#define __SCALE_BAR_WIDGET_DECLARE__
#include "ScaleBarWidget.h"
#undef __SCALE_BAR_WIDGET_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include "AnnotationScaleBar.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "WuQMacroManager.h"

using namespace caret;


    
/**
 * \class caret::ScaleBarWidget 
 * \brief Widget for editing a scale bar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ScaleBarWidget::ScaleBarWidget(const QString& objectNamePrefix)
: QWidget()
{
    m_objectNamePrefix = (objectNamePrefix
                          + ":ScaleBarWidget");
    setObjectName(m_objectNamePrefix);
    
    QWidget* lengthWidget = createLengthWidget();
    QWidget* miscWidget   = createMiscWidget();
    QWidget* ticksWidget  = createTickMarksWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(lengthWidget);
    layout->addWidget(ticksWidget);
    layout->addWidget(miscWidget);
}

/**
 * Destructor.
 */
ScaleBarWidget::~ScaleBarWidget()
{
}

/**
 * @return Instance of the length widget
 */
QWidget*
ScaleBarWidget::createLengthWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_showLengthTextCheckBox = new QCheckBox("Show Length");
    m_showLengthTextCheckBox->setToolTip("Show the length of the scale bar");
    QObject::connect(m_showLengthTextCheckBox, &QCheckBox::clicked,
                     this, &ScaleBarWidget::showLengthCheckBoxClicked);
    m_showLengthTextCheckBox->setObjectName(m_objectNamePrefix
                                              + ":m_showLengthTextCheckBox");
    macroManager->addMacroSupportToObject(m_showLengthTextCheckBox,
                                          "Show Length");

    m_showLengthUnitsCheckbox = new QCheckBox("Show Length Units");
    m_showLengthUnitsCheckbox->setToolTip("Show the length units after the length");
    QObject::connect(m_showLengthUnitsCheckbox, &QCheckBox::clicked,
                     this, &ScaleBarWidget::showLengthUnitsCheckBoxClicked);
    m_showLengthUnitsCheckbox->setObjectName(m_objectNamePrefix
                                            + ":m_showLengthUnitsCheckbox");
    macroManager->addMacroSupportToObject(m_showLengthUnitsCheckbox,
                                          "Show Length Units");

    QLabel* lengthLabel = new QLabel("Length");
    m_lengthSpinBox = new QDoubleSpinBox();
    m_lengthSpinBox->setToolTip("Set the length of the scale bar");
    m_lengthSpinBox->setRange(0.0, 100000.0);
    QObject::connect(m_lengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &ScaleBarWidget::lengthSpinBoxValueChanged);
    m_lengthSpinBox->setObjectName(m_objectNamePrefix
                                            + ":m_lengthSpinBox");
    macroManager->addMacroSupportToObject(m_lengthSpinBox,
                                          "Set Length");


    QLabel* unitsLabel = new QLabel("Length Units");
    m_lengthUnitsComboBox = new EnumComboBoxTemplate(this);
    m_lengthUnitsComboBox->getComboBox()->setToolTip("Select the units for the length of the scale bar");
    m_lengthUnitsComboBox->setup<AnnotationScaleBarUnitsTypeEnum,AnnotationScaleBarUnitsTypeEnum::Enum>();
    QObject::connect(m_lengthUnitsComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ScaleBarWidget::lengthEnumComboBoxItemActivated);
    m_lengthUnitsComboBox->getWidget()->setToolTip("Units displayed at end of scale bar");
    m_lengthUnitsComboBox->getWidget()->setObjectName(m_objectNamePrefix
                                            + ":m_lengthUnitsComboBox");
    macroManager->addMacroSupportToObject(m_lengthUnitsComboBox->getComboBox(),
                                          "Select Length Units");

    QLabel* locationLabel = new QLabel("Location");
    m_lengthTextLocationComboBox = new EnumComboBoxTemplate(this);
    m_lengthTextLocationComboBox->getComboBox()->setToolTip("Set the location of the length text relative to scale bar");
    m_lengthTextLocationComboBox->setup<AnnotationScaleBarTextLocationEnum, AnnotationScaleBarTextLocationEnum::Enum>();
    QObject::connect(m_lengthTextLocationComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ScaleBarWidget::lengthTextLocationComboBoxActivated);
    m_lengthTextLocationComboBox->getComboBox()->setObjectName(m_objectNamePrefix
                                            + ":m_lengthTextLocationComboBox");
    macroManager->addMacroSupportToObject(m_lengthTextLocationComboBox->getComboBox(),
                                          "Set Location of Text");

    QGroupBox* groupBox = new QGroupBox("Length");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    int32_t row(0);
    gridLayout->addWidget(m_showLengthTextCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(m_showLengthUnitsCheckbox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(lengthLabel, row, 0);
    gridLayout->addWidget(m_lengthSpinBox, row, 1);
    row++;
    gridLayout->addWidget(unitsLabel, row, 0);
    gridLayout->addWidget(m_lengthUnitsComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(locationLabel, row, 0);
    gridLayout->addWidget(m_lengthTextLocationComboBox->getWidget(), row, 1);
    row++;

    return groupBox;
}

/**
 * @return Instance of the misc widget
 */
QWidget*
ScaleBarWidget::createMiscWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    QLabel* positionModeLabel = new QLabel("Positioning");
    m_positionModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_positionModeEnumComboBox->setup<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>();
    QObject::connect(m_positionModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ScaleBarWidget::positionModeEnumComboBoxItemActivated);
    m_positionModeEnumComboBox->getWidget()->setToolTip("AUTOMATIC - color bars are stacked\n"
                                                        "   in lower left corner of Tab/Window\n"
                                                        "MANUAL - user must set the X and Y\n"
                                                        "   coordinates in the Tab/Window\n");
    m_positionModeEnumComboBox->getComboBox()->setObjectName(m_objectNamePrefix
                                            + ":m_positionModeEnumComboBox");
    macroManager->addMacroSupportToObject(m_positionModeEnumComboBox->getComboBox(),
                                          "Positionm Mode");

    QGroupBox* groupBox = new QGroupBox("Misc");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    int32_t row(0);
    gridLayout->addWidget(positionModeLabel, row, 0);
    gridLayout->addWidget(m_positionModeEnumComboBox->getWidget(), row, 1);
    row++;
    
    return groupBox;
}

/**
 * @return Instance of the tick marks widget
 */
QWidget*
ScaleBarWidget::createTickMarksWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();

    m_showTickMarksCheckBox = new QCheckBox("Show Tick Marks");
    m_showTickMarksCheckBox->setToolTip("Show tick marks on the scale bar");
    QObject::connect(m_showTickMarksCheckBox, &QCheckBox::clicked,
                     this, &ScaleBarWidget::showTickMarksCheckBoxClicked);
    m_showTickMarksCheckBox->setObjectName(m_objectNamePrefix
                                            + ":m_showTickMarksCheckBox");
    macroManager->addMacroSupportToObject(m_showTickMarksCheckBox,
                                          "Show Tick Marks");

    QLabel* subdivisionsLabel = new QLabel("Subdivisions");
    m_tickMarksSubdivisionsSpinBox = new QSpinBox();
    m_tickMarksSubdivisionsSpinBox->setToolTip("Set the number of subdivisions (space between tick marks");
    m_tickMarksSubdivisionsSpinBox->setRange(0, 10);
    QObject::connect(m_tickMarksSubdivisionsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &ScaleBarWidget::tickMarksSubdivsionsSpinBoxValueChanged);
    m_tickMarksSubdivisionsSpinBox->setObjectName(m_objectNamePrefix
                                            + ":m_tickMarksSubdivisionsSpinBox");
    macroManager->addMacroSupportToObject(m_tickMarksSubdivisionsSpinBox,
                                          "Set Number of Subdivisions");

    
    QGroupBox* groupBox = new QGroupBox("Tick Marks");
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    int32_t row(0);
    gridLayout->addWidget(m_showTickMarksCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(subdivisionsLabel, row, 0);
    gridLayout->addWidget(m_tickMarksSubdivisionsSpinBox, row, 1);
    row++;
    
    return groupBox;
}

/**
 * Update the widgets content
 *  @param browserTabContent
 *       The tab's content
 */
void
ScaleBarWidget::updateContent(BrowserTabContent* browserTabContent)
{
    m_scaleBar = NULL;
    if (browserTabContent != NULL) {
        m_scaleBar = browserTabContent->getScaleBar();
    }
    
    if (m_scaleBar != NULL) {
        const AnnotationColorBarPositionModeEnum::Enum positionMode = m_scaleBar->getPositionMode();
        m_positionModeEnumComboBox->setSelectedItem<AnnotationColorBarPositionModeEnum,AnnotationColorBarPositionModeEnum::Enum>(positionMode);
    
        const AnnotationScaleBarUnitsTypeEnum::Enum unitsType = m_scaleBar->getLengthUnits();
        m_lengthUnitsComboBox->setSelectedItem<AnnotationScaleBarUnitsTypeEnum,AnnotationScaleBarUnitsTypeEnum::Enum>(unitsType);
        
        m_showLengthTextCheckBox->setChecked(m_scaleBar->isShowLengthText());
        
        m_showLengthUnitsCheckbox->setChecked(m_scaleBar->isShowLengthUnitsText());
        
        QSignalBlocker lengthBlocker(m_lengthSpinBox);
        m_lengthSpinBox->setValue(m_scaleBar->getLength());
        
        m_showTickMarksCheckBox->setChecked(m_scaleBar->isShowTickMarks());
        QSignalBlocker ticksSignalBlocker(m_tickMarksSubdivisionsSpinBox);
        m_tickMarksSubdivisionsSpinBox->setValue(m_scaleBar->getTickMarksSubdivsions());
        
        const AnnotationScaleBarTextLocationEnum::Enum textLocation = m_scaleBar->getLengthTextLocation();
        m_lengthTextLocationComboBox->setSelectedItem<AnnotationScaleBarTextLocationEnum, AnnotationScaleBarTextLocationEnum::Enum>(textLocation);
    }

    setEnabled(m_scaleBar != NULL);
}

/**
 * Called when show length check box is clicked
 *
 *  @param status
 *    New checked status
 */
void
ScaleBarWidget::showTickMarksCheckBoxClicked(bool status)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setShowTickMarks(status);
        updateGraphics();
    }
}

/**
 * Called when tick marks subdivisions spin box value is changed
 *
 *  @param value
 *    New value
 */
void
ScaleBarWidget::tickMarksSubdivsionsSpinBoxValueChanged(int value)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setTickMarksSubdivisions(value);
        updateGraphics();
    }
}

/**
 * Called length text location value is changed
 *
 */
void
ScaleBarWidget::lengthTextLocationComboBoxActivated()
{
    if (m_scaleBar != NULL) {
        const AnnotationScaleBarTextLocationEnum::Enum location = m_lengthTextLocationComboBox->getSelectedItem<AnnotationScaleBarTextLocationEnum, AnnotationScaleBarTextLocationEnum::Enum>();
        m_scaleBar->setLengthTextLocation(location);
        updateGraphics();
    }
}

/**
 * Called when show length check box is clicked
 *
 *  @param status
 *    New checked status
 */
void
ScaleBarWidget::showLengthCheckBoxClicked(bool status)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setShowLengthText(status);
        updateGraphics();
    }
}

/**
 * Called when show length units check box is clicked
 *
 *  @param status
 *    New checked status
 */
void
ScaleBarWidget::showLengthUnitsCheckBoxClicked(bool status)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setShowLengthUnitsText(status);
        updateGraphics();
    }
}

/**
 * Called when  length double spin box value is changed
 *
 *  @param value
 *    New value
 */
void
ScaleBarWidget::lengthSpinBoxValueChanged(double value)
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setLength(value);
        updateGraphics();
    }
}

/**
 * Caleld when position mode combo box selection is made
 */
void
ScaleBarWidget::positionModeEnumComboBoxItemActivated()
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setPositionMode(m_positionModeEnumComboBox->getSelectedItem<AnnotationColorBarPositionModeEnum, AnnotationColorBarPositionModeEnum::Enum>());
        updateGraphics();
    }
}

/**
 * Caleld when length units combo box selection is made
 */
void
ScaleBarWidget::lengthEnumComboBoxItemActivated()
{
    if (m_scaleBar != NULL) {
        m_scaleBar->setLengthUnits(m_lengthUnitsComboBox->getSelectedItem<AnnotationScaleBarUnitsTypeEnum, AnnotationScaleBarUnitsTypeEnum::Enum>());
        updateGraphics();
    }
}

/**
 * Update graphics
 */
void
ScaleBarWidget::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
