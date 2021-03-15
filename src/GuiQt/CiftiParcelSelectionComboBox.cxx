
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

#include <set>

#define __CIFTI_PARCEL_SELECTION_COMBO_BOX_DECLARE__
#include "CiftiParcelSelectionComboBox.h"
#undef __CIFTI_PARCEL_SELECTION_COMBO_BOX_DECLARE__

#include <QComboBox>

#include "AStringNaturalComparison.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiParcelsMap.h"

using namespace caret;


    
/**
 * \class caret::CiftiParcelSelectionComboBox 
 * \brief ComboBox for selection of a Parcel from a CiftiParcelsMap.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
CiftiParcelSelectionComboBox::CiftiParcelSelectionComboBox(QObject* parent)
: WuQWidget(parent)
{
    m_comboBox = new QComboBox();

#if QT_VERSION >= 0x060000
    QObject::connect(m_comboBox, &QComboBox::textActivated,
                     this, &CiftiParcelSelectionComboBox::parcelNameSelected);
#else
    QObject::connect(m_comboBox, SIGNAL(activated(const QString&)),
                     this, SIGNAL(parcelNameSelected(const QString&)));
#endif
}

/**
 * Destructor.
 */
CiftiParcelSelectionComboBox::~CiftiParcelSelectionComboBox()
{
}

/**
 * @return The QComboBox for adding to a layout, enalbling, etc.
 */
QWidget*
CiftiParcelSelectionComboBox::getWidget()
{
    return m_comboBox;
}

/**
 * Update the combo box with the given parcels map.  If NULL,
 * combo box will be empty.
 *
 * @param parcelsMap
 *    Parcels map inserted into combo box.
 */
void
CiftiParcelSelectionComboBox::updateComboBox(const CiftiParcelsMap* parcelsMap)
{
    QString selectedParcelName = m_comboBox->currentText();
    m_comboBox->clear();
    
    if (parcelsMap != NULL) {
        std::set<AString, AStringNaturalComparison> sortedNames;
        const std::vector<CiftiParcelsMap::Parcel>& allParcels = parcelsMap->getParcels();
        for (std::vector<CiftiParcelsMap::Parcel>::const_iterator parcelIter = allParcels.begin();
             parcelIter != allParcels.end();
             parcelIter++) {
            sortedNames.insert(parcelIter->m_name);
        }
        
        for (std::set<AString, AStringNaturalComparison>::iterator iter = sortedNames.begin();
             iter != sortedNames.end();
             iter++) {
            m_comboBox->addItem(*iter);
        }
        
//        QStringList parcelNamesList = QStringList::fromSet(sortedNames);
//        parcelNamesList.append(parcelIter->m_name);
//        
//        m_comboBox->addItems(parcelNamesList);
        
    }
    
    if ( ! selectedParcelName.isEmpty()) {
        if (m_comboBox->findText(selectedParcelName) < 0) {
            selectedParcelName = "";
        }
    }
    if (selectedParcelName.isEmpty()) {
        if (m_comboBox->count() > 0) {
            selectedParcelName = m_comboBox->itemText(0);
        }
    }
    
    if ( ! selectedParcelName.isEmpty()) {
        setSelectedParcelName(selectedParcelName);
    }
}

/**
 * @return Name of selected parcel.
 */
AString
CiftiParcelSelectionComboBox::getSelectedParcelName()
{
    return m_comboBox->currentText();
}

/**
 * Set the selected parcel to the parcel with the given name.
 */
void
CiftiParcelSelectionComboBox::setSelectedParcelName(const QString& parcelName)
{
    const int32_t parcelNameIndex = m_comboBox->findText(parcelName);
    if (parcelNameIndex >= 0) {
        m_comboBox->setCurrentIndex(parcelNameIndex);
    }
    else {
        CaretLogWarning("Parcel named \""
                        + parcelName
                        + "\" is not valid name in CiftiParcelSelectionComboBox");
    }
}

