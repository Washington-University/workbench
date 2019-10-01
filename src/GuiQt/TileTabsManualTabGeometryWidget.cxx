
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

#define __TILE_TABS_MANUAL_TAB_GEOMETRY_WIDGET_DECLARE__
#include "TileTabsManualTabGeometryWidget.h"
#undef __TILE_TABS_MANUAL_TAB_GEOMETRY_WIDGET_DECLARE__

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

#include "AnnotationBrowserTab.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "TileTabsConfigurationDialog.h"
#include "TileTabsBrowserTabGeometry.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::TileTabsManualTabGeometryWidget 
 * \brief Contains widgets for adjusting a manual layout tab's geometry
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param tileTabsConfigurationDialog
 *    The tile tabs configuration dialog.
 * @param index
 *    Index of this item in dialog (NOT the tab index)
 * @param gridLayout
 *    Gridlayout for widgets
 * @param parent
 *    Parent QObject
 */
TileTabsManualTabGeometryWidget::TileTabsManualTabGeometryWidget(TileTabsConfigurationDialog* tileTabsConfigurationDialog,
                                                                 const int32_t index,
                                                                 QGridLayout* gridLayout,
                                                                 QObject* parent)
: QObject(parent),
m_tileTabsConfigurationDialog(tileTabsConfigurationDialog),
m_index(index)
{
    CaretAssert(tileTabsConfigurationDialog);
    CaretAssert(gridLayout);
    
    m_showTabCheckBox = new QCheckBox("");
    QObject::connect(m_showTabCheckBox, &QCheckBox::clicked,
                     this, &TileTabsManualTabGeometryWidget::showCheckBoxClicked);
    
    m_tabNumberLabel = new QLabel("    ");
    m_tabNumberLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    m_xMinSpinBox = createPercentSpinBox("Left edge of tab", true);
    QObject::connect(m_xMinSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &TileTabsManualTabGeometryWidget::xMinSpinBoxValueChanged);

    m_xMaxSpinBox = createPercentSpinBox("Right edge of tab", false);
    QObject::connect(m_xMaxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &TileTabsManualTabGeometryWidget::xMaxSpinBoxValueChanged);

    m_yMinSpinBox = createPercentSpinBox("Bottom edge of tab", true);
    QObject::connect(m_yMinSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &TileTabsManualTabGeometryWidget::yMinSpinBoxValueChanged);
    
    m_yMaxSpinBox = createPercentSpinBox("Top edge of tab", false);
    QObject::connect(m_yMaxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &TileTabsManualTabGeometryWidget::yMaxSpinBoxValueChanged);

    const QString stackOrderToolTipText("Larger numbers are in front of other tabs.  This value only needs adjustment "
                                        "if this tab overlaps with another tab");
    m_stackingOrderSpinBox = new QSpinBox();
    m_stackingOrderSpinBox->setMinimum(-100);
    m_stackingOrderSpinBox->setMaximum(100);
    m_stackingOrderSpinBox->setSingleStep(1);
    m_stackingOrderSpinBox->setToolTip(getStackOrderToolTipText());
    QObject::connect(m_stackingOrderSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &TileTabsManualTabGeometryWidget::stackingOrderValueChanged);

    m_TileTabsLayoutBackgroundTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_TileTabsLayoutBackgroundTypeEnumComboBox->setup<TileTabsLayoutBackgroundTypeEnum,TileTabsLayoutBackgroundTypeEnum::Enum>();
    m_TileTabsLayoutBackgroundTypeEnumComboBox->getComboBox()->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    QObject::connect(m_TileTabsLayoutBackgroundTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(tileTabsLayoutBackgroundTypeEnumComboBoxItemActivated()));

    m_gridLayoutGroup = new WuQGridLayoutGroup(gridLayout);
    const int32_t rowIndex(gridLayout->rowCount());
    int32_t columnIndex(0);
    m_gridLayoutGroup->addWidget(m_showTabCheckBox, rowIndex, columnIndex++, Qt::AlignHCenter);
    m_gridLayoutGroup->addWidget(m_tabNumberLabel, rowIndex, columnIndex++, Qt::AlignLeft);
    m_gridLayoutGroup->addWidget(m_xMinSpinBox, rowIndex, columnIndex++, Qt::AlignHCenter);
    m_gridLayoutGroup->addWidget(m_xMaxSpinBox, rowIndex, columnIndex++, Qt::AlignHCenter);
    m_gridLayoutGroup->addWidget(m_yMinSpinBox, rowIndex, columnIndex++, Qt::AlignHCenter);
    m_gridLayoutGroup->addWidget(m_yMaxSpinBox, rowIndex, columnIndex++, Qt::AlignHCenter);
    m_gridLayoutGroup->addWidget(m_TileTabsLayoutBackgroundTypeEnumComboBox->getWidget(), rowIndex, columnIndex++, Qt::AlignHCenter);
    m_gridLayoutGroup->addWidget(m_stackingOrderSpinBox, rowIndex, columnIndex++);
}

