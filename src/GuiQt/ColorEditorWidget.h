#ifndef __COLOR_EDITOR_WIDGET__H_
#define __COLOR_EDITOR_WIDGET__H_

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


#include <QWidget>

class QSlider;
class QSpinBox;

namespace caret {

    class WuQWidgetObjectGroup;
    
    class ColorEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        ColorEditorWidget(const bool alphaControlEnabled = false,
                          QWidget* parent = 0);
        
        virtual ~ColorEditorWidget();

        void setColor(const float rgba[4]);
        
        void getColor(float rgba[4]) const;

        void setColor(const int rgba[4]);
        
        void getColor(int rgba[4]) const;
        
    signals:
        void colorChanged(const float*);
        void colorChanged(const int*);
        
    public slots:
        void redValueChanged(int);
        void blueValueChanged(int);
        void greenValueChanged(int);
        void alphaValueChanged(int);
        
        void emitColorChangedSignal();
        
    private:
        ColorEditorWidget(const ColorEditorWidget&);

        ColorEditorWidget& operator=(const ColorEditorWidget&);
        
    private:
        WuQWidgetObjectGroup* controlsWidgetGroup;
        
        QSpinBox* redSpinBox;
        QSpinBox* greenSpinBox;
        QSpinBox* blueSpinBox;
        QSpinBox* alphaSpinBox;
        
    };
    
#ifdef __COLOR_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __COLOR_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__COLOR_EDITOR_WIDGET__H_
