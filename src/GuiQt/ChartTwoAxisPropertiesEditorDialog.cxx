
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

#define __CHART_TWO_AXIS_PROPERTIES_EDITOR_DIALOG_DECLARE__
#include "ChartTwoAxisPropertiesEditorDialog.h"
#undef __CHART_TWO_AXIS_PROPERTIES_EDITOR_DIALOG_DECLARE__

#include <QVBoxLayout>

#include "CaretAssert.h"
#include "ChartTwoAxisPropertiesEditorWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoAxisPropertiesEditorDialog 
 * \brief Dialog for editing chart two axeis properties
 * \ingroup GuiQt
 *
 *  This is not a traditional dialog.  It has no close buttons nor titlebar
 *  and uses the Qt::Popup window attribute.
 */

/**
 * Constructor.
 * @param axisLocation
 *    Location of the axis
 * @param parentObjectName
 *    Name of parent for macros
 * @param parent
 *    The parent widget
 */
ChartTwoAxisPropertiesEditorDialog::ChartTwoAxisPropertiesEditorDialog(const ChartAxisLocationEnum::Enum axisLocation,
                                                                       const QString& parentObjectName,
                                                                       QWidget* parent)
: QDialog(parent,
          Qt::Popup) /* use 'Popup' attribute so not borders */
{
    m_editorWidget = new ChartTwoAxisPropertiesEditorWidget(axisLocation,
                                                            parentObjectName,
                                                            parent);
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout , 0, 0);
    layout->addWidget(m_editorWidget);
}

/**
 * Destructor.
 */
ChartTwoAxisPropertiesEditorDialog::~ChartTwoAxisPropertiesEditorDialog()
{
}

/**
 * @param chartOverlaySet
 *   Update with this chart overlay set
 * @param chartAxis
 *   Axis for updating dialog
 */
void
ChartTwoAxisPropertiesEditorDialog::updateControls(ChartTwoOverlaySet* chartOverlaySet,
                                                   ChartTwoCartesianAxis* chartAxis)
{
    m_editorWidget->updateControls(chartOverlaySet,
                                   chartAxis);
}
