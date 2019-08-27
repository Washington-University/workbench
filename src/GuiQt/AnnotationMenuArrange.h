#ifndef __ANNOTATION_MENU_ARRANGE_H__
#define __ANNOTATION_MENU_ARRANGE_H__

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


#include <stdint.h>

#include <QMenu>

#include "AnnotationAlignmentEnum.h"
#include "AnnotationDistributeEnum.h"
#include "AnnotationGroupingModeEnum.h"
#include "UserInputModeEnum.h"

namespace caret {

    class AnnotationMenuArrange : public QMenu {
        
        Q_OBJECT

    public:
        AnnotationMenuArrange(const UserInputModeEnum::Enum userInputMode,
                              const int32_t browserWindowIndex,
                              QWidget* parent = 0);
        
        virtual ~AnnotationMenuArrange();
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void menuActionTriggered(QAction* action);
        
        void menuAboutToShow();
        
    private:
        void addAlignmentSelections();
        
        void addDistributeSelections();
        
        void addGroupingSelections();
        
        void addTileTabsSelections();
        
        void applyAlignment(const AnnotationAlignmentEnum::Enum alignment);
        
        void applyDistribute(const AnnotationDistributeEnum::Enum distribute);
        
        void applyGrouping(const AnnotationGroupingModeEnum::Enum grouping);
        
        AnnotationMenuArrange(const AnnotationMenuArrange&);

        AnnotationMenuArrange& operator=(const AnnotationMenuArrange&);
        
        QPixmap createAlignmentPixmap(const QWidget* widget,
                                      const AnnotationAlignmentEnum::Enum alignment);
        
        QPixmap createDistributePixmap(const QWidget* widget,
                                       const AnnotationDistributeEnum::Enum distribute);
        
        QPixmap createGroupingPixmap(const QWidget* widget,
                                     const AnnotationGroupingModeEnum::Enum grouping);
        
        void drawLine(QSharedPointer<QPainter>& painter,
                      const QLineF& line,
                      const qreal x,
                      const qreal y);
        
        void drawRect(QSharedPointer<QPainter>& painter,
                      const QColor& color,
                      const QRectF& rectangle,
                      const qreal x,
                      const qreal y);
        
        bool processTileTabsMenu(QAction* actionSelected);
        
        void processExpandTabMenuItem();
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_browserWindowIndex;
        
        QAction* m_groupAction = NULL;
        
        QAction* m_regroupAction = NULL;
        
        QAction* m_ungroupAction = NULL;
        
        QAction* m_tileTabsExpandToFillAction = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_MENU_ARRANGE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_MENU_ARRANGE_DECLARE__

} // namespace
#endif  //__ANNOTATION_MENU_ARRANGE_H__
