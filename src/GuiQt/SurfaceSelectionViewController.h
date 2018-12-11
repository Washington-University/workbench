#ifndef __SURFACE_SELECTION_CONTROL__H_
#define __SURFACE_SELECTION_CONTROL__H_

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

#include "WuQWidget.h"

class QComboBox;

namespace caret {
    class BrainStructure;
    class Surface;
    class SurfaceSelectionModel;

    class SurfaceSelectionViewController : public WuQWidget {
        
        Q_OBJECT

    public:
        SurfaceSelectionViewController(QObject* parent,
                                SurfaceSelectionModel* surfaceSelectionModel,
                                       const QString& objectName);
        
        SurfaceSelectionViewController(QObject* parent,
                                       BrainStructure* brainStructure,
                                       const QString& objectName);
        
        SurfaceSelectionViewController(QObject* parent,
                                       const QString& objectName);
        
        virtual ~SurfaceSelectionViewController();
        
        QWidget* getWidget();
        
        Surface* getSurface();
        
        void updateControl();
        
        void updateControl(SurfaceSelectionModel* surfaceSelectionModel);
        
    signals:
        void surfaceSelected(Surface*);
        
    public slots:
        void setSurface(Surface*);
        
    private slots:
        void comboBoxCurrentIndexChanged(int);
        
    private:
        SurfaceSelectionViewController(const SurfaceSelectionViewController&);

        SurfaceSelectionViewController& operator=(const SurfaceSelectionViewController&);
        
    private:
        enum Mode {
            /** Use all surfaces from a brain structure */
            MODE_BRAIN_STRUCTURE,
            /** Selection mode NOT passed to constructor, user must use updateControl(SurfaceSelectionModel*) */
            MODE_SELECTION_MODEL_DYNAMIC,
            /** Selection model passed to constructor, and use it */
            MODE_SELECTION_MODEL_STATIC
        };
        
        void initializeControl(const Mode mode,
                               SurfaceSelectionModel* surfaceSelectionModel,
                               const QString& objectName);
        
        Mode mode;
        
        SurfaceSelectionModel* surfaceSelectionModel;
        
        QComboBox* surfaceComboBox;
    };
    
#ifdef __SURFACE_SELECTION_CONTROL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_SELECTION_CONTROL_DECLARE__

} // namespace
#endif  //__SURFACE_SELECTION_CONTROL__H_
