#ifndef __WU_Q_TAB_WIDGET__H_
#define __WU_Q_TAB_WIDGET__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "WuQWidget.h"
#include "SceneableInterface.h"

class QTabBar;
class QStackedWidget;

namespace caret {

    class WuQTabWidget : public WuQWidget, public SceneableInterface {
        
        Q_OBJECT

    public:
        enum TabAlignment {
            TAB_ALIGN_LEFT,
            TAB_ALIGN_CENTER,
            TAB_ALIGN_RIGHT
        };
        
        WuQTabWidget(const TabAlignment alignment,
                     QObject* parent);
        
        virtual ~WuQTabWidget();
        
        QWidget* getWidget();
        
        void addTab(QWidget* page,
                    const QString& label);
        
        int currentIndex() const;
        
        QWidget* currentWidget() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    signals:
        void currentChanged(int index);
        
    public slots:
        void setCurrentIndex(int index);
        
        void setCurrentWidget(QWidget* widget);
        
    private slots:
        void tabBarCurrentIndexChanged(int index);
        
    private:
        WuQTabWidget(const WuQTabWidget&);

        WuQTabWidget& operator=(const WuQTabWidget&);
        
        QTabBar* m_tabBar;
        
        QStackedWidget* m_stackedWidget;
        
        QWidget* m_widget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_TAB_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TAB_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_TAB_WIDGET__H_