/**
 * Destructor.
 */
TileTabsManualTabGeometryWidget::~TileTabsManualTabGeometryWidget()
{
}

/**
 * @return Tooltip text for order spin box
 */
QString
TileTabsManualTabGeometryWidget::getStackOrderToolTipText()
{
    return WuQtUtilities::createWordWrappedToolTipText("Larger numbers are in front of other tabs.  This value only needs adjustment "
                                                       "if this tab overlaps with another tab");
}

/**
 * Update the content of this item
 *
 * @param browserTabContent
 *    Tab content for the widgets
 */
void
TileTabsManualTabGeometryWidget::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;
    const bool showFlag(m_browserTabContent != NULL);
    
    if (showFlag) {
        m_tabNumberLabel->setText(browserTabContent->getTabName());
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        annotationBrowserTab->getBounds2D(minX, maxX, minY, maxY);
        updatePercentSpinBox(m_xMinSpinBox, minX);
        updatePercentSpinBox(m_xMaxSpinBox, maxX);
        updatePercentSpinBox(m_yMinSpinBox, minY);
        updatePercentSpinBox(m_yMaxSpinBox, maxY);

        m_showTabCheckBox->setChecked(annotationBrowserTab->isBrowserTabDisplayed());
        const TileTabsLayoutBackgroundTypeEnum::Enum background = annotationBrowserTab->getBackgroundType();
        m_TileTabsLayoutBackgroundTypeEnumComboBox->setSelectedItem<TileTabsLayoutBackgroundTypeEnum,TileTabsLayoutBackgroundTypeEnum::Enum>(background);

        QSignalBlocker blocker(m_stackingOrderSpinBox); // setValue() causes signal
        m_stackingOrderSpinBox->setValue(annotationBrowserTab->getStackingOrder());
    }
    
    m_gridLayoutGroup->setVisible(showFlag);
    
}

/**
 * Update a percentage spin box value
 *
 * @param spinBox
 *     The spin box
 * @param value
 *     New value for spin box
 */
void
TileTabsManualTabGeometryWidget::updatePercentSpinBox(QDoubleSpinBox* spinBox,
                                                      const double value)
{
    CaretAssert(spinBox);
    
    /*
     * Must block signal because setValue() trigger's the signal
     */
    QSignalBlocker blocker(spinBox);
    spinBox->setValue(value);
}

/**
 * Create a double spin box for percentage value
 *
 * @param toolTip
 *     Tooltip for the spin box
 * @param minValueFlag
 *     True if spin box is for a minimum value, else a maximum value
 * @return
 *     The spin box
 */
QDoubleSpinBox*
TileTabsManualTabGeometryWidget::createPercentSpinBox(const QString& toolTip,
                                                      const bool minValueFlag)
{
    QDoubleSpinBox* dsb = new QDoubleSpinBox();
    if (minValueFlag) {
        dsb->setMinimum(-99.0);
        dsb->setMaximum(99.0);
    }
    else {
        dsb->setMinimum(1.0);
        dsb->setMaximum(199.0);
    }
    dsb->setDecimals(1);
    dsb->setSingleStep(0.1);
    dsb->setToolTip(toolTip);
    
    /*
     * If keyboard tracking is on and user types values 4, 0 then two signals are
     * emitted 4 and 40.  We do not want this since we want to keep minimum values
     * less than maximum values
     */
    dsb->setKeyboardTracking(false);
    
    return dsb;
}

