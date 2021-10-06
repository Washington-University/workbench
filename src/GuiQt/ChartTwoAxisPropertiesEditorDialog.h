#ifndef __CHART_TWO_AXIS_PROPERTIES_EDITOR_DIALOG_H__
#define __CHART_TWO_AXIS_PROPERTIES_EDITOR_DIALOG_H__

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



#include <memory>

#include <QDialog>

#include "ChartAxisLocationEnum.h"

namespace caret {

    class ChartTwoAxisPropertiesEditorWidget;
    class ChartTwoCartesianAxis;
    class ChartTwoOverlaySet;
    
    class ChartTwoAxisPropertiesEditorDialog : public QDialog {
        
    public:
        ChartTwoAxisPropertiesEditorDialog(const ChartAxisLocationEnum::Enum axisLocation,
                                           const QString& parentObjectName,
                                           QWidget* parent);
        
        ChartTwoAxisPropertiesEditorDialog();
        
        virtual ~ChartTwoAxisPropertiesEditorDialog();
        
        ChartTwoAxisPropertiesEditorDialog(const ChartTwoAxisPropertiesEditorDialog&) = delete;

        ChartTwoAxisPropertiesEditorDialog& operator=(const ChartTwoAxisPropertiesEditorDialog&) = delete;
        
        void updateControls(ChartTwoOverlaySet* chartOverlaySet,
                            ChartTwoCartesianAxis* chartAxis);
        
    private:
        ChartTwoAxisPropertiesEditorWidget* m_editorWidget;

    };
    
#ifdef __CHART_TWO_AXIS_PROPERTIES_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_AXIS_PROPERTIES_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__CHART_TWO_AXIS_PROPERTIES_EDITOR_DIALOG_H__
