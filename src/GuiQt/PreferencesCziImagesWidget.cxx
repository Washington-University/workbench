
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __PREFERENCES_CZI_IMAGE_DECLARE__
#include "PreferencesCziImagesWidget.h"
#undef __PREFERENCES_CZI_IMAGE_DECLARE__

    
/**
 * \class caret::PreferencesCziImagesWidget
 * \brief Widget for recent file properties in preferences
 * \ingroup GuiQt
 */
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Constructor.
 */
PreferencesCziImagesWidget::PreferencesCziImagesWidget(QWidget* parent)
: QWidget(parent)
{
    const QString dimTT("Preferred dimension for loading higher-resolution CZI Images.  "
                        "Larger size will take longer to load but will reduce the "
                        "frequency of loading higher-resolution data.");
    const QString dimensionToolTip(WuQtUtilities::createWordWrappedToolTipText(dimTT));
    
    QLabel* dimensionLabel = new QLabel("Image Size");
    dimensionLabel->setToolTip(dimensionToolTip);
    m_dimensionComboBox = new QComboBox();
    m_dimensionComboBox->setToolTip(dimensionToolTip);
    
    std::vector<std::pair<int32_t, QString>> integerAndTextValues;
    CaretPreferences::getSupportedCziDimensions(integerAndTextValues);
    for (auto it : integerAndTextValues) {
        m_dimensionComboBox->addItem(it.second,
                                     QVariant(it.first));
    }
    
    QObject::connect(m_dimensionComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &PreferencesCziImagesWidget::dimensionChanged);
    
    /*
     * Layouts
     */
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(2, 100);
    layout->setRowStretch(100, 100);
    int32_t row(0);
    layout->addWidget(dimensionLabel,
                      row, 0);
    layout->addWidget(m_dimensionComboBox,
                      row, 1, Qt::AlignLeft);
    row++;
}

/**
 * Destructor.
 */
PreferencesCziImagesWidget::~PreferencesCziImagesWidget()
{
}

/*
 * Update the content in this widget
 * @param caretPreferences
 *    The caret preferences
 */
void
PreferencesCziImagesWidget::updateContent(CaretPreferences* caretPreferences)
{
    m_preferences = caretPreferences;
    CaretAssert(m_preferences);
    const int32_t dimValue(m_preferences->getCziDimension());
    
    bool foundFlag(false);
    const int32_t numItems = m_dimensionComboBox->count();
    for (int32_t i = 0; i < numItems; i++) {
        if (dimValue == m_dimensionComboBox->itemData(i)) {
            m_dimensionComboBox->setCurrentIndex(i);
            foundFlag = true;
            break;
        }
    }
    
    if ( ! foundFlag) {
        CaretLogSevere("Unable to find CZI Dimension when updating Preferences Dialog.");
    }
}

/**
 * Called when dimension combo box is changed
 * @param index
 *    Index of item selected
 */
void
PreferencesCziImagesWidget::dimensionChanged(int index)
{
    const int32_t dimension = m_dimensionComboBox->itemData(index).toInt();
    CaretAssert(m_preferences);
    m_preferences->setCziDimension(dimension);
}