/**
 * Called when show checkbox is clicked
 *
 * @param status
 *     New checked status
 */
void
TileTabsManualTabGeometryWidget::showCheckBoxClicked(bool status)
{
    CaretAssert(m_browserTabContent);
    if (m_browserTabContent != NULL) {
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);
        annotationBrowserTab->setBrowserTabDisplayed(status);
        emit itemChanged();
    }
}

/**
 * Called when x-min value is changed
 *
 * @param value
 *     New value
 */
void
TileTabsManualTabGeometryWidget::xMinSpinBoxValueChanged(double value)
{
    CaretAssert(m_browserTabContent);
    if (m_browserTabContent != NULL) {
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        annotationBrowserTab->getBounds2D(minX, maxX, minY, maxY);
        minX = value;
        annotationBrowserTab->setBounds2D(minX, maxX, minY, maxY);
        updateContent(m_browserTabContent); // 'geometry' ensures min < max
        emit itemChanged();
    }
}

/**
 * Called when x-max value is changed
 *
 * @param value
 *     New value
 */
void
TileTabsManualTabGeometryWidget::xMaxSpinBoxValueChanged(double value)
{
    CaretAssert(m_browserTabContent);
    if (m_browserTabContent != NULL) {
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        annotationBrowserTab->getBounds2D(minX, maxX, minY, maxY);
        maxX = value;
        annotationBrowserTab->setBounds2D(minX, maxX, minY, maxY);
        updateContent(m_browserTabContent); // 'geometry' ensures min < max
        emit itemChanged();
    }
}

/**
 * Called when y-min value is changed
 *
 * @param value
 *     New value
 */
void
TileTabsManualTabGeometryWidget::yMinSpinBoxValueChanged(double value)
{
    CaretAssert(m_browserTabContent);
    if (m_browserTabContent != NULL) {
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        annotationBrowserTab->getBounds2D(minX, maxX, minY, maxY);
        minY = value;
        annotationBrowserTab->setBounds2D(minX, maxX, minY, maxY);
        updateContent(m_browserTabContent); // 'geometry' ensures min < max
        emit itemChanged();
    }
}

/**
 * Called when x-max value is changed
 *
 * @param value
 *     New value
 */
void
TileTabsManualTabGeometryWidget::yMaxSpinBoxValueChanged(double value)
{
    CaretAssert(m_browserTabContent);
    if (m_browserTabContent != NULL) {
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);
        float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
        annotationBrowserTab->getBounds2D(minX, maxX, minY, maxY);
        maxY = value;
        annotationBrowserTab->setBounds2D(minX, maxX, minY, maxY);
        updateContent(m_browserTabContent); // 'geometry' ensures min < max
        emit itemChanged();
    }
}

/**
 * Called when stacking order value is changed
 *
 * @param value
 *     New value
 */
void
TileTabsManualTabGeometryWidget::stackingOrderValueChanged(int value)
{
    CaretAssert(m_browserTabContent);
    if (m_browserTabContent != NULL) {
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);
        annotationBrowserTab->setStackingOrder(value);
        emit itemChanged();
    }
}

/**
 * Called when background value is changed
 *
 * @param value
 *     New value
 */
void
TileTabsManualTabGeometryWidget::tileTabsLayoutBackgroundTypeEnumComboBoxItemActivated()
{
    CaretAssert(m_browserTabContent);
    if (m_browserTabContent != NULL) {
        AnnotationBrowserTab* annotationBrowserTab = m_browserTabContent->getManualLayoutBrowserTabAnnotation();
        CaretAssert(annotationBrowserTab);

        const TileTabsLayoutBackgroundTypeEnum::Enum backgroundType = m_TileTabsLayoutBackgroundTypeEnumComboBox->getSelectedItem<TileTabsLayoutBackgroundTypeEnum,TileTabsLayoutBackgroundTypeEnum::Enum>();
        annotationBrowserTab->setBackgroundType(backgroundType);
        emit itemChanged();
    }
}
