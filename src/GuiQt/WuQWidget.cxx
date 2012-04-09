
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

#define __WU_Q_WIDGET_DECLARE__
#include "WuQWidget.h"
#undef __WU_Q_WIDGET_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::WuQWidget 
 * \brief Class for extending Qt GUI Widgets through encapsulation
 *
 * It is often desirable to extend a Qt GUI widget but one does
 * not want to directly subclass a QWidget.  For example, a 
 * QComboBox is a useful control for selection of an enumerated
 * type.  It is desirable to allow the user of the class to only
 * access the combo box by setting and getting the enumerated type.
 * However, if one directly subclasses QComboBox, it allows access
 * to many methods including those that get and set via an index
 * or name which could allow the insertion of invalid values.  So,
 * by encapsualting, one can provide accessor methods using the
 * enumerated types.  Protected inheritance is not a viable solution
 * because it prevents the connection of signals and slots.
 *
 * This class is not derived from QWidget since that would require.
 * the additional of a layout to hold the actual widget.  Instead,
 * this class is derived from QObject so that there are no 'widget'
 * methods available to the user and so that the signal and slot
 * mechanism is available.  Subclasses can define, signals and slots
 * that are appropriate, such as those that use an enumerated type
 * as a parameter.
 *
 * The parent of derived classes must be passed to the contructor.
 * The parent is typically some deriviative of QWidget such as 
 * QDialog.  By using a parent, Qt will destroy an instance of 
 * this class when the parent is destroyed.
 *
 * An instance of this class is never added to a layout.  Instead,
 * deriving classes implement the getWidget() method to provide
 * the enapsulated widget for insertion into a layout.
 *
 * Since the encapsulated QWidget is added to a layout, never
 * delete the encapsulated widget since it will have a Qt parent
 * which will destroy it.
 */

/**
 * Constructor.
 */
WuQWidget::WuQWidget(QObject* parent)
: QObject(parent)
{
    CaretAssert(parent);
}

/**
 * Destructor.
 */
WuQWidget::~WuQWidget()
{
}



